#ifndef OAUTH2WITHCLIENTCREDENTIALSGRANT_H
#define OAUTH2WITHCLIENTCREDENTIALSGRANT_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>


class OAuth2WithClientCredentialsGrant : public QObject
{
   Q_OBJECT
   Q_PROPERTY(ConnectionState state READ state NOTIFY stateChanged)
   Q_PROPERTY(QString tokenEndpoint READ tokenEndpoint WRITE setTokenEndpoint)
   Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken)

public:
   enum ConnectionState
   {
       STATE_AUTHENTICATING,
       STATE_AUTHENTICATED,
       STATE_ERROR,
   };
   Q_ENUMS( ConnectionState )

   OAuth2WithClientCredentialsGrant(QObject* parent = 0);

   OAuth2WithClientCredentialsGrant(const QString& tokenEndpoint,
                                    const QString& clientId,
                                    const QString& clientSecret,
                                    const QString& accessToken,
                                    QObject* parent = 0);

   ConnectionState state() const;

   QString tokenEndpoint() const;
   void setTokenEndpoint(const QString& tokenEndpoint);

   QString accessToken() const;
   void setAccessToken(const QString& accessToken);

   void setClientId(const QString& clientId);

   void setClientSecret(const QString& clientSecret);

   QNetworkRequest& authorize(QNetworkRequest& request,
                              QNetworkAccessManager::Operation op,
                              const QList<QPair<QString, QString>>& params = {});

signals:
   void stateChanged();

public slots:
   void reloadAccessToken();
   void onAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);

private slots:
   void onAccessTokenRequestFinished(QNetworkReply* reply);

private:
   void setState(ConnectionState state);

   ConnectionState state_;
   QNetworkAccessManager http_;
   QString tokenEndpoint_;
   QString clientId_;
   QString clientSecret_;
   QString accessToken_;
};


#endif // OAUTH2WITHCLIENTCREDENTIALSGRANT_H
