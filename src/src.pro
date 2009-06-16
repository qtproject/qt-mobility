TEMPLATE = lib
TARGET = QtLocation

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

include(../location.pri)
