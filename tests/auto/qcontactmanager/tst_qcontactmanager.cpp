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

#include <QtTest/QtTest>

#include "qtcontacts.h"
#include "qcontactchangeset.h"
#include "qcontactmanager_p.h"

// Eventually these will make it into qtestcase.h
// but we might need to tweak the timeout values here.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif


#define QTRY_WAIT(code, __expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)

//TESTED_CLASS=
//TESTED_FILES=

// to get QFETCH to work with the template expression...
typedef QMap<QString,QString> tst_QContactManager_QStringMap;
Q_DECLARE_METATYPE(tst_QContactManager_QStringMap)
Q_DECLARE_METATYPE(QList<QContactLocalId>)

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
    QList<QContactDetail> removeAllDefaultDetails(const QList<QContactDetail>& details);
    void addManagers(); // add standard managers to the data

    QContactManagerDataHolder managerDataHolder;

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
    void metadata();
    void nullIdOperations();
    void add();
    void update();
    void remove();
    void batch();
    void signalEmission();
    void detailDefinitions();
    void displayName();
    void actionPreferences();
    void selfContactId();
    void detailOrders();
    void relationships();
    void contactType();

    /* Tests that take no data */
    void contactValidation();
    void errorStayingPut();
    void ctors();
    void invalidManager();
    void memoryManager();
    void changeSet();

    /* data providers (mostly all engines) */
    void uriParsing_data(); // Special data
    void nameSynthesis_data(); // Special data
    void metadata_data() {addManagers();}
    void nullIdOperations_data() {addManagers();}
    void add_data() {addManagers();}
    void update_data() {addManagers();}
    void remove_data() {addManagers();}
    void batch_data() {addManagers();}
    void signalEmission_data() {addManagers();}
    void detailDefinitions_data() {addManagers();}
    void displayName_data() {addManagers();}
    void actionPreferences_data() {addManagers();}
    void selfContactId_data() {addManagers();}
    void detailOrders_data() {addManagers();}
    void relationships_data() {addManagers();}
    void contactType_data() {addManagers();}
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
    QCOMPARE(n1.customLabel(), n2.customLabel());

#if 0 // XXX TODO: update this after removing deprecated API
    // Check the display label
    QCOMPARE(a.displayLabel().label(), b.displayLabel().label());
    QCOMPARE(a.displayLabel().isSynthesized(), b.displayLabel().isSynthesized());
#endif

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

    // Now dump the extra details that were unmatched in A (note that DisplayLabel and Type are always present).
    aDetails = a.details();
    bDetails = b.details();
    foreach(QContactDetail d, aDetails) {
        if (d.definitionName() != QContactDisplayLabel::DefinitionName && d.definitionName() != QContactType::DefinitionName)
            qDebug() << "A contact had extra detail:" << d.definitionName() << d.values();
    }
    // and same for B
    foreach(QContactDetail d, bDetails) {
        if (d.definitionName() != QContactDisplayLabel::DefinitionName && d.definitionName() != QContactType::DefinitionName)
            qDebug() << "B contact had extra detail:" << d.definitionName() << d.values();
    }

    // now test specifically the display label and the type
    if (a.displayLabel() != b.displayLabel()) {
        qDebug() << "A contact display label =" << a.displayLabel().label();
        qDebug() << "B contact display label =" << b.displayLabel().label();
    }
    if (a.type() != b.type()) {
        qDebug() << "A contact type =" << a.type();
        qDebug() << "B contact type =" << b.type();
    }
}

bool tst_QContactManager::isSuperset(const QContact& ca, const QContact& cb)
{
    // returns true if contact ca is a superset of contact cb
    // we use this test instead of equality because dynamic information
    // such as presence/location, and synthesised information such as
    // display label and (possibly) type, may differ between a contact
    // in memory and the contact in the managed store.

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

    // check for contact type updates
    if (!a.type().isEmpty())
        if (!b.type().isEmpty())
            if (a.type() != b.type())
                return false; // nonempty type is different.

    // Now check to see if b has any details remaining; if so, a is not a superset.
    // Note that the DisplayLabel and Type can never be removed.
    if (b.details().size() > 2
            || (b.details().size() == 2 && (b.details().value(0).definitionName() != QContactDisplayLabel::DefinitionName
                                            || b.details().value(1).definitionName() != QContactType::DefinitionName)))
        return false;
    return true;
}

void tst_QContactManager::dumpContact(const QContact& contact)
{
    QContactManager m;
    qDebug() << "Contact: " << contact.id().localId() << "(" << m.synthesizeDisplayLabel(contact) << ")";
    QList<QContactDetail> details = contact.details();
    foreach(QContactDetail d, details) {
        qDebug() << "  " << d.definitionName() << ":";
        qDebug() << "    Vals:" << d.values();
    }
}

void tst_QContactManager::dumpContacts(QContactManager *cm)
{
    QList<QContactLocalId> ids = cm->contacts();

    qDebug() << "There are" << ids.count() << "contacts in" << cm->managerUri();

    foreach(QContactLocalId id, ids) {
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


void tst_QContactManager::metadata()
{
    // ensure that the backend is publishing its metadata (name / parameters / uri) correctly
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);
    QVERIFY(QContactManager::buildUri(cm->managerName(), cm->managerParameters()) == cm->managerUri());
}


void tst_QContactManager::nullIdOperations()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);
    QVERIFY(!cm->removeContact(QContactLocalId()));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);


    QContact c = cm->contact(QContactLocalId());
    QVERIFY(c.id() == QContactId());
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
    QContactManager cm4(cm.managerUri()); // should fail

    QContactManager* cm5 = QContactManager::fromUri(QContactManager::buildUri(defaultStore, QMap<QString, QString>()));
    QContactManager* cm6 = QContactManager::fromUri(cm.managerUri()); // uri is not a name; should fail.
    QContactManager* cm9 = QContactManager::fromUri(QString(), &parent);

    QVERIFY(cm9->parent() == &parent);

    /* OLD TEST WAS THIS: */
    //QCOMPARE(cm.managerUri(), cm2.managerUri());
    //QCOMPARE(cm.managerUri(), cm3.managerUri());
    //QCOMPARE(cm.managerUri(), cm5->managerUri());
    //QCOMPARE(cm.managerUri(), cm6->managerUri());
    //QCOMPARE(cm.managerUri(), cm9->managerUri());
    /* NEW TEST IS THIS: Test that the names of the managers are the same */
    QCOMPARE(cm.managerName(), cm2.managerName());
    QCOMPARE(cm.managerName(), cm3.managerName());
    QCOMPARE(cm.managerName(), cm5->managerName());
    QCOMPARE(cm.managerName(), cm6->managerName());
    QCOMPARE(cm.managerName(), cm9->managerName());

    QVERIFY(cm.managerUri() != cm4.managerUri()); // don't pass a uri to the ctor

    /* Test that we get invalid stores when we do silly things */
    QContactManager em("non existent");
    QContactManager em2("non existent", QMap<QString, QString>());
    QContactManager em3("memory", randomParameters);

    /* Also invalid, since we don't have one of these anyway */
    QContactManager* em4 = QContactManager::fromUri("invalid uri");
    QContactManager* em5 = QContactManager::fromUri(QContactManager::buildUri("nonexistent", QMap<QString, QString>()));
    QContactManager* em6 = QContactManager::fromUri(em3.managerUri());


    /*
     * Sets of stores that should be equivalent:
     * - 1, 2, 4, 5
     * - 3, 6
     */

    /* First some URI testing for equivalent stores */
    QVERIFY(em.managerUri() == em2.managerUri());
    QVERIFY(em.managerUri() == em5->managerUri());
    QVERIFY(em.managerUri() == em4->managerUri());
    QVERIFY(em2.managerUri() == em4->managerUri());
    QVERIFY(em2.managerUri() == em5->managerUri());
    QVERIFY(em4->managerUri() == em5->managerUri());

    QVERIFY(em3.managerUri() == em6->managerUri());

    /* Test the stores that should not be the same */
    QVERIFY(em.managerUri() != em3.managerUri());
    QVERIFY(em.managerUri() != em6->managerUri());

    /* now the components */
    QCOMPARE(em.managerName(), QString("invalid"));
    QCOMPARE(em2.managerName(), QString("invalid"));
    QCOMPARE(em3.managerName(), QString("memory"));
    QCOMPARE(em4->managerName(), QString("invalid"));
    QCOMPARE(em5->managerName(), QString("invalid"));
    QCOMPARE(em6->managerName(), QString("memory"));
    QCOMPARE(em.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em2.managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em4->managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em5->managerParameters(), tst_QContactManager_QStringMap());
    QCOMPARE(em3.managerParameters(), em6->managerParameters()); // memory engine discards the given params, replaces with id.


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

