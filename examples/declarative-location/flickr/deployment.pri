# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmlflickr_src = $$PWD
symbian {
    load(data_caging_paths)
    # Globally UID of this application TODO this is not from appropriate range
    qmlflickr_uid3 = E00000BD
    qmlflickr_files.path = $$APP_PRIVATE_DIR_BASE/$$qmlflickr_uid3
}
qmlflickr_files.sources = $$qmlflickr_src/flickr.qml $$qmlflickr_src/common $$qmlflickr_src/mobile
DEPLOYMENT += qmlflickr_files
