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

#include "mt_cntsqlsort.h"
#include <QtTest/QtTest>
#include <QDebug>

void MT_CntSqlSort::init()
{
    QContactManager cm("symbian");
    QList<QContactLocalId> ids = cm.contactIds();
    
    // Confirm db is empty
    ids = cm.contactIds();
    if (!ids.isEmpty()) {
        QMap<int, QContactManager::Error> errorMap;
        if (!cm.removeContacts(ids, &errorMap))
            QSKIP("Error clearing database!!", SkipAll);
    }
}

void MT_CntSqlSort::cleanup()
{
    // Clear all test data
    QContactManager cm("symbian");
    QList<QContactLocalId> ids = cm.contactIds();
    QMap<int, QContactManager::Error> errorMap;
    cm.removeContacts(ids, &errorMap);
}

void MT_CntSqlSort::sortFirstName()
{
    QContactManager cm("symbian");
    QContactName n1;
    n1.setFirstName("Geroge");
    n1.setLastName("A");
    QContact c1;
    QVERIFY(c1.saveDetail(&n1));
    QVERIFY(cm.saveContact(&c1));
    
    QContactName n2;
    n2.setFirstName("Cf");
    n2.setLastName("Anto");
    QContact c2;
    QVERIFY(c2.saveDetail(&n2));
    QVERIFY(cm.saveContact(&c2));
    
    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    QVERIFY(ids.count() == 2);
    QVERIFY(c2.localId() == ids.at(0));
    QVERIFY(c1.localId() == ids.at(1));
}

void MT_CntSqlSort::sortLastName()
{
    QContactManager cm("symbian");
    QContactName n1;
    n1.setFirstName("B");
    n1.setLastName("Ca");
    QContact c1;
    QVERIFY(c1.saveDetail(&n1));
    QVERIFY(cm.saveContact(&c1));
    
    QContactName n2;
    n2.setFirstName("Cf");
    n2.setLastName("Anto");
    QContact c2;
    QVERIFY(c2.saveDetail(&n2));
    QVERIFY(cm.saveContact(&c2));

    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    QVERIFY(ids.count() == 2);
    QVERIFY(c2.localId() == ids.at(0));
    QVERIFY(c1.localId() == ids.at(1));
}

void MT_CntSqlSort::sortFirstLast()
{
    QContactManager cm("symbian");
    QContactName n1;
    n1.setFirstName("Doll");
    n1.setLastName("Aa");
    QContact c1;
    QVERIFY(c1.saveDetail(&n1));
    QVERIFY(cm.saveContact(&c1));
    
    QContactName n2;
    n2.setFirstName("Af");
    n2.setLastName("Zebra");
    QContact c2;
    QVERIFY(c2.saveDetail(&n2));
    QVERIFY(cm.saveContact(&c2));
    
    QContactName n3;
    n3.setLastName("Cow");
    QContact c3;
    QVERIFY(c3.saveDetail(&n3));
    QVERIFY(cm.saveContact(&c3));
    
    QContact c4;
    QVERIFY(cm.saveContact(&c4));
    
    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    
    QContactSortOrder order2;
    order2.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);
    order2.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    sortOrders.append(order2);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    QVERIFY(ids.count() == 4);
    QVERIFY(c2.localId() == ids.at(0));
    QVERIFY(c3.localId() == ids.at(1));
    QVERIFY(c1.localId() == ids.at(2));
    QVERIFY(c4.localId() == ids.at(3));
}

void MT_CntSqlSort::sortLastFirst()
{
    QContactManager cm("symbian");
    QContactName n1;
    n1.setFirstName("Doll");
    n1.setLastName("Aa");
    QContact c1;
    QVERIFY(c1.saveDetail(&n1));
    QVERIFY(cm.saveContact(&c1));
    
    QContactName n2;
    n2.setFirstName("Af");
    n2.setLastName("Zebra");
    QContact c2;
    QVERIFY(c2.saveDetail(&n2));
    QVERIFY(cm.saveContact(&c2));
    
    QContactName n3;
    n3.setFirstName("Cow");
    QContact c3;
    QVERIFY(c3.saveDetail(&n3));
    QVERIFY(cm.saveContact(&c3));
    
    QContact c4;
    QVERIFY(cm.saveContact(&c4));
    
    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    
    QContactSortOrder order2;
    order2.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    order2.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    sortOrders.append(order2);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    QVERIFY(ids.count() == 4);
    QVERIFY(c1.localId() == ids.at(0));
    QVERIFY(c3.localId() == ids.at(1));
    QVERIFY(c2.localId() == ids.at(2));
    QVERIFY(c4.localId() == ids.at(3));
}

void MT_CntSqlSort::sortCompanyName()
{
    QContactManager cm("symbian");
    QContact c1;
    QContactOrganization o1;
    o1.setName("Zawadi");
    QVERIFY(c1.saveDetail(&o1));
    QVERIFY(cm.saveContact(&c1));
    
    QContact c2;
    QContactOrganization o2;
    o2.setName("Crab");
    QVERIFY(c2.saveDetail(&o2));
    QVERIFY(cm.saveContact(&c2));
    
    QContact c3;
    QVERIFY(cm.saveContact(&c3));
    
    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactOrganization::DefinitionName,
            QContactOrganization::FieldName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    QVERIFY(ids.count() == 3);
    QVERIFY(c2.localId() == ids.at(0));
    QVERIFY(c1.localId() == ids.at(1));
    QVERIFY(c3.localId() == ids.at(2));
}

