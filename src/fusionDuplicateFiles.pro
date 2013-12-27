QT       += core

QT       -= gui
QT       += sql # On rajoute SQL
TARGET = test_database
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app



SOURCES += main.cpp \
    database.cpp \
    extended_filesystem.cpp

HEADERS += \
    database.h \
    extended_filesystem.h

 INCLUDEPATH+=/usr/local/boost_1_55_0
 LIBS+=-L/usr/local/boost_1_55_0/stage/lib -lboost_system -lboost_filesystem

 INCLUDEPATH+=/usr/local/openssl-1.0.1e/include
 LIBS+=-L/usr/local/openssl-1.0.1e/ -lssl -lcrypto

# INCLUDEPATH+=/home/kramel/boostInstall/boost_1_55_0
