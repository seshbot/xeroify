#ifndef SHOPIFY_H
#define SHOPIFY_H

#include <QObject>

#include "ConnectionSettings.h"


class Shopify : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ConnectionState state READ state NOTIFY stateChanged)
    Q_PROPERTY(ConnectionSettings* settings READ settings)

public:
    enum ConnectionState
    {
        STATE_DISCONNECTED,
        STATE_CONNECTED,
        STATE_CONNECTING,
    };
    Q_ENUMS( ConnectionState )

    explicit Shopify(QObject *parent = 0);

    ConnectionState state() const;
    ConnectionSettings* settings();

signals:
    void stateChanged();

public slots:
    void connect();
    void disconnect();

private:
    ConnectionState state_;
    ConnectionSettings settings_;
};

#endif // SHOPIFY_H
