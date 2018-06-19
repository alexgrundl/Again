TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

win32: LIBS += -lhid -lsetupapi
else: unix: LIBS += -pthread -lrt

DEFINES += _LOG_TO_CONSOLE
unix: DEFINES += _LOG_TO_SYSLOG

INCLUDEPATH += c-extensions
INCLUDEPATH += c-platform
INCLUDEPATH += common
INCLUDEPATH += common/types
INCLUDEPATH += common/statemachines
INCLUDEPATH += common/ptpmessage
INCLUDEPATH += common/licensecheck
INCLUDEPATH += linux
INCLUDEPATH += windows

SOURCES += main.cpp \
    common/ptpclock.cpp \
    common/timeawaresystem.cpp \
    common/portmanager.cpp \
    common/timecontrol.cpp \
    common/systemport.cpp \
    common/timesyncdaemon.cpp \
    common/receivepackage.cpp \
    common/statemachinemanager.cpp \
    common/statemachines/sitesyncsync.cpp \
    common/statemachines/statemachinebase.cpp \
    common/statemachines/portsyncsyncreceive.cpp \
    common/statemachines/clockmastersyncsend.cpp \
    common/statemachines/clockmastersyncoffset.cpp \
    common/statemachines/clockmastersyncreceive.cpp \
    common/statemachines/portsyncsyncsend.cpp \
    common/statemachines/clockslavesync.cpp \
    common/statemachines/mdsyncreceivesm.cpp \
    common/statemachines/mdsyncsendsm.cpp \
    common/statemachines/mdpdelayreq.cpp \
    common/statemachines/mdpdelayresp.cpp \
    common/statemachines/portannounceinformation.cpp \
    common/statemachines/portannouncereceive.cpp \
    common/statemachines/portroleselection.cpp \
    common/statemachines/portannouncetransmit.cpp \
    common/statemachines/mdportannouncetransmit.cpp \
    common/statemachines/portipc.cpp \
    common/statemachines/linkdelaysyncintervalsetting.cpp \
    common/statemachines/platformsync.cpp \
    common/ptpmessage/ptpmessagebase.cpp \
    common/ptpmessage/ptpmessagefollowup.cpp \
    common/ptpmessage/ptpmessagepdelayreq.cpp \
    common/ptpmessage/ptpmessagepdelayresp.cpp \
    common/ptpmessage/ptpmessagepdelayrespfollowup.cpp \
    common/ptpmessage/ptpmessagesync.cpp \
    common/ptpmessage/ptpmessageannounce.cpp \
    common/ptpmessage/ptpmessagesignaling.cpp \
    common/types/scaledns.cpp \
    common/types/timestamp.cpp \
    common/types/uscaledns.cpp \
    common/types/extendedtimestamp.cpp \
    common/licensecheck/sha3.c \
    common/licensecheck/byte_order.c \
    common/licensecheck/licensecheck.cpp \
    c-platform/linux/platform_linux.cpp \
    c-platform/win/platform_win.cpp \
    c-extensions/ThreadWrapper.cpp \
    c-extensions/TimeMeas.cpp \
    linux/ptpclocklinux.cpp \
    linux/licensechecklinux.cpp \
    linux/timesyncdaemonlinux.cpp \
    linux/netportlinux.cpp \
    linux/sharedmemoryipclinux.cpp \
    windows/licensecheckwindows.cpp \
    windows/ptpclockwindows.cpp \
    windows/timesyncdaemonwindows.cpp \
    windows/netportwindows.cpp \
    common/ptpconfig.cpp \
    linux/seriallinux.cpp \
    common/gpssync.cpp \
    common/gpsclock.cpp

HEADERS += \
    common/types/types.h \
    common/commstructs.h \
    common/interfaces.h \
    common/timeawaresystem.h \
    common/portmanager.h \
    common/timecontrol.h \
    common/ipcdef.hpp \
    common/ptpclock.h \
    common/inetport.h \
    common/receivepackage.h \
    common/systemport.h \
    common/timesyncdaemon.h \
    common/statemachinemanager.h \
    common/statemachines/sitesyncsync.h \
    common/statemachines/statemachinebase.h \
    common/statemachines/portsyncsyncreceive.h \
    common/statemachines/clockmastersyncsend.h \
    common/statemachines/clockmastersyncoffset.h \
    common/statemachines/clockmastersyncreceive.h \
    common/statemachines/portsyncsyncsend.h \
    common/statemachines/clockslavesync.h \
    common/statemachines/mdsyncreceivesm.h \
    common/statemachines/mdsyncsendsm.h \
    common/statemachines/mdpdelayreq.h \
    common/statemachines/mdpdelayresp.h \
    common/statemachines/portannounceinformation.h \
    common/statemachines/portannouncereceive.h \
    common/statemachines/portroleselection.h \
    common/statemachines/portannouncetransmit.h \
    common/statemachines/mdportannouncetransmit.h \
    common/statemachines/portipc.h \
    common/statemachines/linkdelaysyncintervalsetting.h \
    common/statemachines/platformsync.h \
    common/ptpmessage/ptpmessagebase.h \
    common/ptpmessage/ptpmessagefollowup.h \
    common/ptpmessage/ptpmessagepdelayreq.h \
    common/ptpmessage/ptpmessagepdelayresp.h \
    common/ptpmessage/ptpmessagepdelayrespfollowup.h \
    common/ptpmessage/ptpmessagesync.h \
    common/ptpmessage/ptpmessagetype.h \
    common/ptpmessage/ptpmessageannounce.h \
    common/ptpmessage/ptpmessagesignaling.h \
    common/c-platform/linux/platform_linux.h \
    common/c-platform/platform.h \
    common/c-platform/platform_types.h \
    common/c-platform/win/platform_win.h \
    common/c-extensions/commondefs.h \
    common/c-extensions/ThreadWrapper.h \
    common/c-extensions/TimeMeas.h \
    common/c-extensions/autolock.h \
    common/c-extensions/locked_deque.h \
    common/c-extensions/locked_map.h \
    common/types/scaledns.h \
    common/types/timestamp.h \
    common/types/uscaledns.h \
    common/types/extendedtimestamp.h \
    common/licensecheck/sha3.h \
    common/licensecheck/ustd.h \
    common/licensecheck/byte_order.h \
    common/licensecheck/licensecheck.h \
    linux/ptpclocklinux.h \
    linux/licensechecklinux.h \
    linux/timesyncdaemonlinux.h \
    linux/netportlinux.h \
    linux/sharedmemoryipclinux.h \
    windows/ptpclockwindows.h \
    windows/licensecheckwindows.h \
    windows/timesyncdaemonwindows.h \
    windows/netportwindows.h \
    common/ptpconfig.h \
    common/serial.h \
    linux/seriallinux.h \
    common/GPSdef.h \
    common/gpssync.h \
    common/gpsclock.h

DISTFILES += \
    ixgbe_4.4.59.patch \
    ixgbe_4.12.14.patch

