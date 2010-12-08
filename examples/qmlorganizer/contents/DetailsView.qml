/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7

import QtMobility.organizer 1.1

Rectangle
{
    id:detailsView
    anchors.fill: parent
    property string  itemId
    property string itemType : item.type
    property variant item

    onItemIdChanged : {
        if (itemId != "")
            item = calendar.organizer.item(itemId);
    }

   // TODOs:
    //  1) add reminder details
    //  2) add recurrences
    //  3) write back

    ListView {
        anchors.fill: parent
        model: {
            switch (itemType) {
                    case "Event" :
                         return eventItemModel;
                    case "EventOccurrence":
                         return eventOccurrenceItemModel;
                    case "Journal":
                         return journalItemModel;
                    case "Note":
                         return noteItemModel;
                    case "Todo":
                         return todoItemModel;
                    case "TodoOccurrence":
                         return todoOccurrenceItemModel;
                    default:
                         break;
            }
            return null;
        }
    }

    //event
    VisualItemModel {
        id:eventItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "displayLabel:" }
                TextInput { text:detailsView.item.displayLabel}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "description:" }
                TextInput { text:detailsView.item.description}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "start:" }
                TextInput { text:detailsView.item.startDateTime}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "end:" }
                TextInput { text:detailsView.item.endDateTime}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "All day event:" }
                TextInput { text:detailsView.item.allDay}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "location:" }
                TextInput { text:detailsView.item.location}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "priority:" }
                TextInput { text:detailsView.item.priority}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "recurrence:" }
                Text { text: "*not implemented*"}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "manager:" }
                Text { text:detailsView.item.manager}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "id:" }
                Text { text: itemId}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "type:" }
                TextInput { text: itemType}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "guid:" }
                TextInput { text:detailsView.item.guid}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "timestamp:" }
                Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}
            }
        }
    }

    //event occurrence
    VisualItemModel {
        id:eventOccurrenceItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "displayLabel:" }
                TextInput { text:detailsView.item.displayLabel}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "displayLabel:" }
                TextInput { text:detailsView.item.displayLabel}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "description:" }
                TextInput { text:detailsView.item.description}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "start:" }
                TextInput { text:detailsView.item.startDateTime}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "end:" }
                TextInput { text:detailsView.item.endDateTime}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "parent id:" }
                TextInput { text:detailsView.item.parentId}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "location:" }
                TextInput { text:detailsView.item.location}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "priority:" }
                TextInput { text:detailsView.item.priority}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "original date:" }
                TextInput { text:detailsView.item.originalDate}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "timestamp:" }
                Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}
            }
        }


        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "manager:" }
                Text { text:detailsView.item.manager}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "id:" }
                Text { text: itemId}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "type:" }
                TextInput { text: itemType}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "guid:" }
                TextInput { text:detailsView.item.guid}
            }
        }
    }

    //journal
    VisualItemModel {
        id:journalItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "displayLabel:" }
                TextInput { text:detailsView.item.displayLabel}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "description:" }
                TextInput { text:detailsView.item.description}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "date time:" }
                TextInput { text:detailsView.item.startDateTime}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "timestamp:" }
                Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "manager:" }
                Text { text:detailsView.item.manager}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "id:" }
                Text { text: itemId}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "type:" }
                TextInput { text: itemType}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "guid:" }
                TextInput { text:detailsView.item.guid}
            }
        }
    }

    //note
    VisualItemModel {
        id:noteItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "displayLabel:" }
                TextInput { text:detailsView.item.displayLabel}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "description:" }
                TextInput { text:detailsView.item.description}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "timestamp:" }
                Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "manager:" }
                Text { text:detailsView.item.manager}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "id:" }
                Text { text: itemId}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "type:" }
                TextInput { text: itemType}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "guid:" }
                TextInput { text:detailsView.item.guid}
            }
        }
    }

    //todo
    VisualItemModel {
        id:todoItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "displayLabel:" }
                TextInput { text:detailsView.item.displayLabel}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "description:" }
                TextInput { text:detailsView.item.description}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "start:" }
                TextInput { text:detailsView.item.startDateTime}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "due:" }
                TextInput { text:detailsView.item.dueDateTime}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "finished:" }
                TextInput { text:detailsView.item.finishedDateTime}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "All day event:" }
                TextInput { text:detailsView.item.allDay}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "progress(%):" }
                TextInput { text:detailsView.item.progressPercentage}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "priority:" }
                TextInput { text:detailsView.item.priority}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "status:" }
                TextInput { text:detailsView.item.status}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "recurrence:" }
                Text { text: "*not implemented*"}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "timestamp:" }
                Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}
            }
        }


        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "manager:" }
                Text { text:detailsView.item.manager}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "id:" }
                Text { text: itemId}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "type:" }
                TextInput { text: itemType}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "guid:" }
                TextInput { text:detailsView.item.guid}
            }
        }
    }

    //todo occurrence
    VisualItemModel {
        id:todoOccurrenceItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "displayLabel:" }
                TextInput { text:detailsView.item.displayLabel}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "description:" }
                TextInput { text:detailsView.item.description}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "start:" }
                TextInput { text:detailsView.item.startDateTime}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "due:" }
                TextInput { text:detailsView.item.dueDateTime}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "finished:" }
                TextInput { text:detailsView.item.finishedDateTime}
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "All day event:" }
                TextInput { text:detailsView.item.allDay}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "progress(%):" }
                TextInput { text:detailsView.item.progressPercentage}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "priority:" }
                TextInput { text:detailsView.item.priority}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "status:" }
                TextInput { text:detailsView.item.status}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "parent id:" }
                TextInput { text:detailsView.item.parentId}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "original date:" }
                TextInput { text:detailsView.item.originalDate}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "timestamp:" }
                Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "manager:" }
                Text { text:detailsView.item.manager}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "id:" }
                Text { text: itemId}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "type:" }
                TextInput { text: itemType}
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Row {
                spacing: 2
                Text { text: "guid:" }
                TextInput { text:detailsView.item.guid}
            }
        }
    }


}
