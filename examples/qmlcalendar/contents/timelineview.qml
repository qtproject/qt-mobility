import Qt 4.7


Rectangle {
    id : timelineView
    anchors.fill : parent
//    width:300
//    height:400


    //Day view
    Rectangle {
        id: dayView
        anchors.right : parent.right
        anchors.left : monthView.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom

        ListView {
            id : dayList
            model : dayModel
            anchors.fill: parent
            clip: true
            focus: true

            delegate : dayDelegate
            highlight: dayHighlight
            preferredHighlightBegin: dayList.height * 0.5
            preferredHighlightEnd: preferredHighlightBegin
            highlightFollowsCurrentItem : true
            Component.onCompleted : {
               var now = new Date();
               var day = now.getUTCDate();
               dayList.positionViewAtIndex(day, Center);
               dayList.currentIndex = day;
            }
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
            Component.onCompleted : {
               var now = new Date();
               var month = now.getMonth();
               monthList.currentIndex = month;
            }

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


        Script {
            function extendYearModel(init) {

                var start = yearModel.start;
                var end = yearModel.end;
                var now = new Date();
                var year = 1900 + now.getYear();

                if (init) {
                    //initializes the year model
                    if (yearModel.count == 1) {
                        yearModel.set(0, {"year" : year});
                        start = year;
                        end = year;
                    }
                }

                if (start == 0) return;

                //extends forward
                if (yearList.currentIndex == yearList.count - 1) {
                    for (var i = 0; i < 10; i ++) {
                        end++;
                        yearModel.append({"year" : end});
                    }
                }


                //extends backward
                if (yearList.currentIndex == 0) {
                    if (start == 1900)
                         break;
                    for (var i = 0; i < 10; i ++) {
                        start--;
                        yearModel.insert(1, {"year" : start});
                    }
                    yearModel.move(0, 10, 1);
                }
                yearModel.start = start;
                yearModel.end = end;
                if (init) {
                    yearList.currentIndex = year - start;
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

            Component.onCompleted: extendYearModel(true);
            onCurrentIndexChanged: extendYearModel(false);
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
