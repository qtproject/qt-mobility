INCLUDEPATH += engines

# invalid backend (nonoptional)
HEADERS += engines/invalid/qcontactinvalidbackend_p.h
SOURCES += engines/invalid/qcontactinvalidbackend.cpp

# in memory backend
contains(CONTACTS_BACKEND, memory) { 
    HEADERS += engines/memory/qcontactmemorybackend_p.h
    SOURCES += engines/memory/qcontactmemorybackend.cpp
}