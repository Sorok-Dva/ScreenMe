QT = core gui widgets

CONFIG += windows

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    include/customTextInput.h \
    include/config_manager.h \
    include/editor.h \
    include/options_window.h \
    include/main_window.h \
    include/login_loader.h \
    include/login_server.h \
    include/uexception.h \
    include/uglobal.h \
    include/uglobalhotkeys.h \
    include/ukeysequence.h \
    include/screenshotdisplay.h

SOURCES += \
        main.cpp \
        src/customTextInput.cpp \
        src/config_manager.cpp \
        src/editor.cpp \
        src/options_window.cpp \
        src/main_window.cpp \
        src/screenshotdisplay.cpp \
        src/login_loader.cpp \
        src/login_server.cpp \
        src/uexception.cpp \
        src/uglobalhotkeys.cpp \
        src/ukeysequence.cpp 

TRANSLATIONS += \
    ScreenMe_fr_FR.ts
CONFIG += lrelease
CONFIG += embed_translations


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .gitattributes \
    .gitignore \
    LICENSE.txt \
    README.md



