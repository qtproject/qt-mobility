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

#include <QtTest/QtTest>

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

// to get QFETCH to work with the template expression...
typedef QMap<QString,QString> tst_QContactManager_QStringMap;
Q_DECLARE_METATYPE(tst_QContactManager_QStringMap)
Q_DECLARE_METATYPE(QList<QUniqueId>)

/* A class that no backend can support */
class UnsupportedMetatype {
    int foo;
};
Q_DECLARE_METATYPE(UnsupportedMetatype)

class tst_QContactManager : public QObject
{
Q_OBJECT

public:
    tst_QContactManager();
    virtual ~tst_QContactManager();

private:
    void dumpContactDifferences(const QContact& a, const QContact& b);
    void dumpContact(const QContact &c);
    void dumpContacts(QContactManager *cm);
    bool isSuperset(const QContact& ca, const QContact& cb);

    void addManagers(); // add standard managers to the data
public slots:
    void init();
    void cleanup();
private slots:

    void doDump();
    void doDump_data() {addManagers();}

    /* Special test with special data */
    void uriParsing();
    void nameSynthesis();

    /* Tests that are run on all managers */
    void nullIdOperations();
    void add();
    void update();
    void remove();
    void groups();
    void batch();
    void signalEmission();
    void detailDefinitions();
    void displayName();

    /* Tests that take no data */
    void contactValidation();
    void errorStayingPut();
    void ctors();
    void invalidManager();
    void memoryManager();

    /* data providers (mostly all engines) */
    void uriParsing_data(); // Special data
    void nameSynthesis_data(); // Special data
    void nullIdOperations_data() {addManagers();}
    void add_data() {addManagers();}
    void update_data() {addManagers();}
    void remove_data() {addManagers();}
    void groups_data() {addManagers();}
    void batch_data() {addManagers();}
    void signalEmission_data() {addManagers();}
    void detailDefinitions_data() {addManagers();}
    void displayName_data() {addManagers();}
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

void tst_QContactManager::dumpContactDifferences(const QContact& ca, const QContact& cb)
{
    // Try to narrow down the differences
    QContact a(ca);
    QContact b(cb);

    QContactName n1 = a.detail(QContactName::DefinitionName);
    QContactName n2 = b.detail(QContactName::DefinitionName);

    // Check the name components in more detail
    QCOMPARE(n1.first(), n2.first());
    QCOMPARE(n1.middle(), n2.middle());
    QCOMPARE(n1.last(), n2.last());
    QCOMPARE(n1.prefix(), n2.prefix());
    QCOMPARE(n1.suffix(), n2.suffix());

    // Check the display label
    QCOMPARE(a.displayLabel().label(), b.displayLabel().label());
    QCOMPARE(a.displayLabel().isSynthesised(), b.displayLabel().isSynthesised());

    // Now look at the rest
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
        if (d.definitionName() != QContactDisplayLabel::DefinitionName)
            qDebug() << "A contact had extra detail:" << d.definitionName() << d.values();
    }
    // and same for B
    foreach(QContactDetail d, bDetails) {
        if (d.definitionName() != QContactDisplayLabel::DefinitionName)
            qDebug() << "B contact had extra detail:" << d.definitionName() << d.values();
    }
}

bool tst_QContactManager::isSuperset(const QContact& ca, const QContact& cb)
{
    // returns true if contact ca is a superset of contact cb
    // we use this test instead of equality because dynamic information
    // such as presence/location, and synthesised information such as
    // display label, may differ between a contact in memory and the
    // contact in the managed store.

    QContact a(ca);
    QContact b(cb);
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

    // Now check to see if b has any details remaining; if so, a is not a superset.
    // Note that the DisplayLabel can never be removed.
    if (b.details().size() > 1 || (b.details().size() == 1 && b.details().value(0).definitionName() != QContactDisplayLabel::DefinitionName))
        return false;
    return true;
}

void tst_QContactManager::dumpContact(const QContact& contact)
{
    QContactManager m;
    qDebug() << "Contact: " << contact.id() << "(" << m.synthesiseDisplayLabel(contact) << ")";
    QList<QContactDetail> details = contact.details();
    foreach(QContactDetail d, details) {
        qDebug() << "  " << d.definitionName() << ":";
        qDebug() << "    Vals:" << d.values();
    }
}

void tst_QContactManager::dumpContacts(QContactManager *cm)
{
    QList<QUniqueId> ids = cm->contacts();

    qDebug() << "There are" << ids.count() << "contacts in" << cm->storeUri();

    foreach(QUniqueId id, ids) {
        QContact c = cm->contact(id);
        dumpContact(c);
    }
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

    /* Known one that will not pass */
    managers.removeAll("invalid");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        if (mgr == "memory") {
            params.insert("id", "tst_QContactManager");
            QTest::newRow(QString("mgr='%1', params").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        }
    }
}


void tst_QContactManager::nullIdOperations()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);
    QVERIFY(!cm->removeContact(QUniqueId()));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);

    QVERIFY(!cm->removeGroup(QUniqueId()));
    QVERIFY(cm->error() == QContactManager::NotSupportedError || cm->error() == QContactManager::DoesNotExistError);

    QContact c = cm->contact(QUniqueId());
    QVERIFY(c.id() == 0);
    QVERIFY(c.isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    delete cm;
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

    QObject parent;

    QContactManager cm; // default
    QContactManager cm2(defaultStore);
    QContactManager cm3(defaultStore, QMap<QString, QString>());
    QContactManager cm4(cm.storeUri()); // should fail

    QContactManager* cm5 = QContactManager::fromUri(QContactManager::buildUri(defaultStore, QMap<QString, QString>()));
    QContactManager* cm6 = QContactManager::fromUri(cm.storeUri());
    QContactManager* cm9 = QContactManager::fromUri(QString(), &parent);

    QVERIFY(cm9->parent() == &parent);

    QCOMPARE(cm.storeUri(), cm2.storeUri());
    QCOMPARE(cm.storeUri(), cm3.storeUri());
    QCOMPARE(cm.storeUri(), cm5->storeUri());
    QCOMPARE(cm.storeUri(), cm6->storeUri());
    QCOMPARE(cm.storeUri(), cm9->storeUri());

    QVERIFY(cm.storeUri() != cm4.storeUri()); // don't pass a uri to the ctor

    /* Test that we get invalid stores when we do silly things */
    QContactManager em("non existent");
    QContactManager em2("non existent", QMap<QString, QString>());
    QContactManager em3("non existent", randomParameters);

    /* Also invalid, since we don't have one of these anyway */
    QContactManager* em4 = QContactManager::fromUri("invalid uri");
    QContactManager* em5 = QContactManager::fromUri(QContactManager::buildUri("nonexistent", QMap<QString, QString>()));
    QContactManager* em6 = QContactManager::fromUri(QContactManager::buildUri("nonexistent", randomParameters));


    /*
     * Sets of stores that should be equivalent:
     * - 1, 2, 4, 5
     * - 3, 6
     */

    /* First some URI testing for equivalent stores */
    QVERIFY(em.storeUri() == em2.storeUri());
    QVERIFY(em.storeUri() == em5->storeUri());
    QVERIFY(em.storeUri() == em4->storeUri());
    QVERIFY(em2.storeUri() == em4->storeUri());
    QVERIFY(em2.storeUri() == em5->storeUri());
    QVERIFY(em4->storeUri() == em5->storeUri());

    QVERIFY(em3.storeUri() == em6->storeUri());

    /* Test the stores that should not be the same */
    QVERIFY(em.storeUri() != em3.storeUri());
    QVERIFY(em.storeUri() != em6->storeUri());

    /* now the components */
    QCOMPARE(em.managerName(), QString("invalid"));
    QCOMPARE(em2.managerName(), QString("invalid"));
    QCOMPARE(em3.managerName(), QString("invalid"));
    QCOMPARE(em4->managerName(), QString("invalid"));
    QCOMPARE(em5->managerName(), QString("invalid"));
    QCOMPARE(em6->managerName(), QString("invalid"));
    QCOMPARE(em.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em2.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em3.managerParameters(), tst_QContactManager_QStringMap()); // invalid engine discards params.
    QCOMPARE(em4->managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em5->managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em6->managerParameters(), tst_QContactManager_QStringMap()); // invalid engine discards params.


    /* Cleanse */
    delete em4;
    delete em5;
    delete em6;
    delete cm5;
    delete cm6;

    /* cm9 should be deleted by ~parent */
}

