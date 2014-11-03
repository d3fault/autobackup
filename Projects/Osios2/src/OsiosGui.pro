#-------------------------------------------------
#
# Project created by QtCreator 2014-10-25T20:36:44
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Osios
TEMPLATE = app

DEFINES += OSIOS_DHT_CONFIG_NEIGHBOR_SENDS_BACK_RENDERING_WITH_CRYPTOGRAPHIC_VERIFICATION_OF_TIMELINE_NODE

SOURCES += main.cpp\
        osiosmainwindow.cpp \
    mainmenuitems/timelinetab_widget_formainmenutabwidget.cpp \
    mainmenuitems/writertab_widget_formainmenutabwidget.cpp \
    osiosgui.cpp \
    osios.cpp \
    itimelinenode.cpp \
    timelinenodetypes/mainmenuactivitychangedtimelinenode.cpp \
    iactivitytab_widget_formainmenutabwidget.cpp \
    osioscreateprofiledialog.cpp \
    timelinenodetypes/keypressinnewemptydoctimelinenode.cpp \
    mainmenuitems/writer/eventemittingplaintextedit.cpp \
    timelineserializer.cpp \
    osiosdhtpeer.cpp \
    osiosdht.cpp \
    osiosprofilemanagerdialog.cpp \
    osiosnotificationswidget.cpp \
    timelinenodetypes/profilecreationannounce_aka_genesistimelinenode.cpp \
    osiosdhtbootstrapsplashdialog.cpp \
    osiossettings.cpp

HEADERS  += osiosmainwindow.h \
    mainmenuitems/timelinetab_widget_formainmenutabwidget.h \
    mainmenuitems/writertab_widget_formainmenutabwidget.h \
    osiosgui.h \
    osios.h \
    itimelinenode.h \
    timelinenodetypes/mainmenuactivitychangedtimelinenode.h \
    osioscommon.h \
    iactivitytab_widget_formainmenutabwidget.h \
    osioscreateprofiledialog.h \
    timelinenodetypes/keypressinnewemptydoctimelinenode.h \
    iosiosclient.h \
    mainmenuitems/writer/eventemittingplaintextedit.h \
    timelineserializer.h \
    osiosdhtpeer.h \
    osiosdht.h \
    osiosprofilemanagerdialog.h \
    osiossettings.h \
    osiosnotificationswidget.h \
    iosioscopycatclient.h \
    timelinenodetypes/profilecreationannounce_aka_genesistimelinenode.h \
    osiosdhtbootstrapsplashdialog.h \
    ibootstrapsplashscreen.h \
    iosiosdhtbootstrapclient.h

include(../../../GloballySharedClasses/labellineeditbrowsebutton.pri)
