INCLUDEPATH += ../common/dynamicproperties/
HEADERS += ../common/dynamicproperties/qdeclarativeopenmetaobject_p.h \
	   ../common/dynamicproperties/qmetaobjectbuilder_p.h
SOURCES += ../common/dynamicproperties/qdeclarativeopenmetaobject.cpp

!win32-msvc* {
    SOURCES += ../../../src/serviceframework/ipc/qmetaobjectbuilder.cpp
}
