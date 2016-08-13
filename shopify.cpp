#include "Shopify.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <QUrlQuery>


namespace
{

template <typename JsonT>
QDate toDateTime(const JsonT& json)
{
    QDate result;
    if (!json.isNull())
    {
        result = QDate::fromString(json.toString(), Qt::ISODate);
    }
    return result;
}

template <typename JsonT>
Order::OrderStatus toOrderStatus(const JsonT& json)
{
    if (json.isNull()) return Order::ORDER_STATUS_ANY;
    auto value = json.toString();
    if (value == "open") return Order::ORDER_STATUS_OPEN;
    if (value == "closed") return Order::ORDER_STATUS_CLOSED;
    if (value == "cancelled") return Order::ORDER_STATUS_CANCELLED;
    return Order::ORDER_STATUS_ANY;
}

template <typename JsonT>
Order::FinancialStatus toFinancialStatus(const JsonT& json)
{
    if (json.isNull()) return Order::FINANCIAL_STATUS_ANY;
    auto value = json.toString();
    if (value == "authorized") return Order::FINANCIAL_STATUS_AUTHORIZED;
    if (value == "pending") return Order::FINANCIAL_STATUS_PENDING;
    if (value == "paid") return Order::FINANCIAL_STATUS_PAID;
    if (value == "partially_paid") return Order::FINANCIAL_STATUS_PARTIALLY_PAID;
    if (value == "refunded") return Order::FINANCIAL_STATUS_REFUNDED;
    if (value == "voided") return Order::FINANCIAL_STATUS_VOIDED;
    if (value == "partially_refunded") return Order::FINANCIAL_STATUS_PARTIALLY_REFUNDED;
    if (value == "unpaid") return Order::FINANCIAL_STATUS_UNPAID;
    return Order::FINANCIAL_STATUS_ANY;
}

template <typename JsonT>
Order::FulfillmentStatus toFulfillmentStatus(const JsonT& json)
{
    if (json.isNull()) return Order::FULFILLMENT_STATUS_ANY;
    auto value = json.toString();
    if (value == "shipped") return Order::FULFILLMENT_STATUS_SHIPPED;
    if (value == "partial") return Order::FULFILLMENT_STATUS_PARTIAL;
    if (value == "unshipped") return Order::FULFILLMENT_STATUS_UNSHIPPED;
    if (value == "fulfilled") return Order::FULFILLMENT_STATUS_FULFILLED;
    return Order::FULFILLMENT_STATUS_ANY;
}

template <typename JsonT>
QString toString(const JsonT& json, const QString& defaultValue)
{
    if (json.isNull()) return defaultValue;
    return json.toString();
}

} // anonymous namespace

//
// Address
//


QString Address::address1() const
{
    return json_["address1"].toString();
}

QString Address::address2() const
{
    return json_["address2"].toString();
}

QString Address::city() const
{
    return json_["city"].toString();
}

QString Address::company() const
{
    return json_["company"].toString();
}

QString Address::country() const
{
    return json_["country"].toString();
}

QString Address::countryCode() const
{
    return json_["countryCode"].toString();
}

QString Address::firstName() const
{
    return json_["firstName"].toString();
}

QString Address::lastName() const
{
    return json_["lastName"].toString();
}

QString Address::name() const
{
    return json_["name"].toString();
}

QString Address::phone() const
{
    return json_["phone"].toString();
}

QString Address::province() const
{
    return json_["province"].toString();
}

QString Address::provinceCode() const
{
    return json_["provinceCode"].toString();
}

QString Address::zip() const
{
    return json_["zip"].toString();
}


//
// Customer
//

int Customer::id() const
{
    return json_["id"].toInt();
}

Address* Customer::defaultAddress()
{
    auto value = json_["default_address"];
    auto obj = value.toObject();
    return new Address(obj, this);
}

QString Customer::email() const
{
    return json_["email"].toString();
}

QString Customer::firstName() const
{
    return json_["first_name"].toString();
}

QString Customer::lastName() const
{
    return json_["last_name"].toString();
}

QString Customer::note() const
{
    return json_["note"].toString();
}

int Customer::orderCount() const
{
    return json_["orders_count"].toInt();
}

QString Customer::state() const
{
    return json_["state"].toString();
}

QString Customer::totalSpent() const
{
    return json_["total_spent"].toString();
}

QDate Customer::createdAt() const
{
    return toDateTime(json_["created_at"]);
}

QDate Customer::updatedAt() const
{
    return toDateTime(json_["updated_at"]);
}

