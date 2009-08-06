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

class tst_QContactDetails : public QObject
{
Q_OBJECT

public:
    tst_QContactDetails();
    virtual ~tst_QContactDetails();

public slots:
    void init();
    void cleanup();

private slots:
    // leaf class testing
    void address();
    void anniversary();
    void avatar();
    void birthday();
    void displayLabel();
    void emailAddress();
    void gender();
    void guid();
    void name();
    void organisation();
    void phoneNumber();
    void relationship();
    void syncTarget();
    void timestamp();
    void url();

    // custom definition testing
    void custom();
};

tst_QContactDetails::tst_QContactDetails()
{
}

tst_QContactDetails::~tst_QContactDetails()
{
}

void tst_QContactDetails::init()
{
}

void tst_QContactDetails::cleanup()
{
}

void tst_QContactDetails::address()
{
    QContact c;
    QContactAddress a1, a2;

    QVERIFY(a1.isEmpty());
    QVERIFY(a2.isEmpty());

    // test property set
    QCOMPARE(a1.displayLabel(), QString());
    a1.setStreet("68 Imaginary Avenue");
    a1.setCountry("Australia");
    QCOMPARE(a1.displayLabel(), QString("68 Imaginary Avenue\nAustralia"));
    QCOMPARE(a1.street(), QString("68 Imaginary Avenue"));
    a1.setDisplayLabel("1234");
    QCOMPARE(a1.displayLabel(), QString("1234"));

    // test property add
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactDisplayLabel::DefinitionName).count(), 1);
    QCOMPARE(QContactAddress(c.details(QContactAddress::DefinitionName).value(0)).displayLabel(), a1.displayLabel());
    a2.setStreet("Test");
    QVERIFY(c.saveDetail(&a2));
    QCOMPARE(c.details(QContactAddress::DefinitionName).count(), 2);

    // test property update
    a1.setValue("label","label1");
    a1.setDisplayLabel("12345");
    QVERIFY(c.saveDetail(&a1));

    // test property remove
    QVERIFY(c.removeDetail(&a1));
    QCOMPARE(c.details(QContactAddress::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&a2));
    QVERIFY(!c.removeDetail(&a2)); // cannot remove twice
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactAddress::DefinitionName).count(), 0);

    // test displayName permutations
    QContactAddress a3;
    QCOMPARE(a3.displayLabel(), QString());

    // set street
    a3.setStreet("Test");
    QCOMPARE(a3.displayLabel(), QString("Test"));
    QCOMPARE(a3.street(), QString("Test"));
    a3.setStreet("");
    QCOMPARE(a3.displayLabel(), QString());

    // set locality
    a3.setLocality("Test");
    QCOMPARE(a3.displayLabel(), QString("Test"));
    QCOMPARE(a3.locality(), QString("Test"));
    a3.setLocality("");
    QCOMPARE(a3.displayLabel(), QString());

    // set region
    a3.setRegion("Test");
    QCOMPARE(a3.displayLabel(), QString("Test"));
    QCOMPARE(a3.region(), QString("Test"));
    a3.setRegion("");
    QCOMPARE(a3.displayLabel(), QString());

    // set postcode
    a3.setPostcode("Test");
    QCOMPARE(a3.displayLabel(), QString("Test"));
    QCOMPARE(a3.postcode(), QString("Test"));
    a3.setPostcode("");
    QCOMPARE(a3.displayLabel(), QString());

    // set country
    a3.setCountry("Test");
    QCOMPARE(a3.displayLabel(), QString("Test"));
    QCOMPARE(a3.country(), QString("Test"));
    a3.setCountry("");
    QCOMPARE(a3.displayLabel(), QString());

    // street + locality
    a3.setStreet("Test");
    a3.setLocality("Test");
    QCOMPARE(a3.displayLabel(), QString("Test\nTest"));
    a3.setStreet("");

    // locality + region
    a3.setRegion("Test");
    QCOMPARE(a3.displayLabel(), QString("Test\nTest"));
    a3.setLocality("");

    // region + country
    a3.setCountry("Test");
    QCOMPARE(a3.displayLabel(), QString("Test\nTest"));
    a3.setRegion("");
    a3.setCountry("");

    // street + country
    a3.setStreet("Test");
    a3.setCountry("Test");
    QCOMPARE(a3.displayLabel(), QString("Test\nTest"));

    // street + locality + country
    a3.setStreet("Test1");
    a3.setLocality("Test2");
    a3.setCountry("Test3");
    QCOMPARE(a3.displayLabel(), QString("Test1\nTest2\nTest3"));

    // street + locality + region + country
    a3.setStreet("Test1");
    a3.setLocality("Test2");
    a3.setRegion("Test3");
    a3.setCountry("Test4");
    QCOMPARE(a3.displayLabel(), QString("Test1\nTest2\nTest3\nTest4"));

    // street + locality + region + postcode + country
    a3.setStreet("Test1");
    a3.setLocality("Test2");
    a3.setRegion("Test3");
    a3.setPostcode("Test4");
    a3.setCountry("Test5");
    QCOMPARE(a3.displayLabel(), QString("Test1\nTest2\nTest3, Test4\nTest5"));
}

