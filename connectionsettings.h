#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include <QObject>
#include <QSettings>

class SimpleHttpConnectionSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString section READ section WRITE setSection NOTIFY sectionChanged)
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    explicit SimpleHttpConnectionSettings(QObject *parent = 0);
    SimpleHttpConnectionSettings(const QString& section, QObject *parent = 0);

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

class OAuthZeroLeggedConnectionSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString consumerKey READ consumerKey WRITE setConsumerKey NOTIFY consumerKeyChanged)
    Q_PROPERTY(QString rsaPrivateKey READ rsaPrivateKey WRITE setRsaPrivateKey NOTIFY rsaPrivateKeyChanged)

public:
    explicit OAuthZeroLeggedConnectionSettings(QObject *parent = 0);
    OAuthZeroLeggedConnectionSettings(const QString& section, QObject *parent = 0);

    QString consumerKey() const;
    void setConsumerKey(const QString& consumerKey);

    QString rsaPrivateKey() const;
    void setRsaPrivateKey(const QString& rsaPrivateKey);

signals:
    void consumerKeyChanged();
    void rsaPrivateKeyChanged();

private:
    QString section_;
    QSettings settings_;
};

class OAuth2Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)

public:
    explicit OAuth2Settings(QObject *parent = 0);
    OAuth2Settings(const QString& section, QObject *parent = 0);

    QString clientId() const;
    void setClientId(const QString& clientId);

    QString clientSecret() const;
    void setClientSecret(const QString& clientSecret);

    QString accessToken() const;
    void setAccessToken(const QString& accessToken);

signals:
    void clientIdChanged();
    void clientSecretChanged();
    void accessTokenChanged();

private:
    QString section_;
    QSettings settings_;
};

#endif // CONNECTIONSETTINGS_H
