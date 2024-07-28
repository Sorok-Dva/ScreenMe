QT += core gui widgets network websockets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = ScreenMe
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    include/customTextEdit.h \
    include/config_manager.h \
    include/editor.h \
    include/globalKeyboardHook.h \
    include/hotkeyEventFilter.h \
    include/hotkeymap.h \
    include/options_window.h \
    include/main_window.h \
    include/login_loader.h \
    include/login_server.h \
    include/uexception.h \
    include/uglobal.h \
    include/uglobalhotkeys.h \
    include/ukeysequence.h \
    include/screenshotdisplay.h \
    include/utils.h

SOURCES += \
        main.cpp \
        src/customTextInput.cpp \
        src/config_manager.cpp \
        src/editor.cpp \
        src/globalKeyboardHook.cpp \
        src/hotkeyEventFilter.cpp \
        src/options_window.cpp \
        src/main_window.cpp \
        src/screenshotdisplay.cpp \
        src/login_loader.cpp \
        src/login_server.cpp \
        src/uexception.cpp \
        src/uglobalhotkeys.cpp \
        src/ukeysequence.cpp  \
        src/utils.cpp

RESOURCES += icons.qrc

macx: LIBS += -framework ApplicationServices -framework Carbon

# Include path for headers
INCLUDEPATH += .

# Include path for Qt modules
INCLUDEPATH += $$[QT_INSTALL_HEADERS]

# Include path for macOS specific frameworks
macx: INCLUDEPATH += /System/Library/Frameworks/ApplicationServices.framework/Versions/A/Headers

# Define macros if necessary
DEFINES += QT_DEPRECATED_WARNINGS

# Enable C++11
CONFIG += c++11

# Additional settings for macOS
macx {
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_LFLAGS += -stdlib=libc++
}
