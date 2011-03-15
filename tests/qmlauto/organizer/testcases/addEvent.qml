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
import QtMobility.organizer 1.1

OrganizerModel {
    id:myModel;

    property Event myEvent : Event {id:myEvent; startDateTime: new Date(); endDateTime: new Date() }
    property RecurrenceRule myReccuranceRule: RecurrenceRule {
                                                    id: myReccuranceRule
                                                    frequency: RecurrenceRule.Monthly
                                                    daysOfWeek:[Qt.Friday]
                                                    limit: 2
                                                }
    property bool testComplete : false

    function addEvents(testEvents)
    {
        for (var index=0; index < testEvents.length; index++){
            var event = testEvents[index]
            console.log("Creating event " + event.subject)
            myModel.myEvent.description = event.subject
            myModel.myEvent.startDateTime = event.startDateTime
            myModel.myEvent.endDateTime = event.endDateTime

            // only support testing simple RecurrenceRules ATM
            if (event.repeat){
                myReccuranceRule.frequency = event.repeat
                myReccuranceRule.limit = event.repeatCount
                myReccuranceRule.daysOfWeek = event.repeatDays
                myModel.myEvent.recurrence.recurrenceRules = [myReccuranceRule]
            }

            // TODO verify that the OrganizerModel::itemsSaved() signal is fired
            saveItem(myModel.myEvent)
        }
        update()
        myModel.testComplete = (items.length == testEvents.length)
    }

    function testEvents()
    {
        return myModel.items
    }
}

