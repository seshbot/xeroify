#include "OAuth2WithClientCredentialsGrant.h"

#include <QDebug>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


OAuth2WithClientCredentialsGrant::OAuth2WithClientCredentialsGrant(QObject* parent)
   : OAuth2WithClientCredentialsGrant( "", "", "", "", parent )
{
}

OAuth2WithClientCredentialsGrant::OAuth2WithClientCredentialsGrant(const QString& tokenEndpoint,
                                                                   const QString& clientId,
                                                                   const QString& clientSecret,
                                                                   const QString& accessToken,
                                                                   QObject* parent)
   : QObject( parent )
   , state_( STATE_ERROR )
   , tokenEndpoint_( tokenEndpoint )
   , clientId_( clientId )
   , clientSecret_( clientSecret )
   , accessToken_( accessToken )
{
   connect(&http_, &QNetworkAccessManager::finished, this, &OAuth2WithClientCredentialsGrant::onAccessTokenRequestFinished);
   connect(&http_, &QNetworkAccessManager::authenticationRequired, this, &OAuth2WithClientCredentialsGrant::onAuthenticationRequired);
   if (accessToken_.isEmpty() && !clientId_.isEmpty() && !clientSecret_.isEmpty())
   {
      reloadAccessToken();
   }
}

OAuth2WithClientCredentialsGrant::ConnectionState OAuth2WithClientCredentialsGrant::state() const
{
    return state_;
}

void OAuth2WithClientCredentialsGrant::setClientId(const QString& clientId)
{
   clientId_ = clientId;
}

void OAuth2WithClientCredentialsGrant::setClientSecret(const QString& clientSecret)
{
   clientSecret_ = clientSecret;
}

QString OAuth2WithClientCredentialsGrant::tokenEndpoint() const
{
   return tokenEndpoint_;
}

void OAuth2WithClientCredentialsGrant::setTokenEndpoint(const QString& tokenEndpoint)
{
   tokenEndpoint_ = tokenEndpoint;
}

QString OAuth2WithClientCredentialsGrant::accessToken() const
{
   return accessToken_;
}

void OAuth2WithClientCredentialsGrant::setAccessToken(const QString& accessToken)
{
   accessToken_ = accessToken;
}

QNetworkRequest& OAuth2WithClientCredentialsGrant::authorize(
      QNetworkRequest& request,
      QNetworkAccessManager::Operation op,
      const QList<QPair<QString, QString>>& params)
{
   (void)op;
   (void)params;
   auto authorizeHeader = ("Bearer " + accessToken_).toLatin1();
   qDebug().nospace() << "setting Authorize=" << authorizeHeader;
   request.setRawHeader("Authorization", authorizeHeader);
   return request;
}

void OAuth2WithClientCredentialsGrant::onAccessTokenRequestFinished(QNetworkReply* reply)
{
   reply->deleteLater();

   qDebug().nospace() << "got auth token response (" << (reply->isFinished() ? "" : "not ") << "finished)";
   if (reply->error() != QNetworkReply::NoError)
   {
       qDebug() << "Network error: " << reply->errorString();
       setState(STATE_ERROR);
       return;
   }

   auto data = reply->readAll();
   auto json = QJsonDocument::fromJson(data);

   auto entityWrapper = json.object();
   accessToken_ = entityWrapper["access_token"].toString();

   qDebug().nospace() << "makeleaps response (" << accessToken_ << ")";

   setState(STATE_AUTHENTICATED);
}

void OAuth2WithClientCredentialsGrant::setState(ConnectionState state)
{
    state_ = state;
    emit stateChanged();
}

void OAuth2WithClientCredentialsGrant::reloadAccessToken()
{
   auto query = QUrlQuery(QUrl(tokenEndpoint_));
   query.addQueryItem("grant_type", "client_credentials");

   auto fullUrl = QUrl(tokenEndpoint_);
   fullUrl.setQuery(query);
   fullUrl.setUserName(clientId_);
   fullUrl.setPassword(clientSecret_);

   auto accessTokenRequest = QNetworkRequest(fullUrl);
   auto authorization = QByteArray(QString("%1:%2").arg(clientId_).arg(clientSecret_).toLatin1()).toBase64();
   accessTokenRequest.setRawHeader("Authorization", "Basic " + authorization);
   accessTokenRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

   qDebug() << "making request: " << accessTokenRequest.url().toString();

   http_.post(accessTokenRequest, QByteArray());

   setState(STATE_AUTHENTICATING);
}

void OAuth2WithClientCredentialsGrant::onAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{
   qDebug() << "setting authentication details";

   (void)reply;
   authenticator->setUser( clientId_ );
   authenticator->setPassword( clientSecret_ );
}
