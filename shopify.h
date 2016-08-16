#ifndef SHOPIFY_H
#define SHOPIFY_H

#include <QObject>
#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDate>
#include <QJsonDocument>

#include <QJsonObject>

#include "ConnectionSettings.h"


class Address : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address1 READ address1 CONSTANT)
    Q_PROPERTY(QString address2 READ address2 CONSTANT)
    Q_PROPERTY(QString city READ city CONSTANT)
    Q_PROPERTY(QString company READ company CONSTANT)
    Q_PROPERTY(QString country READ country CONSTANT)
    Q_PROPERTY(QString countryCode READ countryCode CONSTANT)
    Q_PROPERTY(QString firstName READ firstName CONSTANT)
    Q_PROPERTY(QString lastName READ lastName CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString phone READ phone CONSTANT)
    Q_PROPERTY(QString province READ province CONSTANT)
    Q_PROPERTY(QString provinceCode READ provinceCode CONSTANT)
    Q_PROPERTY(QString zip READ zip CONSTANT)
    Q_PROPERTY(QString json READ json CONSTANT)

public:

    explicit Address(QObject *parent = 0) : QObject(parent) { }
    Address(const QJsonObject& json, QObject *parent = 0) : QObject(parent), json_(json) { }

    QString address1() const;
    QString address2() const;
    QString city() const;
    QString company() const;
    QString country() const;
    QString countryCode() const;
    QString firstName() const;
    QString lastName() const;
    QString name() const;
    QString phone() const;
    QString province() const;
    QString provinceCode() const;
    QString zip() const;

    QString json() const { return QJsonDocument(json_).toJson(QJsonDocument::Indented); }

private:
    QJsonObject json_;
};

class Customer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QDate createdAt READ createdAt CONSTANT)
    Q_PROPERTY(QDate updatedAt READ updatedAt CONSTANT)
    Q_PROPERTY(Address* defaultAddress READ defaultAddress CONSTANT)
    Q_PROPERTY(QString email READ email CONSTANT)
    Q_PROPERTY(QString firstName READ firstName CONSTANT)
    Q_PROPERTY(QString lastName READ lastName CONSTANT)
    Q_PROPERTY(QString note READ note CONSTANT)
    Q_PROPERTY(int orderCount READ orderCount CONSTANT)
    Q_PROPERTY(QString state READ state CONSTANT)
    Q_PROPERTY(QString totalSpent READ totalSpent CONSTANT)
    Q_PROPERTY(QStringList tags READ tags CONSTANT)
    Q_PROPERTY(QString json READ json CONSTANT)

public:

    explicit Customer(QObject *parent = 0) : QObject(parent) { }
    Customer(const QJsonObject& json, QObject *parent = 0) : QObject(parent), json_(json) { }

    int id() const;
    QDate createdAt() const;
    QDate updatedAt() const;

    Address* defaultAddress();

    QString email() const;
    QString firstName() const;
    QString lastName() const;
    QString note() const;
    int orderCount() const;
    QString state() const;
    QString totalSpent() const;

    QStringList tags() const;

    QString json() const { return QJsonDocument(json_).toJson(QJsonDocument::Indented); }

private:
    QJsonObject json_;
};

class TaxLine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double rate READ rate CONSTANT)
    Q_PROPERTY(QString price READ price CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)

public:
    explicit TaxLine(QObject *parent = 0) : QObject(parent) { }
    TaxLine(const QJsonObject& json, const QString& currency, QObject *parent = 0) : QObject(parent), currency_(currency), json_(json) { }

    double rate() const;
    QString price() const;
    QString title() const;

private:
    QString currency_;
    QJsonObject json_;
};

class ShippingLine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString code READ code CONSTANT)
    Q_PROPERTY(QString price READ price CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString source READ source CONSTANT)
    Q_PROPERTY(QList<QObject*> taxLines READ taxLines CONSTANT)

public:
    explicit ShippingLine(QObject* parent = 0) : QObject(parent) { }
    ShippingLine(const QJsonObject& json, const QString& currency, QObject* parent = 0) : QObject(parent), currency_(currency), json_(json) { }

    QString code() const;
    QString price() const;
    QString title() const;
    QString source() const;

    QList<QObject*> taxLines();

    QString json() const { return QJsonDocument(json_).toJson(QJsonDocument::Indented); }

private:
    QString currency_;
    QJsonObject json_;
};

class LineItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QDate createdAt READ createdAt CONSTANT)
    Q_PROPERTY(QDate updatedAt READ updatedAt CONSTANT)
    Q_PROPERTY(QList<QObject*> taxLines READ taxLines CONSTANT)

public:
    explicit LineItem(QObject *parent = 0) : QObject(parent) { }
    LineItem(const QJsonObject& json, QObject *parent = 0) : QObject(parent), json_(json) { }

    int id() const;
    QDate createdAt() const;
    QDate updatedAt() const;

    QString name() const;
    QString title() const;
    QString variantTitle() const;
    QString price() const;
    int quantity() const;

    QList<QObject*> taxLines();

    QString json() const { return QJsonDocument(json_).toJson(QJsonDocument::Indented); }

