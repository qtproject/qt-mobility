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

private slots:
    void details();
    void actions();
    void preferences();
    void displayName();
    void emptiness();
};

tst_QContact::tst_QContact()
{
}

tst_QContact::~tst_QContact()
{
}

void tst_QContact::details()
{
    QContact c;

    // Test there are no details (apart from the name :/) by default
    QVERIFY(c.isEmpty() == true);
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    // Test retrieving the first detail (the display label)
    QContactDetail detail = c.detail(QString());
    QVERIFY(detail.definitionName() == QContactDisplayLabel::staticType());

    detail = c.detail("nonexistent");
    QVERIFY(detail.isEmpty());
    QVERIFY(detail.definitionName().isEmpty());

    QVERIFY(c.details("nonexistent").count() == 0);

    // Add a detail
    QContactPhoneNumber p;
    p.setNumber("12345678");
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.isEmpty() == false);

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
    QVERIFY(c.isEmpty() == true);
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
    QVERIFY(c.isEmpty() == false);
    QVERIFY(c.details().count() == 2);
    QContactPhoneNumber p2 = c.detail(QContactPhoneNumber::staticType());
    QCOMPARE(p, p2);

    QVERIFY(c.removeDetail(&p2));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.isEmpty() == true);
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
    QVERIFY(c2.isEmpty() == true);
    QVERIFY(c.saveDetail(&p));
    QVERIFY(c.error() == QContact::NoError);
    QVERIFY(c2.saveDetail(&p));
    QVERIFY(c2.error() == QContact::NoError);
    QVERIFY(c2.isEmpty() == false);

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
    QVERIFY(c.isEmpty() == true);
    QVERIFY(c.details().count() == 1);
    QVERIFY(c.details(QContactPhoneNumber::staticType()).count() == 0);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 0);
    QVERIFY(c.detail(QContactPhoneNumber::staticType()).isEmpty());
    QVERIFY(c.detail<QContactPhoneNumber>().isEmpty());

    // but not the second
    QVERIFY(c2.isEmpty() == false);
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

    // Null pointer tests
    QVERIFY(c.saveDetail(0) == false);
    QVERIFY(c.error() == QContact::BadArgumentError);

    QVERIFY(c.removeDetail(0) == false);
    QVERIFY(c.error() == QContact::BadArgumentError);

}

void tst_QContact::actions()
{
    QContact c;
    QContactEmailAddress e;
    e.setEmailAddress("test@nokia.com");
    c.saveDetail(&e);

    // Prior to plugin loading:
    QStringList availableActions = c.availableActions();
    QVERIFY(availableActions.isEmpty());
    QContactDetail d = c.detailWithAction("SendEmail");
    QVERIFY(d.isEmpty());
    QList<QContactDetail> dets = c.detailsWithAction("SendEmail");
    QVERIFY(dets.isEmpty());

    // set the correct path to look for plugins and load them
    QString path = QApplication::applicationDirPath() + "/dummyplugin/plugins/";
    QApplication::addLibraryPath(path);
    QContactManager cm;

    // available actions - should be one there now.
    availableActions = c.availableActions();
    QVERIFY(availableActions.contains("SendEmail"));

    // detail with action
    d = c.detailWithAction("SendEmail");
    QVERIFY(d == e);
    QCOMPARE(c.error(), QContact::NoError);
    d = c.detailWithAction("NonexistentAction");
    QCOMPARE(c.error(), QContact::DetailDoesNotExistError);
    d = c.detailWithAction(QString());
    QCOMPARE(c.error(), QContact::BadArgumentError);

    // details with action
    dets = c.detailsWithAction("SendEmail");
    QVERIFY(dets.contains(e));
    dets = c.detailsWithAction("NonexistentAction");
    QCOMPARE(c.error(), QContact::DetailDoesNotExistError);
    dets = c.detailsWithAction(QString());
    QCOMPARE(c.error(), QContact::BadArgumentError);

    // remove the library path.
    QApplication::removeLibraryPath(path);
}