void tst_QContactManager::add()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    QContact alice;
    QContactName na;
    na.setFirst("Alice");
    na.setLast("inWonderland");
    na.setCustomLabel(cm->synthesizeDisplayLabel(alice));
    alice.saveDetail(&na);

    QContactPhoneNumber ph;
    ph.setNumber("1234567");
    ph.setContexts("Home");
    ph.setSubTypes(QStringList("Mobile"));

    alice.saveDetail(&ph);
    int currCount = cm->contacts().count();
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);

    QVERIFY(alice.id() != QContactId());
    QCOMPARE(cm->contacts().count(), currCount+1);

    QContact added = cm->contact(alice.id().localId());
    QVERIFY(added.id() != QContactId());
    QVERIFY(added.id() == alice.id());

    if (!isSuperset(added, alice)) {
        dumpContacts(cm);
        dumpContactDifferences(added, alice);
        QCOMPARE(added, alice);
    }

    // now try adding a contact that does not exist in the database with non-zero id
    QContact nonexistent;
    QContactName name;
    name.setCustomLabel("nonexistent contact");
    nonexistent.saveDetail(&name);
    QVERIFY(cm->saveContact(&nonexistent));       // should work
    QVERIFY(cm->removeContact(nonexistent.id().localId())); // now nonexistent has an id which does not exist
    QVERIFY(!cm->saveContact(&nonexistent));      // hence, should fail
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);
    nonexistent.setId(QContactId());
    QVERIFY(cm->saveContact(&nonexistent));       // after setting id to zero, should save
    QVERIFY(cm->removeContact(nonexistent.id().localId()));

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
        // if the definition is read only, we cannot create details of the definition, so skip it.
        if (def.accessConstraint() == QContactDetailDefinition::ReadOnly) {
            continue;
        }

        // otherwise, create a new detail of the given type and save it to the contact
        QContactDetail det(def.name());
        QMap<QString, QContactDetailDefinitionField> fieldmap = def.fields();
        QStringList fieldKeys = fieldmap.keys();
        foreach (const QString& fieldKey, fieldKeys) {
            // get the field, and check to see that it's not constrained.
            QContactDetailDefinitionField currentField = fieldmap.value(fieldKey);
            if (currentField.accessConstraint() == QContactDetailDefinitionField::ReadOnly) {
                // we cannot write to this field.
                continue;
            }

            // we can write to this field.  attempt to create a worthy value
            if (!currentField.allowableValues().isEmpty()) {
                // we want to save a value that will be accepted.
                if (currentField.dataType() == QVariant::StringList)
                    det.setValue(fieldKey, QStringList() << currentField.allowableValues().first().toString());
                else if (currentField.dataType() == QVariant::List)
                    det.setValue(fieldKey, QVariantList() << currentField.allowableValues().first());
                else
                    det.setValue(fieldKey, currentField.allowableValues().first());
            } else {
                // any value of the correct type will be accepted
                bool savedSuccessfully = false;
                QVariant dummyValue = QVariant(fieldKey); // try to get some unique string data
                if (dummyValue.canConvert(currentField.dataType())) {
                    savedSuccessfully = dummyValue.convert(currentField.dataType());
                    if (savedSuccessfully) {
                        // we have successfully created a (supposedly) valid field for this detail.
                        det.setValue(fieldKey, dummyValue);
                        continue;
                    }
                }

                // nope, couldn't save the string value (test); try a date.
                dummyValue = QVariant(QDate::currentDate());
                if (dummyValue.canConvert(currentField.dataType())) {
                    savedSuccessfully = dummyValue.convert(currentField.dataType());
                    if (savedSuccessfully) {
                        // we have successfully created a (supposedly) valid field for this detail.
                        det.setValue(fieldKey, dummyValue);
                        continue;
                    }
                }

                // nope, couldn't convert a string or a date - try the integer value (42)
                dummyValue = QVariant(42);
                if (dummyValue.canConvert(currentField.dataType())) {
                    savedSuccessfully = dummyValue.convert(currentField.dataType());
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

    QContactName testName;
    testName.setCustomLabel("testlabel");
    megacontact.saveDetail(&testName);
    QVERIFY(cm->saveContact(&megacontact)); // must be able to save since built from definitions.
    QContact retrievedMegacontact = cm->contact(megacontact.id().localId());
    if (retrievedMegacontact != megacontact) {
        dumpContactDifferences(megacontact, retrievedMegacontact);
        QEXPECT_FAIL("mgr='wince'", "Address Display Label mismatch", Continue);
        QCOMPARE(megacontact, retrievedMegacontact);
    }

    // now a contact with many details of a particular definition
    // this will fail on some backends; how do we query for this capability?
    QContact veryContactable;
    QContactName contactableName;
    contactableName.setCustomLabel("Very Contactable");
    veryContactable.saveDetail(&contactableName);
    for (int i = 0; i < 50; i++) {
        QString phnStr = QString::number(i);
        QContactPhoneNumber vcphn;
        vcphn.setNumber(phnStr);
        QVERIFY(veryContactable.saveDetail(&vcphn));
    }

    // check that all the numbers were added successfully, and that it can be saved.
    QVERIFY(veryContactable.details(QContactPhoneNumber::DefinitionName).size() == 50);
    QVERIFY(cm->saveContact(&veryContactable));
    QContact retrievedContactable = cm->contact(veryContactable.id().localId());
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

    QList<QContactLocalId> ids = cm->contacts();
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
    QVERIFY(cm->contact(alice.id().localId()) == alice);

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
    QList<QContactLocalId> ids = cm->contacts();
    for(int i = 0; i < ids.count(); i++) {
        QContact current = cm->contact(ids.at(i));
        QContactName nc = current.detail(QContactName::DefinitionName);
        if (nc.first() == "Alice" && nc.last() == "inWonderland") {
            int currCount = cm->contacts().count();
            atLeastOne = cm->removeContact(current.id().localId());
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
    QVERIFY(contacts.at(0).id() != QContactId());
    QVERIFY(contacts.at(1).id() != QContactId());
    QVERIFY(contacts.at(2).id() != QContactId());

    QVERIFY(contacts.at(0).detail(QContactName::DefinitionName) == na);
    QVERIFY(contacts.at(1).detail(QContactName::DefinitionName) == nb);
    QVERIFY(contacts.at(2).detail(QContactName::DefinitionName) == nc);

    /* Retrieve again */
    a = cm->contact(contacts.at(0).id().localId());
    b = cm->contact(contacts.at(1).id().localId());
    c = cm->contact(contacts.at(2).id().localId());
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
    a = cm->contact(contacts.at(0).id().localId());
    b = cm->contact(contacts.at(1).id().localId());
    c = cm->contact(contacts.at(2).id().localId());
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
    QList<QContactLocalId> ids;
    QContactLocalId removedIdForLater = b.id().localId();
    ids << a.id().localId() << b.id().localId() << c.id().localId();
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
    QVERIFY(cm->contact(a.id().localId()).id() == QContactId());
    QVERIFY(cm->contact(a.id().localId()).isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    QVERIFY(cm->contact(b.id().localId()).id() == QContactId());
    QVERIFY(cm->contact(b.id().localId()).isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    QVERIFY(cm->contact(c.id().localId()).id() == QContactId());
    QVERIFY(cm->contact(c.id().localId()).isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);

    /* Now try removing with all invalid ids (e.g. the ones we just removed) */
    ids.clear();
    ids << a.id().localId() << b.id().localId() << c.id().localId();
    errors = cm->removeContacts(&ids);
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    QVERIFY(errors.count() == 3);
    QVERIFY(errors.at(0) == QContactManager::DoesNotExistError);
    QVERIFY(errors.at(1) == QContactManager::DoesNotExistError);
    QVERIFY(errors.at(2) == QContactManager::DoesNotExistError);

    /* Try adding some new ones again, this time one with an error */
    contacts.clear();
    a.setId(QContactId());
    b.setId(QContactId());
    c.setId(QContactId());

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
        QVERIFY(contacts.at(0).id() != QContactId());
    } else {
        QVERIFY(contacts.at(0).id() == QContactId());
    }
    /* B should definitely have failed */
    QVERIFY(errors.at(1) == QContactManager::InvalidDetailError);
    QVERIFY(contacts.at(1).id() == QContactId());

    /* C might have gone through */
    if (errors.at(2) == QContactManager::NoError) {
        QVERIFY(contacts.at(2).id() != QContactId());
    } else {
        QVERIFY(contacts.at(2).id() == QContactId());
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
    ids << contacts.at(0).id().localId();
    ids << removedIdForLater;
    ids << contacts.at(2).id().localId();

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

    QVERIFY(manager.synthesizeDisplayLabel(foo).isEmpty());
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.saveContact(&foo) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(foo.id() == QContactId());
    QVERIFY(manager.contacts().count() == 0);

    QVERIFY(manager.contact(foo.id().localId()).id() == QContactId());
    QVERIFY(manager.contact(foo.id().localId()).isEmpty());
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.removeContact(foo.id().localId()) == false);
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
    QVERIFY(manager.contacts(f | f).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contacts(df | df).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.filterSupported(f) == false);
    QVERIFY(manager.filterSupported(df) == false);

    QList<QContact> list;
    list << foo;
    QVERIFY(manager.saveContacts(&list) == (QList<QContactManager::Error>() << QContactManager::NotSupportedError));
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.removeContacts(0) == QList<QContactManager::Error>());
    QVERIFY(manager.error() == QContactManager::BadArgumentError);

    QList<QContactLocalId> idlist;
    idlist << foo.id().localId();
    QVERIFY(manager.removeContacts(&idlist) == (QList<QContactManager::Error>() << QContactManager::NotSupportedError));
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    /* Detail definitions */
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);

    QContactDetailDefinition def;
    def.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    def.setUnique(true);
    def.setName("new field");
    QMap<QString, QContactDetailDefinitionField> fields;
    QContactDetailDefinitionField currField;
    currField.setDataType(QVariant::String);
    fields.insert("value", currField);
    def.setFields(fields);

    QVERIFY(manager.saveDetailDefinition(def) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.detailDefinition("new field").name() == QString());

    QVERIFY(manager.removeDetailDefinition(def.name()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);

    /* Capabilities */
    QVERIFY(manager.supportedDataTypes().count() == 0);
    QVERIFY(!manager.hasFeature(QContactManager::ActionPreferences));
    QVERIFY(!manager.hasFeature(QContactManager::MutableDefinitions));
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

    QVERIFY(m1.hasFeature(QContactManager::ActionPreferences));
    QVERIFY(m1.hasFeature(QContactManager::MutableDefinitions));
    QVERIFY(m1.hasFeature(QContactManager::Anonymous));

    // add a contact to each of m1, m2, m3
    QContact c;
    QContactName nc;
    nc.setFirst("John");
    nc.setLast("Civilian");
    c.saveDetail(&nc);
    m1.saveContact(&c);
    c.setId(QContactId());
    QContact c2;
    QContactName nc2 = c2.detail(QContactName::DefinitionName);
    c2 = c;
    nc2.setMiddle("Public");
    c2.saveDetail(&nc2);
    m2.saveContact(&c2);            // save c2 first; c will be given a higher id
    m2.saveContact(&c);             // save c to m2
    c.setId(QContactId());
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
    m2.removeContact(c2.id().localId());
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
    QVERIFY(m4.removeContact(c.id().localId()));
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
    QContactOrganization org, org2;

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

    org.setName(company);
    org2.setName(secondcompany);

    if (addname)
        c.saveDetail(&name);
    if (addname2)
        c.saveDetail(&name2);
    if (addcompany)
        c.saveDetail(&org);
    if (addcompany2)
        c.saveDetail(&org2);

    // Finally!
    QCOMPARE(cm.synthesizeDisplayLabel(c), expected);
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
    QMap<QString, QContactDetailDefinitionField> fields;
    QContactDetailDefinitionField field;
    field.setDataType(QVariant::String);
    fields.insert("value", field);

    uniqueDef.setName("UniqueDetail");
    uniqueDef.setFields(fields);
    uniqueDef.setUnique(true);

    QVERIFY(cm->saveDetailDefinition(uniqueDef));

    QContactDetailDefinition restrictedDef;
    restrictedDef.setName("RestrictedDetail");
    fields.clear();
    field.setAllowableValues(QVariantList() << "One" << "Two" << "Three");
    fields.insert("value", field);
    restrictedDef.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(restrictedDef));

    QContactDetailDefinition createOnlyDef;
    createOnlyDef.setName("CreateOnlyDetail");
    createOnlyDef.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    fields.clear();
    field.setAllowableValues(QList<QVariant>());
    fields.insert("value", field);
    createOnlyDef.setFields(fields);

    QVERIFY(cm->saveDetailDefinition(createOnlyDef));

    QContactDetailDefinition createOnlyUniqueDef;
    createOnlyUniqueDef.setName("CreateOnlyUniqueDetail");
    createOnlyUniqueDef.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    createOnlyUniqueDef.setUnique(true);
    fields.clear();
    field.allowableValues().clear();
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



    delete cm;
}

void tst_QContactManager::signalEmission()
{
    QFETCH(QString, uri);
    QContactManager* m1 = QContactManager::fromUri(uri);
    QContactManager* m2 = QContactManager::fromUri(uri);

    QVERIFY(m1->hasFeature(QContactManager::Anonymous) ==
        m2->hasFeature(QContactManager::Anonymous));

    qRegisterMetaType<QContactLocalId>("QContactLocalId");
    qRegisterMetaType<QList<QContactLocalId> >("QList<QContactLocalId>");
    QSignalSpy spyCA(m1, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    QSignalSpy spyCM(m1, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    QSignalSpy spyCR(m1, SIGNAL(contactsRemoved(QList<QContactLocalId>)));

    QList<QVariant> args;
    QContact c;
    QContactLocalId temp;
    QList<QContact> batchAdd;
    QList<QContactLocalId> batchRemove;
    QList<QContactLocalId> sigids;
    int addSigCount = 0; // the expected signal counts.
    int modSigCount = 0;
    int remSigCount = 0;

    // verify add emits signal added
    QContactName nc;
    nc.setFirst("John");
    c.saveDetail(&nc);
    m1->saveContact(&c);
    addSigCount += 1;
    QTRY_COMPARE(spyCA.count(), addSigCount);
    args = spyCA.takeFirst();
    addSigCount -= 1;
    QVERIFY(args.count() == 1);
    temp = QContactLocalId(args.at(0).value<quint32>());

    // verify save modified emits signal changed
    nc.setLast("Citizen");
    c.saveDetail(&nc);
    m1->saveContact(&c);
    modSigCount += 1;
    QTRY_COMPARE(spyCM.count(), modSigCount);
    args = spyCM.takeFirst();
    modSigCount -= 1;
    QVERIFY(args.count() == 1);
    QCOMPARE(temp, QContactLocalId(args.at(0).value<quint32>()));

    // verify remove emits signal removed
    m1->removeContact(c.id().localId());
    remSigCount += 1;
    QTRY_COMPARE(spyCR.count(), remSigCount);
    args = spyCR.takeFirst();
    remSigCount -= 1;
    QVERIFY(args.count() == 1);
    QCOMPARE(temp, QContactLocalId(args.at(0).value<quint32>()));

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
    QTRY_COMPARE(spyCM.count(), modSigCount);
    QTRY_COMPARE(spyCA.count(), addSigCount);

    // verify multiple modifies works as advertised
    nc2.setLast("M.");
    c2.saveDetail(&nc2);
    QVERIFY(m1->saveContact(&c2));
    modSigCount += 1;
    nc2.setPrefix("Mr.");
    nc3.setLast("G.");
    c2.saveDetail(&nc2);
    c3.saveDetail(&nc3);
    QVERIFY(m1->saveContact(&c2));
    modSigCount += 1;
    QVERIFY(m1->saveContact(&c3));
    modSigCount += 1;
    QTRY_COMPARE(spyCM.count(), modSigCount);

    // verify multiple removes works as advertised
    m1->removeContact(c3.id().localId());
    remSigCount += 1;
    m1->removeContact(c2.id().localId());
    remSigCount += 1;
    QTRY_COMPARE(spyCR.count(), remSigCount);

    QVERIFY(!m1->removeContact(c.id().localId())); // not saved.

    /* Now test the batch equivalents */
    spyCA.clear();
    spyCM.clear();
    spyCR.clear();

    /* Batch adds - set ids to zero so add succeeds. */
    c.setId(QContactId());
    c2.setId(QContactId());
    c3.setId(QContactId());
    batchAdd << c << c2 << c3;
    m1->saveContacts(&batchAdd);

    QVERIFY(batchAdd.count() == 3);
    c = batchAdd.at(0);
    c2 = batchAdd.at(1);
    c3 = batchAdd.at(2);

    /* We basically loop, processing events, until we've seen an Add signal for each contact */
    sigids.clear();

    QTRY_WAIT( while(spyCA.size() > 0) {sigids += spyCA.takeFirst().at(0).value<QList<QContactLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));
    QTRY_COMPARE(spyCM.count(), 0);
    QTRY_COMPARE(spyCR.count(), 0);

    /* Batch modifies */
    QContactName modifiedName = c.detail(QContactName::DefinitionName);
    modifiedName.setCustomLabel("This is modified number 1");
    c.saveDetail(&modifiedName);
    modifiedName = c2.detail(QContactName::DefinitionName);
    modifiedName.setCustomLabel("This is modified number 2");
    c2.saveDetail(&modifiedName);
    modifiedName = c3.detail(QContactName::DefinitionName);
    modifiedName.setCustomLabel("This is modified number 3");
    c3.saveDetail(&modifiedName);

    batchAdd.clear();
    batchAdd << c << c2 << c3;
    m1->saveContacts(&batchAdd);

    sigids.clear();
    QTRY_WAIT( while(spyCM.size() > 0) {sigids += spyCM.takeFirst().at(0).value<QList<QContactLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));

    /* Batch removes */
    batchRemove << c.id().localId() << c2.id().localId() << c3.id().localId();
    m1->removeContacts(&batchRemove);

    sigids.clear();
    QTRY_WAIT( while(spyCR.size() > 0) {sigids += spyCR.takeFirst().at(0).value<QList<QContactLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));

    QTRY_COMPARE(spyCA.count(), 0);
    QTRY_COMPARE(spyCM.count(), 0);

    /* Now some cross manager testing */
    if (!m1->hasFeature(QContactManager::Anonymous)) {
        // verify that signals are emitted for modifications made to other managers (same id).
        QContactName ncs = c.detail(QContactName::DefinitionName);
        ncs.setSuffix("Test");
        c.saveDetail(&ncs);
        c.setId(QContactId()); // reset id so save can succeed.
        m2->saveContact(&c);
        ncs.setPrefix("Test2");
        c.saveDetail(&ncs);
        m2->saveContact(&c);
        QTRY_COMPARE(spyCA.count(), 1); // check that we received the update signals.
        QTRY_COMPARE(spyCM.count(), 1); // check that we received the update signals.
        m2->removeContact(c.localId());
        QTRY_COMPARE(spyCR.count(), 1); // check that we received the remove signal.
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
    QMap<QString, QContactDetailDefinition> defs = cm->detailDefinitions();

    /* Try to make a credible definition */
    QContactDetailDefinition newDef;
    QContactDetailDefinitionField field;
    QMap<QString, QContactDetailDefinitionField> fields;
    field.setDataType(cm->supportedDataTypes().value(0));
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
    field.setAllowableValues(field.allowableValues() << (QVariant(field.dataType())));
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
    QMap<QString, QContactDetailDefinitionField> badfields;
    badfields.insert(QString(), field);
    invalidFieldKeyDef.setFields(badfields);

    QContactDetailDefinition invalidFieldTypeDef;
    invalidFieldTypeDef.setName("Invalid field type");
    badfields.clear();
    QContactDetailDefinitionField badfield;
    badfield.setDataType((QVariant::Type) qMetaTypeId<UnsupportedMetatype>());
    badfields.insert("Bad type", badfield);
    invalidFieldTypeDef.setFields(badfields);

    QContactDetailDefinition invalidAllowedValuesDef;
    invalidAllowedValuesDef.setName("Invalid field allowed values");
    badfields.clear();
    badfield.setDataType(field.dataType()); // use a supported type
    badfield.setAllowableValues(QList<QVariant>() << "String" << 5); // but unsupported value
    badfields.insert("Bad allowed", badfield);
    invalidAllowedValuesDef.setFields(badfields);

    /* XXX Multiply defined fields.. depends on semantics. */

    if (cm->hasFeature(QContactManager::MutableDefinitions)) {
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
        QMap<QString, QContactDetailDefinitionField> newFields = def.fields();
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
    QSKIP("This test needs to be updated after we remove the deprecated API!", SkipSingle);
#if 0 // XXX TODO: update displayName tests without deprecated API
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

    QString synth = cm->synthesizeDisplayLabel(d);

    /*
     * The display label is not updated until you save the contact, or
     * do it manually.
     */
    d = cm->contact(d.id().localId());
    QVERIFY(!d.isEmpty());

    QCOMPARE(d.displayLabel().label(), synth);
    QVERIFY(d.displayLabel().isSynthesized() == true);

    /* Set something else */
    d.setDisplayLabel("The grand old duchess");
    QVERIFY(d.displayLabel().label() == "The grand old duchess");
    QVERIFY(d.displayLabel().isSynthesized() == false);

    /* Remove the detail via removeDetail */
    QContactDisplayLabel old = d.displayLabel();
    int count = d.details().count();
    QVERIFY(d.removeDetail(&old));
    QVERIFY(d.isEmpty() == false);
    QVERIFY(d.details().count() == count); // it should not be removed, only cleared (!)

    /* Save the contact again */
    QVERIFY(cm->saveContact(&d));
    d = cm->contact(d.id().localId());
    QVERIFY(!d.isEmpty());

    /* Make sure we go back to the old synth version */
    QVERIFY(d.displayLabel().isSynthesized() == true);
    QCOMPARE(d.displayLabel().label(), synth);

    /* And delete the contact */
    QVERIFY(cm->removeContact(d.id().localId()));

    delete cm;
#endif
}

void tst_QContactManager::actionPreferences()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    // early out if the manager doesn't support action preference saving.
    if (!cm->hasFeature(QContactManager::ActionPreferences)) {
        delete cm;
        QSKIP("Manager does not support action preferences", SkipSingle);
    }

    // create a sample contact
    QContactAvatar a;
    a.setAvatar("test.png");
    QContactPhoneNumber p1;
    p1.setNumber("12345");
    QContactPhoneNumber p2;
    p2.setNumber("34567");
    QContactPhoneNumber p3;
    p3.setNumber("56789");
    QContactUrl u;
    u.setUrl("http://test.nokia.com");
    QContactName n;
    n.setCustomLabel("TestContact");

    QContact c;
    c.saveDetail(&a);
    c.saveDetail(&p1);
    c.saveDetail(&p2);
    c.saveDetail(&p3);
    c.saveDetail(&u);
    c.saveDetail(&n);

    // set a preference for dialing a particular saved phonenumber.
    c.setPreferredDetail("Dial", p2);

    QVERIFY(cm->saveContact(&c));          // save the contact
    QContact loaded = cm->contact(c.id().localId()); // reload the contact

    // test that the preference was saved correctly.
    QContactDetail pref = loaded.preferredDetail("Dial");
    QVERIFY(pref == p2);

    cm->removeContact(c.id().localId());
    delete cm;
}

void tst_QContactManager::changeSet()
{
    QContactLocalId id(1);

    QContactChangeSet cs;
    QVERIFY(cs.addedContacts().isEmpty());
    QVERIFY(cs.changedContacts().isEmpty());
    QVERIFY(cs.removedContacts().isEmpty());

    cs.addedContacts().insert(id);
    QVERIFY(!cs.addedContacts().isEmpty());
    QVERIFY(cs.changedContacts().isEmpty());
    QVERIFY(cs.removedContacts().isEmpty());
    QVERIFY(cs.addedContacts().contains(id));

    cs.changedContacts().insert(id);
    cs.changedContacts().insert(id);
    QVERIFY(cs.changedContacts().size() == 1); // set, should only be added once.
    QVERIFY(!cs.addedContacts().isEmpty());
    QVERIFY(!cs.changedContacts().isEmpty());
    QVERIFY(cs.removedContacts().isEmpty());
    QVERIFY(cs.changedContacts().contains(id));

    QVERIFY(cs.dataChanged() == false);
    QContactChangeSet cs2;
    cs2 = cs;
    QVERIFY(cs.addedContacts() == cs2.addedContacts());
    cs.emitSignals(0);

    cs2.clear();
    QVERIFY(cs.addedContacts() != cs2.addedContacts());

    QContactChangeSet cs3(cs2);
    QVERIFY(cs.addedContacts() != cs3.addedContacts());
    QVERIFY(cs2.addedContacts() == cs3.addedContacts());

    cs.setDataChanged(true);
    QVERIFY(cs.dataChanged() == true);
    QVERIFY(cs.dataChanged() != cs2.dataChanged());
    QVERIFY(cs.dataChanged() != cs3.dataChanged());
    cs.emitSignals(0);
}

void tst_QContactManager::selfContactId()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    // early out if the manager doesn't support self contact id saving
    QContactLocalId selfContact = cm->selfContactId();
    if (!cm->hasFeature(QContactManager::SelfContact)) {
        // ensure that the error codes / return values are meaningful failures.
        QVERIFY(cm->error() == QContactManager::DoesNotExistError);
        QVERIFY(!cm->setSelfContactId(QContactLocalId(123)));
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        QSKIP("Manager does not support the concept of a self-contact", SkipSingle);
    }

    // create a new "self" contact and retrieve its Id
    QVERIFY(cm->error() == QContactManager::NoError || cm->error() == QContactManager::DoesNotExistError);
    QContact self;
    QContactPhoneNumber selfPhn;
    selfPhn.setNumber("12345");
    self.saveDetail(&selfPhn);
    if (!cm->saveContact(&self)) {
        QSKIP("Unable to save the generated self contact", SkipSingle);
    }
    QContactLocalId newSelfContact = self.localId();

    // Setup signal spy
    qRegisterMetaType<QContactLocalId>("QContactLocalId");
    QSignalSpy spy(cm, SIGNAL(selfContactIdChanged(QContactLocalId,QContactLocalId)));
        
    // Set new self contact
    QVERIFY(cm->setSelfContactId(newSelfContact));
    QVERIFY(cm->error() == QContactManager::NoError);
    QTRY_VERIFY(spy.count() == 1);
    QVERIFY(spy.at(0).count() == 2);
    // note: for some reason qvariant_cast<QContactLocalId>(spy.at(0).at(0)) returns always zero
    // because the type is not recognized. Hence the ugly casting below.
    QVERIFY(*((const QContactLocalId*) spy.at(0).at(0).constData()) == selfContact);
    QVERIFY(*((const QContactLocalId*) spy.at(0).at(1).constData()) == newSelfContact);
    QVERIFY(cm->selfContactId() == newSelfContact);
    
    // Remove self contact
    if(!cm->removeContact(self.localId())) {
        QSKIP("Unable to remove self contact", SkipSingle);
    }        
    QTRY_VERIFY(spy.count() == 2);
    QVERIFY(spy.at(1).count() == 2);
    QVERIFY(*((const QContactLocalId*) spy.at(1).at(0).constData()) == newSelfContact);
    QVERIFY(*((const QContactLocalId*) spy.at(1).at(1).constData()) == QContactLocalId(0));
    QVERIFY(cm->selfContactId() == QContactLocalId(0)); // ensure reset after removed.    
    
    // reset to original state.
    cm->setSelfContactId(selfContact);
}

QList<QContactDetail> tst_QContactManager::removeAllDefaultDetails(const QList<QContactDetail>& details)
{
    QList<QContactDetail> newlist;
    foreach (const QContactDetail d, details) {
        if (d.definitionName() != QContactDisplayLabel::DefinitionName
                && d.definitionName() != QContactType::DefinitionName
                && d.definitionName() != QContactTimestamp::DefinitionName) {
            newlist << d;
        }
    }
    return newlist;
}

void tst_QContactManager::detailOrders()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    QContact a;
    QContactName name;
    QContactPhoneNumber number;
    QContactEmailAddress email;
    QContactAddress address;

    name.setFirst("Aaron");
    name.setLast("Aaronson");
    number.setNumber("555-1212");
    email.setEmailAddress("aaron@example.com");
    address.setStreet("Brandl St");
    address.setRegion("Brisbane");

    a.saveDetail(&name);
    a.saveDetail(&number);
    a.saveDetail(&email);
    a.saveDetail(&address);

    QVERIFY(cm->saveContact(&a));

    a = cm->contact(a.id().localId());

    QList<QContactDetail> details = removeAllDefaultDetails(a.details());
    QVERIFY(details.count() == 4);
    QVERIFY(details.at(0).definitionName() == QContactName::DefinitionName);
    QVERIFY(details.at(1).definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(details.at(2).definitionName() == QContactEmailAddress::DefinitionName);
    QVERIFY(details.at(3).definitionName() == QContactAddress::DefinitionName);

    QVERIFY(a.removeDetail(&details[2]));
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    details = removeAllDefaultDetails(a.details());
    QVERIFY(details.count() == 3);
    QVERIFY(details.at(0).definitionName() == QContactName::DefinitionName);
    QVERIFY(details.at(1).definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(details.at(2).definitionName() == QContactAddress::DefinitionName);

    a.saveDetail(&email);
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    details = removeAllDefaultDetails(a.details());
    QVERIFY(details.count() == 4);
    QVERIFY(details.at(0).definitionName() == QContactName::DefinitionName);
    QVERIFY(details.at(1).definitionName() == QContactPhoneNumber::DefinitionName);
    QVERIFY(details.at(2).definitionName() == QContactAddress::DefinitionName);
    QVERIFY(details.at(3).definitionName() == QContactEmailAddress::DefinitionName);

    QVERIFY(a.removeDetail(&details[3]));
    QVERIFY(a.removeDetail(&details[1]));
    QVERIFY(a.removeDetail(&details[0]));
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    details = removeAllDefaultDetails(a.details());
    QVERIFY(details.count() == 1);
    QVERIFY(details.at(0).definitionName() == QContactAddress::DefinitionName);

    QVERIFY(cm->removeContact(a.id().localId()));
    QVERIFY(cm->error() == QContactManager::NoError);
}

void tst_QContactManager::relationships()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    if (!cm->hasFeature(QContactManager::Relationships))
        QSKIP("Skipping: This manager does not support relationships!", SkipSingle);

    int totalRelationships = cm->relationships().size();
    int totalManagerRelationships = cm->relationships(QContactRelationship::IsManagerOf).size();

    QStringList availableRelationshipTypes = cm->supportedRelationshipTypes();
    if (availableRelationshipTypes.isEmpty()) {
        // if empty, but has the relationships feature, then it must support arbitrary types.
        // so, add a few types that we can use.
        // if it doesn't support relationships, then it doesn't matter anyway.
        availableRelationshipTypes.append(QContactRelationship::IsManagerOf);
        availableRelationshipTypes.append(QContactRelationship::IsSpouseOf);
        availableRelationshipTypes.append(QContactRelationship::IsAssistantOf);
    }

    QContact source;
    QContact dest1, dest2, dest3;
    QContactPhoneNumber n1, n2, n3;
    n1.setNumber("1");
    n2.setNumber("2");
    n3.setNumber("3");

    dest1.saveDetail(&n1);
    dest2.saveDetail(&n2);
    dest3.saveDetail(&n3);

    cm->saveContact(&source);
    cm->saveContact(&dest1);
    cm->saveContact(&dest2);
    cm->saveContact(&dest3);

    if (!cm->hasFeature(QContactManager::Relationships)) {
        // ensure that the operations all fail as required.
        QContactRelationship r1, r2, r3;
        r1.setFirst(source.id());
        r1.setSecond(dest1.id());
        r1.setRelationshipType(availableRelationshipTypes.at(0));
        r2.setFirst(source.id());
        r2.setSecond(dest2.id());
        r2.setRelationshipType(availableRelationshipTypes.at(0));
        r3.setFirst(source.id());
        r3.setSecond(dest3.id());
        r3.setRelationshipType(availableRelationshipTypes.at(0));

        QList<QContactRelationship> batchList;
        batchList << r2 << r3;

        // test save and remove
        QVERIFY(!cm->saveRelationship(&r1));
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        QVERIFY(!cm->removeRelationship(r1));
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        QVERIFY(cm->saveRelationships(&batchList).isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        QVERIFY(cm->removeRelationships(batchList).isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        // test retrieval
        QList<QContactRelationship> retrieveList;
        retrieveList = cm->relationships(source.id(), QContactRelationshipFilter::First);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(source.id(), QContactRelationshipFilter::Second);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(source.id()); // Either
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);


        retrieveList = cm->relationships(availableRelationshipTypes.at(0), source.id(), QContactRelationshipFilter::First);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(availableRelationshipTypes.at(0), source.id(), QContactRelationshipFilter::Second);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(availableRelationshipTypes.at(0), source.id(), QContactRelationshipFilter::Either);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(availableRelationshipTypes.at(0), source.id());
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(availableRelationshipTypes.at(0));
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        //QSKIP("Skipping: This manager does not support relationships!", SkipSingle);
        return;
    }

    totalRelationships = cm->relationships().size();
    totalManagerRelationships = cm->relationships(QContactRelationship::IsManagerOf).size();

    QContactId dest1Uri = dest1.id();
    QContactId dest1EmptyUri;
    dest1EmptyUri.setManagerUri(QString());
    dest1EmptyUri.setLocalId(dest1.id().localId());
    QContactId dest2Uri = dest2.id();
    QContactId dest3Uri = dest3.id();
    QContactId dest3EmptyUri;
    dest3EmptyUri.setManagerUri(QString());
    dest3EmptyUri.setLocalId(dest3.id().localId());

    // build our relationship - source is the manager all of the dest contacts.
    QContactRelationship customRelationshipOne;
    customRelationshipOne.setFirst(source.id());
    customRelationshipOne.setSecond(dest1EmptyUri);
    customRelationshipOne.setRelationshipType(availableRelationshipTypes.at(0));
    QCOMPARE(customRelationshipOne.first(), source.id());
    QCOMPARE(customRelationshipOne.second(), dest1EmptyUri);
    QVERIFY(customRelationshipOne.relationshipType() == availableRelationshipTypes.at(0));

    // save the relationship
    QVERIFY(cm->saveRelationship(&customRelationshipOne));
    // test that the empty manager URI has been updated.
    QCOMPARE(customRelationshipOne.second(), dest1Uri); // updated with correct manager URI

    // test our accessors.
    QCOMPARE(cm->relationships(availableRelationshipTypes.at(0)).size(), (totalManagerRelationships + 1));
    QVERIFY(cm->relationships(availableRelationshipTypes.at(0), source.id()).size() == 1);

    // remove the dest1 contact, relationship shoudl be removed.
    cm->removeContact(dest1.localId());
    QCOMPARE(cm->relationships(availableRelationshipTypes.at(0), dest1Uri, QContactRelationshipFilter::Second).size(), 0);

    // modify and save the relationship
    customRelationshipOne.setSecond(dest2Uri);
    QVERIFY(cm->saveRelationship(&customRelationshipOne));

    // attempt to save the relationship again.  XXX TODO: what should the result be?  currently succeeds (overwrites)
    int relationshipCount = cm->relationships().count();
    QVERIFY(cm->saveRelationship(&customRelationshipOne));    // succeeds, but just overwrites
    QCOMPARE(relationshipCount, cm->relationships().count()); // shouldn't change; save should have overwritten.

    // removing the source contact should result in removal of the relationship.
    QCOMPARE(cm->relationships().size(), (totalRelationships + 1));
    QVERIFY(cm->removeContact(source.id().localId()));
    QCOMPARE(cm->relationships().size(), totalRelationships); // the relationship should have been removed.

    // now ensure that qcontact relationship caching works as required - perhaps this should be in tst_QContact?
    source.setId(QContactId());         // reset id so we can resave
    QVERIFY(cm->saveContact(&source));  // save source again.
    customRelationshipOne.setFirst(source.id());
    customRelationshipOne.setSecond(dest2.id());
    QVERIFY(cm->saveRelationship(&customRelationshipOne));

    QContactRelationship customRelationshipTwo;
    if (availableRelationshipTypes.size() > 1) {
        customRelationshipTwo.setFirst(source.id());
        customRelationshipTwo.setRelationshipType(availableRelationshipTypes.at(1));
        customRelationshipTwo.setSecond(dest3.id());
        QVERIFY(cm->saveRelationship(&customRelationshipTwo));
    }

    // currently, the contacts are "stale" - no cached relationships
    QVERIFY(dest3.relatedContacts().isEmpty());
    QVERIFY(dest3.relationships().isEmpty());
    QVERIFY(dest2.relatedContacts().isEmpty());
    QVERIFY(dest2.relationships().isEmpty());

    // now refresh the contacts
    dest3 = cm->contact(dest3.localId());
    dest2 = cm->contact(dest2.localId());
    source = cm->contact(source.localId());

    // and test again.
    QVERIFY(!dest3.relationships().contains(customRelationshipOne));
    if (availableRelationshipTypes.size() > 1) {
        QVERIFY(dest3.relatedContacts().contains(source.id()));
        QVERIFY(dest3.relationships().contains(customRelationshipTwo));
    }
    QVERIFY(dest2.relatedContacts().contains(source.id()));
    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0)).contains(source.id()));
    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0), QContactRelationshipFilter::First).contains(source.id()));
    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0), QContactRelationshipFilter::Second).isEmpty());
    QVERIFY(dest2.relationships().contains(customRelationshipOne));
    if (availableRelationshipTypes.size() > 1) {
        QVERIFY(!dest2.relationships().contains(customRelationshipTwo));
    }

    QVERIFY(!dest3.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipOne));
    if (availableRelationshipTypes.size() > 1) {
        QVERIFY(!dest3.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipTwo));
        QVERIFY(dest3.relationships(availableRelationshipTypes.at(1)).contains(customRelationshipTwo));
        QVERIFY(!dest3.relationships(availableRelationshipTypes.at(1)).contains(customRelationshipOne));
        QVERIFY(dest3.relatedContacts(availableRelationshipTypes.at(1)).contains(source.id()));
    }

    QVERIFY(dest2.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipOne));
    QVERIFY(!dest2.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipTwo));
    if (availableRelationshipTypes.size() > 1) {
        QVERIFY(dest2.relationships(availableRelationshipTypes.at(1)).isEmpty());
    }

    QVERIFY(!dest3.relatedContacts(availableRelationshipTypes.at(0)).contains(source.id()));
    if (availableRelationshipTypes.size() > 1) {
        QVERIFY(dest3.relatedContacts(availableRelationshipTypes.at(1)).contains(source.id())); // role = either
        QVERIFY(!dest3.relatedContacts(availableRelationshipTypes.at(1), QContactRelationshipFilter::Second).contains(source.id()));
        QVERIFY(dest3.relatedContacts(availableRelationshipTypes.at(1), QContactRelationshipFilter::First).contains(source.id()));
    }

    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0)).contains(source.id()));
    if (availableRelationshipTypes.size() > 1) {
        QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(1)).isEmpty());
    }
    QVERIFY(!dest2.relatedContacts(availableRelationshipTypes.at(0), QContactRelationshipFilter::Second).contains(source.id()));
    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0), QContactRelationshipFilter::First).contains(source.id()));

    QVERIFY(source.relatedContacts(QString(), QContactRelationshipFilter::First).isEmpty()); // source is always the first, so this should be empty.
    QVERIFY(source.relatedContacts(QString(), QContactRelationshipFilter::Second).contains(dest2.id()));
    QVERIFY(source.relatedContacts(QString(), QContactRelationshipFilter::Either).contains(dest2.id()));
    QVERIFY(source.relatedContacts(availableRelationshipTypes.at(0), QContactRelationshipFilter::Second).contains(dest2.id()));
    QVERIFY(source.relatedContacts(availableRelationshipTypes.at(0), QContactRelationshipFilter::First).isEmpty());

    // test arbitrary relationship types.
    if (cm->hasFeature(QContactManager::ArbitraryRelationshipTypes)) {
        customRelationshipOne.setFirst(source.id());
        customRelationshipOne.setSecond(dest3.id());
        customRelationshipOne.setRelationshipType("test-arbitrary-relationship-type");
        QVERIFY(cm->saveRelationship(&customRelationshipOne));
        dest3 = cm->contact(dest3.localId()); // refresh
        QVERIFY(dest3.relationships("test-arbitrary-relationship-type").size() == 1);
        QVERIFY(dest3.relationships("test-arbitrary-relationship-type").contains(customRelationshipOne));
    }

    // test batch API and ordering in contacts
    QList<QContactRelationship> currentRelationships = cm->relationships(source.id(), QContactRelationshipFilter::First);
    QList<QContactRelationship> batchList;
    QContactRelationship br1, br2, br3;
    br1.setFirst(source.id());
    br1.setSecond(dest2.id());
    br1.setRelationshipType(QContactRelationship::HasMember);
    br2.setFirst(source.id());
    br2.setSecond(dest3.id());
    br2.setRelationshipType(QContactRelationship::HasMember);
    br3.setFirst(source.id());
    br3.setSecond(dest3.id());
    br3.setRelationshipType(QContactRelationship::IsAssistantOf);
    batchList << br1 << br2 << br3;

    // ensure that the batch save works properly
    cm->saveRelationships(&batchList);
    QVERIFY(cm->error() == QContactManager::NoError);
    QList<QContactRelationship> batchRetrieve = cm->relationships(source.id(), QContactRelationshipFilter::First);
    QVERIFY(batchRetrieve.contains(br1));
    QVERIFY(batchRetrieve.contains(br2));
    QVERIFY(batchRetrieve.contains(br3));

    // test relationship ordering in the contact
    source = cm->contact(source.localId());
    QList<QContactRelationship> cachedRelationships = source.relationships();
    QList<QContactRelationship> orderedRelationships = source.relationshipOrder();
    QCOMPARE(cachedRelationships, orderedRelationships); // initially, should be the same
    QVERIFY(orderedRelationships.contains(br1));
    QVERIFY(orderedRelationships.contains(br2));
    QVERIFY(orderedRelationships.contains(br3));

    // ensure that the reordering works as required.
    QContactRelationship temp1 = orderedRelationships.takeAt(0); // now fiddle with the order
    QContactRelationship temp2 = orderedRelationships.at(0);     // this is the new first relationship
    orderedRelationships.insert(2, temp1);                       // and save the old first back at position 3.
    source.setRelationshipOrder(orderedRelationships);           // set the new relationship order
    cm->saveContact(&source);                                    // save the contact to persist the new order
    source = cm->contact(source.localId());                      // reload the contact
    QCOMPARE(source.relationshipOrder(), orderedRelationships);  // ensure that it was persisted.

    // now lets try a negative reordering test: adding relationships which don't exist in the database.
    QContactRelationship maliciousRel;
    maliciousRel.setFirst(source.id());
    maliciousRel.setSecond(dest2.id());
    maliciousRel.setRelationshipType("test-nokia-invalid-relationship-type");
    orderedRelationships << maliciousRel;
    source.setRelationshipOrder(orderedRelationships);
    QVERIFY(!cm->saveContact(&source));
    QVERIFY(cm->error() == QContactManager::InvalidRelationshipError);
    orderedRelationships.removeOne(br3);
    source.setRelationshipOrder(orderedRelationships);
    QVERIFY(!cm->saveContact(&source));
    QVERIFY(cm->error() == QContactManager::InvalidRelationshipError);
    source.setRelationshipOrder(QList<QContactRelationship>());
    QVERIFY(!cm->saveContact(&source));
    QVERIFY(cm->error() == QContactManager::InvalidRelationshipError);

    // remove a single relationship
    QVERIFY(cm->removeRelationship(br3));
    batchRetrieve = cm->relationships(source.id(), QContactRelationshipFilter::First);
    QVERIFY(batchRetrieve.contains(br1));
    QVERIFY(batchRetrieve.contains(br2));
    QVERIFY(!batchRetrieve.contains(br3)); // has already been removed.

    // now ensure that the batch remove works and we get returned to the original state.
    batchList.removeOne(br3);
    cm->removeRelationships(batchList);
    QVERIFY(cm->error() == QContactManager::NoError);
    QCOMPARE(cm->relationships(source.id(), QContactRelationshipFilter::First), currentRelationships);

    // attempt to save relationships between an existing source but non-existent destination
    QContactId nonexistentDest;
    quint32 idSeed = 0x5544;
    QContactLocalId nonexistentLocalId = QContactLocalId(idSeed);
    nonexistentDest.setManagerUri(cm->managerUri());
    while (true) {
        nonexistentLocalId = cm->contact(nonexistentLocalId).localId();
        if (nonexistentLocalId == QContactLocalId(0)) {
            // found a "spare" local id (no contact with that id)
            break;
        }

        // keep looking...
        idSeed += 1;
        nonexistentLocalId = QContactLocalId(idSeed);
        QVERIFY(nonexistentLocalId != QContactLocalId(0)); // integer overflow check.
    }
    nonexistentDest.setLocalId(nonexistentLocalId);
    maliciousRel.setFirst(source.id());
    maliciousRel.setSecond(nonexistentDest);
    maliciousRel.setRelationshipType("nokia-test-invalid-relationship-type");
    QVERIFY(!cm->saveRelationship(&maliciousRel));

    // attempt to save a circular relationship
    maliciousRel.setFirst(source.id());
    maliciousRel.setSecond(source.id());
    maliciousRel.setRelationshipType(availableRelationshipTypes.at(0));
    QVERIFY(!cm->saveRelationship(&maliciousRel));

    // more negative testing, but force manager to recognise the empty URI
    QContactId circularId = source.id();
    circularId.setManagerUri(QString());
    maliciousRel.setFirst(circularId);
    maliciousRel.setSecond(circularId);
    maliciousRel.setRelationshipType(availableRelationshipTypes.at(0));
    QVERIFY(!cm->saveRelationship(&maliciousRel));
    maliciousRel.setFirst(source.id());
    maliciousRel.setSecond(circularId);
    maliciousRel.setRelationshipType(availableRelationshipTypes.at(0));
    QVERIFY(!cm->saveRelationship(&maliciousRel));
    maliciousRel.setFirst(circularId);
    maliciousRel.setSecond(source.id());
    maliciousRel.setRelationshipType(availableRelationshipTypes.at(0));
    QVERIFY(!cm->saveRelationship(&maliciousRel));

    // attempt to save a relationship where the source contact comes from another manager
    circularId.setManagerUri("test-nokia-invalid-manager-uri");
    maliciousRel.setFirst(circularId);   // an invalid source contact
    maliciousRel.setSecond(dest2.id());       // a valid destination contact
    maliciousRel.setRelationshipType(availableRelationshipTypes.at(0));
    QVERIFY(!cm->saveRelationship(&maliciousRel));

    // remove the nonexistent relationship
    relationshipCount = cm->relationships().count();
    QVERIFY(!cm->removeRelationship(maliciousRel));         // does not exist; fail remove.
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    QCOMPARE(cm->relationships().count(), relationshipCount); // should be unchanged.

    // now clean up and remove our dests.
    QVERIFY(cm->removeContact(source.localId()));
    QVERIFY(cm->removeContact(dest2.localId()));
    QVERIFY(cm->removeContact(dest3.localId()));

    // attempt to save relationships with nonexistent contacts
    QVERIFY(!cm->saveRelationship(&br1));
    QVERIFY(cm->error() == QContactManager::InvalidRelationshipError);
    cm->saveRelationships(&batchList);
    QVERIFY(cm->error() == QContactManager::InvalidRelationshipError);
    QVERIFY(!cm->removeRelationship(br1));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    cm->removeRelationships(batchList);
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
}

