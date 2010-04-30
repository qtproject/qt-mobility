SOURCES = main.cpp \
    view.cpp \
    inputcontroller.cpp \
    keycontroller.cpp \
    accelerometercontroller.cpp \
    compasscontroller.cpp \
    orientationcontroller.cpp \
    magnetometercontroller.cpp \
    timedcontroller.cpp \
    rotationcontroller.cpp
RESOURCES = panorama.qrc
CONFIG += mobility
MOBILITY += sensors
INCLUDEPATH += ../../maemo6

# install
target.path = $$[QT_INSTALL_EXAMPLES]/panorama/panorama
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    $$FORMS \
    panorama.pro \
    images
sources.path = $$[QT_INSTALL_EXAMPLES]/panorama/panorama
INSTALLS += target \
    sources
HEADERS += view.h \
    inputcontroller.h \
    keycontroller.h \
    accelerometercontroller.h \
    compasscontroller.h \
    magnetometercontroller.h \
    orientationcontroller.h \
    rotationcontroller.h \
    tapcontroller.h \
    timedcontroller.h
