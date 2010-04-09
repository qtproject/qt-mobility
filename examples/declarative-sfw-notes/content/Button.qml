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
            color :if (mr.pressed) { activePalette.dark } else { activePalette.light } }
        GradientStop { position: 1.0; color: activePalette.button }
    }

    Image {
        id: imgItem;
        smooth: true
        width: 16; height: 16;
        source: container.image; anchors.centerIn: container
    }

    MouseArea { id: mr; anchors.fill: parent; onClicked: container.clicked() }

    Text {
        id: txtItem; text: container.text; anchors.centerIn: container; color: activePalette.buttonText
    }
}
