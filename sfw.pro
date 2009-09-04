TEMPLATE = subdirs

CONFIG+=ordered
symbian:SUBDIRS += serviceframework/symbian/dll/databasemanagerserver_dll.pro \
                   serviceframework/symbian/exe/databasemanagerserver_exe.pro
SUBDIRS += serviceframework tools tests examples
