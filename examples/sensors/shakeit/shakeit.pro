TEMPLATE = app
TARGET = shakeit
QT += declarative
SOURCES = main.cpp

app.files = \
    $$files(*.qml) \
    icon.png \
    audio \
    content

target.path = $$[QT_INSTALL_EXAMPLES]/qtsensors/shakeit
app.path = $$[QT_INSTALL_EXAMPLES]/qtsensors/shakeit
INSTALLS += target app
