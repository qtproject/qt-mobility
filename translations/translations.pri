defineReplace(prependAll) {
    prepend = $$1
    arglist = $$2
    append  = $$3
    for(a,arglist) {
      result += $${prepend}$${a}$${append}
    }
    return ($$result)
}

qtPrepareTool(LUPDATE, lupdate)
LUPDATE += -locations relative -no-ui-lines

QTMOBILITY_TS        = ar cs da de es fr he hu ja_JP pl pt ru sk sl sv uk zh_CN zh_TW

lupdate.commands = (cd $$QT_MOBILITY_SOURCE_TREE && $$LUPDATE \
                                -I../include \
                                src/bearer \
                                src/contacts \
                                src/location \
                                src/messaging \
                                src/multimedia \
                                src/publishsubscribe \
                                src/sensors \
                                src/serviceframework \
                                src/systeminfo \
                                src/versit \
                                plugins \
                                -ts $$prependAll($$QT_MOBILITY_SOURCE_TREE/translations/qtmobility_,$$QTMOBILITY_TS,.ts))

QMAKE_EXTRA_TARGETS += lupdate
