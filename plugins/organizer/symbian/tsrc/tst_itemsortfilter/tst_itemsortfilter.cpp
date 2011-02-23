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

#include <qmobilityglobal.h>
#include <QObject>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

/*!
 * A test helper class that acts as a placeholder for QOrganizerItemDetail data.
 */
class QTstDetailField
{
public:
    QTstDetailField(QString definitionName, QString fieldName, QVariant value)
    {
        m_definitionName = definitionName;
        m_fieldName = fieldName;
        m_value = value;
    }
    QString m_definitionName;
    QString m_fieldName;
    QVariant m_value;
};

class QTstSortOrder
{
public:
    QTstSortOrder(QString definitionName, QString fieldName, QOrganizerItemSortOrder::BlankPolicy blankPolicy,
        Qt::SortOrder direction, Qt::CaseSensitivity caseSensitivity)
    {
        m_definitionName = definitionName;
        m_fieldName = fieldName;
        m_blankPolicy = blankPolicy;
        m_direction = direction;
        m_caseSensitivity = caseSensitivity;
    }
    QString m_definitionName;
    QString m_fieldName;
    QOrganizerItemSortOrder::BlankPolicy m_blankPolicy;
    Qt::SortOrder m_direction;
    Qt::CaseSensitivity m_caseSensitivity;
};

typedef QList<QTstDetailField> QTstDetailFieldList;
typedef QList<QTstSortOrder> QTstSortOrderList;
typedef QList<QOrganizerItemId> QOrganizerItemIdList;
typedef QList<QOrganizerItemSortOrder> QOrganizerItemSortOrderList;
Q_DECLARE_METATYPE(QOrganizerItemFilter)
Q_DECLARE_METATYPE(QOrganizerItemSortOrderList)

/*!
 * For testing symbian backend via QOrganizerManager API. The target is
 * to implement test cases in a platform independent manner so that this test
 * module could be used also with other backends and potentially used as a
 * QtMobility auto test with as little porting work as possible.
 */
class tst_itemSortFilter : public QObject
{
    Q_OBJECT
public:
    tst_itemSortFilter();
    virtual ~tst_itemSortFilter();

private slots:  // Test cases
    void filterItems_data();
    void filterItems();
    void sortItems_data();
    void sortItems();

private: // util functions
    void addEvent_data(int mgrIndex);
    void addTodo_data(int mgrIndex);
    bool parseDetails(QTstDetailFieldList detailsString, QList<QOrganizerItemDetail> &details);
    QOrganizerItemId addTestDataItem(int mgrIndex, QString displayLabel, QTstDetailFieldList detailsList);
    void addNewFilterRow(int mgrIndex, QOrganizerItemFilter filter, QString matchingItems);
    void addNewSortRow(int mgrIndex, QTstSortOrderList sortDetailList, QString matchingItemsStr);
    QString convertIds(int mgrIndex, QOrganizerItemIdList ids);
    QOrganizerItemFilter invalidFilter();
    QOrganizerItemFilter defaultFilter();
    QOrganizerItemFilter detailFilter(QString defNam, QString fieldNam, int flags, QVariant filterCriterion);
    QOrganizerItemFilter detailRangeFilter(QString defNam, QString fieldNam, int flags, QVariant min, QVariant max);
private:
    QList<QOrganizerManager *> m_managers;
    QMap<int, QOrganizerItemIdList> m_itemIds; // <"manager index", "item ids">
};

tst_itemSortFilter::tst_itemSortFilter()
{
    QStringList managerNames = QOrganizerManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    int i=0;

    foreach (QString manager, managerNames) {
        m_managers << new QOrganizerManager(manager);
        addTodo_data(i++);
//TODO: addEvent_data();
    }
}

tst_itemSortFilter::~tst_itemSortFilter()
{
    foreach (QOrganizerManager* manager, m_managers) {
        manager->removeItems(manager->itemIds());
        delete manager;
        manager = 0;
    }
}

QOrganizerItemId tst_itemSortFilter::addTestDataItem(int mgrIndex, QString displayLabel, QTstDetailFieldList detailsList)
{
    QOrganizerItem item;
    item.setDisplayLabel(displayLabel);
    QList<QOrganizerItemDetail> details;

    if (parseDetails(detailsList, details)) {
        foreach(QOrganizerItemDetail detail, details) {
            item.saveDetail(&detail);
        }
    }

    m_managers.at(mgrIndex)->saveItem(&item);
    if (m_managers.at(mgrIndex)->error() != QOrganizerManager::NoError) {
        QWARN("[tst_itemSortFilter::addTestDataItem] Test item creation failed");
    }
    return item.id();
}