QStringList Customer::tags() const
{
    auto value = json_["tags"];
    if (value.isNull()) return {};
    auto result = value.toString().split(',');
    for (auto& token: result)
    {
        token = token.trimmed();
    }
    return result;
}


//
// TaxLine
//

double TaxLine::rate() const
{
    return json_["rate"].toDouble();
}

QString TaxLine::price() const
{
    return json_["price"].toString();
}

QString TaxLine::title() const
{
    return json_["title"].toString();
}


//
// LineItem
//

int LineItem::id() const
{
    return json_["id"].toInt();
}

QDate LineItem::createdAt() const
{
    return toDateTime(json_["created_at"]);
}

QDate LineItem::updatedAt() const
{
    return toDateTime(json_["updated_at"]);
}

QString LineItem::name() const
{
    return json_["name"].toString();
}

QString LineItem::title() const
{
    return json_["title"].toString();
}

QString LineItem::variantTitle() const
{
    return json_["variant_title"].toString();
}

QString LineItem::price() const
{
    return json_["price"].toString();
}

int LineItem::quantity() const
{
    return json_["quantity"].toInt();
}

QList<QObject*> LineItem::taxLines()
{
    QList<QObject*> results;
    auto values = json_["tax_lines"].toArray();
    for (auto value: values)
    {
        results.append(new TaxLine(value.toObject(), this));
    }
    return results;
}


//
// Order
//

Order::Order(QObject *parent)
    : QObject(parent)
{
}

Order::Order(const QJsonObject& json, QObject *parent)
    : QObject(parent)
    , json_(json)
{
}

int Order::id() const
{
    return json_["id"].toInt();
}

int Order::orderNumber() const
{
    return json_["order_number"].toInt();
}

QString Order::name() const
{
    return json_["name"].toString();
}

Order::OrderStatus Order::orderStatus() const
{
    if (cancelledAt().isValid()) return ORDER_STATUS_CANCELLED;
    if (closedAt().isValid()) return ORDER_STATUS_CLOSED;
    return ORDER_STATUS_OPEN;
}

QString Order::orderStatusString() const
{
    switch (orderStatus())
    {
    case ORDER_STATUS_CANCELLED: return "cancelled";
    case ORDER_STATUS_CLOSED: return "closed";
    default: return "open";
    }
}

Order::FinancialStatus Order::financialStatus() const
{
    return toFinancialStatus(json_["financial_status"]);
}

QString Order::financialStatusString() const
{
    return toString(json_["financial_status"], "");
}

Order::FulfillmentStatus Order::fulfillmentStatus() const
{
    return toFulfillmentStatus(json_["fulfillment_status"]);
}

QString Order::fulfillmentStatusString() const
{
    return toString(json_["fulfillment_status"], "not fulfilled");
}

QDate Order::createdAt() const
{
    return toDateTime(json_["created_at"]);
}

QDate Order::updatedAt() const
{
    return toDateTime(json_["updated_at"]);
}

QDate Order::closedAt() const
{
    return toDateTime(json_["closed_at"]);
}

QDate Order::cancelledAt() const
{
    return toDateTime(json_["cancelled_at"]);
}

QDate Order::processedAt() const
{
    return toDateTime(json_["processed_at"]);
}

QString Order::note() const
{
    return json_["note"].toString();
}

QString Order::currency() const
{
    return json_["currency"].toString();
}

QString Order::totalPrice() const
{
    return json_["total_price"].toString();
}

QString Order::subtotalPrice() const
{
    return json_["subtotal_price"].toString();
}

QString Order::totalLineItemsPrice() const
{
    return json_["total_line_items_price"].toString();
}

QString Order::totalTax() const
{
    return json_["total_tax"].toString();
}

bool Order::taxesIncluded() const
{
    return json_["taxes_included"].toBool();
}

Customer* Order::customer()
{
    return new Customer(json_["customer"].toObject(), this);
}

QList<QObject*> Order::lineItems()
{
    QList<QObject*> results;
    auto values = json_["line_items"].toArray();
    for (auto value: values)
    {
        results.append(new LineItem(value.toObject(), this));
    }
    return results;
}

QStringList Order::tags() const
{
    auto value = json_["tags"];
    if (value.isNull()) return {};
    auto result = value.toString().split(',');
    for (auto& token: result)
    {
        token = token.trimmed();
    }
    return result;
}

//Order Order::fromJson(const QJsonObject& json)
//{
//    json["test"].toBool();
//    json["confirmed"].toBool();



//    // customer

//    //      "shipping_lines": [
//    //      ],
//    //      "fulfillments": [
//    //      ],


