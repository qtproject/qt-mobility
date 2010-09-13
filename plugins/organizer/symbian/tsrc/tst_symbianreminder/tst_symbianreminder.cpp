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

#include <QObject>
#include <qmobilityglobal.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

const QString m_managerNameSymbian("symbian");

Q_DECLARE_METATYPE(QOrganizerItemReminder)

class TestSymbianReminder : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();

private slots:
    void addReminder();
    /*void modifyReminder();
    void removeReminder();
    void setInvalidReminder();*/

private:
    QOrganizerItemManager *m_om;
};

void TestSymbianReminder::init()
{
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(m_managerNameSymbian);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
}

void TestSymbianReminder::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestSymbianReminder::addReminder()
{
    // Event with date time reminder
    QOrganizerItem event;
    event.setType(QOrganizerItemType::TypeEvent);
    QOrganizerEventTimeRange range;
    range.setStartDateTime(QDateTime::currentDateTime());
    QOrganizerItemReminder reminder;
    reminder.setDateTime(QDateTime::currentDateTime().addSecs(-300));
    event.saveDetail(&range);
    event.saveDetail(&reminder);
    QVERIFY(m_om->saveItem(&event));
    event = m_om->item(event.localId());
    QOrganizerItemReminder itemReminder = event.detail<QOrganizerItemReminder>();
    QCOMPARE(reminder.dateTime(), itemReminder.dateTime());
    
    // Event with time delta reminder
    QOrganizerItem event2;
    event2.setType(QOrganizerItemType::TypeEvent);
    QOrganizerItemReminder reminder2;
    reminder2.setTimeDelta(300);
    event2.saveDetail(&range);
    event2.saveDetail(&reminder2);
    QVERIFY(m_om->saveItem(&event2));
    event2 = m_om->item(event2.localId());
    QOrganizerItemReminder itemReminder2 = event2.detail<QOrganizerItemReminder>();
    QCOMPARE(reminder2.timeDelta(), itemReminder2.timeDelta());

    // To-do with date time reminder 
    QOrganizerItem todo;
    todo.setType(QOrganizerItemType::TypeTodo);
    QOrganizerTodoTimeRange todoRange;
    todoRange.setStartDateTime(QDateTime::currentDateTime());
    todoRange.setDueDateTime(QDateTime::currentDateTime());
    todo.saveDetail(&todoRange);
    QOrganizerItemReminder reminder3;
    reminder3.setDateTime(QDateTime::currentDateTime().addSecs(-300));
    todo.saveDetail(&reminder3);
    QVERIFY(m_om->saveItem(&todo));
    todo = m_om->item(todo.localId());
    QOrganizerItemReminder itemReminder3 = todo.detail<QOrganizerItemReminder>();
    QCOMPARE(reminder3.dateTime(), itemReminder3.dateTime());

    // To-do with time delta reminder
    QOrganizerItem todo2;
    todo2.setType(QOrganizerItemType::TypeTodo);
    QOrganizerItemReminder reminder4;
    reminder4.setTimeDelta(300);
    todo2.saveDetail(&todoRange);
    todo2.saveDetail(&reminder4);
    QVERIFY(m_om->saveItem(&todo2));
    todo2 = m_om->item(todo2.localId());
    QOrganizerItemReminder itemReminder4 = todo2.detail<QOrganizerItemReminder>();
    QCOMPARE(reminder4.timeDelta(), itemReminder4.timeDelta());
    
    // Repeating event with reminder
    QOrganizerItem repeatingEvent;
    repeatingEvent.setType(QOrganizerItemType::TypeEvent);
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(QDateTime::currentDateTime());
    timeRange.setEndDateTime(QDateTime::currentDateTime().addSecs(3000));
    QOrganizerItemRecurrence recurrence;
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
    rrule.setCount(3);
    QList<QOrganizerItemRecurrenceRule> list;
    list.append(rrule);
    recurrence.setRecurrenceRules(list);
    QOrganizerItemReminder repeatReminder;
    repeatReminder.setDateTime(QDateTime::currentDateTime().addSecs(-300));
    repeatingEvent.saveDetail(&timeRange);
    repeatingEvent.saveDetail(&recurrence);
    repeatingEvent.saveDetail(&repeatReminder);
    QVERIFY(m_om->saveItem(&repeatingEvent));
    QList<QOrganizerItem> itemInstances = m_om->itemInstances(repeatingEvent, QDateTime::currentDateTime(), QDateTime(), 3);
    QVERIFY(itemInstances.count() == 3);
    QOrganizerItem instance1 = itemInstances.at(0);
    QOrganizerItemReminder rptReminder = instance1.detail<QOrganizerItemReminder>();
    QCOMPARE(rptReminder.dateTime(), repeatReminder.dateTime());
    instance1 = itemInstances.at(1);
    rptReminder = instance1.detail<QOrganizerItemReminder>();
    QCOMPARE(rptReminder.dateTime(), repeatReminder.dateTime());
    instance1 = itemInstances.at(2);
    rptReminder = instance1.detail<QOrganizerItemReminder>();
    QCOMPARE(rptReminder.dateTime(), repeatReminder.dateTime());
}

/*
void TestSymbianReminder::modifyReminder()
{
    
}

void TestSymbianReminder::removeReminder()
{
    
}

void TestSymbianReminder::setInvalidReminder()
{
    
}
*/

QTEST_MAIN(TestSymbianReminder)

#include "tst_symbianreminder.moc"