void tst_itemSortFilter::addNewFilterRow(int mgrIndex, QOrganizerItemFilter filter, QString matchingItemsStr)
{
    QString title = QString("[%1] Filter : filterType=%2 matchingItems=%3").arg((m_managers.at(mgrIndex)->managerName()).toLatin1().constData())
        .arg(filter.type()).arg(matchingItemsStr.toLatin1().constData());
    QTest::newRow(title.toAscii().constData()) << mgrIndex << filter << matchingItemsStr;
}

void tst_itemSortFilter::addNewSortRow(int mgrIndex, QTstSortOrderList sortDetailList, QString matchingItemsStr)
{
    QOrganizerItemSortOrderList sortOrderList;

    foreach (QTstSortOrder tstSortOrder, sortDetailList) {
        QOrganizerItemSortOrder sorting;
        sorting.setDetailDefinitionName(tstSortOrder.m_definitionName, tstSortOrder.m_fieldName);
        sorting.setBlankPolicy(tstSortOrder.m_blankPolicy);
        sorting.setDirection(tstSortOrder.m_direction);
        sorting.setCaseSensitivity(tstSortOrder.m_caseSensitivity);
        sortOrderList.append(sorting);
    }

    QString title = QString("[%1] Sort : sortOrderCount=%2 firstSortOrder=%3 matchingItems=%4").arg((m_managers.at(mgrIndex)->managerName()).toLatin1().constData())
        .arg(sortOrderList.count()).arg(sortOrderList.first().detailFieldName().toLatin1().constData()).arg(matchingItemsStr.toLatin1().constData());
    QTest::newRow(title.toAscii().constData()) << mgrIndex << sortOrderList << matchingItemsStr;
}

void tst_itemSortFilter::filterItems_data()
{
    QTest::addColumn<int>("mgrIndex");
    QTest::addColumn<QOrganizerItemFilter>("filter");
    QTest::addColumn<QString>("matchingItemsStr");

    for (int i=0;i < m_managers.size();i++) {
        addNewFilterRow(i, invalidFilter(), QString());
        addNewFilterRow(i, defaultFilter(), "abcdef");

        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchExactly, QVariant("Abc")), "ad");
        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchExactly, QVariant("Cde")), "cf");
        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchContains, QVariant("b")), "abde");
        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchContains, QVariant("de")), "cf");
        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchStartsWith, QVariant("b")), "be");
        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchStartsWith, QVariant("bc")), "be");
        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchEndsWith, QVariant("c")), "ad");
        addNewFilterRow(i, detailFilter(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
            QOrganizerItemFilter::MatchEndsWith, QVariant("bc")), "ad");
        addNewFilterRow(i, detailFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus,
            QOrganizerItemFilter::MatchExactly, QVariant(QOrganizerTodoProgress::StatusNotStarted)), "ad");
        addNewFilterRow(i, detailFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus,
            QOrganizerItemFilter::MatchExactly, QVariant(QOrganizerTodoProgress::StatusInProgress)), "be");
        addNewFilterRow(i, detailFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus,
            QOrganizerItemFilter::MatchExactly, QVariant(QOrganizerTodoProgress::StatusComplete)), "cf");

        if (m_managers.at(i)->detailDefinitions(QOrganizerItemType::TypeTodo).value(QOrganizerTodoProgress::DefinitionName)
            .fields().contains(QOrganizerTodoProgress::FieldPercentageComplete)) {
            addNewFilterRow(i, detailRangeFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete,
                QOrganizerItemFilter::MatchExactly, QVariant(), QVariant()), "bcef");
            addNewFilterRow(i, detailRangeFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete,
                QOrganizerItemFilter::MatchExactly, QVariant(25), QVariant(80)), "be");
            addNewFilterRow(i, detailRangeFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete,
                QOrganizerItemFilter::MatchExactly, QVariant(75), QVariant(80)), "b");
        }

        addNewFilterRow(i, detailRangeFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime,
            QOrganizerItemFilter::MatchExactly, QVariant(QDateTime::currentDateTime().addDays(-1)), QVariant(QDateTime::currentDateTime().addDays(5))), "cf");
        addNewFilterRow(i, detailRangeFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime,
            QOrganizerItemFilter::MatchExactly, QVariant(QDateTime::currentDateTime().addDays(-1)), QVariant(QDateTime::currentDateTime())), "c");
        addNewFilterRow(i, detailRangeFilter(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime,
            QOrganizerItemFilter::MatchExactly, QVariant(), QVariant()), "cf");
        }
}