void tst_QContactManager::doDump()
{
    // Only do this if it has been explicitly selected
    if (QCoreApplication::arguments().contains("doDump")) {
        QFETCH(QString, uri);
        QContactManager* cm = QContactManager::fromUri(uri);

        dumpContacts(cm);
    }
}

void tst_QContactManager::groups()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    if (cm->information()->hasFeature(QContactManagerInfo::Groups)) {
        /* Positive testing */

        /* Test adding a null doesn't crash, before we do anything else */
        QVERIFY(cm->saveGroup(0) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        /* We need some contacts.. */

        QContact a;
        QContactName na;
        na.setFirst("XXXXXX Albert");
        a.saveDetail(&na);

        QContact b;
        QContactName nb;
        nb.setFirst("XXXXXX Bob");
        b.saveDetail(&nb);

        QContact c;
        QContactName nc;
        nc.setFirst("XXXXXX Carol");
        c.saveDetail(&nc);

        /* Add them */
        QVERIFY(cm->saveContact(&a));
        QVERIFY(cm->saveContact(&b));
        QVERIFY(cm->saveContact(&c));

        /* Get the initial list of groups */
        QList<QUniqueId> origGroups = cm->groups();
        QCOMPARE(cm->error(), QContactManager::NoError);

        /* Add a group */
        QContactGroup g;

        /* No name, so should fail */
        QVERIFY(cm->saveGroup(&g) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        /* Name but no contents should be legal */
        g.setName("XXXXXX Group");
        QVERIFY(cm->saveGroup(&g) == true);
        QCOMPARE(cm->error(), QContactManager::NoError);
        QVERIFY(g.id() != 0);

        QContactGroup g2 = cm->group(g.id());
        QCOMPARE(cm->error(), QContactManager::NoError);
        QVERIFY(!g2.isEmpty());
        QVERIFY(g2.id() == g.id());
        QVERIFY(g2.members().count() == 0);
        QCOMPARE(g.members(), g2.members());

        /* Update the group */
        g.addMember(a.id());
        QVERIFY(g.members().count() == 1);
        QVERIFY(g.members().at(0) == a.id());

        QVERIFY(cm->saveGroup(&g) == true);
        QCOMPARE(cm->error(), QContactManager::NoError);

        /* Check that retrieving a again shows the updated group */
        QContact a2 = cm->contact(a.id());
        QVERIFY(a2.groups().contains(g.id()));

        g2 = cm->group(g2.id());
        QCOMPARE(cm->error(), QContactManager::NoError);

        QVERIFY(!g2.isEmpty());
        QVERIFY(g2.id() == g.id());
        QVERIFY(g2.members().count() == 1);
        QVERIFY(g2.members().at(0) == a.id());
        QCOMPARE(g.members(), g2.members());

        /* Remove the group */
        QVERIFY(cm->removeGroup(g.id()) == true);
        QCOMPARE(cm->error(), QContactManager::NoError);

        g2 = cm->group(g2.id());
        QCOMPARE(cm->error(), QContactManager::DoesNotExistError);
        QVERIFY(g2.isEmpty());
        QVERIFY(g2.id() == 0);
        QVERIFY(g2.members().count() == 0);

        /* Check that retrieving a again shows the updated group */
        a2 = cm->contact(a.id());
        QVERIFY(!a2.groups().contains(g.id()));

        /* Save a new group with multiple people */
        QContactGroup g3;
        g3.setName("XXXXXX Group 2");
        g3.addMember(b.id());
        g3.addMember(c.id());

        QVERIFY(cm->saveGroup(&g3));
        QCOMPARE(cm->error(), QContactManager::NoError);
        QVERIFY(g3.id() != 0);

        g2 = cm->group(g3.id());
        QVERIFY(g2.id() == g3.id());
        QVERIFY(g2.members().count() == 2);
        QCOMPARE(g2.members(), g3.members());
        /* We don't check order at this time */

        /* Remove one member and save again */
        g3.removeMember(c.id());
        QVERIFY(cm->saveGroup(&g3));
        g2 = cm->group(g3.id());
        QVERIFY(g2.id() == g3.id());
        QVERIFY(g2.members().count() == 1);
        QCOMPARE(g2.members(), g3.members());

        /* Add it back again */
        g3.addMember(c.id());
        QVERIFY(cm->saveGroup(&g3));
        g2 = cm->group(g3.id());
        QVERIFY(g2.id() == g3.id());
        QVERIFY(g2.members().count() == 2);
        QCOMPARE(g2.members(), g3.members());


        /* Add the first group back again */
        QVERIFY(cm->saveGroup(&g));
        /* Add the g group to another two contacts */
        c = cm->contact(c.id());
        QList<QUniqueId> ids = c.groups();
        ids.append(g.id());
        c.setGroups(ids);
        b = cm->contact(b.id());
        ids = b.groups();
        ids.append(g.id());
        b.setGroups(ids);
        QList<QContact> contacts;
        contacts << b << c;
        QVERIFY(cm->saveContacts(&contacts).count() == 2);

        /* Now remove the contacts, make sure they disappear from the groups */
        QVERIFY(cm->removeContact(a.id()));
        QVERIFY(cm->removeContact(b.id()));

        /* Make sure we can't save the group with a or b in it */
        QVERIFY(!cm->saveGroup(&g2));
        QVERIFY(cm->error() == QContactManager::DoesNotExistError);

        /* Just adjust c to not have any groups */
        c.setGroups(QList<QUniqueId>());
        QVERIFY(cm->saveContact(&c));

        g2 = cm->group(g3.id());
        QVERIFY(g2.id() == g3.id());
        QVERIFY(g2.members().count() == 0);

        /* Now remove again */
        QVERIFY(cm->removeGroup(g3.id()));
        QCOMPARE(cm->error(), QContactManager::NoError);

        /* Test double remove */
        QVERIFY(cm->removeGroup(g3.id()) == false);
        QCOMPARE(cm->error(), QContactManager::DoesNotExistError);

        /* Test that g has no members (but is not empty) as well */
        g2 = cm->group(g.id());
        QVERIFY(!g2.isEmpty());
        QVERIFY(g2.id() == g.id());
        QVERIFY(g2.members().count() == 0);

        /* Remove g as well */
        QVERIFY(cm->removeGroup(g.id()));

        /* And remove c as well */
        QVERIFY(cm->removeContact(c.id()));

        /*
         * Now we should hopefully be back to our normal number of groups
         * (unless some other process added groups behind our back.. )
         */
        QCOMPARE(cm->groups().count(), origGroups.count());

    } else {
        /* Negative testing - make sure it fails properly */
        QList<QUniqueId> groups = cm->groups();
        QVERIFY(groups.count() == 0);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        QContactGroup g;
        g.addMember(1);
        g.addMember(2);

        /* Test adding a null doesn't crash, before we do anything else */
        QVERIFY(cm->saveGroup(0) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        QVERIFY(cm->saveGroup(&g) == false);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        QVERIFY(cm->removeGroup(1) == false);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        g = cm->group(1);
        QVERIFY(g.isEmpty());
        QVERIFY(g.id() == 0);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
    }
    delete cm;
}

void tst_QContactManager::add()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    QContact alice;
    QContactName na;
    na.setFirst("Alice");
    na.setLast("inWonderland");
    alice.saveDetail(&na);
    QContactDisplayLabel label = alice.displayLabel();
    label.setLabel(cm->synthesiseDisplayLabel(alice));
    label.setSynthesised(true);
    alice.setDisplayLabel(label);

    QContactPhoneNumber ph;
    ph.setNumber("1234567");
    ph.setContexts("Home");
    ph.setSubTypes(QStringList("Mobile"));

    alice.saveDetail(&ph);
    int currCount = cm->contacts().count();
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);

    QVERIFY(alice.id() != 0);
    QCOMPARE(cm->contacts().count(), currCount+1);

    QContact added = cm->contact(alice.id());
    QVERIFY(added.id() != 0);
    QVERIFY(added.id() == alice.id());

    if (!isSuperset(added, alice)) {
        dumpContacts(cm);
        dumpContactDifferences(added, alice);
        QCOMPARE(added, alice);
    }

    // now try adding a contact that does not exist in the database with non-zero id
    QContact nonexistent;
    nonexistent.setDisplayLabel("nonexistent contact");
    QVERIFY(cm->saveContact(&nonexistent));       // should work
    QVERIFY(cm->removeContact(nonexistent.id())); // now nonexistent has an id which does not exist
    QVERIFY(!cm->saveContact(&nonexistent));      // hence, should fail
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);
    nonexistent.setId(QUniqueId(0));
    QVERIFY(cm->saveContact(&nonexistent));       // after setting id to zero, should save
    QVERIFY(cm->removeContact(nonexistent.id()));

    // now try adding a "megacontact"
    // - get list of all definitions supported by the manager
    // - add one detail of each definition to a contact
    // - save the contact
    // - read it back
    // - ensure that it's the same.
    QContact megacontact;
    QMap<QString, QContactDetailDefinition> defmap = cm->detailDefinitions();
    QList<QContactDetailDefinition> defs = defmap.values();
    foreach (const QContactDetailDefinition def, defs) {
        QContactDetail det(def.name());
        QMap<QString, QContactDetailDefinition::Field> fieldmap = def.fields();
        QStringList fieldKeys = fieldmap.keys();
        foreach (const QString& fieldKey, fieldKeys) {
            QContactDetailDefinition::Field currentField = fieldmap.value(fieldKey);
            if (!currentField.allowableValues.isEmpty()) {
                // we want to save a value that will be accepted.
                if (currentField.dataType == QVariant::StringList)
                    det.setValue(fieldKey, QStringList() << currentField.allowableValues.first().toString());
                else if (currentField.dataType == QVariant::List)
                    det.setValue(fieldKey, QVariantList() << currentField.allowableValues.first());
                else
                    det.setValue(fieldKey, currentField.allowableValues.first());
            } else {
                // any value of the correct type will be accepted
                bool savedSuccessfully = false;
                QVariant dummyValue = QVariant(fieldKey); // try to get some unique string data
                if (dummyValue.canConvert(currentField.dataType)) {
                    savedSuccessfully = dummyValue.convert(currentField.dataType);
                    if (savedSuccessfully) {
                        // we have successfully created a (supposedly) valid field for this detail.
                        det.setValue(fieldKey, dummyValue);
                        continue;
                    }
                }

                // nope, couldn't save the string value (test); try a date.
                dummyValue = QVariant(QDate::currentDate());
                if (dummyValue.canConvert(currentField.dataType)) {
                    savedSuccessfully = dummyValue.convert(currentField.dataType);
                    if (savedSuccessfully) {
                        // we have successfully created a (supposedly) valid field for this detail.
                        det.setValue(fieldKey, dummyValue);
                        continue;
                    }
                }

                // nope, couldn't convert a string or a date - try the integer value (42)
                dummyValue = QVariant(42);
                if (dummyValue.canConvert(currentField.dataType)) {
                    savedSuccessfully = dummyValue.convert(currentField.dataType);
                    if (savedSuccessfully) {
                        // we have successfully created a (supposedly) valid field for this detail.
                        det.setValue(fieldKey, dummyValue);
                        continue;
                    }
                }

                // if we get here, we don't know what sort of value can be saved...
            }
        }
        megacontact.saveDetail(&det);
    }

    QContactDisplayLabel testLabel;
    testLabel.setLabel("testlabel");
    testLabel.setSynthesised(false);
    megacontact.setDisplayLabel(testLabel);
    QVERIFY(cm->saveContact(&megacontact)); // must be able to save since built from definitions.
    QContact retrievedMegacontact = cm->contact(megacontact.id());
    if (retrievedMegacontact != megacontact) {
        dumpContactDifferences(megacontact, retrievedMegacontact);
        QEXPECT_FAIL("mgr='wince'", "Address Display Label mismatch", Continue);
        QCOMPARE(megacontact, retrievedMegacontact);
    }

    // now a contact with many details of a particular definition
    // this will fail on some backends; how do we query for this capability?
    QContact veryContactable;
    veryContactable.setDisplayLabel("Very Contactable");
    for (int i = 0; i < 50; i++) {
        QString phnStr = QString::number(i);
        QContactPhoneNumber vcphn;
        vcphn.setNumber(phnStr);
        QVERIFY(veryContactable.saveDetail(&vcphn));
    }

    // check that all the numbers were added successfully, and that it can be saved.
    QVERIFY(veryContactable.details(QContactPhoneNumber::DefinitionName).size() == 50);
    QVERIFY(cm->saveContact(&veryContactable));
    QContact retrievedContactable = cm->contact(veryContactable.id());
    if (retrievedContactable != veryContactable) {
        dumpContactDifferences(veryContactable, retrievedContactable);
        QEXPECT_FAIL("mgr='wince'", "Number of phones supported mismatch", Continue);
        QCOMPARE(veryContactable, retrievedContactable);
    }

    delete cm;
}

