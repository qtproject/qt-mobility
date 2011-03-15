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

//TESTED_COMPONENT=src/organizer

import QtQuick 1.0
import QtQuickTest 1.0
import QtMobility.organizer 1.1

TestCase {
    name: "ModelTests"
    id:modelTests

    property Rectangle rect: Rectangle {
        id:myRectangle
    }

    function test_componentCreation_data() {
        return [
                // OrganizerModel
                {tag: "No properties",
                 code: "import QtQuick 1.0\n"
                    + "import QtMobility.organizer 1.1 \n"
                    + "   OrganizerModel {\n"
                    + "   }"
                },
                {tag: "Only id property",
                 code: "import QtQuick 1.0\n"
                   + "import QtMobility.organizer 1.1\n"
                   + "    OrganizerModel {\n"
                   + "        id:organizerModelId\n"
                   + "     }\n"
                },
                {tag: "Valuetype properties",
                 code: "import QtQuick 1.0\n"
                   + "import QtMobility.organizer 1.1\n"
                   + "    OrganizerModel {\n"
                   + "        id:organizerModelId\n"
                   + "        manager:'memory'\n"
                   + "        startPeriod:'2010-08-12T13:22:01'\n"
                   + "        endPeriod:'2010-09-12T13:22:01'\n"
                   + "     }\n"
                },
                {tag: "With filter",
                 code: "import QtQuick 1.0\n"
                   + "import QtMobility.organizer 1.1\n"
                   + "OrganizerModel {\n"
                   + "    id:organizerModelId\n"
                   + "    filter:DetailFilter{\n"
                   + "        id:filter\n"
                   + "        field:EventTime.StartDateTime\n"
                   + "        value:'2010-08-12T13:22:01'\n"
                   + "    }\n"
                   + "}\n"
                },
                {tag: "With invalid filter",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerModel {\n"
                    + "   id:organizerModelId\n"
                    + "   filter:InvalidFilter{\n"
                    + "       id:filter\n"
                    + "   }\n"
                    + "}\n"
                },
                {tag: "With range filter",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerModel {\n"
                    + "   id:organizerModelId\n"
                    + "   filter:DetailRangeFilter{\n"
                    + "       id:filter\n"
                    + "       field:EventTime.StartDateTime\n"
                    + "       min:'2010-08-12T13:22:01'\n"
                    + "       max:'2010-09-12T13:22:01'\n"
                    + "   }\n"
                    + "}\n"
                },
                {tag: "With changelog filter",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerModel {\n"
                    + "   id:organizerModelId\n"
                    + "   filter:ChangeLogFilter{\n"
                    + "       id:filter\n"
                    + "       eventType:ChangeLogFilter.EventAdded\n"
                    + "       since:'2010-08-12T13:22:01'\n"
                    + "   }\n"
                    + "}\n"
                },
                // Reserved for future use
                /*
                {tag: "With action filter",
                code: "import Qt 4.7\n"
                   + "import QtMobility.organizer 1.1\n"
                   + "OrganizerModel {\n"
                   + "   id:organizerModelId\n"
                   + "   filter:ActionFilter{\n"
                   + "       id:filter\n"
                   + "       actionName:'SomeAction'\n"
                   + "   }\n"
                   + "}\n"
                },
                */
                {tag: "With collection filter",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerModel {\n"
                    + "   id:organizerModelId\n"
                    + "   filter:CollectionFilter{\n"
                    + "       id:filter\n"
                    + "       ids:['1234', '456', '90']\n"
                    + "   }\n"
                    + "}\n"
                },
                {tag: "With intersection filter",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerModel {\n"
                    + "   id:organizerModelId\n"
                    + "   filter:IntersectionFilter {\n"
                    + "       id:filter\n"
                    + "       filters:[\n"
                    + "           DetailFilter{\n"
                    + "               id:filter1\n"
                    + "               field:EventTime.StartDateTime\n"
                    + "               value:'2010-08-12T13:22:01'\n"
                    + "           },\n"
                    + "           DetailRangeFilter{\n"
                    + "               id:filter2\n"
                    + "               field:EventTime.StartDateTime\n"
                    + "               min:'2010-08-12T13:22:01'\n"
                    + "               max:'2010-09-12T13:22:01'\n"
                    + "           }\n"
                    + "      ]\n"
                    + "   }\n"
                    + "}\n"
                },
                {tag: "With fetchHint",
                 code: "import Qt 4.7 \n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerModel {\n"
                    + "    id:organizerModelId\n"
                    + "    fetchHint:FetchHint {\n"
                    + "        id:hint\n"
                    + "        optimizationHints:FetchHint.AllRequired\n"
                    + "    }\n"
                    + "}\n"
                },

                // Organizer Items
                {tag: "Base organizer item",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerItem {\n"
                    + "}\n"
                },
                {tag: "Base organizer item: only id",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerItem {\n"
                    + "    id:organizerItem\n"
                    + "}\n"
                },
                {tag: "Base organizer item: Valuetype properties",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerItem {\n"
                    + "    id:organizerItem\n"
                    + "    displayLabel:'test item'\n"
                    + "    description:'item description'\n"
                    + "    guid:'1112232133'\n"
                    + "}\n"
                },
                {tag: "Base organizer item: default property",
                 code: "import Qt 4.7\n"
                    + "import QtMobility.organizer 1.1\n"
                    + "OrganizerItem {\n"
                    + "    id:organizerItem\n"
                    + "    DisplayLabel {\n"
                    + "        label:'test item'\n"
                    + "    }\n"
                    + "    Description {\n"
                    + "        description:'item description'\n"
                    + "    }\n"
                    + "    Guid{\n"
                    + "        guid:'111223213'\n"
                    + "    }\n"
                    + "}\n"
                },

                //Event
                {tag: "Organizer event",
                 code: "import Qt 4.7 \n"
                    + "import QtMobility.organizer 1.1\n"
                    + "Event {\n"
                    + "}\n"
                },
                {tag: "Organizer event:Valuetype properties",
                 code: "import Qt 4.7 \n"
                    + "import QtMobility.organizer 1.1\n"
                    + "Event {\n"
                    + "    id:organizerEvent\n"
                    + "    displayLabel:'meeting'\n"
                    + "    startDateTime:'2010-08-12T13:00:00'\n"
                    + "    endDateTime:'2010-08-12T15:00:00'\n"
                    + "    allDay:false\n"
                    + "    location:'office'\n"
                    + "    Location {\n"
                    + "        label:'53 Brandl st'\n"
                    + "        latitude:-27.579570\n"
                    + "        longitude:153.10031\n"
                    + "    }\n"
                    + "    priority:Priority.Low\n"
                    + "    recurrence.recurrenceRules:[\n"
                    + "        RecurrenceRule {\n"
                    + "        }\n"
                    + "    ]\n"
                    + "    recurrence.recurrenceDates:[]\n"
                    + "    recurrence.exceptionDates:[]\n"
                    + "}\n"
                },
                /*
                {tag: "",
                 code: ""
                },
                */

        ]
    }

    function test_componentCreation(data)
    {
        try{
            var obj = Qt.createQmlObject(data.code, myRectangle, "dynamicSnippet1");
            verify(obj != undefined, "Unable to load script for " + data.tag);
            obj.destroy();
            console.log("Completed test on '" + data.tag + "'")
        }
        catch (errorObject)  {
            console.log("For code " + data.code + " Error was seen is " +  errorObject );
        }

    }

    function checkDetails(events)
    {
        var testDataEvents = test_addEvent_data()[0].events
        var foundEvent = false
        var testDataEvent = undefined
        for (var index = 0; index < testDataEvents.length; index++){
            testDataEvent = testDataEvents[index]
            foundEvent = false
            console.log("Checking event " + testDataEvent.subject)
            for (var index2 = 0; index2 < events.length; index2++){
                var actualEvent = events[index2];
                if (testDataEvent.subject == actualEvent.description){
                    foundEvent = true
                    compare(actualEvent.type, testDataEvent.type)
                    compare(actualEvent.startDateTime.toString(), testDataEvent.startDateTime.toString())
                    compare(actualEvent.endDateTime.toString(), testDataEvent.endDateTime.toString())
                    if (testDataEvent.repeat != undefined){
                        verify(actualEvent.recurrence != undefined, "Expected recurrance Element to be present for event")
                        verify(actualEvent.recurrence.recurrenceRules.length == 1, "Expected there to be one RecurranceRule")
                        var recurranceRule = actualEvent.recurrence.recurrenceRules[0] // first rule only
                        switch (event.repeat){
                        case RecurrenceRule.Daily:
                        case RecurrenceRule.Weekly:
                        case RecurrenceRule.Monthly:
                        case RecurrenceRule.Yearly:
                            compare(recurranceRule.frequency, RecurrenceRule.Weekly)
                            for (var dayIndex=0; dayIndex < testDataEvent.repeatDays; dayIndex++){
                                verify(recurranceRule.daysOfWeek.indexOf(testDataEvent.repeatDays[dayIndex]) != -1, "Expected event to recurrence on day " + testDataEvent.repeatDays[dayIndex])
                            }
                            compare(recurranceRule.limit.toString(), testDataEvent.limit.toString())
                        break
                        default:
                            if (event.repeat != RecurrenceRule.Invalid)
                                console.log("Unsupported repeat for Event actual:" + actualEvent.reoccurance + " expected:" + testDataEvent.repeat);
                        }
                    }
                    break
                }
            }
            verify(foundEvent, "Did not find event " + testDataEvent.subject)
        }
        console.log("Done checking Event")
    }

    // test data is handled by addEvent.qml function addEvents()
    function test_addEvent_data() {
        return [
            {tag: "Event set 1",
                events:[
                    {tag: "Event 1",
                     type: 'Event',
                     subject: "Event 1",
                     description: "starts 2010-12-09 8AM finishes 5PM",
                     startDateTime: new Date(2010, 12, 9, 8, 0),
                     endDateTime: new Date(2010, 12, 9, 17, 0),
                    },
                    {tag: "Event 2",
                     type: 'Event',
                     subject: "Event 2",
                     description: "starts 2010-12-08 8AM finishes 1PM",
                     startDateTime: new Date(2010, 12, 8, 8, 0),
                     endDateTime: new Date(2010, 12, 8, 13, 0),
                    },
                    {tag: "Event 3",
                     type: 'Event',
                     subject: "Event 3",
                     description: "starts a month from 2010-12-08 at 11AM finish at 2PM",
                     startDateTime: new Date(2010, 12, 8, 11, 0),
                     endDateTime: new Date(2010, 12, 8, 14, 0),
                    },
                    {tag: "Event 4",
                     type: 'Event',
                     subject: "Event 4",
                     description: "starts after Event 3 and finishes 4PM",
                     startDateTime: new Date(2010, 12, 8, 14, 0),
                     endDateTime: new Date(2010, 12, 8, 16, 0),
                    },
                    {tag: "Event 5",
                     type: 'Event',
                     subject: "Event 5",
                     description: "starts after Event 4 and finishes 5PM",
                     startDateTime: new Date(2010, 12, 8, 16, 0),
                     endDateTime: new Date(2010, 12, 8, 17, 0),
                    },
                    {tag: "Event 6",
                     type: 'Event',
                     subject: "Event 6",
                     description: "starts 2010-12-10 at 11AM finishing 1PM, repeating for 4 weeks",
                     startDateTime: new Date(2010, 12, 10, 11, 0),
                     endDateTime: new Date(2010, 12, 10, 13, 0),
                     repeat: RecurrenceRule.Weekly,
                     repeatDays: [Qt.Friday],
                     repeatCount: 4
                    },
                ]
            }
        ]
    }

    function test_addEvent(data)
    {
        var component = Qt.createComponent("addEvent.qml")
        var obj = component.createObject(top)
        if (obj == undefined)
            console.log("Unable to load component from " + name +  " error is ", component.errorString())
        verify(obj != undefined, 'Unable to load component ' + name)
        obj.addEvents(data.events)
        var items = obj.testEvents()
        checkDetails(items)
        if (items.length == 0)
            console.log("No records added")
        obj.destroy()
        component.destroy()
    }
}

