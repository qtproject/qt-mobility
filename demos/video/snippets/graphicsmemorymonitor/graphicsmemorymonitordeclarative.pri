include($$PWD/graphicsmemorymonitor.pri)
QT += declarative
SOURCES += $$PWD/graphicsmemorymonitordeclarative.cpp

defineTest(graphicsMemoryItemAddDeployment) {
    symbian: graphicsmemoryitem_folder.source = $$PWD/$$SNIPPETS_PATH/graphicsmemorymonitor/qml/graphicsmemorymonitor
    else: graphicsmemoryitem_folder.source = $$SNIPPETS_PATH/graphicsmemorymonitor/qml/graphicsmemorymonitor
    graphicsmemoryitem_folder.target = qml
    DEPLOYMENTFOLDERS += graphicsmemoryitem_folder

    export(graphicsmemoryitem_folder.source)
    export(graphicsmemoryitem_folder.target)
    export(DEPLOYMENTFOLDERS)
}

message("graphicsmemorymonitordeclarative.pri loaded")

