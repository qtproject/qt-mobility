INCLUDEPATH += engines \
    ./

# invalid backend (nonoptional)
PRIVATE_HEADERS += engines/qorganizeriteminvalidbackend_p.h
SOURCES += engines/qorganizeriteminvalidbackend.cpp

# in memory backend (also nonoptional)
PRIVATE_HEADERS += engines/qorganizeritemmemorybackend_p.h
SOURCES += engines/qorganizeritemmemorybackend.cpp

