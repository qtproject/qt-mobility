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
#include <QDebug>

QTM_USE_NAMESPACE

const QString m_managerNameSymbian("symbian");

class TestDetailDef : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
    void fetchEventDetailDef();
    void fetchEventOccDetailDef();
    void fetchTodoDetailDef();
    void fetchTodoOccDetailDef();
    void fetchNoteDetailDef();
    void fetchJournalDetailDef();
    
private:
    QOrganizerManager *m_om;
};

void TestDetailDef::init()
{
    // Create a new item manager instance
    m_om = new QOrganizerManager(m_managerNameSymbian);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds());
}

void TestDetailDef::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestDetailDef::fetchEventDetailDef()
{
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions(QOrganizerItemType::TypeEvent);
    QVERIFY(!detailDef.isEmpty());
    
    // Now check against the details supported on Symbian
    QOrganizerItemDetailDefinition def;
    QOrganizerItemDetailDefinition def1;
    
    // Item type
    def = detailDef.value(QOrganizerItemType::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemType::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // GUID
    def = detailDef.value(QOrganizerItemGuid::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemGuid::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Timestamp
    def = detailDef.value(QOrganizerItemTimestamp::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemTimestamp::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Display label
    def = detailDef.value(QOrganizerItemDisplayLabel::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Description
    def = detailDef.value(QOrganizerItemDescription::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDescription::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Comment - Not supported
    def = detailDef.value(QOrganizerItemComment::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemComment::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Recurrence
    def = detailDef.value(QOrganizerItemRecurrence::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemRecurrence::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Event time range
    def = detailDef.value(QOrganizerEventTime::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerEventTime::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Priority
    def = detailDef.value(QOrganizerItemPriority::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemPriority::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Location
    def = detailDef.value(QOrganizerItemLocation::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemLocation::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Instance origin - Not supported
    def = detailDef.value(QOrganizerItemParent::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemParent::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // To-do progress - Not supported
    def = detailDef.value(QOrganizerTodoProgress::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerTodoProgress::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // To-do time range - Not supported
    def = detailDef.value(QOrganizerTodoTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerTodoTime::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
    
    // Journal time range - Not supported
    def = detailDef.value(QOrganizerJournalTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerJournalTime::DefinitionName, QOrganizerItemType::TypeEvent);
    QCOMPARE(def, def1);
}

void TestDetailDef::fetchEventOccDetailDef()
{
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions(QOrganizerItemType::TypeEventOccurrence);
    QVERIFY(!detailDef.isEmpty());
    
    // Now check against the details supported on Symbian
    QOrganizerItemDetailDefinition def;
    QOrganizerItemDetailDefinition def1;
    
    // Item type
    def = detailDef.value(QOrganizerItemType::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemType::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // GUID
    def = detailDef.value(QOrganizerItemGuid::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemGuid::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Timestamp
    def = detailDef.value(QOrganizerItemTimestamp::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemTimestamp::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Display label
    def = detailDef.value(QOrganizerItemDisplayLabel::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Description
    def = detailDef.value(QOrganizerItemDescription::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDescription::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Comment - Not supported
    def = detailDef.value(QOrganizerItemComment::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemComment::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Recurrence - Not supported
    def = detailDef.value(QOrganizerItemRecurrence::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemRecurrence::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Event time range
    def = detailDef.value(QOrganizerEventTime::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerEventTime::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Priority
    def = detailDef.value(QOrganizerItemPriority::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemPriority::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Location
    def = detailDef.value(QOrganizerItemLocation::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemLocation::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Instance origin
    def = detailDef.value(QOrganizerItemParent::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemParent::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // To-do progress - Not supported
    def = detailDef.value(QOrganizerTodoProgress::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerTodoProgress::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // To-do time range - Not supported
    def = detailDef.value(QOrganizerTodoTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerTodoTime::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
    
    // Journal time range - Not supported
    def = detailDef.value(QOrganizerJournalTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerJournalTime::DefinitionName, QOrganizerItemType::TypeEventOccurrence);
    QCOMPARE(def, def1);
}

void TestDetailDef::fetchTodoDetailDef()
{
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions(QOrganizerItemType::TypeTodo);
    QVERIFY(!detailDef.isEmpty());
    
    // Now check against the details supported on Symbian
    QOrganizerItemDetailDefinition def;
    QOrganizerItemDetailDefinition def1;
    
    // Item type
    def = detailDef.value(QOrganizerItemType::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemType::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // GUID
    def = detailDef.value(QOrganizerItemGuid::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemGuid::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Timestamp
    def = detailDef.value(QOrganizerItemTimestamp::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemTimestamp::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Display label
    def = detailDef.value(QOrganizerItemDisplayLabel::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Description
    def = detailDef.value(QOrganizerItemDescription::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDescription::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Comment - Not supported
    def = detailDef.value(QOrganizerItemComment::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemComment::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Recurrence
    def = detailDef.value(QOrganizerItemRecurrence::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemRecurrence::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Event time range - Not supported
    def = detailDef.value(QOrganizerEventTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerEventTime::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Priority
    def = detailDef.value(QOrganizerItemPriority::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemPriority::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Location - Not supported
    def = detailDef.value(QOrganizerItemLocation::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemLocation::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Instance origin - Not supported
    def = detailDef.value(QOrganizerItemParent::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemParent::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // To-do progress
    def = detailDef.value(QOrganizerTodoProgress::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerTodoProgress::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // To-do time range
    def = detailDef.value(QOrganizerTodoTime::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerTodoTime::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
    
    // Journal time range - Not supported
    def = detailDef.value(QOrganizerJournalTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerJournalTime::DefinitionName, QOrganizerItemType::TypeTodo);
    QCOMPARE(def, def1);
}

void TestDetailDef::fetchTodoOccDetailDef()
{
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions(QOrganizerItemType::TypeTodoOccurrence);
    QVERIFY(!detailDef.isEmpty());
    
    // Now check against the details supported on Symbian
    QOrganizerItemDetailDefinition def;
    QOrganizerItemDetailDefinition def1;
    
    // Item type
    def = detailDef.value(QOrganizerItemType::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemType::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // GUID
    def = detailDef.value(QOrganizerItemGuid::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemGuid::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Timestamp
    def = detailDef.value(QOrganizerItemTimestamp::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemTimestamp::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Display label
    def = detailDef.value(QOrganizerItemDisplayLabel::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Description
    def = detailDef.value(QOrganizerItemDescription::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDescription::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Comment - Not supported
    def = detailDef.value(QOrganizerItemComment::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemComment::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Recurrence - Not supported
    def = detailDef.value(QOrganizerItemRecurrence::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemRecurrence::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Event time range - Not supported
    def = detailDef.value(QOrganizerEventTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerEventTime::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Priority
    def = detailDef.value(QOrganizerItemPriority::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemPriority::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Location - Not supported
    def = detailDef.value(QOrganizerItemLocation::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemLocation::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Instance origin
    def = detailDef.value(QOrganizerItemParent::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemParent::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // To-do progress
    def = detailDef.value(QOrganizerTodoProgress::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerTodoProgress::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // To-do time range
    def = detailDef.value(QOrganizerTodoTime::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerTodoTime::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
    
    // Journal time range - Not supported
    def = detailDef.value(QOrganizerJournalTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerJournalTime::DefinitionName, QOrganizerItemType::TypeTodoOccurrence);
    QCOMPARE(def, def1);
}

void TestDetailDef::fetchNoteDetailDef()
{
    QSKIP("Note support not yet implemented", SkipSingle);

    /*
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions(QOrganizerItemType::TypeNote);
    QVERIFY(!detailDef.isEmpty());
    
    // Now check against the details supported on Symbian
    QOrganizerItemDetailDefinition def;
    QOrganizerItemDetailDefinition def1;
    
    // Item type
    def = detailDef.value(QOrganizerItemType::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemType::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // GUID
    def = detailDef.value(QOrganizerItemGuid::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemGuid::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Timestamp
    def = detailDef.value(QOrganizerItemTimestamp::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemTimestamp::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Display label - Not supported
    def = detailDef.value(QOrganizerItemDisplayLabel::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Description
    def = detailDef.value(QOrganizerItemDescription::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    def1 = m_om->detailDefinition(QOrganizerItemDescription::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Comment - Not supported
    def = detailDef.value(QOrganizerItemComment::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemComment::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Recurrence - Not supported
    def = detailDef.value(QOrganizerItemRecurrence::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemRecurrence::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Event time range - Not supported
    def = detailDef.value(QOrganizerEventTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerEventTime::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Priority - Not supported
    def = detailDef.value(QOrganizerItemPriority::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemPriority::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Location - Not supported
    def = detailDef.value(QOrganizerItemLocation::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemLocation::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Instance origin - Not supported
    def = detailDef.value(QOrganizerItemParent::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerItemParent::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // To-do progress - Not supported
    def = detailDef.value(QOrganizerTodoProgress::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerTodoProgress::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // To-do time range - Not supported
    def = detailDef.value(QOrganizerTodoTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerTodoTime::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    
    // Journal time range - Not supported
    def = detailDef.value(QOrganizerJournalTime::DefinitionName);
    QVERIFY(def.isEmpty());
    def1 = m_om->detailDefinition(QOrganizerJournalTime::DefinitionName, QOrganizerItemType::TypeNote);
    QCOMPARE(def, def1);
    */
}

void TestDetailDef::fetchJournalDetailDef()
{
    // Journal is not supported at all
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions(QOrganizerItemType::TypeJournal);
    QVERIFY(detailDef.isEmpty());
}

QTEST_MAIN(TestDetailDef);

#include "tst_detaildef.moc"
