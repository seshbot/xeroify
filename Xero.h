#ifndef XERO_H
#define XERO_H

#include <QObject>

#include "ConnectionSettings.h"


class Xero : public QObject
{
   Q_OBJECT
   Q_PROPERTY(ConnectionSettings* settings READ settings)

public:
   explicit Xero(QObject *parent = 0);

   ConnectionSettings* settings();

signals:

public slots:

private:
   ConnectionSettings settings_;
};

#endif // XERO_H
