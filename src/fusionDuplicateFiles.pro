QT       += core

QT       -= gui
QT       += sql # On rajoute SQL
TARGET = test_database
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# Génération d'un fichier de traduction pour la langue anglaise
TRANSLATIONS += test_database_en.ts

SOURCES += main.cpp \
    database.cpp \
    extended_filesystem.cpp

HEADERS += \
    database.h \
    extended_filesystem.h

 INCLUDEPATH+=/usr/local/boost_1_55_0
 LIBS+=-L/usr/local/boost_1_55_0/stage/lib -lboost_system -lboost_filesystem


# INCLUDEPATH+=/home/kramel/boostInstall/boost_1_55_0
