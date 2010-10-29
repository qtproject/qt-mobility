# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmlbattery_src = $$PWD
symbian {
    load(data_caging_paths)
}
qmlbattery_files.sources = $$qmlbattery_src/battery-bubble.qml $$qmlbattery_src/images
DEPLOYMENT += qmlbattery_files

install_files.files = $$qmlbattery_src/battery-bubble.qml $$qmlbattery_src/images
install_files.path = $$QT_MOBILITY_EXAMPLES
INSTALLS += install_files
