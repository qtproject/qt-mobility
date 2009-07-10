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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QContact: public QObject
{
Q_OBJECT

public:
    tst_QContact();
    virtual ~tst_QContact();

public slots:
    void init();
    void cleanup();
private slots:
    void name();
    void details();
    void actions();
    void preferences();
};

tst_QContact::tst_QContact()
{
}

tst_QContact::~tst_QContact()
{
}

void tst_QContact::init()
{
}

void tst_QContact::cleanup()
{
}


void tst_QContact::name()
{
    QContact c;
    const QContact& cref = c;

    // Basic name operations
    c.name().setFirst("First");
    c.name().setLast("Last");

    QVERIFY(c.name().first() == "First");
    QVERIFY(c.name().last() == "Last");

    // Const accessor
    QVERIFY(cref.name() == c.name());

    // Try to remove the name
    QVERIFY(c.removeDetail(&c.name()) == false);
    QVERIFY(c.error() == QContact::PermissionsError);

    // Add a new name (overwrites)
    QContactName n = c.name();
    n.setLast("Alternate Last");

    QCOMPARE(c.name().last(), QString("Last"));
    QCOMPARE(n.last(), QString("Alternate Last"));
    QVERIFY(c.saveDetail(&n) == true);
    QVERIFY(c.error() == QContact::NoError);

    // check that it overwrote
    QCOMPARE(c.name().last(), QString("Alternate Last"));
    QCOMPARE(c.name(), n);
}

