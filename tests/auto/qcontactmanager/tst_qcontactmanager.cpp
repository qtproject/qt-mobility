/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtGlobal>

#include "qtcontacts.h"
#include "qcontactchangeset.h"
#include "qcontactmanagerdataholder.h"

#if defined(USE_VERSIT_PLZ)
// This makes it easier to create specific QContacts
#include "qversitcontactimporter.h"
#include "qversitdocument.h"
#include "qversitreader.h"
#endif

QTM_USE_NAMESPACE
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

#define QCONTACTMANAGER_REMOVE_VERSIONS_FROM_URI(params)  params.remove(QString::fromAscii(QTCONTACTS_VERSION_NAME)); \
                                                          params.remove(QString::fromAscii(QTCONTACTS_IMPLEMENTATION_VERSION_NAME))

//TESTED_COMPONENT=src/contacts
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
Q_DECLARE_METATYPE(QContact)
Q_DECLARE_METATYPE(QContactManager::Error)

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
    QContact createContact(QContactDetailDefinition nameDef, QString firstName, QString lastName, QString phoneNumber);
    void saveContactName(QContact *contact, QContactDetailDefinition nameDef, QContactName *contactName, const QString &name) const;
    QString generateRandomString(int strlen, bool numeric = false) const;

    QScopedPointer<QContactManagerDataHolder> managerDataHolder;

public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:

    void doDump();
    void doDump_data() {addManagers();}

    void doDumpSchema();
    void doDumpSchema_data() {addManagers();}

    /* Special test with special data */
    void uriParsing();
    void nameSynthesis();
    void compatibleContact();

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

#if defined(USE_VERSIT_PLZ)
    void partialSave();
    void partialSave_data() {addManagers();}
#endif

    /* Tests that take no data */
    void contactValidation();
    void errorStayingPut();
    void ctors();
    void invalidManager();
    void memoryManager();
    void changeSet();
    void fetchHint();
    void benchmarking();

    /* Special test with special data */
    void uriParsing_data();
    void nameSynthesis_data();
    void compatibleContact_data();
    /* Tests that are run on all managers */
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
    void benchmarking_data() {addManagers();}
};

tst_QContactManager::tst_QContactManager()
{
}

tst_QContactManager::~tst_QContactManager()
{
}

void tst_QContactManager::initTestCase()
{
    managerDataHolder.reset(new QContactManagerDataHolder());

    /* Make sure these other test plugins are NOT loaded by default */
    // These are now removed from the list of managers in addManagers()
    //QVERIFY(!QContactManager::availableManagers().contains("testdummy"));
    //QVERIFY(!QContactManager::availableManagers().contains("teststaticdummy"));
    //QVERIFY(!QContactManager::availableManagers().contains("maliciousplugin"));
}

void tst_QContactManager::cleanupTestCase()
{
    managerDataHolder.reset(0);
}

void tst_QContactManager::dumpContactDifferences(const QContact& ca, const QContact& cb)
{
    // Try to narrow down the differences
    QContact a(ca);
    QContact b(cb);

    QContactName n1 = a.detail(QContactName::DefinitionName);
    QContactName n2 = b.detail(QContactName::DefinitionName);

    // Check the name components in more detail
    QCOMPARE(n1.firstName(), n2.firstName());
    QCOMPARE(n1.middleName(), n2.middleName());
    QCOMPARE(n1.lastName(), n2.lastName());
    QCOMPARE(n1.prefix(), n2.prefix());
    QCOMPARE(n1.suffix(), n2.suffix());
    QCOMPARE(n1.customLabel(), n2.customLabel());

    // Check the display label
    QCOMPARE(a.displayLabel(), b.displayLabel());

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
            qDebug() << "A contact had extra detail:" << d.definitionName() << d.variantValues();
    }
    // and same for B
    foreach(QContactDetail d, bDetails) {
        if (d.definitionName() != QContactDisplayLabel::DefinitionName && d.definitionName() != QContactType::DefinitionName)
            qDebug() << "B contact had extra detail:" << d.definitionName() << d.variantValues();
    }

    // now test specifically the display label and the type
    if (a.displayLabel() != b.displayLabel()) {
        qDebug() << "A contact display label =" << a.displayLabel();
        qDebug() << "B contact display label =" << b.displayLabel();
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

    // Second remove any superset matches (eg, backend adds a field)
    aDetails = a.details();
    bDetails = b.details();
    foreach (QContactDetail d, aDetails) {
        foreach (QContactDetail d2, bDetails) {
            if (d.definitionName() == d2.definitionName()) {
                bool canRemove = true;
                QMap<QString, QVariant> d2map = d2.variantValues();
                foreach (QString key, d2map.keys()) {
                    if (d.value(key) != d2.value(key)) {
                        // d can have _more_ keys than d2,
                        // but not _less_; and it cannot
                        // change the value.
                        canRemove = false;
                    }
                }

                if (canRemove) {
                    // if we get to here, we can remove the details.
                    a.removeDetail(&d);
                    b.removeDetail(&d2);
                    break;
                }
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
    qDebug() << "Contact: " << contact.id().localId() << "(" << m.synthesizedContactDisplayLabel(contact) << ")";
    QList<QContactDetail> details = contact.details();
    foreach(QContactDetail d, details) {
        qDebug() << "  " << d.definitionName() << ":";
        qDebug() << "    Vals:" << d.variantValues();
    }
}

void tst_QContactManager::dumpContacts(QContactManager *cm)
{
    QList<QContactLocalId> ids = cm->contactIds();

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
    managers.removeAll("testdummy");
    managers.removeAll("teststaticdummy");
    managers.removeAll("maliciousplugin");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        if (mgr == "memory") {
            params.insert("id", "tst_QContactManager");
            QTest::newRow(QString("mgr='%1', params").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        }
    }
}

/*
 * Helper method for creating a QContact instance with name and phone number
 * details. Name is generated according to the detail definition assuming that
 * either first and last name or custom label is supported.
 */
QContact tst_QContactManager::createContact(
    QContactDetailDefinition nameDef,
    QString firstName,
    QString lastName,
    QString phoneNumber)
{
    QContact contact;

    if(!firstName.isEmpty() || !lastName.isEmpty()) {
        QContactName n;

        if(nameDef.fields().contains(QContactName::FieldFirstName)
            && nameDef.fields().contains(QContactName::FieldFirstName)) {
            n.setFirstName(firstName);
            n.setLastName(lastName);
        } else if(nameDef.fields().contains(QContactName::FieldCustomLabel)) {
            n.setCustomLabel(firstName + " " + lastName);
        } else {
            // assume that either first and last name or custom label is supported
            QTest::qWarn("Neither custom label nor first name/last name supported!");
            return QContact();
        }
        contact.saveDetail(&n);
    }

    if (!phoneNumber.isEmpty()) {
        QContactPhoneNumber ph;
        ph.setNumber(phoneNumber);
        contact.saveDetail(&ph);
    }

    return contact;
}

void tst_QContactManager::saveContactName(QContact *contact, QContactDetailDefinition nameDef, QContactName *contactName, const QString &name) const
{
    // check which name fields are supported in the following order:
    // 1. custom label, 2. first name, 3. last name
    if(nameDef.fields().contains(QContactName::FieldCustomLabel)) {
        contactName->setCustomLabel(name);
    } else if(nameDef.fields().contains(QContactName::FieldFirstName)) {
        contactName->setFirstName(name);
    } else if(nameDef.fields().contains(QContactName::FieldLastName)) {
        contactName->setLastName(name);
    } else {
        // Assume that at least one of the above name fields is supported by the backend
        QVERIFY(false);
    }
    contact->saveDetail(contactName);
}

void tst_QContactManager::metadata()
{
    // ensure that the backend is publishing its metadata (name / parameters / uri) correctly
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(new QContactManager("memory"));
    QVERIFY(QContactManager::buildUri(cm->managerName(), cm->managerParameters()) == cm->managerUri());
}


void tst_QContactManager::nullIdOperations()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(new QContactManager("memory"));
    QVERIFY(!cm->removeContact(QContactLocalId()));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);


    QContact c = cm->contact(QContactLocalId());
    QVERIFY(c.id() == QContactId());
    QVERIFY(c.isEmpty());
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
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
        QVERIFY(QContactManager::parseUri(uri, 0, 0)); // no out parms

        // 1 out param
        QVERIFY(QContactManager::parseUri(uri, &outmanager, 0));
        QCOMPARE(manager, outmanager);
        QVERIFY(QContactManager::parseUri(uri, 0, &outparameters));

        QCONTACTMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);

        QCOMPARE(parameters, outparameters);

        outmanager.clear();
        outparameters.clear();
        QVERIFY(QContactManager::parseUri(uri, &outmanager, &outparameters));

        QCONTACTMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);

        QCOMPARE(manager, outmanager);
        QCOMPARE(parameters, outparameters);
    } else {
        /* bad splitting */
        outmanager.clear();
        outparameters.clear();
        QVERIFY(QContactManager::parseUri(uri, 0, 0) == false);
        QVERIFY(QContactManager::parseUri(uri, &outmanager, 0) == false);
        QVERIFY(outmanager.isEmpty());
        QVERIFY(QContactManager::parseUri(uri, 0, &outparameters) == false);
        QCONTACTMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);
        QVERIFY(outparameters.isEmpty());

        /* make sure the in parameters don't change with a bad split */
        outmanager = manager;
        outparameters = parameters;
        QVERIFY(QContactManager::parseUri(uri, &outmanager, 0) == false);
        QCOMPARE(manager, outmanager);
        QVERIFY(QContactManager::parseUri(uri, 0, &outparameters) == false);
        QCONTACTMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);
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

    QScopedPointer<QContactManager> cm5(QContactManager::fromUri(QContactManager::buildUri(defaultStore, QMap<QString, QString>())));
    QScopedPointer<QContactManager> cm6(QContactManager::fromUri(cm.managerUri())); // uri is not a name; should fail.
    QScopedPointer<QContactManager> cm9(QContactManager::fromUri(QString(), &parent));

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
    QScopedPointer<QContactManager> em4(QContactManager::fromUri("invalid uri"));
    QScopedPointer<QContactManager> em5(QContactManager::fromUri(QContactManager::buildUri("nonexistent", QMap<QString, QString>())));
    QScopedPointer<QContactManager> em6(QContactManager::fromUri(em3.managerUri()));


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


    // Finally test the platform specific engines are actually the defaults
#if defined(Q_OS_SYMBIAN)
    QCOMPARE(defaultStore, QString("symbian"));
#elif defined(Q_WS_MAEMO_6)
    QCOMPARE(defaultStore, QString("tracker"));
#elif defined(Q_WS_MAEMO_5)
    QCOMPARE(defaultStore, QString("maemo5"));
#elif defined(Q_OS_WINCE)
    QCOMPARE(defaultStore, QString("wince"));
#else
    QCOMPARE(defaultStore, QString("memory"));
#endif
}

void tst_QContactManager::doDump()
{
    // Only do this if it has been explicitly selected
    if (QCoreApplication::arguments().contains("doDump")) {
        QFETCH(QString, uri);
        QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

        dumpContacts(cm.data());
    }
}

Q_DECLARE_METATYPE(QVariant)

void tst_QContactManager::doDumpSchema()
{
    // Only do this if it has been explicitly selected
    if (QCoreApplication::arguments().contains("doDumpSchema")) {
        QFETCH(QString, uri);
        QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

        // Get the schema for each supported type
        foreach(QString type, cm->supportedContactTypes()) {
            QMap<QString, QContactDetailDefinition> defs = cm->detailDefinitions(type);

            foreach(QContactDetailDefinition def, defs.values()) {
                if (def.isUnique())
                    qDebug() << QString("%2::%1 (Unique) {").arg(def.name()).arg(type).toAscii().constData();
                else
                    qDebug() << QString("%2::%1 {").arg(def.name()).arg(type).toAscii().constData();
                QMap<QString, QContactDetailFieldDefinition> fields = def.fields();

                foreach(QString fname, fields.keys()) {
                    QContactDetailFieldDefinition field = fields.value(fname);

                    if (field.allowableValues().count() > 0) {
                        // Make some pretty output
                        QStringList allowedList;
                        foreach(QVariant var, field.allowableValues()) {
                            QString allowed;
                            if (var.type() == QVariant::String)
                                allowed = QString("'%1'").arg(var.toString());
                            else if (var.type() == QVariant::StringList)
                                allowed = QString("'%1'").arg(var.toStringList().join(","));
                            else {
                                // use the textstream <<
                                QDebug dbg(&allowed);
                                dbg << var;
                            }
                            allowedList.append(allowed);
                        }

                        qDebug() << QString("   %2 %1 {%3}").arg(fname).arg(QMetaType::typeName(field.dataType())).arg(allowedList.join(",")).toAscii().constData();
                    } else
                        qDebug() << QString("   %2 %1").arg(fname).arg(QMetaType::typeName(field.dataType())).toAscii().constData();
                }

                qDebug() << "}";
            }
        }
    }
}

