# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
device_src = $$PWD
symbian {
    load(data_caging_paths)
}
device_files.sources = $$device_src/device.qml
DEPLOYMENT += device_files
