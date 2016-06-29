#include "Shopify.h"

Shopify::Shopify(QObject *parent)
    : QObject(parent)
    , state_(STATE_DISCONNECTED)
    , settings_("shopify")
{
    qDebug() << "creating shopify";
}

Shopify::ConnectionState Shopify::state() const
{
    return state_;
}

ConnectionSettings* Shopify::settings()
{
    return &settings_;
}

void Shopify::connect()
{
    if ( STATE_CONNECTED == state_ ) return;
    qDebug() << "connecting...";
    state_ = STATE_CONNECTED;
    emit stateChanged();
}

void Shopify::disconnect()
{
    if ( STATE_DISCONNECTED == state_ ) return;
    qDebug() << "disconnecting...";
    state_ = STATE_DISCONNECTED;
    emit stateChanged();
}
