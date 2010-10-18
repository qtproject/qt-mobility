# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmldialer_src = $$PWD

symbian {
    load(data_caging_paths)
}

deployment_files.sources = $$qmldialer_src/declarative-sfw-dialer.qml $$qmldialer_src/content-sfw-dialer
DEPLOYMENT += deployment_files

install_files.files = $$qmldialer_src/declarative-sfw-dialer.qml $$qmldialer_src/content-sfw-dialer
install_files.path = $$QT_MOBILITY_EXAMPLES
INSTALLS += install_files

