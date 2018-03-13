TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -pthread

INCLUDEPATH += c-extensions
INCLUDEPATH += c-platform
INCLUDEPATH += types
INCLUDEPATH += statemachines
INCLUDEPATH += ptpmessage
INCLUDEPATH += comm

SOURCES += main.cpp \
    statemachines/sitesyncsync.cpp \
    statemachines/statemachinebase.cpp \
    statemachines/portsyncsyncreceive.cpp \
    statemachines/clockmastersyncsend.cpp \
    statemachines/clockmastersyncoffset.cpp \
    statemachines/clockmastersyncreceive.cpp \
    statemachines/portsyncsyncsend.cpp \
    statemachines/clockslavesync.cpp \
    ptpmessage/ptpmessagebase.cpp \
    ptpmessage/ptpmessagefollowup.cpp \
    ptpmessage/ptpmessagepdelayreq.cpp \
    ptpmessage/ptpmessagepdelayresp.cpp \
    ptpmessage/ptpmessagepdelayrespfollowup.cpp \
    ptpmessage/ptpmessagesync.cpp \
    ptpclock.cpp \
    statemachines/mdsyncreceivesm.cpp \
    statemachines/mdsyncsendsm.cpp \
    c-platform/linux/platform_linux.cpp \
    c-extensions/ThreadWrapper.cpp \
    statemachinemanager.cpp \
    statemachines/mdpdelayreq.cpp \
    statemachines/mdpdelayresp.cpp \
    timeawaresystem.cpp \
    c-extensions/TimeMeas.cpp \
    comm/linux_netport.cpp \
    portmanager.cpp \
    statemachines/portannounceinformation.cpp \
    statemachines/portannouncereceive.cpp \
    statemachines/portroleselection.cpp \
    ptpmessage/ptpmessageannounce.cpp \
    statemachines/portannouncetransmit.cpp \
    statemachines/mdportannouncetransmit.cpp \
    types/scaledns.cpp \
    types/timestamp.cpp \
    types/uscaledns.cpp \
    types/extendedtimestamp.cpp

HEADERS += \
    types/types.h \
    statemachines/sitesyncsync.h \
    commstructs.h \
    statemachines/statemachinebase.h \
    statemachines/portsyncsyncreceive.h \
    statemachines/clockmastersyncsend.h \
    statemachines/clockmastersyncoffset.h \
    statemachines/clockmastersyncreceive.h \
    statemachines/portsyncsyncsend.h \
    statemachines/clockslavesync.h \
    interfaces.h \
    ptpmessage/ptpmessagebase.h \
    ptpmessage/ptpmessagefollowup.h \
    ptpmessage/ptpmessagepdelayreq.h \
    ptpmessage/ptpmessagepdelayresp.h \
    ptpmessage/ptpmessagepdelayrespfollowup.h \
    ptpmessage/ptpmessagesync.h \
    ptpmessage/ptpmessagetype.h \
    ptpclock.h \
    statemachines/mdsyncreceivesm.h \
    statemachines/mdsyncsendsm.h \
    c-platform/linux/platform_linux.h \
    c-platform/platform.h \
    c-platform/platform_types.h \
    c-extensions/commondefs.h \
    c-extensions/ThreadWrapper.h \
    statemachinemanager.h \
    statemachines/mdpdelayreq.h \
    statemachines/mdpdelayresp.h \
    timeawaresystem.h \
    c-extensions/TimeMeas.h \
    comm/netport.h \
    comm/linux_netport.h \
    portmanager.h \
    statemachines/portannounceinformation.h \
    statemachines/portannouncereceive.h \
    statemachines/portroleselection.h \
    ptpmessage/ptpmessageannounce.h \
    comm/netport.h \
    statemachines/portannouncetransmit.h \
    statemachines/mdportannouncetransmit.h \
    c-extensions/autolock.h \
    c-extensions/locked_deque.h \
    c-extensions/locked_map.h \
    types/scaledns.h \
    types/timestamp.h \
    types/uscaledns.h \
    types/extendedtimestamp.h

