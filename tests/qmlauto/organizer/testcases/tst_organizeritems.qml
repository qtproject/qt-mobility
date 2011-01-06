
/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import QtQuickTest 1.0
import QtMobility.organizer 1.1

TestCase {
    name: "OrganizerItemTests"
    OrganizerModel {
      id:model
      manager:"memory"
      startPeriod:'2009-01-01'
      endPeriod:'2012-12-31'
      autoUpdate:false
    }

    Event { id: event }
    Todo {  id:todo   }
    Journal { id:journal  }
    Note {  id:note }

    DetailFilter {
        id:detailFilter
        detail:Detail.DisplayLabel
        field:DisplayLabel.Label
    }

    Event {
        id:newEvent1
        displayLabel: "NewEvent1"
        description: "This is a new created event 1"
        startDateTime: '2010-12-12'
        endDateTime: '2010-12-13'
    }

    Event {
        id:newEvent2
        displayLabel: "NewEvent2"
        description: "This is a new created event 2"
        startDateTime: '2010-12-13'
        endDateTime: '2010-12-14'
    }

    function test_event() {
        verify(!event.modified);
        verify(event.type == "Event");
        event.location = "test address";
        verify(event.location == "test address");
        verify(event.modified);
        event.startDateTime = new Date();
        verify(event.startDateTime, new Date());

        event.endDateTime = new Date();
        verify(event.endDateTime, new Date());

        verify(!event.allDay);
        event.allDay = true;
        verify(event.allDay);

        event.priority = Priority.VeryHigh;
        verify(event.priority == Priority.VeryHigh);
        event.priority = Priority.VeryLow;
        verify(event.priority == Priority.VeryLow);

        verify(event.recurrence, undefined);
        event.recurrence.recurrenceDates = [new Date(2010, 11, 12), new Date(2010, 11, 14)];
        verify(event.recurrence.recurrenceDates.length == 2);
        verify(event.recurrence.recurrenceDates[0].getMonth() == 11);
        verify(event.recurrence.recurrenceDates[0].getFullYear() == 2010);
        verify(event.recurrence.recurrenceDates[0].getDate() == 12);
        verify(event.recurrence.recurrenceDates[1].getDate() == 14);

        event.recurrence.exceptionDates =event.recurrence.recurrenceDates;
        event.recurrence.recurrenceDates = [];
        verify (event.recurrence.exceptionDates.length == 2);
        verify(event.recurrence.recurrenceDates.length === 0);
        verify(event.recurrence.exceptionDates[0].getMonth() == 11);
        verify(event.recurrence.exceptionDates[0].getFullYear() == 2010);
        verify(event.recurrence.exceptionDates[0].getDate() == 12);
        verify(event.recurrence.exceptionDates[1].getDate() == 14);

        verify (event.recurrence.recurrenceRules.length === 0);
        verify (event.recurrence.exceptionRules.length === 0);
        var rrule = Qt.createQmlObject("import QtMobility.organizer 1.1\nRecurrenceRule {}", event.recurrence);
        event.recurrence.recurrenceRules = [rrule];
        verify (event.recurrence.recurrenceRules.length == 1);

        verify(model.itemCount === 0);
        model.saveItem(newEvent1) ;
        //make sure event saved
        wait(100);
        verify(model.itemCount === 1);
        var ids = model.itemIds();
        verify(ids.length === 1);
        var item = model.item(ids[0]);
        verify(item.displayLabel === "NewEvent1");
        verify(item.description === "This is a new created event 1");

        //modify
        item.displayLabel = "EditedEvent1";
        model.saveItem(item);
        //make sure event saved
        wait(100);
        verify(model.itemCount === 1);
        var item2 = model.item(ids[0]);
        verify(item2.displayLabel === "EditedEvent1");

        //filter
        model.saveItem(newEvent2);
        wait(100);
        verify(model.itemCount === 2);
        model.autoUpdate = true;
        detailFilter.value = "New"
        detailFilter.matchFlags = Filter.MatchStartsWith;
        model.filter = detailFilter;
        wait(100);
        verify(model.itemCount === 1); //only NewEvent2 matched

        model.autoUpdate = false;
        detailFilter.value = "Event"
        detailFilter.matchFlags = Filter.MatchContains;
        wait(100);
        verify(model.itemCount === 1); //not updated yet
        model.update();
        wait(100);
        verify(model.itemCount === 2); //both events matched now

    }

    function test_todo() {
    }

    function test_journal() {
    }

    function test_note() {
    }

    function test_eventOccurrence() {
    }

    function test_todoOccurrence() {
    }

}