private:
    QJsonObject json_;
};

class Order : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(int orderNumber READ orderNumber CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString currency READ currency CONSTANT)
    Q_PROPERTY(QString totalPrice READ totalPrice CONSTANT)
    Q_PROPERTY(OrderStatus orderStatus READ orderStatus CONSTANT)
    Q_PROPERTY(QString orderStatusString READ orderStatusString CONSTANT)
    Q_PROPERTY(FinancialStatus financialStatus READ financialStatus CONSTANT)
    Q_PROPERTY(QString financialStatusString READ financialStatusString CONSTANT)
    Q_PROPERTY(FulfillmentStatus fulfillmentStatus READ fulfillmentStatus CONSTANT)
    Q_PROPERTY(QString fulfillmentStatusString READ fulfillmentStatusString CONSTANT)
    Q_PROPERTY(QDate createdAt READ createdAt CONSTANT)
    Q_PROPERTY(QDate updatedAt READ updatedAt CONSTANT)
    Q_PROPERTY(QDate closedAt READ closedAt CONSTANT)
    Q_PROPERTY(QDate cancelledAt READ cancelledAt CONSTANT)
    Q_PROPERTY(QDate processedAt READ processedAt CONSTANT)
    Q_PROPERTY(Customer* customer READ customer CONSTANT)
    Q_PROPERTY(QList<QObject*> lineItems READ lineItems CONSTANT)
    Q_PROPERTY(QList<QObject*> taxLines READ taxLines CONSTANT)
    Q_PROPERTY(QStringList tags READ tags CONSTANT)
    Q_PROPERTY(QString json READ json CONSTANT)

public:
    enum OrderStatus
    {
        ORDER_STATUS_ANY,
        ORDER_STATUS_OPEN,
        ORDER_STATUS_CLOSED,
        ORDER_STATUS_CANCELLED,
    };
    Q_ENUMS( OrderStatus )

    enum FinancialStatus
    {
        FINANCIAL_STATUS_ANY,
        FINANCIAL_STATUS_AUTHORIZED,
        FINANCIAL_STATUS_PENDING,
        FINANCIAL_STATUS_PAID,
        FINANCIAL_STATUS_PARTIALLY_PAID,
        FINANCIAL_STATUS_REFUNDED,
        FINANCIAL_STATUS_VOIDED,
        FINANCIAL_STATUS_PARTIALLY_REFUNDED,
        FINANCIAL_STATUS_UNPAID,
    };
    Q_ENUMS( FinancialStatus )

    enum FulfillmentStatus
    {
        FULFILLMENT_STATUS_ANY,
        FULFILLMENT_STATUS_PARTIAL = 1,
        FULFILLMENT_STATUS_UNSHIPPED = 2,
        FULFILLMENT_STATUS_SHIPPED = 4,
    };
    Q_ENUMS( FulfillmentStatus )

    explicit Order(QObject *parent = 0);
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

    QDate createdAt() const;
    QDate updatedAt() const;
    QDate closedAt() const;
    QDate cancelledAt() const;
    QDate processedAt() const;

    QString note() const;

    QString currency() const;
    QString totalPrice() const;
    QString subtotalPrice() const;
    QString totalLineItemsPrice() const;
    QString totalTax() const;

    bool taxesIncluded() const;

    Customer* customer();
    QList<QObject*> lineItems();

    QList<QObject*> taxLines();

    QStringList tags() const;

    QString json() const { return QJsonDocument(json_).toJson(QJsonDocument::Indented); }

private:
    QJsonObject json_;
};

class Shopify;

class OrderBook : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Shopify* shopify READ shopify WRITE setShopify)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(QString customer READ customer WRITE setCustomer)
    Q_PROPERTY(bool filterByLastModifiedStart READ filterByLastModifiedStart WRITE setFilterByLastModifiedStart NOTIFY filterByLastModifiedStartChanged)
    Q_PROPERTY(bool filterByLastModifiedEnd READ filterByLastModifiedEnd WRITE setFilterByLastModifiedEnd NOTIFY filterByLastModifiedEndChanged)
    Q_PROPERTY(QDate lastModifiedStart READ lastModifiedStart WRITE setLastModifiedStart)
    Q_PROPERTY(QDate lastModifiedEnd READ lastModifiedEnd WRITE setLastModifiedEnd)
    Q_PROPERTY(QList<QObject*> orders READ orders NOTIFY ordersChanged)
    Q_PROPERTY(bool showUnshipped READ showUnshipped WRITE setShowUnshipped NOTIFY showUnshippedUpdated)
    Q_PROPERTY(bool showPartial READ showPartial WRITE setShowPartial NOTIFY showPartialUpdated)
    Q_PROPERTY(bool showShipped READ showShipped WRITE setShowShipped NOTIFY showShippedUpdated)