void tst_QContactManager::update()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    /* Save a new contact first */
    QContact alice;
    QContactName na;
    na.setFirst("Alice");
    na.setLast("inWonderland");
    alice.saveDetail(&na);

    QContactPhoneNumber ph;
    ph.setNumber("1234567");
    ph.setContexts(QStringList("Home"));
    ph.setSubTypes(QStringList("Mobile"));

    alice.saveDetail(&ph);

    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);

    QList<QUniqueId> ids = cm->contacts();
    for(int i = 0; i < ids.count(); i++) {
        QContact current = cm->contact(ids.at(i));
        QContactName nc = current.detail(QContactName::DefinitionName);
        if (nc.first() == "Alice" && nc.last() == "inWonderland") {
            nc.setMiddle("Fictional");
            current.saveDetail(&nc);
            QVERIFY(cm->saveContact(&current));
            QVERIFY(cm->error() == QContactManager::NoError);

            QContact updated = cm->contact(ids.at(i));
            QContactName cn = updated.detail(QContactName::DefinitionName);
            QCOMPARE(cn.middle(), nc.middle());
            delete cm;
            return; // pass
        }
    }

    // now test context/subtype specific cases.
    ph.setContexts(QStringList() << "Work" << "x-nokia-mobility-contacts-test-invalidcontext");
    ph.setSubTypes("Mobile");
    alice.saveDetail(&ph);
    QVERIFY(!cm->saveContact(&alice)); // validation should fail due to invalid context
    ph.setContexts(QStringList()); // empty list
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->contact(alice.id()) == alice);

    delete cm;

    // force fail.
    QVERIFY(false);
}

void tst_QContactManager::remove()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    /* Save a new contact first */
    QContact alice;
    QContactName na;
    na.setFirst("Alice");
    na.setLast("inWonderland");
    alice.saveDetail(&na);

    QContactPhoneNumber ph;
    ph.setNumber("1234567");
    ph.setContexts(QStringList("Home"));
    ph.setSubTypes(QStringList("Mobile"));

    alice.saveDetail(&ph);

    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);

    bool atLeastOne = false;
    QList<QUniqueId> ids = cm->contacts();
    for(int i = 0; i < ids.count(); i++) {
        QContact current = cm->contact(ids.at(i));
        QContactName nc = current.detail(QContactName::DefinitionName);
        if (nc.first() == "Alice" && nc.last() == "inWonderland") {
            int currCount = cm->contacts().count();
            atLeastOne = cm->removeContact(current.id());
            QVERIFY(atLeastOne);
            QCOMPARE(cm->contacts().count(), currCount - 1);
        }
    }

    delete cm;

    QVERIFY(atLeastOne);
}

