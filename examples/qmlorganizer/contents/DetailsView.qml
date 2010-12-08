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
    property OrganizerItem item

    onItemIdChanged : {
        if (itemId != "")
            item = calendar.organizer.item(itemId);
    }

    color: "#f1f727"
    border.color: "#d5cbcb"
    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#f1f727";
        }
        GradientStop {
            position: 1.00;
            color: "#ffffff";
        }
    }

   // TODOs:
    //  1) add reminder details
    //  2) add recurrences
    //  3) write back


    //event
    Rectangle {
        id:eventItem
        opacity: itemType == "Event" ? 1: 0

        Grid {
            columns: 2
            spacing: 2
            Text { text: "displayLabel:" }
            TextInput { text:detailsView.item.displayLabel}

            Text { text: "description:" }
            TextInput { text:detailsView.item.description}

            Text { text: "start:" }
            TextInput { text:detailsView.item.startDateTime}

            Text { text: "end:" }
            TextInput { text:detailsView.item.endDateTime}

            Text { text: "All day event:" }
            TextInput { text:detailsView.item.allDay}

            Text { text: "location:" }
            TextInput { text:detailsView.item.location}

            Text { text: "priority:" }
            TextInput { text:detailsView.item.priority}

            Text { text: "recurrence:" }
            Text { text: "*not implemented*"}

            Text { text: "manager:" }
            Text { text:detailsView.item.manager}

            Text { text: "id:" }
            Text { text: itemId}

            Text { text: "type:" }
            TextInput { text: itemType}

            Text { text: "guid:" }
            TextInput { text:detailsView.item.guid}

            Text { text: "timestamp:" }
            Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}

        }
    }

    //event occurrence
    Rectangle {
        id:eventOccurrenceItem
        opacity: itemType == "EventOccurrence" ? 1: 0

        Grid {
            columns: 2
            spacing: 2
            Text { text: "displayLabel:" }
            TextInput { text:detailsView.item.displayLabel}

            Text { text: "description:" }
            TextInput { text:detailsView.item.description}

            Text { text: "start:" }
            TextInput { text:detailsView.item.startDateTime}

            Text { text: "end:" }
            TextInput { text:detailsView.item.endDateTime}

            Text { text: "parent id:" }
            TextInput { text:detailsView.item.parentId}

            Text { text: "location:" }
            TextInput { text:detailsView.item.location}

            Text { text: "priority:" }
            TextInput { text:detailsView.item.priority}

            Text { text: "original date:" }
            TextInput { text:detailsView.item.originalDate}

            Text { text: "timestamp:" }
            Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}


            Text { text: "manager:" }
            Text { text:detailsView.item.manager}

            Text { text: "id:" }
            Text { text: itemId}

            Text { text: "type:" }
            TextInput { text: itemType}

            Text { text: "guid:" }
            TextInput { text:detailsView.item.guid}
        }
    }

    //journal
    Rectangle {
        id:journalItem
        opacity: itemType == "Journal" ? 1: 0

        Grid {
            columns: 2
            spacing: 2
            Text { text: "displayLabel:" }
            TextInput { text:detailsView.item.displayLabel}

            Text { text: "description:" }
            TextInput { text:detailsView.item.description}

            Text { text: "date time:" }
            TextInput { text:detailsView.item.startDateTime}

            Text { text: "timestamp:" }
            Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}


            Text { text: "manager:" }
            Text { text:detailsView.item.manager}

            Text { text: "id:" }
            Text { text: itemId}

            Text { text: "type:" }
            TextInput { text: itemType}

            Text { text: "guid:" }
            TextInput { text:detailsView.item.guid}
        }
    }

    //note
    Rectangle {
        id:noteItem
        opacity: itemType == "Note" ? 1: 0

        Grid {
            columns: 2
            spacing: 2
            Text { text: "displayLabel:" }
            TextInput { text:detailsView.item.displayLabel}

            Text { text: "description:" }
            TextInput { text:detailsView.item.description}

            Text { text: "timestamp:" }
            Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}


            Text { text: "manager:" }
            Text { text:detailsView.item.manager}

            Text { text: "id:" }
            Text { text: itemId}

            Text { text: "type:" }
            TextInput { text: itemType}

            Text { text: "guid:" }
            TextInput { text:detailsView.item.guid}
        }
    }

    //todo
    Rectangle {
        id:todoItem
        opacity: itemType == "Todo" ? 1: 0

        Grid {
            columns: 2
            spacing: 2
            Text { text: "displayLabel:" }
            TextInput { text:detailsView.item.displayLabel}

            Text { text: "description:" }
            TextInput { text:detailsView.item.description}

            Text { text: "start:" }
            TextInput { text:detailsView.item.startDateTime}

            Text { text: "due:" }
            TextInput { text:detailsView.item.dueDateTime}

            Text { text: "finished:" }
            TextInput { text:detailsView.item.finishedDateTime}

            Text { text: "All day event:" }
            TextInput { text:detailsView.item.allDay}

            Text { text: "progress(%):" }
            TextInput { text:detailsView.item.progressPercentage}

            Text { text: "priority:" }
            TextInput { text:detailsView.item.priority}

            Text { text: "status:" }
            TextInput { text:detailsView.item.status}

            Text { text: "recurrence:" }
            Text { text: "*not implemented*"}

            Text { text: "timestamp:" }
            Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}


            Text { text: "manager:" }
            Text { text:detailsView.item.manager}

            Text { text: "id:" }
            Text { text: itemId}

            Text { text: "type:" }
            TextInput { text: itemType}

            Text { text: "guid:" }
            TextInput { text:detailsView.item.guid}
        }
    }

    //todo occurrence
    Rectangle {
        id:todoOccurrenceItem
        opacity: itemType == "TodoOccurrence" ? 1: 0

        Grid {
            columns: 1
            spacing: 2

            Text { text: "displayLabel:" }
            TextInput { text:detailsView.item.displayLabel}

            Text { text: "description:" }
            TextInput { text:detailsView.item.description}

            Text { text: "start:" }
            TextInput { text:detailsView.item.startDateTime}

            Text { text: "due:" }
            TextInput { text:detailsView.item.dueDateTime}

            Text { text: "finished:" }
            TextInput { text:detailsView.item.finishedDateTime}

            Text { text: "All day event:" }
            TextInput { text:detailsView.item.allDay}

            Text { text: "progress(%):" }
            TextInput { text:detailsView.item.progressPercentage}

            Text { text: "priority:" }
            TextInput { text:detailsView.item.priority}

            Text { text: "status:" }
            TextInput { text:detailsView.item.status}

            Text { text: "parent id:" }
            TextInput { text:detailsView.item.parentId}

            Text { text: "original date:" }
            TextInput { text:detailsView.item.originalDate}

            Text { text: "timestamp:" }
            Text { text: "created:" + detailsView.item.timestamp.created + "lastModified:" + detailsView.item.timestamp.lastModified}

            Text { text: "manager:" }
            Text { text:detailsView.item.manager}

            Text { text: "id:" }
            Text { text: itemId}

            Text { text: "type:" }
            TextInput { text: itemType}

            Text { text: "guid:" }
            TextInput { text:detailsView.item.guid}
        }
    }


}
