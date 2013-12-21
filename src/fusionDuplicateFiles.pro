QT       += core

QT       -= gui
QT       += sql # On rajoute SQL
TARGET = test_database
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    database.cpp \
    vdisplayer.cpp \
    vmd5.cpp \
    vadddatabase.cpp

HEADERS += \
    database.h \
    path.h \
    visitor.h \
    vdisplayer.h \
    vmd5.h \
    vadddatabase.h

 INCLUDEPATH+=/usr/local/boost_1_55_0
 LIBS+=-L/usr/local/boost_1_55_0/stage/lib -lboost_system -lboost_filesystem

# INCLUDEPATH+=/home/kramel/boostInstall/boost_1_55_0
