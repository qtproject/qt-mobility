!symbian:warning("DEPLOYMENT rules only tested on Symbian.")

QT += declarative network
SOURCES += $$PWD/qmllocationflickr.cpp
include($$PWD/deployment.pri)
include($$QT_MOBILITY_BUILD_TREE/examples/examples.pri)

symbian {
    TARGET.UID3 = 0x$$qmlflickr_uid3 # defined in deployment.pri
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
