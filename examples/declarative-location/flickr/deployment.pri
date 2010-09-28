# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
qmlflickr_src = $$PWD
symbian {
    load(data_caging_paths)
}
deployment_files.sources = $$qmlflickr_src/flickr.qml $$qmlflickr_src/flickrcommon $$qmlflickr_src/flickrmobile
DEPLOYMENT += deployment_files
install_files.files = $$qmlflickr_src/flickr.qml $$qmlflickr_src/flickrcommon $$qmlflickr_src/flickrmobile
install_files.path = $$QT_MOBILITY_EXAMPLES
INSTALLS += install_files

