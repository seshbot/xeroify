#include "Xero.h"

#include <QDir>
#include <QUrl>
#include <QUrlQuery>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include <XeroInvoice.h>


namespace
{

auto XERO_API_BASE = "https://api.xero.com/api.xro/2.0";

}

Xero::Xero(QObject *parent)
   : QObject(parent)
   , state_(STATE_IDLE)
   , oauth_(settings_.consumerKey(), settings_.rsaPrivateKey())
   , currentReply_(nullptr)
{
   qDebug() << "creating shopify";
   connect(&http_, &QNetworkAccessManager::finished, this, &Xero::onReplyFinished);
}

Xero::ConnectionState Xero::state() const
{
    return state_;
}

OAuthZeroLeggedConnectionSettings* Xero::settings()
{
    return &settings_;
}

QList<QObject*> Xero::invoices() const
{
    return invoices_;
}

void Xero::load()
{
    if (STATE_LOADING == state_) return;

    qDebug() << "connecting...";
    setState(STATE_LOADING);

    currentReply_ = loadEntity("Invoices");
}

void Xero::abort()
{
    if (STATE_LOADING != state_) return;

    qDebug() << "aborting...";
    setState(STATE_ABORTING);
    currentReply_->abort();
}

QNetworkReply* Xero::loadEntity(const QString& entity, const QString& entityId)
{
   oauth_.setConsumerKey(settings_.consumerKey());
   oauth_.setRsaPrivateKey(settings_.rsaPrivateKey());

   auto url = QUrl { QString("%1/%2/%3").arg(XERO_API_BASE).arg(entity).arg(entityId) };
   auto query = QUrlQuery { url };
   //query.addQueryItem("ContactID", "333f324f-ced8-481b-8f5d-231d26e35c51");
   url.setQuery(query);

   auto request = QNetworkRequest{ url };
   request.setRawHeader("Accept", "application/json");

   oauth_.authorize(request, QNetworkAccessManager::GetOperation, query.queryItems());

   qDebug() << "making request: " << url.toString().toLatin1();
   return http_.get(request);
}

void Xero::setState(ConnectionState state)
{
    state_ = state;
    emit stateChanged();
}

void Xero::onReplyFinished(QNetworkReply* reply)
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

   qDebug().nospace() << "xero response (" << entities.size() << " entities)";
   invoices_.clear();
//   for (auto entity: entities)
//   {
//       invoices_.push_back(new XeroInvoice(entity.toObject()));
//   }

   emit invoicesLoaded();
}
