QT += qml quick

CONFIG += c++11

SOURCES += main.cpp \
    ConnectionSettings.cpp \
    Shopify.cpp \
    Xero.cpp \
    Http.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ConnectionSettings.h \
    Shopify.h \
    Xero.h \
    Http.h