void tst_QContactManager::add()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    QContactDetailDefinition nameDef = cm->detailDefinition(QContactName::DefinitionName, QContactType::TypeContact);
    QContact alice = createContact(nameDef, "Alice", "inWonderland", "1234567");
    int currCount = cm->contactIds().count();
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);

    QVERIFY(!alice.id().managerUri().isEmpty());
    QVERIFY(alice.id().localId() != 0);
    QCOMPARE(cm->contactIds().count(), currCount+1);

    QContact added = cm->contact(alice.id().localId());
    QVERIFY(added.id() == alice.id());
    
    if (!isSuperset(added, alice)) {
        dumpContacts(cm.data());
        dumpContactDifferences(added, alice);
        QCOMPARE(added, alice);
    }

    // now try adding a contact that does not exist in the database with non-zero id
    if (cm->managerName() == "symbiansim") {
        // TODO: symbiansim backend fails this test currently. Will be fixed later.
        QWARN("This manager has a known issue with saving a non-zero id contact. Skipping this test step.");
    } else {
        QContact nonexistent = createContact(nameDef, "nonexistent", "contact", "");
        QVERIFY(cm->saveContact(&nonexistent));       // should work
        QVERIFY(cm->removeContact(nonexistent.id().localId())); // now nonexistent has an id which does not exist
        QVERIFY(!cm->saveContact(&nonexistent));      // hence, should fail
        QCOMPARE(cm->error(), QContactManager::DoesNotExistError);
        nonexistent.setId(QContactId());
        QVERIFY(cm->saveContact(&nonexistent));       // after setting id to zero, should save
        QVERIFY(cm->removeContact(nonexistent.id().localId()));
    }

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

        // Leave these warnings here - might need an API for this
        // XXX FIXME: access constraint reporting as moved to the detail itself
        //if (def.accessConstraint() == QContactDetailDefinition::ReadOnly) {
        //    continue;
        //}

        if (cm->managerName() == "maemo5") {
            // The maemo5 backend only supports reading of Guid and QCOA
            if (def.name() == QContactGuid::DefinitionName)
                continue;
            if (def.name() == QContactOnlineAccount::DefinitionName)
                continue;
	    if (def.name() == QContactPresence::DefinitionName)
                continue;
        }

        // This is probably read-only
        if (def.name() == QContactTimestamp::DefinitionName)
            continue;

        // otherwise, create a new detail of the given type and save it to the contact
        QContactDetail det(def.name());
        QMap<QString, QContactDetailFieldDefinition> fieldmap = def.fields();
        QStringList fieldKeys = fieldmap.keys();
        foreach (const QString& fieldKey, fieldKeys) {
            // get the field, and check to see that it's not constrained.
            QContactDetailFieldDefinition currentField = fieldmap.value(fieldKey);
            
            // Don't test detail uris as these are manager specific
            if (fieldKey == QContactDetail::FieldDetailUri)
                continue;

            // Special case: phone number.
            if (def.name() == QContactPhoneNumber::DefinitionName &&
                fieldKey == QContactPhoneNumber::FieldNumber) {
                det.setValue(fieldKey, "+3581234567890");
                continue;
            }

            // Attempt to create a worthy value
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
        if (!det.isEmpty())
            megacontact.saveDetail(&det);
    }

    QVERIFY(cm->saveContact(&megacontact)); // must be able to save since built from definitions.
    QContact retrievedMegacontact = cm->contact(megacontact.id().localId());
    if (!isSuperset(retrievedMegacontact, megacontact)) {
        dumpContactDifferences(megacontact, retrievedMegacontact);
        QEXPECT_FAIL("mgr='wince'", "Address Display Label mismatch", Continue);
        QCOMPARE(megacontact, retrievedMegacontact);
    }

    // now a contact with many details of a particular definition
    // if the detail is not unique it should then support minumum of two of the same kind
    const int nrOfdetails = 2;
    QContact veryContactable = createContact(nameDef, "Very", "Contactable", "");
    for (int i = 0; i < nrOfdetails; i++) {
        QString phnStr = QString::number(i);
        QContactPhoneNumber vcphn;
        vcphn.setNumber(phnStr);
        QVERIFY(veryContactable.saveDetail(&vcphn));
    }

    // check that all the numbers were added successfully
    QVERIFY(veryContactable.details(QContactPhoneNumber::DefinitionName).size() == nrOfdetails);
    
    // check if it can be saved
    QContactDetailDefinition def = cm->detailDefinition(QContactPhoneNumber::DefinitionName);
    if (def.isUnique()) {    
        QVERIFY(!cm->saveContact(&veryContactable));
    }
    else {
        QVERIFY(cm->saveContact(&veryContactable));
        
        // verify save
        QContact retrievedContactable = cm->contact(veryContactable.id().localId());
        if (!isSuperset(retrievedContactable, veryContactable)) {
            dumpContactDifferences(veryContactable, retrievedContactable);
            QEXPECT_FAIL("mgr='wince'", "Number of phones supported mismatch", Continue);
            QCOMPARE(veryContactable, retrievedContactable);
        }
    }
}

void tst_QContactManager::update()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->managerName() == QString(QLatin1String("maemo5"))) {
        // we specifically want to test the update semantics of the maemo5 backend
        // since there are various complexities relating to roster contacts.
        QContact mt;
        QContactName mtn;
        mtn.setFirstName("test");
        mtn.setLastName("maemo");
        QContactPhoneNumber pn;
        pn.setNumber("12345");

        mt.saveDetail(&mtn);
        cm->saveContact(&mt);
        mt = cm->contact(mt.localId()); // force reload of (persisted) contact
        QVERIFY(mt.details<QContactPhoneNumber>().count() == 0);

        // now save a single phonenumber
        mt.saveDetail(&pn);
        cm->saveContact(&mt);
        mt = cm->contact(mt.localId()); // force reload of (persisted) contact
        QVERIFY(mt.details<QContactPhoneNumber>().count() == 1);

        // edit some other existing detail and save (shouldn't duplicate the phone number)
        mtn.setMiddleName("middle");
        mt.saveDetail(&mtn);
        cm->saveContact(&mt);
        mt = cm->contact(mt.localId()); // force reload of (persisted) contact
        QCOMPARE(mt.details<QContactPhoneNumber>().count(), 1);

        // add some other detail and save (shouldn't duplicate the phone number)
        QContactEmailAddress mte;
        mte.setEmailAddress("test@test.com");
        mt.saveDetail(&mte);
        cm->saveContact(&mt);
        mt = cm->contact(mt.localId()); // force reload of (persisted) contact
        QCOMPARE(mt.details<QContactPhoneNumber>().count(), 1);

        // add another phone number detail and save (should create a single other phone number)
        QContactPhoneNumber pn2;
        pn2.setNumber("98765");
        mt.saveDetail(&pn2);
        cm->saveContact(&mt);
        mt = cm->contact(mt.localId()); // force reload of (persisted) contact
        QCOMPARE(mt.details<QContactPhoneNumber>().count(), 2);

        // here we do something tricky: we save one of the previously saved phone numbers
        // in a _different_ contact, and see if that causes problems with the overwrite vs new detail code.
        QContactPhoneNumber pn2Copy = pn2;
        QContact mt2;
        QContactName mt2n;
        mt2n.setFirstName("test2");
        mt2.saveDetail(&mt2n);
        QContactPhoneNumber shouldBeNew = pn;
        mt2.saveDetail(&shouldBeNew);
        QVERIFY(cm->saveContact(&mt2));
        mt2 = cm->contact(mt2.localId());
        QCOMPARE(mt2.details<QContactPhoneNumber>().count(), 1);
        mt2.saveDetail(&pn2);
        QVERIFY(cm->saveContact(&mt2));
        mt2 = cm->contact(mt2.localId());
        QCOMPARE(mt2.details<QContactPhoneNumber>().count(), 2);
        pn2 = pn2Copy; // reset just in case backend added some fields.

        // remove the other phone number detail, shouldn't cause side effects to the first...
        // NOTE: we need to reload the details before attempting to remove/edit them
        // because the backend can change the ids.
        QList<QContactPhoneNumber> pnums = mt.details<QContactPhoneNumber>();
        foreach (const QContactPhoneNumber& pd, pnums) {
            if (pd.number() == pn2.number())
                pn2 = pd;
            else if (pd.number() == pn.number())
                pn = pd;
        }
        mt.removeDetail(&pn2);
        cm->saveContact(&mt);
        mt = cm->contact(mt.localId()); // force reload of (persisted) contact
        QCOMPARE(mt.details<QContactPhoneNumber>().count(), 1);

        // edit the original phone number detail, shouldn't duplicate the phone number
        // NOTE: we need to reload the details before attempting to remove/edit them
        // because the backend can change the ids.
        pnums = mt.details<QContactPhoneNumber>();
        foreach (const QContactPhoneNumber& pd, pnums) {
            if (pd.number() == pn2.number())
                pn2 = pd;
            else if (pd.number() == pn.number())
                pn = pd;
        }
        pn.setNumber("54321");
        mt.saveDetail(&pn);
        cm->saveContact(&mt);
        mt = cm->contact(mt.localId());
        QCOMPARE(mt.details<QContactPhoneNumber>().count(), 1);
        QVERIFY(mt.detail<QContactPhoneNumber>() == pn);

        // we also should do the same test for other details (for example, gender).
        // if the backend cannot save multiple copies of a detail (eg, gender always overwrites)
        // it should FAIL the save operation if the contact has multiple of that detail type,
        // and set error to QContactManager::LimitReachedError.
        QContactGender mtg, mtg2;
        mtg.setGender(QContactGender::GenderFemale);
        mtg2.setGender(QContactGender::GenderMale);
        mt.saveDetail(&mtg);
        QVERIFY(cm->saveContact(&mt)); // one gender is fine
        mt.saveDetail(&mtg2);
        QVERIFY(!cm->saveContact(&mt)); // two is not
        //QCOMPARE(cm->error(), QContactManager::LimitReachedError); // should be LimitReachedError.
        mt = cm->contact(mt.localId());
        QVERIFY(mt.details<QContactGender>().count() == 1);
    }

    /* Save a new contact first */
    int contactCount = cm->contacts().size();
    QContactDetailDefinition nameDef = cm->detailDefinition(QContactName::DefinitionName, QContactType::TypeContact);
    QContact alice = createContact(nameDef, "Alice", "inWonderland", "1234567");
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);
    contactCount += 1; // added a new contact.
    QCOMPARE(cm->contacts().size(), contactCount);

    /* Update name */
    QContactName name = alice.detail(QContactName::DefinitionName);
    saveContactName(&alice, nameDef, &name, "updated");
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);
    saveContactName(&alice, nameDef, &name, "updated2");
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);
    alice = cm->contact(alice.localId()); // force reload of (persisted) alice
    QContact updated = cm->contact(alice.localId());
    QContactName updatedName = updated.detail(QContactName::DefinitionName);
    QCOMPARE(updatedName, name);
    QCOMPARE(cm->contacts().size(), contactCount); // contact count should be the same, no new contacts

    /* Test that adding a new detail doesn't cause unwanted side effects */
    int detailCount = alice.details().size();
    QContactEmailAddress email;
    email.setEmailAddress("test@example.com");
    alice.saveDetail(&email);
    QVERIFY(cm->saveContact(&alice));
    QCOMPARE(cm->contacts().size(), contactCount); // contact count shoudl be the same, no new contacts

    // This test is dangerous, since backends can add timestamps etc...
    detailCount += 1;
    QCOMPARE(detailCount, alice.details().size()); // adding a detail should cause the detail count to increase by one.

    /* Test that removal of fields in a detail works */
    QContactPhoneNumber phn = alice.detail<QContactPhoneNumber>();
    phn.setNumber("1234567");
    phn.setContexts(QContactDetail::ContextHome);
    alice.saveDetail(&phn);
    QVERIFY(cm->saveContact(&alice));
    alice = cm->contact(alice.localId()); // force reload of (persisted) alice
    QVERIFY(alice.detail<QContactPhoneNumber>().contexts().contains(QContactDetail::ContextHome)); // check context saved.
    phn = alice.detail<QContactPhoneNumber>(); // reload the detail, since it's key could have changed
    phn.setContexts(QStringList()); // remove context field.
    alice.saveDetail(&phn);
    QVERIFY(cm->saveContact(&alice));
    alice = cm->contact(alice.localId()); // force reload of (persisted) alice
    QVERIFY(alice.detail<QContactPhoneNumber>().contexts().isEmpty()); // check context removed.
    QCOMPARE(cm->contacts().size(), contactCount); // removal of a field of a detail shouldn't affect the contact count

    // This test is dangerous, since backends can add timestamps etc...
    QCOMPARE(detailCount, alice.details().size()); // removing a field from a detail should affect the detail count

    /* Test that removal of details works */
    phn = alice.detail<QContactPhoneNumber>(); // reload the detail, since it's key could have changed
    alice.removeDetail(&phn);
    QVERIFY(cm->saveContact(&alice));
    alice = cm->contact(alice.localId()); // force reload of (persisted) alice
    QVERIFY(alice.details<QContactPhoneNumber>().isEmpty()); // no such detail.
    QCOMPARE(cm->contacts().size(), contactCount); // removal of a detail shouldn't affect the contact count

    // This test is dangerous, since backends can add timestamps etc...
    //detailCount -= 1;
    //QCOMPARE(detailCount, alice.details().size()); // removing a detail should cause the detail count to decrease by one.

    if (cm->hasFeature(QContactManager::Groups)) {
        // Try changing types - not allowed
        // from contact -> group
        alice.setType(QContactType::TypeGroup);
        QContactName na = alice.detail(QContactName::DefinitionName);
        alice.removeDetail(&na);
        QVERIFY(!cm->saveContact(&alice));
        QVERIFY(cm->error() == QContactManager::AlreadyExistsError);

        // from group -> contact
        QContact jabberwock = createContact(nameDef, "", "", "1234567890");
        jabberwock.setType(QContactType::TypeGroup);
        QVERIFY(cm->saveContact(&jabberwock));
        jabberwock.setType(QContactType::TypeContact);
        QVERIFY(!cm->saveContact(&jabberwock));
        QVERIFY(cm->error() == QContactManager::AlreadyExistsError);
    }
}