public:
    enum State
    {
        STATE_LOADING,
        STATE_ERROR,
        STATE_LOADED,
    };
    Q_ENUMS(State)

    // the app uses 'Open, Unfulfilled, Unpaid'

    explicit OrderBook(QObject* parent = 0);
    OrderBook(Shopify& shopify, QObject* parent = 0);

    State state() const { return state_; }
    QString errorMessage() const { return errorMessage_; }

    Shopify* shopify() { return shopify_; }
    void setShopify(Shopify* shopify) { shopify_ = shopify; reload(); }

    QString customer() const { return customer_; }
    void setCustomer(const QString& customer) { customer_ = customer; }

    void setFilterByLastModifiedStart(bool value) { if (filterByLastModifiedStart_ == value) return; filterByLastModifiedStart_ = value; emit filterByLastModifiedStartChanged(); reload(); }
    bool filterByLastModifiedStart() const { return filterByLastModifiedStart_; }
    void setLastModifiedStart(const QDate& date) { lastModifiedStart_ = date; if (filterByLastModifiedStart_) reload(); }
    QDate lastModifiedStart() const { return lastModifiedStart_; }

    void setFilterByLastModifiedEnd(bool value) { if (filterByLastModifiedEnd_ == value) return; filterByLastModifiedEnd_ = value; emit filterByLastModifiedEndChanged(); reload(); }
    bool filterByLastModifiedEnd() const { return filterByLastModifiedEnd_; }
    void setLastModifiedEnd(const QDate& date) { lastModifiedEnd_ = date; if (filterByLastModifiedEnd_) reload(); }
    QDate lastModifiedEnd() const { return lastModifiedEnd_; }

    bool showUnshipped() const { return filterShowUnshipped_; }
    bool showPartial() const { return filterShowPartial_; }
    bool showShipped() const { return filterShowShipped_; }

    void setShowUnshipped(bool value) { filterShowUnshipped_ = value; reload(); }
    void setShowPartial(bool value) { filterShowPartial_ = value; reload(); }
    void setShowShipped(bool value) { filterShowShipped_ = value; reload(); }
    QList<QObject*> orders() { return orders_; }

signals:
    void stateChanged();
    void ordersChanged();
    void errorMessageChanged();
    void filterByLastModifiedStartChanged();
    void filterByLastModifiedEndChanged();
    void showUnshippedUpdated();
    void showPartialUpdated();
    void showShippedUpdated();

private slots:
    void onReplyFinished();

private:
    void setState(State state);
    void reload();

    Shopify* shopify_;

    State state_;
    QString errorMessage_;

    QString customer_;

    bool filterByLastModifiedStart_;
    bool filterByLastModifiedEnd_;
    QDate lastModifiedStart_;
    QDate lastModifiedEnd_;
    bool filterShowUnshipped_;
    bool filterShowPartial_;
    bool filterShowShipped_;

    QList<QObject*> orders_;

    QNetworkReply* currentReply_;
};


//"customer": {
//    "id": 207119551,
//    "email": "bob.norman@hostmail.com",
//    "accepts_marketing": false,
//    "created_at": "2016-06-20T13:41:39-04:00",
//    "updated_at": "2016-06-20T13:41:39-04:00",
//    "first_name": "Bob",
//    "last_name": "Norman",
//    "orders_count": 1,
//    "state": "disabled",
//    "total_spent": "41.94",
//    "last_order_id": 450789469,
//    "note": null,
//    "verified_email": true,
//    "multipass_identifier": null,
//    "tax_exempt": false,
//    "tags": "",
//    "last_order_name": "#1001",
//    "default_address": {
//      "id": 207119551,
//      "first_name": null,
//      "last_name": null,
//      "company": null,
//      "address1": "Chestnut Street 92",
//      "address2": "",
//      "city": "Louisville",
//      "province": "Kentucky",
//      "country": "United States",
//      "zip": "40202",
//      "phone": "555-625-1199",
//      "name": "",
//      "province_code": "KY",
//      "country_code": "US",
//      "country_name": "United States",
//      "default": true
//    },
//    "addresses": [
//      {
//        "id": 207119551,
//        "first_name": null,
//        "last_name": null,
//        "company": null,
//        "address1": "Chestnut Street 92",
//        "address2": "",
//        "city": "Louisville",
//        "province": "Kentucky",
//        "country": "United States",
//        "zip": "40202",
//        "phone": "555-625-1199",
//        "name": "",
//        "province_code": "KY",
//        "country_code": "US",
//        "country_name": "United States",
//        "default": true
//      }
//    ]
//  }



class Shopify : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SimpleHttpConnectionSettings* settings READ settings)

public:
    explicit Shopify(QObject *parent = 0);

    SimpleHttpConnectionSettings* settings();

private:
    friend class OrderBook;

    QNetworkReply* makeGetRequest(const QString& entity, const QUrlQuery& query);

    QUrl makeBaseUrl() const;
    QUrl makeGetUrl(const QString& entity, const QUrlQuery& query) const;

    SimpleHttpConnectionSettings settings_;
    QNetworkAccessManager http_;
};

#endif // SHOPIFY_H
