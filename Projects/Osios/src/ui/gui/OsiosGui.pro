#-------------------------------------------------
#
# Project created by QtCreator 2014-06-25T01:21:31
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = OsiosGui
TEMPLATE = app


SOURCES += main.cpp \
    osiosmainwindow.cpp \
    messages/osiosmessagescontactswidget.cpp \
    messages/osiosmessageconversationwidget.cpp \
    webbrowser/osioswebbrowserwidget.cpp \
    office/osiosofficenotepadwidget.cpp \
    filebrowser/osiosfilebrowserwidget.cpp \
    todolist/todolistwidget.cpp

HEADERS  += \
    osiosmainwindow.h \
    messages/osiosmessagescontactswidget.h \
    messages/osiosmessageconversationwidget.h \
    webbrowser/osioswebbrowserwidget.h \
    office/osiosofficenotepadwidget.h \
    filebrowser/osiosfilebrowserwidget.h \
    todolist/todolistwidget.h
