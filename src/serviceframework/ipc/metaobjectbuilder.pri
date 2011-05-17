#check version for 4.7 ...
contains(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 8) {
  OBJECTBUILDER_INCLUDEPATH += ipc
  OBJECTBUILDER_DEPENDPATH += ipc
  OBJECTBUILDER_HEADERS += ipc/qmetaobjectbuilder_47_p.h
  OBJECTBUILDER_SOURCES += ipc/qmetaobjectbuilder_47.cpp
} else {
  OBJECTBUILDER_INCLUDEPATH += ipc
  OBJECTBUILDER_DEPENDPATH += ipc
  OBJECTBUILDER_HEADERS += ipc/qmetaobjectbuilder_p.h
  OBJECTBUILDER_SOURCES += ipc/qmetaobjectbuilder.cpp
}