void tst_QContact::preferences()
{
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
    QCOMPARE(c.isPreferredDetail("nonexistentAction", det), false);
    QCOMPARE(c.preferredDetail("nonexistentAction"), det2);

    // test for detail that is not part of the contact
    QContactDetail det3("TestId");
    det3.setValue("test", QVariant("test3"));
    QCOMPARE(c.setPreferredDetail("nonexistentAction", det3), false);
    QCOMPARE(c.preferredDetail("nonexistentAction"), det2); // shouldn't have changed.

    // test invalid set
    QCOMPARE(c.setPreferredDetail(QString(), det3), false);
    QCOMPARE(c.setPreferredDetail(QString(), QContactDetail()), false);
    QCOMPARE(c.setPreferredDetail("nonexistentAction", QContactDetail()), false);
    QCOMPARE(c.preferredDetail("nonexistentAction"), det2); // shouldn't have changed.

    // test invalid query
    QContactDetail det4;
    det4.setValue("test", QVariant("test4"));
    c.saveDetail(&det4);
    QCOMPARE(c.isPreferredDetail(QString(), QContactDetail()), false);
    QCOMPARE(c.isPreferredDetail(QString(), det4), false); // valid detail, but no pref set.
    QCOMPARE(c.isPreferredDetail("nonexistentAction", QContactDetail()), false);
}

void tst_QContact::displayName()
{
    QContact c;
    QContactManager cm("memory"); // for formatting names

    QContactDisplayLabel label = c.displayLabel();
    QVERIFY(label.label().isEmpty());
    QVERIFY(c.displayLabel().isSynthesised() == true);

    label.setLabel("Wesley Wentworth Worrier");
    QVERIFY(label.isEmpty() == false);
    QVERIFY(label.label().isEmpty() == false);
    QVERIFY(label.isSynthesised() == false);

    label.setSynthesised(true);

    QVERIFY(c.setDisplayLabel(label));
    QVERIFY(c.displayLabel().label() == "Wesley Wentworth Worrier");
    QVERIFY(c.displayLabel().isSynthesised() == true);

    //label.setLabel(QString());
    //QVERIFY(label.isSynthesised() == true);

    /* Clear the label again */
    QVERIFY(c.setDisplayLabel(QString()));
    QVERIFY(c.displayLabel().label().isEmpty());
    QVERIFY(c.displayLabel().isSynthesised() == true);

    /* Use the string mutator */
    QVERIFY(c.setDisplayLabel("Wesley Wentworth Worrier"));
    QVERIFY(c.displayLabel().label() == "Wesley Wentworth Worrier");
    QVERIFY(c.displayLabel().isSynthesised() == false);

    /* Try to make this a bit more consistent by using a single name */
    QContact d;
    QContactName name;
    name.setFirst("Wesley");

    QVERIFY(d.displayLabel().label().isEmpty());
    QVERIFY(d.saveDetail(&name));

    /*
     * The display label is not updated until you save the contact, or
     * do it manually.
     */
    QString synth = cm.synthesiseDisplayLabel(d);

    QVERIFY(d.displayLabel().label().isEmpty());
    QVERIFY(d.displayLabel().isSynthesised() == true);

    QVERIFY(synth == name.first()); // XXX Perhaps not guaranteed

    /* Set something else */
    QVERIFY(d.setDisplayLabel("The grand old duchess"));
    QVERIFY(d.displayLabel().label() == "The grand old duchess");
    QVERIFY(d.displayLabel().isSynthesised() == false);

    /* Remove the detail via removeDetail */
    QContactDisplayLabel old = d.displayLabel();
    QVERIFY(d.details().count() == 2);
    QVERIFY(d.removeDetail(&old));
    QVERIFY(d.isEmpty() == false);
    QVERIFY(d.details().count() == 2); // it should not be removed, only cleared (!)

    /* Make sure we go back to the old synth version */
    QVERIFY(d.displayLabel().isSynthesised() == true);
    QVERIFY(d.displayLabel().label().isEmpty());
}

void tst_QContact::emptiness()
{
    QContact c;
    QVERIFY(c.isEmpty() == true);

    QContactDisplayLabel label = c.displayLabel();
    QVERIFY(label.label().isEmpty());

    label.setLabel("Wesley Wentworth Worrier");
    QVERIFY(label.isEmpty() == false);
    QVERIFY(label.label().isEmpty() == false);

    QVERIFY(c.setDisplayLabel(label));
    QVERIFY(c.isEmpty() == false);

    QVERIFY(c.displayLabel().label() == "Wesley Wentworth Worrier");
    QVERIFY(c.displayLabel().isSynthesised() == false);

    QVERIFY(c.setDisplayLabel(QString()));
    QVERIFY(c.isEmpty() == true);
}

QTEST_MAIN(tst_QContact)
#include "tst_qcontact.moc"
