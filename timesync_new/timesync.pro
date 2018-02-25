TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread

SOURCES += main.cpp \
    sitesyncsync.cpp \
    statemachinebase.cpp \
    portsyncsyncreceive.cpp \
    clockmastersyncsend.cpp \
    clockmastersyncoffset.cpp \
    clockmastersyncreceive.cpp \
    portsyncsyncsend.cpp \
    clockslavesync.cpp \
    ptpmessage/ptpmessagebase.cpp \
    ptpmessage/ptpmessagefollowup.cpp \
    ptpmessage/ptpmessagepdelayreq.cpp \
    ptpmessage/ptpmessagepdelayresp.cpp \
    ptpmessage/ptpmessagepdelayrespfollowup.cpp \
    ptpmessage/ptpmessagesync.cpp \
    ptpclock.cpp \
    mdsyncreceivesm.cpp \
    mdsyncsendsm.cpp \
    c-platform/linux/platform_linux.cpp \
    c-extensions/ThreadWrapper.cpp \
    statemachinemanager.cpp \
    mdpdelayreq.cpp \
    mdpdelayresp.cpp \
    timeawaresystem.cpp

HEADERS += \
    types.h \
    sitesyncsync.h \
    commstructs.h \
    statemachinebase.h \
    portsyncsyncreceive.h \
    clockmastersyncsend.h \
    clockmastersyncoffset.h \
    clockmastersyncreceive.h \
    portsyncsyncsend.h \
    clockslavesync.h \
    interfaces.h \
    ptpmessage/ptpmessagebase.h \
    ptpmessage/ptpmessagefollowup.h \
    ptpmessage/ptpmessagepdelayreq.h \
    ptpmessage/ptpmessagepdelayresp.h \
    ptpmessage/ptpmessagepdelayrespfollowup.h \
    ptpmessage/ptpmessagesync.h \
    ptpmessage/ptpmessagetype.h \
    ptpclock.h \
    mdsyncreceivesm.h \
    mdsyncsendsm.h \
    c-platform/linux/platform_linux.h \
    c-platform/platform.h \
    c-platform/platform_types.h \
    c-extensions/commondefs.h \
    c-extensions/ThreadWrapper.h \
    statemachinemanager.h \
    mdpdelayreq.h \
    mdpdelayresp.h \
    timeawaresystem.h

