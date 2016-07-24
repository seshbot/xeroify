QT += qml quick

CONFIG += c++11

SOURCES += main.cpp \
    ConnectionSettings.cpp \
    Shopify.cpp \
    Xero.cpp \
    OAuthZeroLegged.cpp \
    XeroInvoice.cpp \
    MakeLeaps.cpp \
    OAuth2WithClientCredentialsGrant.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ConnectionSettings.h \
    Shopify.h \
    Xero.h \
    OAuthZeroLegged.h \
    XeroInvoice.h \
    MakeLeaps.h \
    OAuth2WithClientCredentialsGrant.h

win32 {
    # LIBS += -lcrypto -lssl -lubsec
    LIBS += -LC:/OpenSSL-Win32/lib -llibeay32 -lssleay32
    INCLUDEPATH += C:/OpenSSL-Win32/include
}
# could use macx, and unix:!macx
unix {
    # osx: brew install openssl
    LIBS += -L/usr/local/opt/openssl/lib -lcrypto -lssl
    INCLUDEPATH += /usr/local/opt/openssl/include
}
DISTFILES +=
