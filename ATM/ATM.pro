#-------------------------------------------------
#
# Project created by QtCreator 2013-11-05T01:20:12
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ATM
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ATM.cpp

HEADERS  += mainwindow.h \
    ATM.h

FORMS    += mainwindow.ui

# Allow C++11
QMAKE_CXXFLAGS += -std=c++11

# Copy database file to target directory after linking
win32 {
    DB_SRC_LOCATION = $$replace(PWD,/,\\)
    DB_DST_LOCATION = $$replace(OUT_PWD,/,\\)
    QMAKE_POST_LINK += copy $$DB_SRC_LOCATION\\bank.db $$DB_DST_LOCATION\\
}
unix {
    QMAKE_POST_LINK += cp $$PWD/bank.db $$OUT_PWD/
}
