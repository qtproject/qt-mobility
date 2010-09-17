# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmlstorageinfo_src = $$PWD
symbian {
    load(data_caging_paths)
}
qmlstorageinfo_files.sources = qmlstorageinfo_src/storage.qml 
DEPLOYMENT += qmlstorageinfo_files
