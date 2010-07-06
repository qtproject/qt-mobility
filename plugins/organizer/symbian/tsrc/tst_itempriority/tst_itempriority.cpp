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

Q_DECLARE_METATYPE(QOrganizerItemPriority)

class TestItemPriority : public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
    void addPriorityDetail();
    void addPriorityDetail_data();
    
private:
    void addPriorityData(QString managerName, QString itemType);
    
private:
    QOrganizerItemManager *m_om;
    
};

void TestItemPriority::init()
{
    QFETCH(QString, managerName);
    
    // Create a new item manager instance
    m_om = new QOrganizerItemManager(managerName);
    
    // Cleanup by deleting all items
    m_om->removeItems(m_om->itemIds(), 0);
}

void TestItemPriority::cleanup()
{
    if (m_om) {
        delete m_om;
        m_om = 0;
    }
}

void TestItemPriority::addPriorityDetail_data()
{
    // Get the list of all available item managers
    QStringList availableManagers = QOrganizerItemManager::availableManagers();
    
    // Remove these since test would fail
    availableManagers.removeAll("invalid");
    availableManagers.removeAll("skeleton");
    
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerItemPriority>("priority");
    
    foreach(QString manager, availableManagers) {
        addPriorityData(manager, QOrganizerItemType::TypeEvent);
    }
}

void TestItemPriority::addPriorityData(QString managerName, QString itemType)
{
    QOrganizerItemPriority priority;
    priority.setPriority(QOrganizerItemPriority::UnknownPriority);
    
    QTest::newRow(QString("UnknownPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::HighestPriority);
    QTest::newRow(QString("HighestPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::ExtremelyHighPriority);
    QTest::newRow(QString("ExtremelyHighPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::VeryHighPriority);
    QTest::newRow(QString("VeryHighPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::HighPriority);
    QTest::newRow(QString("HighPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::MediumPriority);
    QTest::newRow(QString("MediumPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::LowPriority);
    QTest::newRow(QString("LowPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::VeryLowPriority);
    QTest::newRow(QString("VeryLowPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::ExtremelyLowPriority);
    QTest::newRow(QString("ExtremelyLowPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;

    priority.setPriority(QOrganizerItemPriority::LowestPriority);
    QTest::newRow(QString("LowestPriority [%1]").arg(managerName).toLatin1().constData())
        << managerName
        << itemType
        << QDateTime::currentDateTime().addDays(1)
        << priority;
}

void TestItemPriority::addPriorityDetail()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerItemPriority, priority);
    
    // Set the item type
    QOrganizerItem item;
    item.setType(itemType);
    
    // Set the start time, without which Symbian does not save entries
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(startTime);
    item.saveDetail(&timeRange);
    
    // Set the priority
    item.saveDetail(&priority);
    
    // Now save the item
    QVERIFY(m_om->saveItem(&item));
    
    // Fetch the item again
    item = m_om->item(item.localId());
    
    // Check if the priority has been set properly
    QOrganizerItemPriority itemPriority;
    itemPriority = item.detail(QOrganizerItemPriority::DefinitionName);
    
    QVERIFY(itemPriority.priority() == priority.priority());
}

QTEST_MAIN(TestItemPriority);

#include "tst_itempriority.moc"