QT       += core

QT       -= gui
QT       += sql # On rajoute SQL
TARGET = test_database
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app



SOURCES += main.cpp \
    database.cpp \
<<<<<<< HEAD
    extended_filesystem.cpp
=======
    vdisplayer.cpp \
    vmd5.cpp \
    vadddatabase.cpp \
    path.cpp
>>>>>>> e90bf016f1fb9ae21fe2382b2acc7405e2773265

HEADERS += \
    database.h \
    extended_filesystem.h

# INCLUDEPATH+=/usr/local/boost_1_55_0
# LIBS+=-L/usr/local/boost_1_55_0/stage/lib -lboost_system -lboost_filesystem

<<<<<<< HEAD
 INCLUDEPATH+=/usr/local/openssl-1.0.1e/include
 LIBS+=-L/usr/local/openssl-1.0.1e/ -lssl -lcrypto

# INCLUDEPATH+=/home/kramel/boostInstall/boost_1_55_0
=======
# Aymeric
INCLUDEPATH+=/usr/local/boost_1_55_0
INCLUDEPATH+=/usr/local/include/boost
INCLUDEPATH+=/usr/local/boost_1_55_0/boost
LIBS+=-L/usr/local/boost_1_55_0/stage/lib -lboost_system -lboost_filesystem
>>>>>>> e90bf016f1fb9ae21fe2382b2acc7405e2773265
