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

class tst_QContactManager : public QObject
{
Q_OBJECT

public:
    tst_QContactManager();
    virtual ~tst_QContactManager();

private:
    void dumpContactDifferences(const QContact& a, const QContact& b);
    void dumpContact(const QContact &c);
    void dumpContacts();

    void addManagers(); // add standard managers to the data
public slots:
    void init();
    void cleanup();
private slots:
    void uriParsing();
    void nullIdOperations();
    void add();
    void update();
    void remove();
    void ctors();
    void invalidManager();
    void memoryManager();
    void referenceCounting();
    void contactValidation();
    void signalEmission();
    void groups();
    void batch();

    void errorStayingPut();

    /* data providers */
    void add_data() {addManagers();}
    void update_data() {addManagers();}
    void remove_data() {addManagers();}
    void groups_data() {addManagers();}
    void batch_data() {addManagers();}

    void uriParsing_data();
    void referenceCounting_data();
};

tst_QContactManager::tst_QContactManager()
{
}

tst_QContactManager::~tst_QContactManager()
{
}

void tst_QContactManager::init()
{
    /* Make sure these other test plugins are NOT loaded by default */
    QVERIFY(!QContactManager::availableManagers().contains("testdummy"));
    QVERIFY(!QContactManager::availableManagers().contains("teststaticdummy"));
}

void tst_QContactManager::cleanup()
{
}

// to get QFETCH to work with the template expression...
typedef QMap<QString,QString> tst_QContactManager_QStringMap;
Q_DECLARE_METATYPE(tst_QContactManager_QStringMap);

void tst_QContactManager::dumpContactDifferences(const QContact& ca, const QContact& cb)
{
    // Try to narrow down the differences
    QContact a(ca);
    QContact b(cb);

    // Check the name components in more detail
    QCOMPARE(a.name().first(), b.name().first());
    QCOMPARE(a.name().middle(), b.name().middle());
    QCOMPARE(a.name().last(), b.name().last());
    QCOMPARE(a.name().prefix(), b.name().prefix());
    QCOMPARE(a.name().suffix(), b.name().suffix());
    QCOMPARE(a.name().displayName(), b.name().displayName());

    QList<QContactDetail> aDetails = a.details();
    QList<QContactDetail> bDetails = b.details();

    // They can be in any order, so loop
    // First remove any matches
    foreach(QContactDetail d, aDetails) {
        foreach(QContactDetail d2, bDetails) {
            if(d == d2) {
                a.removeDetail(&d);
                b.removeDetail(&d2);
                break;
            }
        }
    }

    // Now dump the extra details that were unmatched in A
    aDetails = a.details();
    bDetails = b.details();
    foreach(QContactDetail d, aDetails) {
        qDebug() << "A contact had extra detail:" << d.definitionId() << d.attributes() << d.values();
    }
    // and same for B
    foreach(QContactDetail d, bDetails) {
        qDebug() << "B contact had extra detail:" << d.definitionId() << d.attributes() << d.values();
    }

    QCOMPARE(b, a);
}

void tst_QContactManager::dumpContact(const QContact& contact)
{
    qDebug() << "Contact: " << contact.id() << "(" << contact.name().displayName() << ")";
    QList<QContactDetail> details = contact.details();
    foreach(QContactDetail d, details) {
        qDebug() << "  " << d.definitionId() << ":";
        qDebug() << "    Attr:" << d.attributes();
        qDebug() << "    Vals:" << d.values();
    }
}

void tst_QContactManager::dumpContacts()
{
    QContactManager m;
    QList<QUniqueId> ids = m.contacts();

    foreach(QUniqueId id, ids) {
        QContact c = m.contact(id);
        dumpContact(c);
    }
}

void tst_QContactManager::nullIdOperations()
{
    QContactManager cm;
    QVERIFY(!cm.removeContact(QUniqueId()));
    QVERIFY(cm.error() == QContactManager::DoesNotExistError);

    QVERIFY(!cm.removeGroup(QUniqueId()));
    QVERIFY(cm.error() == QContactManager::DoesNotExistError);

    QContact c = cm.contact(QUniqueId());
    QVERIFY(c.id() == 0);
    QVERIFY(cm.error() == QContactManager::DoesNotExistError);
}

void tst_QContactManager::uriParsing_data()
{
    QTest::addColumn<QString>("uri");
    QTest::addColumn<bool>("good"); // is this a good uri or not
    QTest::addColumn<QString>("manager");
    QTest::addColumn<QMap<QString, QString> >("parameters");

    QMap<QString, QString> inparameters;
    inparameters.insert("foo", "bar");
    inparameters.insert("bazflag", QString());
    inparameters.insert("bar", "glob");

    QMap<QString, QString> inparameters2;
    inparameters2.insert("this has spaces", QString());
    inparameters2.insert("and& an", " &amp;");
    inparameters2.insert("and an ", "=quals");

    QTest::newRow("built") << QContactManager::buildUri("manager", inparameters) << true << "manager" << inparameters;
    QTest::newRow("built with escaped parameters") << QContactManager::buildUri("manager", inparameters2) << true << "manager" << inparameters2;
    QTest::newRow("no scheme") << "this should not split" << false << QString() << tst_QContactManager_QStringMap();
    QTest::newRow("wrong scheme") << "invalidscheme:foo bar" << false << QString() << tst_QContactManager_QStringMap();
    QTest::newRow("right scheme, no colon") << "qtcontacts" << false << QString() << tst_QContactManager_QStringMap();
    QTest::newRow("no manager, colon, no params") << "qtcontacts::" << false  << "manager" << tst_QContactManager_QStringMap();
    QTest::newRow("yes manager, no colon, no params") << "qtcontacts:manager" << true << "manager" << tst_QContactManager_QStringMap();
    QTest::newRow("yes manager, yes colon, no params") << "qtcontacts:manager:" << true << "manager"<< tst_QContactManager_QStringMap();
    QTest::newRow("yes params") << "qtcontacts:manager:foo=bar&bazflag=&bar=glob" << true << "manager" << inparameters;
    QTest::newRow("yes params but misformed") << "qtcontacts:manager:foo=bar&=gloo&bar=glob" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 2") << "qtcontacts:manager:=&=gloo&bar=glob" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 3") << "qtcontacts:manager:==" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 4") << "qtcontacts:manager:&&" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 5") << "qtcontacts:manager:&goo=bar" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 6") << "qtcontacts:manager:goo&bar" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 7") << "qtcontacts:manager:goo&bar&gob" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 8") << "qtcontacts:manager:==&&==&goo=bar" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 9") << "qtcontacts:manager:foo=bar=baz" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 10") << "qtcontacts:manager:foo=bar=baz=glob" << false << "manager" << inparameters;
    QTest::newRow("no manager but yes params") << "qtcontacts::foo=bar&bazflag=&bar=glob" << false << QString() << inparameters;
    QTest::newRow("no manager or params") << "qtcontacts::" << false << QString() << inparameters;
    QTest::newRow("no manager or params or colon") << "qtcontacts:" << false << QString() << inparameters;
}

