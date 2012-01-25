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
    property string itemId
    property string itemType : item == undefined? "" : item.type
    property variant item

    onItemIdChanged : {
        if (itemId != "")
            item = calendar.organizer.item(itemId);
    }

    ListView {
        anchors.fill: parent
        clip: true
        spacing: 9.0

        model: {
            if (item == undefined)
                return null;
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
            Text {
                text:"displayLabel: " + detailsView.item.displayLabel
                width: parent.width
                wrapMode : Text.WrapAnywhere
           }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"description: " + detailsView.item.description
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"start: " + detailsView.item.startDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"end: " + detailsView.item.endDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"All day event: " + detailsView.item.allDay
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"location: " + detailsView.item.location
                width: parent.width
                wrapMode : Text.WrapAnywhere
           }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"priority: " + detailsView.item.priority
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"recurrence: " + "*not implemented*"
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"manager: " + detailsView.item.manager
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"id: " + itemId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"type: " + itemType
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"guid: " + detailsView.item.guid
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"created: " + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
    }

    //event occurrence
    VisualItemModel {
        id:eventOccurrenceItemModel

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"displayLabel: " + detailsView.item.displayLabel
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"description: " + detailsView.item.description
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"start: " + detailsView.item.startDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"end: " + detailsView.item.endDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"parent id: " + detailsView.item.parentId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"location: " + detailsView.item.location
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"priority: " + detailsView.item.priority
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"original date: " + detailsView.item.originalDate
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }


        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"manager: " + detailsView.item.manager
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"id: " + itemId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"type: " + itemType
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"guid: " + detailsView.item.guid
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
    }


    //journal
    VisualItemModel {
        id:journalItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"displayLabel: " + detailsView.item.displayLabel
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"description: " + detailsView.item.description
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"date time: " + detailsView.item.startDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"manager: " + detailsView.item.manager
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"id: " + itemId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"type: " + itemType
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"guid: " + detailsView.item.guid
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
    }

    //note
    VisualItemModel {
        id:noteItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"displayLabel: " + detailsView.item.displayLabel
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"description: " + detailsView.item.description
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"manager: " + detailsView.item.manager
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"id: " + itemId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"type: " + itemType
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"guid: " + detailsView.item.guid
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
    }

    //todo
    VisualItemModel {
        id:todoItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"displayLabel: " + detailsView.item.displayLabel
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"description: " + detailsView.item.description
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"start: " + detailsView.item.startDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"due: " + detailsView.item.dueDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"finished: " + detailsView.item.finishedDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"All day event: " + detailsView.item.allDay
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"progress(%): " + detailsView.item.progressPercentage
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"priority: " + detailsView.item.priority
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"status: " + detailsView.item.status
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"recurrence: " + "*not implemented*"
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }


        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"manager: " + detailsView.item.manager
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"id: " + itemId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"type: " + itemType
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"guid: " + detailsView.item.guid
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
    }

    //todo occurrence
    VisualItemModel {
        id:todoOccurrenceItemModel
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"displayLabel: " + detailsView.item.displayLabel
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"description: " + detailsView.item.description
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"start: " + detailsView.item.startDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"due: " + detailsView.item.dueDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"finished: " + detailsView.item.finishedDateTime
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"All day event: " + detailsView.item.allDay
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"progress(%): " + detailsView.item.progressPercentage
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"priority: " + detailsView.item.priority
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"status: " + detailsView.item.status
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"parent id: " + detailsView.item.parentId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"original date: " + detailsView.item.originalDate
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"manager: " + detailsView.item.manager
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"id: " + itemId
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"type: " + itemType
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }

        Rectangle {
            width: detailsView.width
            height: childrenRect.height
            Text {
                text:"guid: " + detailsView.item.guid
                width: parent.width
                wrapMode : Text.WrapAnywhere
            }
        }
    }
}