void tst_QContactDetails::anniversary()
{
    QContact c;
    QContactAnniversary a1, a2;
    QDate currDate = QDate::currentDate();

    // test property set
    a1.setCalendarId("1234");
    QCOMPARE(a1.calendarId(), QString("1234"));
    QCOMPARE(a1.value(QContactAnniversary::FieldCalendarId), QString("1234"));
    a1.setEvent("4321");
    QCOMPARE(a1.event(), QString("4321"));
    QCOMPARE(a1.value(QContactAnniversary::FieldEvent), QString("4321"));
    a1.setOriginalDate(currDate);
    QCOMPARE(a1.originalDate(), currDate);
    QCOMPARE(a1.value<QDate>(QContactAnniversary::FieldOriginalDate), currDate);

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 1);
    QCOMPARE(QContactAnniversary(c.details(QContactAnniversary::DefinitionName).value(0)).event(), a1.event());

    // test property update
    a1.setValue("label","label1");
    a1.setCalendarId("12345");
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).value(0).value(QContactAnniversary::FieldCalendarId), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&a1));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&a2));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&a2));
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&a2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactAnniversary::DefinitionName).count(), 0);
}

void tst_QContactDetails::avatar()
{
    QContact c;
    QContactAvatar a1, a2;

    // test property set
    a1.setAvatar("1234");
    QCOMPARE(a1.avatar(), QString("1234"));
    QCOMPARE(a1.value(QContactAvatar::FieldAvatar), QString("1234"));

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 1);
    QCOMPARE(QContactAvatar(c.details(QContactAvatar::DefinitionName).value(0)).avatar(), a1.avatar());

    // test property update
    a1.setValue("label","label1");
    a1.setAvatar("12345");
    QVERIFY(c.saveDetail(&a1));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).value(0).value(QContactAvatar::FieldAvatar), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&a1));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&a2));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&a2));
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&a2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactAvatar::DefinitionName).count(), 0);
}

void tst_QContactDetails::birthday()
{
    QContact c;
    QContactBirthday b1, b2;
    QDate currDate = QDate::currentDate();

    // test property set
    b1.setDate(currDate);
    QCOMPARE(b1.date(), currDate);
    QCOMPARE(b1.value<QDate>(QContactBirthday::FieldBirthday), currDate);

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&b1));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 1);
    QCOMPARE(QContactBirthday(c.details(QContactBirthday::DefinitionName).value(0)).date(), b1.date());

    // test property update
    b1.setValue("label","label1");
    b1.setDate(currDate.addDays(3));
    QVERIFY(c.saveDetail(&b1));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).value(0).value<QDate>(QContactBirthday::FieldBirthday), currDate.addDays(3));

    // test property remove
    QVERIFY(c.removeDetail(&b1));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&b2));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&b2));
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&b2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactBirthday::DefinitionName).count(), 0);
}

