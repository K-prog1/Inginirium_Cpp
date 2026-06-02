QT       += core gui sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = phonebook
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    databasemanager.cpp \
    contactdialog.cpp

HEADERS += \
    mainwindow.h \
    databasemanager.h \
    contact.h \
    contactdialog.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