//    //      "line_items": [
//    //        {
//    //          "id": 1071823191,
//    //          "variant_id": 447654529,
//    //          "title": "IPod Touch 8GB",
//    //          "quantity": 1,
//    //          "price": "199.00",
//    //          "grams": 200,
//    //          "sku": "IPOD2009BLACK",
//    //          "variant_title": "Black",
//    //          "vendor": null,
//    //          "fulfillment_service": "manual",
//    //          "product_id": 921728736,
//    //          "requires_shipping": true,
//    //          "taxable": true,
//    //          "gift_card": false,
//    //          "name": "IPod Touch 8GB - Black",
//    //          "variant_inventory_management": "shopify",
//    //          "properties": [
//    //          ],
//    //          "product_exists": true,
//    //          "fulfillable_quantity": 1,
//    //          "total_discount": "0.00",
//    //          "fulfillment_status": null,
//    //          "tax_lines": [
//    //          ]
//    //        }
//    //      ],


//}

//
// OrderBook
//

OrderBook::OrderBook(QObject* parent)
    : QObject(parent)
    , shopify_(nullptr)
    , state_(STATE_LOADING)
    , currentReply_(nullptr)
{
}

OrderBook::OrderBook(Shopify& shopify, QObject* parent)
    : QObject(parent)
    , shopify_(&shopify)
    , state_(STATE_LOADING)
    , currentReply_(nullptr)
{
    reload();
}

void OrderBook::onReplyFinished()
{
    if (currentReply_->error() != QNetworkReply::NoError)
    {
        errorMessage_ = currentReply_->errorString();
        emit errorMessageChanged();
        setState(STATE_ERROR);
        return;
    }
    else
    {
        auto data = currentReply_->readAll();
        auto json = QJsonDocument::fromJson(data);

        qDebug("OrderBook response:\n%s", json.toJson(QJsonDocument::Indented).toStdString().c_str());

        auto entityWrapper = json.object();
        auto entities = entityWrapper[entityWrapper.keys()[0]].toArray();

        orders_.clear();
        for (auto entity: entities)
        {
            orders_.push_back(new Order(entity.toObject()));
        }

        emit ordersChanged();

        setState(STATE_LOADED);
    }

    if (currentReply_)
    {
        currentReply_->deleteLater();
        currentReply_ = nullptr;
    }
}

void OrderBook::setState(State state)
{
    state_ = state;
    emit stateChanged();
}

void OrderBook::reload()
{
    if (currentReply_)
    {
        currentReply_->deleteLater();
    }

    setState(STATE_LOADING);

    QUrlQuery query;
    qDebug() << "considering query params: " << filterByLastModifiedStart_ << ", " << filterByLastModifiedEnd_;
    if (filterByLastModifiedStart_ && !lastModifiedStart_.isNull())
    {
        qDebug() << "adding lastmodified min: " << lastModifiedStart_.toString(Qt::ISODate);
        // 2014-04-25T16:15:47-04:00
        query.addQueryItem("updated_at_min", lastModifiedStart_.toString(Qt::ISODate));
    }
    if (filterByLastModifiedEnd_ && !lastModifiedEnd_.isNull())
    {
        qDebug() << "adding lastmodified max: " << lastModifiedStart_.toString(Qt::ISODate);
        query.addQueryItem("updated_at_max", lastModifiedEnd_.toString(Qt::ISODate));
    }

    currentReply_ = shopify_->makeGetRequest("orders", query);
    currentReply_->setParent(this);
    connect(currentReply_, &QNetworkReply::finished, this, &OrderBook::onReplyFinished);
}


//
// Shopify
//

Shopify::Shopify(QObject *parent)
    : QObject(parent)
    , settings_("shopify")
{
    qDebug() << "creating shopify";
}

SimpleHttpConnectionSettings* Shopify::settings()
{
    return &settings_;
}

QNetworkReply* Shopify::makeGetRequest(const QString& entity, const QUrlQuery& query)
{
    QNetworkRequest request;
    request.setUrl(makeGetUrl(entity, query));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "requesting:" << request.url().url();

    return http_.get(request);
}

QUrl Shopify::makeBaseUrl() const
{
   QUrl url(settings_.url());
   if (url.scheme().isEmpty())
       url.setScheme("https");
   url.setUserName(settings_.apiKey());
   url.setPassword(settings_.password());
   return url;
}

QUrl Shopify::makeGetUrl(const QString& entity, const QUrlQuery& query) const
{
   auto base = makeBaseUrl();

   auto relative = QUrl("/admin/" + entity + ".json");
   relative.setQuery(query);

   auto resolved = base.resolved(relative);
   qDebug() << "resolved url:" << resolved.url();

   return resolved;
}
