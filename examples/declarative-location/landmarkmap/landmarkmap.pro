include(../../mobility_examples.pri)

QT += declarative network
SOURCES += qmllandmarkmap.cpp

TARGET = qml_landmarkmap
TEMPLATE=app

win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=location
}

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.CAPABILITY += LocalServices ReadDeviceData WriteDeviceData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    landmarkmap.qrc

#workaround for QTMOBILITY-1502
my_files.sources = nmealog.txt mylm.lmx
DEPLOYMENT += my_files

install_files.files += nmealog.txt mylm.lmx
install_files.path = $$QT_MOBILITY_EXAMPLES
INSTALLS += install_files
#end workaround for QTMOBILITY-1502
