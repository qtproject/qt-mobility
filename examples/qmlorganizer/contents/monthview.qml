import Qt 4.7
import "month.js" as Month
Item {
    id:monthView
    property int month: 9
    property int year: 2010
    property date startDay:new Date(year, month, 1)
    property int startWeekday:startDay.getDay()
    property int today

    anchors.fill: parent

    Component.onCompleted : {
    }

    Grid {
        id:container
        anchors.fill: parent
        columns: 7
        Repeater {
            model:["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
            Rectangle { width: container.width / 7
                        height: container.height / 7
                        color: "lightgray"
                        border.color: "#3f4947"
                        Text { text: modelData
                               font.pointSize: 10
                               anchors.centerIn: parent
                        }
            }
        }

        Repeater { model: 42
                   Rectangle { width: container.width / 7
                               height: container.height / 7
                               color: Month.getColorOfDay(startDay,   index - startWeekday +1)
                               border.color: "black"
                               Text { text: Month.getDayOfMonth(startDay,   index - startWeekday +1)
                                      font.pointSize: 10
                                      anchors.centerIn: parent
                               }
                   }
        }
    }
}