void tst_QContactDetails::displayLabel()
{
    QContact c;
    QContactDisplayLabel d1, d2;

    // test property set
    d1.setLabel("label one");
    QCOMPARE(d1.label(), QString("label one"));
    QCOMPARE(d1.value(QContactDisplayLabel::FieldLabel), QString("label one"));

    // test property add
    QVERIFY(c.saveDetail(&d1));
    QCOMPARE(c.details(QContactDisplayLabel::DefinitionName).count(), 1);
    QCOMPARE(QContactDisplayLabel(c.details(QContactDisplayLabel::DefinitionName).value(0)).label(), d1.label());

    // test property update
    d1.setLabel("label two");
    QVERIFY(c.saveDetail(&d1));
    QCOMPARE(c.details(QContactDisplayLabel::DefinitionName).value(0).value(QContactDisplayLabel::FieldLabel), QString("label two"));

    // test property remove
    QVERIFY(c.removeDetail(&d1));
    QVERIFY(c.error() == QContact::NoError); // should successfully _clear_ (but not remove) the label
    QCOMPARE(c.displayLabel().label(), QString());
    QCOMPARE(c.details(QContactDisplayLabel::DefinitionName).count(), 1); // cannot remove display label
    d2.setLabel("second label");
    QVERIFY(c.saveDetail(&d2));    // should successfully _replace_ the label
    QVERIFY(c.error() == QContact::NoError); // should successfully _clear_ (but not remove) the label
    QCOMPARE(c.displayLabel().label(), QString("second label"));
    QCOMPARE(c.details(QContactDisplayLabel::DefinitionName).count(), 1);
}

void tst_QContactDetails::emailAddress()
{
    QContact c;
    QContactEmailAddress e1, e2;

    // test property set
    e1.setEmailAddress("1234");
    QCOMPARE(e1.emailAddress(), QString("1234"));
    QCOMPARE(e1.value(QContactEmailAddress::FieldEmailAddress), QString("1234"));

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&e1));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 1);
    QCOMPARE(QContactEmailAddress(c.details(QContactEmailAddress::DefinitionName).value(0)).emailAddress(), e1.emailAddress());

    // test property update
    e1.setValue("label","label1");
    e1.setEmailAddress("12345");
    QVERIFY(c.saveDetail(&e1));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).value(0).value(QContactEmailAddress::FieldEmailAddress), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&e1));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&e2));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&e2));
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&e2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactEmailAddress::DefinitionName).count(), 0);
}

void tst_QContactDetails::gender()
{
    QContact c;
    QContactGender g1, g2;

    // test property set
    g1.setGender("1234");
    QCOMPARE(g1.gender(), QString("1234"));
    QCOMPARE(g1.value(QContactGender::FieldGender), QString("1234"));

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 1);
    QCOMPARE(QContactGender(c.details(QContactGender::DefinitionName).value(0)).gender(), g1.gender());

    // test property update
    g1.setValue("label","label1");
    g1.setGender("12345");
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGender::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactGender::DefinitionName).value(0).value(QContactGender::FieldGender), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&g1));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&g2));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&g2));
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&g2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactGender::DefinitionName).count(), 0);
}

void tst_QContactDetails::guid()
{
    QContact c;
    QContactGuid g1, g2;

    // test property set
    g1.setGuid("1234");
    QCOMPARE(g1.guid(), QString("1234"));
    QCOMPARE(g1.value(QContactGuid::FieldGuid), QString("1234"));

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 1);
    QCOMPARE(QContactGuid(c.details(QContactGuid::DefinitionName).value(0)).guid(), g1.guid());

    // test property update
    g1.setValue("label","label1");
    g1.setGuid("12345");
    QVERIFY(c.saveDetail(&g1));
    QCOMPARE(c.details(QContactGuid::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactGuid::DefinitionName).value(0).value(QContactGuid::FieldGuid), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&g1));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&g2));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&g2));
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&g2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactGuid::DefinitionName).count(), 0);
}

void tst_QContactDetails::name()
{
    QContact c;
    QContactName n1, n2;

    QVERIFY(n1.isEmpty());
    QVERIFY(n2.isEmpty());

    // test property set
    n1.setPrefix("Dr");
    n1.setFirst("Freddy");
    n1.setMiddle("William Preston");
    n1.setLast("Gumboots");
    n1.setSuffix("Esquire");
    QCOMPARE(n1.prefix(), QString("Dr"));
    QCOMPARE(n1.first(), QString("Freddy"));
    QCOMPARE(n1.middle(), QString("William Preston"));
    QCOMPARE(n1.last(), QString("Gumboots"));
    QCOMPARE(n1.suffix(), QString("Esquire"));

    // test property add
    QVERIFY(c.saveDetail(&n1));
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
    n2.setFirst("Billy");
    n2.setLast("Galoshes");
    QVERIFY(c.saveDetail(&n2));
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 2);

    // test property update
    n1.setValue("label","label1");
    QVERIFY(c.saveDetail(&n1));

    // test property remove
    QVERIFY(c.removeDetail(&n1)); // remove
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
    QVERIFY(c.saveDetail(&n2));   // save but already added; count remains the same.
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&n2)); // remove it
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 0);
    QVERIFY(!c.removeDetail(&n2));// remove now fails
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QVERIFY(c.saveDetail(&n1));   // save the original name
    QCOMPARE(c.details(QContactName::DefinitionName).count(), 1);
}

