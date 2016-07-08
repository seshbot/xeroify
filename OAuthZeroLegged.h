#ifndef OAUTHONELEGGED_H
#define OAUTHONELEGGED_H

#include <QString>
#include <QNetworkRequest>
#include <QNetworkAccessManager>


class OAuthZeroLegged
{
public:
   OAuthZeroLegged(const QString& consumerKey, const QString& rsaPrivateKey);

   void setConsumerKey(const QString& consumerKey);

   void setRsaPrivateKey(const QString& rsaPrivateKey);

   QNetworkRequest& authorize(QNetworkRequest& request,
                              QNetworkAccessManager::Operation op,
                              const QList<QPair<QString, QString>>& params = {});

private:
   QString consumerKey_;
   QString rsaPrivateKey_;
};


#endif // OAUTHONELEGGED_H