void tst_QContactManager::remove()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    /* Save a new contact first */
    QContactDetailDefinition nameDef = cm->detailDefinition(QContactName::DefinitionName, QContactType::TypeContact);
    QContact alice = createContact(nameDef, "Alice", "inWonderland", "1234567");
    QVERIFY(cm->saveContact(&alice));
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(alice.id() != QContactId());

    /* Remove the created contact */
    const int contactCount = cm->contactIds().count();
    QVERIFY(cm->removeContact(alice.localId()));
    QCOMPARE(cm->contactIds().count(), contactCount - 1);
    QVERIFY(cm->contact(alice.localId()).isEmpty());
    QCOMPARE(cm->error(), QContactManager::DoesNotExistError);
}

void tst_QContactManager::batch()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    /* First test null pointer operations */
    QVERIFY(!cm->saveContacts(NULL, NULL));
    QVERIFY(cm->error() == QContactManager::BadArgumentError);

    QVERIFY(!cm->removeContacts(QList<QContactLocalId>(), NULL));
    QVERIFY(cm->error() == QContactManager::BadArgumentError);
    
    // Get supported name field
    QString nameField = QContactName::FieldFirstName;
    QContactDetailDefinition def = cm->detailDefinition(QContactName::DefinitionName);
    if (!def.fields().contains(QContactName::FieldFirstName)) {
        if(def.fields().contains(QContactName::FieldCustomLabel))
            nameField = QLatin1String(QContactName::FieldCustomLabel);
        else
            QSKIP("This backend does not support the required name field!", SkipSingle);
    }

    /* Now add 3 contacts, all valid */
    QContact a;
    QContactName na;
    na.setValue(nameField, "XXXXXX Albert");
    a.saveDetail(&na);

    QContact b;
    QContactName nb;
    nb.setValue(nameField, "XXXXXX Bob");
    b.saveDetail(&nb);

    QContact c;
    QContactName nc;
    nc.setValue(nameField, "XXXXXX Carol");
    c.saveDetail(&nc);

    QList<QContact> contacts;
    contacts << a << b << c;

    QMap<int, QContactManager::Error> errorMap;
    // Add one dummy error to test if the errors are reset
    errorMap.insert(0, QContactManager::NoError);
    QVERIFY(cm->saveContacts(&contacts, &errorMap));
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errorMap.count() == 0);

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

    /* Save again, with a null error map */
    QVERIFY(cm->saveContacts(&contacts, NULL));
    QVERIFY(cm->error() == QContactManager::NoError);

    /* Now make an update to them all */
    QContactPhoneNumber number;
    number.setNumber("1234567");

    QVERIFY(contacts[0].saveDetail(&number));
    number.setNumber("234567");
    QVERIFY(contacts[1].saveDetail(&number));
    number.setNumber("34567");
    QVERIFY(contacts[2].saveDetail(&number));

    QVERIFY(cm->saveContacts(&contacts, &errorMap));
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errorMap.count() == 0);

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

    /* Retrieve them with the batch ID fetch API */
    QList<QContactLocalId> batchIds;
    batchIds << a.localId() << b.localId() << c.localId();

    // Null error map first (doesn't crash)
    QList<QContact> batchFetch = cm->contacts(batchIds, 0, QContactFetchHint());
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(batchFetch.count() == 3);
    QVERIFY(batchFetch.at(0).detail(QContactName::DefinitionName) == na);
    QVERIFY(batchFetch.at(1).detail(QContactName::DefinitionName) == nb);
    QVERIFY(batchFetch.at(2).detail(QContactName::DefinitionName) == nc);

    // With error map
    batchFetch = cm->contacts(batchIds, &errorMap, QContactFetchHint());
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errorMap.count() == 0);
    QVERIFY(batchFetch.count() == 3);
    QVERIFY(batchFetch.at(0).detail(QContactName::DefinitionName) == na);
    QVERIFY(batchFetch.at(1).detail(QContactName::DefinitionName) == nb);
    QVERIFY(batchFetch.at(2).detail(QContactName::DefinitionName) == nc);

    /* Now an empty id */
    batchIds.clear();
    batchIds << QContactLocalId() << a.localId() << b.localId() << c.localId();
    batchFetch = cm->contacts(batchIds, 0, QContactFetchHint());
    QVERIFY(cm->error() != QContactManager::NoError);
    QVERIFY(batchFetch.count() == 4);
    QVERIFY(batchFetch.at(0).detail(QContactName::DefinitionName) == QContactDetail());
    QVERIFY(batchFetch.at(1).detail(QContactName::DefinitionName) == na);
    QVERIFY(batchFetch.at(2).detail(QContactName::DefinitionName) == nb);
    QVERIFY(batchFetch.at(3).detail(QContactName::DefinitionName) == nc);

    batchFetch = cm->contacts(batchIds, &errorMap, QContactFetchHint());
    QVERIFY(cm->error() != QContactManager::NoError);
    QVERIFY(batchFetch.count() == 4);
    QVERIFY(errorMap.count() == 1);
    QVERIFY(errorMap[0] == QContactManager::DoesNotExistError);
    QVERIFY(batchFetch.at(0).detail(QContactName::DefinitionName) == QContactDetail());
    QVERIFY(batchFetch.at(1).detail(QContactName::DefinitionName) == na);
    QVERIFY(batchFetch.at(2).detail(QContactName::DefinitionName) == nb);
    QVERIFY(batchFetch.at(3).detail(QContactName::DefinitionName) == nc);

    /* Now multiple of the same contact */
    batchIds.clear();
    batchIds << c.localId() << b.localId() << c.localId() << a.localId() << a.localId();
    batchFetch = cm->contacts(batchIds, &errorMap, QContactFetchHint());
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(batchFetch.count() == 5);
    QVERIFY(errorMap.count() == 0);
    QVERIFY(batchFetch.at(0).detail(QContactName::DefinitionName) == nc);
    QVERIFY(batchFetch.at(1).detail(QContactName::DefinitionName) == nb);
    QVERIFY(batchFetch.at(2).detail(QContactName::DefinitionName) == nc);
    QVERIFY(batchFetch.at(3).detail(QContactName::DefinitionName) == na);
    QVERIFY(batchFetch.at(4).detail(QContactName::DefinitionName) == na);

    /* Now delete them all */
    QList<QContactLocalId> ids;
    ids << a.id().localId() << b.id().localId() << c.id().localId();
    QVERIFY(cm->removeContacts(ids, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(cm->error() == QContactManager::NoError);

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
    QVERIFY(!cm->removeContacts(ids, &errorMap));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);
    QVERIFY(errorMap.count() == 3);
    QVERIFY(errorMap.values().at(0) == QContactManager::DoesNotExistError);
    QVERIFY(errorMap.values().at(1) == QContactManager::DoesNotExistError);
    QVERIFY(errorMap.values().at(2) == QContactManager::DoesNotExistError);

    /* And again with a null error map */
    QVERIFY(!cm->removeContacts(ids, NULL));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError);

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
    QVERIFY(!cm->saveContacts(&contacts, &errorMap));
    /* We can't really say what the error will be.. maybe bad argument, maybe invalid detail */
    QVERIFY(cm->error() != QContactManager::NoError);

    /* It's permissible to fail all the adds, or to add the successful ones */
    QVERIFY(errorMap.count() > 0);
    QVERIFY(errorMap.count() <= 3);

    // A might have gone through
    if (errorMap.keys().contains(0)) {
        QVERIFY(errorMap.value(0) != QContactManager::NoError);
        QVERIFY(contacts.at(0).id() == QContactId());
    } else {
        QVERIFY(contacts.at(0).id() != QContactId());
    }

    // B should have failed
    QVERIFY(errorMap.value(1) == QContactManager::InvalidDetailError);
    QVERIFY(contacts.at(1).id() == QContactId());

    // C might have gone through
    if (errorMap.keys().contains(2)) {
        QVERIFY(errorMap.value(2) != QContactManager::NoError);
        QVERIFY(contacts.at(2).id() == QContactId());
    } else {
        QVERIFY(contacts.at(2).id() != QContactId());
    }

    /* Fix up B and re save it */
    QVERIFY(contacts[1].removeDetail(&bad));
    QVERIFY(cm->saveContacts(&contacts, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(cm->error() == QContactManager::NoError);
    
    // Save and remove a fourth contact. Store the id.
    a.setId(QContactId());
    QVERIFY(cm->saveContact(&a));
    QContactLocalId removedId = a.localId();
    QVERIFY(cm->removeContact(removedId));

    /* Now delete 3 items, but with one bad argument */
    ids.clear();
    ids << contacts.at(0).id().localId();
    ids << removedId;
    ids << contacts.at(2).id().localId();

    QVERIFY(!cm->removeContacts(ids, &errorMap));
    QVERIFY(cm->error() != QContactManager::NoError);

    /* Again, the backend has the choice of either removing the successful ones, or not */
    QVERIFY(errorMap.count() > 0);
    QVERIFY(errorMap.count() <= 3);

    // A might have gone through
    if (errorMap.keys().contains(0)) {
        QVERIFY(errorMap.value(0) != QContactManager::NoError);
        QVERIFY(contacts.at(0).id() == QContactId());
    } else {
        QVERIFY(contacts.at(0).id() != QContactId());
    }

    /* B should definitely have failed */
    QVERIFY(errorMap.value(1) == QContactManager::DoesNotExistError);
    QVERIFY(ids.at(1) == removedId);

    // A might have gone through
    if (errorMap.keys().contains(2)) {
        QVERIFY(errorMap.value(2) != QContactManager::NoError);
        QVERIFY(contacts.at(2).id() == QContactId());
    } else {
        QVERIFY(contacts.at(2).id() != QContactId());
    }
}

void tst_QContactManager::invalidManager()
{
    /* Create an invalid manager */
    QContactManager manager("this should never work");
    QVERIFY(manager.managerName() == "invalid");
    QVERIFY(manager.managerVersion() == 0);

    /* also, test the other ctor behaviour is sane also */
    QContactManager anotherManager("this should never work", 15);
    QVERIFY(anotherManager.managerName() == "invalid");
    QVERIFY(anotherManager.managerVersion() == 0);

    /* Now test that all the operations fail */
    QVERIFY(manager.contactIds().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QContact foo;
    QContactName nf;
    nf.setLastName("Lastname");
    foo.saveDetail(&nf);

    QVERIFY(manager.synthesizedContactDisplayLabel(foo).isEmpty());
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.saveContact(&foo) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(foo.id() == QContactId());
    QVERIFY(manager.contactIds().count() == 0);

    QVERIFY(manager.contact(foo.id().localId()).id() == QContactId());
    QVERIFY(manager.contact(foo.id().localId()).isEmpty());
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.removeContact(foo.id().localId()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QMap<int, QContactManager::Error> errorMap;
    errorMap.insert(0, QContactManager::NoError);
    QVERIFY(!manager.saveContacts(0, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QContactManager::BadArgumentError);

    /* filters */
    QContactFilter f; // matches everything
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);
    QVERIFY(manager.contactIds(QContactFilter()).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contactIds(df).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contactIds(f | f).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.contactIds(df | df).count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(manager.isFilterSupported(f) == false);
    QVERIFY(manager.isFilterSupported(df) == false);

    QList<QContact> list;
    list << foo;

    QVERIFY(!manager.saveContacts(&list, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    QVERIFY(!manager.removeContacts(QList<QContactLocalId>(), &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QContactManager::BadArgumentError);

    QList<QContactLocalId> idlist;
    idlist << foo.id().localId();
    QVERIFY(!manager.removeContacts(idlist, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);

    /* Detail definitions */
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);

    QContactDetailDefinition def;
    def.setUnique(true);
    def.setName("new field");
    QMap<QString, QContactDetailFieldDefinition> fields;
    QContactDetailFieldDefinition currField;
    currField.setDataType(QVariant::String);
    fields.insert("value", currField);
    def.setFields(fields);

    QVERIFY(manager.saveDetailDefinition(def, QContactType::TypeContact) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.saveDetailDefinition(def) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.detailDefinitions().count(QContactType::TypeContact) == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.detailDefinition("new field").name() == QString());
    QVERIFY(manager.removeDetailDefinition(def.name(), QContactType::TypeContact) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.removeDetailDefinition(def.name()) == false);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::InvalidContactTypeError);

    /* Self contact id */
    QVERIFY(!manager.setSelfContactId(QContactLocalId(12)));
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.selfContactId() == QContactLocalId());
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::DoesNotExistError);

    /* Relationships */
    QContactId idOne, idTwo;
    idOne.setLocalId(QContactLocalId(15));
    idOne.setManagerUri(QString());
    idTwo.setLocalId(QContactLocalId(22));
    idTwo.setManagerUri(QString());
    QContactRelationship invalidRel;
    invalidRel.setFirst(idOne);
    invalidRel.setSecond(idTwo);
    QList<QContactRelationship> invalidRelList;
    invalidRelList << invalidRel;
    QVERIFY(!manager.saveRelationship(&invalidRel));
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    QVERIFY(manager.relationships().isEmpty());
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    manager.saveRelationships(&invalidRelList, NULL);
    QVERIFY(manager.error() == QContactManager::NotSupportedError);
    manager.removeRelationships(invalidRelList, NULL);
    QVERIFY(manager.error() == QContactManager::NotSupportedError || manager.error() == QContactManager::DoesNotExistError);

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
    nc.setFirstName("John");
    nc.setLastName("Civilian");
    c.saveDetail(&nc);
    m1.saveContact(&c);
    c.setId(QContactId());
    QContact c2;
    QContactName nc2 = c2.detail(QContactName::DefinitionName);
    c2 = c;
    nc2.setMiddleName("Public");
    c2.saveDetail(&nc2);
    m2.saveContact(&c2);            // save c2 first; c will be given a higher id
    m2.saveContact(&c);             // save c to m2
    c.setId(QContactId());
    nc.setSuffix("MD");
    c.saveDetail(&nc);
    m3.saveContact(&c);

    /* test that m1 != m2 != m3 and that m3 == m4 */

    // check the counts are correct - especially note m4 and m3.
    QCOMPARE(m1.contactIds().count(), 1);
    QCOMPARE(m2.contactIds().count(), 2);
    QCOMPARE(m3.contactIds().count(), 1);
    QCOMPARE(m4.contactIds().count(), 1);
    QCOMPARE(m5.contactIds().count(), 0);

    // remove c2 from m2 - ensure that this doesn't affect any other manager.
    m2.removeContact(c2.id().localId());
    QCOMPARE(m1.contactIds().count(), 1);
    QCOMPARE(m2.contactIds().count(), 1);
    QCOMPARE(m3.contactIds().count(), 1);
    QCOMPARE(m4.contactIds().count(), 1);
    QCOMPARE(m5.contactIds().count(), 0);

    // check that the contacts contained within are different.
    // note that in the m1->m2 case, only the id will be different!
    QVERIFY(m1.contact(m1.contactIds().at(0)) != m2.contact(m2.contactIds().at(0)));
    QVERIFY(m1.contact(m1.contactIds().at(0)) != m3.contact(m3.contactIds().at(0)));
    QVERIFY(m2.contact(m2.contactIds().at(0)) != m3.contact(m3.contactIds().at(0)));
    QVERIFY(m3.contact(m3.contactIds().at(0)) == m4.contact(m4.contactIds().at(0)));

    // now, we should be able to remove from m4, and have m3 empty
    QVERIFY(m4.removeContact(c.id().localId()));
    QCOMPARE(m3.contactIds().count(), 0);
    QCOMPARE(m4.contactIds().count(), 0);
    QCOMPARE(m5.contactIds().count(), 0);
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
    name.setFirstName(first);
    name.setMiddleName(middle);
    name.setLastName(last);
    name.setSuffix(suffix);

    name2.setPrefix(secondprefix);
    name2.setFirstName(secondfirst);
    name2.setMiddleName(secondmiddle);
    name2.setLastName(secondlast);
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
    QCOMPARE(cm.synthesizedContactDisplayLabel(c), expected);
}

void tst_QContactManager::compatibleContact_data()
{
    QTest::addColumn<QContact>("input");
    QTest::addColumn<QContact>("expected");
    QTest::addColumn<QContactManager::Error>("error");

    QContact baseContact;
    QContactName name;
    name.setFirstName(QLatin1String("First"));
    baseContact.saveDetail(&name);

    {
        QTest::newRow("already compatible") << baseContact << baseContact << QContactManager::NoError;
    }

    {
        QContact contact(baseContact);
        QContactDetail detail("UnknownDetail");
        detail.setValue("Key", QLatin1String("Value"));
        contact.saveDetail(&detail);
        QTest::newRow("unknown detail") << contact << baseContact << QContactManager::NoError;
    }

    {
        QContact contact(baseContact);
        QContactType type1;
        type1.setType(QContactType::TypeContact);
        contact.saveDetail(&type1);
        QContactType type2;
        type2.setType(QContactType::TypeGroup);
        contact.saveDetail(&type2);
        QContact expected(baseContact);
        expected.saveDetail(&type2);
        QTest::newRow("duplicate unique field") << contact << expected << QContactManager::NoError;
    }

    {
        QContact contact(baseContact);
        QContactPhoneNumber phoneNumber;
        phoneNumber.setValue("UnknownKey", "Value");
        contact.saveDetail(&phoneNumber);
        QTest::newRow("unknown field") << contact << baseContact << QContactManager::NoError;
    }

    {
        QContact contact(baseContact);
        QContactDisplayLabel displayLabel;
        displayLabel.setValue(QContactDisplayLabel::FieldLabel, QStringList("Value"));
        contact.saveDetail(&displayLabel);
        QTest::newRow("wrong type") << contact << baseContact << QContactManager::NoError;
    }

    {
        QContact contact(baseContact);
        QContactPhoneNumber phoneNumber1;
        phoneNumber1.setNumber(QLatin1String("1234"));
        phoneNumber1.setSubTypes(QStringList()
                                << QContactPhoneNumber::SubTypeMobile
                                << QContactPhoneNumber::SubTypeVoice
                                << QLatin1String("InvalidSubtype"));
        contact.saveDetail(&phoneNumber1);
        QContact expected(baseContact);
        QContactPhoneNumber phoneNumber2;
        phoneNumber2.setNumber(QLatin1String("1234"));
        phoneNumber2.setSubTypes(QStringList()
                                << QContactPhoneNumber::SubTypeMobile
                                << QContactPhoneNumber::SubTypeVoice);
        expected.saveDetail(&phoneNumber2);
        QTest::newRow("bad value (list)") << contact << expected << QContactManager::NoError;
    }

    {
        QContact contact(baseContact);
        QContactPhoneNumber phoneNumber1;
        phoneNumber1.setNumber(QLatin1String("1234"));
        phoneNumber1.setSubTypes(QStringList(QLatin1String("InvalidSubtype")));
        contact.saveDetail(&phoneNumber1);
        QContact expected(baseContact);
        QContactPhoneNumber phoneNumber2;
        phoneNumber2.setNumber(QLatin1String("1234"));
        expected.saveDetail(&phoneNumber2);
        QTest::newRow("all bad value (list)") << contact << expected << QContactManager::NoError;
    }

    {
        QContact contact(baseContact);
        QContactGender gender;
        gender.setGender(QLatin1String("UnknownGender"));
        contact.saveDetail(&gender);
        QTest::newRow("bad value (string)") << contact << baseContact << QContactManager::NoError;
    }

    {
        QContact contact;
        QContactGender gender;
        gender.setGender(QLatin1String("UnknownGender"));
        contact.saveDetail(&gender);
        QTest::newRow("bad value (string)") << contact << QContact() << QContactManager::DoesNotExistError;
    }
}

void tst_QContactManager::compatibleContact()
{
    QContactManager cm("memory");

    QFETCH(QContact, input);
    QFETCH(QContact, expected);
    QFETCH(QContactManager::Error, error);
    QCOMPARE(cm.compatibleContact(input), expected);
    QCOMPARE(cm.error(), error);
}

void tst_QContactManager::contactValidation()
{
    /* Use the memory engine as a reference (validation is not engine specific) */
    QScopedPointer<QContactManager> cm(new QContactManager("memory"));
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
    QMap<QString, QContactDetailFieldDefinition> fields;
    QContactDetailFieldDefinition field;
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
}

void tst_QContactManager::signalEmission()
{
    QTest::qWait(500); // clear the signal queue
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> m1(QContactManager::fromUri(uri));

    qRegisterMetaType<QContactLocalId>("QContactLocalId");
    qRegisterMetaType<QList<QContactLocalId> >("QList<QContactLocalId>");
    QSignalSpy spyCA(m1.data(), SIGNAL(contactsAdded(QList<QContactLocalId>)));
    QSignalSpy spyCM(m1.data(), SIGNAL(contactsChanged(QList<QContactLocalId>)));
    QSignalSpy spyCR(m1.data(), SIGNAL(contactsRemoved(QList<QContactLocalId>)));

    QList<QVariant> args;
    QList<QContactLocalId> arg;
    QContact c;
    QList<QContact> batchAdd;
    QList<QContactLocalId> batchRemove;
    QList<QContactLocalId> sigids;
    int addSigCount = 0; // the expected signal counts.
    int modSigCount = 0;
    int remSigCount = 0;

    QContactDetailDefinition nameDef = m1->detailDefinition(QContactName::DefinitionName, QContactType::TypeContact);

    // verify add emits signal added
    QContactName nc;
    saveContactName(&c, nameDef, &nc, "John");
    QVERIFY(m1->saveContact(&c));
    QContactLocalId cid = c.id().localId();
    addSigCount += 1;
    QTRY_COMPARE(spyCA.count(), addSigCount);
    args = spyCA.takeFirst();
    addSigCount -= 1;
    arg = args.first().value<QList<quint32> >();
    QVERIFY(arg.count() == 1);
    QCOMPARE(QContactLocalId(arg.at(0)), cid);

    // verify save modified emits signal changed
    saveContactName(&c, nameDef, &nc, "Citizen");
    QVERIFY(m1->saveContact(&c));
    modSigCount += 1;
    QTRY_COMPARE(spyCM.count(), modSigCount);
    args = spyCM.takeFirst();
    modSigCount -= 1;
    arg = args.first().value<QList<quint32> >();
    QVERIFY(arg.count() == 1);
    QCOMPARE(QContactLocalId(arg.at(0)), cid);

    // verify remove emits signal removed
    m1->removeContact(c.id().localId());
    remSigCount += 1;
    QTRY_COMPARE(spyCR.count(), remSigCount);
    args = spyCR.takeFirst();
    remSigCount -= 1;
    arg = args.first().value<QList<quint32> >();
    QVERIFY(arg.count() == 1);
    QCOMPARE(QContactLocalId(arg.at(0)), cid);

    // verify multiple adds works as advertised
    QContact c2, c3;
    QContactName nc2, nc3;
    saveContactName(&c2, nameDef, &nc2, "Mark");
    saveContactName(&c3, nameDef, &nc3, "Garry");
#if defined(Q_OS_SYMBIAN)
    // TODO: symbiansim backend fails this test currently. Commented out for
    // now. Will be fixed later.
    if(!uri.contains("symbiansim")) {
        QVERIFY(!m1->saveContact(&c)); // saving contact with nonexistent id fails
    }
#endif
    QVERIFY(m1->saveContact(&c2));
    addSigCount += 1;
    QVERIFY(m1->saveContact(&c3));
    addSigCount += 1;
    QTRY_COMPARE(spyCM.count(), modSigCount);
    QTRY_COMPARE(spyCA.count(), addSigCount);

    // verify multiple modifies works as advertised
    saveContactName(&c2, nameDef, &nc2, "M.");
    QVERIFY(m1->saveContact(&c2));
    modSigCount += 1;
    saveContactName(&c2, nameDef, &nc2, "Mark");
    saveContactName(&c3, nameDef, &nc3, "G.");
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
    QMap<int, QContactManager::Error> errorMap;
    QVERIFY(m1->saveContacts(&batchAdd, &errorMap));

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
    saveContactName(&c, nameDef, &modifiedName, "Modified number 1");
    modifiedName = c2.detail(QContactName::DefinitionName);
    saveContactName(&c2, nameDef, &modifiedName, "Modified number 2");
    modifiedName = c3.detail(QContactName::DefinitionName);
    saveContactName(&c3, nameDef, &modifiedName, "Modified number 3");

    batchAdd.clear();
    batchAdd << c << c2 << c3;
    QVERIFY(m1->saveContacts(&batchAdd, &errorMap));

    sigids.clear();
    QTRY_WAIT( while(spyCM.size() > 0) {sigids += spyCM.takeFirst().at(0).value<QList<QContactLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));

    /* Batch removes */
    batchRemove << c.id().localId() << c2.id().localId() << c3.id().localId();
    QVERIFY(m1->removeContacts(batchRemove, &errorMap));

    sigids.clear();
    QTRY_WAIT( while(spyCR.size() > 0) {sigids += spyCR.takeFirst().at(0).value<QList<QContactLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));

    QTRY_COMPARE(spyCA.count(), 0);
    QTRY_COMPARE(spyCM.count(), 0);

    QScopedPointer<QContactManager> m2(QContactManager::fromUri(uri));
    
    // During construction SIM backend (m2) will try writing contacts with 
    // nickname, email and additional number to find out if the SIM card
    // will support these fields. The other backend (m1) will then receive
    // signals about that. These need to be caught so they don't interfere
    // with the tests. (This trial and error method is used because existing
    // API for checking the availability of these fields is not public.)
	// NOTE: This applies only to pre 10.1 platforms (S60 3.1, 3.2, ect.)
    if (uri.contains("symbiansim")) {
        QTest::qWait(0);
        spyCA.clear();
        spyCM.clear();
        spyCR.clear();
    }

    QVERIFY(m1->hasFeature(QContactManager::Anonymous) ==
        m2->hasFeature(QContactManager::Anonymous));

    /* Now some cross manager testing */
    if (!m1->hasFeature(QContactManager::Anonymous)) {
        // verify that signals are emitted for modifications made to other managers (same id).
        QContactName ncs = c.detail(QContactName::DefinitionName);
        saveContactName(&c, nameDef, &ncs, "Test");
        c.setId(QContactId()); // reset id so save can succeed.
        QVERIFY(m2->saveContact(&c));
        saveContactName(&c, nameDef, &ncs, "Test2");
        QVERIFY(m2->saveContact(&c));
        QTRY_COMPARE(spyCA.count(), 1); // check that we received the update signals.
        QTRY_COMPARE(spyCM.count(), 1); // check that we received the update signals.
        m2->removeContact(c.localId());
        QTRY_COMPARE(spyCR.count(), 1); // check that we received the remove signal.
    }
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
    m2.saveContacts(0, 0);
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
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));
    QMap<QString, QContactDetailDefinition> defs = cm->detailDefinitions();

    /* Validate the existing definitions */

    // Do some sanity checking on the definitions first
    if (defs.keys().count() != defs.uniqueKeys().count()) {
        qDebug() << "ERROR - duplicate definitions with the same name:";

        QList<QString> defkeys = defs.keys();
        foreach(QString uniq, defs.uniqueKeys()) {
            if (defkeys.count(uniq) > 1) {
                qDebug() << QString(" %1").arg(uniq).toAscii().constData();
                defkeys.removeAll(uniq);
            }
        }
        QVERIFY(defs.keys().count() == defs.uniqueKeys().count());
    }

    foreach(QContactDetailDefinition def, defs.values()) {
        QMap<QString, QContactDetailFieldDefinition> fields = def.fields();

        // Again some sanity checking
        if (fields.keys().count() != fields.uniqueKeys().count()) {
            qDebug() << "ERROR - duplicate fields with the same name:";

            QList<QString> defkeys = fields.keys();
            foreach(QString uniq, fields.uniqueKeys()) {
                if (defkeys.count(uniq) > 1) {
                    qDebug() << QString(" %2::%1").arg(uniq).arg(def.name()).toAscii().constData();
                    defkeys.removeAll(uniq);
                }
            }
            QVERIFY(fields.keys().count() == fields.uniqueKeys().count());
        }

        foreach(QContactDetailFieldDefinition field, def.fields().values()) {
            // Sanity check the allowed values
            if (field.allowableValues().count() > 0) {
                if (field.dataType() == QVariant::StringList) {
                    // We accept QString or QStringList allowed values
                    foreach(QVariant var, field.allowableValues()) {
                        if (var.type() != QVariant::String && var.type() != QVariant::StringList) {
                            QString foo;
                            QDebug dbg(&foo);
                            dbg.nospace() << var;
                            qDebug().nospace() << "Field " << QString("%1::%2").arg(def.name()).arg(def.fields().key(field)).toAscii().constData() << " allowable value '" << foo.simplified().toAscii().constData() << "' not supported for field type " << QMetaType::typeName(field.dataType());
                        }
                        QVERIFY(var.type() == QVariant::String || var.type() == QVariant::StringList);
                    }
                } else if (field.dataType() == QVariant::List || field.dataType() == QVariant::Map || field.dataType() == (QVariant::Type) qMetaTypeId<QVariant>()) {
                    // Well, anything goes
                } else {
                    // The type of each allowed value must match the data type
                    foreach(QVariant var, field.allowableValues()) {
                        if (var.type() != field.dataType()) {
                            QString foo;
                            QDebug dbg(&foo);
                            dbg.nospace() << var;
                            qDebug().nospace() << "Field " << QString("%1::%2").arg(def.name()).arg(def.fields().key(field)).toAscii().constData() << " allowable value '" << foo.simplified().toAscii().constData() << "' not supported for field type " << QMetaType::typeName(field.dataType());
                        }
                        QVERIFY(var.type() == field.dataType());
                    }
                }
            }
        }
    }


    /* Try to make a credible definition */
    QContactDetailDefinition newDef;
    QContactDetailFieldDefinition field;
    QMap<QString, QContactDetailFieldDefinition> fields;
    field.setDataType(cm->supportedDataTypes().value(0));
    fields.insert("New Value", field);
    newDef.setName("New Definition");
    newDef.setFields(fields);

    /* Updated version of an existing definition */
    QContactDetailDefinition updatedDef = defs.begin().value(); // XXX TODO Fixme
    fields = updatedDef.fields();
    fields.insert("New Value", field);
    updatedDef.setFields(fields);

    /* A detail definition with valid allowed values (or really just one) */
    QContactDetailDefinition allowedDef = newDef;
    field.setAllowableValues(field.allowableValues() << (QVariant(field.dataType())));
    fields.clear();
    fields.insert("Restricted value", field);
    allowedDef.setFields(fields);

    /* Many invalid definitions */
    QContactDetailDefinition noIdDef;
    noIdDef.setFields(fields);

    QContactDetailDefinition noFieldsDef;
    noFieldsDef.setName("No fields");

    QContactDetailDefinition invalidFieldKeyDef;
    invalidFieldKeyDef.setName("Invalid field key");
    QMap<QString, QContactDetailFieldDefinition> badfields;
    badfields.insert(QString(), field);
    invalidFieldKeyDef.setFields(badfields);

    QContactDetailDefinition invalidFieldTypeDef;
    invalidFieldTypeDef.setName("Invalid field type");
    badfields.clear();
    QContactDetailFieldDefinition badfield;
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

    /* XXX Multiply defined fields.. depends on semantichangeSet. */

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
        QMap<QString, QContactDetailFieldDefinition> newFields = def.fields();
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
}

void tst_QContactManager::displayName()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    /*
     * Very similar to the tst_QContact functions, except we test
     * saving and retrieving contacts updates the display label
     */

    /* Try to make this a bit more consistent by using a single name */
    QContact d;
    QContactName name;
    saveContactName(&d, cm->detailDefinition(QContactName::DefinitionName, QContactType::TypeContact), &name, "Wesley");

    QVERIFY(d.displayLabel().isEmpty());

    QString synth = cm->synthesizedContactDisplayLabel(d);

    // Make sure this doesn't crash
    cm->synthesizeContactDisplayLabel(0);

    // Make sure this gives the same results
    cm->synthesizeContactDisplayLabel(&d);
    QCOMPARE(d.displayLabel(), synth);

    /*
     * The display label is not updated until you save the contact or call synthCDL
     */
    QVERIFY(cm->saveContact(&d));
    d = cm->contact(d.id().localId());
    QVERIFY(!d.isEmpty());
    QCOMPARE(d.displayLabel(), synth);

    /* Remove the detail via removeDetail */
    QContactDisplayLabel old;
    int count = d.details().count();
    QVERIFY(!d.removeDetail(&old)); // should fail.
    QVERIFY(d.isEmpty() == false);
    QVERIFY(d.details().count() == count); // it should not be removed!

    /* Save the contact again */
    QVERIFY(cm->saveContact(&d));
    d = cm->contact(d.id().localId());
    QVERIFY(!d.isEmpty());

    /* Make sure the label is still the synth version */
    QCOMPARE(d.displayLabel(), synth);

    /* And delete the contact */
    QVERIFY(cm->removeContact(d.id().localId()));
}

void tst_QContactManager::actionPreferences()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    // early out if the manager doesn't support action preference saving.
    if (!cm->hasFeature(QContactManager::ActionPreferences)) {
        QSKIP("Manager does not support action preferences", SkipSingle);
    }

    // create a sample contact
    QContactAvatar a;
    a.setImageUrl(QUrl("test.png"));
    QContactPhoneNumber p1;
    p1.setNumber("12345");
    QContactPhoneNumber p2;
    p2.setNumber("34567");
    QContactPhoneNumber p3;
    p3.setNumber("56789");
    QContactUrl u;
    u.setUrl("http://test.nokia.com");
    QContactName n;
    QContact c;
    saveContactName(&c, cm->detailDefinition(QContactName::DefinitionName, QContactType::TypeContact), &n, "TestContact");
    c.saveDetail(&a);
    c.saveDetail(&p1);
    c.saveDetail(&p2);
    c.saveDetail(&p3);
    c.saveDetail(&u);

    // set a preference for dialing a particular saved phonenumber.
    c.setPreferredDetail("Dial", p2);

    QVERIFY(cm->saveContact(&c));          // save the contact
    QContact loaded = cm->contact(c.id().localId()); // reload the contact

    // test that the preference was saved correctly.
    QContactDetail pref = loaded.preferredDetail("Dial");
    QVERIFY(pref == p2);

    cm->removeContact(c.id().localId());
}