void tst_QContactManager::batch()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    /* First test null pointer operations */
    QVERIFY(cm->saveContacts(0).count() == 0);
    QVERIFY(cm->error() == QContactManager::BadArgumentError);

    QVERIFY(cm->removeContacts(0).count() == 0);
    QVERIFY(cm->error() == QContactManager::BadArgumentError);

    /* Now add 3 contacts, all valid */
    QContact a;
    QContactName na;
    na.setFirst("XXXXXX Albert");
    a.saveDetail(&na);

    QContact b;
    QContactName nb;
    nb.setFirst("XXXXXX Bob");
    b.saveDetail(&nb);

    QContact c;
    QContactName nc;
    nc.setFirst("XXXXXX Carol");
    c.saveDetail(&nc);

    QList<QContact> contacts;
    contacts << a << b << c;

    QList<QContactManager::Error> errors;

    errors = cm->saveContacts(&contacts);

    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errors.count() == 3);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Make sure our contacts got updated too */
    QVERIFY(contacts.count() == 3);
    QVERIFY(contacts.at(0).id() != 0);
    QVERIFY(contacts.at(1).id() != 0);
    QVERIFY(contacts.at(2).id() != 0);

    QVERIFY(contacts.at(0).detail(QContactName::DefinitionName) == na);
    QVERIFY(contacts.at(1).detail(QContactName::DefinitionName) == nb);
    QVERIFY(contacts.at(2).detail(QContactName::DefinitionName) == nc);

    /* Retrieve again */
    a = cm->contact(contacts.at(0).id());
    b = cm->contact(contacts.at(1).id());
    c = cm->contact(contacts.at(2).id());
    QVERIFY(contacts.at(0).detail(QContactName::DefinitionName) == na);
    QVERIFY(contacts.at(1).detail(QContactName::DefinitionName) == nb);
    QVERIFY(contacts.at(2).detail(QContactName::DefinitionName) == nc);

    /* Now make an update to them all */
    QContactPhoneNumber number;
    number.setNumber("1234567");

    QVERIFY(contacts[0].saveDetail(&number));
    number.setNumber("234567");
    QVERIFY(contacts[1].saveDetail(&number));
    number.setNumber("34567");
    QVERIFY(contacts[2].saveDetail(&number));

    errors = cm->saveContacts(&contacts);
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errors.count() == 3);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Retrieve them and check them again */
    a = cm->contact(contacts.at(0).id());
    b = cm->contact(contacts.at(1).id());
    c = cm->contact(contacts.at(2).id());
    QVERIFY(contacts.at(0).detail(QContactName::DefinitionName) == na);
    QVERIFY(contacts.at(1).detail(QContactName::DefinitionName) == nb);
    QVERIFY(contacts.at(2).detail(QContactName::DefinitionName) == nc);

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
    errors = cm->removeContacts(&ids);
    QVERIFY(errors.count() == 3);
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Make sure all the ids are now 0 */
    QVERIFY(ids.count() == 3);
    QVERIFY(ids.at(0) == 0);
    QVERIFY(ids.at(1) == 0);
    QVERIFY(ids.at(2) == 0);

    /* Make sure the contacts really don't exist any more */
    QVERIFY(cm->contact(a.id()).id() == 0);
    QVERIFY(cm->contact(a.id()).isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    QVERIFY(cm->contact(b.id()).id() == 0);
    QVERIFY(cm->contact(b.id()).isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    QVERIFY(cm->contact(c.id()).id() == 0);
    QVERIFY(cm->contact(c.id()).isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);

    /* Now try removing with all invalid ids (e.g. the ones we just removed) */
    ids.clear();
    ids << a.id() << b.id() << c.id();
    errors = cm->removeContacts(&ids);
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
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
    errors = cm->saveContacts(&contacts);
    /* We can't really say what the error will be.. maybe bad argument, maybe invalid detail */
    QVERIFY(cm->error() != QContactManager::NoError);

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
    errors = cm->saveContacts(&contacts);
    QVERIFY(errors.count() == 3);
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errors.at(0) == QContactManager::NoError);
    QVERIFY(errors.at(1) == QContactManager::NoError);
    QVERIFY(errors.at(2) == QContactManager::NoError);

    /* Now delete 3 items, but with one bad argument */
    ids.clear();
    ids << contacts.at(0).id();
    ids << removedIdForLater;
    ids << contacts.at(2).id();

    errors = cm->removeContacts(&ids);
    QVERIFY(errors.count() == 3);
    QVERIFY(cm->error() != QContactManager::NoError);

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

    delete cm;
}

