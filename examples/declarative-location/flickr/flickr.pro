include($$PWD/../declarative-location.pri)

QT += declarative network
SOURCES += $$PWD/qmllocationflickr.cpp
TARGET = qml_location_flickr
RESOURCES += flickr.qrc

OTHER_FILES += \
    flickr.qml \
    flickrcommon/Progress.qml \
    flickrcommon/RestModel.qml \
    flickrcommon/ScrollBar.qml \
    flickrcommon/Slider.qml \
    flickrmobile/Button.qml \
    flickrmobile/GeoTab.qml \
    flickrmobile/GridDelegate.qml \
    flickrmobile/ImageDetails.qml \
    flickrmobile/ListDelegate.qml \
    flickrmobile/nmealog.txt \
    flickrmobile/TitleBar.qml \
    flickrmobile/ToolBar.qml \
    flickrmobile/images/gloss.png \
    flickrmobile/images/lineedit.png \
    flickrmobile/images/lineedit.sci \
    flickrmobile/images/moon.png \
    flickrmobile/images/quit.png \
    flickrmobile/images/star.png \
    flickrmobile/images/stripes.png \
    flickrmobile/images/sun.png \
    flickrmobile/images/titlebar.png \
    flickrmobile/images/titlebar.sci \
    flickrmobile/images/toolbutton.png \
    flickrmobile/images/toolbutton.sci

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
