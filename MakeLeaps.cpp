#include "MakeLeaps.h"

#include <QNetworkReply>
#include <QUrl>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QAuthenticator>

namespace
{

auto MAKELEAPS_OAUTH_TOKEN_ENDPOINT = "https://api.makeleaps.com/user/oauth2/token/";
auto MAKELEAPS_API_BASE = "https://api.makeleaps.com/api/";

bool IsResource( const QJsonValue& v ) {
   if (!v.isObject()) return false;

   auto object = v.toObject();
   if ( !object.contains("url") || !object.value("url").toString().startsWith(MAKELEAPS_API_BASE) )
   {
      return false;
   }

   return true;
}

QString StringValue(const QJsonValue& value )
{
   switch (value.type())
   {
   case QJsonValue::Null: return "<null>";
   case QJsonValue::Bool: return value.toBool() ? "true" : "false";
   case QJsonValue::Double: return QString::number(value.toDouble());
   case QJsonValue::String: return value.toString();
   case QJsonValue::Array: return "<array>";
   case QJsonValue::Object: return "<object>";
   case QJsonValue::Undefined: return "<undefined>";
   default: return "???";
   }
}

}


//
// MakeLeapsResourceProperty
//

ApiProperty::ApiProperty(QObject* parent)
   : QObject(parent), api_(nullptr), type_ { TYPE_NULL }, object_(nullptr), isEndpoint_(false)
{

}

ApiProperty::ApiProperty(MakeLeaps& api, const QString& name, const QJsonValue& value, QObject* parent)
   : QObject(parent), api_(&api), type_ { TYPE_NULL }, object_(nullptr), value_(value), name_(name), isEndpoint_(false)
{
   // "https://api.makeleaps.com/api/..." is an endpoint
   if (value_.type() == QJsonValue::String && value_.toString().startsWith(MAKELEAPS_API_BASE))
   {
      isEndpoint_ = true;
   }

   // [ { }, ... ]
   if (value_.type() == QJsonValue::Array)
   {
      type_ = TYPE_ARRAY;

      auto array = value_.toArray();
      for (auto v: array)
      {
         objects_.append(new ApiProperty(*api_, "", v, this));
      }
   }
   // { ... }
   else if (value_.isObject())
   {
      type_ = IsResource(value_) ? TYPE_RESOURCE : TYPE_OBJECT;
      auto object = value_.toObject();
      object_ = new ApiObject(*api_, object, this);
      if (name_.isEmpty() && object.contains("name")) name_ = object["name"].toString();
      if (name_.isEmpty() && object.contains("display_name")) name_ = object["display_name"].toString();
   }
   // ""
   else if (value_.isNull() || value_.isUndefined())
   {
      type_ = TYPE_NULL;
   }
   else
   {
      type_ = TYPE_SCALAR;
   }

   stringValue_ = StringValue( value_ );
}

MakeLeapsEndpoint* ApiProperty::asEndpoint()
{
   return new MakeLeapsEndpoint(*api_, value_.toString(), this);
}

ApiObject::ApiObject(MakeLeaps& api, const QJsonObject& object, QObject* parent)
   : QObject(parent)
   , api_(&api)
   , object_(object)
   , isResource_(IsResource(object))
{
   for (auto key: object_.keys())
   {
      properties_.append( new ApiProperty( *api_, key, object_.value(key), this ) );
   }
}


//
// MakeLeaps
//

MakeLeaps::MakeLeaps(QObject *parent)
   : QObject(parent)
   , state_(STATE_IDLE)
   , oauth_(MAKELEAPS_OAUTH_TOKEN_ENDPOINT, settings_.clientId(), settings_.clientSecret(), settings_.accessToken())
   , rootEndpoint_(*this, QUrl(MAKELEAPS_API_BASE), this)
{
   qDebug() << "creating makeleaps";
   connect(&http_, &QNetworkAccessManager::authenticationRequired, &oauth_, &OAuth2WithClientCredentialsGrant::onAuthenticationRequired);
   connect(&oauth_, &OAuth2WithClientCredentialsGrant::stateChanged, this, &MakeLeaps::onAuthStateChanged);
   connect(&rootEndpoint_, &MakeLeapsEndpoint::stateChanged, this, &MakeLeaps::onRootEndpointStateChanged);
}

MakeLeaps::ConnectionState MakeLeaps::state() const
{
    return state_;
}

OAuth2Settings* MakeLeaps::settings()
{
    return &settings_;
}

