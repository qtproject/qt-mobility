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
    void fetchDetailDef();
    void fetchEventDetailDef();
    
private:
    QOrganizerItemManager *m_om;
};

void TestDetailDef::init()
{
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(m_managerNameSymbian);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
}

void TestDetailDef::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestDetailDef::fetchDetailDef()
{
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions();
    QVERIFY(!detailDef.isEmpty());
}

void TestDetailDef::fetchEventDetailDef()
{
    QMap<QString, QOrganizerItemDetailDefinition> detailDef = m_om->detailDefinitions(QOrganizerItemType::TypeEvent);
    QVERIFY(!detailDef.isEmpty());
    
    // Now check against the details supported on Symbian
    QOrganizerItemDetailDefinition def;
    
    // Item type
    def = detailDef.value(QOrganizerItemType::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // GUID
    def = detailDef.value(QOrganizerItemGuid::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Timestamp
    def = detailDef.value(QOrganizerItemTimestamp::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Display label
    def = detailDef.value(QOrganizerItemDisplayLabel::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Description
    def = detailDef.value(QOrganizerItemDescription::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Comment - Not supported
    def = detailDef.value(QOrganizerItemComment::DefinitionName);
    QVERIFY(def.isEmpty());
    
    // Recurrence
    def = detailDef.value(QOrganizerItemRecurrence::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Event time range
    def = detailDef.value(QOrganizerEventTimeRange::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Priority
    def = detailDef.value(QOrganizerItemPriority::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Location
    def = detailDef.value(QOrganizerItemLocation::DefinitionName);
    QVERIFY(!def.isEmpty());
    QVERIFY(def.isUnique());
    
    // Instance origin - Not supported
    def = detailDef.value(QOrganizerItemInstanceOrigin::DefinitionName);
    QVERIFY(def.isEmpty());
    
    // To-do progress - Not supported
    def = detailDef.value(QOrganizerTodoProgress::DefinitionName);
    QVERIFY(def.isEmpty());
    
    // To-do time range - Not supported
    def = detailDef.value(QOrganizerTodoTimeRange::DefinitionName);
    QVERIFY(def.isEmpty());
    
    // Journal time range - Not supported
    def = detailDef.value(QOrganizerJournalTimeRange::DefinitionName);
    QVERIFY(def.isEmpty());
}

QTEST_MAIN(TestDetailDef);

#include "tst_detaildef.moc"