void tst_QContactManager::addManagers()
{
    QTest::addColumn<QString>("uri");

    QStringList managers = QContactManager::availableManagers();

    /* Known bad ones */
    managers.removeAll("invalid");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        if (mgr == "memory") {
            params.insert("id", "tst_QContactManager");
        }
        QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
    }
}

void tst_QContactManager::uriParsing()
{
    QFETCH(QString, uri);
    QFETCH(bool, good);
    QFETCH(QString, manager);
    QFETCH(tst_QContactManager_QStringMap, parameters);

    QString outmanager;
    QMap<QString, QString> outparameters;

    if (good) {
        /* Good split */
        /* Test splitting */
        QVERIFY(QContactManager::splitUri(uri, 0, 0)); // no out parms

        // 1 out param
        QVERIFY(QContactManager::splitUri(uri, &outmanager, 0));
        QCOMPARE(manager, outmanager);
        QVERIFY(QContactManager::splitUri(uri, 0, &outparameters));
        QCOMPARE(parameters, outparameters);

        outmanager.clear();
        outparameters.clear();
        QVERIFY(QContactManager::splitUri(uri, &outmanager, &outparameters));
        QCOMPARE(manager, outmanager);
        QCOMPARE(parameters, outparameters);
    } else {
        /* bad splitting */
        outmanager.clear();
        outparameters.clear();
        QVERIFY(QContactManager::splitUri(uri, 0, 0) == false);
        QVERIFY(QContactManager::splitUri(uri, &outmanager, 0) == false);
        QVERIFY(outmanager.isEmpty());
        QVERIFY(QContactManager::splitUri(uri, 0, &outparameters) == false);
        QVERIFY(outparameters.isEmpty());

        /* make sure the in parameters don't change with a bad split */
        outmanager = manager;
        outparameters = parameters;
        QVERIFY(QContactManager::splitUri(uri, &outmanager, 0) == false);
        QCOMPARE(manager, outmanager);
        QVERIFY(QContactManager::splitUri(uri, 0, &outparameters) == false);
        QCOMPARE(parameters, outparameters);
    }
}

void tst_QContactManager::ctors()
{
    /* test the different ctors to make sure we end up with the same uri */
    QVERIFY(QContactManager::availableManagers().count() > 1); // invalid + something else
    QVERIFY(QContactManager::availableManagers().contains("invalid"));
    QString defaultStore = QContactManager::availableManagers().value(0);
    QVERIFY(defaultStore != "invalid");

    qDebug() << "Available managers:" << QContactManager::availableManagers();

    QMap<QString, QString> randomParameters;
    randomParameters.insert("something", "old");
    randomParameters.insert("something...", "new");
    randomParameters.insert("something ", "borrowed");
    randomParameters.insert(" something", "blue");

    QContactManager cm; // default
    QContactManager cm2(defaultStore);
    QContactManager cm3(defaultStore, QMap<QString, QString>());
    QContactManager cm4(cm.storeUri());
    QContactManager cm5 = QContactManager::fromUri(QContactManager::buildUri(defaultStore, QMap<QString, QString>()));
    QContactManager cm6 = QContactManager::fromUri(cm.storeUri());
    QContactManager cm7(cm);
    QContactManager cm8;
    cm8 = cm;
    QContactManager cm9 = QContactManager::fromUri(QString());

    QCOMPARE(cm.storeUri(), cm2.storeUri());
    QCOMPARE(cm.storeUri(), cm3.storeUri());
    QCOMPARE(cm.storeUri(), cm5.storeUri());
    QCOMPARE(cm.storeUri(), cm6.storeUri());
    QCOMPARE(cm.storeUri(), cm7.storeUri());
    QCOMPARE(cm.storeUri(), cm8.storeUri());
    QCOMPARE(cm.storeUri(), cm9.storeUri());

    QVERIFY(cm.storeUri() != cm4.storeUri()); // don't pass a uri to the ctor

    /* Test self assignment doesn't do anything measurable */
    cm = cm;

    QCOMPARE(cm.storeUri(), cm2.storeUri());
    QCOMPARE(cm.storeUri(), cm3.storeUri());
    QCOMPARE(cm.storeUri(), cm5.storeUri());
    QCOMPARE(cm.storeUri(), cm6.storeUri());
    QCOMPARE(cm.storeUri(), cm7.storeUri());
    QCOMPARE(cm.storeUri(), cm8.storeUri());
    QCOMPARE(cm.storeUri(), cm9.storeUri());

    QVERIFY(cm.storeUri() != cm4.storeUri()); // don't pass a uri to the ctor


    /* Test that we get invalid stores when we do silly things */
    QContactManager em("non existent");
    QContactManager em2("non existent", QMap<QString, QString>());
    QContactManager em3("non existent", randomParameters);

    /* Also invalid, since we don't have one of these anyway */
    QContactManager em4 = QContactManager::fromUri("invalid uri");
    QContactManager em5 = QContactManager::fromUri(QContactManager::buildUri("nonexistent", QMap<QString, QString>()));
    QContactManager em6 = QContactManager::fromUri(QContactManager::buildUri("nonexistent", randomParameters));


    /*
     * Sets of stores that should be equivalent:
     * - 1, 2, 4, 5
     * - 3, 6
     */

    /* First some URI testing for equivalent stores */
    QVERIFY(em.storeUri() == em2.storeUri());
    QVERIFY(em.storeUri() == em5.storeUri());
    QVERIFY(em.storeUri() == em4.storeUri());
    QVERIFY(em2.storeUri() == em4.storeUri());
    QVERIFY(em2.storeUri() == em5.storeUri());
    QVERIFY(em4.storeUri() == em5.storeUri());

    QVERIFY(em3.storeUri() == em6.storeUri());

    /* Test the stores that should not be the same */
    QVERIFY(em.storeUri() != em3.storeUri());
    QVERIFY(em.storeUri() != em6.storeUri());

    /* now the components */
    QCOMPARE(em.managerId(), QString("invalid"));
    QCOMPARE(em2.managerId(), QString("invalid"));
    QCOMPARE(em3.managerId(), QString("invalid"));
    QCOMPARE(em4.managerId(), QString("invalid"));
    QCOMPARE(em5.managerId(), QString("invalid"));
    QCOMPARE(em6.managerId(), QString("invalid"));
    QCOMPARE(em.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em2.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em3.managerParameters(), randomParameters);
    QCOMPARE(em4.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em5.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em6.managerParameters(), randomParameters);
}