void tst_QContactManager::invalidManager()
{
    /* Create an invalid manager */
    QContactManager manager("this should never work");
    QVERIFY(manager.managerName() == "invalid");

    /* Now test that all the operations fail */
    QVERIFY(manager.contacts().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QContact foo;
    QContactName nf;
    nf.setLast("Lastname");
    foo.saveDetail(&nf);

    QVERIFY(manager.synthesiseDisplayLabel(foo).isEmpty());
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.saveContact(&foo) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(foo.id() == 0);
    QVERIFY(manager.contacts().count() == 0);

    QVERIFY(manager.contact(foo.id()).id() == 0);
    QVERIFY(manager.contact(foo.id()).isEmpty());
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.removeContact(foo.id()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.saveContacts(0) == QList<QContactManager::Error>());
    QVERIFY(manager.error() == QContactManager::BadArgumentError);


    /* filters */
    QContactFilter f; // matches everything
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    QVERIFY(manager.contacts(QContactFilter()).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contacts(df).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contacts(f || f).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contacts(df || df).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.information()->filterSupported(f) == false);
    QVERIFY(manager.information()->filterSupported(df) == false);

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

    QVERIFY(manager.saveGroup(0) == false);
    QVERIFY(manager.error() == QContactManager::BadArgumentError);

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
    def.setName("new field");
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field currField;
    currField.dataType = QVariant::String;
    fields.insert("value", currField);
    def.setFields(fields);

    QVERIFY(manager.saveDetailDefinition(def) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.detailDefinition("new field").name() == QString());

    QVERIFY(manager.removeDetailDefinition(def.name()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    /* Capabilities */
    QContactManagerInfo* info = manager.information();
    QVERIFY(info->supportedDataTypes().count() == 0);
    QVERIFY(!info->hasFeature(QContactManagerInfo::Groups));
    QVERIFY(!info->hasFeature(QContactManagerInfo::Locking));
    QVERIFY(!info->hasFeature(QContactManagerInfo::Batch));
    QVERIFY(!info->hasFeature(QContactManagerInfo::ActionPreferences));
    QVERIFY(!info->hasFeature(QContactManagerInfo::ReadOnlyDetails));
    QVERIFY(!info->hasFeature(QContactManagerInfo::CreateOnlyDetails));
    QVERIFY(!info->hasFeature(QContactManagerInfo::MutableDefinitions));
    QVERIFY(!info->hasFeature(QContactManagerInfo::NativeFiltering));
    QVERIFY(!info->hasFeature(QContactManagerInfo::NativeSorting));
    QVERIFY(!info->hasFeature(QContactManagerInfo::Synchronous));
    QVERIFY(!info->hasFeature(QContactManagerInfo::Asynchronous));

    /* See if we get the same pointer */
    QVERIFY(info == manager.information());
}

void tst_QContactManager::memoryManager()
{
    QMap<QString, QString> params;
    QContactManager m1("memory");
    params.insert("random", "shouldNotBeUsed");
    QContactManager m2("memory", params);
    params.insert("id", "shouldBeUsed");
    QContactManager m3("memory", params);
    QContactManager m4("memory", params);
    params.insert("id", QString(""));
    QContactManager m5("memory", params); // should be another anonymous

    QContactManagerInfo* info = m1.information();

    QVERIFY(info->hasFeature(QContactManagerInfo::Groups));
    QVERIFY(info->hasFeature(QContactManagerInfo::Batch));
    QVERIFY(info->hasFeature(QContactManagerInfo::ActionPreferences));
    QVERIFY(info->hasFeature(QContactManagerInfo::ReadOnlyDetails));
    QVERIFY(info->hasFeature(QContactManagerInfo::CreateOnlyDetails));
    QVERIFY(info->hasFeature(QContactManagerInfo::MutableDefinitions));
    QVERIFY(info->hasFeature(QContactManagerInfo::Synchronous));

    QVERIFY(!info->hasFeature(QContactManagerInfo::Locking));
    QVERIFY(!info->hasFeature(QContactManagerInfo::NativeFiltering));
    QVERIFY(!info->hasFeature(QContactManagerInfo::NativeSorting));
    QVERIFY(!info->hasFeature(QContactManagerInfo::Asynchronous));

    /* See if we get the same pointer */
    QVERIFY(info == m1.information());

    // add a contact to each of m1, m2, m3
    QContact c;
    QContactName nc;
    nc.setFirst("John");
    nc.setLast("Civilian");
    c.saveDetail(&nc);
    m1.saveContact(&c);
    c.setId(0);
    QContact c2;
    QContactName nc2 = c2.detail(QContactName::DefinitionName);
    c2 = c;
    nc2.setMiddle("Public");
    c2.saveDetail(&nc2);
    m2.saveContact(&c2);            // save c2 first; c will be given a higher id
    m2.saveContact(&c);             // save c to m2
    c.setId(0);
    nc.setSuffix("MD");
    c.saveDetail(&nc);
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

void tst_QContactManager::nameSynthesis_data()
{
    QTest::addColumn<QString>("expected");

    QTest::addColumn<bool>("addname");
    QTest::addColumn<QString>("prefix");
    QTest::addColumn<QString>("first");
    QTest::addColumn<QString>("middle");
    QTest::addColumn<QString>("last");
    QTest::addColumn<QString>("suffix");

    QTest::addColumn<bool>("addcompany");
    QTest::addColumn<QString>("company");

    QTest::addColumn<bool>("addname2");
    QTest::addColumn<QString>("secondprefix");
    QTest::addColumn<QString>("secondfirst");
    QTest::addColumn<QString>("secondmiddle");
    QTest::addColumn<QString>("secondlast");
    QTest::addColumn<QString>("secondsuffix");

    QTest::addColumn<bool>("addcompany2");
    QTest::addColumn<QString>("secondcompany");

    QString e; // empty string.. gets a work out

    /* Various empty ones */
    QTest::newRow("empty contact") << e
            << false << e << e << e << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("empty name") << e
            << true << e << e << e << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("empty names") << e
            << true << e << e << e << e << e
            << false << e
            << true << e << e << e << e << e
            << false << e;
    QTest::newRow("empty org") << e
            << false << e << e << e << e << e
            << true << e
            << false << e << e << e << e << e
            << true << e;
    QTest::newRow("empty orgs") << e
            << false << e << e << e << e << e
            << true << e
            << false << e << e << e << e << e
            << true << e;
    QTest::newRow("empty orgs and names") << e
            << true << e << e << e << e << e
            << true << e
            << true << e << e << e << e << e
            << true << e;

    /* Single values */
    QTest::newRow("prefix") << "Prefix"
            << true << "Prefix" << e << e << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("first") << "First"
            << true << e << "First" << e << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("middle") << "Middle"
            << true << e << e << "Middle" << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("last") << "Last"
            << true << e << e << e << "Last" << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("suffix") << "Suffix"
            << true << e << e << e << e << "Suffix"
            << false << e
            << false << e << e << e << e << e
            << false << e;

    /* Single values in the second name */
    QTest::newRow("prefix in second") << "Prefix"
            << false << "Prefix" << e << e << e << e
            << false << e
            << true << "Prefix" << e << e << e << e
            << false << e;
    QTest::newRow("first in second") << "First"
            << false << e << "First" << e << e << e
            << false << e
            << true << e << "First" << e << e << e
            << false << e;
    QTest::newRow("middle in second") << "Middle"
            << false << e << e << "Middle" << e << e
            << false << e
            << true << e << e << "Middle" << e << e
            << false << e;
    QTest::newRow("last in second") << "Last"
            << false << e << e << e << "Last" << e
            << false << e
            << true << e << e << e << "Last" << e
            << false << e;
    QTest::newRow("suffix in second") << "Suffix"
            << false << e << e << e << e << "Suffix"
            << false << e
            << true << e << e << e << e << "Suffix"
            << false << e;

    /* Multiple name values */
    QTest::newRow("prefix first") << "Prefix First"
            << true << "Prefix" << "First" << e << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("prefix middle") << "Prefix Middle"
            << true << "Prefix" << e << "Middle" << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("prefix last") << "Prefix Last"
            << true << "Prefix" << e << e << "Last" << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("prefix suffix") << "Prefix Suffix"
            << true << "Prefix" << e << e << e << "Suffix"
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("first middle") << "First Middle"
            << true << e << "First" << "Middle" << e << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("first last") << "First Last"
            << true << e << "First" << e << "Last" << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("first suffix") << "First Suffix"
            << true << e << "First" << e << e << "Suffix"
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("middle last") << "Middle Last"
            << true << e << e << "Middle" << "Last" << e
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("middle suffix") << "Middle Suffix"
            << true << e << e << "Middle" << e << "Suffix"
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("last suffix") << "Last Suffix"
            << true << e << e << e << "Last" << "Suffix"
            << false << e
            << false << e << e << e << e << e
            << false << e;

    /* Everything.. */
    QTest::newRow("all name") << "Prefix First Middle Last Suffix"
            << true << "Prefix" << "First" << "Middle" << "Last" << "Suffix"
            << false << e
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("all name second") << "Prefix First Middle Last Suffix"
            << false << "Prefix" << "First" << "Middle" << "Last" << "Suffix"
            << false << e
            << true << "Prefix" << "First" << "Middle" << "Last" << "Suffix"
            << false << e;

    /* Org */
    QTest::newRow("org") << "Company"
            << false << e << e << e << e << e
            << true << "Company"
            << false << e << e << e << e << e
            << false << e;
    QTest::newRow("second org") << "Company"
            << false << e << e << e << e << e
            << false << e
            << false << e << e << e << e << e
            << true << "Company";

    /* Mix */
    QTest::newRow("org and empty name") << "Company"
            << true << e << e << e << e << e
            << true << "Company"
            << false << e << e << e << e << e
            << false << e;

    QTest::newRow("name and empty org") << "Prefix First Middle Last Suffix"
            << true << "Prefix" << "First" << "Middle" << "Last" << "Suffix"
            << false << e
            << false << e << e << e << e << e
            << false << e;

    /* names are preferred to orgs */
    QTest::newRow("name and org") << "Prefix First Middle Last Suffix"
            << true << "Prefix" << "First" << "Middle" << "Last" << "Suffix"
            << true << "Company"
            << false << e << e << e << e << e
            << false << e;

}

void tst_QContactManager::nameSynthesis()
{
    QContactManager cm("memory");

    QFETCH(QString, expected);

    QFETCH(QString, prefix);
    QFETCH(QString, first);
    QFETCH(QString, middle);
    QFETCH(QString, last);
    QFETCH(QString, suffix);
    QFETCH(QString, company);

    QFETCH(QString, secondprefix);
    QFETCH(QString, secondfirst);
    QFETCH(QString, secondmiddle);
    QFETCH(QString, secondlast);
    QFETCH(QString, secondsuffix);
    QFETCH(QString, secondcompany);

    QFETCH(bool, addname);
    QFETCH(bool, addname2);
    QFETCH(bool, addcompany);
    QFETCH(bool, addcompany2);

    /* Test the default name synthesis code */
    QContact c;

    QContactName name, name2;
    QContactOrganisation org, org2;

    name.setPrefix(prefix);
    name.setFirst(first);
    name.setMiddle(middle);
    name.setLast(last);
    name.setSuffix(suffix);

    name2.setPrefix(secondprefix);
    name2.setFirst(secondfirst);
    name2.setMiddle(secondmiddle);
    name2.setLast(secondlast);
    name2.setSuffix(secondsuffix);

    org.setDisplayLabel(company);
    org2.setDisplayLabel(secondcompany);

    if (addname)
        c.saveDetail(&name);
    if (addname2)
        c.saveDetail(&name2);
    if (addcompany)
        c.saveDetail(&org);
    if (addcompany2)
        c.saveDetail(&org2);

    // Finally!
    QCOMPARE(cm.synthesiseDisplayLabel(c), expected);
}

void tst_QContactManager::contactValidation()
{
    /* Use the memory engine as a reference (validation is not engine specific) */
    QContactManager* cm = new QContactManager("memory");
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

    uniqueDef.setName("UniqueDetail");
    uniqueDef.setFields(fields);
    uniqueDef.setUnique(true);

    QVERIFY(cm->saveDetailDefinition(uniqueDef));

    QContactDetailDefinition restrictedDef;
    restrictedDef.setName("RestrictedDetail");
    fields.clear();
    field.allowableValues = QVariantList() << "One" << "Two" << "Three";
    fields.insert("value", field);
    restrictedDef.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(restrictedDef));

    QContactDetailDefinition createOnlyDef;
    createOnlyDef.setName("CreateOnlyDetail");
    createOnlyDef.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    fields.clear();
    field.allowableValues.clear();
    fields.insert("value", field);
    createOnlyDef.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(createOnlyDef));

    QContactDetailDefinition createOnlyUniqueDef;
    createOnlyUniqueDef.setName("CreateOnlyUniqueDetail");
    createOnlyUniqueDef.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    createOnlyUniqueDef.setUnique(true);
    fields.clear();
    field.allowableValues.clear();
    fields.insert("value", field);
    createOnlyUniqueDef.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(createOnlyUniqueDef));

    // first, test an invalid definition
    QContactDetail d1 = QContactDetail("UnknownDefinition");
    d1.setValue("test", "test");
    c.saveDetail(&d1);
    QVERIFY(!cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d1);

    // second, test an invalid uniqueness constraint
    QContactDetail d2 = QContactDetail("UniqueDetail");
    d2.setValue("value", "test");
    c.saveDetail(&d2);

    // One unique should be ok
    QVERIFY(cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::NoError);

    // Two uniques should not be ok
    QContactDetail d3 = QContactDetail("UniqueDetail");
    d3.setValue("value", "test2");
    c.saveDetail(&d3);
    QVERIFY(!cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::AlreadyExistsError);
    c.removeDetail(&d3);
    c.removeDetail(&d2);

    // third, test an invalid field name
    QContactDetail d4 = QContactDetail(QContactPhoneNumber::DefinitionName);
    d4.setValue("test", "test");
    c.saveDetail(&d4);
    QVERIFY(!cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d4);

    // fourth, test an invalid field data type
    QContactDetail d5 = QContactDetail(QContactPhoneNumber::DefinitionName);
    d5.setValue(QContactPhoneNumber::FieldNumber, QDateTime::currentDateTime());
    c.saveDetail(&d5);
    QVERIFY(!cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d5);

    // fifth, test an invalid field value (not in the allowed list)
    QContactDetail d6 = QContactDetail("RestrictedDetail");
    d6.setValue("value", "Seven"); // not in One, Two or Three
    c.saveDetail(&d6);
    QVERIFY(!cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::InvalidDetailError);
    c.removeDetail(&d6);

    /* Now a valid value */
    d6.setValue("value", "Two");
    c.saveDetail(&d6);
    QVERIFY(cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::NoError);
    c.removeDetail(&d6);

    // Test a completely valid one.
    QContactDetail d7 = QContactDetail(QContactPhoneNumber::DefinitionName);
    d7.setValue(QContactPhoneNumber::FieldNumber, "0123456");
    c.saveDetail(&d7);
    QVERIFY(cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::NoError);
    c.removeDetail(&d7);

    /* Test a write once detail */
    QContactDetail d8 = QContactDetail("CreateOnlyDetail");
    d8.setValue("value", "First value");
    c.saveDetail(&d8);

    QVERIFY(cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::NoError);

    /* Changing or adding some other detail should also be fine, and resaving it */
    QContactPhoneNumber p1;
    p1.setNumber("123467");
    c.saveDetail(&p1);
    QVERIFY(cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::NoError);

    /* Adding a second detail should be fine */
    QContactDetail d9 = QContactDetail("CreateOnlyDetail");
    d9.setValue("value", "Second value");
    c.saveDetail(&d9);
    QVERIFY(cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::NoError);

    /* Changing a value should fail */
    d8.setValue("value", "Third value");
    c.saveDetail(&d8);

    QVERIFY(!cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::DetailAccessError);

    c.removeDetail(&d8);
    /* Removing a create only should also fail */
    QVERIFY(!cm->saveContact(&c));
    QCOMPARE(cm->error(), QContactManager::DetailAccessError);

    /* Have to test group validation, as well */
    QContact d;
    d.setDisplayLabel("Dummy contact");

    QVERIFY(cm->saveContact(&d));
    QList<QUniqueId> ids;
    d.setGroups(ids);
    QVERIFY(cm->saveContact(&d));

    // Add a few groups
    QContactGroup g1, g2, g3;
    g1.setName("Group XXXXXX-1");
    g2.setName("Group XXXXXX-2");
    g3.setName("To be removed");

    QVERIFY(cm->saveGroup(&g1));
    QVERIFY(cm->saveGroup(&g2));
    QVERIFY(cm->saveGroup(&g3));

    QVERIFY(cm->removeGroup(g3.id()));
    QVERIFY(cm->group(g3.id()).id() == 0);

    // now g3.id is an invalid group

    ids << g1.id() << g2.id();
    d.setGroups(ids);
    QVERIFY(cm->saveContact(&d));
    QVERIFY(cm->error() == QContactManager::NoError);

    ids << g3.id();
    d.setGroups(ids);
    QVERIFY(!cm->saveContact(&d));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);

    QVERIFY(cm->removeGroup(g2.id()));
    QVERIFY(cm->removeGroup(g1.id()));
    QVERIFY(cm->removeContact(d.id()));

    delete cm;
}