void tst_QContactManager::changeSet()
{
    QContactLocalId id(1);

    QContactChangeSet changeSet;
    QVERIFY(changeSet.addedContacts().isEmpty());
    QVERIFY(changeSet.changedContacts().isEmpty());
    QVERIFY(changeSet.removedContacts().isEmpty());

    changeSet.insertAddedContact(id);
    QVERIFY(!changeSet.addedContacts().isEmpty());
    QVERIFY(changeSet.changedContacts().isEmpty());
    QVERIFY(changeSet.removedContacts().isEmpty());
    QVERIFY(changeSet.addedContacts().contains(id));

    changeSet.insertChangedContact(id);
    changeSet.insertChangedContacts(QList<QContactLocalId>() << id);
    QVERIFY(changeSet.changedContacts().size() == 1); // set, should only be added once.
    QVERIFY(!changeSet.addedContacts().isEmpty());
    QVERIFY(!changeSet.changedContacts().isEmpty());
    QVERIFY(changeSet.removedContacts().isEmpty());
    QVERIFY(changeSet.changedContacts().contains(id));
    changeSet.clearChangedContacts();
    QVERIFY(changeSet.changedContacts().isEmpty());

    changeSet.insertRemovedContacts(QList<QContactLocalId>() << id);
    QVERIFY(changeSet.removedContacts().contains(id));
    changeSet.clearRemovedContacts();
    QVERIFY(changeSet.removedContacts().isEmpty());

    QVERIFY(changeSet.dataChanged() == false);
    QContactChangeSet changeSet2;
    changeSet2 = changeSet;
    QVERIFY(changeSet.addedContacts() == changeSet2.addedContacts());
    changeSet.emitSignals(0);

    changeSet2.clearAddedContacts();
    QVERIFY(changeSet2.addedContacts().isEmpty());
    changeSet2.insertAddedContacts(changeSet.addedContacts().toList());
    QVERIFY(changeSet.addedContacts() == changeSet2.addedContacts());

    changeSet2.clearAll();
    QVERIFY(changeSet.addedContacts() != changeSet2.addedContacts());

    QContactChangeSet changeSet3(changeSet2);
    QVERIFY(changeSet.addedContacts() != changeSet3.addedContacts());
    QVERIFY(changeSet2.addedContacts() == changeSet3.addedContacts());

    changeSet.setDataChanged(true);
    QVERIFY(changeSet.dataChanged() == true);
    QVERIFY(changeSet.dataChanged() != changeSet2.dataChanged());
    QVERIFY(changeSet.dataChanged() != changeSet3.dataChanged());
    changeSet.emitSignals(0);

    changeSet.addedRelationshipsContacts().insert(id);
    changeSet.insertAddedRelationshipsContacts(QList<QContactLocalId>() << id);
    QVERIFY(changeSet.addedRelationshipsContacts().contains(id));
    changeSet.clearAddedRelationshipsContacts();
    QVERIFY(changeSet.addedRelationshipsContacts().isEmpty());
    changeSet.insertRemovedRelationshipsContacts(QList<QContactLocalId>() << id);
    QVERIFY(changeSet.removedRelationshipsContacts().contains(id));
    changeSet.clearRemovedRelationshipsContacts();
    QVERIFY(changeSet.removedRelationshipsContacts().isEmpty());
    changeSet.emitSignals(0);
    changeSet.removedRelationshipsContacts().insert(id);
    changeSet.emitSignals(0);

    changeSet.setOldAndNewSelfContactId(QPair<QContactLocalId, QContactLocalId>(QContactLocalId(0), id));
    changeSet2 = changeSet;
    QVERIFY(changeSet2.addedRelationshipsContacts() == changeSet.addedRelationshipsContacts());
    QVERIFY(changeSet2.removedRelationshipsContacts() == changeSet.removedRelationshipsContacts());
    QVERIFY(changeSet2.oldAndNewSelfContactId() == changeSet.oldAndNewSelfContactId());
    changeSet.emitSignals(0);
    changeSet.setOldAndNewSelfContactId(QPair<QContactLocalId, QContactLocalId>(id, QContactLocalId(0)));
    QVERIFY(changeSet2.oldAndNewSelfContactId() != changeSet.oldAndNewSelfContactId());
    changeSet.setDataChanged(true);
    changeSet.emitSignals(0);
}

