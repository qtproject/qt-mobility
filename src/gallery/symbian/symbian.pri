iNCLUDEPATH += $$PWD
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

contains(mds_25_enabled, yes) {
    DEFINES += MDS_25_COMPILATION_ENABLED
}

contains(mds_25_92mcl_enabled, yes) {
    DEFINES += MDS_25_92MCL_COMPILATION_ENABLED
}

# Input
LIBS +=     -lmdeclient.dll \
            -leuser.dll \
            -lcharconv

SOURCES += $$PWD/qmdegalleryresultset.cpp \
           $$PWD/qgallerymdsutility.cpp \
           $$PWD/qmdegallerytyperesultset.cpp \
           $$PWD/qmdegalleryitemresultset.cpp \
           $$PWD/qmdegalleryqueryresultset.cpp \
           $$PWD/qmdegallerycategoryresultset.cpp \
           $$PWD/qmdesession.cpp \
           $$PWD/qdocumentgallery_symbian.cpp

PRIVATE_HEADERS += $$PWD/qmdegalleryresultset_p.h \
           $$PWD/qgallerymdsutility_p.h \
           $$PWD/qmdegallerytyperesultset_p.h \
           $$PWD/qmdegalleryitemresultset_p.h \
           $$PWD/qmdegalleryqueryresultset_p.h \
           $$PWD/qmdegallerycategoryresultset_p.h \
           $$PWD/qmdesession_p.h