void tst_QContactDetails::organisation()
{
    QContact c;
    QContactOrganisation o1, o2;

    // test property set
    o1.setDisplayLabel("organisation one");
    QCOMPARE(o1.displayLabel(), QString("organisation one"));
    QCOMPARE(o1.value(QContactOrganisation::FieldDisplayLabel), QString("organisation one"));

    // test property add
    QVERIFY(c.saveDetail(&o1));
    QCOMPARE(c.details(QContactOrganisation::DefinitionName).count(), 1);
    QCOMPARE(QContactOrganisation(c.details(QContactOrganisation::DefinitionName).value(0)).displayLabel(), o1.displayLabel());

    // test property update
    o1.setDisplayLabel("organisation two");
    QVERIFY(c.saveDetail(&o1));
    QCOMPARE(c.details(QContactOrganisation::DefinitionName).value(0).value(QContactOrganisation::FieldDisplayLabel), QString("organisation two"));

    // test property remove
    QVERIFY(c.removeDetail(&o1));
    QCOMPARE(c.details(QContactOrganisation::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&o2));
    QCOMPARE(c.details(QContactOrganisation::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&o2));
    QCOMPARE(c.details(QContactOrganisation::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&o2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactOrganisation::DefinitionName).count(), 0);
}

void tst_QContactDetails::phoneNumber()
{
    QContact c;
    QContactPhoneNumber p1, p2;

    // test property set
    p1.setNumber("1234");
    QCOMPARE(p1.number(), QString("1234"));
    QCOMPARE(p1.value(QContactPhoneNumber::FieldNumber), QString("1234"));

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&p1));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 1);
    QCOMPARE(QContactPhoneNumber(c.details(QContactPhoneNumber::DefinitionName).value(0)).number(), p1.number());

    // test property update
    p1.setValue("label","label1");
    p1.setNumber("12345");
    QVERIFY(c.saveDetail(&p1));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).value(0).value(QContactPhoneNumber::FieldNumber), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&p1));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&p2));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&p2));
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&p2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactPhoneNumber::DefinitionName).count(), 0);
}

void tst_QContactDetails::relationship()
{
    QContact c;
    QContactRelationship r1, r2;

    // test property set
    r1.setRelatedContactUid("contact12345");
    QCOMPARE(r1.relatedContactUid(), QString("contact12345"));
    QCOMPARE(r1.value(QContactRelationship::FieldRelatedContactUid), QString("contact12345"));

    // test property add
    QVERIFY(c.saveDetail(&r1));
    QCOMPARE(c.details(QContactRelationship::DefinitionName).count(), 1);
    QCOMPARE(QContactRelationship(c.details(QContactRelationship::DefinitionName).value(0)).relatedContactUid(), r1.relatedContactUid());

    // test property update
    r1.setRelatedContactUid("contact54321");
    QVERIFY(c.saveDetail(&r1));
    QCOMPARE(c.details(QContactRelationship::DefinitionName).value(0).value(QContactRelationship::FieldRelatedContactUid), QString("contact54321"));

    // test property remove
    QVERIFY(c.removeDetail(&r1));
    QCOMPARE(c.details(QContactRelationship::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&r2));
    QCOMPARE(c.details(QContactRelationship::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&r2));
    QCOMPARE(c.details(QContactRelationship::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&r2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactRelationship::DefinitionName).count(), 0);
}

void tst_QContactDetails::syncTarget()
{
    QContact c;
    QContactSyncTarget s1, s2;

    // test property set
    s1.setSyncTarget("1234");
    QCOMPARE(s1.syncTarget(), QString("1234"));
    QCOMPARE(s1.value(QContactSyncTarget::FieldSyncTarget), QString("1234"));

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&s1));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 1);
    QCOMPARE(QContactSyncTarget(c.details(QContactSyncTarget::DefinitionName).value(0)).syncTarget(), s1.syncTarget());

    // test property update
    s1.setValue("label","label1");
    s1.setSyncTarget("12345");
    QVERIFY(c.saveDetail(&s1));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).value(0).value(QContactSyncTarget::FieldSyncTarget), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&s1));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&s2));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&s2));
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&s2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactSyncTarget::DefinitionName).count(), 0);
}

