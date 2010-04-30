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

#include "performance.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QFile>
#include <QTime>

void SymbianPluginPerfomance::initTestCase()
{
    mCntMng = new QContactManager("symbian");

    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = mCntMng->contactIds();
    mCntMng->removeContacts(cnt_ids, 0);
}

void SymbianPluginPerfomance::cleanupTestCase()
{
    delete mCntMng;
}

void SymbianPluginPerfomance::createSimpleContacts()
{
    // Create N contacts
    QList<QContact> contactsList;

    for(int i=0; i<NO_OF_CONTACTS; i++) {
        QString c = QString::number(i);
        QString first("Alice");
        QContact alice;

        // Contact details
        QContactName aliceName;
        aliceName.setFirstName(first.append(c));
        alice.saveDetail(&aliceName);

        contactsList.append(alice);
    }

    // Save the contacts
    mTime.start();
    mCntMng->saveContacts(&contactsList, 0);
    int elapsed = mTime.elapsed();
    qDebug() << "Created " << contactsList.count() << " simple contacts in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::removeSimpleContacts()
{
    // Remove N contacts
    QList<QContactLocalId> cnt_ids = mCntMng->contactIds();
    mTime.restart();
    mCntMng->removeContacts(cnt_ids, 0);
    int elapsed = mTime.elapsed();
    qDebug() << "Removed " << cnt_ids.count() << " simple contacts in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::createComplexContacts()
{
    // Create N contacts
    QList<QContact> contactsList;
    for(int i=0; i<NO_OF_CONTACTS; i++) {
        QContact alice;

        // Contact details
        QString c = QString::number(i);
        QString first("Alice");
        QString last("Jones");

        QContactName aliceName;
        aliceName.setFirstName(first.append(c));
        aliceName.setLastName(last.append(c));
        alice.saveDetail(&aliceName);

        QContactPhoneNumber number;
        number.setContexts("Home");
        number.setSubTypes("Mobile");
        number.setNumber("12345678");
        alice.saveDetail(&number);

        QContactPhoneNumber number2;
        number2.setContexts("Work");
        number2.setSubTypes("Landline");
        number2.setNumber("555-4444");
        alice.saveDetail(&number2);

        QContactAddress add;
        add.setStreet("Leeds West Yorkshire");
        add.setPostcode("10087");
        add.setRegion("New York");
        add.setCountry("United States");
        alice.saveDetail(&add);

        /* Commented out to ensure compatibility with pre-10.1 platforms
        QContactGender gender;
        gender.setGender("Female");
        alice.saveDetail(&gender);
        */

        QContactBirthday bday;
        bday.setDate(QDate(25,10,1978));
        alice.saveDetail(&bday);

        /* Commented out to ensure compatibility with pre-10.1 platforms
        QContactOnlineAccount acc;
        acc.setAccountUri("sips:alice.jones@nokia.com");
        acc.setSubTypes(QContactOnlineAccount::SubTypeSip);
        alice.saveDetail(&acc);
        */

        QContactEmailAddress email;
        email.setEmailAddress("mailto:alice.jones@nokia.com");
        alice.saveDetail(&email);

        QContactOrganization org;
        org.setDepartment(QStringList("Services"));
        org.setTitle("Assistant Manager");
        // Commented out to ensure compatibility with pre-10.1 platforms
        //org.setLocation("Nokia Cyber Park");
        alice.saveDetail(&org);

        contactsList.append(alice);
    }
    // Save the contacts
    mTime.restart();
    QMap<int, QContactManager::Error> errors;
    mCntMng->saveContacts(&contactsList, &errors);
    foreach(QContactManager::Error error, errors) {
        QCOMPARE(error, QContactManager::NoError);
    }
    int elapsed = mTime.elapsed();
    qDebug() << "Created " << contactsList.count() << " complex contacts in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::sortContacts()
{
    QList<QContactLocalId> cnt_ids;
    QContactSortOrder sortOrder;
    QContactSortOrder sortOrder1;

    // first name sort order
    sortOrder.setDetailDefinitionName(QContactName::DefinitionName,  QContactName::FieldFirstName);
    sortOrder.setBlankPolicy(QContactSortOrder::BlanksLast);
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setCaseSensitivity(Qt::CaseInsensitive);

    // last name sort order
    sortOrder1.setDetailDefinitionName(QContactName::DefinitionName,  QContactName::FieldLastName);
    sortOrder1.setBlankPolicy(QContactSortOrder::BlanksLast);
    sortOrder1.setDirection(Qt::AscendingOrder);
    sortOrder1.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> sortOrders;
    sortOrders.append(sortOrder);
    sortOrders.append(sortOrder1);

    measureContactsFetch(
            "Sorting with first name, last name sort order with",
            QContactInvalidFilter(),
            sortOrders);

    cnt_ids.clear();
    sortOrders.clear();
    sortOrders.append(sortOrder1);
    sortOrders.append(sortOrder);

    measureContactsFetch(
            "Sorting with last name, first name sort order with",
            QContactInvalidFilter(),
            sortOrders);
}

void SymbianPluginPerfomance::filterContacts()
{
    QContactIntersectionFilter intersectionFilter;

    // Landline
    QContactDetailFilter landlineFilter;
    landlineFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes);
    landlineFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeLandline));

    // Mobile
    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes);
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));

    // Combine both filters in an intersection filter
    QContactUnionFilter unionFilter1;
    unionFilter1.append(landlineFilter);
    unionFilter1.append(mobileFilter);

    // Int Address
    QContactDetailFilter intAddFilter;
    intAddFilter.setDetailDefinitionName(QContactAddress::DefinitionName, QContactAddress::FieldSubTypes);
    intAddFilter.setValue(QLatin1String(QContactAddress::SubTypeInternational));

    // Dom Address
    QContactDetailFilter domAddFilter;
    domAddFilter.setDetailDefinitionName(QContactAddress::DefinitionName, QContactAddress::FieldSubTypes);
    domAddFilter.setValue(QLatin1String(QContactAddress::SubTypeDomestic));

    // Combine both filters in an intersection filter
    QContactUnionFilter unionFilter2;
    unionFilter2.append(intAddFilter);
    unionFilter2.append(domAddFilter);

    // Add intersection filters to a union filter
    intersectionFilter.append(unionFilter1);
    intersectionFilter.append(unionFilter2);

    measureContactsFetch(
            "Filtering with ( Landline || Mobile ) && ( International address || Domestic address ) from",
            intersectionFilter);
}

