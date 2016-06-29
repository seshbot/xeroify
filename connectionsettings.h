#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <QObject>
#include <QSettings>

class ConnectionSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString section READ section WRITE setSection NOTIFY sectionChanged)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    explicit ConnectionSettings(QObject *parent = 0);
    ConnectionSettings(const QString& section, QObject *parent = 0);

    QString section() const;
    void setSection( const QString& section );

    QString url() const;
    void setUrl( const QString& url );

    QString apiKey() const;
    void setApiKey( const QString& apiKey );

    QString password() const;
    void setPassword( const QString& password );

signals:
    void sectionChanged();
    void urlChanged();
    void apiKeyChanged();
    void passwordChanged();

    void changed();

public slots:

private:
    QString section_;
    QSettings settings_;
};

#endif // CONNECTIONSETTINGS_H
