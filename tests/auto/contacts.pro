include(../../staticconfig.pri)

requires(contains(mobility_modules,contacts))

TEMPLATE = subdirs
SUBDIRS +=  qcontact \
    qcontactasync \
    qcontactdetail \
    qcontactdetaildefinition \
    qcontactdetails \
    qcontactfilter \
    qcontactmanager \
    qcontactmanagerplugins \
    qcontactrelationship \
    qlatin1constant
# This needs glibc:
linux*: SUBDIRS += qcontactmemusage

#contains(mobility_modules,versit):contains(QT_CONFIG,declarative) {
#    SUBDIRS += qdeclarativecontact
#}
contains(mobility_modules,serviceframework){
    SUBDIRS += \
        qcontactmanagerfiltering \
        qcontactactions
} else: warning(Some contacts unit tests depend on service framework too)