void tst_QContactManager::fetchHint()
{
    QContactFetchHint hint;
    hint.setOptimizationHints(QContactFetchHint::NoBinaryBlobs);
    QCOMPARE(hint.optimizationHints(), QContactFetchHint::NoBinaryBlobs);
    QStringList rels;
    rels << QString(QLatin1String(QContactRelationship::HasMember));
    hint.setRelationshipTypesHint(rels);
    QCOMPARE(hint.relationshipTypesHint(), rels);
}

void tst_QContactManager::selfContactId()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    // early out if the manager doesn't support self contact id saving
    QContactLocalId selfContact = cm->selfContactId();
    if (!cm->hasFeature(QContactManager::SelfContact)) {
        // ensure that the error codes / return values are meaningful failures.
        QEXPECT_FAIL("mgr='maemo5'", "maemo5 supports getting the self contact but not setting it.", Continue);
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
    QSignalSpy spy(cm.data(), SIGNAL(selfContactIdChanged(QContactLocalId,QContactLocalId)));

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
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (!cm->hasFeature(QContactManager::DetailOrdering))
        QSKIP("Skipping: This manager does not support detail ordering!", SkipSingle);

    QContact a;
    //phone numbers

    QContactDetailDefinition d = cm->detailDefinition(QContactPhoneNumber::DefinitionName, QContactType::TypeContact);
    QContactDetailFieldDefinition supportedContexts = d.fields().value(QContactDetail::FieldContext);
    QString contextOther = QContactDetail::ContextOther;
    if (!supportedContexts.allowableValues().contains(contextOther)) {
        contextOther = QString();
    }    
    
    QContactPhoneNumber number1, number2, number3;
    
    number1.setNumber("11111111");
    number1.setContexts(QContactPhoneNumber::ContextHome);

    number2.setNumber("22222222");
    number2.setContexts(QContactPhoneNumber::ContextWork);

    number3.setNumber("33333333");
    if (!contextOther.isEmpty())
        number3.setContexts(contextOther);

    a.saveDetail(&number1);
    a.saveDetail(&number2);
    a.saveDetail(&number3);

    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    
    QList<QContactDetail> details = a.details(QContactPhoneNumber::DefinitionName);
    QVERIFY(details.count() == 3);
    QVERIFY(details.at(0).value(QContactPhoneNumber::FieldContext) == QContactPhoneNumber::ContextHome);
    QVERIFY(details.at(1).value(QContactPhoneNumber::FieldContext) == QContactPhoneNumber::ContextWork);
    QVERIFY(details.at(2).value(QContactPhoneNumber::FieldContext) == contextOther);
    
    QVERIFY(a.removeDetail(&number2));
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    details = a.details(QContactPhoneNumber::DefinitionName);
    QVERIFY(details.count() == 2);
    QVERIFY(details.at(0).value(QContactPhoneNumber::FieldContext) == QContactPhoneNumber::ContextHome);
    QVERIFY(details.at(1).value(QContactPhoneNumber::FieldContext) == contextOther);

    a.saveDetail(&number2);
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    
    details = a.details(QContactPhoneNumber::DefinitionName);
    QVERIFY(details.count() == 3);
    QVERIFY(details.at(0).value(QContactPhoneNumber::FieldContext) == QContactPhoneNumber::ContextHome);
    QVERIFY(details.at(1).value(QContactPhoneNumber::FieldContext) == contextOther);
    QVERIFY(details.at(2).value(QContactPhoneNumber::FieldContext) == QContactPhoneNumber::ContextWork);

    //addresses
    
    d = cm->detailDefinition(QContactAddress::DefinitionName, QContactType::TypeContact);
    supportedContexts = d.fields().value(QContactDetail::FieldContext);
    contextOther = QString(QLatin1String(QContactDetail::ContextOther));
    if (!supportedContexts.allowableValues().contains(contextOther)) {
        contextOther = QString();
    }     
    
    QContactAddress address1, address2, address3;
    
    address1.setStreet("Brandl St");
    address1.setRegion("Brisbane");
    address3 = address2 = address1;

    address1.setContexts(QContactAddress::ContextHome);
    address2.setContexts(QContactAddress::ContextWork);
    if (!contextOther.isEmpty())
        address3.setContexts(contextOther);

    a.saveDetail(&address1);
    a.saveDetail(&address2);
    a.saveDetail(&address3);

    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    
    details = a.details(QContactAddress::DefinitionName);
    QVERIFY(details.count() == 3);
    
    QVERIFY(details.at(0).value(QContactAddress::FieldContext) == QContactAddress::ContextHome);
    QVERIFY(details.at(1).value(QContactAddress::FieldContext) == QContactAddress::ContextWork);
    QVERIFY(details.at(2).value(QContactAddress::FieldContext) == contextOther);

    QVERIFY(a.removeDetail(&address2));
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    details = a.details(QContactAddress::DefinitionName);
    QVERIFY(details.count() == 2);
    QVERIFY(details.at(0).value(QContactAddress::FieldContext) == QContactAddress::ContextHome);
    QVERIFY(details.at(1).value(QContactAddress::FieldContext) == contextOther);

    a.saveDetail(&address2);
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    
    details = a.details(QContactAddress::DefinitionName);
    QVERIFY(details.count() == 3);
    QVERIFY(details.at(0).value(QContactAddress::FieldContext) == QContactAddress::ContextHome);
    QVERIFY(details.at(1).value(QContactAddress::FieldContext) == contextOther);
    QVERIFY(details.at(2).value(QContactAddress::FieldContext) == QContactAddress::ContextWork);


    //emails
    d = cm->detailDefinition(QContactEmailAddress::DefinitionName, QContactType::TypeContact);
    supportedContexts = d.fields().value(QContactDetail::FieldContext);
    contextOther = QString(QLatin1String(QContactDetail::ContextOther));
    if (!supportedContexts.allowableValues().contains(contextOther)) {
        contextOther = QString();
    }      
    
    QContactEmailAddress email1, email2, email3;

    email1.setEmailAddress("aaron@example.com");
    email3 = email2 = email1;
    email1.setContexts(QContactEmailAddress::ContextHome);
    email2.setContexts(QContactEmailAddress::ContextWork);
    if (!contextOther.isEmpty())
        email3.setContexts(contextOther);

    a.saveDetail(&email1);
    a.saveDetail(&email2);
    a.saveDetail(&email3);

    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    
    details = a.details(QContactEmailAddress::DefinitionName);
    QVERIFY(details.count() == 3);
    
    QVERIFY(details.at(0).value(QContactEmailAddress::FieldContext) == QContactEmailAddress::ContextHome);
    QVERIFY(details.at(1).value(QContactEmailAddress::FieldContext) == QContactEmailAddress::ContextWork);
    QVERIFY(details.at(2).value(QContactEmailAddress::FieldContext) == contextOther);

    QVERIFY(a.removeDetail(&email2));
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    details = a.details(QContactEmailAddress::DefinitionName);
    QVERIFY(details.count() == 2);
    QVERIFY(details.at(0).value(QContactEmailAddress::FieldContext) == QContactEmailAddress::ContextHome);
    QVERIFY(details.at(1).value(QContactEmailAddress::FieldContext) == contextOther);

    a.saveDetail(&email2);
    QVERIFY(cm->saveContact(&a));
    a = cm->contact(a.id().localId());
    
    details = a.details(QContactEmailAddress::DefinitionName);
    QVERIFY(details.count() == 3);
    QVERIFY(details.at(0).value(QContactEmailAddress::FieldContext) == QContactEmailAddress::ContextHome);
    QVERIFY(details.at(1).value(QContactEmailAddress::FieldContext) == contextOther);
    QVERIFY(details.at(2).value(QContactEmailAddress::FieldContext) == QContactEmailAddress::ContextWork);

    QVERIFY(cm->removeContact(a.id().localId()));
    QVERIFY(cm->error() == QContactManager::NoError);
}

