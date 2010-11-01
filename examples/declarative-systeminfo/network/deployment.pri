# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmlnetworkinfo_src = $$PWD
symbian {
    load(data_caging_paths)
}
qmlnetworkinfo_files.sources = $$qmlnetworkinfo_src/wifi.qml $$qmlnetworkinfo_src/images
DEPLOYMENT += qmlnetworkinfo_files

install_files.files =  $$qmlnetworkinfo_src/wifi.qml $$qmlnetworkinfo_src/images
install_files.path = $$QT_MOBILITY_EXAMPLES
INSTALLS += install_files
