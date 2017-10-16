#-------------------------------------------------
#
# Project created by QtCreator 2017-10-11T14:58:58
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = AssistCollector
TEMPLATE = app

DEFINES += ASSISTCOLLECTOR_LIBRARY

SOURCES += \
    assist_collector.cpp \
    utilities/shared_buffer.cpp \
    library_exportfunction.cpp \
    main.cpp

HEADERS +=\
    assist_collector.h \
    utilities/shared_buffer.h \
    main_component_header.h \
    common.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