void SymbianPluginPerfomance::filterUnions()
{
    QContactUnionFilter unionFilter;

    // Filter contacts that have a phone number that contains one of the
    // numbers 0, 1, 2, ..., 9
    for(int i(0); i < 10; i++) {
        QContactDetailFilter mobileFilter;
        mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
        mobileFilter.setValue(i);
        mobileFilter.setMatchFlags(QContactFilter::MatchContains);
        unionFilter.append(mobileFilter);
    }
    measureContactsFetch(
            "Filter: mobile number contains digit 0, 1, 2, ... or 9",
            unionFilter);


    // First name or last name contains "alice"
    unionFilter = QContactUnionFilter();
    QContactDetailFilter filt;
    filt.setValue("alice");
    filt.setMatchFlags(QContactFilter::MatchContains);

    filt.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    unionFilter.append(filt);
    filt.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
    unionFilter.append(filt);

    measureContactsFetch(
            "Filter: first name \"alice\" or last name \"alice\"",
            unionFilter);
}

void SymbianPluginPerfomance::filterNameListStyle_data()
{
    // The search key; this is typed in by the user in name list style filtering
    QTest::addColumn<QString>("searchKey");
    // The expected count of the contacts in the result
    QTest::addColumn<int>("expectedCount");
    // parameters: test row name, search key, expected result count
    QTest::newRow("A") << "A" << 195;
    QTest::newRow("a") << "a" << 195;
    QTest::newRow("a b") << "a b" << 11;
    QTest::newRow("joan") << "joan" << 22;
}

void SymbianPluginPerfomance::filterNameListStyle()
{
    QFETCH(QString, searchKey);
    QFETCH(int, expectedCount);

    QVariant value = searchKey;
    // If there is more than one search term, use a QStringList value
    QStringList searchList = searchKey.split(QChar(0x20), QString::SkipEmptyParts);
    if(searchList.count() > 1)
        value = searchList;


    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    filter.setMatchFlags(QContactFilter::MatchStartsWith);
    filter.setValue(value);

    int count = measureContactsFetch(
            "Filter names list style",
            filter);

    // Commented out the expected result count check. Currently all the
    // contacts created by performance test module have the same name.
    // This means that all the filters give either 0 or 1000 contacts as a
    // result. So there is not much point checking the result count.
    // TODO: Maybe we should use a pre-defined set of contacts with different
    // names and other details. This would give more usable measurements results.
    //QCOMPARE(count, expectedCount);
}

void SymbianPluginPerfomance::filterPhoneNumberMatch_data()
{
    // The search key; this is typed in by the user in name list style filtering
    QTest::addColumn<QString>("searchKey");
    // The expected count of the contacts in the result
    QTest::addColumn<int>("expectedCount");
    // parameters: test row name, search key, expected result count
    QTest::newRow("76766466") << "76766466" << 15;
    QTest::newRow("+35876766466") << "+35876766466" << 15;
    QTest::newRow("111222333444555") << "111222333444555" << 0;
}

void SymbianPluginPerfomance::filterPhoneNumberMatch()
{
    QFETCH(QString, searchKey);
    QFETCH(int, expectedCount);

    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    filter.setMatchFlags(QContactFilter::MatchPhoneNumber);
    filter.setValue(searchKey);

    int count = measureContactsFetch(
            "Filter phone numbers",
            filter);

    // Commented out the expected result count check. Currently all the
    // contacts created by performance test module have the same name.
    // This means that all the filters give either 0 or 1000 contacts as a
    // result. So there is not much point checking the result count.
    // TODO: Maybe we should use a pre-defined set of contacts with different
    // names and other details. This would give more usable measurements results.
    //QCOMPARE(count, expectedCount);
}

