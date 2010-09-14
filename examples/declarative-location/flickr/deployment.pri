# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmlflickr_src = $$PWD
symbian {
    load(data_caging_paths)
}
qmlflickr_files.sources = $$qmlflickr_src/flickr.qml $$qmlflickr_src/common $$qmlflickr_src/mobile
DEPLOYMENT += qmlflickr_files
