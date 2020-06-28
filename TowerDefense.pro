
QT       += core gui multimedia     # meltimedia用于播放声音

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TowerDefense
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    towerposition.cpp \
    tower.cpp \
    waypoint.cpp \
    enemy.cpp \
    bullet.cpp \
    audioplayer.cpp \
    plistreader.cpp \
    welcomewindow.cpp \
    winwindow.cpp

HEADERS  += mainwindow.h \
    towerposition.h \
    tower.h \
    waypoint.h \
    utility.h \
    enemy.h \
    bullet.h \
    audioplayer.h \
    plistreader.h \
    welcomewindow.h \
    winwindow.h

FORMS    += mainwindow.ui \
    welcomewindow.ui \
    winwindow.ui

RESOURCES += \
    resource.qrc
