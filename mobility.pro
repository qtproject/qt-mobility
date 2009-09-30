TEMPLATE = subdirs
TARGET =

CONFIG += ordered

# Enable messaging module for platforms that we support
wince*|win32|symbian|maemo {
    SUBDIRS = messaging tests examples
} else {
    # Fallback to QMF. Enabled if QMF_LIBDIR and QMF_INCLUDEDIR are set
    exists($(QMF_LIBDIR)):exists($(QMF_INCLUDEDIR)) {
        SUBDIRS = messaging tests examples
    }
}

