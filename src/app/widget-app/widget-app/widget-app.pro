CX_TARGET_NAME = widget-app

QT       += core gui widgets qml quick quickcontrols2 quickwidgets

SOURCES += \
    bridge.cpp \
    main.cpp \
    mainwindow.cpp \
    textmetrics.cpp

HEADERS += \
    bridge.h \
    mainwindow.h \
    textmetrics.h

include($$PWD/module/module.pri)

include($$PWD/../../../utils/app.pri)

RESOURCES += \
    res.qrc
