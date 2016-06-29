#include "Http.h"

#include <QNetworkReply>
#include <QAuthenticator>
#include <QDebug>

Http::Http(QObject *parent) : QObject(parent)
{
   QObject::connect(&http_, &QNetworkAccessManager::authenticationRequired, this, &Http::handleAuthenticationRequired);
   QObject::connect(&http_, &QNetworkAccessManager::sslErrors, this, &Http::handleSslErrors);
}

void Http::handleAuthenticationRequired(QNetworkReply* reply, QAuthenticator*)
{
    qDebug() << "auth required for " << reply->request().url();
}

void Http::handleSslErrors(QNetworkReply* reply,const QList<QSslError> &errors)
{
    qDebug() << "SSL errors for " << reply->request().url() << ": ";
    for ( auto& error: errors )
    {
        qDebug() << " - " << error.errorString();
    }
}
