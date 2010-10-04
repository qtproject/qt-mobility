# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
my_src_dir = $$PWD
symbian {
    load(data_caging_paths)
}
my_files.sources = $$my_src_dir/mapviewer.qml
DEPLOYMENT += my_files

install_files.files += $$my_src_dir/mapviewer.qml
install_files.path = $$QT_MOBILITY_EXAMPLES
INSTALLS += install_files
