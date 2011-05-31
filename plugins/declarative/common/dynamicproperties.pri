INCLUDEPATH += ../../../plugins/declarative/common/dynamicproperties/
HEADERS += ../../../plugins/declarative/common/dynamicproperties/qdeclarativeopenmetaobject_p.h
SOURCES += ../../../plugins/declarative/common/dynamicproperties/qdeclarativeopenmetaobject.cpp
include(../../../src/serviceframework/ipc/metaobjectbuilder.pri)
INCLUDEPATH += ../../../src/serviceframework/$$OBJECTBUILDER_INCLUDEPATH
DEPENDPATH += ../../../src/serviceframework/$$OBJECTBUILDER_DEPENDPATH
HEADERS += ../../../src/serviceframework/$$OBJECTBUILDER_HEADERS
SOURCES += ../../../src/serviceframework/$$OBJECTBUILDER_SOURCES

