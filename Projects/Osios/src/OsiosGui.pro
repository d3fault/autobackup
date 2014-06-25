#-------------------------------------------------
#
# Project created by QtCreator 2014-06-25T01:21:31
#
#-------------------------------------------------

QT       += core gui #ideally i'd be using 'serviceframework', but i suck at compiling qt from scratch (was bummed it didn't make essentials)...

lessThan(QT_MAJOR_VERSION, 5): QT += webkit
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = OsiosGui
TEMPLATE = app

SOURCES += ui/gui/main.cpp \
    ui/gui/osiosmainwindow.cpp \
    ui/gui/messages/osiosmessagescontactswidget.cpp \
    ui/gui/messages/osiosmessageconversationwidget.cpp \
    ui/gui/webbrowser/osioswebbrowserwidget.cpp \
    ui/gui/office/osiosofficenotepadwidget.cpp \
    ui/gui/filebrowser/osiosfilebrowserwidget.cpp \
    ui/gui/todolist/todolistwidget.cpp \
    osios.cpp \
    ui/gui/osiosgui.cpp \
    ui/iosiosui.cpp \
    ui/gui/iosiosguifactory.cpp \
    ui/osiosuiapp.cpp

HEADERS  += \
    ui/gui/osiosmainwindow.h \
    ui/gui/messages/osiosmessagescontactswidget.h \
    ui/gui/messages/osiosmessageconversationwidget.h \
    ui/gui/webbrowser/osioswebbrowserwidget.h \
    ui/gui/office/osiosofficenotepadwidget.h \
    ui/gui/filebrowser/osiosfilebrowserwidget.h \
    ui/gui/todolist/todolistwidget.h \
    osios.h \
    ui/gui/osiosgui.h \
    ui/iosiosui.h \
    ui/iosiosuifactory.h \
    ui/gui/iosiosguifactory.h \
    ui/osiosuiapp.h

include(../../../GloballySharedClasses/objectonthreadgroup.pri)
