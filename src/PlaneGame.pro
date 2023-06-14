QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    main.cpp \
    mainwindow.cpp \
    mesh.cpp \
    model.cpp \
    myopenglwidget.cpp \
    objectpose.cpp

HEADERS += \
    camera.h \
    mainwindow.h \
    mesh.h \
    model.h \
    myopenglwidget.h \
    objectpose.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lassimp-vc143-mtd

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
