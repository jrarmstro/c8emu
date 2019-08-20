TEMPLATE = app
TARGET = c8emu
INCLUDEPATH += ./src
QT += core gui widgets
OBJECTS_DIR = ./build
CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += src/chip8.h \
    src/window.h
SOURCES += src/chip8.cc \
           src/main.cc \
           src/window.cc

FORMS += \
    src/window.ui
