import Qt 4.7

Rectangle {
    id: container

    signal clicked
    property string image: ""
    property string text: ""

    color: activePalette.button; smooth: true
    border.width: 1; border.color: Qt.darker(activePalette.button); radius: 8;

    gradient: Gradient {
        GradientStop {
            id: topGrad; position: 0.0
            color : mr.pressed ?  activePalette.dark : activePalette.light
        }
        GradientStop { position: 1.0; color: activePalette.button }
    }
    width: stuff.width + 4
    height: stuff.height + 4

    Item {
        id: stuff
        width: childrenRect.width;
        height: childrenRect.height;

        Image {
            id: imgItem;
            smooth: true
            width: 16; height: 16;
            source: container.image;
            x: 8
            y: 2;
        }

        Item {
            width: txtItem.width + 12
            height: txtItem.height
            Text {
                x: 6
                id: txtItem; text: container.text; font.pixelSize: 16; color: activePalette.buttonText
            }
        }
    }

    // Don't make mr part of the size determination, since it uses anchors.fill
    MouseArea { id: mr; anchors.fill: parent; onClicked: container.clicked() }

}
