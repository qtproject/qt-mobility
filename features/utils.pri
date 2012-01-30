# Helper functions

defineReplace(mobilityDeployFilename) {
   unset(MOBILITY_DEPLOY_NAME)
   MOBILITY_DEPLOY_NAME = $$1
   CONFIG(debug, debug|release): {
      mac:RET = $$member(MOBILITY_DEPLOY_NAME, 0)_debug
      else:win32:RET = $$member(MOBILITY_DEPLOY_NAME, 0)d
   }
   isEmpty(RET):RET = $$MOBILITY_DEPLOY_NAME
   return($$RET)
}

symbian {
    defineReplace(mobilityUID) {
        ret = $$1
        !isEmpty(QT_LIBINFIX): ret = $$replace(ret, 0x2, 0xE)
        return($$ret)
    }
}

defineReplace(mobilityPluginTarget) {
    target = $$1
    symbian: target = $$target$${QT_LIBINFIX}
    return($$qtLibraryTarget($$target))
}

