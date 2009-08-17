import Qt 4.6


Item {
   id: DialButton
   property alias buttonText: InnerText.text;
   property alias color: buttonRect.color
   property color hoverColor: "lightsteelblue"
   property color pressColor: "slategray"
   signal clicked
   width: 50
   height: 50

   Rect {
       id: buttonRect
       anchors.fill: parent
       radius: 5
       border.width: 3
       color: "steelblue"
       border.color: "black"
       Text {
           id: InnerText
           text: DialButton.text
           font.size: 20
           anchors.centerIn: parent
       }
   }
   states: [
       State {
           name: "Hovering"
           SetProperties {
               target: buttonRect
               color: hoverColor
           }
       },
       State {
           name: "Pressed"
           SetProperties {
               target: buttonRect
               color: pressColor
           }
       }
   ]
   transitions: [
       Transition {
           fromState: ""; toState: "Hovering"
           ColorAnimation { duration: 100 }
       },
       Transition {
           fromState: "*"; toState: "Pressed"
           ColorAnimation { duration: 10 }
       }
   ]
   MouseRegion {
       anchors.fill: DialButton
       onEntered: { DialButton.state='Hovering'}
       onExited: { DialButton.state=''}
       onClicked: { DialButton.clicked();}
       onPressed: { DialButton.state="Pressed" }
       onReleased: {
           if (containsMouse)
               DialButton.state="Hovering";
           else
               DialButton.state="";
       }
   }
}
