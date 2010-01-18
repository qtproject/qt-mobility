TEMPLATE = app
TARGET = qcmexample
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../git/qtmobility/install/include
INCLUDEPATH += ../../git/kinetic/include/QtDeclarative
QT += declarative
QT += script
LIBS += -lQtContacts

# Input
SOURCES += main.cpp \
    qmlcontacts.cpp \
    qmlcontact.cpp
HEADERS += qmlcontactsa.h \
    qmlcontact.h
RESOURCES += qmlcontacts.qrc
OTHER_FILES += example.qml \
    Recipes.qml \
    contents/pics/vegetable-soup.jpg \
    contents/pics/trash.png \
    contents/pics/pancakes.jpg \
    contents/pics/moreUp.png \
    contents/pics/moreDown.png \
    contents/pics/list-remove.png \
    contents/pics/list-add.png \
    contents/pics/lemonade.jpg \
    contents/pics/hamburger.jpg \
    contents/pics/go-up.png \
    contents/pics/go-down.png \
    contents/pics/fruit-salad.jpg \
    contents/pics/del.png \
    contents/pics/button.png \
    contents/pics/button-pressed.png \
    contents/pics/archive-remove.png \
    contents/pics/archive-insert.png \
    contents/pics/add.png
