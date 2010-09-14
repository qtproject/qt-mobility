# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
my_src_dir = $$PWD
symbian {
    load(data_caging_paths)
}
my_files.sources = $$my_src_dir/landmarkmap.qml $$my_src_dir/common $$my_src_dir/mobile
DEPLOYMENT += my_files