void tst_QContactManager::groups()
{
    QFETCH(QString, uri);
    QContactManager cm = QContactManager::fromUri(uri);

    if (cm.capabilities().capabilities().contains("Groups")) {
        /* Positive testing */

        /* Test adding a null doesn't crash, before we do anything else */
        QVERIFY(cm.saveGroup(0) == false);
        QVERIFY(cm.error() == QContactManager::BadArgumentError);

        /* We need some contacts.. */

        QContact a;
        a.name().setFirst("XXXXXX Albert");

        QContact b;
        b.name().setFirst("XXXXXX Bob");

        QContact c;
        c.name().setFirst("XXXXXX Carol");

        /* Add them */
        QVERIFY(cm.saveContact(&a));
        QVERIFY(cm.saveContact(&b));
        QVERIFY(cm.saveContact(&c));

        /* Get the initial list of groups */
        QList<QUniqueId> origGroups = cm.groups();
        QCOMPARE(cm.error(), QContactManager::NoError);

        /* Add a group */
        QContactGroup g;

        /* No name, so should fail */
        QVERIFY(cm.saveGroup(&g) == false);
        QVERIFY(cm.error() == QContactManager::BadArgumentError);

        /* Name but no contents should be legal */
        g.setName("XXXXXX Group");
        QVERIFY(cm.saveGroup(&g) == true);
        QCOMPARE(cm.error(), QContactManager::NoError);
        QVERIFY(g.id() != 0);

        QContactGroup g2 = cm.group(g.id());
        QCOMPARE(cm.error(), QContactManager::NoError);
        QVERIFY(g2.id() == g.id());
        QVERIFY(g2.members().count() == 0);
        QCOMPARE(g.members(), g2.members());

        /* Update the group */
        g.addMember(a.id());
        QVERIFY(g.members().count() == 1);
        QVERIFY(g.members().at(0) == a.id());

        QVERIFY(cm.saveGroup(&g) == true);
        QCOMPARE(cm.error(), QContactManager::NoError);

        g2 = cm.group(g2.id());
        QCOMPARE(cm.error(), QContactManager::NoError);

        QVERIFY(g2.id() == g.id());
        QVERIFY(g2.members().count() == 1);
        QVERIFY(g2.members().at(0) == a.id());
        QCOMPARE(g.members(), g2.members());

        /* Remove the group */
        QVERIFY(cm.removeGroup(g.id()) == true);
        QCOMPARE(cm.error(), QContactManager::NoError);

        g2 = cm.group(g2.id());
        QCOMPARE(cm.error(), QContactManager::DoesNotExistError);
        QVERIFY(g2.id() == 0);
        QVERIFY(g2.members().count() == 0);

        /* Save a new group with multiple people */
        QContactGroup g3;
        g3.setName("XXXXXX Group 2");
        g3.addMember(b.id());
        g3.addMember(c.id());

        QVERIFY(cm.saveGroup(&g3));
        QCOMPARE(cm.error(), QContactManager::NoError);
        QVERIFY(g3.id() != 0);

        g2 = cm.group(g3.id());
        QVERIFY(g2.id() == g3.id());
        QVERIFY(g2.members().count() == 2);
        QCOMPARE(g2.members(), g3.members());
        /* We don't check order at this time */

        /* Now remove the contacts, make sure they disappear from the groups */
        QVERIFY(cm.removeContact(a.id()));
        QVERIFY(cm.removeContact(b.id()));
        QVERIFY(cm.removeContact(c.id()));

        g2 = cm.group(g3.id());
        QVERIFY(g2.id() == g3.id());
        QVERIFY(g2.members().count() == 0);

        /* Now remove again */
        QVERIFY(cm.removeGroup(g3.id()));
        QCOMPARE(cm.error(), QContactManager::NoError);

        /* Test double remove */
        QVERIFY(cm.removeGroup(g3.id()) == false);
        QCOMPARE(cm.error(), QContactManager::DoesNotExistError);

        /*
         * Now we should hopefully be back to our normal number of groups
         * (unless some other process added groups behind our back.. )
         */
        QCOMPARE(cm.groups().count(), origGroups.count());

    } else {
        /* Negative testing - make sure it fails properly */
        QList<QUniqueId> groups = cm.groups();
        QVERIFY(groups.count() == 0);
        QVERIFY(cm.error() == QContactManager::NotSupportedError);

        QContactGroup g;
        g.addMember(1);
        g.addMember(2);

        /* Test adding a null doesn't crash, before we do anything else */
        QVERIFY(cm.saveGroup(0) == false);
        QVERIFY(cm.error() == QContactManager::BadArgumentError);

        QVERIFY(cm.saveGroup(&g) == false);
        QVERIFY(cm.error() == QContactManager::NotSupportedError);

        QVERIFY(cm.removeGroup(1) == false);
        QVERIFY(cm.error() == QContactManager::NotSupportedError);

        g = cm.group(1);
        QVERIFY(g.id() == 0);
        QVERIFY(cm.error() == QContactManager::NotSupportedError);
    }
}

