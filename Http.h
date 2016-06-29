#ifndef HTTP_H
#define HTTP_H

#include <QObject>
#include <QNetworkAccessManager>

class Http : public QObject
{
   Q_OBJECT

public:
   explicit Http(QObject *parent = 0);

signals:

public slots:
   void handleAuthenticationRequired(QNetworkReply* reply, QAuthenticator*);
   void handleSslErrors(QNetworkReply* reply,const QList<QSslError> &errors);

private:
   QNetworkAccessManager http_;
};

#endif // HTTP_H
