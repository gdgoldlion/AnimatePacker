#-------------------------------------------------
#
# Project created by QtCreator 2012-05-02T20:43:06
#
#-------------------------------------------------

QT += \
    core \
    gui \
    xml

TARGET = AnimatePacker

TEMPLATE = app

ICON = image/icon.icns

RC_FILE = AnimatePacker.rc

INCLUDEPATH += \
    source

SOURCES += \
    source/sprites_list.cpp \
    source/sprite_frames_list.cpp \
    source/plist_list.cpp \
    source/main.cpp \
    source/animations_table.cpp \
    source/animate_packer.cpp

HEADERS  += \
    source/sprites_list.h \
    source/sprite_frames_list.h \
    source/plist_list.h \
    source/animations_table.h \
    source/animate_packer.h

FORMS    += \
    animate_packer.ui

RESOURCES += \
    AnimatePacker.qrc

#win32:CONFIG += console

OTHER_FILES += \
    AnimatePacker.rc