void tst_itemSortFilter::filterItems()
{
    QFETCH(QOrganizerItemFilter, filter);
    QFETCH(int, mgrIndex);
    QFETCH(QString, matchingItemsStr);

    // Verify item occurrences
    QList<QOrganizerItem> actualItems = m_managers.at(mgrIndex)->items(filter);
    // Cannot use id comparison, since no ids are available for occurrences
    QCOMPARE(actualItems.count(), matchingItemsStr.count());

    // Verify items for export
    actualItems = m_managers.at(mgrIndex)->itemsForExport(QDateTime(), QDateTime(), filter);
    QList<QOrganizerItemId> actualIds;
    QString actualItemsStr;

    foreach(QOrganizerItem item, actualItems) {
        actualIds << item.id();
    }
    actualItemsStr = convertIds(mgrIndex, actualIds);
    QCOMPARE(actualItemsStr, matchingItemsStr);
}

void tst_itemSortFilter::sortItems_data()
{
    QTest::addColumn<int>("mgrIndex");
    QTest::addColumn<QOrganizerItemSortOrderList>("sortOrderList");
    QTest::addColumn<QString>("matchingItemsStr");

    for (int i=0;i < m_managers.size();i++) {
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::AscendingOrder, Qt::CaseSensitive), "defabc");
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::DescendingOrder, Qt::CaseSensitive), "cbafed");
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime,
                QOrganizerItemSortOrder::BlanksLast, Qt::AscendingOrder, Qt::CaseInsensitive)
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::AscendingOrder, Qt::CaseSensitive), "abcdef");
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime,
                QOrganizerItemSortOrder::BlanksLast, Qt::DescendingOrder, Qt::CaseInsensitive)
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::DescendingOrder, Qt::CaseSensitive), "fedcba");
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus,
                QOrganizerItemSortOrder::BlanksLast, Qt::DescendingOrder, Qt::CaseSensitive)
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::DescendingOrder, Qt::CaseSensitive), "cfbead");
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus,
                QOrganizerItemSortOrder::BlanksLast, Qt::AscendingOrder, Qt::CaseSensitive)
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::DescendingOrder, Qt::CaseSensitive), "adbecf");
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus,
                QOrganizerItemSortOrder::BlanksLast, Qt::AscendingOrder, Qt::CaseSensitive)
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::AscendingOrder, Qt::CaseSensitive), "daebfc");
        addNewSortRow(i, QTstSortOrderList()
            << QTstSortOrder(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus,
                QOrganizerItemSortOrder::BlanksLast, Qt::DescendingOrder, Qt::CaseSensitive)
            << QTstSortOrder(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel,
                QOrganizerItemSortOrder::BlanksLast, Qt::AscendingOrder, Qt::CaseSensitive), "fcebda");
    }
}

void tst_itemSortFilter::sortItems()
{
    QFETCH(QOrganizerItemSortOrderList, sortOrderList);
    QFETCH(int, mgrIndex);
    QFETCH(QString, matchingItemsStr);

    // Verify item occurrences
    QList<QOrganizerItem> actualItems = m_managers.at(mgrIndex)->items(QOrganizerItemFilter(), sortOrderList);
    // Cannot use id comparison, since no ids are available for occurrences
    QCOMPARE(actualItems.count(), matchingItemsStr.count());

    // Verify items for export
    actualItems = m_managers.at(mgrIndex)->itemsForExport(QDateTime(), QDateTime(), QOrganizerItemFilter(), sortOrderList);
    QList<QOrganizerItemId> actualIds;
    QString actualItemsStr;

    foreach(QOrganizerItem item, actualItems) {
        actualIds << item.id();
    }
    actualItemsStr = convertIds(mgrIndex, actualIds);

    QCOMPARE(actualItemsStr, matchingItemsStr);
}

/**
 * Adds event related test data.
 */
void tst_itemSortFilter::addEvent_data(int mgrIndex)
{
//TODO: to be implemented
    Q_UNUSED(mgrIndex);
}

/**
 * Adds todo item related test data.
 */