void tst_QContactManager::relationships()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    // save some contacts
    QContact source;
    QContact dest1, dest2, dest3, dest4;
    QContactPhoneNumber n1, n2, n3, n4;
    n1.setNumber("1");
    n2.setNumber("2");
    n3.setNumber("3");
    n4.setNumber("4");

    dest1.saveDetail(&n1);
    dest2.saveDetail(&n2);
    dest3.saveDetail(&n3);
    dest4.saveDetail(&n3);

    cm->saveContact(&source);
    cm->saveContact(&dest1);
    cm->saveContact(&dest2);
    cm->saveContact(&dest3);
    cm->saveContact(&dest4);

    // check if manager supports relationships
    if (!cm->hasFeature(QContactManager::Relationships)) {
        // ensure that the operations all fail as required.
        QContactRelationship r1, r2, r3;
        r1.setFirst(source.id());
        r1.setSecond(dest1.id());
        r1.setRelationshipType(QContactRelationship::HasManager);
        r2.setFirst(source.id());
        r2.setSecond(dest2.id());
        r2.setRelationshipType(QContactRelationship::HasManager);
        r3.setFirst(source.id());
        r3.setSecond(dest3.id());
        r3.setRelationshipType(QContactRelationship::HasManager);

        QList<QContactRelationship> batchList;
        batchList << r2 << r3;

        // test save and remove
        QVERIFY(!cm->saveRelationship(&r1));
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        QVERIFY(!cm->removeRelationship(r1));
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        cm->saveRelationships(&batchList, NULL);
        QVERIFY(cm->error() == QContactManager::NotSupportedError);

        // test retrieval
        QList<QContactRelationship> retrieveList;
        retrieveList = cm->relationships(source.id(), QContactRelationship::First);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(source.id(), QContactRelationship::Second);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(source.id(), QContactRelationship::Either); // Either
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);


        retrieveList = cm->relationships(QContactRelationship::HasManager, source.id(), QContactRelationship::First);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(QContactRelationship::HasManager, source.id(), QContactRelationship::Second);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(QContactRelationship::HasManager, source.id(), QContactRelationship::Either);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(QContactRelationship::HasManager, source.id());
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        retrieveList = cm->relationships(QContactRelationship::HasManager);
        QVERIFY(retrieveList.isEmpty());
        QVERIFY(cm->error() == QContactManager::NotSupportedError);
        return;
    }
    
    // Get supported relationship types
    QStringList availableRelationshipTypes;
    if (cm->isRelationshipTypeSupported(QContactRelationship::HasMember))
        availableRelationshipTypes << QContactRelationship::HasMember;    
    if (cm->isRelationshipTypeSupported(QContactRelationship::HasAssistant))
        availableRelationshipTypes << QContactRelationship::HasAssistant;
    if (cm->isRelationshipTypeSupported(QContactRelationship::HasManager))
        availableRelationshipTypes << QContactRelationship::HasManager;
    if (cm->isRelationshipTypeSupported(QContactRelationship::HasSpouse))
        availableRelationshipTypes << QContactRelationship::HasSpouse;
    if (cm->isRelationshipTypeSupported(QContactRelationship::IsSameAs))
        availableRelationshipTypes << QContactRelationship::IsSameAs;

    
    // Check arbitary relationship support
    if (cm->hasFeature(QContactManager::ArbitraryRelationshipTypes)) {
        // add some arbitary type for testing
        if (availableRelationshipTypes.count())
            availableRelationshipTypes.insert(0, "test-arbitrary-relationship-type");
        else {
            availableRelationshipTypes.append("test-arbitrary-relationship-type");
            availableRelationshipTypes.append(QContactRelationship::HasMember);
            availableRelationshipTypes.append(QContactRelationship::HasAssistant);
        }
    }
    
    // Verify that we have relationship types. If there are none then the manager
    // is saying it supports relationships but does not actually implement any 
    // relationship type.
    QVERIFY(!availableRelationshipTypes.isEmpty());
    
    // Some backends (eg. symbian) require that when type is "HasMember" 
    // then "first" contact must be a group.
    if (availableRelationshipTypes.at(0) == QContactRelationship::HasMember) {
        cm->removeContact(source.localId());
        source.setId(QContactId());
        source.setType(QContactType::TypeGroup);
        cm->saveContact(&source);
    }

    // Create some common contact id's for testing
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
    int managerRelationshipsCount = cm->relationships(availableRelationshipTypes.at(0)).count();
    QVERIFY(cm->saveRelationship(&customRelationshipOne));
    // test that the empty manager URI has been updated.
    QCOMPARE(customRelationshipOne.second(), dest1Uri); // updated with correct manager URI

    // test our accessors.
    QCOMPARE(cm->relationships(availableRelationshipTypes.at(0)).count(), (managerRelationshipsCount + 1));
    QVERIFY(cm->relationships(availableRelationshipTypes.at(0), source.id()).count() == 1);

    // remove the dest1 contact, relationship should be removed.
    cm->removeContact(dest1.localId());
    QCOMPARE(cm->relationships(availableRelationshipTypes.at(0), dest1Uri, QContactRelationship::Second).count(), 0);

    // modify and save the relationship
    customRelationshipOne.setSecond(dest2Uri);
    QVERIFY(cm->saveRelationship(&customRelationshipOne));

    // attempt to save the relationship again.  XXX TODO: what should the result be?  currently succeeds (overwrites)
    int relationshipsCount = cm->relationships().count();
    QVERIFY(cm->saveRelationship(&customRelationshipOne));    // succeeds, but just overwrites
    QCOMPARE(relationshipsCount, cm->relationships().count()); // shouldn't change; save should have overwritten.

    // removing the source contact should result in removal of the relationship.
    QVERIFY(cm->removeContact(source.id().localId()));
    QCOMPARE(cm->relationships().count(), relationshipsCount - 1); // the relationship should have been removed.

    // now ensure that qcontact relationship caching works as required - perhaps this should be in tst_QContact?
    source.setId(QContactId());         // reset id so we can resave
    QVERIFY(cm->saveContact(&source));  // save source again.
    customRelationshipOne.setFirst(source.id());
    customRelationshipOne.setSecond(dest2.id());
    QVERIFY(cm->saveRelationship(&customRelationshipOne));

    // Add a second relationship
    QContactRelationship customRelationshipTwo;
    customRelationshipTwo.setFirst(source.id());
    if (availableRelationshipTypes.count() > 1)
        customRelationshipTwo.setRelationshipType(availableRelationshipTypes.at(1));
    else
        customRelationshipTwo.setRelationshipType(availableRelationshipTypes.at(0));
    customRelationshipTwo.setSecond(dest3.id());
    QVERIFY(cm->saveRelationship(&customRelationshipTwo));

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
    QVERIFY(source.relatedContacts(QString(), QContactRelationship::First).isEmpty()); // source is always the first, so this should be empty.
    QVERIFY(source.relatedContacts(QString(), QContactRelationship::Second).contains(dest2.id()));
    QVERIFY(source.relatedContacts(QString(), QContactRelationship::Either).contains(dest2.id()));
    QVERIFY(source.relatedContacts(QString(), QContactRelationship::Second).contains(dest3.id()));
    QVERIFY(source.relatedContacts(QString(), QContactRelationship::Either).contains(dest3.id()));
    QVERIFY(source.relatedContacts(availableRelationshipTypes.at(0), QContactRelationship::Second).contains(dest2.id()));
    QVERIFY(source.relatedContacts(availableRelationshipTypes.at(0), QContactRelationship::First).isEmpty());

    QVERIFY(dest2.relatedContacts().contains(source.id()));
    QVERIFY(dest2.relationships().contains(customRelationshipOne));
    QVERIFY(!dest2.relationships().contains(customRelationshipTwo));
    QVERIFY(dest2.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipOne));
    QVERIFY(!dest2.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipTwo));
    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0)).contains(source.id()));
    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0), QContactRelationship::First).contains(source.id()));
    QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(0), QContactRelationship::Second).isEmpty());
    QVERIFY(!dest2.relatedContacts(availableRelationshipTypes.at(0), QContactRelationship::Second).contains(source.id()));
    
    QVERIFY(dest3.relatedContacts().contains(source.id()));
    QVERIFY(!dest3.relationships().contains(customRelationshipOne));
    QVERIFY(dest3.relationships().contains(customRelationshipTwo));
    QVERIFY(!dest3.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipOne));

    // Test iteration
    QList<QContactRelationship> relats = source.relationships();
    QList<QContactRelationship>::iterator it = relats.begin();

    while (it != relats.end()) {
        QContactId firstId = it->first();
        QVERIFY(firstId == source.id());
        QVERIFY(it->second() == dest2.id() || it->second() == dest3.id());
        it++;
    }
    
    if (availableRelationshipTypes.count() > 1) {
        QVERIFY(source.relatedContacts(availableRelationshipTypes.at(1), QContactRelationship::Second).contains(dest3.id()));
        QVERIFY(source.relatedContacts(availableRelationshipTypes.at(1), QContactRelationship::First).isEmpty());
        
        QVERIFY(dest2.relationships(availableRelationshipTypes.at(1)).isEmpty());
        
        QVERIFY(!dest3.relationships(availableRelationshipTypes.at(0)).contains(customRelationshipTwo));
        QVERIFY(dest3.relationships(availableRelationshipTypes.at(1)).contains(customRelationshipTwo));
        QVERIFY(!dest3.relationships(availableRelationshipTypes.at(1)).contains(customRelationshipOne));
        QVERIFY(dest3.relatedContacts(availableRelationshipTypes.at(1)).contains(source.id()));
        QVERIFY(!dest3.relatedContacts(availableRelationshipTypes.at(0)).contains(source.id()));
        QVERIFY(dest3.relatedContacts(availableRelationshipTypes.at(1)).contains(source.id())); // role = either
        QVERIFY(!dest3.relatedContacts(availableRelationshipTypes.at(1), QContactRelationship::Second).contains(source.id()));
        QVERIFY(dest3.relatedContacts(availableRelationshipTypes.at(1), QContactRelationship::First).contains(source.id()));
        QVERIFY(dest2.relatedContacts(availableRelationshipTypes.at(1)).isEmpty());
    }
    else {
        QVERIFY(source.relatedContacts(availableRelationshipTypes.at(0), QContactRelationship::Second).contains(dest3.id()));
    }
    
    // Cleanup a bit
    QMap<int, QContactManager::Error> errorMap;
    QList<QContactRelationship> moreRels;

    moreRels << customRelationshipOne << customRelationshipTwo;
    errorMap.insert(5, QContactManager::BadArgumentError);
    QVERIFY(cm->removeRelationships(moreRels, &errorMap));
    QVERIFY(errorMap.count() == 0);

    // test batch API and ordering in contacts
    QList<QContactRelationship> currentRelationships = cm->relationships(source.id(), QContactRelationship::First);
    QList<QContactRelationship> batchList;
    QContactRelationship br1, br2, br3;
    br1.setFirst(source.id());
    br1.setSecond(dest2.id());
    br1.setRelationshipType(availableRelationshipTypes.at(0));    
    br2.setFirst(source.id());
    br2.setSecond(dest3.id());
    br2.setRelationshipType(availableRelationshipTypes.at(0));
    if (availableRelationshipTypes.count() > 1)
    {
        br3.setFirst(source.id());
        br3.setSecond(dest3.id());
        br3.setRelationshipType(availableRelationshipTypes.at(1));
    }
    else
    {
        br3.setFirst(source.id());
        br3.setSecond(dest4.id());
        br3.setRelationshipType(availableRelationshipTypes.at(0));
    }
    batchList << br1 << br2 << br3;

    // ensure that the batch save works properly
    cm->saveRelationships(&batchList, NULL);
    QCOMPARE(cm->error(), QContactManager::NoError);
    QList<QContactRelationship> batchRetrieve = cm->relationships(source.id(), QContactRelationship::First);
    QVERIFY(batchRetrieve.contains(br1));
    QVERIFY(batchRetrieve.contains(br2));
    QVERIFY(batchRetrieve.contains(br3));
    
    // remove a single relationship
    QVERIFY(cm->removeRelationship(br3));
    batchRetrieve = cm->relationships(source.id(), QContactRelationship::First);
    QVERIFY(batchRetrieve.contains(br1));
    QVERIFY(batchRetrieve.contains(br2));
    QVERIFY(!batchRetrieve.contains(br3)); // has already been removed.

    // now ensure that the batch remove works and we get returned to the original state.
    batchList.removeOne(br3);
    cm->removeRelationships(batchList, NULL);
    QVERIFY(cm->error() == QContactManager::NoError);
    QCOMPARE(cm->relationships(source.id(), QContactRelationship::First), currentRelationships);

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
    QContactRelationship maliciousRel;
    maliciousRel.setFirst(source.id());
    maliciousRel.setSecond(nonexistentDest);
    maliciousRel.setRelationshipType("nokia-test-invalid-relationship-type");
    QVERIFY(!cm->saveRelationship(&maliciousRel));

    // attempt to save a circular relationship - should fail!
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
    relationshipsCount = cm->relationships().count();
    QVERIFY(!cm->removeRelationship(maliciousRel));         // does not exist; fail remove.
    QVERIFY(cm->error() == QContactManager::DoesNotExistError || cm->error() == QContactManager::InvalidRelationshipError);
    QCOMPARE(cm->relationships().count(), relationshipsCount); // should be unchanged.

    // now we want to ensure that a relationship is removed if one of the contacts is removed.
    customRelationshipOne.setFirst(source.id());
    customRelationshipOne.setSecond(dest2.id());
    customRelationshipOne.setRelationshipType(availableRelationshipTypes.at(0));

    // Test batch save with an error map
    moreRels.clear();
    moreRels << customRelationshipOne;
    errorMap.insert(0, QContactManager::BadArgumentError);
    QVERIFY(cm->saveRelationships(&moreRels, &errorMap));
    QVERIFY(cm->error() == QContactManager::NoError);
    QVERIFY(errorMap.count() == 0); // should be reset
    source = cm->contact(source.localId());
    dest2 = cm->contact(dest2.localId());
    QVERIFY(cm->removeContact(dest2.localId())); // remove dest2, the relationship should be removed
    QVERIFY(cm->relationships(availableRelationshipTypes.at(0), dest2.id(), QContactRelationship::Second).isEmpty());
    source = cm->contact(source.localId());
    QVERIFY(!source.relatedContacts().contains(dest2.id())); // and it shouldn't appear in cache.

    // now clean up and remove our dests.
    QVERIFY(cm->removeContact(source.localId()));
    QVERIFY(cm->removeContact(dest3.localId()));

    // attempt to save relationships with nonexistent contacts
    QVERIFY(!cm->saveRelationship(&br1));
    QVERIFY(cm->error() == QContactManager::InvalidRelationshipError);
    cm->saveRelationships(&batchList, NULL);
    QVERIFY(cm->error() == QContactManager::InvalidRelationshipError);
    QVERIFY(!cm->removeRelationship(br1));
    QVERIFY(cm->error() == QContactManager::DoesNotExistError || cm->error() == QContactManager::InvalidRelationshipError);
    cm->removeRelationships(batchList, NULL);
    QVERIFY(cm->error() == QContactManager::DoesNotExistError || cm->error() == QContactManager::InvalidRelationshipError);
}

