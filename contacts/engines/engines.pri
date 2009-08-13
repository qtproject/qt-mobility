INCLUDEPATH += engines

# invalid backend (nonoptional)
HEADERS += engines/qcontactinvalidbackend_p.h
SOURCES += engines/qcontactinvalidbackend.cpp

# in memory backend
contains(CONTACTS_BACKENDS, memory) {
    HEADERS += engines/qcontactmemorybackend_p.h
    SOURCES += engines/qcontactmemorybackend.cpp
}