void tst_itemSortFilter::addTodo_data(int mgrIndex)
{
    QTstDetailField itemTypeTodo(QOrganizerItemType::DefinitionName, QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo);
    QOrganizerItemIdList itemIdList;
    bool fieldPercentageSupported = (m_managers.at(mgrIndex)->detailDefinitions(QOrganizerItemType::TypeTodo)
        .value(QOrganizerTodoProgress::DefinitionName).fields().contains(QOrganizerTodoProgress::FieldPercentageComplete));

    itemIdList << addTestDataItem(mgrIndex, "abc", QTstDetailFieldList() << itemTypeTodo
        << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusNotStarted)
        << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
        << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime()));

    if (fieldPercentageSupported) {
        itemIdList << addTestDataItem(mgrIndex,"bcd", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusInProgress)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete, 75));
        itemIdList << addTestDataItem(mgrIndex, "cde", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete, 100));
    } else {
        itemIdList << addTestDataItem(mgrIndex,"bcd", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusInProgress)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime()));
        itemIdList << addTestDataItem(mgrIndex, "cde", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime, QDateTime::currentDateTime()));
    }

    itemIdList << addTestDataItem(mgrIndex, "Abc", QTstDetailFieldList() << itemTypeTodo
        << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusNotStarted)
        << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(3))
        << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime().addDays(2)));

    if (fieldPercentageSupported) {
        itemIdList << addTestDataItem(mgrIndex, "Bcd", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusInProgress)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(3))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime().addDays(2))
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete, 25));
        itemIdList << addTestDataItem(mgrIndex, "Cde", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(3))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime().addDays(2))
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime, QDateTime::currentDateTime().addDays(2))
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldPercentageComplete, 100));
    } else {
        itemIdList << addTestDataItem(mgrIndex, "Bcd", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusInProgress)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(3))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime().addDays(2)));
        itemIdList << addTestDataItem(mgrIndex, "Cde", QTstDetailFieldList() << itemTypeTodo
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete)
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldDueDateTime, QDateTime::currentDateTime().addDays(3))
            << QTstDetailField(QOrganizerTodoTime::DefinitionName, QOrganizerTodoTime::FieldStartDateTime, QDateTime::currentDateTime().addDays(2))
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime, QDateTime::currentDateTime().addDays(2)));
    }

    m_itemIds.insert(mgrIndex, itemIdList);
}

/*!
 * Helper function for parsing QTstDetailFields into organizer item details
 */
bool tst_itemSortFilter::parseDetails(QTstDetailFieldList detailsList, QList<QOrganizerItemDetail> &details)
{
    foreach (QTstDetailField tstdetail, detailsList) {
        QOrganizerItemDetail detail(tstdetail.m_definitionName);
        int existingIndex(-1);
        foreach (QOrganizerItemDetail existingDetail, details) {
            if (existingDetail.definitionName() == detail.definitionName()) {
                // Replace the existing detail in the list so that the new
                // field is added to the existing detail
                // TODO: check that the field does not exist already
                existingIndex = details.indexOf(existingDetail);
                detail = existingDetail;
            }
        }

        if (detail.setValue(tstdetail.m_fieldName, tstdetail.m_value)) {
            // If a detail with same name exists, replace
            if (existingIndex != -1) {
                details.replace(existingIndex, detail);
            } else {
                details.append(detail);
            }
        } else {
            qDebug() << QString("Failed to set field value!");
            return false;
        }
    }
    return true;
}

QString tst_itemSortFilter::convertIds(int mgrIndex, QOrganizerItemIdList ids)
{
    QOrganizerItemIdList managerIds = m_itemIds.value(mgrIndex);
    QString ret;
    /* Expected is of the form "abcd".. it's possible that there are some extra contacts */
    for (int i = 0; i < ids.size(); i++) {
        if (managerIds.indexOf(ids.at(i)) >= 0)
            ret += ('a' + managerIds.indexOf(ids.at(i)));
    }
    return ret;
}

QOrganizerItemFilter tst_itemSortFilter::invalidFilter()
{
    QOrganizerItemInvalidFilter f;
    return f;
}

QOrganizerItemFilter tst_itemSortFilter::defaultFilter()
{
    QOrganizerItemFilter f;
    return f;
}

QOrganizerItemFilter tst_itemSortFilter::detailFilter(QString defNam, QString fieldNam, int flags, QVariant filterCriterion)
{
    QOrganizerItemDetailFilter f;
    f.setDetailDefinitionName(defNam, fieldNam);
    f.setMatchFlags(QOrganizerItemFilter::MatchFlags(flags));
    f.setValue(filterCriterion);
    return f;
}

QOrganizerItemFilter tst_itemSortFilter::detailRangeFilter(QString defNam, QString fieldNam, int flags, QVariant min, QVariant max)
{
    QOrganizerItemDetailRangeFilter f;
    f.setDetailDefinitionName(defNam, fieldNam);
    f.setMatchFlags(QOrganizerItemFilter::MatchFlags(flags));
    f.setRange(min, max);
    return f;
}

QTEST_MAIN(tst_itemSortFilter);
#include "tst_itemsortfilter.moc"
