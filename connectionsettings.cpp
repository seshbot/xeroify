#include "ConnectionSettings.h"

#include <QDebug>


ConnectionSettings::ConnectionSettings(QObject *parent)
    : ConnectionSettings("default", parent)
{
}

ConnectionSettings::ConnectionSettings(const QString& section, QObject *parent)
    : QObject(parent)
    , settings_("BusyBot", "Xeroify")
{
    qDebug() << "Creating connection settings " << section;

    settings_.beginGroup(section);

    QObject::connect( this, &ConnectionSettings::urlChanged, &ConnectionSettings::changed );
    QObject::connect( this, &ConnectionSettings::apiKeyChanged, &ConnectionSettings::changed );
    QObject::connect( this, &ConnectionSettings::passwordChanged, &ConnectionSettings::changed );
}

QString ConnectionSettings::section() const
{
    return section_;
}

void ConnectionSettings::setSection( const QString& section )
{
    if ( section_ == section ) return;
    section_ = section;
    settings_.endGroup();
    settings_.beginGroup(section);
    emit urlChanged();
    emit apiKeyChanged();
    emit passwordChanged();
}

QString ConnectionSettings::url() const
{
    return settings_.value("url").toString();
}

void ConnectionSettings::setUrl( const QString& url )
{
    qDebug() << "setting url: " << url;
    if ( this->url() == url ) return;
    settings_.setValue("url", url);
    emit urlChanged();
}

QString ConnectionSettings::apiKey() const
{
    return settings_.value("apiKey").toString();
}

void ConnectionSettings::setApiKey( const QString& apiKey )
{
    if ( this->apiKey() == apiKey ) return;
    settings_.setValue("apiKey", apiKey);
    emit apiKeyChanged();
}

QString ConnectionSettings::password() const
{
    return settings_.value("password").toString();
}

void ConnectionSettings::setPassword( const QString& password )
{
    if ( this->password() == password ) return;
    settings_.setValue("password", password);
    emit passwordChanged();
}
