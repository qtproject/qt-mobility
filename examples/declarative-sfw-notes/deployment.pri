# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmlnotes_src = $$PWD

symbian {
    load(data_caging_paths)
}

deployment_files.sources = $$qmlnotes_src/declarative-sfw-notes.qml $$qmlnotes_src/content-sfw-notes
DEPLOYMENT += deployment_files

maemo* {
    install_files.files = $$qmlnotes_src/maemo-sfw-notes.qml
} else {
    install_files.files = $$qmlnotes_src/declarative-sfw-notes.qml
}
install_files.files += $$qmlnotes_src/content-sfw-notes
install_files.path = $$QT_MOBILITY_EXAMPLES
INSTALLS += install_files

