/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmobilityglobal.h"
#include "qtorganizer.h"

#include <QDebug>
#include <QCoreApplication>
#include <QObject>
#include <QTimer>

QTM_USE_NAMESPACE

static void snippets();
static void dumpItems(QOrganizerManager* manager);
static void dumpItem(const QOrganizerItem& item);

int main(int, char**)
{
    snippets();
    return 0;
}

void snippets()
{
    //! [Instantiating the default manager for the platform]
    QOrganizerManager defaultManager;
    //! [Instantiating the default manager for the platform]

    //! [Instantiating a specific manager]
    QOrganizerManager specificManager("KCal");
    //! [Instantiating a specific manager]

    // XXX TODO: use rrule instead of rdates.
    QDateTime startDateTime = QDateTime::currentDateTime();
    QDate firstOccDate = startDateTime.date().addDays(7);
    QDate secondOccDate = startDateTime.date().addDays(14);
    QDate thirdOccDate = startDateTime.date().addDays(21);
    QDateTime endDateTime = startDateTime.addDays(28);
    QSet<QDate> rDates;
    rDates << firstOccDate << secondOccDate << thirdOccDate;

    //! [Creating a recurrent event]
    QOrganizerEvent marshmallowMeeting;
    marshmallowMeeting.setRecurrenceDates(rDates);
    marshmallowMeeting.setPriority(QOrganizerItemPriority::HighPriority);
    marshmallowMeeting.setLocation("Meeting Room 8");
    marshmallowMeeting.setDescription("A meeting every wednesday to discuss the vitally important topic of marshmallows");
    marshmallowMeeting.setDisplayLabel("Marshmallow Conference");
    if (!defaultManager.saveItem(&marshmallowMeeting))
        qDebug() << "Failed to save the recurrent event; error:" << defaultManager.error();
    //! [Creating a recurrent event]

    //! [Retrieving occurrences of a particular recurrent event within a time period]
    QList<QOrganizerItem> instances = defaultManager.itemOccurrences(marshmallowMeeting, startDateTime, endDateTime);
    //! [Retrieving occurrences of a particular recurrent event within a time period]
    qDebug() << "dumping retrieved instances:";
    foreach(const QOrganizerItem& currInst, instances)
    {
        dumpItem(currInst);
        qDebug() << "....................";
    }


    //! [Retrieving the next 5 occurrences of a particular recurrent event]
    instances = defaultManager.itemOccurrences(marshmallowMeeting, QDateTime::currentDateTime(), QDateTime(), 5);
    //! [Retrieving the next 5 occurrences of a particular recurrent event]

    //! [Retrieving the next 10 occurrences of any item (Agenda View)]
    instances = defaultManager.items(QDateTime::currentDateTime(), QDateTime());
    instances = instances.mid(0, 10);
    //! [Retrieving the next 10 occurrences of any item (Agenda View)]

    //! [Creating a non-recurrent entry]
    // a default constructed journal will have it's date/time set to the current date/time.
    QOrganizerJournal journal;
    journal.setDescription("The conference went well.  We all agree that marshmallows are awesome, "\
                    "but we were unable to reach any agreement as to how we could possibly "\
                    "increase our intake of marshmallows.  Several action points were assigned "\
                    "to various members of the group; I have been tasked with finding a good "\
                    "recipe that combines both marshmallows and chocolate, by next Wednesday.");
    defaultManager.saveItem(&journal);
    //! [Creating a non-recurrent entry]

    //! [Editing a non-recurrent entry]
    journal.addComment("Serves: 8.  Ingredients: 500g Milk Chocolate, 500g Marshmallows."\
                    "  Step 1: Put the marshmallows into 8 separate bowls."\
                    "  Step 2: Melt the chocolate."\
                    "  Step 3: Pour the chocolate over the marshmallows in the bowls."\
                    "  Step 4: Put the bowls into the refrigerator for 20 minutes; serve chilled.");
    if (!defaultManager.saveItem(&journal))
        qDebug() << "Unable to save updated journal!  Error:" << defaultManager.error();
    //! [Editing a non-recurrent entry]
    
    //! [Removing an entry]
    defaultManager.removeItem(journal.id());
    //! [Removing an entry]

    //! [Retrieving entries for a time period]
    QList<QOrganizerItem> entries =
        defaultManager.items(QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)),
                             QDateTime(QDate(2010, 1, 31), QTime(23, 59, 59)));
    //! [Retrieving entries for a time period]

    //! [Retrieving entries with a filter]
    entries = defaultManager.items(QOrganizerItemLocation::match("Meeting Room 8"));
    //! [Retrieving entries with a filter]

    //! [Downcasting items]
    QList<QOrganizerItem> items = defaultManager.items();
    foreach (QOrganizerItem item, entries) {
        if (item.type() == QOrganizerItemType::TypeEvent) {
            QOrganizerEvent event(item);
            qDebug() << "Event:" << event.startDateTime() << ", " << event.displayLabel();
        } else if (item.type() == QOrganizerItemType::TypeEventOccurrence) {
            QOrganizerEventOccurrence event(item);
            qDebug() << "Event:" << event.startDateTime() << ", " << event.displayLabel();
        } else if (item.type() == QOrganizerItemType::TypeTodo) {
            // process todos
        } else if (item.type() == QOrganizerItemType::TypeTodoOccurrence) {
            // process recurring todos
        } else if (item.type() == QOrganizerItemType::TypeJournal) {
            // process journals
        } else if (item.type() == QOrganizerItemType::TypeNote) {
            // process notes
        }
    }
    //! [Downcasting items]

    //! [Creating an exception to a particular recurrent event]
    QOrganizerEventOccurrence nextMarshmallowMeeting = defaultManager.itemOccurrences(marshmallowMeeting).value(0);
    nextMarshmallowMeeting.setStartDateTime(QDateTime::fromString("13.05.2010 18:00:00", "dd.MM.yy hh:mm:ss"));
    nextMarshmallowMeeting.setEndDateTime(QDateTime::fromString("13.05.2010 20:00:00", "dd.MM.yy hh:mm:ss"));
    nextMarshmallowMeeting.addComment("The next meeting will go for an hour longer (starting one "\
                                      "hour earlier than usual), since we have scheduled one hour"\
                                      "to taste the results of the recipe that I will be presenting "\
                                      "at the meeting.");
    defaultManager.saveItem(&nextMarshmallowMeeting);
    //! [Creating an exception to a particular recurrent event]

    //! [Getting a list of collections]
    QList<QOrganizerCollection> collections = defaultManager.collections();
    //! [Getting a list of collections]

    QOrganizerCollection collection = collections.first();

    //! [Saving an item to a collection]
    marshmallowMeeting.setCollectionId(collection.id());
    defaultManager.saveItem(&marshmallowMeeting);
    //! [Saving an item to a collection]

    //! [Retrieving items in a collection]
    QOrganizerItemCollectionFilter collectionFilter;
    collectionFilter.setCollectionId(collection.id());
    items = defaultManager.items(collectionFilter);
    //! [Retrieving items in a collection]

    dumpItems(&defaultManager);
}

void dumpItems(QOrganizerManager* manager)
{
    QList<QOrganizerItem> items = manager->items();
    qDebug() << "dumping" << items.count() << "items:";
    qDebug() << "=============================";
    for (int i = 0; i < items.size(); ++i) {
        QOrganizerItem curr = items.at(i);
        dumpItem(curr);
        if (i < (items.size() - 1)) {
            qDebug() << "--------------";
        }
    }
    qDebug() << "=============================";
}

void dumpItem(const QOrganizerItem& item)
{
    qDebug() << "item:" << item.displayLabel() << ", id:" << item.id();
    QList<QOrganizerItemDetail> dets = item.details();
    foreach (const QOrganizerItemDetail det, dets) {
        qDebug() << "    new" << det.definitionName() << "detail:";
        QVariantMap values = det.variantValues();
        QStringList keys = values.keys();
        foreach (const QString& key, keys) {
            qDebug() << "        " << key << "=" << values.value(key);
        }
    }
}
