#include "MakeLeaps.h"

#include <QNetworkReply>
#include <QUrl>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAuthenticator>

// Property:
// - name
// - type: resource, resource_array, endpoint

// Resource:
//  - name
//  - url
//  - properties

/// https://app.makeleaps.com/api/
//{
//  "partners": [
//      {
//          "url": "https://app.makeleaps.com/api/partner/1725857643916378083/",
//          "name": "Kyoto Brewing Co., Ltd"
//      }
//  ],
//  "currencies": "https://app.makeleaps.com/api/currency/"
//}

/// https://app.makeleaps.com/api/partner/1725857643916378083/
//{
//   "url": "https://app.makeleaps.com/api/partner/1725857643916378083/",
//   "name": "Kyoto Brewing Co., Ltd",
//   "clients": "https://app.makeleaps.com/api/partner/1725857643916378083/client/",
//   "contacts": "https://app.makeleaps.com/api/partner/1725857643916378083/contact/",
//   "documents": "https://app.makeleaps.com/api/partner/1725857643916378083/document/",
//   "tags": "https://app.makeleaps.com/api/partner/1725857643916378083/tag/"
//}

/// https://app.makeleaps.com/api/partner/1725857643916378083/contact/
//[
//   {
//      "url": "https://app.makeleaps.com/api/partner/1725857643916378083/contact/1725857644256983755/",
//      "mid": "1725857644256983755",
//      "owner": "https://app.makeleaps.com/api/partner/1725857643916378083/",
//      "owner_type": "partner",
//      "contact_type": "organization",
//      "name": "Your Company",
//      "display_name": "Your Company",
//      "title": "",
//      "is_default": true,
//      "email": null,
//      "addresses": [
//          {
//              "format": "jp_default",
//              "country_name": "JP",
//              "street_address": "西九条高畠町25-1",
//              "extended_address": "",
//              "locality": "京都市南区",
//              "region": "kyoto",
//              "postal_code": "601-8446",
//              "display": "601-8446京都府京都市南区西九条高畠町25-1"
//          }
//      ],
//      "default_address": {
//          "formatted_html": "\n\n\n\n\n<div class=\"adr jp-default\">\n〒<span class=\"postal-code\">601-8446</span><br><span class=\"region\">京都府</span><span class=\"locality\">京都市南区</span><span class=\"street-address\">西九条高畠町25-1</span>\n</div>\n\n\n\n\n"
//      }
//   }
//]

namespace
{

auto MAKELEAPS_OAUTH_TOKEN_ENDPOINT = "https://api.makeleaps.com/user/oauth2/token/";
auto MAKELEAPS_API_BASE = "https://api.makeleaps.com/api/";

}


//
// MakeLeapsResourceProperty
//

MakeLeapsResourceProperty::MakeLeapsResourceProperty(const QString& name, const QJsonValue& value, QObject* parent)
   : QObject(parent), name_(name), resource_(nullptr), endpoint_(nullptr), value_(value)
{
   // "https://..." is an endpoint
   if (value_.type() == QJsonValue::String && value_.toString().startsWith(MAKELEAPS_API_BASE))
   {
      qDebug() << name << ": endpoint";
      type_ = TYPE_ENDPOINT;
      endpoint_ = new MakeLeapsEndpoint(value_.toString(), false, this);
   }
   // [ { }, ... ]
   else if (value_.type() == QJsonValue::Array)
   {
      qDebug() << name << ": array";
      auto array = value_.toArray();
      auto arrayOfResources = true;
      for (auto v: array)
      {
         if (!v.isObject())
         {
            arrayOfResources = false;
            break;
         }

         auto object = v.toObject();
         if ( !object.contains("name") || !object.contains("url") )
         {
            arrayOfResources = false;
            break;
         }
      }

      // [ { id: '', url: '', ... }, ... ]
      if ( arrayOfResources )
      {
         type_ = TYPE_RESOURCE_ARRAY;
         for (auto v: array)
         {
            resources_.append(new MakeLeapsResource(v.toObject(), this));
         }
      }
      // [ { ... }, ... ] or [ "", ... ]
      else
      {
         type_ = TYPE_VALUE_ARRAY;
         for (auto v: array)
         {
            properties_.append(new MakeLeapsResourceProperty("", v, this));
         }
      }
   }
   // { ... }
   else if (value_.type() == QJsonValue::Object)
   {
      qDebug() << name << ": object";

      type_ = TYPE_RESOURCE;
      resource_ = new MakeLeapsResource(value_.toObject(), this);
   }
   // ""
   else
   {
      qDebug() << name << ": value";
      type_ = TYPE_VALUE;
      stringValue_ = QJsonDocument::fromVariant(value_.toVariant()).toJson(QJsonDocument::Compact);
   }
}


//
// MakeLeaps
//

