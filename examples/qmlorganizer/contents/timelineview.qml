import Qt 4.7
import "timeline.js" as Timeline

Rectangle {
    id : timelineView
    anchors.fill : parent
    opacity : parent.opacity

    //Day view
    Rectangle {
        id: dayView
        anchors.right : parent.right
        anchors.left : monthView.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        opacity : parent.opacity

        ListView {
            id : dayList
            model : dayModel
            anchors.fill: parent
            clip: true
            focus: true
            opacity : parent.opacity

            delegate : dayDelegate
            highlight: dayHighlight
            preferredHighlightBegin: dayList.height * 0.5
            preferredHighlightEnd: preferredHighlightBegin
            highlightFollowsCurrentItem : true
            highlightMoveSpeed : 2000
            keyNavigationWraps : true

            Component.onCompleted : Timeline.changeToday()
            onOpacityChanged : Timeline.changeToday()
            Keys.onUpPressed : Timeline.changeDate()
            Keys.onDownPressed : Timeline.changeDate()
        }

        Component {
            id: dayHighlight
            Rectangle {
                width: dayList.width; height: dayList.height /7 ; color: "lightsteelblue" ;radius: 5
            }
        }

        Component {
            id: dayDelegate
            Item {
                width : dayList.width
                height : dayList.height / 7
                Column {
                    Rectangle {
                        height : 1
                        width : dayList.width
                        color : "black"
                    }
                    Text {
                        text: day
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked : dayList.currentIndex = index
                }
            }
        }

        ListModel {
            id : dayModel
            ListElement {day : "1"}
            ListElement {day : "2"}
            ListElement {day : "3"}
            ListElement {day : "4"}
            ListElement {day : "5"}
            ListElement {day : "6"}
            ListElement {day : "7"}
            ListElement {day : "8"}
            ListElement {day : "9"}
            ListElement {day : "10"}
            ListElement {day : "11"}
            ListElement {day : "12"}
            ListElement {day : "13"}
            ListElement {day : "14"}
            ListElement {day : "15"}
            ListElement {day : "16"}
            ListElement {day : "17"}
            ListElement {day : "18"}
            ListElement {day : "19"}
            ListElement {day : "20"}
            ListElement {day : "21"}
            ListElement {day : "22"}
            ListElement {day : "23"}
            ListElement {day : "24"}
            ListElement {day : "25"}
            ListElement {day : "26"}
            ListElement {day : "27"}
            ListElement {day : "28"}
            ListElement {day : "29"}
            ListElement {day : "30"}
            ListElement {day : "31"}
        }
    }



    //Month view
    Rectangle {
        id:monthView
        color : "lightgray"
        width : 30
        anchors.left : yearView.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom

        ListView {
            id : monthList
            model : monthModel
            anchors.fill: parent
            clip: true
            delegate : monthDelegate
            highlight: monthHighlight
            preferredHighlightBegin: monthList.height * 0.5
            preferredHighlightEnd: preferredHighlightBegin
            highlightFollowsCurrentItem : true
            highlightMoveSpeed : 1000
            Component.onCompleted : {
               var now = new Date();
               var month = now.getMonth();
               monthList.currentIndex = month;
               var  d = Date.parse("Feb 31, 2010");
            }
//            onCurrentIndexChanged : {

//            }


        }

        Component {
            id: monthHighlight
            Rectangle {
                width: monthList.width; height: monthList.height / 12 ; color: "lightsteelblue" ;radius: 5
            }
        }

        Component {
            id: monthDelegate
            Item {
                width : monthList.width
                height : monthList.height / 12
                Column {
                    Rectangle {
                        height : 1
                        width : monthList.width
                        color : "black"
                    }
                    Text {
                        text: month
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked : monthList.currentIndex = index
                }
            }
        }

        ListModel {
            id : monthModel
            ListElement {month : "Jan"}
            ListElement {month : "Feb"}
            ListElement {month : "Mar"}
            ListElement {month : "Apr"}
            ListElement {month : "May"}
            ListElement {month : "Jun"}
            ListElement {month : "Jul"}
            ListElement {month : "Aug"}
            ListElement {month : "Sep"}
            ListElement {month : "Oct"}
            ListElement {month : "Nov"}
            ListElement {month : "Dec"}
        }
    }

    //Year view
    Rectangle {
        id:yearView
        color : "gray"
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        anchors.left : parent.left
        width : 50


        Component {
            id: yearHighlight
            Rectangle {
                width: yearList.width; height: yearList.height / 10; color: "lightsteelblue" ;radius: 5
            }
        }

        Component {
            id: yearDelegate

            Item {
                width : yearList.width
                height : yearList.height / 10
                Column {
                    Rectangle {
                        height : 1
                        width : yearList.width
                        color : "black"
                    }
                    Text {
                        text: year
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked : yearList.currentIndex = index
                }
            }
        }


        ListView {
            id : yearList
            model : yearModel
            delegate : yearDelegate
            anchors.fill: parent
            clip: true
            highlight: yearHighlight

            preferredHighlightBegin: yearList.height * 0.5
            preferredHighlightEnd: preferredHighlightBegin
            highlightRangeMode: "StrictlyEnforceRange"
            highlightFollowsCurrentItem : true

            Component.onCompleted: Timeline.extendYearModel(true);
            onCurrentIndexChanged: Timeline.extendYearModel(false);
        }

        ListModel {
            id : yearModel
            property int start : 0;
            property int end : 0;

            ListElement {
                year : 0;
            }
        }
    }

}
