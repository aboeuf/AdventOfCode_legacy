QT += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    2019/intcodecomputer.cpp \
    2019/puzzle_2019_01.cpp \
    2019/puzzle_2019_02.cpp \
    2019/puzzle_2019_03.cpp \
    2019/puzzle_2019_04.cpp \
    2019/puzzle_2019_05.cpp \
    2019/puzzle_2019_06.cpp \
    2020/puzzle_2020_01.cpp \
    2020/puzzle_2020_02.cpp \
    2020/puzzle_2020_03.cpp \
    2020/puzzle_2020_04.cpp \
    2020/puzzle_2020_05.cpp \
    2020/puzzle_2020_06.cpp \
    2020/puzzle_2020_07.cpp \
    2020/puzzle_2020_08.cpp \
    2020/puzzle_2020_09.cpp \
    2020/puzzle_2020_10.cpp \
    2020/puzzle_2020_11.cpp \
    2020/puzzle_2020_12.cpp \
    2020/puzzle_2020_13.cpp \
    common.cpp \
    main.cpp \
    mainwindow.cpp \
    solvers.cpp

HEADERS += \
    2019/intcodecomputer.h \
    2019/puzzle_2019_01.h \
    2019/puzzle_2019_02.h \
    2019/puzzle_2019_03.h \
    2019/puzzle_2019_04.h \
    2019/puzzle_2019_05.h \
    2019/puzzle_2019_06.h \
    2020/puzzle_2020_01.h \
    2020/puzzle_2020_02.h \
    2020/puzzle_2020_03.h \
    2020/puzzle_2020_04.h \
    2020/puzzle_2020_05.h \
    2020/puzzle_2020_06.h \
    2020/puzzle_2020_07.h \
    2020/puzzle_2020_08.h \
    2020/puzzle_2020_09.h \
    2020/puzzle_2020_10.h \
    2020/puzzle_2020_11.h \
    2020/puzzle_2020_12.h \
    2020/puzzle_2020_13.h \
    common.h \
    jsonhelper.h \
    mainwindow.h \
    solvers.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
