#ifndef MAKELEAPS_H
#define MAKELEAPS_H

#include <QObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "OAuth2WithClientCredentialsGrant.h"
#include "connectionsettings.h"


class JsonValue : public QObject
{
   Q_OBJECT
   Q_PROPERTY(Type type READ type CONSTANT)
   Q_PROPERTY(QString json READ json CONSTANT)
   Q_PROPERTY(QString typeString READ typeString CONSTANT)
   Q_PROPERTY(QString stringValue READ stringValue CONSTANT)
   Q_PROPERTY(QList<QObject*> arrayValue READ arrayValue CONSTANT)

public:
   enum Type
   {
      TYPE_NULL = QJsonValue::Null,
      TYPE_BOOL = QJsonValue::Bool,
      TYPE_DOUBLE = QJsonValue::Double,
      TYPE_STRING = QJsonValue::String,
      TYPE_ARRAY = QJsonValue::Array,
      TYPE_OBJECT = QJsonValue::Object,
      TYPE_UNDEFINED = QJsonValue::Undefined,
   };
   Q_ENUMS(Type)

   JsonValue(QJsonValueRef value, QObject* parent = 0)
      : QObject(parent)
      , value_(value)
   {
   }

   Type type() const { return static_cast<Type>( value_.type() ); }
   QString typeString() const
   {
      switch (type())
      {
      case TYPE_NULL: return "NULL";
      case TYPE_BOOL: return "BOOL";
      case TYPE_DOUBLE: return "DOUBLE";
      case TYPE_STRING: return "STRING";
      case TYPE_ARRAY: return "ARRAY";
      case TYPE_OBJECT: return "OBJECT";
      case TYPE_UNDEFINED: return "UNDEFINED";
      default: return "???";
      }
   }

   QString stringValue() const { return value_.toString(); }
   QList<QObject*> arrayValue();

   QString json() const {
      return QJsonDocument::fromVariant(value_.toVariant()).toJson(QJsonDocument::Indented);
   }

private:
   QJsonValue value_;
};


// Endoint 1
//
//{
//    "partners": [
//        {
//            "url": "https://app.makeleaps.com/api/partner/1725857643916378083/",
//            "name": "Kyoto Brewing Co., Ltd"
//        }
//    ],
//    "currencies": "https://app.makeleaps.com/api/currency/"
//}

// Endpoint 2
// {
//   "url": "https://app.makeleaps.com/api/partner/1725857643916378083/",
//   "name": "Kyoto Brewing Co., Ltd",
//   "clients": "https://app.makeleaps.com/api/partner/1725857643916378083/client/",
//   "contacts": "https://app.makeleaps.com/api/partner/1725857643916378083/contact/",
//   "documents": "https://app.makeleaps.com/api/partner/1725857643916378083/document/",
//   "tags": "https://app.makeleaps.com/api/partner/1725857643916378083/tag/"
// }


//
// endpoint could reference:
// - resource (client/12345)
// - collection (client/)
// -

class MakeLeapsResource;
class MakeLeapsEndpoint;

class MakeLeapsResourceProperty : public QObject
{
   Q_OBJECT
   Q_PROPERTY(QString name READ name CONSTANT)
   Q_PROPERTY(Type type READ type CONSTANT)
   Q_PROPERTY(QString typeString READ typeString CONSTANT)
   Q_PROPERTY(MakeLeapsResource* resource READ resource CONSTANT)
   Q_PROPERTY(QList<QObject*> resources READ resources CONSTANT)
   Q_PROPERTY(QList<QObject*> properties READ properties CONSTANT)
   Q_PROPERTY(QString value READ value CONSTANT)
   Q_PROPERTY(MakeLeapsEndpoint* endpoint READ endpoint CONSTANT)

public:
   enum Type
   {
      TYPE_VALUE,
      TYPE_VALUE_ARRAY,
      TYPE_RESOURCE,
      TYPE_RESOURCE_ARRAY,
      TYPE_ENDPOINT,
   };
   Q_ENUMS(Type)

   MakeLeapsResourceProperty(const QString& name, const QJsonValue& value, QObject* parent = 0);

