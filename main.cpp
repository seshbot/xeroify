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
#include <MakeLeaps.h>
#include <ConnectionSettings.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<SimpleHttpConnectionSettings>("BusyBot", 1, 0, "SimpleHttpConnectionSettings");
    qmlRegisterType<OAuthZeroLeggedConnectionSettings>("BusyBot", 1, 0, "OAuthZeroLeggedConnectionSettings");
    qmlRegisterType<OAuth2Settings>("BusyBot", 1, 0, "OAuth2Settings");
    qmlRegisterType<OAuth2WithClientCredentialsGrant>("BusyBot", 1, 0, "OAuth2WithClientCredentialsGrant");
    qmlRegisterType<Shopify>("BusyBot", 1, 0, "Shopify");
    qmlRegisterType<Order>("BusyBot", 1, 0, "Order");
    qmlRegisterType<OrderBook>("BusyBot", 1, 0, "OrderBook");
    qmlRegisterType<Xero>("BusyBot", 1, 0, "Xero");
    qmlRegisterType<MakeLeaps>("BusyBot", 1, 0, "MakeLeaps");
    qmlRegisterType<MakeLeapsEndpoint>("BusyBot", 1, 0, "MakeLeapsEndpoint");
    qmlRegisterType<MakeLeapsPartner>("BusyBot", 1, 0, "MakeLeapsPartner");
    qmlRegisterType<ApiProperty>("BusyBot", 1, 0, "ApiProperty");
    qmlRegisterType<ApiObject>("BusyBot", 1, 0, "ApiObject");
    qmlRegisterUncreatableType<JsonValue>("BusyBot", 1, 0, "JsonValue", "Cannot create JsonValue");

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