void SymbianPluginPerfomance::removeComplexContacts()
{
    // Remove N contacts
    QList<QContactLocalId> cnt_ids = mCntMng->contactIds();
    mTime.restart();
    mCntMng->removeContacts(cnt_ids, 0);
    int elapsed = mTime.elapsed();
    qDebug() << "Removed " << cnt_ids.count() << " complex contacts in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

int SymbianPluginPerfomance::measureContactsFetch(
        QString debugMessage,
        const QContactFilter &filter,
        const QList<QContactSortOrder>& sortOrders)
{
    QList<QContactLocalId> cnt_ids;
    mTime.restart();

    if(filter.type() != QContactFilter::InvalidFilter)
        cnt_ids = mCntMng->contactIds(filter, sortOrders);
    else
        cnt_ids = mCntMng->contactIds(sortOrders);

    int elapsed = mTime.elapsed();
    qDebug() << debugMessage
            << mCntMng->contactIds().count() << "contacts, gave" << cnt_ids.count() << "contacts."
            << "Time taken:" << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
    return cnt_ids.count();
}

void SymbianPluginPerfomance::createComplexContactsWithOnlineAccount()
{
    QList<QContact> contactsList;
    for(int i=0; i<NO_OF_CONTACTS; i++) {
        QContact alice;
        QString c = QString::number(i);
        QString first("Alice");
        QString last("Jones");
        QContactName aliceName;
        aliceName.setFirstName(first.append(c));
        aliceName.setLastName(last.append(c));
        alice.saveDetail(&aliceName);
        QContactPhoneNumber number;
        number.setContexts("Home");
        number.setSubTypes("Mobile");
        number.setNumber("12345678");
        alice.saveDetail(&number);
        QContactPhoneNumber number2;
        number2.setContexts("Work");
        number2.setSubTypes("Landline");
        number2.setNumber("555-4444");
        alice.saveDetail(&number2);
        QContactAddress add;
        add.setStreet("Leeds West Yorkshire");
        add.setPostcode("10087");
        add.setRegion("New York");
        add.setCountry("United States");
        alice.saveDetail(&add);
        QContactBirthday bday;
        bday.setDate(QDate(25,10,1978));
        alice.saveDetail(&bday);
        QContactEmailAddress email;
        email.setEmailAddress("mailto:alice.jones@nokia.com");
        alice.saveDetail(&email);
        QContactOrganization org;
        org.setDepartment(QStringList("Services"));
        org.setTitle("Assistant Manager");
        alice.saveDetail(&org);
        QContactOnlineAccount aliceOnlineAccount;
        QString uri = first + "@yahoo.com";
        aliceOnlineAccount.setAccountUri(uri);
        aliceOnlineAccount.setServiceProvider("yahoo");
        aliceOnlineAccount.setSubTypes(QContactOnlineAccount::SubTypeSip);
        alice.saveDetail(&aliceOnlineAccount);
        contactsList.append(alice);
    }
    mTime.restart();
    QMap<int, QContactManager::Error> errors;
    mCntMng->saveContacts(&contactsList, &errors);
    foreach(QContactManager::Error error, errors) {
        QCOMPARE(error, QContactManager::NoError);
    }
    int elapsed = mTime.elapsed();
    qDebug() << "Created " << contactsList.count() << " complex contacts with online account in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}
void SymbianPluginPerfomance::sortContactsWithOnlineAccount()
{
    QList<QContactLocalId> cnt_ids;
    QContactSortOrder sortOrder;
    QContactSortOrder sortOrder1;
    sortOrder.setDetailDefinitionName(QContactName::DefinitionName,  QContactName::FieldFirstName);
    sortOrder.setBlankPolicy(QContactSortOrder::BlanksLast);
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setCaseSensitivity(Qt::CaseInsensitive);
    sortOrder1.setDetailDefinitionName(QContactName::DefinitionName,  QContactName::FieldLastName);
    sortOrder1.setBlankPolicy(QContactSortOrder::BlanksLast);
    sortOrder1.setDirection(Qt::AscendingOrder);
    sortOrder1.setCaseSensitivity(Qt::CaseInsensitive);
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(sortOrder);
    sortOrders.append(sortOrder1);
    measureContactsFetch(
            "Sorting with first name, last name and online account sort order with",
            QContactInvalidFilter(),
            sortOrders);
    cnt_ids.clear();
    sortOrders.clear();
    sortOrders.append(sortOrder1);
    sortOrders.append(sortOrder);
    measureContactsFetch(
            "Sorting with last name, first name and online account sort order with",
            QContactInvalidFilter(),
            sortOrders);
}
void SymbianPluginPerfomance::removeComplextContactsWithOnlineAccount()
{
    QList<QContactLocalId> cnt_ids = mCntMng->contactIds();
    mTime.restart();
    mCntMng->removeContacts(cnt_ids, 0);
    int elapsed = mTime.elapsed();
    qDebug() << "Removed " << cnt_ids.count() << " complext contacts with online account in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}
QTEST_MAIN(SymbianPluginPerfomance);
