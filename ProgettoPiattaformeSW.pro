#-------------------------------------------------
#
# Project created by QtCreator 2013-08-20T10:49:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


isEmpty(PREFIX) {
 PREFIX = /usr/bin
}
target = ProgettoPiattaformeSW
target.path = $$PREFIX/

TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagefilter.cpp \
    openfiledialog.cpp

HEADERS  += mainwindow.h \
    openfiledialog.h

FORMS    += mainwindow.ui \
    openfiledialog.ui

INSTALLS += target

