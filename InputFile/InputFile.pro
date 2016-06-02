TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


SOURCES += main.cpp \
		InputFile.cpp \
    stdio_file.cpp \
    file.cpp \
    stringUtils.cpp \
    stringBuffer.cpp

HEADERS += \
		InputFile.h \
    stdio_file.h \
    file.h \
    stringUtils.h \
    stringBuffer.h

DISTFILES += \
    libalgo.incl

