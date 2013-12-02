QT       += core

QT       -= gui
QT       += sql # On rajoute SQL
TARGET = test_database
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    database.cpp

HEADERS += \
    database.h
