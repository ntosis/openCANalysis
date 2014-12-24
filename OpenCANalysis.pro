greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport sql
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/home/unix/qtserialport/src/serialport/qt4support/serialport.prf)
    QT += sql widgets
}

TARGET = terminal
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    sendform.cpp \
    candatabank.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    sendform.h \
    candatabank.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    sendform.ui \
    candatabank.ui

RESOURCES += \
    terminal.qrc