void MT_CntSqlSort::sortBlanksFirst()
{
    QContactManager cm("symbian");
    QContactName n1;
    n1.setFirstName("Doll");
    n1.setLastName("Aa");
    QContact c1;
    QVERIFY(c1.saveDetail(&n1));
    QVERIFY(cm.saveContact(&c1));
    
    QContactName n2;
    n2.setFirstName("Af");
    n2.setLastName("Zebra");
    QContact c2;
    QVERIFY(c2.saveDetail(&n2));
    QVERIFY(cm.saveContact(&c2));
    
    QContactPhoneNumber p3;
    p3.setNumber("12345");
    p3.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    QContact c3;
    QVERIFY(c3.saveDetail(&p3));
    QVERIFY(cm.saveContact(&c3));
    
    QContact c4;
    QVERIFY(cm.saveContact(&c4));
    
    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    order1.setBlankPolicy(QContactSortOrder::BlanksFirst);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    QVERIFY(ids.count() == 4);
    QVERIFY(c3.localId() == ids.at(0));
    QVERIFY(c4.localId() == ids.at(1));
    QVERIFY(c2.localId() == ids.at(2));
    QVERIFY(c1.localId() == ids.at(3));
}


void MT_CntSqlSort::sortFirstLastCompany()
{
    QContactManager cm("symbian");
    QContactName n1;
    n1.setFirstName("Doll");
    n1.setLastName("Aa");
    QContact c1;
    QVERIFY(c1.saveDetail(&n1));
    QVERIFY(cm.saveContact(&c1));
    
    QContactName n2;
    n2.setFirstName("Af");
    QContact c2;
    QVERIFY(c2.saveDetail(&n2));
    QVERIFY(cm.saveContact(&c2));
    
    QContactOrganization o3;
    o3.setName("Boss");
    QContact c3;
    QVERIFY(c3.saveDetail(&o3));
    QVERIFY(cm.saveContact(&c3));
    
    QContact c4;
    QVERIFY(cm.saveContact(&c4));
    
    QContactName n5;
    n5.setLastName("Francis");
    QContact c5;
    QVERIFY(c5.saveDetail(&n5));
    QVERIFY(cm.saveContact(&c5));
    
    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    
    QContactSortOrder order2;
    order2.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    order2.setCaseSensitivity(Qt::CaseInsensitive);
    
    QContactSortOrder order3;
    order3.setDetailDefinitionName(QContactOrganization::DefinitionName,
            QContactOrganization::FieldName);
    order3.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    sortOrders.append(order2);
    sortOrders.append(order3);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    QVERIFY(ids.count() == 5);
    QVERIFY(c1.localId() == ids.at(0));
    QVERIFY(c2.localId() == ids.at(1));
    QVERIFY(c3.localId() == ids.at(2));
    QVERIFY(c5.localId() == ids.at(3));
    QVERIFY(c4.localId() == ids.at(4));
}

void MT_CntSqlSort::sortPriority()
{
    // Symbol possiblities (!, ", # etc)
    QList<char> sl;
    /*sl << '"' << '#' << '¤' << '&' << '/' << '(' << ')' << '=' << '?'
            << ' ' << '!' << '`' << '+' << 'µ';*/
    
    QList<QContactLocalId> sids;
    QContactManager cm("symbian");
    foreach(char c, sl) {
        QContactName n1;
        QString s("George");
        s = s.prepend(c);
        n1.setFirstName(s);
        QContact c1;
        QVERIFY(c1.saveDetail(&n1));
        QVERIFY(cm.saveContact(&c1));
        sids.append(c1.localId());
    }
    
    QContactName n2;
    n2.setFirstName("3Cf");
    QContact c2;
    QVERIFY(c2.saveDetail(&n2));
    QVERIFY(cm.saveContact(&c2));
    
    QContactName n3;
    n3.setFirstName("Cf");
    QContact c3;
    QVERIFY(c3.saveDetail(&n3));
    QVERIFY(cm.saveContact(&c3));
    
    QContactName n4;
    QContact c4;
    QVERIFY(c4.saveDetail(&n4));
    QVERIFY(cm.saveContact(&c4));
    
    QContactName n5;
    n5.setFirstName("%Zebra");
    QContact c5;
    QVERIFY(c5.saveDetail(&n5));
    QVERIFY(cm.saveContact(&c5));
    
    QContactName n6;
    n6.setFirstName("0Cf");
    QContact c6;
    QVERIFY(c6.saveDetail(&n6));
    QVERIFY(cm.saveContact(&c6));
    
    QContactName n7;
    n7.setFirstName("ÄCf");
    QContact c7;
    QVERIFY(c7.saveDetail(&n7));
    QVERIFY(cm.saveContact(&c7));
    
    QContactSortOrder order1;
    order1.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    order1.setCaseSensitivity(Qt::CaseInsensitive);
    
    QList<QContactSortOrder> sortOrders;
    sortOrders.append(order1);
    
    QList<QContactLocalId> ids = cm.contactIds(sortOrders);
    
    QVERIFY(ids.count() == 6 + sids.count());
    QVERIFY(c6.localId() == ids.at(0));
    QVERIFY(c2.localId() == ids.at(1));
    QVERIFY(c7.localId() == ids.at(2));
    QVERIFY(c3.localId() == ids.at(3));
    for(int i=0; i<sids.count(); i++) {
        // Order of symbols not checked
        QContactLocalId id = ids.at(4 + i);
        QVERIFY(sids.contains(id));
    }
    QVERIFY(c5.localId() == ids.at(4 + sids.count()));
    QVERIFY(c4.localId() == ids.at(5 + sids.count()));
}