void tst_QContactDetails::timestamp()
{
    QContact c;
    QContactTimestamp t1, t2;
    QDateTime modified = QDateTime::currentDateTime();
    QDateTime created = modified.addSecs(-43);

    // test property set
    t1.setCreated(created);
    QCOMPARE(t1.created(), created);
    QCOMPARE(t1.variantValue(QContactTimestamp::FieldCreationTimestamp).toDateTime(), created);

    // test property add
    QVERIFY(c.saveDetail(&t1));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 1);
    QCOMPARE(QContactTimestamp(c.details(QContactTimestamp::DefinitionName).value(0)).created(), t1.created());

    // test property update
    t1.setValue("label","label1");
    t1.setLastModified(modified);
    QVERIFY(c.saveDetail(&t1));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).value(0).variantValue(QContactTimestamp::FieldCreationTimestamp).toDateTime(), created);
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).value(0).variantValue(QContactTimestamp::FieldModificationTimestamp).toDateTime(), modified);

    // test property remove
    QVERIFY(c.removeDetail(&t1));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 0);
    t2.setCreated(created.addSecs(15));
    QVERIFY(c.saveDetail(&t2));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&t2));
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&t2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactTimestamp::DefinitionName).count(), 0);
}

void tst_QContactDetails::url()
{
    QContact c;
    QContactUrl u1, u2;

    // test property set
    u1.setUrl("1234");
    QCOMPARE(u1.url(), QString("1234"));
    QCOMPARE(u1.value(QContactUrl::FieldUrl), QString("1234"));

    // test property attributes
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
//    p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
//    QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&u1));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 1);
    QCOMPARE(QContactUrl(c.details(QContactUrl::DefinitionName).value(0)).url(), u1.url());

    // test property update
    u1.setValue("label","label1");
    u1.setUrl("12345");
    QVERIFY(c.saveDetail(&u1));
    QCOMPARE(c.details(QContactUrl::DefinitionName).value(0).value("label"), QString("label1"));
    QCOMPARE(c.details(QContactUrl::DefinitionName).value(0).value(QContactUrl::FieldUrl), QString("12345"));

    // test property remove
    QVERIFY(c.removeDetail(&u1));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 0);
    QVERIFY(c.saveDetail(&u2));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 1);
    QVERIFY(c.removeDetail(&u2));
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 0);
    QVERIFY(c.removeDetail(&u2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details(QContactUrl::DefinitionName).count(), 0);
}

void tst_QContactDetails::custom()
{
    QContact c;
    QContactDetail c1("mycustom"), c2("mycustom");

    // test property set
    c1.setValue("custom", "1234");
    QCOMPARE(c1.value("custom"), QString("1234"));

    // test property attributes
    //QCOMPARE(p1.attributes(), QContactPhoneNumber::Landline); // default value
    //p1.setAttributes(QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);
    //QCOMPARE(p1.attributes(), QContactPhoneNumber::Mobile | QContactPhoneNumber::Voip);

    // test property add
    QVERIFY(c.saveDetail(&c1));
    QCOMPARE(c.details("mycustom").count(), 1);
    QCOMPARE((c.details("mycustom").value(0)).value("custom"), c1.value("custom"));

    // test property update
    c1.setValue("label","label1");
    c1.setValue("custom", "12345");
    QVERIFY(c.saveDetail(&c1));

    // test property remove
    QVERIFY(c.removeDetail(&c1));
    QCOMPARE(c.details("mycustom").count(), 0);
    QVERIFY(c.saveDetail(&c2));
    QCOMPARE(c.details("mycustom").count(), 1);
    QVERIFY(c.removeDetail(&c2));
    QCOMPARE(c.details("mycustom").count(), 0);
    QVERIFY(c.removeDetail(&c2) == false);
    QVERIFY(c.error() == QContact::DetailDoesNotExistError);
    QCOMPARE(c.details("mycustom").count(), 0);
}


QTEST_MAIN(tst_QContactDetails)
#include "tst_qcontactdetails.moc"
