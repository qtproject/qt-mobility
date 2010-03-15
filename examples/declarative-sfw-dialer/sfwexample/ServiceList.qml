import Qt 4.6
import QtSFW 1.0

//Layout of the ServiceList control
//---------------------------------
//|ServiceList                    |
//| ----------------------------- |
//| |title                      | |
//| ----------------------------- |
//| ----------------------------- |
//| |listFrame                  | |
//| |-------------------------- | |
//| ||serviceListView         | | |
//| ||- listItem              | | |
//| ||- listItem              | | |
//| ||- listItem              | | |
//| |---------------------------| |
//| ----------------------------- |
//---------------------------------
    
Rectangle {
    property variant dialService: 0
    signal serviceSelected
    
    Text {
        id: title
        height: 20
        width: 200
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.leftMargin: 5
        text: "<b>Select dial service:</b>"
    }
    
    Rectangle {
        id : listFrame
        width: childrenRect.width
        height: childrenRect.height
        anchors.top: title.bottom
        anchors.left: parent.left;
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        border.color: "black"
        border.width: 3
        property bool nohighlightlistitem : true
        //! [1]
        Component {
            id: delegate
            //! [1]
            //Rectangle item to draw a list view item
            //This includes 2 line of text:
            // ------------------------------------------
            // |Service: LandDialer (1.0)               |
            // |Interface: com.nokia.qt.examples Dialer |
            // ------------------------------------------
            Rectangle {
                id: listItem
                width: serviceListView.width
                height: 40
                border.color: "black"
                border.width: 1
                opacity: 0.6

                //! [2]
                MouseRegion {
                    id: listItemMouseRegion
                    anchors.fill: parent
                    onClicked: {
                        if(listFrame.nohighlightlistitem){
                            serviceListView.highlight = highlight
                            listFrame.nohighlightlistitem = false;
                        }
                        serviceListView.currentIndex = index;
                        dialService = model.modelData;
                        serviceSelected()
                    }
                }

                Text { 
                    id: serviceItemInfo
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 5
                    anchors.leftMargin: 3
                    text: " <b>Service:</b> " + serviceName + "  (" + version + ")"
                }
                
                Text { 
                    id: serviceItemInterfaceName
                    anchors.top: serviceItemInfo.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 2
                    anchors.leftMargin: 3
                    text: " <b>Interface:</b> " + interfaceName;
                }
                //! [2]
            }
        }
        
        //! [3]
        Component {
            id: highlight
            
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
            id: serviceListView
            height: 100
            width: 260
            anchors.topMargin: 5
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            model: services.registeredservices
         //! [0]
            opacity: 1
            delegate: delegate
            currentIndex: -1
            clip: true
        }
    }

    Services {
        id: services
    }
}
