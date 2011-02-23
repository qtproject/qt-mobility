# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
battery2_src = $$PWD
symbian {
    load(data_caging_paths)
}
battery2_files.sources = $$battery2_src/battery2.qml
DEPLOYMENT += battery2_files
