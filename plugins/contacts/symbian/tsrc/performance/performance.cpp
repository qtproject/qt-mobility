/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
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
    QList<QUniqueId> cnt_ids = mCntMng->contacts();
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
        mContactList.append(alice.id());
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
    mTime.restart();
    mCntMng->removeContacts(&mContactList);
    int elapsed = mTime.elapsed();
    qDebug() << "Removed " << mContactList.count() << " simple contacts in" 
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
    
    mContactList.clear();
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
        alice.saveDetail(&add);
        
        QContactGender gender;
        gender.setGender("Female");
        alice.saveDetail(&gender);
        
        QContactBirthday bday;
        bday.setDate(QDate(25,10,1978));
        alice.saveDetail(&bday);
        
        // Not implemented yet
        /*
        QContactOnlineAccount acc;
        acc.setAccountUri("sips:alice.jones@nokia.com");
        alice.saveDetail(&acc);
        */
        QContactEmailAddress email;
        email.setEmailAddress("mailto:alice.jones@nokia.com");
        alice.saveDetail(&email);
        
        QContactOrganization org;
        org.setDepartment("Services");
        org.setTitle("Assistant Manager");
        org.setLocation("Nokia Cyber Park");
        alice.saveDetail(&email);
        
        contactsList.append(alice);
        mContactList.append(alice.id());
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
    QList<QUniqueId> cnt_ids;
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
    qDebug() << "First name last name sort order with " << mContactList.count() << "contacts: "
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
    
    cnt_ids.clear();
    sortOrders.clear();
    sortOrders.append(sortOrder1);
    sortOrders.append(sortOrder);
    
    mTime.restart();
    cnt_ids = mCntMng->contacts(filter, sortOrders);
    elapsed = mTime.elapsed();
    qDebug() << "Last name first name sort order with " << mContactList.count() << "contacts: "
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::filterContacts()
{
    QList<QUniqueId> cnt_ids;
    QContactUnionFilter unionFilter;
    
    // Landline
    QContactDetailFilter landlineFilter;
    landlineFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    landlineFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeLandline));
    
    // Address
    QContactDetailFilter addFilter;
    addFilter.setDetailDefinitionName(QContactAddress::DefinitionName, QContactAddress::FieldSubTypes); 
    addFilter.setValue(QLatin1String(QContactAddress::SubTypeInternational));
    
    // Combine both filters in an intersection filter
    QContactIntersectionFilter intersectionFilter1;
    intersectionFilter1.append(landlineFilter);
    intersectionFilter1.append(addFilter);
    
    // Mobile
    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    
    // Address
    QContactDetailFilter addFilter2;
    addFilter2.setDetailDefinitionName(QContactAddress::DefinitionName, QContactAddress::FieldSubTypes); 
    addFilter2.setValue(QLatin1String(QContactAddress::SubTypeDomestic));
    
    // Combine both filters in an intersection filter
    QContactIntersectionFilter intersectionFilter2;
    intersectionFilter2.append(mobileFilter);
    intersectionFilter2.append(addFilter);
    
    // Add intersection filters to a union filter
    unionFilter.append(intersectionFilter1);
    unionFilter.append(intersectionFilter2);

    mTime.restart();
    cnt_ids = mCntMng->contacts(unionFilter);
    int elapsed = mTime.elapsed();
    qDebug() << "( Landline || International address ) && ( Mobile || Domestic address ) filter from" 
            << mContactList.count() << "contacts results:" << cnt_ids.count();        
    qDebug() << "Time taken:" << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
}

void SymbianPluginPerfomance::removeComplexContacts()
{
    // Remove N contacts
    mTime.restart();
    mCntMng->removeContacts(&mContactList);
    int elapsed = mTime.elapsed();
    qDebug() << "Removed " << mContactList.count() << " simple contacts in" 
        << elapsed / 1000 << "s" << elapsed % 1000 << "ms";
    
    mContactList.clear();
}

QTEST_MAIN(SymbianPluginPerfomance);
