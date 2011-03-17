QT += declarative network
SOURCES += $$PWD/qmllandmarkmap.cpp
include($$PWD/../declarative-location.pri)
RESOURCES += landmarkmap.qrc

TARGET = qml_landmarkmap

OTHER_FILES = \
    landmarkmap.qml \
    nmealog.txt \
    landmarkmapcommon/ScrollBar.qml \
    landmarkmapcommon/Slider.qml \
    landmarkmapmobile/Button.qml \
    landmarkmapmobile/Floater.qml \
    landmarkmapmobile/ListDelegate.qml \
    landmarkmapmobile/StatusBar.qml \
    landmarkmapmobile/TitleBar.qml \
    landmarkmapmobile/ToolBar.qml \
    landmarkmapmobile/images/gloss.png \
    landmarkmapmobile/images/landmarkstar.png \
    landmarkmapmobile/images/lineedit.png \
    landmarkmapmobile/images/lineedit.sci \
    landmarkmapmobile/images/moon.png \
    landmarkmapmobile/images/quit.png \
    landmarkmapmobile/images/star.png \
    landmarkmapmobile/images/stripes.png \
    landmarkmapmobile/images/sun.png \
    landmarkmapmobile/images/titlebar.png \
    landmarkmapmobile/images/titlebar.sci \
    landmarkmapmobile/images/toolbutton.png \
    landmarkmapmobile/images/toolbutton.sci

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