void tst_QContactManager::contactType()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

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
    QVERIFY(cm->contactIds(groupFilter).contains(g1.localId()));
    QVERIFY(cm->contactIds(groupFilter).contains(g2.localId()));
    QVERIFY(!cm->contactIds(groupFilter).contains(c.localId()));

    QList<QContactSortOrder> sortOrders;
    QContactSortOrder byPhoneNumber;
    byPhoneNumber.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    sortOrders.append(byPhoneNumber);

    // and ensure that sorting works properly with typed contacts also
    QList<QContactLocalId> sortedIds = cm->contactIds(groupFilter, sortOrders);
    QVERIFY(sortedIds.indexOf(g2.localId()) < sortedIds.indexOf(g1.localId()));

    cm->removeContact(g1.localId());
    cm->removeContact(g2.localId());
    cm->removeContact(c.localId());
}

#if defined(USE_VERSIT_PLZ)
void tst_QContactManager::partialSave()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    QVersitContactImporter imp;
    QVersitReader reader(QByteArray(
            "BEGIN:VCARD\r\nFN:Alice\r\nN:Alice\r\nTEL:12345\r\nEND:VCARD\r\n"
            "BEGIN:VCARD\r\nFN:Bob\r\nN:Bob\r\nTEL:5678\r\nEND:VCARD\r\n"
            "BEGIN:VCARD\r\nFN:Carol\r\nN:Carol\r\nEMAIL:carol@example.com\r\nEND:VCARD\r\n"
            "BEGIN:VCARD\r\nFN:David\r\nN:David\r\nORG:DavidCorp\r\nEND:VCARD\r\n"));
    reader.startReading();
    reader.waitForFinished();
    qDebug() << reader.error();
    QCOMPARE(reader.error(), QVersitReader::NoError);

    QCOMPARE(reader.results().count(), 4);
    QVERIFY(imp.importDocuments(reader.results()));
    QCOMPARE(imp.contacts().count(), 4);
    QVERIFY(imp.contacts()[0].displayLabel() == QLatin1String("Alice"));
    QVERIFY(imp.contacts()[1].displayLabel() == QLatin1String("Bob"));
    QVERIFY(imp.contacts()[2].displayLabel() == QLatin1String("Carol"));
    QVERIFY(imp.contacts()[3].displayLabel() == QLatin1String("David"));

    QList<QContact> contacts = imp.contacts();

    // First save these contacts
    QVERIFY(cm->saveContacts(&contacts, 0));
    QList<QContact> originalContacts = contacts;

    // Now try some partial save operations
    // 0) empty mask == full save
    // 1) Ignore an added phonenumber
    // 2) Only save a modified phonenumber, not a modified email
    // 3) Remove an email address & phone, mask out phone
    // 4) Have a bad manager uri in the middle
    // 5) Have a non existing contact in the middle
    // 6) new contact, no details in the mask
    // 7) new contact, some details in the mask
    // 8) new contact, all details ok

    QContactPhoneNumber pn;
    pn.setNumber("111111");
    contacts[0].saveDetail(&pn);

    // 0) empty mask
    QVERIFY(cm->saveContacts(&contacts, QStringList(), 0));

    // That should have updated everything
    QContact a = cm->contact(originalContacts[0].localId());
    QVERIFY(a.details<QContactPhoneNumber>().count() == 2);

    // 1) Add a phone number to b, mask it out
    contacts[1].saveDetail(&pn);
    QVERIFY(cm->saveContacts(&contacts, QStringList(QContactEmailAddress::DefinitionName), 0));
    //

    QContact b = cm->contact(originalContacts[1].localId());
    QVERIFY(b.details<QContactPhoneNumber>().count() == 1);

    // 2) save a modified detail in the mask
    QContactEmailAddress e;
    e.setEmailAddress("example@example.com");
    contacts[1].saveDetail(&e); // contacts[1] should have both phone and email

    QVERIFY(cm->saveContacts(&contacts, QStringList(QContactEmailAddress::DefinitionName), 0));
    b = cm->contact(originalContacts[1].localId());
    QVERIFY(b.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(b.details<QContactEmailAddress>().count() == 1);

    // 3) Remove an email address and a phone number
    QVERIFY(contacts[1].removeDetail(&e));
    QVERIFY(contacts[1].removeDetail(&pn));
    QVERIFY(contacts[1].details<QContactEmailAddress>().count() == 0);
    QVERIFY(contacts[1].details<QContactPhoneNumber>().count() == 1);
    QVERIFY(cm->saveContacts(&contacts, QStringList(QContactEmailAddress::DefinitionName), 0));
    b = cm->contact(originalContacts[1].localId());
    QVERIFY(b.details<QContactPhoneNumber>().count() == 1);
    QVERIFY(b.details<QContactEmailAddress>().count() == 0);

    // 4,5  TODO

    // 6 - New contact, no details in the mask
    QContact newContact = originalContacts[3];
    newContact.setId(QContactId());

    contacts.append(newContact);
    QVERIFY(cm->saveContacts(&contacts, QStringList(QContactEmailAddress::DefinitionName), 0));
    QVERIFY(contacts[4].localId() != 0); // Saved
    b = cm->contact(contacts[4].localId());
    qDebug() << b;
    QVERIFY(b.details<QContactOrganization>().count() == 0); // not saved
    QVERIFY(b.details<QContactName>().count() == 0); // not saved

    // 7 - New contact, some details in the mask
    newContact = originalContacts[2];
    contacts.append(newContact);
    QVERIFY(cm->saveContacts(&contacts, QStringList(QContactEmailAddress::DefinitionName), 0));
    QVERIFY(contacts[5].localId() != 0); // Saved
    b = cm->contact(contacts[5].localId());
    QVERIFY(b.details<QContactEmailAddress>().count() == 1);
    QVERIFY(b.details<QContactName>().count() == 0); // not saved
    qDebug() << b;


}
#endif

QString tst_QContactManager::generateRandomString(int strlen, bool numeric) const
{
    QString retn;

    for (int i = 0; i < strlen; ++i) {
        int random = qrand();
        if (numeric) {
            random = random % 10;
            retn += QString::number(random);
        } else {
            random = random % 26;
            unsigned char letter = ('a' + static_cast<unsigned char>(random));
            retn += QChar(letter);
        }
    }

    return retn;
}

