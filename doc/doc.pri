
win32:!win32-g++ {
    unixstyle = false
} else:win32-g++:isEmpty(QMAKE_SH) {
    unixstyle = false
} else {
    unixstyle = true
}

LINE_SEP=$$escape_expand(\n\t)
GENERATOR = $$[QT_INSTALL_BINS]/qhelpgenerator
QDOC = $$[QT_INSTALL_BINS]/qdoc3
MOBILITY_DOCUMENTATION = $$QDOC $${QT_MOBILITY_SOURCE_TREE}/doc/src/qtmobility.qdocconf $$LINE_SEP \
                         cd $${QT_MOBILITY_SOURCE_TREE} && \
                          $$GENERATOR doc/html/qtmobility.qhp -o doc/qch/qtmobility.qch

# Sensors uses .dia files which must be compiled into .png for the docs
unix {
    LOGNAME=$$(LOGNAME)
    equals(LOGNAME,lramsay) {
        # Only do this on Unix when logged in as me
        MOBILITY_DOCUMENTATION=\
            @( cd "$$PWD/src";\
            for file in *.dia; do\
                destfile="images/\$$(echo "\$$file" | sed 's/dia\$$/png/')";\
                if [ "\$$file" -nt "\$$destfile" ]; then\
                    dia -e "\$$destfile" "\$$file";\
                fi;\
            done ) || true\
            $$LINE_SEP $$MOBILITY_DOCUMENTATION
    }
}

contains(unixstyle, false):MOBILITY_DOCUMENTATION = $$replace(MOBILITY_DOCUMENTATION, "/", "\\")

# Build rules
qch_docs.commands = $$MOBILITY_DOCUMENTATION

docs.depends = qch_docs


QMAKE_EXTRA_TARGETS += qch_docs docs