void tst_QContactManager::contactType()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    if (!cm->hasFeature(QContactManager::Groups))
        QSKIP("Skipping: This manager does not support group contacts!", SkipSingle);

    QContact g1, g2, c;
    g1.setType(QContactType::TypeGroup);
    g2.setType(QContactType::TypeGroup);

    QContactPhoneNumber g1p, g2p, cp;
    g1p.setNumber("22222");
    g2p.setNumber("11111");
    cp.setNumber("33333");

    g1.saveDetail(&g1p);
    g2.saveDetail(&g2p);
    c.saveDetail(&cp);

    QVERIFY(cm->saveContact(&g1));
    QVERIFY(cm->saveContact(&g2));
    QVERIFY(cm->saveContact(&c));

    // test that the accessing by type works properly
    QContactDetailFilter groupFilter;
    groupFilter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    groupFilter.setValue(QString(QLatin1String(QContactType::TypeGroup)));
    QVERIFY(cm->contacts(groupFilter).contains(g1.localId()));
    QVERIFY(cm->contacts(groupFilter).contains(g2.localId()));
    QVERIFY(!cm->contacts(groupFilter).contains(c.localId()));

    QList<QContactSortOrder> sortOrders;
    QContactSortOrder byPhoneNumber;
    byPhoneNumber.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    sortOrders.append(byPhoneNumber);

    // and ensure that sorting works properly with typed contacts also
    QList<QContactLocalId> sortedIds = cm->contacts(groupFilter, sortOrders);
    QVERIFY(sortedIds.indexOf(g2.localId()) < sortedIds.indexOf(g1.localId()));

    cm->removeContact(g1.localId());
    cm->removeContact(g2.localId());
    cm->removeContact(c.localId());
}

QTEST_MAIN(tst_QContactManager)
#include "tst_qcontactmanager.moc"