void tst_QContact::details()
{
    QContact c;

    // Test there are no details (apart from the name :/) by default
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    // Test retrieving the first detail (the name)
    QContactDetail detail = c.detail(QString());
    QVERIFY(detail.definitionName() == QContactName::staticType());

    detail = c.detail("nonexistent");
    QVERIFY(detail.isEmpty());
    QVERIFY(detail.definitionName().isEmpty());

    QVERIFY(c.details("nonexistent").count() == 0);

    // Add a detail
    QContactPhoneNumber p;
    p.setNumber("12345678");
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);

    QVERIFY(c.details().count() == 2);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 1);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(!c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(!c.detail<QContactPhoneNumber>().isEmpty());
    QCOMPARE(c.detail<QContactPhoneNumber>(), p);

    // Remove detail
    QVERIFY(c.removeDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    // Try removing it again
    QVERIFY(!c.removeDetail(&p));
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);

    // Add again, and remove a different way (retrieved copy)
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.details().count() == 2);
    QContactPhoneNumber p2 = c.detail(QContactPhoneNumber::staticType());
    QCOMPARE(p, p2);

    QVERIFY(c.removeDetail(&p2));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    QCOMPARE(p, p2);

    // Add again again, and remove a different way (base class)
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.details().count() == 2);
    QContactDetail p3 = c.detail(QContactPhoneNumber::staticType());
    QVERIFY(p == p3);

    QVERIFY(c.removeDetail(&p3));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    QVERIFY(p == p3);

    // Now try adding a detail to multiple contacts

    QContact c2;
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c2.saveDetail(&p));
    QVERIFY(c2.error() == QContact::NoError);

    QVERIFY(c.details().count() == 2);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 1);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(!c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(!c.detail<QContactPhoneNumber>().isEmpty());
    QCOMPARE(c.detail<QContactPhoneNumber>(), p);

    QVERIFY(c2.details().count() == 2);
    QVERIFY(c2.details(QContactPhoneNumber::staticType()).count() == 1);
    QVERIFY(c2.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(!c2.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(!c2.detail<QContactPhoneNumber>().isEmpty());
    QCOMPARE(c2.detail<QContactPhoneNumber>(), p);

    // Now try removing it from one
    QVERIFY(c.removeDetail(&p));
    QVERIFY(c.error() == QContact::NoError);

    // Make sure it's gone from the first contact
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    // but not the second
    QVERIFY(c2.details().count() == 2);
    QVERIFY(c2.details(QContactPhoneNumber::staticType()).count() == 1);
    QVERIFY(c2.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(!c2.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(!c2.detail<QContactPhoneNumber>().isEmpty());
    QCOMPARE(c2.detail<QContactPhoneNumber>(), p);

    // Now remove it from the second as well
    QVERIFY(c2.removeDetail(&p));
    QVERIFY(c2.error() == QContact::NoError);

    // Make sure it's gone from both
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    QVERIFY(c2.details().count() == 1);
    QVERIFY(c2.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c2.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c2.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c2.detail<QContactPhoneNumber>().isEmpty());

    // add a, add b, remove a, add a, remove b, remove a
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c2.saveDetail(&p));
    QVERIFY(c2.error() == QContact::NoError);
    QVERIFY(c.removeDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c2.removeDetail(&p));
    QVERIFY(c2.error() == QContact::NoError);
    QVERIFY(c.removeDetail(&p));
    QVERIFY(c.error() == QContact::NoError);

    // Now add a detail with the same values twice
    QContactPhoneNumber one;
    QContactPhoneNumber two;

    one.setNumber("12345");
    two.setNumber("12345");

    // add it once
    QVERIFY(c.saveDetail(&one));
    QVERIFY(c.details().count() == 2);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 1);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(!c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(!c.detail<QContactPhoneNumber>().isEmpty());
    QCOMPARE(c.detail<QContactPhoneNumber>(), one);

    // add it twice
    QVERIFY(c.saveDetail(&two));
    QVERIFY(c.details().count() == 3);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 2);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 2);
    QVERIFY(!c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(!c.detail<QContactPhoneNumber>().isEmpty());
    QCOMPARE(c.detail<QContactPhoneNumber>(), one);
    QCOMPARE(c.details<QContactPhoneNumber>()[0], one);
    QCOMPARE(c.details<QContactPhoneNumber>()[1], two);

    // Remove it once
    QVERIFY(c.removeDetail(&one));
    QVERIFY(c.details().count() == 2);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 1);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(!c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(!c.detail<QContactPhoneNumber>().isEmpty());
    QCOMPARE(c.detail<QContactPhoneNumber>(), two);

    // Remove it twice
    QVERIFY(c.removeDetail(&two));
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());
}

void tst_QContact::actions()
{
    
}

void tst_QContact::preferences()
{
#if 0 // information() under test not implemented yet (requires QSFW)
    QContact c;

    // test first set
    QContactDetail det("TestId");
    det.setValue("test", QVariant("test1"));
    c.saveDetail(&det);
    QCOMPARE(c.isPreferredDetail("nonexistentAction", det), false);
    QCOMPARE(c.setPreferredDetail("nonexistentAction", det), true);
    QCOMPARE(c.isPreferredDetail("nonexistentAction", det), true);
    QCOMPARE(c.preferredDetail("nonexistentAction"), det);

    // test replacement
    QContactDetail det2("TestId");
    det2.setValue("test", QVariant("test2"));
    c.saveDetail(&det2);
    QCOMPARE(c.isPreferredDetail("nonexistentAction", det2), false);
    QCOMPARE(c.setPreferredDetail("nonexistentAction", det2), true);
    QCOMPARE(c.isPreferredDetail("nonexistentAction", det2), true);
    QCOMPARE(c.isPreferredDetail("nonexistnetAction", det), false);
    QCOMPARE(c.preferredDetail("nonexistentAction"), det2);

    // test for detail that is not part of the contact
    QContactDetail det3("TestId");
    det3.setValue("test", QVariant("test3"));
    QCOMPARE(c.setPreferredDetail("nonexistentAction", det3), false);
    QCOMPARE(c.preferredDetail("nonexistentAction", det2); // shouldn't have changed.
#endif
}

QTEST_MAIN(tst_QContact)
#include "tst_qcontact.moc"