void tst_QContactManager::benchmarking()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    qDebug() << "Performing Benchmark Test: this may take some time, please be patient...";

    // we aggregate results from multiple runs of each benchmark
    int AGGREGATION_LIMIT = 5;
    int NUMBER_OF_CONTACTS = 500;

    // benchmark 1: save 500 new contacts.
    // benchmark 2: update 500 existing contacts.
    // benchmark 3: retrieve all contact ids.
    // benchmark 4: retrieve contact ids matching filter.
    // benchmark 5: retrieve contact ids matching filter, sorted.
    // benchmark 6: retrieve all contacts.
    // benchmark 7: retrieve contacts matching filter.
    // benchmark 8: retrieve contacts matching filter, sorted.
    // benchmark 9: remove 100 selected contacts.
    // benchmark 10: remove all contacts.
    QStringList benchmarkDescriptions;
    benchmarkDescriptions << "Save 500 contacts" << "Update 500 contacts" << "Retrieve ids (all)" << "Retrieve ids (filter)"
                          << "Retrieve ids (filter, sorted)" << "Retrieve contacts (all)" << "Retrieve contacts (filter)"
                          << "Retrieve contacts (filter, sorted)" << "Remove 100 contacts" << "Remove all contacts";
    int largestDescriptionLength = 0;
    for (int i = 0; i < benchmarkDescriptions.size(); ++i) {
        if (benchmarkDescriptions.at(i).length() > largestDescriptionLength) {
            largestDescriptionLength = benchmarkDescriptions.at(i).length();
        }
    }

    // we perform each benchmark synchronously, and asynchronously.
    // the average time for each benchmark is reported, however
    // this time does not include the time taken for erroneous operations,
    // as this is reported separately.

    // successful synchronous benchmarks
    quint64 ssb[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // erroneous synchronous benchmarks
    quint64 esb[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // successful asynchronous benchmarks
    quint64 sab[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // erroneous asynchronous benchmarks
    quint64 eab[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // count of erroneous synchronous benchmark operations
    quint64 esbc[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // count of erroneous asynchronous benchmark operations
    quint64 eabc[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // define some details which we'll use.
    QContactName name;
    QContactPhoneNumber phoneNumber;
    QContactEmailAddress emailAddress;
    QContactThumbnail thumbnail;
    QByteArray thumbnailArray =
        "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"\
        "ngAAAABJRU5ErkJggg==";
    QImage thumbnailImage = QImage::fromData(QByteArray::fromBase64(thumbnailArray));
    thumbnail.setThumbnail(thumbnailImage);

    // the filter and sort order which we'll use.
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldPrefix);
    df.setValue("Dr.");
    df.setMatchFlags(QContactFilter::MatchExactly);
    QContactSortOrder singleSort;
    singleSort.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
    QList<QContactSortOrder> sorting;
    sorting.append(singleSort);

    // intialise the random number generator
    qsrand(QDateTime::currentDateTime().toTime_t());

    // now generate the contacts.
    QList<QContact> contacts;
    for (int i = 0; i < NUMBER_OF_CONTACTS; ++i) {
        // the first 100 contacts are special: they get a prefix
        if (i < 100) {
            name.setPrefix("Dr.");
        } else {
            name.setPrefix("");
        }
        name.setFirstName(generateRandomString(((qrand()%8) + 1)));
        name.setLastName(generateRandomString(((qrand()%10) + 1)));
        phoneNumber.setNumber(generateRandomString(((qrand()%10) + 1), true));
        QString email = generateRandomString(((qrand()%7) + 1));
        email += "@test.com";
        emailAddress.setEmailAddress(email);
        QContact temp;
        temp.saveDetail(&name);
        temp.saveDetail(&phoneNumber);
        temp.saveDetail(&emailAddress);
        temp.saveDetail(&thumbnail);
        contacts.append(temp);
    }

    // now we can perform the benchmarks.
    QTime timer;
    bool success = false;
    QMap<int, QContactManager::Error> errorMap;
    for (int i = 0; i < AGGREGATION_LIMIT; ++i) {
        // synchronous benchmark 1 - saving 500 new contacts.
        QList<QContact> tempList = contacts;
        tempList.swap(0,1); // force detach, we don't want that to contaminate benchmark.
        timer.start();
        success = cm->saveContacts(&tempList, &errorMap);
        int tempElapsed = timer.elapsed();
        if (!success) {
            esb[0] += tempElapsed;
            esbc[0] += 1;
        } else {
            ssb[0] += tempElapsed;
        }

        // synchronous benchmark 2 - update all 500 existing contacts.
        for (int j = 0; j < tempList.size(); ++j) {
            QContact tempContact = tempList.at(j);
            QContactName tempName = tempContact.detail<QContactName>();
            tempName.setMiddleName("updated");
            tempContact.saveDetail(&tempName);
            tempList.replace(j, tempContact);
        }
        timer.start();
        success = cm->saveContacts(&tempList, &errorMap);
        tempElapsed = timer.elapsed();
        if (!success) {
            esb[1] += tempElapsed;
            esbc[1] += 1;
        } else {
            ssb[1] += tempElapsed;
        }

        // synchronous benchmark 3 - retrieve all contact ids.
        timer.start();
        QList<QContactLocalId> allIds = cm->contactIds();
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[2] += tempElapsed;
            esbc[2] += 1;
        } else {
            ssb[2] += tempElapsed;
        }

        // synchronous benchmark 4 - fetching contactids with filter.
        timer.start();
        QList<QContactLocalId> ids = cm->contactIds(df);
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[3] += tempElapsed;
            esbc[3] += 1;
        } else {
            ssb[3] += tempElapsed;
        }

        // synchronous benchmark 5 - fetching contactids with filter, sorted.
        timer.start();
        ids = cm->contactIds(df, sorting);
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[4] += tempElapsed;
            esbc[4] += 1;
        } else {
            ssb[4] += tempElapsed;
        }

        // synchronous benchmark 6 - fetching all contacts.
        tempList.clear();
        timer.start();
        tempList = cm->contacts();
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[5] += tempElapsed;
            esbc[5] += 1;
        } else {
            ssb[5] += tempElapsed;
        }

        // synchronous benchmark 7 - fetching contacts with filter
        tempList.clear();
        timer.start();
        tempList = cm->contacts(df);
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[6] += tempElapsed;
            esbc[6] += 1;
        } else {
            ssb[6] += tempElapsed;
        }

        // synchronous benchmark 8 - fetching contacts with filter, sorted
        tempList.clear();
        timer.start();
        tempList = cm->contacts(df, sorting);
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[7] += tempElapsed;
            esbc[7] += 1;
        } else {
            ssb[7] += tempElapsed;
        }

        // synchronous benchmark 9 - removing 100 contacts
        ids.clear();
        ids = cm->contactIds(df);
        timer.start();
        success = cm->removeContacts(ids, &errorMap);
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[8] += tempElapsed;
            esbc[8] += 1;
        } else {
            ssb[8] += tempElapsed;
        }

        // synchronous benchmark 10 - removing all contacts
        ids.clear();
        ids = cm->contactIds();
        timer.start();
        success = cm->removeContacts(ids, &errorMap);
        tempElapsed = timer.elapsed();
        if (cm->error() != QContactManager::NoError) {
            esb[9] += tempElapsed;
            esbc[9] += 1;
        } else {
            ssb[9] += tempElapsed;
        }

        // asynchronous benchmark 1 - save 500 new contacts.
        QContactSaveRequest csr;
        csr.setManager(cm);
        tempList = contacts;
        tempList.swap(0,1); // force detach, we don't want that to contaminate benchmark.
        csr.setContacts(tempList);
        timer.start();
        csr.start();
        success = csr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (csr.error() != QContactManager::NoError || !success) {
            eab[0] += tempElapsed;
            eabc[0] += 1;
        } else {
            sab[0] += tempElapsed;
        }

        // asynchronous benchmark 2 - update 500 existing contacts.
        for (int j = 0; j < tempList.size(); ++j) {
            QContact tempContact = tempList.at(j);
            QContactName tempName = tempContact.detail<QContactName>();
            tempName.setMiddleName("updated");
            tempContact.saveDetail(&tempName);
            tempList.replace(j, tempContact);
        }
        csr.setContacts(tempList);
        timer.start();
        csr.start();
        success = csr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (csr.error() != QContactManager::NoError || !success) {
            eab[1] += tempElapsed;
            eabc[1] += 1;
        } else {
            sab[1] += tempElapsed;
        }

        // asynchronous benchmark 3 - retrieve all contact ids.
        QContactLocalIdFetchRequest cifr;
        cifr.setManager(cm);
        timer.start();
        cifr.start();
        success = cifr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (cifr.error() != QContactManager::NoError || !success) {
            eab[2] += tempElapsed;
            eabc[2] += 1;
        } else {
            sab[2] += tempElapsed;
        }

        // asynchronous benchmark 4 - retrieve contact ids matching filter.
        cifr.setFilter(df);
        timer.start();
        cifr.start();
        success = cifr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (cifr.error() != QContactManager::NoError || !success) {
            eab[3] += tempElapsed;
            eabc[3] += 1;
        } else {
            sab[3] += tempElapsed;
        }

        // asynchronous benchmark 5 - retrieve contact ids matching filter, sorted.
        cifr.setFilter(df);
        cifr.setSorting(sorting);
        timer.start();
        cifr.start();
        success = cifr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (cifr.error() != QContactManager::NoError || !success) {
            eab[4] += tempElapsed;
            eabc[4] += 1;
        } else {
            sab[4] += tempElapsed;
        }

        // asynchronous benchmark 6 - retrieve all contacts.
        QContactFetchRequest cfr;
        cfr.setManager(cm);
        cfr.setFilter(QContactFilter()); // matches all.
        timer.start();
        cfr.start();
        success = cfr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (cfr.error() != QContactManager::NoError || !success) {
            eab[5] += tempElapsed;
            eabc[5] += 1;
        } else {
            sab[5] += tempElapsed;
        }

        // asynchronous benchmark 7 - retrieve contacts matching filter.
        cfr.setFilter(df);
        timer.start();
        cfr.start();
        success = cfr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (cfr.error() != QContactManager::NoError || !success) {
            eab[6] += tempElapsed;
            eabc[6] += 1;
        } else {
            sab[6] += tempElapsed;
        }

        // asynchronous benchmark 8 - retrieve contacts matching filter, sorted.
        cfr.setFilter(df);
        cfr.setSorting(sorting);
        timer.start();
        cfr.start();
        success = cfr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (cfr.error() != QContactManager::NoError || !success) {
            eab[7] += tempElapsed;
            eabc[7] += 1;
        } else {
            sab[7] += tempElapsed;
        }

        // asynchronous benchmark 9 - remove 100 selected contacts.
        ids = cm->contactIds(df);
        QContactRemoveRequest crr;
        crr.setManager(cm);
        crr.setContactIds(ids);
        timer.start();
        crr.start();
        success = crr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (crr.error() != QContactManager::NoError || !success) {
            eab[8] += tempElapsed;
            eabc[8] += 1;
        } else {
            sab[8] += tempElapsed;
        }

        // asynchronous benchmark 10 - remove all contacts.
        ids = cm->contactIds();
        crr.setContactIds(ids);
        timer.start();
        crr.start();
        success = crr.waitForFinished();
        tempElapsed = timer.elapsed();
        if (crr.error() != QContactManager::NoError || !success) {
            eab[9] += tempElapsed;
            eabc[9] += 1;
        } else {
            sab[9] += tempElapsed;
        }
    }

    quint64 succeededCount, failedCount, averageSuccessTime, averageFailTime;
    QString outputMessage = QString("\nBenchmark results for manager: %1\n  Synchronous:").arg(cm->managerUri());

    for (int i = 0; i < 10; ++i) {
        succeededCount = AGGREGATION_LIMIT - esbc[i];
        failedCount = esbc[i];
        if (succeededCount)
            averageSuccessTime = (ssb[i] / succeededCount);
        else
            averageSuccessTime = -1;
        if (failedCount)
            averageFailTime = (esb[i] / failedCount);
        else
            averageFailTime = -1;
        QString tempStr = QString("%1:").arg(benchmarkDescriptions.at(i));
        int whitespaceAdjust = largestDescriptionLength - benchmarkDescriptions.at(i).length();
        for (int j = 0; j < whitespaceAdjust; ++j) {
            tempStr += " ";
        }
        tempStr += QString("  succeeded: %1  average_time(success): %2  failed: %3  average_time(failed): %4").arg(succeededCount).arg(averageSuccessTime).arg(failedCount).arg(averageFailTime);
        if (i == 9) {
            outputMessage += QString(QLatin1String("\n    (")) + QString::number(i+1) + QString(QLatin1String(") "));
        } else {
            outputMessage += QString(QLatin1String("\n     (")) + QString::number(i+1) + QString(QLatin1String(") "));
        }
        outputMessage += tempStr;
    }

    outputMessage += QLatin1String("\n  Asynchronous:");
    for (int i = 0; i < 10; ++i) {
        succeededCount = AGGREGATION_LIMIT - eabc[i];
        failedCount = eabc[i];
        if (succeededCount)
            averageSuccessTime = (sab[i] / succeededCount);
        else
            averageSuccessTime = -1;
        if (failedCount)
            averageFailTime = (sab[i] / failedCount);
        else
            averageFailTime = -1;
        QString tempStr = QString("%1:").arg(benchmarkDescriptions.at(i));
        int whitespaceAdjust = largestDescriptionLength - benchmarkDescriptions.at(i).length();
        for (int j = 0; j < whitespaceAdjust; ++j) {
            tempStr += " ";
        }
        tempStr += QString("  succeeded: %1  average_time(success): %2  failed: %3 average_time(failed): %4").arg(succeededCount).arg(averageSuccessTime).arg(failedCount).arg(averageFailTime);
        if (i == 9) {
            outputMessage += QString(QLatin1String("\n    (")) + QString::number(i+1) + QString(QLatin1String(") "));
        } else {
            outputMessage += QString(QLatin1String("\n     (")) + QString::number(i+1) + QString(QLatin1String(") "));
        }
        outputMessage += tempStr;
    }

    outputMessage += "\n(All times measured in milliseconds).";
    qDebug() << outputMessage;
    delete cm;
}

QTEST_MAIN(tst_QContactManager)
#include "tst_qcontactmanager.moc"
