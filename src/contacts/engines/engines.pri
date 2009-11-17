INCLUDEPATH += engines

# invalid backend (nonoptional)
PRIVATE_HEADERS += engines/qcontactinvalidbackend_p.h
SOURCES += engines/qcontactinvalidbackend.cpp

# in memory backend
contains(CONTACTS_BACKENDS, memory) {
    PRIVATE_HEADERS += engines/qcontactmemorybackend_p.h
    SOURCES += engines/qcontactmemorybackend.cpp
}

