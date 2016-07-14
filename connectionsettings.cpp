#include "ConnectionSettings.h"

#include <QDebug>


//
// SimpleHttpConnectionSettings
//

SimpleHttpConnectionSettings::SimpleHttpConnectionSettings(QObject *parent)
    : SimpleHttpConnectionSettings("default", parent)
{
}

SimpleHttpConnectionSettings::SimpleHttpConnectionSettings(const QString& section, QObject *parent)
    : QObject(parent)
    , settings_("BusyBot", "Xeroify")
{
    qDebug() << "Creating connection settings " << section;

    settings_.beginGroup(section);

    QObject::connect( this, &SimpleHttpConnectionSettings::urlChanged, &SimpleHttpConnectionSettings::changed );
    QObject::connect( this, &SimpleHttpConnectionSettings::apiKeyChanged, &SimpleHttpConnectionSettings::changed );
    QObject::connect( this, &SimpleHttpConnectionSettings::passwordChanged, &SimpleHttpConnectionSettings::changed );
}

QString SimpleHttpConnectionSettings::section() const
{
    return section_;
}

void SimpleHttpConnectionSettings::setSection( const QString& section )
{
    if ( section_ == section ) return;
    section_ = section;
    settings_.endGroup();
    settings_.beginGroup(section);
    emit urlChanged();
    emit apiKeyChanged();
    emit passwordChanged();
}

QString SimpleHttpConnectionSettings::url() const
{
    return settings_.value("url").toString();
}

void SimpleHttpConnectionSettings::setUrl( const QString& url )
{
    qDebug() << "setting url: " << url;
    if ( this->url() == url ) return;
    settings_.setValue("url", url);
    emit urlChanged();
}

QString SimpleHttpConnectionSettings::apiKey() const
{
    return settings_.value("apiKey").toString();
}

void SimpleHttpConnectionSettings::setApiKey( const QString& apiKey )
{
    if ( this->apiKey() == apiKey ) return;
    settings_.setValue("apiKey", apiKey);
    emit apiKeyChanged();
}

QString SimpleHttpConnectionSettings::password() const
{
    return settings_.value("password").toString();
}

void SimpleHttpConnectionSettings::setPassword( const QString& password )
{
    if ( this->password() == password ) return;
    settings_.setValue("password", password);
    emit passwordChanged();
}


//
// OAuthZeroLeggedConnectionSettings
//


OAuthZeroLeggedConnectionSettings::OAuthZeroLeggedConnectionSettings(QObject *parent)
    : OAuthZeroLeggedConnectionSettings("default", parent)
{
}

OAuthZeroLeggedConnectionSettings::OAuthZeroLeggedConnectionSettings(const QString& section, QObject *parent)
    : QObject(parent)
    , settings_("BusyBot", "Xeroify")
{
    settings_.beginGroup(section);
}

QString OAuthZeroLeggedConnectionSettings::consumerKey() const
{
    return settings_.value("consumerKey").toString();
}

void OAuthZeroLeggedConnectionSettings::setConsumerKey(const QString& consumerKey)
{
    if ( this->consumerKey() == consumerKey ) return;
    settings_.setValue("consumerKey", consumerKey);
    emit consumerKeyChanged();
}

QString OAuthZeroLeggedConnectionSettings::rsaPrivateKey() const
{
    return settings_.value("rsaPrivateKey").toString();
}

void OAuthZeroLeggedConnectionSettings::setRsaPrivateKey(const QString& rsaPrivateKey)
{
    if ( this->rsaPrivateKey() == rsaPrivateKey ) return;
    settings_.setValue("rsaPrivateKey", rsaPrivateKey);
    emit rsaPrivateKeyChanged();
}

//
// OAuth2Settings
//

OAuth2Settings::OAuth2Settings(QObject *parent)
    : OAuth2Settings("default", parent)
{

}

OAuth2Settings::OAuth2Settings(const QString& section, QObject *parent)
    : QObject(parent)
    , settings_("BusyBot", "MakeLeaps")
{
    settings_.beginGroup(section);
}

QString OAuth2Settings::clientId() const
{
    return settings_.value("clientId").toString();
}

void OAuth2Settings::setClientId(const QString& clientId)
{
    if ( this->clientId() == clientId ) return;
    settings_.setValue("clientId", clientId);
    emit clientIdChanged();
}

QString OAuth2Settings::clientSecret() const
{
    return settings_.value("clientSecret").toString();
}

void OAuth2Settings::setClientSecret(const QString& clientSecret)
{
    if ( this->clientSecret() == clientSecret ) return;
    settings_.setValue("clientSecret", clientSecret);
    emit clientSecretChanged();
}

QString OAuth2Settings::accessToken() const
{
    return settings_.value("accessToken").toString();
}

void OAuth2Settings::setAccessToken(const QString& accessToken)
{
    if ( this->accessToken() == accessToken ) return;
    settings_.setValue("accessToken", accessToken);
    emit accessTokenChanged();
}
