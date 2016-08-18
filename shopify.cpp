#include "Shopify.h"

#include <QLocale>
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

QString toPrice(const QString& value, const QString& currency)
{
    if (currency == "JPY")
    {
        auto ok = false;
        auto doubleValue = value.toDouble(&ok);
        if (ok)
        {
            auto locale = QLocale(QLocale::Japanese, QLocale::Japan);
            return locale.toCurrencyString(doubleValue);
        }
    }

    return value;
}

QString toPrice(const QJsonValue& value, const QJsonValue& currency)
{
    return toPrice(value.toString(), currency.toString());
}

template <typename JsonT>
Order::FinancialStatus toFinancialStatus(const JsonT& json)
{
    if (json.isNull()) return Order::FINANCIAL_STATUS_ANY; // this is probably wrong
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
    if (json.isNull()) return Order::FULFILLMENT_STATUS_UNSHIPPED;
    auto value = json.toString();
    if (value == "fulfilled") return Order::FULFILLMENT_STATUS_SHIPPED;
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
    return json_["country_code"].toString();
}

QString Address::firstName() const
{
    return json_["first_name"].toString();
}

QString Address::lastName() const
{
    return json_["last_name"].toString();
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
    return json_["province_code"].toString();
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
    return toPrice(json_["price"].toString(), currency_);
}

QString TaxLine::title() const
{
    return json_["title"].toString();
}


//
// ShippingLine
//

QString ShippingLine::code() const
{
    return json_["code"].toString();
}

QString ShippingLine::price() const
{
    return toPrice(json_["price"].toString(), currency_);
}

QString ShippingLine::title() const
{
    return json_["title"].toString();
}

QString ShippingLine::source() const
{
    return json_["source"].toString();
}

QList<QObject*> ShippingLine::taxLines()
{
    QList<QObject*> results;
    auto values = json_["tax_lines"].toArray();
    for (auto value: values)
    {
        results.append(new TaxLine(value.toObject(), currency_, this));
    }
    return results;
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
    return toPrice(json_["price"].toString(), currency_);
}

int LineItem::quantity() const
{
    return json_["quantity"].toInt();
}

int LineItem::fulfillableQuantity() const
{
    return json_["fulfillable_quantity"].toInt();
}

QList<QObject*> LineItem::taxLines()
{
    QList<QObject*> results;
    auto values = json_["tax_lines"].toArray();
    for (auto value: values)
    {
        results.append(new TaxLine(value.toObject(), json_["currency"].toString(), this));
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
    return toPrice(json_["total_price"], json_["currency"]);
}

QString Order::subtotalPrice() const
{
    return toPrice(json_["subtotal_price"], json_["currency"]);
}

QString Order::totalLineItemsPrice() const
{
    return toPrice(json_["total_line_items_price"], json_["currency"]);
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
        results.append(new LineItem(value.toObject(), json_["currency"].toString(), this));
    }
    return results;
}

QList<QObject*> Order::taxLines()
{
    QList<QObject*> results;
    auto values = json_["tax_lines"].toArray();
    for (auto value: values)
    {
        results.append(new TaxLine(value.toObject(), json_["currency"].toString(), this));
    }
    return results;
}

QList<QObject*> Order::shippingLines()
{
    QList<QObject*> results;
    auto values = json_["shipping_lines"].toArray();
    for (auto value: values)
    {
        results.append(new ShippingLine(value.toObject(), json_["currency"].toString(), this));
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

//
// OrderBook
//

OrderBook::OrderBook(QObject* parent)
    : QObject(parent)
    , shopify_(nullptr)
    , state_(STATE_LOADING)
    , filter_(FILTER_UNFULFILLED)
    , currentReply_(nullptr)
{
}

OrderBook::OrderBook(Shopify& shopify, QObject* parent)
    : QObject(parent)
    , shopify_(&shopify)
    , state_(STATE_LOADING)
    , filter_(FILTER_UNFULFILLED)
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
        currentReply_->abort();
        currentReply_->deleteLater();
    }

    setState(STATE_LOADING);

    QUrlQuery query;
    switch (filter_)
    {
    case FILTER_UNFULFILLED: query.addQueryItem("fulfillment_status", "unshipped"); break;
    case FILTER_PAYMENT_PENDING: query.addQueryItem("financial_status", "pending"); break;
    case FILTER_ALL: // intentional fallthrough
    default:
        break;
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
   return resolved;
}
