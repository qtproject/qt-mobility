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

class tst_QContactGroup: public QObject
{
Q_OBJECT

public:
    tst_QContactGroup();
    virtual ~tst_QContactGroup();

public slots:
    void init();
    void cleanup();
private slots:
    void ctors();
    void name();
    void members();
    void id();
    void empty();
};

tst_QContactGroup::tst_QContactGroup()
{
}

tst_QContactGroup::~tst_QContactGroup()
{
}

void tst_QContactGroup::init()
{
}

void tst_QContactGroup::cleanup()
{
}

void tst_QContactGroup::ctors()
{
    QContactGroup g;
    QVERIFY(g.addMember(QUniqueId(1)));
    QCOMPARE(g.members().count(), 1);

    QContactGroup g2 = g;
    QCOMPARE(g2.members().count(), 1);

    QContactGroup g3(g2);
    QCOMPARE(g3.members().count(), 1);
}

void tst_QContactGroup::name()
{
    QContactGroup g;
    QVERIFY(g.name().isEmpty());
    g.setName("test");
    QCOMPARE(g.name(), QString("test"));

    QContactGroup g2;
    g2 = g;
    QCOMPARE(g2.name(), g.name());
    g2.setName("test2");
    QCOMPARE(g.name(), QString("test"));
    QCOMPARE(g2.name(), QString("test2"));
}

void tst_QContactGroup::members()
{
    QContact c;
    c.setId(QUniqueId(224));
    QContactName n;
    n.setFirst("John");
    n.setLast("Citizen");
    c.saveDetail(&n);

    // create a new group
    QContactGroup g;
    QVERIFY(g.members().isEmpty());

    // test add
    QVERIFY(g.addMember(c.id()));
    QCOMPARE(g.members().size(), 1);
    QVERIFY(g.hasMember(c.id()));

    // test remove
    QVERIFY(g.removeMember(c.id()));
    QVERIFY(g.members().isEmpty());
    QVERIFY(!g.hasMember(c.id()));

    // test multiple adds
    int currentCount = 0;
    for (int i = 1; i < 50; i++) {
        QCOMPARE(g.hasMember(QUniqueId(i)), false);
        QVERIFY(g.addMember(QUniqueId(i)));
        currentCount += 1;
        QCOMPARE(g.hasMember(QUniqueId(i)), true);
        QCOMPARE(g.members().count(), currentCount);
        QCOMPARE(g.error(), QContactGroup::NoError);
    }

    while (currentCount > 0) {
        QVERIFY(g.removeMember(QUniqueId(currentCount)));
        currentCount -= 1;
        QCOMPARE(g.members().count(), currentCount);
        QCOMPARE(g.error(), QContactGroup::NoError);
    }

    // test that adding the same contact twice fails properly
    QVERIFY(g.addMember(QUniqueId(1)));
    QVERIFY(!g.addMember(QUniqueId(1)));
    QCOMPARE(g.error(), QContactGroup::ContactAlreadyInGroupError);
    QCOMPARE(g.members().count(), 1);

    // test removing a non-existent contact fails properly
    QVERIFY(!g.removeMember(QUniqueId(2)));
    QCOMPARE(g.error(), QContactGroup::ContactNotInGroupError);
    QCOMPARE(g.members().count(), 1);
}

void tst_QContactGroup::id()
{
    QContactGroup g;
    g.setId(QUniqueId(1));
    QCOMPARE(g.id(), QUniqueId(1));
    g.setId(QUniqueId(2));
    QCOMPARE(g.id(), QUniqueId(2));
}

void tst_QContactGroup::empty()
{
    QContactGroup g;
    QVERIFY(g.isEmpty());

    g.setName("Name");
    QVERIFY(!g.isEmpty());

    g.setName(QString());
    QVERIFY(g.isEmpty());

    g.setId(243234);
    QVERIFY(!g.isEmpty());

    g.setId(0);
    QVERIFY(g.isEmpty());

    g.addMember(123);
    QVERIFY(!g.isEmpty());

    g.removeMember(123);
    QVERIFY(g.isEmpty());

    g.setId(1123);
    g.addMember(123);
    QVERIFY(!g.isEmpty());

    g.removeMember(123);
    QVERIFY(!g.isEmpty());

    g.setId(0);
    QVERIFY(g.isEmpty());
}

QTEST_MAIN(tst_QContactGroup)
#include "tst_qcontactgroup.moc"
