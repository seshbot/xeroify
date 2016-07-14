#ifndef MAKELEAPS_H
#define MAKELEAPS_H

#include <QObject>

#include "OAuth2WithClientCredentialsGrant.h"
#include "connectionsettings.h"


class MakeLeaps : public QObject
{
   Q_OBJECT
   Q_PROPERTY(ConnectionState state READ state NOTIFY stateChanged)
   Q_PROPERTY(OAuth2Settings* settings READ settings)

public:
   enum ConnectionState
   {
       STATE_IDLE,
       STATE_AUTHENTICATING,
       STATE_LOADING,
       STATE_ABORTING,
       STATE_ERROR,
   };
   Q_ENUMS( ConnectionState )

   explicit MakeLeaps(QObject *parent = 0);

   ConnectionState state() const;
   OAuth2Settings* settings();

signals:
   void stateChanged();
   void invoicesLoaded();

public slots:
   void reloadAccessToken();
   void load();
   void abort();

private slots:
   void onAuthStateChanged();
   void onReplyFinished(QNetworkReply* reply);

private:
   void setState(ConnectionState state);
   QNetworkReply* loadEntity(const QString& entity, const QString& entityId = "");

   ConnectionState state_;
   QNetworkAccessManager http_;
   OAuth2Settings settings_;
   OAuth2WithClientCredentialsGrant oauth_;
   QNetworkReply* currentReply_;
};

#endif // MAKELEAPS_H