void tst_QContactManager::signalEmission()
{
    QFETCH(QString, uri);
    QContactManager* m1 = QContactManager::fromUri(uri);
    QContactManager* m2 = QContactManager::fromUri(uri);

    QVERIFY(m1->information()->hasFeature(QContactManagerInfo::ExternalNotifications) ==
        m2->information()->hasFeature(QContactManagerInfo::ExternalNotifications));

    qRegisterMetaType<QUniqueId>("QUniqueId");
    qRegisterMetaType<QList<QUniqueId> >("QList<QUniqueId>");
    QSignalSpy spyCA(m1, SIGNAL(contactsAdded(QList<QUniqueId>)));
    QSignalSpy spyCM(m1, SIGNAL(contactsChanged(QList<QUniqueId>)));
    QSignalSpy spyCR(m1, SIGNAL(contactsRemoved(QList<QUniqueId>)));
    QSignalSpy spyGA(m1, SIGNAL(groupsAdded(QList<QUniqueId>)));
    QSignalSpy spyGC(m1, SIGNAL(groupsChanged(QList<QUniqueId>)));
    QSignalSpy spyGR(m1, SIGNAL(groupsRemoved(QList<QUniqueId>)));

    QList<QVariant> args;
    QContact c;
    QUniqueId temp;
    QList<QContact> batchAdd;
    QList<QUniqueId> batchRemove;
    QList<QUniqueId> sigids;
    int addSigCount = 0; // the expected signal counts.
    int modSigCount = 0;
    int remSigCount = 0;

    // verify add emits signal added
    QContactName nc;
    nc.setFirst("John");
    c.saveDetail(&nc);
    m1->saveContact(&c);
    addSigCount += 1;
    QCOMPARE(spyCA.count(), addSigCount);
    args = spyCA.takeFirst();
    addSigCount -= 1;
    QVERIFY(args.count() == 1);
    temp = QUniqueId(args.at(0).value<quint32>());

    // verify save modified emits signal changed
    nc.setLast("Citizen");
    c.saveDetail(&nc);
    m1->saveContact(&c);
    modSigCount += 1;
    QCOMPARE(spyCM.count(), modSigCount);
    args = spyCM.takeFirst();
    modSigCount -= 1;
    QVERIFY(args.count() == 1);
    QCOMPARE(temp, QUniqueId(args.at(0).value<quint32>()));

    // verify remove emits signal removed
    m1->removeContact(c.id());
    remSigCount += 1;
    QCOMPARE(spyCR.count(), remSigCount);
    args = spyCR.takeFirst();
    remSigCount -= 1;
    QVERIFY(args.count() == 1);
    QCOMPARE(temp, QUniqueId(args.at(0).value<quint32>()));

    // verify multiple adds works as advertised
    QContact c2, c3;
    QContactName nc2, nc3;
    nc2.setFirst("Mark");
    nc3.setFirst("Garry");
    c2.saveDetail(&nc2);
    c3.saveDetail(&nc3);
    QVERIFY(!m1->saveContact(&c)); // saving contact with nonexistent id fails
    QVERIFY(m1->saveContact(&c2));
    addSigCount += 1;
    QVERIFY(m1->saveContact(&c3));
    addSigCount += 1;
    QCOMPARE(spyCM.count(), modSigCount);
    QCOMPARE(spyCA.count(), addSigCount);

    // verify multiple modifies works as advertised
    nc2.setLast("M.");
    c2.saveDetail(&nc2);
    m1->saveContact(&c2);
    modSigCount += 1;
    nc2.setPrefix("Mr.");
    nc3.setLast("G.");
    c2.saveDetail(&nc2);
    c3.saveDetail(&nc3);
    m1->saveContact(&c2);
    modSigCount += 1;
    m1->saveContact(&c3);
    modSigCount += 1;
    QCOMPARE(spyCM.count(), modSigCount);

    // verify multiple removes works as advertised
    m1->removeContact(c3.id());
    remSigCount += 1;
    m1->removeContact(c2.id());
    remSigCount += 1;
    QCOMPARE(spyCR.count(), remSigCount);

    QVERIFY(!m1->removeContact(c.id())); // not saved.

    /* Now test the batch equivalents */
    spyCA.clear();
    spyCM.clear();
    spyCR.clear();

    /* Batch adds - set ids to zero so add succeeds. */
    c.setId(0);
    c2.setId(0);
    c3.setId(0);
    batchAdd << c << c2 << c3;
    m1->saveContacts(&batchAdd);

    QVERIFY(batchAdd.count() == 3);
    c = batchAdd.at(0);
    c2 = batchAdd.at(1);
    c3 = batchAdd.at(2);

    QCOMPARE(spyCA.count(), 1); // coalesced updates
    QCOMPARE(spyCM.count(), 0);
    QCOMPARE(spyCR.count(), 0);
    args = spyCA.takeFirst();
    QVERIFY(args.size() == 1);
    sigids = args.at(0).value<QList<QUniqueId> >();
    QVERIFY(sigids.count() == 3);
    // The order is indeterminate
    QVERIFY(sigids.contains(c.id()));
    QVERIFY(sigids.contains(c2.id()));
    QVERIFY(sigids.contains(c3.id()));

    /* Batch modifies */
    c.setDisplayLabel("This is modified number 1");
    c2.setDisplayLabel("This is a modified number 2");
    c3.setDisplayLabel("This is a modified number 3");

    batchAdd.clear();
    batchAdd << c << c2 << c3;
    m1->saveContacts(&batchAdd);
    QCOMPARE(spyCA.count(), 0);
    QCOMPARE(spyCM.count(), 1); // 1 signal only
    QCOMPARE(spyCR.count(), 0);
    args = spyCM.takeFirst();
    QVERIFY(args.size() == 1);
    sigids = args.at(0).value<QList<QUniqueId> >();
    QVERIFY(sigids.count() == 3);
    QVERIFY(sigids.contains(c.id()));
    QVERIFY(sigids.contains(c2.id()));
    QVERIFY(sigids.contains(c3.id()));

    /* Batch removes */
    batchRemove << c.id() << c2.id() << c3.id();
    m1->removeContacts(&batchRemove);
    QCOMPARE(spyCA.count(), 0);
    QCOMPARE(spyCM.count(), 0);
    QCOMPARE(spyCR.count(), 1); // 1 signal only
    args = spyCR.takeFirst();
    QVERIFY(args.size() == 1);
    sigids = args.at(0).value<QList<QUniqueId> >();
    QVERIFY(sigids.count() == 3);
    QVERIFY(sigids.contains(c.id()));
    QVERIFY(sigids.contains(c2.id()));
    QVERIFY(sigids.contains(c3.id()));

    /* Now some cross manager testing */
    if (m1->information()->hasFeature(QContactManagerInfo::ExternalNotifications)) {
        // verify that signals are emitted for modifications made to other managers (same id).
        QContactName ncs = c.detail(QContactName::DefinitionName);
        ncs.setSuffix("Test");
        c.saveDetail(&ncs);
        c.setId(0); // reset id so save can succeed.
        m2->saveContact(&c);
        ncs.setPrefix("Test2");
        c.saveDetail(&ncs);
        m2->saveContact(&c);
        QCOMPARE(spyCA.count(), 1); // check that we received the update signals.
        QCOMPARE(spyCM.count(), 1); // check that we received the update signals.
        m2->removeContact(c.id());
        QCOMPARE(spyCR.count(), 1); // check that we received the remove signal.
    }

    /* Check groups, if supported */
    if (m1->information()->hasFeature(QContactManagerInfo::Groups)) {
        QContactGroup g1;
        g1.setName("XXXXXX Group");

        /* For cross notification testing, add everything to m2 and watch on m1 */
        QContactManager *adder = m2->information()->hasFeature(QContactManagerInfo::ExternalNotifications) ? m2 : m1;

        adder->removeContact(c.id());
        adder->removeContact(c2.id());

        // Reset the counts so we don't get confused.
        spyCA.clear();
        spyCM.clear();
        spyCR.clear();
        spyGA.clear();
        spyGC.clear();
        spyGR.clear();

        // Make sure we have two contacts to test with
        c.setId(0);
        c2.setId(0);
        batchAdd.clear();
        batchAdd << c << c2;
        QVERIFY(adder->saveContacts(&batchAdd).count() == 2);
        QVERIFY(adder->error() == QContactManager::NoError);

        c = batchAdd.at(0);
        c2 = batchAdd.at(1);

        QVERIFY(spyCA.count() == 1); // only one signal, even though there's 2 contacts.
        QVERIFY(spyCM.count() == 0);
        QVERIFY(spyCR.count() == 0);
        QVERIFY(spyGA.count() == 0);
        QVERIFY(spyGC.count() == 0);
        QVERIFY(spyGR.count() == 0);

        args = spyCA.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 2);
        // The order is indeterminate
        QVERIFY(sigids.contains(c.id()));
        QVERIFY(sigids.contains(c2.id()));

        // Save an empty group
        QVERIFY(adder->saveGroup(&g1));

        QVERIFY(spyCA.count() == 0);
        QVERIFY(spyCM.count() == 0);
        QVERIFY(spyCR.count() == 0);
        QVERIFY(spyGA.count() == 1);
        QVERIFY(spyGC.count() == 0);
        QVERIFY(spyGR.count() == 0);

        args = spyGA.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == g1.id());

        // Add one member (via the group)
        g1.addMember(c.id());
        QVERIFY(adder->saveGroup(&g1));

        QVERIFY(spyCA.count() == 0);
        QVERIFY(spyCM.count() == 1);
        QVERIFY(spyCR.count() == 0);
        QVERIFY(spyGA.count() == 0);
        QVERIFY(spyGC.count() == 1);
        QVERIFY(spyGR.count() == 0);

        args = spyCM.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == c.id());

        args = spyGC.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == g1.id());

        // Add one member (via the contact)
        c2.setGroups(QList<QUniqueId>() << g1.id());
        QVERIFY(adder->saveContact(&c2));

        QVERIFY(spyCA.count() == 0);
        QVERIFY(spyCM.count() == 1);
        QVERIFY(spyCR.count() == 0);
        QVERIFY(spyGA.count() == 0);
        QVERIFY(spyGC.count() == 1);
        QVERIFY(spyGR.count() == 0);

        args = spyCM.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == c2.id());

        args = spyGC.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == g1.id());

        // delete from group (via removing contact)
        batchRemove.clear();
        batchRemove << c.id();
        QVERIFY(adder->removeContacts(&batchRemove).value(0, QContactManager::AlreadyExistsError) == QContactManager::NoError);
        QVERIFY(spyCA.count() == 0);
        QVERIFY(spyCM.count() == 0);
        QVERIFY(spyCR.count() == 1);
        QVERIFY(spyGA.count() == 0);
        QVERIFY(spyGC.count() == 1);
        QVERIFY(spyGR.count() == 0);

        args = spyCR.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == c.id());

        args = spyGC.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == g1.id());

        // Delete the group
        QVERIFY(adder->removeGroup(g1.id()));
        QVERIFY(spyCA.count() == 0);
        QVERIFY(spyCM.count() == 1);
        QVERIFY(spyCR.count() == 0);
        QVERIFY(spyGA.count() == 0);
        QVERIFY(spyGC.count() == 0);
        QVERIFY(spyGR.count() == 1);

        args = spyCM.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == c2.id());

        args = spyGR.takeFirst();
        QVERIFY(args.size() == 1);
        sigids = args.at(0).value<QList<QUniqueId> >();
        QVERIFY(sigids.count() == 1);
        QVERIFY(sigids.value(0) == g1.id());

    }

    delete m1;
    delete m2;
}

