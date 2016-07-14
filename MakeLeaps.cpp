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

}

MakeLeaps::MakeLeaps(QObject *parent)
   : QObject(parent)
   , state_(STATE_IDLE)
   , oauth_(MAKELEAPS_OAUTH_TOKEN_ENDPOINT, settings_.clientId(), settings_.clientSecret(), settings_.accessToken())
   , currentReply_(nullptr)
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

void MakeLeaps::onReplyFinished(QNetworkReply* reply)
{
   reply->deleteLater();
   currentReply_ = nullptr;

   qDebug().nospace() << "got response (" << (reply->isFinished() ? "" : "not ") << "finished)";
   if ( reply->error() != QNetworkReply::NoError )
   {
       qDebug() << "Network error: " << reply->errorString();
       setState( STATE_ERROR );
       return;
   }

   setState( STATE_IDLE );

   auto data = reply->readAll();
   auto json = QJsonDocument::fromJson(data);

   auto entityWrapper = json.object();
   auto response = entityWrapper["response"].toObject();

   qDebug().nospace() << "makeleaps response (" << data << ")";

   emit invoicesLoaded();
}
