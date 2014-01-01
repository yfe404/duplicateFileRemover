QT       += core

QT       += gui
QT       += sql # On rajoute SQL
TARGET = test_database
CONFIG   += app_bundle

# Permet d'afficher les messages de debug sur une console lors de l'exécution du programme en mode graphique.
CONFIG   += console debug


TEMPLATE = app


# Génération d'un fichier de traduction pour la langue anglaise
TRANSLATIONS += test_database_en.ts


SOURCES += main.cpp \
    database.cpp \
    extended_filesystem.cpp

HEADERS += \
    database.h \
    extended_filesystem.h

# Ajout de la bibliothèque boost
INCLUDEPATH+=/usr/local/boost_1_55_0
LIBS+=-L/usr/local/boost_1_55_0/stage/lib -lboost_system -lboost_filesystem


# INCLUDEPATH+=/home/kramel/boostInstall/boost_1_55_0