void tst_QContactManager::errorStayingPut()
{
    /* Make sure that when we clone a manager, we don't clone the error */
    QMap<QString, QString> params;
    params.insert("id", "error isolation test");
    QContactManager m1("memory",params);

    QVERIFY(m1.error() == QContactManager::NoError);

    /* Remove an invalid contact to get an error */
    QVERIFY(m1.removeContact(0) == false);
    QVERIFY(m1.error() == QContactManager::DoesNotExistError);

    /* Create a new manager with hopefully the same backend */
    QContactManager m2("memory", params);

    QVERIFY(m1.error() == QContactManager::DoesNotExistError);
    QVERIFY(m2.error() == QContactManager::NoError);

    /* Cause an error on the other ones and check the first is not affected */
    m2.saveContacts(0);
    QVERIFY(m1.error() == QContactManager::DoesNotExistError);
    QVERIFY(m2.error() == QContactManager::BadArgumentError);

    QContact c;
    QContactDetail d("This does not exist and if it does this will break");
    d.setValue("Value that also doesn't exist", 5);
    c.saveDetail(&d);

    QVERIFY(m1.saveContact(&c) == false);
    QVERIFY(m1.error() == QContactManager::InvalidDetailError);
    QVERIFY(m2.error() == QContactManager::BadArgumentError);
}

