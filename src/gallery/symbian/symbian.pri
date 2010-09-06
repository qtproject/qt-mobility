iNCLUDEPATH += $$PWD
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
LIBS +=     -lmdeclient.dll \
            -leuser.dll \
            -lcharconv

SOURCES += $$PWD/qmdegalleryresultset.cpp \
           $$PWD/qgallerymdsutility.cpp \
           $$PWD/qmdegallerytyperesultset.cpp \
           $$PWD/qmdegalleryitemresultset.cpp \
           $$PWD/qmdegalleryremoveresultset.cpp \
           $$PWD/qmdegalleryqueryresultset.cpp \
           $$PWD/qmdesession.cpp \
           $$PWD/qdocumentgallery_symbian.cpp

PRIVATE_HEADERS += $$PWD/qmdegalleryresultset.h \
           $$PWD/qgallerymdsutility.h \
           $$PWD/qmdegallerytyperesultset.h \
           $$PWD/qmdegalleryitemresultset.h \
           $$PWD/qmdegalleryremoveresultset.h \
           $$PWD/qmdegalleryqueryresultset.h \
           $$PWD/qmdesession.h
