#ifndef SHOPIFY_H
#define SHOPIFY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QJsonObject>

#include "ConnectionSettings.h"


class Order : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(int orderNumber READ orderNumber CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString totalPrice READ totalPrice CONSTANT)
    Q_PROPERTY(OrderStatus orderStatus READ orderStatus CONSTANT)
    Q_PROPERTY(QString orderStatusString READ orderStatusString CONSTANT)
    Q_PROPERTY(FinancialStatus financialStatus READ financialStatus CONSTANT)
    Q_PROPERTY(QString financialStatusString READ financialStatusString CONSTANT)
    Q_PROPERTY(FulfillmentStatus fulfillmentStatus READ fulfillmentStatus CONSTANT)
    Q_PROPERTY(QString fulfillmentStatusString READ fulfillmentStatusString CONSTANT)

public:
    enum OrderStatus
    {
        ORDER_STATUS_OPEN,
        ORDER_STATUS_CLOSED,
        ORDER_STATUS_CANCELLED,
        ORDER_STATUS_ANY,
    };
    Q_ENUMS( OrderStatus )

    enum FinancialStatus
    {
        FINANCIAL_STATUS_AUTHORIZED,
        FINANCIAL_STATUS_PENDING,
        FINANCIAL_STATUS_PAID,
        FINANCIAL_STATUS_PARTIALLY_PAID,
        FINANCIAL_STATUS_REFUNDED,
        FINANCIAL_STATUS_VOIDED,
        FINANCIAL_STATUS_PARTIALLY_REFUNDED,
        FINANCIAL_STATUS_ANY,
        FINANCIAL_STATUS_UNPAID,
    };
    Q_ENUMS( FinancialStatus )

    enum FulfillmentStatus
    {
        FULFILLMENT_STATUS_SHIPPED,
        FULFILLMENT_STATUS_PARTIAL,
        FULFILLMENT_STATUS_UNSHIPPED,
        FULFILLMENT_STATUS_ANY,
    };
    Q_ENUMS( FulfillmentStatus )

    Order(const QJsonObject& json, QObject *parent = 0);

    int id() const;
    int orderNumber() const;
    QString name() const;

    OrderStatus orderStatus() const;
    QString orderStatusString() const;
    FinancialStatus financialStatus() const;
    QString financialStatusString() const;
    FulfillmentStatus fulfillmentStatus() const;
    QString fulfillmentStatusString() const;

    QDateTime createdAt() const;
    QDateTime updatedAt() const;
    QDateTime closedAt() const;
    QDateTime cancelledAt() const;
    QDateTime processedAt() const;

    QString note() const;

    QString totalPrice() const;
    QString subtotalPrice() const;
    QString totalLineItemsPrice() const;
    QString totalTax() const;

    bool taxesIncluded() const;

    QStringList tags() const;

private:
    QJsonObject json_;
};

class Shopify : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ConnectionState state READ state NOTIFY stateChanged)
    Q_PROPERTY(SimpleHttpConnectionSettings* settings READ settings)
    Q_PROPERTY(QList<QObject*> orders READ orders NOTIFY ordersLoaded)

public:
    enum ConnectionState
    {
        STATE_IDLE,
        STATE_LOADING,
        STATE_ABORTING,
        STATE_ERROR,
    };
    Q_ENUMS( ConnectionState )

    explicit Shopify(QObject *parent = 0);

    ConnectionState state() const;
    SimpleHttpConnectionSettings* settings();
    QList<QObject*> orders() const;

signals:
    void stateChanged();
    void ordersLoaded();

public slots:
    void load();
    void abort();

private slots:
    void onReplyFinished(QNetworkReply* reply);

private:
    void setState(ConnectionState state);
    QUrl makeBaseUrl() const;
    QUrl makeGetUrl(const QString& entity) const;

    ConnectionState state_;
    SimpleHttpConnectionSettings settings_;
    QNetworkAccessManager http_;
    QNetworkReply* currentReply_;
    QList<QObject*> orders_;
};

#endif // SHOPIFY_H
