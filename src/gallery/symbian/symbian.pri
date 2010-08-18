iNCLUDEPATH += $$PWD
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
LIBS +=     -lmdeclient.dll \
            -leuser.dll

SOURCES += $$PWD/qmdegalleryresultset.cpp \
           $$PWD/qgallerymdsutility.cpp \
           $$PWD/qmdegallerytyperesultset.cpp \
           $$PWD/qmdegalleryitemresultset.cpp \
           $$PWD/qmdesession.cpp \
           $$PWD/qdocumentgallery_symbian.cpp \
           $$PWD/qmdequery.cpp

PRIVATE_HEADERS += $$PWD/qmdegalleryresultset.h \
           $$PWD/qgallerymdsutility.h \
           $$PWD/qmdegallerytyperesultset.h \
           $$PWD/qmdegalleryitemresultset.h \
           $$PWD/qmdesession.h \
           $$PWD/qmdequery.h
