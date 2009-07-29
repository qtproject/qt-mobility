INCLUDEPATH += engines

# invalid backend (nonoptional)
HEADERS += engines/invalid/qcontactinvalidbackend_p.h
SOURCES += engines/invalid/qcontactinvalidbackend.cpp

# in memory backend
contains(CONTACTS_BACKENDS, memory) {
    HEADERS += engines/memory/qcontactmemorybackend_p.h
    SOURCES += engines/memory/qcontactmemorybackend.cpp
}