   QString name() const { return name_; }
   Type type() const { return type_; }
   QString typeString() const
   {
      switch (type())
      {
      case TYPE_VALUE: return "VALUE";
      case TYPE_VALUE_ARRAY: return "VALUE_ARRAY";
      case TYPE_RESOURCE: return "RESOURCE";
      case TYPE_RESOURCE_ARRAY: return "RESOURCE ARRAY";
      case TYPE_ENDPOINT: return "ENDPOINT";
      default: return "UNKNOWN";
      }
   }
   MakeLeapsResource* resource() { return resource_; }
   QList<QObject*> resources() { return resources_; }
   MakeLeapsEndpoint* endpoint() { return endpoint_; }
   QString value() { return stringValue_; }
   QList<QObject*> properties() { return properties_; }

private:
   QString name_;
   Type type_;
   MakeLeapsResource* resource_;
   QList<QObject*> resources_;
   MakeLeapsEndpoint* endpoint_;

   QJsonValue value_;
   QString stringValue_;

   QList<QObject*> properties_;
};

// resource has child nodes that are either URLs or arrays?
class MakeLeapsResource : public QObject
{
   Q_OBJECT
   Q_PROPERTY(QString name READ name CONSTANT)
   Q_PROPERTY(QString url READ url CONSTANT)
   Q_PROPERTY(QString jsonString READ jsonString CONSTANT)
   Q_PROPERTY(QStringList keys READ keys CONSTANT)
   Q_PROPERTY(QList<QObject*> properties READ properties CONSTANT)

public:
   explicit MakeLeapsResource(const QJsonObject& object, QObject* parent = 0)
      : QObject(parent)
      , object_(object)
   {
      for (auto key: object_.keys())
      {
         properties_.append( new MakeLeapsResourceProperty( key, object_[key], this ) );
      }
   }

   QString name() const { return object_["name"].toString(); }
   QString url() const { return object_["url"].toString(); }
   QString jsonString() const { return QJsonDocument(object_).toJson(QJsonDocument::Indented); }
   QStringList keys() const { return object_.keys(); }

   QList<QObject*> properties() { return properties_; }
   JsonValue* operator[](const QString& key) { return new JsonValue( object_[key], this ); }

private:
   QJsonObject object_;
   QList<QObject*> properties_;
};

inline QList<QObject*> JsonValue::arrayValue()
{
   QList<QObject*> result;
   for ( auto value: value_.toArray() )
   {
      qDebug() << "value " << value.type();
      result.append( new JsonValue(value, this) );
   }
   return result;
}


class MakeLeapsEndpoint : public QObject
{
   Q_OBJECT
   Q_PROPERTY(State state READ state NOTIFY stateChanged)
   Q_PROPERTY(bool isModifyable READ isModifyable CONSTANT)
   Q_PROPERTY(QString url READ urlString CONSTANT)
   Q_PROPERTY(MakeLeapsResource* resource READ resource CONSTANT)
   Q_PROPERTY(QList<QObject*> resources READ resources CONSTANT)

public:
   enum State
   {
      STATE_INVALID,
      STATE_LOADING,
      STATE_LOADED,
      STATE_ERROR,
   };
   Q_ENUMS(State)

   MakeLeapsEndpoint(QObject* parent = 0)
      : QObject(parent)
      , state_(STATE_INVALID)
      , isModifyable_(false)
      , resource_(nullptr)
   { }

   MakeLeapsEndpoint(const QUrl& url, bool isModifyable, QObject* parent = 0)
      : QObject(parent)
      , state_(STATE_LOADING)
      , url_(url)
      , isModifyable_(isModifyable)
      , resource_(nullptr)
   { }

   QUrl url() const { return url_; }

   QString urlString() const { return url_.url(); }

   bool isModifyable() const { return isModifyable_; }

   State state() const { return state_; }

   MakeLeapsResource* resource() const { return resource_; }

   QList<QObject*> resources() const { return resources_; }

   void setResource(MakeLeapsResource* resource) { resource_ = resource; setState(STATE_LOADED); }
   void setResources(QList<QObject*> resources) { resources_ = resources; setState(STATE_LOADED); }

signals:
   void stateChanged();

private:
   void setState(State state) { if (state_ != state) { state_ = state; emit stateChanged(); } }

   // JsonValue object_; // impl
   State state_;
   QUrl url_;
   bool isModifyable_;
   MakeLeapsResource* resource_;
   QList<QObject*> resources_;

};


class MakeLeaps : public QObject
{
   Q_OBJECT
   Q_PROPERTY(OAuth2Settings* settings READ settings)
   Q_PROPERTY(ConnectionState state READ state NOTIFY stateChanged)
   Q_PROPERTY(MakeLeapsEndpoint* endpoint READ endpoint NOTIFY endpointChanged)

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
   MakeLeapsEndpoint* endpoint();

signals:
   void stateChanged();
   void endpointChanged();

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
   MakeLeapsEndpoint* currentEndpoint_;
};


#endif // MAKELEAPS_H
