#ifndef XERO_H
#define XERO_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

#include "ConnectionSettings.h"
#include "OAuthZeroLegged.h"


class Xero : public QObject
{
   Q_OBJECT
   Q_PROPERTY(ConnectionState state READ state NOTIFY stateChanged)
   Q_PROPERTY(OAuthZeroLeggedConnectionSettings* settings READ settings)
   Q_PROPERTY(QList<QObject*> invoices READ invoices NOTIFY invoicesLoaded)

public:
   enum ConnectionState
   {
       STATE_IDLE,
       STATE_LOADING,
       STATE_ABORTING,
       STATE_ERROR,
   };
   Q_ENUMS( ConnectionState )

   explicit Xero(QObject *parent = 0);

   ConnectionState state() const;
   OAuthZeroLeggedConnectionSettings* settings();
   QList<QObject*> invoices() const;

signals:
   void stateChanged();
   void invoicesLoaded();

public slots:
   void load();
   void abort();

private slots:
   void onReplyFinished(QNetworkReply* reply);

private:
   void setState(ConnectionState state);
   QNetworkReply* loadEntity(const QString& entity, const QString& entityId = "");

   ConnectionState state_;
   QNetworkAccessManager http_;
   OAuthZeroLeggedConnectionSettings settings_;
   OAuthZeroLegged oauth_;
   QNetworkReply* currentReply_;
   QList<QObject*> invoices_;
};

#endif // XERO_H