void tst_QContactManager::detailDefinitions()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    QContactManagerInfo* info = cm->information();
    QMap<QString, QContactDetailDefinition> defs = cm->detailDefinitions();

    /* Try to make a credible definition */
    QContactDetailDefinition newDef;
    QContactDetailDefinition::Field field;
    QMap<QString, QContactDetailDefinition::Field> fields;
    field.dataType = info->supportedDataTypes().value(0);
    fields.insert("New Value", field);
    newDef.setName("New Definition");
    newDef.setFields(fields);

    /* Updated version of an existing definition */
    QContactDetailDefinition updatedDef = defs.begin().value();
    fields = updatedDef.fields();
    fields.insert("New Value", field);
    updatedDef.setFields(fields);

    /* A detail definition with valid allowed values (or really just one) */
    QContactDetailDefinition allowedDef = newDef;
    field.allowableValues.append(QVariant(field.dataType));
    fields.clear();
    fields.insert("Restricted value", field);
    allowedDef.setFields(fields);

    /* XXX A create only definition */

    /* Many invalid definitions */
    QContactDetailDefinition noIdDef;
    noIdDef.setFields(fields);

    QContactDetailDefinition noFieldsDef;
    noFieldsDef.setName("No fields");

    QContactDetailDefinition readOnlyDef;
    readOnlyDef.setName("Read only");
    readOnlyDef.setAccessConstraint(QContactDetailDefinition::ReadOnly);
    readOnlyDef.setFields(fields);

    QContactDetailDefinition invalidFieldKeyDef;
    invalidFieldKeyDef.setName("Invalid field key");
    QMap<QString, QContactDetailDefinition::Field> badfields;
    badfields.insert(QString(), field);
    invalidFieldKeyDef.setFields(badfields);

    QContactDetailDefinition invalidFieldTypeDef;
    invalidFieldTypeDef.setName("Invalid field type");
    badfields.clear();
    QContactDetailDefinition::Field badfield;
    badfield.dataType = (QVariant::Type) qMetaTypeId<UnsupportedMetatype>();
    badfields.insert("Bad type", badfield);
    invalidFieldTypeDef.setFields(badfields);

    QContactDetailDefinition invalidAllowedValuesDef;
    invalidAllowedValuesDef.setName("Invalid field allowed values");
    badfields.clear();
    badfield.dataType = field.dataType; // use a supported type
    badfield.allowableValues << "String" << 5; // but unsupported value
    badfields.insert("Bad allowed", badfield);
    invalidAllowedValuesDef.setFields(badfields);

    /* XXX Multiply defined fields.. depends on semantics. */

    if (info->hasFeature(QContactManagerInfo::MutableDefinitions)) {
        /* First do some negative testing */

        /* Bad add class */
        QVERIFY(cm->saveDetailDefinition(QContactDetailDefinition()) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        /* Bad remove string */
        QVERIFY(cm->removeDetailDefinition(QString()) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(noIdDef) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(noFieldsDef) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(readOnlyDef) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(invalidFieldKeyDef) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(invalidFieldTypeDef) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(invalidAllowedValuesDef) == false);
        QVERIFY(cm->error() == QContactManager::BadArgumentError);

        /* Check that our new definition doesn't already exist */
        QVERIFY(cm->detailDefinition(newDef.name()).isEmpty());
        QVERIFY(cm->error() == QContactManager::DoesNotExistError);

        QVERIFY(cm->removeDetailDefinition(newDef.name()) == false);
        QVERIFY(cm->error() == QContactManager::DoesNotExistError);

        /* Add a new definition */
        QVERIFY(cm->saveDetailDefinition(newDef) == true);
        QVERIFY(cm->error() == QContactManager::NoError);

        /* Now retrieve it */
        QContactDetailDefinition def = cm->detailDefinition(newDef.name());
        QVERIFY(def == newDef);

        /* Update it */
        QMap<QString, QContactDetailDefinition::Field> newFields = def.fields();
        newFields.insert("Another new value", field);
        newDef.setFields(newFields);

        QVERIFY(cm->saveDetailDefinition(newDef) == true);
        QVERIFY(cm->error() == QContactManager::NoError);

        QVERIFY(cm->detailDefinition(newDef.name()) == newDef);

        /* Remove it */
        QVERIFY(cm->removeDetailDefinition(newDef.name()) == true);
        QVERIFY(cm->error() == QContactManager::NoError);

        /* and make sure it does not exist any more */
        QVERIFY(cm->detailDefinition(newDef.name()) == QContactDetailDefinition());
        QVERIFY(cm->error() == QContactManager::DoesNotExistError);

        /* Add the other good one */
        QVERIFY(cm->saveDetailDefinition(allowedDef) == true);
        QVERIFY(cm->error() == QContactManager::NoError);

        QVERIFY(allowedDef == cm->detailDefinition(allowedDef.name()));

        /* and remove it */
        QVERIFY(cm->removeDetailDefinition(allowedDef.name()) == true);
        QVERIFY(cm->detailDefinition(allowedDef.name()) == QContactDetailDefinition());
        QVERIFY(cm->error() == QContactManager::DoesNotExistError);

    } else {
        /* Bad add class */
        QVERIFY(cm->saveDetailDefinition(QContactDetailDefinition()) == false);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        /* Make sure we can't add/remove/modify detail definitions */
        QVERIFY(cm->removeDetailDefinition(QString()) == false);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        /* Try updating an existing definition */
        QVERIFY(cm->saveDetailDefinition(updatedDef) == false);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        /* Try removing an existing definition */
        QVERIFY(cm->removeDetailDefinition(updatedDef.name()) == false);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
    }

    delete cm;
}

void tst_QContactManager::displayName()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    /*
     * Very similar to the tst_QContact functions, except we test
     * saving and retrieving contacts updates the display label
     */

    /* Try to make this a bit more consistent by using a single name */
    QContact d;
    QContactName name;
    name.setFirst("Wesley");

    QVERIFY(d.displayLabel().label().isEmpty());
    QVERIFY(d.saveDetail(&name));

    QVERIFY(cm->saveContact(&d));

    QString synth = cm->synthesiseDisplayLabel(d);

    /*
     * The display label is not updated until you save the contact, or
     * do it manually.
     */
    d = cm->contact(d.id());
    QVERIFY(!d.isEmpty());

    QCOMPARE(d.displayLabel().label(), synth);
    QVERIFY(d.displayLabel().isSynthesised() == true);

    /* Set something else */
    d.setDisplayLabel("The grand old duchess");
    QVERIFY(d.displayLabel().label() == "The grand old duchess");
    QVERIFY(d.displayLabel().isSynthesised() == false);

    /* Remove the detail via removeDetail */
    QContactDisplayLabel old = d.displayLabel();
    int count = d.details().count();
    QVERIFY(d.removeDetail(&old));
    QVERIFY(d.isEmpty() == false);
    QVERIFY(d.details().count() == count); // it should not be removed, only cleared (!)

    /* Save the contact again */
    QVERIFY(cm->saveContact(&d));
    d = cm->contact(d.id());
    QVERIFY(!d.isEmpty());

    /* Make sure we go back to the old synth version */
    QVERIFY(d.displayLabel().isSynthesised() == true);
    QCOMPARE(d.displayLabel().label(), synth);

    /* And delete the contact */
    QVERIFY(cm->removeContact(d.id()));

    delete cm;
}

QTEST_MAIN(tst_QContactManager)
#include "tst_qcontactmanager.moc"
