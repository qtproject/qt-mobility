INCLUDEPATH += ../../../plugins/declarative/common/dynamicproperties/
HEADERS += ../../../plugins/declarative/common/dynamicproperties/qdeclarativeopenmetaobject_p.h \
	   ../../../plugins/declarative/common/dynamicproperties/qmetaobjectbuilder_p.h
SOURCES += ../../../plugins/declarative/common/dynamicproperties/qdeclarativeopenmetaobject.cpp

!win32-msvc* {
    SOURCES += ../../../src/serviceframework/ipc/qmetaobjectbuilder.cpp
}
