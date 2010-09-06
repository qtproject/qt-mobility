INCLUDEPATH += engines

# invalid backend (nonoptional)
PRIVATE_HEADERS += engines/qcontactinvalidbackend_p.h
SOURCES += engines/qcontactinvalidbackend.cpp

# in memory backend (also nonoptional)
PRIVATE_HEADERS += engines/qcontactmemorybackend_p.h
SOURCES += engines/qcontactmemorybackend.cpp

# simulator backend
simulator {
    PRIVATE_HEADERS += engines/qcontactsimulatorbackend_p.h
    SOURCES += engines/qcontactsimulatorbackend.cpp
}