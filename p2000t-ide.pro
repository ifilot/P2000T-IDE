QT     += core gui widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/dialogslotselection.cpp \
    src/assemblyhighlighter.cpp \
    src/codeeditor.cpp \
    src/flashthread.cpp \
    src/ioworker.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/qhexview.cpp \
    src/readthread.cpp \
    src/serial_interface.cpp \
    src/serialwidget.cpp \
    src/threadcompile.cpp \
    src/threadrun.cpp \
    src/threadtl866.cpp \
    src/tl866widget.cpp

HEADERS += \
    src/dialogslotselection.h \
    src/assemblyhighlighter.h \
    src/codeeditor.h \
    src/config.h \
    src/flashthread.h \
    src/ioworker.h \
    src/mainwindow.h \
    src/qhexview.h \
    src/readthread.h \
    src/serial_interface.h \
    src/serialwidget.h \
    src/threadcompile.h \
    src/threadrun.h \
    src/threadtl866.h \
    src/tl866widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# add libraries
win32 {
    INCLUDEPATH +=  "D:\PROGRAMMING\LIBS\boost-1.74.0-win-x64\include"
    Debug:LIBS +=   "-LD:\PROGRAMMING\LIBS\boost-1.74.0-win-x64\lib" -lboost_date_time-vc142-mt-gd-x64-1_74
    Release:LIBS +=   "-LD:\PROGRAMMING\LIBS\boost-1.74.0-win-x64\lib" -lboost_date_time-vc142-mt-x64-1_74
}

RESOURCES += \
    resources.qrc
