import Qt 4.6

//Layout of the ServiceList control
//---------------------------------
//|ServiceList                    |
//| ----------------------------- |
//| |Title                      | |
//| ----------------------------- |
//| ----------------------------- |
//| |ListFrame                  | |
//| |-------------------------- | |
//| ||ServiceListView         | | |
//| ||- ListItem              | | |
//| ||- ListItem              | | |
//| ||- ListItem              | | |
//| |---------------------------| |
//| ----------------------------- |
//---------------------------------
    
Rectangle {
    property variant dialService: 0
    signal serviceSelected
    
    Text {
        id: Title
        height: 20
        width: 200
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.leftMargin: 5
        text: "<b>Select dial service:</b>"
    }
    
    Rectangle {
        id : ListFrame
        width: childrenRect.width
        height: childrenRect.height
        anchors.top: Title.bottom
        anchors.left: parent.left;
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        border.color: "black"
        border.width: 3
        property bool nohighlightlistitem : true
        //! [1]
        Component {
            id: Delegate
            //! [1]
            //Rectangle item to draw a list view item
            //This includes 2 line of text:
            // ------------------------------------------
            // |Service: LandDialer (1.0)               |
            // |Interface: com.nokia.qt.examples Dialer |
            // ------------------------------------------
            Rectangle {
                id: ListItem
                width: ServiceListView.width
                height: 40
                border.color: "black"
                border.width: 1
                opacity: 0.6

                //! [2]
                MouseRegion {
                    id: ListItemMouseRegion
                    anchors.fill: parent
                    onClicked: {
                        if(ListFrame.nohighlightlistitem){
                            ServiceListView.highlight = Highlight
                            ListFrame.nohighlightlistitem = false;
                        }
                        ServiceListView.currentIndex = index;
                        dialService = modelData;
                        serviceSelected()
                    }
                }

                Text { 
                    id: ServiceItemInfo
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 5
                    anchors.leftMargin: 3
                    text: " <b>Service:</b> "+ modelData.serviceName + "  ("+modelData.version+")"
                }
                
                Text { 
                    id: ServiceItemInterfaceName
                    anchors.top: ServiceItemInfo.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 2
                    anchors.leftMargin: 3
                    text: " <b>Interface:</b> " + modelData.interfaceName;
                }
                //! [2]
            }
        }
        
        //! [3]
        Component {
            id: Highlight
            
            Rectangle {
                width: childrenRect.width
                border.color: "black"; border.width: 2
                height: 30
                color : "lightsteelblue"
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "steelblue"}
                    GradientStop {position: 0.5; color: "lightsteelblue"}
                    GradientStop {position: 1.0; color: "steelblue"}
                }
            }
        }
        //! [3]
        
        //! [0]
        ListView {
            id: ServiceListView
            height: 100
            width: 260
            anchors.topMargin: 5
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            model: services
            //! [0]
            opacity: 1
            delegate: Delegate
            currentIndex: -1
            clip: true
        }
    }
}