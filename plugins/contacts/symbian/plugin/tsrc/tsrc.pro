TEMPLATE = subdirs
TARGET = 
CONFIG += ordered

include(../symbian_defines.pri)

SUBDIRS += performance \
           # tst_performance is commented out because of Versit compilation issues
           #tst_performance \
           tst_qcontactmanagersymbian \
           ut_symbian
#           tst_details

!contains(DEFINES, SYMBIAN_BACKEND_USE_SQLITE) {
   SUBDIRS += tst_cntfilteringdbms
}