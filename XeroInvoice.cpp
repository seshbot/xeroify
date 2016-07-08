#include "XeroInvoice.h"

namespace
{


template <typename JsonT>
XeroInvoice::InvoiceType toInvoiceType(const JsonT& json)
{
   if (json.isNull()) return XeroInvoice::INVOICE_TYPE_UNKNOWN;
   auto value = json.toString();
   if (value == "ACCPAY") return XeroInvoice::INVOICE_TYPE_ACCOUNTS_PAYABLE;
   if (value == "ACCREC") return XeroInvoice::INVOICE_TYPE_ACCOUNTS_RECEIVABLE;
   return XeroInvoice::INVOICE_TYPE_UNKNOWN;
}

template <typename JsonT>
XeroInvoice::InvoiceStatus toInvoiceStatus(const JsonT& json)
{
   if (json.isNull()) return XeroInvoice::INVOICE_STATUS_UNKNOWN;
   auto value = json.toString();
   if (value == "DRAFT") return XeroInvoice::INVOICE_STATUS_DRAFT;
   if (value == "SUBMITTED") return XeroInvoice::INVOICE_STATUS_SUBMITTED;
   if (value == "DELETED") return XeroInvoice::INVOICE_STATUS_DELETED;
   if (value == "AUTHORISED") return XeroInvoice::INVOICE_STATUS_AUTHORISED;
   if (value == "PAID") return XeroInvoice::INVOICE_STATUS_PAID;
   if (value == "VOIDED") return XeroInvoice::INVOICE_STATUS_VOIDED;
   return XeroInvoice::INVOICE_STATUS_UNKNOWN;
}

template <typename JsonT>
QString toString(const JsonT& json, const QString& defaultValue)
{
    if (json.isNull()) return defaultValue;
    return json.toString();
}

template <typename JsonT>
QDateTime toDateTime(const JsonT& json)
{
    QDateTime result;
    if (!json.isNull())
    {
        result = QDateTime::fromString(json.toString(), "yyyy-MM-dd");
    }
    return result;
}

} // anonymous namespace


XeroInvoice::XeroInvoice(const QJsonObject& json, QObject *parent)
   : QObject(parent)
   , json_(json)
{
}

QString XeroInvoice::id() const
{
    return json_["InvoiceID"].toString();
}

XeroInvoice::InvoiceType XeroInvoice::type() const
{
    return toInvoiceType(json_["Type"]);
}

XeroInvoice::InvoiceStatus XeroInvoice::status() const
{
    return toInvoiceStatus(json_["Status"]);
}

QString XeroInvoice::number() const
{
    return toString(json_["InvoiceNumber"], "");
}

QString XeroInvoice::reference() const
{
    return toString(json_["Reference"], "");
}

QDateTime XeroInvoice::date() const
{
    return toDateTime(json_["Date"]);
}

QDateTime XeroInvoice::dueDate() const
{
    return toDateTime(json_["DueDate"]);
}

QString XeroInvoice::amountDue() const
{
    return toString(json_["AmountDue"], "");
}

QString XeroInvoice::amountPaid() const
{
    return toString(json_["AmountPaid"], "");
}

QString XeroInvoice::amountCredited() const
{
    return toString(json_["AmountCredited"], "");
}

QString XeroInvoice::url() const
{
    return toString(json_["Url"], "");
}
