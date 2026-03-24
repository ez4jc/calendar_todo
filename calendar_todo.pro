QT += core gui widgets sql dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix:LIBS += -lX11

TARGET = calendar_todo
TEMPLATE = app

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    calendarview.cpp \
    calendarcell.cpp \
    tododialog.cpp \
    todomanager.cpp \
    databasemanager.cpp \
    systemtraymanager.cpp

HEADERS += \
    mainwindow.h \
    calendarview.h \
    calendarcell.h \
    tododialog.h \
    todoitem.h \
    todomanager.h \
    databasemanager.h \
    systemtraymanager.h

RESOURCES += \
    resources.qrc
