TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += "C:\boost\boost_1_69_0"
LIBS += "-LC:\boost\boost_1_69_0\lib32-msvc-14.1"

SOURCES += \
        kasp_db_mock.cpp \
    kasp_thread_pool.cpp \
        main.cpp \
    kasp_decorator.cpp \
    kasp_event.cpp

HEADERS += \
    kasp_db_mock.h \
    kasp_sdk.h \
    kasp_decorator.h \
    kasp_event.h \
    kasp_thread_pool.h \
    kasp_timer.h