MakeLeaps::MakeLeaps(QObject *parent)
   : QObject(parent)
   , state_(STATE_IDLE)
   , oauth_(MAKELEAPS_OAUTH_TOKEN_ENDPOINT, settings_.clientId(), settings_.clientSecret(), settings_.accessToken())
   , currentReply_(nullptr)
   , currentEndpoint_(nullptr)
{
   qDebug() << "creating makeleaps";
   connect(&http_, QNetworkAccessManager::finished, this, MakeLeaps::onReplyFinished);
   connect(&http_, QNetworkAccessManager::authenticationRequired, &oauth_, OAuth2WithClientCredentialsGrant::onAuthenticationRequired);
   connect(&oauth_, OAuth2WithClientCredentialsGrant::stateChanged, this, MakeLeaps::onAuthStateChanged);
}

MakeLeaps::ConnectionState MakeLeaps::state() const
{
    return state_;
}

OAuth2Settings* MakeLeaps::settings()
{
    return &settings_;
}

MakeLeapsEndpoint* MakeLeaps::endpoint()
{
   return currentEndpoint_;
}

void MakeLeaps::reloadAccessToken()
{
   oauth_.reloadAccessToken();
}

void MakeLeaps::load()
{
    if (STATE_LOADING == state_) return;

    qDebug() << "loading makeleaps...";
    setState(STATE_LOADING);

    currentReply_ = loadEntity("Invoices");
}

void MakeLeaps::abort()
{
    if (STATE_LOADING != state_) return;

    qDebug() << "aborting...";
    setState(STATE_ABORTING);
    currentReply_->abort();
}

QNetworkReply* MakeLeaps::loadEntity(const QString& entity, const QString& entityId)
{
   (void)entity;
   (void)entityId;

   auto url = QUrl { QString(MAKELEAPS_API_BASE) };

   auto request = QNetworkRequest { url };
   request.setRawHeader( "Accept", "application/json" );

   oauth_.authorize( request, QNetworkAccessManager::GetOperation, {} );

   qDebug() << "making request: " << url.toString().toLatin1();
   return http_.get( request );
}

void MakeLeaps::setState(ConnectionState state)
{
    state_ = state;
    emit stateChanged();
}

void MakeLeaps::onAuthStateChanged()
{
   switch ( oauth_.state() )
   {
   case OAuth2WithClientCredentialsGrant::STATE_AUTHENTICATING:
      setState( STATE_AUTHENTICATING );
      break;
   case OAuth2WithClientCredentialsGrant::STATE_AUTHENTICATED:
      qDebug() << "OAuth authenticated: " << oauth_.accessToken();
      settings_.setAccessToken( oauth_.accessToken() );
      setState( STATE_IDLE );
      break;
   case OAuth2WithClientCredentialsGrant::STATE_ERROR:
      qWarning() << "OAuth error";
      setState( STATE_ERROR );
      break;
   default:
      qWarning() << "unrecognised OAuth state";
   }
}

namespace
{

bool HeaderContainsModifyingOperations(const QByteArray& header)
{
   auto tokens = header.split(',');
   for (auto& token: tokens)
   {
      auto value = QString(token).trimmed().toUpper();
      if ( value == "POST" ||
           value == "PATCH" ||
           value == "PUT" ||
           value == "DELETE" )
      {
         return true;
      }
   }
   return false;
}

bool AllowHeaderIsModifiable(QNetworkReply* reply)
{
   return HeaderContainsModifyingOperations(reply->rawHeader("Allow"));
}

}

void MakeLeaps::onReplyFinished(QNetworkReply* reply)
{
   reply->deleteLater();
   currentReply_ = nullptr;

   qDebug().nospace() << "got response (" << (reply->isFinished() ? "" : "not ") << "finished)";
   switch ( reply->error() )
   {
   case QNetworkReply::NoError:
      break;
   case QNetworkReply::AuthenticationRequiredError:
      qDebug() << "Authentication error - re-authenticating";
      oauth_.reloadAccessToken();
      return;
   default:
      qDebug() << "Network error: " << reply->errorString();
      setState( STATE_ERROR );
      return;
   }

   setState( STATE_IDLE );

   auto data = reply->readAll();
   auto json = QJsonDocument::fromJson(data);

   qDebug("makeleaps response:\n%s", json.toJson(QJsonDocument::Indented).toStdString().c_str());

   if ( currentEndpoint_ ) currentEndpoint_->deleteLater();
   currentEndpoint_ = new MakeLeapsEndpoint(reply->request().url(), AllowHeaderIsModifiable(reply), this);

   auto entityWrapper = json.object();
   if ( entityWrapper["response"].isArray() )
   {
      QList<QObject*> resources;
      for (auto resource: entityWrapper["response"].toArray())
      {
         resources.append(new MakeLeapsResource(resource.toObject(), currentEndpoint_));
      }
      currentEndpoint_->setResources(resources);
   }
   else
   {
      currentEndpoint_->setResource(new MakeLeapsResource(entityWrapper["response"].toObject(), currentEndpoint_));
   }

   emit endpointChanged();
}
