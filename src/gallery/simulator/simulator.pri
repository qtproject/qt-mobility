
INCLUDEPATH += $$PWD

DEFINES += QT_DOCUMENT_GALLERY_SIMULATOR

PRIVATE_HEADERS += \
        $$PWD/qsimulatorgalleryresultset_p.h \
        $$PWD/docgalleryconnection_simulator.h

SOURCES += \
        $$PWD/qdocumentgallery_simulator.cpp \
        $$PWD/qsimulatorgalleryresultset.cpp \
        $$PWD/docgalleryconnection_simulator.cpp \
        $$PWD/docgallerysimulatordata.cpp

HEADERS += \
    $$PWD/docgallerysimulatordata.h

INCLUDEPATH += ../mobilitysimulator
qtAddLibrary(QtMobilitySimulator)
