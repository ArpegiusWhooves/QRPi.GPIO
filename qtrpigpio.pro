TEMPLATE = lib
TARGET = QtRPiGPIO
QT += qml quick
CONFIG += plugin c++11

TARGET = $$qtLibraryTarget($$TARGET)
uri = qtrpi.gpio

# Input
SOURCES += \
        source/gpio.cpp \
        source/qtrpigpio_plugin.cpp

HEADERS += \
        source/qrpigpio_plugin.h \
        source/gpio.h

win32:{
    DEFINES += QTRPI_MOCKUP
} else: {

HEADERS += source/c_gpio.h \
    source/common.h \
    source/cpuinfo.h \
    source/event_gpio.h \
    source/gpio.h

SOURCES += source/gpio.cpp \
    source/c_gpio.c \
    source/common.c \
    source/cpuinfo.c \
    source/event_gpio.c

}

DISTFILES = qmldir \
    qml/test.qml

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir

installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
qmldir.path = $$installPath
target.path = $$installPath
INSTALLS += target qmldir


