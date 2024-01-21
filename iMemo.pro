QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutwindow.cpp \
    configman.cpp \
    editor.cpp \
    gotoflashcarddialog.cpp \
    main.cpp \
    mainmenu.cpp \
    quickeditor.cpp \
    reader.cpp \
    settings.cpp \
    stackexporter.cpp \
    stackmenu.cpp

HEADERS += \
    aboutwindow.h \
    configman.h \
    editor.h \
    gotoflashcarddialog.h \
    mainmenu.h \
    quickeditor.h \
    reader.h \
    settings.h \
    stackexporter.h \
    stackmenu.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets.qrc
