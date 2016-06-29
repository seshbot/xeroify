#include "Xero.h"

#include <QtDebug>


Xero::Xero(QObject *parent)
   : QObject(parent)
   , settings_("xero")
{
   qDebug() << "creating shopify";
}

ConnectionSettings* Xero::settings()
{
    return &settings_;
}
