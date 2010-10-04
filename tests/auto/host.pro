include(../../staticconfig.pri)

requires(!cross_compile)

TEMPLATE = subdirs
SUBDIRS = \
    maketestselftest