MakeLeapsEndpoint* MakeLeaps::apiRoot()
{
   return &rootEndpoint_;
}

void MakeLeaps::reloadAccessToken()
{
   oauth_.setClientId(settings_.clientId());
   oauth_.setClientSecret(settings_.clientSecret());
   oauth_.reloadAccessToken();
}

void MakeLeaps::load()
{
    rootEndpoint_.getResource();
}

void MakeLeaps::abort()
{
   rootEndpoint_.abort();
}

QNetworkReply* MakeLeaps::getResource(const QUrl& url)
{
   auto request = QNetworkRequest { url };
   request.setRawHeader( "Accept", "application/json" );

   oauth_.authorize( request, QNetworkAccessManager::GetOperation, {} );

   qDebug() << "making GET request: " << url.toString().toLatin1();
   return http_.get( request );
}

QNetworkReply* MakeLeaps::deleteResource(const QUrl& url)
{
   auto request = QNetworkRequest { url };
   request.setRawHeader( "Accept", "application/json" );

   oauth_.authorize( request, QNetworkAccessManager::DeleteOperation, {} );

   qDebug() << "making DELETE request: " << url.toString().toLatin1();
   return http_.deleteResource( request );
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

      if ( rootEndpoint_.state() == MakeLeapsEndpoint::STATE_NEEDS_AUTHENTICATION )
      {
         qDebug() << "Reloading root endpoint";
         rootEndpoint_.getResource();
      }

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

void MakeLeaps::onRootEndpointStateChanged()
{
   if ( rootEndpoint_.state() == MakeLeapsEndpoint::STATE_ERROR )
   {
      setState(MakeLeaps::STATE_ERROR);
   }

   if ( rootEndpoint_.state() == MakeLeapsEndpoint::STATE_LOADED )
   {
      partners_.clear();

      auto* response = rootEndpoint_.rootProperty()->asObject();
      auto* partners = response->property("partners");
      for ( auto* p: partners->asArray() )
      {
         auto* partner = qobject_cast< ApiProperty* >( p );
         auto* partnerEndpoint = partner->asObject()->property("url")->asEndpoint();
         partners_.append( new MakeLeapsPartner( partner->name(), partnerEndpoint, this ) );
      }

      emit partnersChanged();
      setState(MakeLeaps::STATE_IDLE);
   }

   if ( rootEndpoint_.state() == MakeLeapsEndpoint::STATE_NEEDS_AUTHENTICATION )
   {
      oauth_.reloadAccessToken();
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

void MakeLeapsEndpoint::getResource()
{
   qDebug() << "getting endpoint" << url_;
   currentReply_ = api_->getResource(url_);
   connect(currentReply_, &QNetworkReply::finished, this, &MakeLeapsEndpoint::onReplyFinished);
   currentReply_->setParent(this);

   setState(STATE_LOADING);
}

void MakeLeapsEndpoint::deleteResource()
{
   currentReply_ = api_->deleteResource(url_);
   connect(currentReply_, &QNetworkReply::finished, this, &MakeLeapsEndpoint::onReplyFinished);
   currentReply_->setParent(this);

   setState(STATE_LOADING);
}

void MakeLeapsEndpoint::abort()
{
   setState(STATE_ABORTING);
   currentReply_->abort();
}

void MakeLeapsEndpoint::onReplyFinished()
{
   qDebug().nospace() << "got response (" << (currentReply_->isFinished() ? "" : "not ") << "finished)";
   switch ( currentReply_->error() )
   {
   case QNetworkReply::NoError:
      break;
   case QNetworkReply::AuthenticationRequiredError:
      qDebug() << "Authentication error - needs reauth";
      lastError_ = "Authentication failed";
      emit lastErrorMessageChanged();
      setState( STATE_NEEDS_AUTHENTICATION );
      return;
   default:
      qDebug() << "Network error: " << currentReply_->errorString();
      lastError_ = currentReply_->errorString();
      emit lastErrorMessageChanged();
      setState( STATE_ERROR );
      return;
   }

   auto data = currentReply_->readAll();
   auto json = QJsonDocument::fromJson(data);

   qDebug("makeleaps response:\n%s", json.toJson(QJsonDocument::Indented).toStdString().c_str());

   auto isModifyable = AllowHeaderIsModifiable(currentReply_);

   if ( isModifyable != isModifyable_ )
   {
      isModifyable_ = isModifyable;
      emit isModifyableChanged();
   }

   auto entityWrapper = json.object();
   setRootProperty( new ApiProperty(*api_, "response", entityWrapper["response"], this) );
}
