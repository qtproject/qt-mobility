
win32:!win32-g++ {
    unixstyle = false
} else:win32-g++:isEmpty(QMAKE_SH) {
    unixstyle = false
} else {
    unixstyle = true
}

GENERATOR = $$[QT_INSTALL_BINS]/qhelpgenerator
QDOC = $$[QT_INSTALL_BINS]/qdoc3
MOBILITY_DOCUMENTATION = ($$QDOC $${QT_MOBILITY_SOURCE_TREE}/doc/src/qtmobility.qdocconf) && \
                         (cd $${QT_MOBILITY_SOURCE_TREE} && \
                          $$GENERATOR doc/html/qtmobility.qhp -o doc/qch/qtmobility.qch)

contains(unixstyle, false):MOBILITY_DOCUMENTATION = $$replace(MOBILITY_DOCUMENTATION, "/", "\\")

# Build rules
qch_docs.commands = $$MOBILITY_DOCUMENTATION

docs.depends = qch_docs


QMAKE_EXTRA_TARGETS += qch_docs docs
