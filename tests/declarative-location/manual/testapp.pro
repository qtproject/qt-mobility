#include(../../staticconfig.pri)
#include (../mobility_examples.pri)
TEMPLATE=app

QT += declarative network
SOURCES += $$PWD/testapp.cpp
#include($$PWD/../declarative-location.pri)
#include($$PWD/deployment.pri)

TARGET = testapp

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
# Deployment rules: which files to deploy along with the C++ project
# Particularly important for Symbian (so that files end up in .sis package)
my_src_dir = $$PWD
symbian {
    load(data_caging_paths)
}
my_files.sources = $$my_src_dir/testapp.qml
DEPLOYMENT += my_files

#install_files.files += $$my_src_dir/testapp.qml
#install_files.path = $$QT_MOBILITY_EXAMPLES
#INSTALLS += install_files
