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

class MakeLeaps;
class ApiObject;
class MakeLeapsEndpoint;

class ApiProperty : public QObject
{
   Q_OBJECT
   Q_PROPERTY(bool isEndpoint READ isEndpoint CONSTANT)
   Q_PROPERTY(QString name READ name CONSTANT)
   Q_PROPERTY(Type type READ type CONSTANT)
   Q_PROPERTY(QString typeString READ typeString CONSTANT)
   Q_PROPERTY(ApiObject* asObject READ asObject CONSTANT)
   Q_PROPERTY(QString asScalar READ asScalar CONSTANT)
   Q_PROPERTY(QList<QObject*> asArray READ asArray CONSTANT)
   Q_PROPERTY(MakeLeapsEndpoint* asEndpoint READ asEndpoint CONSTANT)

public:
   enum Type
   {
      TYPE_NULL,
      TYPE_SCALAR,
      TYPE_OBJECT,
      TYPE_RESOURCE,
      TYPE_ARRAY,
   };
   Q_ENUMS(Type)

   ApiProperty(QObject* parent = 0);
   ApiProperty(MakeLeaps& api, const QString& name, const QJsonValue& value, QObject* parent = 0);

   MakeLeapsEndpoint* asEndpoint();

   bool isEndpoint() const { return isEndpoint_; }
   QString name() const { return name_; }

   Type type() const { return type_; }
   QString typeString() const
   {
      switch (type())
      {
      case TYPE_NULL: return "NULL";
      case TYPE_SCALAR: return "SCALAR";
      case TYPE_OBJECT: return "OBJECT";
      case TYPE_RESOURCE: return "RESOURCE";
      case TYPE_ARRAY: return "ARRAY";
      default: return "UNKNOWN";
      }
   }
   QString asScalar() { return stringValue_; }
   ApiObject* asObject() { return object_; }
   QList<QObject*> asArray() { return objects_; }

private:
   MakeLeaps* api_;
   Type type_;
   ApiObject* object_;
   QList<QObject*> objects_;

   QJsonValue value_;
   QString stringValue_;

   QString name_;
   bool isEndpoint_;
};

// resource has child nodes that are either URLs or arrays?
class ApiObject : public QObject
{
   Q_OBJECT
   Q_PROPERTY(bool isResource READ isResource CONSTANT)
   Q_PROPERTY(QString name READ name CONSTANT)
   Q_PROPERTY(QString url READ url CONSTANT)
   Q_PROPERTY(QString jsonString READ jsonString CONSTANT)
   Q_PROPERTY(QStringList keys READ keys CONSTANT)
   Q_PROPERTY(QList<QObject*> properties READ properties CONSTANT)

public:
   explicit ApiObject(MakeLeaps& api, const QJsonObject& object, QObject* parent = 0);

   bool isResource() const { return isResource_; }
   QString name() const
   {
      if (object_.contains("name")) return object_["name"].toString();
      if (object_.contains("display_name")) return object_["display_name"].toString();
      return "-";
   }
   QString url() const { return object_["url"].toString(); }
   QString jsonString() const { return QJsonDocument(object_).toJson(QJsonDocument::Indented); }
   QStringList keys() const { return object_.keys(); }

   QList<QObject*> properties() { return properties_; }

private:
   MakeLeaps* api_;
   QJsonObject object_;
   bool isResource_;
   QList<QObject*> properties_;
};

inline QList<QObject*> JsonValue::arrayValue()
{
   QList<QObject*> result;
   for ( auto value: value_.toArray() )
   {
      result.append( new JsonValue(value, this) );
   }
   return result;
}


class MakeLeapsEndpoint : public QObject
{
   Q_OBJECT
   Q_PROPERTY(MakeLeaps* api READ api CONSTANT)
   Q_PROPERTY(State state READ state NOTIFY stateChanged)
   Q_PROPERTY(bool isModifyable READ isModifyable NOTIFY isModifyableChanged)
   Q_PROPERTY(QString url READ urlString CONSTANT)
   Q_PROPERTY(ApiProperty* rootProperty READ rootProperty NOTIFY rootPropertyChanged)
   Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY lastErrorMessageChanged)

public:
   enum State
   {
      STATE_INVALID,
      STATE_LOADING,
      STATE_ABORTING,
      STATE_LOADED,
      STATE_NEEDS_AUTHENTICATION,
      STATE_ERROR,
   };
   Q_ENUMS(State)

   MakeLeapsEndpoint(QObject* parent = 0)
      : QObject(parent)
      , api_(nullptr)
      , state_(STATE_INVALID)
      , isModifyable_(false)
      , rootProperty_(nullptr)
      , currentReply_(nullptr)
   { }

   MakeLeapsEndpoint(MakeLeaps& api, const QUrl& url, bool isModifyable, QObject* parent = 0)
      : QObject(parent)
      , api_(&api)
      , state_(STATE_LOADING)
      , url_(url)
      , isModifyable_(isModifyable)
      , rootProperty_(nullptr)
      , currentReply_(nullptr)
   { }

   MakeLeaps* api() { return api_; }

   QUrl url() const { return url_; }

   QString urlString() const { return url_.url(); }

   bool isModifyable() const { return isModifyable_; }

   State state() const { return state_; }

   ApiProperty* rootProperty() const { return rootProperty_; }

   void setRootProperty(ApiProperty* property) { rootProperty_ = property; setState(STATE_LOADED); emit rootPropertyChanged(); }

   QString lastErrorMessage() const { return lastError_; }

signals:
   void stateChanged();
   void isModifyableChanged();
   void rootPropertyChanged();
   void lastErrorMessageChanged();

public slots:
   void load();
   void abort();

public slots:
   void onReplyFinished();

private:
   void setState(State state) { if (state_ != state) { state_ = state; emit stateChanged(); } }

   MakeLeaps* api_;
   State state_;
   QUrl url_;
   bool isModifyable_;
   ApiProperty* rootProperty_;
   QNetworkReply* currentReply_;
   QString lastError_;
};


class MakeLeaps : public QObject
{
   Q_OBJECT
   Q_PROPERTY(OAuth2Settings* settings READ settings)
   Q_PROPERTY(ConnectionState state READ state NOTIFY stateChanged)
   Q_PROPERTY(MakeLeapsEndpoint* apiRoot READ apiRoot NOTIFY rootChanged)

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
   MakeLeapsEndpoint* apiRoot();

   Q_INVOKABLE MakeLeapsEndpoint* createEndpoint(QString url) {
      return new MakeLeapsEndpoint( *this, QUrl(url), false, this );
   }

signals:
   void stateChanged();
   void rootChanged();

public slots:
   void reloadAccessToken();
   void load();
   void abort();

private slots:
   void onAuthStateChanged();
   void onRootEndpointStateChanged();

private:
   friend class MakeLeapsEndpoint;

   void setState(ConnectionState state);
   QNetworkReply* loadEndpointUrl(const QUrl& url);

   ConnectionState state_;
   QNetworkAccessManager http_;
   OAuth2Settings settings_;
   OAuth2WithClientCredentialsGrant oauth_;
   MakeLeapsEndpoint rootEndpoint_;
};


#endif // MAKELEAPS_H
