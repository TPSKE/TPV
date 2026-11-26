QT = core
QT += xml

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../SharedModules

SOURCES += \
        TestingDrawContext.cpp \
        TestingLogger.cpp \
        TestingTrendWindows.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TrendDispBE/release/ -lTrendDispBE
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TrendDispBE/debug/ -lTrendDispBE
else:unix:!macx: LIBS += -L$$OUT_PWD/../TrendDispBE/ -lTrendDispBE

INCLUDEPATH += $$PWD/../TrendDispBE
DEPENDPATH += $$PWD/../TrendDispBE

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TrendDispBE/release/libTrendDispBE.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TrendDispBE/debug/libTrendDispBE.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TrendDispBE/release/TrendDispBE.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TrendDispBE/debug/TrendDispBE.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../TrendDispBE/libTrendDispBE.a

HEADERS += \
    TestingDrawContext.h \
    TestingLogger.h \
    TestingTrendWindows.h
