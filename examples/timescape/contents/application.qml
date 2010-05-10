import Qt 4.6
import "qrc:/contents/components" as TimeScape

Rectangle {
    id: topItem
    width: 360
    height: 640
    x: 0
    y: 0

    color: "#080808";

    SystemPalette { id: activePalette }

    TimeScape.Button {
        anchors.centerIn: parent;
        text: "I'm a button";
    }

}
