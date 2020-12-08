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
    2019/puzzle_2019_1.cpp \
    2019/puzzle_2019_2.cpp \
    2019/puzzle_2019_3.cpp \
    2019/puzzle_2019_4.cpp \
    2019/puzzle_2019_5.cpp \
    2020/puzzle_2020_1.cpp \
    2020/puzzle_2020_2.cpp \
    2020/puzzle_2020_3.cpp \
    2020/puzzle_2020_4.cpp \
    2020/puzzle_2020_5.cpp \
    2020/puzzle_2020_6.cpp \
    2020/puzzle_2020_7.cpp \
    2020/puzzle_2020_8.cpp \
    common.cpp \
    main.cpp \
    mainwindow.cpp \
    solvers.cpp

HEADERS += \
    2019/intcodecomputer.h \
    2019/puzzle_2019_1.h \
    2019/puzzle_2019_2.h \
    2019/puzzle_2019_3.h \
    2019/puzzle_2019_4.h \
    2019/puzzle_2019_5.h \
    2020/puzzle_2020_1.h \
    2020/puzzle_2020_2.h \
    2020/puzzle_2020_3.h \
    2020/puzzle_2020_4.h \
    2020/puzzle_2020_5.h \
    2020/puzzle_2020_6.h \
    2020/puzzle_2020_7.h \
    2020/puzzle_2020_8.h \
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