void tst_QContactManager::add()
{
    QFETCH(QString, uri);
    QContactManager cm = QContactManager::fromUri(uri);

    QContact alice;
    alice.name().setFirst("Alice");
    alice.name().setLast("inWonderland");

    QContactPhoneNumber ph;
    ph.setNumber("1234567");
    ph.setAttribute("Context", "Home");
    ph.setAttribute("SubType", "Mobile");

    alice.saveDetail(&ph);
    int currCount = cm.contacts().count();
    QVERIFY(cm.saveContact(&alice));
    QVERIFY(cm.error() == QContactManager::NoError);

    QVERIFY(alice.id() != 0);
    QCOMPARE(cm.contacts().count(), currCount+1);

    QContact added = cm.contact(alice.id());
    QVERIFY(added.id() != 0);
    QVERIFY(added.id() == alice.id());

    if (added != alice) {
        dumpContacts();
        dumpContactDifferences(added, alice);
    }
}

void tst_QContactManager::update()
{
    QFETCH(QString, uri);
    QContactManager cm = QContactManager::fromUri(uri);

    /* Save a new contact first */
    QContact alice;
    alice.name().setFirst("Alice");
    alice.name().setLast("inWonderland");

    QContactPhoneNumber ph;
    ph.setNumber("1234567");
    ph.setAttribute("Context", "Home");
    ph.setAttribute("SubType", "Mobile");

    alice.saveDetail(&ph);

    QVERIFY(cm.saveContact(&alice));
    QVERIFY(cm.error() == QContactManager::NoError);

    QList<QUniqueId> ids = cm.contacts();
    for(int i = 0; i < ids.count(); i++) {
        QContact current = cm.contact(ids.at(i));
        if (current.name().first() == "Alice" && current.name().last() == "inWonderland") {
            current.name().setMiddle("Fictional");
            QVERIFY(cm.saveContact(&current));
            QVERIFY(cm.error() == QContactManager::NoError);

            QContact updated = cm.contact(ids.at(i));
            QCOMPARE(current.name().middle(), updated.name().middle());
            return; // pass
        }
    }

    // force fail.
    QVERIFY(false);
}

void tst_QContactManager::remove()
{
    QFETCH(QString, uri);
    QContactManager cm = QContactManager::fromUri(uri);

    /* Save a new contact first */
    QContact alice;
    alice.name().setFirst("Alice");
    alice.name().setLast("inWonderland");

    QContactPhoneNumber ph;
    ph.setNumber("1234567");
    ph.setAttribute("Context", "Home");
    ph.setAttribute("SubType", "Mobile");

    alice.saveDetail(&ph);

    QVERIFY(cm.saveContact(&alice));
    QVERIFY(cm.error() == QContactManager::NoError);

    bool atLeastOne = false;
    QList<QUniqueId> ids = cm.contacts();
    for(int i = 0; i < ids.count(); i++) {
        QContact current = cm.contact(ids.at(i));
        if (current.name().first() == "Alice" && current.name().last() == "inWonderland") {
            int currCount = cm.contacts().count();
            atLeastOne = cm.removeContact(current.id());
            QVERIFY(atLeastOne);
            QCOMPARE(cm.contacts().count(), currCount - 1);
        }
    }

    QVERIFY(atLeastOne);
}

