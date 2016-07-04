#include "Shopify.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QtDebug>


namespace
{

template <typename JsonT>
QDateTime toDateTime(const JsonT& json)
{
    QDateTime result;
    if (!json.isNull())
    {
        result = QDateTime::fromString(json.toString(), Qt::ISODate);
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
    return Order::FULFILLMENT_STATUS_ANY;
}

template <typename JsonT>
QString toString(const JsonT& json, const QString& defaultValue)
{
    if (json.isNull()) return defaultValue;
    return json.toString();
}

} // anonymous namespace


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
    return toString(json_["fulfillment_status"], "");
}

QDateTime Order::createdAt() const
{
    return toDateTime(json_["created_at"]);
}

QDateTime Order::updatedAt() const
{
    return toDateTime(json_["updated_at"]);
}

QDateTime Order::closedAt() const
{
    return toDateTime(json_["closed_at"]);
}

QDateTime Order::cancelledAt() const
{
    return toDateTime(json_["cancelled_at"]);
}

QDateTime Order::processedAt() const
{
    return toDateTime(json_["processed_at"]);
}

QString Order::note() const
{
    return json_["note"].toString();
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


Shopify::Shopify(QObject *parent)
    : QObject(parent)
    , state_(STATE_IDLE)
    , settings_("shopify")
    , currentReply_(nullptr)
{
    qDebug() << "creating shopify";
    QObject::connect(&http_, &QNetworkAccessManager::finished, this, &Shopify::handleFinished);
}

Shopify::ConnectionState Shopify::state() const
{
    return state_;
}

ConnectionSettings* Shopify::settings()
{
    return &settings_;
}

QList<QObject*> Shopify::orders() const
{
    return orders_;
}

void Shopify::load()
{
    if (STATE_LOADING == state_) return;

    qDebug() << "connecting...";
    setState(STATE_LOADING);

    QNetworkRequest request;
    request.setUrl(makeGetUrl("orders"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //QNetworkReply *reply = nam.post(request, QJsonDocument(json).toJson());

    currentReply_ = http_.get(request);
}

void Shopify::abort()
{
    if (STATE_LOADING != state_) return;

    qDebug() << "aborting...";
    setState(STATE_ABORTING);
    currentReply_->abort();
}

void Shopify::handleFinished(QNetworkReply* reply)
{
    reply->deleteLater();
    currentReply_ = nullptr;

    qDebug().nospace() << "got response (" << (reply->isFinished() ? "" : "not ") << "finished)";
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network error: " << reply->errorString();
        setState(STATE_ERROR);
        return;
    }

    setState(STATE_IDLE);

    auto data = reply->readAll();
    auto json = QJsonDocument::fromJson(data);

    auto entityWrapper = json.object();
    auto entities = entityWrapper[entityWrapper.keys()[0]].toArray();

    qDebug().nospace() << "shopify response (" << entities.size() << " entities)";
    orders_.clear();
    for (auto entity: entities)
    {
        orders_.push_back(new Order(entity.toObject()));
    }

    emit ordersLoaded();

}

void Shopify::setState(ConnectionState state)
{
    state_ = state;
    emit stateChanged();
}

QUrl Shopify::makeBaseUrl() const
{
   QUrl url(settings_.url());
   url.setUserName(settings_.apiKey());
   url.setPassword(settings_.password());
   return url;
}

QUrl Shopify::makeGetUrl(const QString& entity) const
{
   return makeBaseUrl().resolved(QUrl("/admin/" + entity + ".json"));
}
