# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
my_src_dir = $$PWD
symbian {
    load(data_caging_paths)
    # Globally UID of this application TODO this is not from proper range ATM
    my_uid3 = E00000BC
    my_files.path = $$APP_PRIVATE_DIR_BASE/$$my_uid3
}
my_files.sources = $$my_src_dir/landmarkmap.qml $$my_src_dir/common $$my_src_dir/mobile
DEPLOYMENT += my_files
