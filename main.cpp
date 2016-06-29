#include <QGuiApplication>
#include <QQmlApplicationEngine>

#ifndef NDEBUG
#include <QDir>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QQuickWindow>
#endif

#include <QDebug>

#include <Shopify.h>
#include <Xero.h>
#include <ConnectionSettings.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<Shopify>("BusyBot", 1, 0, "Shopify");
    qmlRegisterType<Xero>("BusyBot", 1, 0, "Xero");
    qmlRegisterType<ConnectionSettings>("BusyBot", 1, 0, "ConnectionSettings");

    QQmlApplicationEngine engine;

#ifndef NDEBUG
    qDebug() << "Current directory: " << QDir::current().absolutePath();

#ifdef __APPLE__
    auto rootFilename = QLatin1String("../../../../Xeroify/main.qml");
#else
    auto rootFilename = QLatin1String("../Xeroify/main.qml");
#endif

    QFileSystemWatcher fileUpdates;
    QDir rootDirectory = QFileInfo( rootFilename ).dir();
    for ( auto fileName : rootDirectory.entryList( QStringList( "*.qml" ) ) ) {
        auto filePath = rootDirectory.filePath( fileName );
        qDebug() << "watching file: " << filePath;
        fileUpdates.addPath( filePath );
    }

    QTimer reload;
    reload.setInterval( 100 );
    reload.setSingleShot( true );

    QObject::connect( &reload, &QTimer::timeout, [&] {
        // ensure the engine doesnt load the previously cached assets
        engine.clearComponentCache();

        // close all windows
        auto* window = qobject_cast< QWindow* >( engine.rootObjects().back() );
        auto geometry = window->geometry();

        window->close();
        window->destroy();

        // re-open the root window
        engine.load( rootFilename );

        window = qobject_cast< QWindow* >( engine.rootObjects().back() );
        window->setGeometry( geometry );
    });

    QObject::connect( &fileUpdates, &QFileSystemWatcher::fileChanged, [&]( const QString& newValue ) {
        qDebug() << "file updated: " << newValue;

        reload.start();

        // watch the new file
        QTimer::singleShot( 50, &engine, [&, newValue] {
            if ( !fileUpdates.addPath( newValue ) ) {
                qWarning() << "cannot watch " << newValue << " for some reason!";
            }
        } );
    });

    engine.load( rootFilename );

#else
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
#endif

    return app.exec();
}
