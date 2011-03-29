# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
deviceinfo_src = $$PWD
symbian {
    load(data_caging_paths)
}
deviceinfo_files.sources = $$deviceinfo_src/deviceinfo.qml
DEPLOYMENT += deviceinfo_files
