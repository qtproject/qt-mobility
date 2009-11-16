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
    QList<QContactLocalId> cnt_ids = mCntMng->contacts();
    mCntMng->removeContacts(&cnt_ids);
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
        aliceName.setFirst(first.append(c));
        alice.saveDetail(&aliceName);

        contactsList.append(alice);
    }

    // Save the contacts
    mTime.start();
    mCntMng->saveContacts(&contactsList);
    int elapsed = mTime.elapsed();
    qDebug() << "Created " << contactsList.count() << " simple contacts in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::removeSimpleContacts()
{
    // Remove N contacts
    QList<QContactLocalId> cnt_ids = mCntMng->contacts();
    mTime.restart();
    mCntMng->removeContacts(&cnt_ids);
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
        aliceName.setFirst(first.append(c));
        aliceName.setLast(last.append(c));
        alice.saveDetail(&aliceName);

        QString label("Ally Jones");
        QContactDisplayLabel aliceDisplay;
        aliceDisplay.setLabel(label.append(c));
        alice.saveDetail(&aliceDisplay);

        QContactPhoneNumber number;
        number.setContexts("Home");
        number.setSubTypes("Mobile");
        number.setNumber("12345678");
        alice.saveDetail(&number);
        alice.setPreferredDetail("DialAction", number);

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
        add.setSubTypes(QContactAddress::SubTypeDomestic);
        alice.saveDetail(&add);

        QContactGender gender;
        gender.setGender("Female");
        alice.saveDetail(&gender);

        QContactBirthday bday;
        bday.setDate(QDate(25,10,1978));
        alice.saveDetail(&bday);

        QContactOnlineAccount acc;
        acc.setAccountUri("sips:alice.jones@nokia.com");
        acc.setSubTypes(QContactOnlineAccount::SubTypeSip);
        alice.saveDetail(&acc);

        QContactEmailAddress email;
        email.setEmailAddress("mailto:alice.jones@nokia.com");
        alice.saveDetail(&email);

        QContactOrganization org;
        org.setDepartment(QStringList("Services"));
        org.setTitle("Assistant Manager");
        org.setLocation("Nokia Cyber Park");
        alice.saveDetail(&email);

        contactsList.append(alice);
    }
    // Save the contacts
    mTime.restart();
    mCntMng->saveContacts(&contactsList);
    int elapsed = mTime.elapsed();
    qDebug() << "Created " << contactsList.count() << " complex contacts in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::sortContacts()
{
    QList<QContactLocalId> cnt_ids;
    QContactFilter filter;
    QContactSortOrder sortOrder;
    QContactSortOrder sortOrder1;

    // first name sort order
    sortOrder.setDetailDefinitionName(QContactName::DefinitionName,  QContactName::FieldFirst);
    sortOrder.setBlankPolicy(QContactSortOrder::BlanksLast);
    sortOrder.setDirection(Qt::AscendingOrder);
    sortOrder.setCaseSensitivity(Qt::CaseInsensitive);

    // last name sort order
    sortOrder1.setDetailDefinitionName(QContactName::DefinitionName,  QContactName::FieldLast);
    sortOrder1.setBlankPolicy(QContactSortOrder::BlanksLast);
    sortOrder1.setDirection(Qt::AscendingOrder);
    sortOrder1.setCaseSensitivity(Qt::CaseInsensitive);

    QList<QContactSortOrder> sortOrders;
    sortOrders.append(sortOrder);
    sortOrders.append(sortOrder1);

    mTime.restart();
    cnt_ids = mCntMng->contacts(filter, sortOrders);
    int elapsed = mTime.elapsed();
    qDebug() << "First name last name sort order with " << mCntMng->contacts().count() << "contacts: "
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";

    cnt_ids.clear();
    sortOrders.clear();
    sortOrders.append(sortOrder1);
    sortOrders.append(sortOrder);

    mTime.restart();
    cnt_ids = mCntMng->contacts(filter, sortOrders);
    elapsed = mTime.elapsed();
    qDebug() << "Last name first name sort order with " << mCntMng->contacts().count() << "contacts: "
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::filterContacts()
{
    QList<QContactLocalId> cnt_ids;
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

    mTime.restart();
    cnt_ids = mCntMng->contacts(intersectionFilter);
    int elapsed = mTime.elapsed();
    qDebug() << "( Landline || Mobile ) && ( International address || Domestic address ) filter from"
            << mCntMng->contacts().count() << "contacts results:" << cnt_ids.count()
            << "Time taken:" << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::removeComplexContacts()
{
    // Remove N contacts
    QList<QContactLocalId> cnt_ids = mCntMng->contacts();
    mTime.restart();
    mCntMng->removeContacts(&cnt_ids);
    int elapsed = mTime.elapsed();
    qDebug() << "Removed " << cnt_ids.count() << " simple contacts in"
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

QTEST_MAIN(SymbianPluginPerfomance);