void tst_QContactManager::batch()
{
    QFETCH(QString, uri);
    QContactManager cm = QContactManager::fromUri(uri);

    /* First test null pointer operations */
    QVERIFY(cm.saveContacts(0).count() == 0);
    QVERIFY(cm.error() == QContactManager::BadArgumentError);

    QVERIFY(cm.removeContacts(0).count() == 0);
    QVERIFY(cm.error() == QContactManager::BadArgumentError);

    /* Now add 3 contacts, all valid */
    QContact a;
    a.name().setDisplayName("XXXXXX Albert");

    QContact b;
    b.name().setDisplayName("XXXXXX Bob");

    QContact c;
    c.name().setDisplayName("XXXXXX Carol");

    QList<QContact> contacts;
    contacts << a << b << c;

    QList<QContactManager::Error> errors;

    errors = cm.saveContacts(&contacts);

    QVERIFY(cm.error() == QContactManager::NoError);
    QVERIFY(errors.count() == 3);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Make sure our contacts got updated too */
    QVERIFY(contacts.count() == 3);
    QVERIFY(contacts.at(0).id() != 0);
    QVERIFY(contacts.at(1).id() != 0);
    QVERIFY(contacts.at(2).id() != 0);

    QVERIFY(contacts.at(0).name() == a.name());
    QVERIFY(contacts.at(1).name() == b.name());
    QVERIFY(contacts.at(2).name() == c.name());

    /* Retrieve again */
    a = cm.contact(contacts.at(0).id());
    b = cm.contact(contacts.at(1).id());
    c = cm.contact(contacts.at(2).id());
    QVERIFY(contacts.at(0).name() == a.name());
    QVERIFY(contacts.at(1).name() == b.name());
    QVERIFY(contacts.at(2).name() == c.name());

    /* Now make an update to them all */
    QContactPhoneNumber number;
    number.setNumber("1234567");

    QVERIFY(contacts[0].saveDetail(&number));
    number.setNumber("234567");
    QVERIFY(contacts[1].saveDetail(&number));
    number.setNumber("34567");
    QVERIFY(contacts[2].saveDetail(&number));

    errors = cm.saveContacts(&contacts);
    QVERIFY(cm.error() == QContactManager::NoError);
    QVERIFY(errors.count() == 3);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Retrieve them and check them again */
    a = cm.contact(contacts.at(0).id());
    b = cm.contact(contacts.at(1).id());
    c = cm.contact(contacts.at(2).id());
    QVERIFY(contacts.at(0).name() == a.name());
    QVERIFY(contacts.at(1).name() == b.name());
    QVERIFY(contacts.at(2).name() == c.name());

    QVERIFY(a.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(b.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(c.details<QContactPhoneNumber>().count() == 1);

    QVERIFY(a.details<QContactPhoneNumber>().at(0).number() == "1234567");
    QVERIFY(b.details<QContactPhoneNumber>().at(0).number() == "234567");
    QVERIFY(c.details<QContactPhoneNumber>().at(0).number() == "34567");

    /* Now delete them all */
    QList<QUniqueId> ids;
    QUniqueId removedIdForLater = b.id();
    ids << a.id() << b.id() << c.id();
    errors = cm.removeContacts(&ids);
    QVERIFY(errors.count() == 3);
    QVERIFY(cm.error() == QContactManager::NoError);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Make sure all the ids are now 0 */
    QVERIFY(ids.count() == 3);
    QVERIFY(ids.at(0) == 0);
    QVERIFY(ids.at(1) == 0);
    QVERIFY(ids.at(2) == 0);

    /* Make sure the contacts really don't exist any more */
    QVERIFY(cm.contact(a.id()).id() == 0);
    QVERIFY(cm.error() == QContactManager::DoesNotExistError);
    QVERIFY(cm.contact(b.id()).id() == 0);
    QVERIFY(cm.error() == QContactManager::DoesNotExistError);
    QVERIFY(cm.contact(c.id()).id() == 0);
    QVERIFY(cm.error() == QContactManager::DoesNotExistError);

    /* Now try removing with all invalid ids (e.g. the ones we just removed) */
    ids.clear();
    ids << a.id() << b.id() << c.id();
    errors = cm.removeContacts(&ids);
    QVERIFY(cm.error() == QContactManager::DoesNotExistError);
    QVERIFY(errors.count() == 3);
    QVERIFY(errors.at(0) == QContactManager::DoesNotExistError);
    QVERIFY(errors.at(1) == QContactManager::DoesNotExistError);
    QVERIFY(errors.at(2) == QContactManager::DoesNotExistError);

    /* Try adding some new ones again, this time one with an error */
    contacts.clear();
    a.setId(0);
    b.setId(0);
    c.setId(0);

    /* Make B the bad guy */
    QContactDetail bad("does not exist and will break if you add it");
    bad.setValue("This is also bad", "Very bad");
    b.saveDetail(&bad);

    contacts << a << b << c;
    errors = cm.saveContacts(&contacts);
    /* We can't really say what the error will be.. maybe bad argument, maybe invalid detail */
    QVERIFY(cm.error() != QContactManager::NoError);

    QVERIFY(errors.count() == 3);
    /* It's permissible to fail all the adds, or to add the successful ones */
    if (errors.at(0) == QContactManager::NoError) {
        QVERIFY(contacts.at(0).id() != 0);
    } else {
        QVERIFY(contacts.at(0).id() == 0);
    }
    /* B should definitely have failed */
    QVERIFY(errors.at(1) == QContactManager::InvalidDetailError);
    QVERIFY(contacts.at(1).id() == 0);

    /* C might have gone through */
    if (errors.at(2) == QContactManager::NoError) {
        QVERIFY(contacts.at(2).id() != 0);
    } else {
        QVERIFY(contacts.at(2).id() == 0);
    }

    /* Fix up B and re save it */
    QVERIFY(contacts[1].removeDetail(&bad));
    errors = cm.saveContacts(&contacts);
    QVERIFY(errors.count() == 3);
    QVERIFY(cm.error() == QContactManager::NoError);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Now delete 3 items, but with one bad argument */
    ids.clear();
    ids << contacts.at(0).id();
    ids << removedIdForLater;
    ids << contacts.at(2).id();

    errors = cm.removeContacts(&ids);
    QVERIFY(errors.count() == 3);
    QVERIFY(cm.error() != QContactManager::NoError);

    /* Again, the backend has the choice of either removing the successful ones, or not */
    if (errors.at(0) == QContactManager::NoError) {
        QVERIFY(ids.at(0) == 0);
    } else {
        QVERIFY(ids.at(0) != 0);
    }

    /* B should definitely have failed */
    QVERIFY(errors.at(1) == QContactManager::DoesNotExistError);
    QVERIFY(ids.at(1) == removedIdForLater);

    /* C might have gone through */
    if (errors.at(2) == QContactManager::NoError) {
        QVERIFY(ids.at(2) == 0);
    } else {
        QVERIFY(ids.at(2) != 0);
    }

}

void tst_QContactManager::invalidManager()
{
    /* Create an invalid manager */
    QContactManager manager("this should never work");
    QVERIFY(manager.managerId() == "invalid");

    /* Now test that all the operations fail */
    QVERIFY(manager.contacts().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QContact foo;
    foo.name().setLast("Lastname");

    QVERIFY(manager.saveContact(&foo) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(foo.id() == 0);
    QVERIFY(manager.contacts().count() == 0);

    QVERIFY(manager.contact(foo.id()).id() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.removeContact(foo.id()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.saveContacts(0) == QList<QContactManager::Error>());
    QVERIFY(manager.error() == QContactManager::BadArgumentError);

    QList<QContact> list;
    list << foo;
    QVERIFY(manager.saveContacts(&list) == (QList<QContactManager::Error>() << QContactManager::NotSupportedError));
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.removeContacts(0) == QList<QContactManager::Error>());
    QVERIFY(manager.error() == QContactManager::BadArgumentError);

    QList<QUniqueId> idlist;
    idlist << foo.id();
    QVERIFY(manager.removeContacts(&idlist) == (QList<QContactManager::Error>() << QContactManager::NotSupportedError));
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    /* Groups */
    QVERIFY(manager.groups().count() == 0);

    QContactGroup group;
    group.addMember(foo.id());
    QVERIFY(manager.saveGroup(&group) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(group.id() == 0);
    QVERIFY(manager.groups().count() == 0);

    QVERIFY(manager.group(group.id()).id() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.removeGroup(group.id()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    /* Detail definitions */
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QContactDetailDefinition def;
    def.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    def.setUnique(true);
    def.setId("new field");
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field currField;
    currField.dataType = QVariant::String;
    fields.insert("value", currField);
    def.setFields(fields);

    QVERIFY(manager.saveDetailDefinition(def) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.detailDefinition("new field").id() == QString());

    QVERIFY(manager.removeDetailDefinition(def.id()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    /* Changelog */
    QDateTime historic(QDate(600, 1, 1), QTime());
    QVERIFY(manager.contactsAddedSince(historic).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contactsModifiedSince(historic).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contactsRemovedSince(historic).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.groupsAddedSince(historic).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.groupsModifiedSince(historic).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.groupsRemovedSince(historic).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    /* Capabilities */
    QContactManagerCapabilities caps = manager.capabilities();
    QVERIFY(caps.fastFilterableDefinitions().count() == 0);
    QVERIFY(caps.supportedDataTypes().count() == 0);
    QVERIFY(caps.capabilities().count() == 0);
}

void tst_QContactManager::memoryManager()
{
    QMap<QString, QString> params;
    QContactManager m1 = QContactManager("memory");
    params.insert("random", "shouldNotBeUsed");
    QContactManager m2 = QContactManager("memory", params);
    params.insert("id", "shouldBeUsed");
    QContactManager m3 = QContactManager("memory", params);
    QContactManager m4 = QContactManager("memory", params);
    params.insert("id", QString(""));
    QContactManager m5 = QContactManager("memory", params); // should be another anonymous

    // add a contact to each of m1, m2, m3
    QContact c;
    c.name().setFirst("John");
    c.name().setLast("Civilian");
    m1.saveContact(&c);
    c.setId(0);
    QContact c2;
    c2 = c;
    c2.name().setMiddle("Public");
    m2.saveContact(&c2);            // save c2 first; c will be given a higher id
    m2.saveContact(&c);             // save c to m2
    c.setId(0);
    c.name().setSuffix("MD");
    m3.saveContact(&c);

    /* test that m1 != m2 != m3 and that m3 == m4 */

    // check the counts are correct - especially note m4 and m3.
    QCOMPARE(m1.contacts().count(), 1);
    QCOMPARE(m2.contacts().count(), 2);
    QCOMPARE(m3.contacts().count(), 1);
    QCOMPARE(m4.contacts().count(), 1);
    QCOMPARE(m5.contacts().count(), 0);

    // remove c2 from m2 - ensure that this doesn't affect any other manager.
    m2.removeContact(c2.id());
    QCOMPARE(m1.contacts().count(), 1);
    QCOMPARE(m2.contacts().count(), 1);
    QCOMPARE(m3.contacts().count(), 1);
    QCOMPARE(m4.contacts().count(), 1);
    QCOMPARE(m5.contacts().count(), 0);

    // check that the contacts contained within are different.
    // note that in the m1->m2 case, only the id will be different!
    QVERIFY(m1.contact(m1.contacts().at(0)) != m2.contact(m2.contacts().at(0)));
    QVERIFY(m1.contact(m1.contacts().at(0)) != m3.contact(m3.contacts().at(0)));
    QVERIFY(m2.contact(m2.contacts().at(0)) != m3.contact(m3.contacts().at(0)));
    QVERIFY(m3.contact(m3.contacts().at(0)) == m4.contact(m4.contacts().at(0)));

    // now, we should be able to remove from m4, and have m3 empty
    QVERIFY(m4.removeContact(c.id()));
    QCOMPARE(m3.contacts().count(), 0);
    QCOMPARE(m4.contacts().count(), 0);
    QCOMPARE(m5.contacts().count(), 0);
}

void tst_QContactManager::referenceCounting_data()
{
    QTest::addColumn<QString>("managerId");
    QTest::addColumn<QObject*>("managerPointerVoid");
    QTest::newRow("default") << QString() << ((QObject*)(new QContactManager));
    QTest::newRow("memory") << QString("memory") << ((QObject*)(new QContactManager("memory")));
}

void tst_QContactManager::referenceCounting()
{
    // to be run under valgrind to check for memory leaks.
    QFETCH(QString, managerId);
    QFETCH(QObject*, managerPointerVoid);
    QContactManager manager(managerId);
    QContactManager* managerPointer = (QContactManager*)managerPointerVoid;

    // first, stack allocation and copies, going out of scope.
    if (true) {
        QContactManager stackCopy(manager);
    }
    if (true) {
        QContactManager stackCopy = manager;
    }
    if (true) {
        QContactManager stackCopy(manager);
        QContactManager stackCopy2 = manager;
        QContactManager stackCopy3(stackCopy2);
    }

    // then, heap allocation and copies
    if (true) {
        QContactManager *heapCopy = new QContactManager(*managerPointer);
        delete heapCopy;
    }
    if (true) {
        QContactManager *heapCopy = new QContactManager(*managerPointer);
        QContactManager *heapCopy2 = new QContactManager(*heapCopy);
        delete heapCopy;
        delete heapCopy2;
    }
    if (true) {
        QContactManager *heapCopy = new QContactManager(*managerPointer);
        QContactManager *heapCopy2 = new QContactManager(*heapCopy);
        delete heapCopy2;
        delete heapCopy;
    }

    // finally, heap allocation with stack copies
    if (true) {
        QContactManager stackCopy(*managerPointer);
    }
    if (true) {
        QContactManager stackCopy = *managerPointer;
    }
    if (true) {
        QContactManager stackCopy(*managerPointer);
        QContactManager stackCopy2 = *managerPointer;
        QContactManager stackCopy3(stackCopy2);
    }

    /* Now check retrieving the capabilities doesn't cause problems */
    if (true) {
        QContactManager *heapCopy = new QContactManager(managerId);
        QContactManagerCapabilities cap = heapCopy->capabilities();

        if (managerId != "invalid") {
            QVERIFY(cap.capabilities() != QStringList());
            QVERIFY(cap.supportedDataTypes() != QList<QVariant::Type>());
            QVERIFY(cap.fastFilterableDefinitions().count() || true); // don't care about return type
        }
        delete heapCopy;
        /* Now the manager has gone, we shouldn't crash, and get empty caps */
        QVERIFY(cap.capabilities() == QStringList());
        QVERIFY(cap.supportedDataTypes() == QList<QVariant::Type>());
        QVERIFY(cap.fastFilterableDefinitions().count() == 0);
    }

    /* test cloning an existing manager as well */
    if (true) {
        QContactManager *heapCopy = new QContactManager(manager);
        QContactManagerCapabilities cap = heapCopy->capabilities();

        if (managerId != "invalid") {
            QVERIFY(cap.capabilities() != QStringList());
            QVERIFY(cap.supportedDataTypes() != QList<QVariant::Type>());
            QVERIFY(cap.fastFilterableDefinitions().count() || true); // don't care about return type
        }
        delete heapCopy;
        /* Now the manager has gone, we shouldn't crash, and get empty caps */
        QVERIFY(cap.capabilities() == QStringList());
        QVERIFY(cap.supportedDataTypes() == QList<QVariant::Type>());
        QVERIFY(cap.fastFilterableDefinitions().count() == 0);
    }

    if (true) {
        /* Test a stack based manager, retrieve caps, then assign the manager elsewhere */
        QContactManager m1(managerId);
        QContactManagerCapabilities cap(m1.capabilities());

        if (managerId != "invalid") {
            QVERIFY(cap.capabilities() != QStringList());
            QVERIFY(cap.supportedDataTypes() != QList<QVariant::Type>());
            QVERIFY(cap.fastFilterableDefinitions().count() || true); // don't care about return type
        }

        m1 = QContactManager("invalid");
        QVERIFY(cap.capabilities() == QStringList());
        QVERIFY(cap.supportedDataTypes() == QList<QVariant::Type>());
        QVERIFY(cap.fastFilterableDefinitions().count() == 0);

        /* Retrieve the caps again */
        m1 = QContactManager(managerId);
        QContactManagerCapabilities cap2 = m1.capabilities();
        cap = cap2;

        if (managerId != "invalid") {
            QVERIFY(cap.capabilities() != QStringList());
            QVERIFY(cap.supportedDataTypes() != QList<QVariant::Type>());
            QVERIFY(cap.fastFilterableDefinitions().count() || true); // don't care about return type
            QVERIFY(cap2.capabilities() != QStringList());
            QVERIFY(cap2.supportedDataTypes() != QList<QVariant::Type>());
            QVERIFY(cap2.fastFilterableDefinitions().count() || true); // don't care about return type

            QVERIFY(cap.capabilities() == cap2.capabilities());
            QVERIFY(cap.supportedDataTypes() == cap2.supportedDataTypes());
            QVERIFY(cap.fastFilterableDefinitions() == cap2.fastFilterableDefinitions());
        }

        /* Self assignment as well */
        cap = cap;
        if (managerId != "invalid") {
            QVERIFY(cap.capabilities() != QStringList());
            QVERIFY(cap.supportedDataTypes() != QList<QVariant::Type>());
            QVERIFY(cap.fastFilterableDefinitions().count() || true); // don't care about return type
            QVERIFY(cap2.capabilities() != QStringList());
            QVERIFY(cap2.supportedDataTypes() != QList<QVariant::Type>());
            QVERIFY(cap2.fastFilterableDefinitions().count() || true); // don't care about return type

            QVERIFY(cap.capabilities() == cap2.capabilities());
            QVERIFY(cap.supportedDataTypes() == cap2.supportedDataTypes());
            QVERIFY(cap.fastFilterableDefinitions() == cap2.fastFilterableDefinitions());
        }

        /* Clear the manager again, and both should be invalid */
        m1 = QContactManager("invalid");
        QVERIFY(cap.capabilities() == QStringList());
        QVERIFY(cap.supportedDataTypes() == QList<QVariant::Type>());
        QVERIFY(cap.fastFilterableDefinitions().count() == 0);
        QVERIFY(cap2.capabilities() == QStringList());
        QVERIFY(cap2.supportedDataTypes() == QList<QVariant::Type>());
        QVERIFY(cap2.fastFilterableDefinitions().count() == 0);

    }



    /* Clean up */
    delete managerPointer;
}

void tst_QContactManager::contactValidation()
{
    /* Use the memory engine as a reference (validation is not engine specific) */
    QContactManager cm("memory");
    QContact c;

    /*
     * Add some definitions for testing:
     *
     * 1) a unique detail
     * 2) a detail with restricted values
     * 3) a create only detail
     * 4) a unique create only detail
     */
    QContactDetailDefinition uniqueDef;
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field field;
    field.dataType = QVariant::String;
    fields.insert("value", field);

    uniqueDef.setId("UniqueDetail");
    uniqueDef.setFields(fields);
    uniqueDef.setUnique(true);

    QVERIFY(cm.saveDetailDefinition(uniqueDef));

    QContactDetailDefinition restrictedDef;
    restrictedDef.setId("RestrictedDetail");
    fields.clear();
    field.allowableValues = QVariantList() << "One" << "Two" << "Three";
    fields.insert("value", field);
    restrictedDef.setFields(fields);

    QVERIFY(cm.saveDetailDefinition(restrictedDef));

    QContactDetailDefinition createOnlyDef;
    createOnlyDef.setId("CreateOnlyDetail");
    createOnlyDef.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    fields.clear();
    field.allowableValues.clear();
    fields.insert("value", field);
    createOnlyDef.setFields(fields);

    QVERIFY(cm.saveDetailDefinition(createOnlyDef));

    QContactDetailDefinition createOnlyUniqueDef;
    createOnlyUniqueDef.setId("CreateOnlyUniqueDetail");
    createOnlyUniqueDef.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    createOnlyUniqueDef.setUnique(true);
    fields.clear();
    field.allowableValues.clear();
    fields.insert("value", field);
    createOnlyUniqueDef.setFields(fields);

    QVERIFY(cm.saveDetailDefinition(createOnlyUniqueDef));

    // first, test an invalid definition
    QContactDetail d1 = QContactDetail("UnknownDefinition");
    d1.setValue("test", "test");
    c.saveDetail(&d1);
    QVERIFY(!cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d1);

    // second, test an invalid uniqueness constraint
    QContactDetail d2 = QContactDetail("UniqueDetail");
    d2.setValue("value", "test");
    c.saveDetail(&d2);

    // One unique should be ok
    QVERIFY(cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::NoError);

    // Two uniques should not be ok
    QContactDetail d3 = QContactDetail("UniqueDetail");
    d3.setValue("value", "test2");
    c.saveDetail(&d3);
    QVERIFY(!cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d3);
    c.removeDetail(&d2);

    // third, test an invalid field name
    QContactDetail d4 = QContactDetail(QContactPhoneNumber::DefinitionId);
    d4.setValue("test", "test");
    c.saveDetail(&d4);
    QVERIFY(!cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d4);

    // fourth, test an invalid field data type
    QContactDetail d5 = QContactDetail(QContactPhoneNumber::DefinitionId);
    d5.setValue(QContactPhoneNumber::FieldNumber, QDateTime::currentDateTime());
    c.saveDetail(&d5);
    QVERIFY(!cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d5);

    // fifth, test an invalid field value (not in the allowed list)
    QContactDetail d6 = QContactDetail("RestrictedDetail");
    d6.setValue("value", "Seven"); // not in One, Two or Three
    c.saveDetail(&d6);
    QVERIFY(!cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d6);

    /* Now a valid value */
    d6.setValue("value", "Two");
    c.saveDetail(&d6);
    QVERIFY(cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::NoError);
    c.removeDetail(&d6);

    // Test a completely valid one.
    QContactDetail d7 = QContactDetail(QContactPhoneNumber::DefinitionId);
    d7.setValue(QContactPhoneNumber::FieldNumber, "0123456");
    c.saveDetail(&d7);
    QVERIFY(cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::NoError);
    c.removeDetail(&d7);

    /* Test a write once detail */
    QContactDetail d8 = QContactDetail("CreateOnlyDetail");
    d8.setValue("value", "First value");
    c.saveDetail(&d8);

    QVERIFY(cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::NoError);

    /* Changing or adding some other detail should also be fine, and resaving it */
    QContactPhoneNumber p1;
    p1.setNumber("123467");
    c.saveDetail(&p1);
    QVERIFY(cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::NoError);

    /* Adding a second detail should be fine */
    QContactDetail d9 = QContactDetail("CreateOnlyDetail");
    d9.setValue("value", "Second value");
    c.saveDetail(&d9);
    QVERIFY(cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::NoError);

    /* Changing a value should fail */
    d8.setValue("value", "Third value");
    c.saveDetail(&d8);

    QVERIFY(!cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::DetailAccessError);

    c.removeDetail(&d8);
    /* Removing a create only should also fail */
    QVERIFY(!cm.saveContact(&c));
    QCOMPARE(cm.error(), QContactManager::DetailAccessError);
}

void tst_QContactManager::signalEmission()
{
    QMap<QString, QString> params;
    params.insert("id", "tst_QContactManager::signalEmission()");
    QContactManager m1("memory", params);
    QContactManager m2("memory", params);

    qRegisterMetaType<QUniqueId>("QUniqueId");
    qRegisterMetaType<QList<QUniqueId> >("QList<QUniqueId>");
    QSignalSpy spy(&m1, SIGNAL(contactsAdded(QList<QUniqueId>)));
    QSignalSpy spy2(&m1, SIGNAL(contactsChanged(QList<QUniqueId>)));
    QSignalSpy spy3(&m1, SIGNAL(contactsRemoved(QList<QUniqueId>)));
    QSignalSpy spy4(&m1, SIGNAL(groupsAdded(QList<QUniqueId>)));
    QSignalSpy spy5(&m1, SIGNAL(groupsChanged(QList<QUniqueId>)));
    QSignalSpy spy6(&m1, SIGNAL(groupsRemoved(QList<QUniqueId>)));

    QList<QVariant> args;
    QContact c;
    QUniqueId temp;
    int addSigCount = 0; // the expected signal counts.
    int modSigCount = 0;
    int remSigCount = 0;

    // verify add emits signal added
    c.name().setFirst("John");
    m1.saveContact(&c);
    addSigCount += 1;
    QCOMPARE(spy.count(), addSigCount);
    args = spy.takeFirst();
    addSigCount -= 1;
    QVERIFY(args.count() == 1);
    temp = QUniqueId(args.at(0).value<quint32>());

    // verify save modified emits signal changed
    c.name().setLast("Citizen");
    m1.saveContact(&c);
    modSigCount += 1;
    QCOMPARE(spy2.count(), modSigCount);
    args = spy2.takeFirst();
    modSigCount -= 1;
    QVERIFY(args.count() == 1);
    QCOMPARE(temp, QUniqueId(args.at(0).value<quint32>()));

    // verify remove emits signal removed
    m1.removeContact(c.id());
    remSigCount += 1;
    QCOMPARE(spy3.count(), remSigCount);
    args = spy3.takeFirst();
    remSigCount -= 1;
    QVERIFY(args.count() == 1);
    QCOMPARE(temp, QUniqueId(args.at(0).value<quint32>()));

    // verify multiple adds works as advertised
    QContact c2, c3;
    c2.name().setFirst("Mark");
    c3.name().setFirst("Garry");
    m1.saveContact(&c);
    addSigCount += 1;
    m1.saveContact(&c2);
    addSigCount += 1;
    m1.saveContact(&c3);
    addSigCount += 1;
    QCOMPARE(spy.count(), addSigCount);

    // verify multiple modifies works as advertised
    c2.name().setLast("M.");
    m1.saveContact(&c2);
    modSigCount += 1;
    c2.name().setPrefix("Mr.");
    c3.name().setLast("G.");
    m1.saveContact(&c2);
    modSigCount += 1;
    m1.saveContact(&c3);
    modSigCount += 1;
    QCOMPARE(spy2.count(), modSigCount);

    // verify multiple removes works as advertised
    m1.removeContact(c3.id());
    remSigCount += 1;
    m1.removeContact(c2.id());
    remSigCount += 1;
    QCOMPARE(spy3.count(), remSigCount);

    // verify that signals are emitted for modifications made to other managers (same id).
    c.name().setSuffix("Test");
    m2.saveContact(&c);
    modSigCount += 1;
    c.name().setPrefix("Test2");
    m2.saveContact(&c);
    modSigCount += 1;
    QCOMPARE(spy2.count(), modSigCount); // check that we received the update signals.
    m2.removeContact(c.id());
    remSigCount += 1;
    QCOMPARE(spy3.count(), remSigCount); // check that we received the remove signal.
}


void tst_QContactManager::errorStayingPut()
{
    /* Make sure that when we clone a manager, we don't clone the error */
    QContactManager m1("memory");

    QVERIFY(m1.error() == QContactManager::NoError);

    /* Remove an invalid contact to get an error */
    QVERIFY(m1.removeContact(0) == false);
    QVERIFY(m1.error() == QContactManager::DoesNotExistError);

    /* Clone it */
    QContactManager m2(m1);
    QContactManager m3;
    m3 = m1;

    QVERIFY(m1.error() == QContactManager::DoesNotExistError);
    QVERIFY(m2.error() == QContactManager::NoError);
    QVERIFY(m3.error() == QContactManager::NoError);

    /* Cause an error on the other ones and check the first is not affected */
    m2.saveContacts(0);
    QVERIFY(m1.error() == QContactManager::DoesNotExistError);
    QVERIFY(m2.error() == QContactManager::BadArgumentError);
    QVERIFY(m3.error() == QContactManager::NoError);

    QContact c;
    QContactDetail d("This does not exist and if it does this will break");
    d.setValue("Value that also doesn't exist", 5);
    c.saveDetail(&d);

    QVERIFY(m3.saveContact(&c) == false);
    QVERIFY(m1.error() == QContactManager::DoesNotExistError);
    QVERIFY(m2.error() == QContactManager::BadArgumentError);
    QVERIFY(m3.error() == QContactManager::InvalidDetailError);
}

QTEST_MAIN(tst_QContactManager)
#include "tst_qcontactmanager.moc"
