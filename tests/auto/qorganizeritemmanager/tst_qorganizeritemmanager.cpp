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

#include "qtorganizer.h"
#include "qorganizeritemchangeset.h"
#include "qorganizeritemmanagerdataholder.h"

#include "qorganizernote.h"
#include "qorganizerevent.h"

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

#define QORGANIZERITEMMANAGER_REMOVE_VERSIONS_FROM_URI(params)  params.remove(QString::fromAscii(QTORGANIZER_VERSION_NAME)); \
                                                          params.remove(QString::fromAscii(QTORGANIZER_IMPLEMENTATION_VERSION_NAME))

//TESTED_CLASS=
//TESTED_FILES=

// to get QFETCH to work with the template expression...
typedef QMap<QString,QString> tst_QOrganizerItemManager_QStringMap;
Q_DECLARE_METATYPE(tst_QOrganizerItemManager_QStringMap)
Q_DECLARE_METATYPE(QList<QOrganizerItemLocalId>)

/* A class that no backend can support */
class UnsupportedMetatype {
    int foo;
};
Q_DECLARE_METATYPE(UnsupportedMetatype)
Q_DECLARE_METATYPE(QOrganizerItem)
Q_DECLARE_METATYPE(QOrganizerItemManager::Error)
Q_DECLARE_METATYPE(QOrganizerItemRecurrenceRule)
Q_DECLARE_METATYPE(QList<QDate>)

class tst_QOrganizerItemManager : public QObject
{
Q_OBJECT

public:
    tst_QOrganizerItemManager();
    virtual ~tst_QOrganizerItemManager();

private:
    void dumpOrganizerItemDifferences(const QOrganizerItem& a, const QOrganizerItem& b);
    void dumpOrganizerItem(const QOrganizerItem &c);
    void dumpOrganizerItems(QOrganizerItemManager *cm);
    bool isSuperset(const QOrganizerItem& ca, const QOrganizerItem& cb);
    QList<QOrganizerItemDetail> removeAllDefaultDetails(const QList<QOrganizerItemDetail>& details);
    void addManagers(); // add standard managers to the data

    QScopedPointer<QOrganizerItemManagerDataHolder> managerDataHolder;

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
    void compatibleItem();
    void recurrenceWithGenerator();

    /* Tests that are run on all managers */
    void metadata();
    void nullIdOperations();
    void add();
    void addOccurrenceExceptionsWithGuid();
    void update();
    void remove();
    void batch();
    void signalEmission();
    void detailDefinitions();
    void detailOrders();
    void itemType();

    /* Tests that take no data */
    void itemValidation();
    void errorStayingPut();
    void ctors();
    void invalidManager();
    void memoryManager();
    void changeSet();
    void fetchHint();

    /* Special test with special data */
    void uriParsing_data();
    void compatibleItem_data();
    void recurrenceWithGenerator_data();
    /* Tests that are run on all managers */
    void metadata_data() {addManagers();}
    void nullIdOperations_data() {addManagers();}
    void add_data() {addManagers();}
    void addOccurrenceExceptionsWithGuid_data() {addManagers();}
    void update_data() {addManagers();}
    void remove_data() {addManagers();}
    void batch_data() {addManagers();}
    void signalEmission_data() {addManagers();}
    void detailDefinitions_data() {addManagers();}
    void detailOrders_data() {addManagers();}
    void itemType_data() {addManagers();}
};

tst_QOrganizerItemManager::tst_QOrganizerItemManager()
{
}

tst_QOrganizerItemManager::~tst_QOrganizerItemManager()
{
}

void tst_QOrganizerItemManager::initTestCase()
{
    managerDataHolder.reset(new QOrganizerItemManagerDataHolder());

    /* Make sure these other test plugins are NOT loaded by default */
    // These are now removed from the list of managers in addManagers()
    //QVERIFY(!QOrganizerItemManager::availableManagers().contains("testdummy"));
    //QVERIFY(!QOrganizerItemManager::availableManagers().contains("teststaticdummy"));
    //QVERIFY(!QOrganizerItemManager::availableManagers().contains("maliciousplugin"));
}

void tst_QOrganizerItemManager::cleanupTestCase()
{
    managerDataHolder.reset(0);
}

void tst_QOrganizerItemManager::dumpOrganizerItemDifferences(const QOrganizerItem& ca, const QOrganizerItem& cb)
{
    // Try to narrow down the differences
    QOrganizerItem a(ca);
    QOrganizerItem b(cb);

    // Check the display label
    QCOMPARE(a.displayLabel(), b.displayLabel());

    // Now look at the rest
    QList<QOrganizerItemDetail> aDetails = a.details();
    QList<QOrganizerItemDetail> bDetails = b.details();

    // They can be in any order, so loop
    // First remove any matches
    foreach(QOrganizerItemDetail d, aDetails) {
        foreach(QOrganizerItemDetail d2, bDetails) {
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
    foreach(QOrganizerItemDetail d, aDetails) {
        if (d.definitionName() != QOrganizerItemDisplayLabel::DefinitionName && d.definitionName() != QOrganizerItemType::DefinitionName)
            qDebug() << "A item had extra detail:" << d.definitionName() << d.variantValues();
    }
    // and same for B
    foreach(QOrganizerItemDetail d, bDetails) {
        if (d.definitionName() != QOrganizerItemDisplayLabel::DefinitionName && d.definitionName() != QOrganizerItemType::DefinitionName)
            qDebug() << "B item had extra detail:" << d.definitionName() << d.variantValues();
    }

    // now test specifically the display label and the type
    if (a.displayLabel() != b.displayLabel()) {
        qDebug() << "A item display label =" << a.displayLabel();
        qDebug() << "B item display label =" << b.displayLabel();
    }
    if (a.type() != b.type()) {
        qDebug() << "A item type =" << a.type();
        qDebug() << "B item type =" << b.type();
    }
}

bool tst_QOrganizerItemManager::isSuperset(const QOrganizerItem& ca, const QOrganizerItem& cb)
{
    // returns true if item ca is a superset of item cb
    // we use this test instead of equality because dynamic information
    // such as presence/location, and synthesised information such as
    // display label and (possibly) type, may differ between a item
    // in memory and the item in the managed store.

    QOrganizerItem a(ca);
    QOrganizerItem b(cb);
    QList<QOrganizerItemDetail> aDetails = a.details();
    QList<QOrganizerItemDetail> bDetails = b.details();

    // They can be in any order, so loop
    // First remove any matches
    foreach(QOrganizerItemDetail d, aDetails) {
        foreach(QOrganizerItemDetail d2, bDetails) {
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
    foreach (QOrganizerItemDetail d, aDetails) {
        foreach (QOrganizerItemDetail d2, bDetails) {
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

    // check for item type updates
    if (!a.type().isEmpty())
        if (!b.type().isEmpty())
            if (a.type() != b.type())
                return false; // nonempty type is different.

    // Now check to see if b has any details remaining; if so, a is not a superset.
    // Note that the DisplayLabel and Type can never be removed.
    if (b.details().size() > 2
            || (b.details().size() == 2 && (b.details().value(0).definitionName() != QOrganizerItemDisplayLabel::DefinitionName
                                            || b.details().value(1).definitionName() != QOrganizerItemType::DefinitionName)))
        return false;
    return true;
}

void tst_QOrganizerItemManager::dumpOrganizerItem(const QOrganizerItem& item)
{
    QOrganizerItemManager m;
    qDebug() << "OrganizerItem: " << item.id().localId() << "(" << item.displayLabel() << ")";
    QList<QOrganizerItemDetail> details = item.details();
    foreach(QOrganizerItemDetail d, details) {
        qDebug() << "  " << d.definitionName() << ":";
        qDebug() << "    Vals:" << d.variantValues();
    }
}

void tst_QOrganizerItemManager::dumpOrganizerItems(QOrganizerItemManager *cm)
{
    QList<QOrganizerItemLocalId> ids = cm->itemIds();

    qDebug() << "There are" << ids.count() << "items in" << cm->managerUri();

    foreach(QOrganizerItemLocalId id, ids) {
        QOrganizerItem c = cm->item(id);
        dumpOrganizerItem(c);
    }
}

void tst_QOrganizerItemManager::uriParsing_data()
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

    QTest::newRow("built") << QOrganizerItemManager::buildUri("manager", inparameters) << true << "manager" << inparameters;
    QTest::newRow("built with escaped parameters") << QOrganizerItemManager::buildUri("manager", inparameters2) << true << "manager" << inparameters2;
    QTest::newRow("no scheme") << "this should not split" << false << QString() << tst_QOrganizerItemManager_QStringMap();
    QTest::newRow("wrong scheme") << "invalidscheme:foo bar" << false << QString() << tst_QOrganizerItemManager_QStringMap();
    QTest::newRow("right scheme, no colon") << "qtorganizer" << false << QString() << tst_QOrganizerItemManager_QStringMap();
    QTest::newRow("no manager, colon, no params") << "qtorganizer::" << false  << "manager" << tst_QOrganizerItemManager_QStringMap();
    QTest::newRow("yes manager, no colon, no params") << "qtorganizer:manager" << true << "manager" << tst_QOrganizerItemManager_QStringMap();
    QTest::newRow("yes manager, yes colon, no params") << "qtorganizer:manager:" << true << "manager"<< tst_QOrganizerItemManager_QStringMap();
    QTest::newRow("yes params") << "qtorganizer:manager:foo=bar&bazflag=&bar=glob" << true << "manager" << inparameters;
    QTest::newRow("yes params but misformed") << "qtorganizer:manager:foo=bar&=gloo&bar=glob" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 2") << "qtorganizer:manager:=&=gloo&bar=glob" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 3") << "qtorganizer:manager:==" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 4") << "qtorganizer:manager:&&" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 5") << "qtorganizer:manager:&goo=bar" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 6") << "qtorganizer:manager:goo&bar" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 7") << "qtorganizer:manager:goo&bar&gob" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 8") << "qtorganizer:manager:==&&==&goo=bar" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 9") << "qtorganizer:manager:foo=bar=baz" << false << "manager" << inparameters;
    QTest::newRow("yes params but misformed 10") << "qtorganizer:manager:foo=bar=baz=glob" << false << "manager" << inparameters;
    QTest::newRow("no manager but yes params") << "qtorganizer::foo=bar&bazflag=&bar=glob" << false << QString() << inparameters;
    QTest::newRow("no manager or params") << "qtorganizer::" << false << QString() << inparameters;
    QTest::newRow("no manager or params or colon") << "qtorganizer:" << false << QString() << inparameters;
}

void tst_QOrganizerItemManager::addManagers()
{
    QTest::addColumn<QString>("uri");

    QStringList managers = QOrganizerItemManager::availableManagers();

    /* Known one that will not pass */
    managers.removeAll("invalid");
    managers.removeAll("testdummy");
    managers.removeAll("teststaticdummy");
    managers.removeAll("maliciousplugin");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QOrganizerItemManager::buildUri(mgr, params);
        if (mgr == "memory") {
            params.insert("id", "tst_QOrganizerItemManager");
            QTest::newRow(QString("mgr='%1', params").arg(mgr).toLatin1().constData()) << QOrganizerItemManager::buildUri(mgr, params);
        }
    }
}

void tst_QOrganizerItemManager::metadata()
{
    // ensure that the backend is publishing its metadata (name / parameters / uri) correctly
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(new QOrganizerItemManager("memory"));
    QVERIFY(QOrganizerItemManager::buildUri(cm->managerName(), cm->managerParameters()) == cm->managerUri());
}


void tst_QOrganizerItemManager::nullIdOperations()
{
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(new QOrganizerItemManager("memory"));
    QVERIFY(!cm->removeItem(QOrganizerItemLocalId()));
    QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);


    QOrganizerItem c = cm->item(QOrganizerItemLocalId());
    QVERIFY(c.id() == QOrganizerItemId());
    QVERIFY(c.isEmpty());
    QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);
}

void tst_QOrganizerItemManager::uriParsing()
{
    QFETCH(QString, uri);
    QFETCH(bool, good);
    QFETCH(QString, manager);
    QFETCH(tst_QOrganizerItemManager_QStringMap, parameters);

    QString outmanager;
    QMap<QString, QString> outparameters;

    if (good) {
        /* Good split */
        /* Test splitting */
        QVERIFY(QOrganizerItemManager::parseUri(uri, 0, 0)); // no out parms

        // 1 out param
        QVERIFY(QOrganizerItemManager::parseUri(uri, &outmanager, 0));
        QCOMPARE(manager, outmanager);
        QVERIFY(QOrganizerItemManager::parseUri(uri, 0, &outparameters));

        QORGANIZERITEMMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);

        QCOMPARE(parameters, outparameters);

        outmanager.clear();
        outparameters.clear();
        QVERIFY(QOrganizerItemManager::parseUri(uri, &outmanager, &outparameters));

        QORGANIZERITEMMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);

        QCOMPARE(manager, outmanager);
        QCOMPARE(parameters, outparameters);
    } else {
        /* bad splitting */
        outmanager.clear();
        outparameters.clear();
        QVERIFY(QOrganizerItemManager::parseUri(uri, 0, 0) == false);
        QVERIFY(QOrganizerItemManager::parseUri(uri, &outmanager, 0) == false);
        QVERIFY(outmanager.isEmpty());
        QVERIFY(QOrganizerItemManager::parseUri(uri, 0, &outparameters) == false);
        QORGANIZERITEMMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);
        QVERIFY(outparameters.isEmpty());

        /* make sure the in parameters don't change with a bad split */
        outmanager = manager;
        outparameters = parameters;
        QVERIFY(QOrganizerItemManager::parseUri(uri, &outmanager, 0) == false);
        QCOMPARE(manager, outmanager);
        QVERIFY(QOrganizerItemManager::parseUri(uri, 0, &outparameters) == false);
        QORGANIZERITEMMANAGER_REMOVE_VERSIONS_FROM_URI(outparameters);
        QCOMPARE(parameters, outparameters);
    }
}

void tst_QOrganizerItemManager::ctors()
{
    /* test the different ctors to make sure we end up with the same uri */
    QVERIFY(QOrganizerItemManager::availableManagers().count() > 1); // invalid + something else
    QVERIFY(QOrganizerItemManager::availableManagers().contains("invalid"));
    QString defaultStore = QOrganizerItemManager::availableManagers().value(0);
    QVERIFY(defaultStore != "invalid");

    qDebug() << "Available managers:" << QOrganizerItemManager::availableManagers();

    QMap<QString, QString> randomParameters;
    randomParameters.insert("something", "old");
    randomParameters.insert("something...", "new");
    randomParameters.insert("something ", "borrowed");
    randomParameters.insert(" something", "blue");

    QObject parent;

    QOrganizerItemManager cm; // default
    QOrganizerItemManager cm2(defaultStore);
    QOrganizerItemManager cm3(defaultStore, QMap<QString, QString>());
    QOrganizerItemManager cm4(cm.managerUri()); // should fail

    QScopedPointer<QOrganizerItemManager> cm5(QOrganizerItemManager::fromUri(QOrganizerItemManager::buildUri(defaultStore, QMap<QString, QString>())));
    QScopedPointer<QOrganizerItemManager> cm6(QOrganizerItemManager::fromUri(cm.managerUri())); // uri is not a name; should fail.
    QScopedPointer<QOrganizerItemManager> cm9(QOrganizerItemManager::fromUri(QString(), &parent));

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
    QOrganizerItemManager em("non existent");
    QOrganizerItemManager em2("non existent", QMap<QString, QString>());
    QOrganizerItemManager em3("memory", randomParameters);

    /* Also invalid, since we don't have one of these anyway */
    QScopedPointer<QOrganizerItemManager> em4(QOrganizerItemManager::fromUri("invalid uri"));
    QScopedPointer<QOrganizerItemManager> em5(QOrganizerItemManager::fromUri(QOrganizerItemManager::buildUri("nonexistent", QMap<QString, QString>())));
    QScopedPointer<QOrganizerItemManager> em6(QOrganizerItemManager::fromUri(em3.managerUri()));


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
    QCOMPARE(em.managerParameters(), tst_QOrganizerItemManager_QStringMap());
    QCOMPARE(em2.managerParameters(), tst_QOrganizerItemManager_QStringMap());
    QCOMPARE(em4->managerParameters(), tst_QOrganizerItemManager_QStringMap());
    QCOMPARE(em5->managerParameters(), tst_QOrganizerItemManager_QStringMap());
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

void tst_QOrganizerItemManager::doDump()
{
    // Only do this if it has been explicitly selected
    if (QCoreApplication::arguments().contains("doDump")) {
        QFETCH(QString, uri);
        QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

        dumpOrganizerItems(cm.data());
    }
}

Q_DECLARE_METATYPE(QVariant)

void tst_QOrganizerItemManager::doDumpSchema()
{
    // Only do this if it has been explicitly selected
    if (QCoreApplication::arguments().contains("doDumpSchema")) {
        QFETCH(QString, uri);
        QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

        // Get the schema for each supported type
        foreach(QString type, cm->supportedItemTypes()) {
            QMap<QString, QOrganizerItemDetailDefinition> defs = cm->detailDefinitions(type);

            foreach(QOrganizerItemDetailDefinition def, defs.values()) {
                if (def.isUnique())
                    qDebug() << QString("%2::%1 (Unique) {").arg(def.name()).arg(type).toAscii().constData();
                else
                    qDebug() << QString("%2::%1 {").arg(def.name()).arg(type).toAscii().constData();
                QMap<QString, QOrganizerItemDetailFieldDefinition> fields = def.fields();

                foreach(QString fname, fields.keys()) {
                    QOrganizerItemDetailFieldDefinition field = fields.value(fname);

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

void tst_QOrganizerItemManager::add()
{
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

    QOrganizerNote note;
    note.setDisplayLabel("This is a note");
    note.setDescription("This note is a particularly notey note");
    int currCount = cm->itemIds().count();
    QVERIFY(cm->saveItem(&note));
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);

    QVERIFY(!note.id().managerUri().isEmpty());
    QVERIFY(note.id().localId() != 0);
    QCOMPARE(cm->itemIds().count(), currCount+1);

    QOrganizerItem added = cm->item(note.id().localId());
    QVERIFY(added.id() == note.id());
    
    if (!isSuperset(added, note)) {
        // XXX TODO: fix the isSuperset so that it ignores timestamps.
        //dumpOrganizerItems(cm.data());
        //dumpOrganizerItemDifferences(added, note);
        QCOMPARE(added, QOrganizerItem(note));
    }

    // now try adding an item that does not exist in the database with non-zero id
    QOrganizerNote nonexistent;
    nonexistent.setDisplayLabel("Another note.");
    nonexistent.setDescription("This is `another note`'s description");
    QVERIFY(cm->saveItem(&nonexistent));       // should work
    QVERIFY(cm->removeItem(nonexistent.id().localId())); // now nonexistent has an id which does not exist
    QVERIFY(!cm->saveItem(&nonexistent));      // hence, should fail
    QCOMPARE(cm->error(), QOrganizerItemManager::DoesNotExistError);
    nonexistent.setId(QOrganizerItemId());
    QVERIFY(cm->saveItem(&nonexistent));       // after setting id to zero, should save
    QVERIFY(cm->removeItem(nonexistent.id().localId()));

    // now try adding a "megaevent"
    // - get list of all definitions supported by the manager
    // - add one detail of each definition to a item
    // - save the item
    // - read it back
    // - ensure that it's the same.
    QOrganizerEvent megaevent;
    QMap<QString, QOrganizerItemDetailDefinition> defmap = cm->detailDefinitions();
    QList<QOrganizerItemDetailDefinition> defs = defmap.values();
    foreach (const QOrganizerItemDetailDefinition def, defs) {

        // This is probably read-only
        if (def.name() == QOrganizerItemTimestamp::DefinitionName)
            continue;

        // otherwise, create a new detail of the given type and save it to the item
        QOrganizerItemDetail det(def.name());
        QMap<QString, QOrganizerItemDetailFieldDefinition> fieldmap = def.fields();
        QStringList fieldKeys = fieldmap.keys();
        foreach (const QString& fieldKey, fieldKeys) {
            // get the field, and check to see that it's not constrained.
            QOrganizerItemDetailFieldDefinition currentField = fieldmap.value(fieldKey);

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
            megaevent.saveDetail(&det);
    }

    QVERIFY(cm->saveItem(&megaevent)); // must be able to save since built from definitions.
    QOrganizerItem retrievedMegaitem = cm->item(megaevent.id().localId());
    if (!isSuperset(retrievedMegaitem, megaevent)) {
        dumpOrganizerItemDifferences(megaevent, retrievedMegaitem);
        QEXPECT_FAIL("mgr='wince'", "Address Display Label mismatch", Continue);
        
    }

    // now a item with many details of a particular definition
    // if the detail is not unique it should then support minumum of two of the same kind
    // const int nrOfdetails = 2;
    // XXX TODO.
}

void tst_QOrganizerItemManager::addOccurrenceExceptionsWithGuid()
{
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

    QOrganizerEvent christmas;
    christmas.setGuid("christmas");
    christmas.setStartDateTime(QDateTime(QDate(2009, 12, 25), QTime(0, 0, 0)));
    christmas.setStartDateTime(QDateTime(QDate(2009, 12, 26), QTime(0, 0, 0)));
    christmas.setDisplayLabel(QLatin1String("Christmas"));
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    christmas.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << rrule);
    QVERIFY(cm->saveItem(&christmas));
    QVERIFY(!christmas.id().managerUri().isEmpty());
    QVERIFY(christmas.id().localId() != 0);

    QOrganizerEvent newYearsDay;
    newYearsDay.setGuid("newyear");
    newYearsDay.setStartDateTime(QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)));
    newYearsDay.setStartDateTime(QDateTime(QDate(2010, 1, 2), QTime(0, 0, 0)));
    newYearsDay.setDisplayLabel(QLatin1String("New Years Day"));
    newYearsDay.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << rrule);
    QVERIFY(cm->saveItem(&newYearsDay));

    QOrganizerTodo report;
    report.setGuid("report");
    report.setDueDateTime(QDateTime(QDate(2010, 1, 11), QTime(0, 0, 0)));
    report.setDisplayLabel(QLatin1String("Report"));
    QVERIFY(cm->saveItem(&report));

    // exception with no guid fails
    QOrganizerEventOccurrence exception;
    exception.setStartDateTime(QDateTime(QDate(2010, 12, 25), QTime(0, 0, 0)));
    exception.setStartDateTime(QDateTime(QDate(2010, 12, 26), QTime(0, 0, 0)));
    exception.setDisplayLabel(QLatin1String("Christmas"));
    exception.addComment(QLatin1String("With the in-laws"));
    QVERIFY(!cm->saveItem(&exception));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // exception with invalid guid fails
    exception.setGuid(QLatin1String("halloween"));
    QVERIFY(!cm->saveItem(&exception));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidOccurrenceError);
    
    // with the guid set, it should work
    exception.setGuid(QLatin1String("christmas"));
    QVERIFY(cm->saveItem(&exception));
    QOrganizerEventOccurrence savedException = cm->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId());

    // with the guid and the parentId both set and consistent, it should work
    exception = savedException;
    QVERIFY(cm->saveItem(&exception));
    savedException = cm->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), christmas.localId());

    // with the guid inconsistent with the parentId, it should fail
    exception.setParentLocalId(newYearsDay.localId());
    QVERIFY(!cm->saveItem(&exception));

    // with just the parentId set, it should work
    exception.setGuid(QLatin1String(""));
    QVERIFY(cm->saveItem(&exception));
    savedException = cm->item(exception.localId());
    QCOMPARE(savedException.parentLocalId(), newYearsDay.localId());
    QCOMPARE(savedException.guid(), QLatin1String("newyear"));

    // can't set parentId to a non-event
    exception.setGuid(QLatin1String(""));
    exception.setParentLocalId(report.localId());
    QVERIFY(!cm->saveItem(&exception));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidOccurrenceError);

    // can't set guid to a non-event
    exception.setGuid(QLatin1String("report"));
    exception.setParentLocalId(0);
    QVERIFY(!cm->saveItem(&exception));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidOccurrenceError);
}

void tst_QOrganizerItemManager::update()
{
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

    /* Save a new item first */
    QOrganizerNote note;
    note.setDisplayLabel("Yet another note");
    note.setDescription("Surprisingly, this note is not a particularly notey note");
    QVERIFY(cm->saveItem(&note));
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);

    /* Update name */
    QOrganizerItemDescription descr = note.detail(QOrganizerItemDescription::DefinitionName);
    descr.setDescription("This note is now slightly noteworthy");
    note.saveDetail(&descr);
    QVERIFY(cm->saveItem(&note));
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);
    descr.setDescription("This is a very noteworthy note");
    note.saveDetail(&descr);
    QVERIFY(cm->saveItem(&note));
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);
    QOrganizerItem updated = cm->item(note.localId());
    QOrganizerItemDescription updatedDescr = updated.detail(QOrganizerItemDescription::DefinitionName);
    QCOMPARE(updatedDescr, descr);
}

void tst_QOrganizerItemManager::remove()
{
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

    /* Save a new item first */
    QOrganizerItem note;
    note.setDisplayLabel("Not another note");
    note.setDescription("Yes, another note!");
    QVERIFY(cm->saveItem(&note));
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);
    QVERIFY(note.id() != QOrganizerItemId());

    /* Remove the created item */
    const int itemCount = cm->itemIds().count();
    QVERIFY(cm->removeItem(note.localId()));
    QCOMPARE(cm->itemIds().count(), itemCount - 1);
    QVERIFY(cm->item(note.localId()).isEmpty());
    QCOMPARE(cm->error(), QOrganizerItemManager::DoesNotExistError);
}

void tst_QOrganizerItemManager::batch()
{
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

    /* First test null pointer operations */
    QVERIFY(!cm->saveItems(NULL, NULL));
    QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

    QVERIFY(!cm->removeItems(QList<QOrganizerItemLocalId>(), NULL));
    QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

    /* Now add 3 items, all valid */
    QOrganizerNote a;
    QOrganizerItemDisplayLabel da;
    da.setValue(QOrganizerItemDisplayLabel::FieldLabel, "XXXXXX A Note");
    a.saveDetail(&da);

    QOrganizerNote b;
    QOrganizerItemDisplayLabel db;
    db.setValue(QOrganizerItemDisplayLabel::FieldLabel, "XXXXXX B Note");
    b.saveDetail(&db);

    QOrganizerNote c;
    QOrganizerItemDisplayLabel dc;
    dc.setValue(QOrganizerItemDisplayLabel::FieldLabel, "XXXXXX C Note");
    c.saveDetail(&dc);

    QList<QOrganizerItem> items;
    items << a << b << c;

    QMap<int, QOrganizerItemManager::Error> errorMap;
    // Add one dummy error to test if the errors are reset
    errorMap.insert(0, QOrganizerItemManager::NoError);
    QVERIFY(cm->saveItems(&items, &errorMap));
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);

    /* Make sure our items got updated too */
    QVERIFY(items.count() == 3);
    QVERIFY(items.at(0).id() != QOrganizerItemId());
    QVERIFY(items.at(1).id() != QOrganizerItemId());
    QVERIFY(items.at(2).id() != QOrganizerItemId());

    QVERIFY(items.at(0).detail(QOrganizerItemDisplayLabel::DefinitionName) == da);
    QVERIFY(items.at(1).detail(QOrganizerItemDisplayLabel::DefinitionName) == db);
    QVERIFY(items.at(2).detail(QOrganizerItemDisplayLabel::DefinitionName) == dc);

    /* Retrieve again */
    a = cm->item(items.at(0).id().localId());
    b = cm->item(items.at(1).id().localId());
    c = cm->item(items.at(2).id().localId());
    QVERIFY(items.at(0).detail(QOrganizerItemDisplayLabel::DefinitionName) == da);
    QVERIFY(items.at(1).detail(QOrganizerItemDisplayLabel::DefinitionName) == db);
    QVERIFY(items.at(2).detail(QOrganizerItemDisplayLabel::DefinitionName) == dc);

    /* Now make an update to them all */
    QOrganizerItemDescription descr;
    descr.setDescription("This note looks slightly shifty");

    QVERIFY(items[0].saveDetail(&descr));
    descr.setDescription("This note is definitely up to no good");
    QVERIFY(items[1].saveDetail(&descr));
    descr.setDescription("This note is a terrible note");
    QVERIFY(items[2].saveDetail(&descr));

    QVERIFY(cm->saveItems(&items, &errorMap));
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);

    /* Retrieve them and check them again */
    a = cm->item(items.at(0).id().localId());
    b = cm->item(items.at(1).id().localId());
    c = cm->item(items.at(2).id().localId());
    QVERIFY(items.at(0).detail(QOrganizerItemDisplayLabel::DefinitionName) == da);
    QVERIFY(items.at(1).detail(QOrganizerItemDisplayLabel::DefinitionName) == db);
    QVERIFY(items.at(2).detail(QOrganizerItemDisplayLabel::DefinitionName) == dc);

    QVERIFY(a.details<QOrganizerItemDescription>().count() == 1);
    QVERIFY(b.details<QOrganizerItemDescription>().count() == 1);
    QVERIFY(c.details<QOrganizerItemDescription>().count() == 1);

    QVERIFY(a.details<QOrganizerItemDescription>().at(0).description() == "This note looks slightly shifty");
    QVERIFY(b.details<QOrganizerItemDescription>().at(0).description() == "This note is definitely up to no good");
    QVERIFY(c.details<QOrganizerItemDescription>().at(0).description() == "This note is a terrible note");

    /* Now delete them all */
    QList<QOrganizerItemLocalId> ids;
    ids << a.id().localId() << b.id().localId() << c.id().localId();
    QVERIFY(cm->removeItems(ids, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);

    /* Make sure the items really don't exist any more */
    QVERIFY(cm->item(a.id().localId()).id() == QOrganizerItemId());
    QVERIFY(cm->item(a.id().localId()).isEmpty());
    QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(cm->item(b.id().localId()).id() == QOrganizerItemId());
    QVERIFY(cm->item(b.id().localId()).isEmpty());
    QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(cm->item(c.id().localId()).id() == QOrganizerItemId());
    QVERIFY(cm->item(c.id().localId()).isEmpty());
    QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);

    /* Now try removing with all invalid ids (e.g. the ones we just removed) */
    ids.clear();
    ids << a.id().localId() << b.id().localId() << c.id().localId();
    QVERIFY(!cm->removeItems(ids, &errorMap));
    QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(errorMap.count() == 3);
    QVERIFY(errorMap.values().at(0) == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(errorMap.values().at(1) == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(errorMap.values().at(2) == QOrganizerItemManager::DoesNotExistError);

    /* Try adding some new ones again, this time one with an error */
    items.clear();
    a.setId(QOrganizerItemId());
    b.setId(QOrganizerItemId());
    c.setId(QOrganizerItemId());

    /* Make B the bad guy */
    QOrganizerItemDetail bad("does not exist and will break if you add it");
    bad.setValue("This is also bad", "Very bad");
    b.saveDetail(&bad);

    items << a << b << c;
    QVERIFY(!cm->saveItems(&items, &errorMap));
    /* We can't really say what the error will be.. maybe bad argument, maybe invalid detail */
    QVERIFY(cm->error() != QOrganizerItemManager::NoError);

    /* It's permissible to fail all the adds, or to add the successful ones */
    QVERIFY(errorMap.count() > 0);
    QVERIFY(errorMap.count() <= 3);

    // A might have gone through
    if (errorMap.keys().contains(0)) {
        QVERIFY(errorMap.value(0) != QOrganizerItemManager::NoError);
        QVERIFY(items.at(0).id() == QOrganizerItemId());
    } else {
        QVERIFY(items.at(0).id() != QOrganizerItemId());
    }

    // B should have failed
    QVERIFY(errorMap.value(1) == QOrganizerItemManager::InvalidDetailError);
    QVERIFY(items.at(1).id() == QOrganizerItemId());

    // C might have gone through
    if (errorMap.keys().contains(2)) {
        QVERIFY(errorMap.value(2) != QOrganizerItemManager::NoError);
        QVERIFY(items.at(2).id() == QOrganizerItemId());
    } else {
        QVERIFY(items.at(2).id() != QOrganizerItemId());
    }

    /* Fix up B and re save it */
    QVERIFY(items[1].removeDetail(&bad));
    QVERIFY(cm->saveItems(&items, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(cm->error() == QOrganizerItemManager::NoError);
    
    // Save and remove a fourth item. Store the id.
    a.setId(QOrganizerItemId());
    QVERIFY(cm->saveItem(&a));
    QOrganizerItemLocalId removedId = a.localId();
    QVERIFY(cm->removeItem(removedId));

    /* Now delete 3 items, but with one bad argument */
    ids.clear();
    ids << items.at(0).id().localId();
    ids << removedId;
    ids << items.at(2).id().localId();

    QVERIFY(!cm->removeItems(ids, &errorMap));
    QVERIFY(cm->error() != QOrganizerItemManager::NoError);

    /* Again, the backend has the choice of either removing the successful ones, or not */
    QVERIFY(errorMap.count() > 0);
    QVERIFY(errorMap.count() <= 3);

    // A might have gone through
    if (errorMap.keys().contains(0)) {
        QVERIFY(errorMap.value(0) != QOrganizerItemManager::NoError);
        QVERIFY(items.at(0).id() == QOrganizerItemId());
    } else {
        QVERIFY(items.at(0).id() != QOrganizerItemId());
    }

    /* B should definitely have failed */
    QVERIFY(errorMap.value(1) == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(ids.at(1) == removedId);

    // A might have gone through
    if (errorMap.keys().contains(2)) {
        QVERIFY(errorMap.value(2) != QOrganizerItemManager::NoError);
        QVERIFY(items.at(2).id() == QOrganizerItemId());
    } else {
        QVERIFY(items.at(2).id() != QOrganizerItemId());
    }
}

void tst_QOrganizerItemManager::invalidManager()
{
    /* Create an invalid manager */
    QOrganizerItemManager manager("this should never work");
    QVERIFY(manager.managerName() == "invalid");
    QVERIFY(manager.managerVersion() == 0);

    /* also, test the other ctor behaviour is sane also */
    QOrganizerItemManager anotherManager("this should never work", 15);
    QVERIFY(anotherManager.managerName() == "invalid");
    QVERIFY(anotherManager.managerVersion() == 0);

    /* Now test that all the operations fail */
    QVERIFY(manager.itemIds().count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);

    QOrganizerItem foo;
    QOrganizerItemDisplayLabel dl;
    dl.setLabel("some label");
    foo.saveDetail(&dl);

    QVERIFY(manager.saveItem(&foo) == false);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);
    QVERIFY(foo.id() == QOrganizerItemId());
    QVERIFY(manager.itemIds().count() == 0);

    QVERIFY(manager.item(foo.id().localId()).id() == QOrganizerItemId());
    QVERIFY(manager.item(foo.id().localId()).isEmpty());
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);

    QVERIFY(manager.removeItem(foo.id().localId()) == false);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);

    QMap<int, QOrganizerItemManager::Error> errorMap;
    errorMap.insert(0, QOrganizerItemManager::NoError);
    QVERIFY(!manager.saveItems(0, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::BadArgumentError);

    /* filters */
    QOrganizerItemFilter f; // matches everything
    QOrganizerItemDetailFilter df;
    df.setDetailDefinitionName(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel);
    QVERIFY(manager.itemIds(QOrganizerItemFilter()).count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);
    QVERIFY(manager.itemIds(df).count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);
    QVERIFY(manager.itemIds(f | f).count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);
    QVERIFY(manager.itemIds(df | df).count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);

    QVERIFY(manager.isFilterSupported(f) == false);
    QVERIFY(manager.isFilterSupported(df) == false);

    QList<QOrganizerItem> list;
    list << foo;

    QVERIFY(!manager.saveItems(&list, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);

    QVERIFY(!manager.removeItems(QList<QOrganizerItemLocalId>(), &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::BadArgumentError);

    QList<QOrganizerItemLocalId> idlist;
    idlist << foo.id().localId();
    QVERIFY(!manager.removeItems(idlist, &errorMap));
    QVERIFY(errorMap.count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError);

    /* Detail definitions */
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);

    QOrganizerItemDetailDefinition def;
    def.setUnique(true);
    def.setName("new field");
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
    QOrganizerItemDetailFieldDefinition currField;
    currField.setDataType(QVariant::String);
    fields.insert("value", currField);
    def.setFields(fields);

    QVERIFY(manager.saveDetailDefinition(def, QOrganizerItemType::TypeNote) == false);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);
    QVERIFY(manager.saveDetailDefinition(def) == false);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);
    QVERIFY(manager.detailDefinitions().count(QOrganizerItemType::TypeNote) == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);
    QVERIFY(manager.detailDefinition("new field").name() == QString());
    QVERIFY(manager.removeDetailDefinition(def.name(), QOrganizerItemType::TypeNote) == false);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);
    QVERIFY(manager.removeDetailDefinition(def.name()) == false);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);
    QVERIFY(manager.detailDefinitions().count() == 0);
    QVERIFY(manager.error() == QOrganizerItemManager::NotSupportedError || manager.error() == QOrganizerItemManager::InvalidItemTypeError);

     /* Capabilities */
    QVERIFY(manager.supportedDataTypes().count() == 0);
    QVERIFY(!manager.hasFeature(QOrganizerItemManager::MutableDefinitions));
}

void tst_QOrganizerItemManager::memoryManager()
{
    QMap<QString, QString> params;
    QOrganizerItemManager m1("memory");
    params.insert("random", "shouldNotBeUsed");
    QOrganizerItemManager m2("memory", params);
    params.insert("id", "shouldBeUsed");
    QOrganizerItemManager m3("memory", params);
    QOrganizerItemManager m4("memory", params);
    params.insert("id", QString(""));
    QOrganizerItemManager m5("memory", params); // should be another anonymous

    QVERIFY(m1.hasFeature(QOrganizerItemManager::MutableDefinitions));
    QVERIFY(m1.hasFeature(QOrganizerItemManager::Anonymous));

    // add a item to each of m1, m2, m3
    QOrganizerItem c;
    QOrganizerItemDisplayLabel c1dl;
    c1dl.setLabel("c1dl");
    c.saveDetail(&c1dl);
    m1.saveItem(&c);
    c.setId(QOrganizerItemId());
    QOrganizerItem c2;
    QOrganizerItemDisplayLabel c2dl = c2.detail(QOrganizerItemDisplayLabel::DefinitionName);
    c2 = c;
    c2dl.setLabel("c2dl");
    c2.saveDetail(&c2dl);
    m2.saveItem(&c2);            // save c2 first; c will be given a higher id
    m2.saveItem(&c);             // save c to m2
    c.setId(QOrganizerItemId());
    c1dl.setLabel("c3dl");
    c.saveDetail(&c1dl);
    m3.saveItem(&c);

    /* test that m1 != m2 != m3 and that m3 == m4 */

    // check the counts are correct - especially note m4 and m3.
    QCOMPARE(m1.itemIds().count(), 1);
    QCOMPARE(m2.itemIds().count(), 2);
    QCOMPARE(m3.itemIds().count(), 1);
    QCOMPARE(m4.itemIds().count(), 1);
    QCOMPARE(m5.itemIds().count(), 0);

    // remove c2 from m2 - ensure that this doesn't affect any other manager.
    m2.removeItem(c2.id().localId());
    QCOMPARE(m1.itemIds().count(), 1);
    QCOMPARE(m2.itemIds().count(), 1);
    QCOMPARE(m3.itemIds().count(), 1);
    QCOMPARE(m4.itemIds().count(), 1);
    QCOMPARE(m5.itemIds().count(), 0);

    // check that the items contained within are different.
    // note that in the m1->m2 case, only the id will be different!
    QVERIFY(m1.item(m1.itemIds().at(0)) != m2.item(m2.itemIds().at(0)));
    QVERIFY(m1.item(m1.itemIds().at(0)) != m3.item(m3.itemIds().at(0)));
    QVERIFY(m2.item(m2.itemIds().at(0)) != m3.item(m3.itemIds().at(0)));
    QVERIFY(m3.item(m3.itemIds().at(0)) == m4.item(m4.itemIds().at(0)));

    // now, we should be able to remove from m4, and have m3 empty
    QVERIFY(m4.removeItem(c.id().localId()));
    QCOMPARE(m3.itemIds().count(), 0);
    QCOMPARE(m4.itemIds().count(), 0);
    QCOMPARE(m5.itemIds().count(), 0);
}


void tst_QOrganizerItemManager::compatibleItem_data()
{
    // XXX TODO: fix this test - need more item types tested etc.
    QTest::addColumn<QOrganizerItem>("input");
    QTest::addColumn<QOrganizerItem>("expected");
    QTest::addColumn<QOrganizerItemManager::Error>("error");

    QOrganizerNote baseNote;
    QOrganizerItemDisplayLabel dl;
    dl.setLabel(QLatin1String("There you go, labelling items again..."));
    baseNote.saveDetail(&dl);

    {
        QTest::newRow("already compatible") << QOrganizerItem(baseNote) << QOrganizerItem(baseNote) << QOrganizerItemManager::NoError;
    }

    {
        QOrganizerItem item(baseNote);
        QOrganizerItemDetail detail("UnknownDetail");
        detail.setValue("Key", QLatin1String("Value"));
        item.saveDetail(&detail);
        QTest::newRow("unknown detail") << item << QOrganizerItem(baseNote) << QOrganizerItemManager::NoError;
    }

    {
        QOrganizerItem item(baseNote);
        QOrganizerItemDescription description1;
        description1.setDescription("This is the first description");
        item.saveDetail(&description1);
        QOrganizerItemDescription description2;
        description2.setDescription("This is the second description");
        item.saveDetail(&description2);
        QOrganizerItem expected(baseNote);
        expected.saveDetail(&description2); // can't have two descriptions, only the second will be kept XXX TODO: surely it's backend specific which gets kept?
        QTest::newRow("duplicate unique field") << item << expected << QOrganizerItemManager::NoError;
    }

    {
        QOrganizerItem item(baseNote);
        QOrganizerItemDescription descr;
        descr.setValue("UnknownKey", "Value");
        item.saveDetail(&descr);
        QTest::newRow("unknown field") << item << QOrganizerItem(baseNote) << QOrganizerItemManager::NoError;
    }

    {
        // XXX TODO: fix this test.
        //QOrganizerEvent item(baseNote);
        //QOrganizerItemDisplayLabel displayLabel;
        //displayLabel.setValue(QOrganizerItemDisplayLabel::FieldLabel, QStringList("Value"));
        //item.saveDetail(&displayLabel);
        //QTest::newRow("wrong type") << QOrganizerItem(item) << QOrganizerItem(baseNote) << QOrganizerItemManager::NoError;
    }
}

void tst_QOrganizerItemManager::compatibleItem()
{
    QOrganizerItemManager cm("memory");

    QFETCH(QOrganizerItem, input);
    QFETCH(QOrganizerItem, expected);
    QFETCH(QOrganizerItemManager::Error, error);
    QCOMPARE(cm.compatibleItem(input), expected);
    QCOMPARE(cm.error(), error);
}

void tst_QOrganizerItemManager::recurrenceWithGenerator_data()
{
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QDate>("eventDate");
    QTest::addColumn<QOrganizerItemRecurrenceRule>("recurrenceRule");
    QTest::addColumn<QDate>("startDate");
    QTest::addColumn<QDate>("endDate");
    QTest::addColumn<QList<QDate> >("occurrenceDates");

    QStringList managers = QOrganizerItemManager::availableManagers();

    /* Known one that will not pass */
    managers.removeAll("invalid");
    managers.removeAll("testdummy");
    managers.removeAll("teststaticdummy");
    managers.removeAll("maliciousplugin");
    managers.removeAll("skeleton");

    foreach(QString mgr, managers) {
        QString managerUri = QOrganizerItemManager::buildUri(mgr, QMap<QString, QString>());

        {
            QOrganizerItemRecurrenceRule rrule;
            rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
            rrule.setEndDate(QDate(2010, 1, 22));
            QTest::newRow(QString("mgr=%1, weekly recurrence").arg(mgr).toLatin1().constData())
                << managerUri << QDate(2010, 1, 1) << rrule
                << QDate(2010, 1, 1) << QDate(2010, 1, 20)
                // stops at the 15th because the query end date is the 20th
                << (QList<QDate>() << QDate(2010, 1, 1) << QDate(2010, 1, 8) << QDate(2010, 1, 15));

            QTest::newRow(QString("mgr=%1, weekly recurrence, end date is non-inclusive").arg(mgr).toLatin1().constData())
                << managerUri << QDate(2010, 1, 1) << rrule
                << QDate(2010, 1, 1) << QDate(2010, 2, 1)
                // still stops at the 15th because the recurrence end date is 22nd, non-inclusively
                << (QList<QDate>() << QDate(2010, 1, 1) << QDate(2010, 1, 8) << QDate(2010, 1, 15));

            rrule.setEndDate(QDate(2010, 1, 23));
            QTest::newRow(QString("mgr=%1, weekly recurrence, end date observed (+1)").arg(mgr).toLatin1().constData())
                << managerUri << QDate(2010, 1, 1) << rrule
                << QDate(2010, 1, 1) << QDate(2010, 2, 1)
                // now stop of the 22nd
                << (QList<QDate>() << QDate(2010, 1, 1) << QDate(2010, 1, 8) << QDate(2010, 1, 15) << QDate(2010, 1, 22));

            rrule.setEndDate(QDate(2010, 1, 21));
            QTest::newRow(QString("mgr=%1, weekly recurrence, end date observed (-1)").arg(mgr).toLatin1().constData())
                << managerUri << QDate(2010, 1, 1) << rrule
                << QDate(2010, 1, 1) << QDate(2010, 2, 1)
                // now stop of the 22nd
                << (QList<QDate>() << QDate(2010, 1, 1) << QDate(2010, 1, 8) << QDate(2010, 1, 15));
        }
    }
}

void tst_QOrganizerItemManager::recurrenceWithGenerator()
{
    QFETCH(QString, uri);
    QFETCH(QDate, eventDate);
    QFETCH(QOrganizerItemRecurrenceRule, recurrenceRule);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));
    QFETCH(QDate, startDate);
    QFETCH(QDate, endDate);
    QFETCH(QList<QDate>, occurrenceDates);

    QOrganizerEvent event;
    event.setDisplayLabel("event");
    event.setStartDateTime(QDateTime(eventDate, QTime(11, 0, 0)));
    event.setEndDateTime(QDateTime(eventDate, QTime(11, 30, 0)));
    event.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>() << recurrenceRule);
    QVERIFY(cm->saveItem(&event));
    QList<QOrganizerItem> items = cm->itemInstances(event,
            QDateTime(startDate, QTime(0, 0, 0)),
            QDateTime(endDate.addDays(1), QTime(0, 0, 0)));
    QCOMPARE(items.size(), occurrenceDates.size());
    for (int i = 0; i < items.size(); i++) {
        QOrganizerItem item = items.at(i);
        QCOMPARE(item.type(), QString(QLatin1String(QOrganizerItemType::TypeEventOccurrence)));
        QDate occurrenceDate;
        if (item.type() == QOrganizerItemType::TypeEventOccurrence) {
            occurrenceDate = item.detail<QOrganizerEventTimeRange>().startDateTime().date();
        } else if (item.type() == QOrganizerItemType::TypeTodoOccurrence) {
            occurrenceDate = item.detail<QOrganizerTodoTimeRange>().startDateTime().date();
        }
        QCOMPARE(occurrenceDate, occurrenceDates.at(i));
    }
}

void tst_QOrganizerItemManager::itemValidation()
{
    /* Use the memory engine as a reference (validation is not engine specific) */
    QScopedPointer<QOrganizerItemManager> cm(new QOrganizerItemManager("memory"));
    QOrganizerNote c;

    /*
     * Add some definitions for testing:
     *
     * 1) a unique detail
     * 2) a detail with restricted values
     * 3) a create only detail
     * 4) a unique create only detail
     */
    QOrganizerItemDetailDefinition uniqueDef;
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
    QOrganizerItemDetailFieldDefinition field;
    field.setDataType(QVariant::String);
    fields.insert("value", field);
    uniqueDef.setName("UniqueDetail");
    uniqueDef.setFields(fields);
    uniqueDef.setUnique(true);
    QVERIFY(cm->saveDetailDefinition(uniqueDef, QOrganizerItemType::TypeNote));

    QOrganizerItemDetailDefinition restrictedDef;
    restrictedDef.setName("RestrictedDetail");
    fields.clear();
    field.setAllowableValues(QVariantList() << "One" << "Two" << "Three");
    fields.insert("value", field);
    restrictedDef.setFields(fields);
    QVERIFY(cm->saveDetailDefinition(restrictedDef, QOrganizerItemType::TypeNote));

    // first, test an invalid definition
    QOrganizerItemDetail d1 = QOrganizerItemDetail("UnknownDefinition");
    d1.setValue("test", "test");
    c.saveDetail(&d1);
    QVERIFY(!cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidDetailError);
    c.removeDetail(&d1);

    // second, test an invalid uniqueness constraint
    QOrganizerItemDetail d2 = QOrganizerItemDetail("UniqueDetail");
    d2.setValue("value", "test");
    c.saveDetail(&d2);

    // One unique should be ok
    QVERIFY(cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::NoError);

    // Two uniques should not be ok
    QOrganizerItemDetail d3 = QOrganizerItemDetail("UniqueDetail");
    d3.setValue("value", "test2");
    c.saveDetail(&d3);
    QVERIFY(!cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::AlreadyExistsError);
    c.removeDetail(&d3);
    c.removeDetail(&d2);

    // third, test an invalid field name
    QOrganizerItemDetail d4 = QOrganizerItemDetail(QOrganizerItemDescription::DefinitionName);
    d4.setValue("test", "test");
    c.saveDetail(&d4);
    QVERIFY(!cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidDetailError);
    c.removeDetail(&d4);

    // fourth, test an invalid field data type
    QOrganizerItemDetail d5 = QOrganizerItemDetail(QOrganizerItemDescription::DefinitionName);
    d5.setValue(QOrganizerItemDescription::FieldDescription, QDateTime::currentDateTime());
    c.saveDetail(&d5);
    QVERIFY(!cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidDetailError);
    c.removeDetail(&d5);

    // fifth, test an invalid field value (not in the allowed list)
    QOrganizerItemDetail d6 = QOrganizerItemDetail("RestrictedDetail");
    d6.setValue("value", "Seven"); // not in One, Two or Three
    c.saveDetail(&d6);
    QVERIFY(!cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::InvalidDetailError);
    c.removeDetail(&d6);

    /* Now a valid value */
    d6.setValue("value", "Two");
    c.saveDetail(&d6);
    QVERIFY(cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::NoError);
    c.removeDetail(&d6);

    // Test a completely valid one.
    QOrganizerItemDetail d7 = QOrganizerItemDetail(QOrganizerItemDescription::DefinitionName);
    d7.setValue(QOrganizerItemDescription::FieldDescription, "A valid description");
    c.saveDetail(&d7);
    QVERIFY(cm->saveItem(&c));
    QCOMPARE(cm->error(), QOrganizerItemManager::NoError);
    c.removeDetail(&d7);
}

void tst_QOrganizerItemManager::signalEmission()
{
    QTest::qWait(500); // clear the signal queue
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> m1(QOrganizerItemManager::fromUri(uri));

    qRegisterMetaType<QOrganizerItemLocalId>("QOrganizerItemLocalId");
    qRegisterMetaType<QList<QOrganizerItemLocalId> >("QList<QOrganizerItemLocalId>");
    QSignalSpy spyCA(m1.data(), SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyCM(m1.data(), SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyCR(m1.data(), SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));

    QList<QVariant> args;
    QList<QOrganizerItemLocalId> arg;
    QOrganizerItem c;
    QList<QOrganizerItem> batchAdd;
    QList<QOrganizerItemLocalId> batchRemove;
    QList<QOrganizerItemLocalId> sigids;
    int addSigCount = 0; // the expected signal counts.
    int modSigCount = 0;
    int remSigCount = 0;

    // verify add emits signal added
    QOrganizerItemDisplayLabel nc;
    nc.setLabel("label me this");
    QVERIFY(m1->saveItem(&c));
    QOrganizerItemLocalId cid = c.id().localId();
    addSigCount += 1;
    QTRY_COMPARE(spyCA.count(), addSigCount);
    args = spyCA.takeFirst();
    addSigCount -= 1;
    arg = args.first().value<QList<quint32> >();
    QVERIFY(arg.count() == 1);
    QCOMPARE(QOrganizerItemLocalId(arg.at(0)), cid);

    // verify save modified emits signal changed
    nc.setLabel("label me that");
    QVERIFY(m1->saveItem(&c));
    modSigCount += 1;
    QTRY_COMPARE(spyCM.count(), modSigCount);
    args = spyCM.takeFirst();
    modSigCount -= 1;
    arg = args.first().value<QList<quint32> >();
    QVERIFY(arg.count() == 1);
    QCOMPARE(QOrganizerItemLocalId(arg.at(0)), cid);

    // verify remove emits signal removed
    m1->removeItem(c.id().localId());
    remSigCount += 1;
    QTRY_COMPARE(spyCR.count(), remSigCount);
    args = spyCR.takeFirst();
    remSigCount -= 1;
    arg = args.first().value<QList<quint32> >();
    QVERIFY(arg.count() == 1);
    QCOMPARE(QOrganizerItemLocalId(arg.at(0)), cid);

    // verify multiple adds works as advertised
    QOrganizerItem c2, c3;
    QOrganizerItemDisplayLabel nc2, nc3;
    nc2.setLabel("Mark");
    nc3.setLabel("Garry");
    QVERIFY(m1->saveItem(&c2));
    addSigCount += 1;
    QVERIFY(m1->saveItem(&c3));
    addSigCount += 1;
    QTRY_COMPARE(spyCM.count(), modSigCount);
    QTRY_COMPARE(spyCA.count(), addSigCount);

    // verify multiple modifies works as advertised
    nc2.setLabel("M.");
    QVERIFY(m1->saveItem(&c2));
    modSigCount += 1;
    nc2.setLabel("Mark");
    nc3.setLabel("G.");
    QVERIFY(m1->saveItem(&c2));
    modSigCount += 1;
    QVERIFY(m1->saveItem(&c3));
    modSigCount += 1;
    QTRY_COMPARE(spyCM.count(), modSigCount);

    // verify multiple removes works as advertised
    m1->removeItem(c3.id().localId());
    remSigCount += 1;
    m1->removeItem(c2.id().localId());
    remSigCount += 1;
    QTRY_COMPARE(spyCR.count(), remSigCount);

    QVERIFY(!m1->removeItem(c.id().localId())); // not saved.

    /* Now test the batch equivalents */
    spyCA.clear();
    spyCM.clear();
    spyCR.clear();

    /* Batch adds - set ids to zero so add succeeds. */
    c.setId(QOrganizerItemId());
    c2.setId(QOrganizerItemId());
    c3.setId(QOrganizerItemId());
    batchAdd << c << c2 << c3;
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m1->saveItems(&batchAdd, &errorMap));

    QVERIFY(batchAdd.count() == 3);
    c = batchAdd.at(0);
    c2 = batchAdd.at(1);
    c3 = batchAdd.at(2);

    /* We basically loop, processing events, until we've seen an Add signal for each item */
    sigids.clear();

    QTRY_WAIT( while(spyCA.size() > 0) {sigids += spyCA.takeFirst().at(0).value<QList<QOrganizerItemLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));
    QTRY_COMPARE(spyCM.count(), 0);
    QTRY_COMPARE(spyCR.count(), 0);

    /* Batch modifies */
    QOrganizerItemDisplayLabel modifiedName = c.detail(QOrganizerItemDisplayLabel::DefinitionName);
    modifiedName.setLabel("Modified number 1");
    modifiedName = c2.detail(QOrganizerItemDisplayLabel::DefinitionName);
    modifiedName.setLabel("Modified number 2");
    modifiedName = c3.detail(QOrganizerItemDisplayLabel::DefinitionName);
    modifiedName.setLabel("Modified number 3");

    batchAdd.clear();
    batchAdd << c << c2 << c3;
    QVERIFY(m1->saveItems(&batchAdd, &errorMap));

    sigids.clear();
    QTRY_WAIT( while(spyCM.size() > 0) {sigids += spyCM.takeFirst().at(0).value<QList<QOrganizerItemLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));

    /* Batch removes */
    batchRemove << c.id().localId() << c2.id().localId() << c3.id().localId();
    QVERIFY(m1->removeItems(batchRemove, &errorMap));

    sigids.clear();
    QTRY_WAIT( while(spyCR.size() > 0) {sigids += spyCR.takeFirst().at(0).value<QList<QOrganizerItemLocalId> >(); }, sigids.contains(c.localId()) && sigids.contains(c2.localId()) && sigids.contains(c3.localId()));

    QTRY_COMPARE(spyCA.count(), 0);
    QTRY_COMPARE(spyCM.count(), 0);

    QScopedPointer<QOrganizerItemManager> m2(QOrganizerItemManager::fromUri(uri));
    
    // During construction SIM backend (m2) will try writing items with
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

    QVERIFY(m1->hasFeature(QOrganizerItemManager::Anonymous) ==
        m2->hasFeature(QOrganizerItemManager::Anonymous));

    /* Now some cross manager testing */
    if (!m1->hasFeature(QOrganizerItemManager::Anonymous)) {
        // verify that signals are emitted for modifications made to other managers (same id).
        QOrganizerItemDisplayLabel ncs = c.detail(QOrganizerItemDisplayLabel::DefinitionName);
        ncs.setLabel("Test");
        c.setId(QOrganizerItemId()); // reset id so save can succeed.
        QVERIFY(m2->saveItem(&c));
        ncs.setLabel("Test2");
        QVERIFY(m2->saveItem(&c));
        QTRY_COMPARE(spyCA.count(), 1); // check that we received the update signals.
        QTRY_COMPARE(spyCM.count(), 1); // check that we received the update signals.
        m2->removeItem(c.localId());
        QTRY_COMPARE(spyCR.count(), 1); // check that we received the remove signal.
    }
}

void tst_QOrganizerItemManager::errorStayingPut()
{
    /* Make sure that when we clone a manager, we don't clone the error */
    QMap<QString, QString> params;
    params.insert("id", "error isolation test");
    QOrganizerItemManager m1("memory",params);

    QVERIFY(m1.error() == QOrganizerItemManager::NoError);

    /* Remove an invalid item to get an error */
    QVERIFY(m1.removeItem(0) == false);
    QVERIFY(m1.error() == QOrganizerItemManager::DoesNotExistError);

    /* Create a new manager with hopefully the same backend */
    QOrganizerItemManager m2("memory", params);

    QVERIFY(m1.error() == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(m2.error() == QOrganizerItemManager::NoError);

    /* Cause an error on the other ones and check the first is not affected */
    m2.saveItems(0, 0);
    QVERIFY(m1.error() == QOrganizerItemManager::DoesNotExistError);
    QVERIFY(m2.error() == QOrganizerItemManager::BadArgumentError);

    QOrganizerItem c;
    QOrganizerItemDetail d("This does not exist and if it does this will break");
    d.setValue("Value that also doesn't exist", 5);
    c.saveDetail(&d);

    QVERIFY(m1.saveItem(&c) == false);
    QVERIFY(m1.error() == QOrganizerItemManager::InvalidDetailError);
    QVERIFY(m2.error() == QOrganizerItemManager::BadArgumentError);
}

void tst_QOrganizerItemManager::detailDefinitions()
{
    QSKIP("Test case skipped due to crash in updatedDef initialization.",SkipAll);

    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));
    QMap<QString, QOrganizerItemDetailDefinition> defs = cm->detailDefinitions();
    QVERIFY(defs.size() > 0);

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

    foreach(QOrganizerItemDetailDefinition def, defs.values()) {
        QMap<QString, QOrganizerItemDetailFieldDefinition> fields = def.fields();

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

        foreach(QOrganizerItemDetailFieldDefinition field, def.fields().values()) {
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
    QOrganizerItemDetailDefinition newDef;
    QOrganizerItemDetailFieldDefinition field;
    QMap<QString, QOrganizerItemDetailFieldDefinition> fields;
    field.setDataType(cm->supportedDataTypes().value(0));
    fields.insert("New Value", field);
    newDef.setName("New Definition");
    newDef.setFields(fields);

    /* Updated version of an existing definition */
    QOrganizerItemDetailDefinition updatedDef = defs.begin().value(); // XXX TODO Fixme
    fields = updatedDef.fields();
    fields.insert("New Value", field);
    updatedDef.setFields(fields);

    /* A detail definition with valid allowed values (or really just one) */
    QOrganizerItemDetailDefinition allowedDef = newDef;
    field.setAllowableValues(field.allowableValues() << (QVariant(field.dataType())));
    fields.clear();
    fields.insert("Restricted value", field);
    allowedDef.setFields(fields);

    /* Many invalid definitions */
    QOrganizerItemDetailDefinition noIdDef;
    noIdDef.setFields(fields);

    QOrganizerItemDetailDefinition noFieldsDef;
    noFieldsDef.setName("No fields");

    QOrganizerItemDetailDefinition invalidFieldKeyDef;
    invalidFieldKeyDef.setName("Invalid field key");
    QMap<QString, QOrganizerItemDetailFieldDefinition> badfields;
    badfields.insert(QString(), field);
    invalidFieldKeyDef.setFields(badfields);

    QOrganizerItemDetailDefinition invalidFieldTypeDef;
    invalidFieldTypeDef.setName("Invalid field type");
    badfields.clear();
    QOrganizerItemDetailFieldDefinition badfield;
    badfield.setDataType((QVariant::Type) qMetaTypeId<UnsupportedMetatype>());
    badfields.insert("Bad type", badfield);
    invalidFieldTypeDef.setFields(badfields);

    QOrganizerItemDetailDefinition invalidAllowedValuesDef;
    invalidAllowedValuesDef.setName("Invalid field allowed values");
    badfields.clear();
    badfield.setDataType(field.dataType()); // use a supported type
    badfield.setAllowableValues(QList<QVariant>() << "String" << 5); // but unsupported value
    badfields.insert("Bad allowed", badfield);
    invalidAllowedValuesDef.setFields(badfields);

    /* XXX Multiply defined fields.. depends on semantichangeSet. */

    if (cm->hasFeature(QOrganizerItemManager::MutableDefinitions)) {
        /* First do some negative testing */

        /* Bad add class */
        QVERIFY(cm->saveDetailDefinition(QOrganizerItemDetailDefinition()) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

        /* Bad remove string */
        QVERIFY(cm->removeDetailDefinition(QString()) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(noIdDef) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(noFieldsDef) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(invalidFieldKeyDef) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(invalidFieldTypeDef) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

        QVERIFY(cm->saveDetailDefinition(invalidAllowedValuesDef) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::BadArgumentError);

        /* Check that our new definition doesn't already exist */
        QVERIFY(cm->detailDefinition(newDef.name()).isEmpty());
        QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);

        QVERIFY(cm->removeDetailDefinition(newDef.name()) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);

        /* Add a new definition */
        QVERIFY(cm->saveDetailDefinition(newDef) == true);
        QVERIFY(cm->error() == QOrganizerItemManager::NoError);

        /* Now retrieve it */
        QOrganizerItemDetailDefinition def = cm->detailDefinition(newDef.name());
        QVERIFY(def == newDef);

        /* Update it */
        QMap<QString, QOrganizerItemDetailFieldDefinition> newFields = def.fields();
        newFields.insert("Another new value", field);
        newDef.setFields(newFields);

        QVERIFY(cm->saveDetailDefinition(newDef) == true);
        QVERIFY(cm->error() == QOrganizerItemManager::NoError);

        QVERIFY(cm->detailDefinition(newDef.name()) == newDef);

        /* Remove it */
        QVERIFY(cm->removeDetailDefinition(newDef.name()) == true);
        QVERIFY(cm->error() == QOrganizerItemManager::NoError);

        /* and make sure it does not exist any more */
        QVERIFY(cm->detailDefinition(newDef.name()) == QOrganizerItemDetailDefinition());
        QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);

        /* Add the other good one */
        QVERIFY(cm->saveDetailDefinition(allowedDef) == true);
        QVERIFY(cm->error() == QOrganizerItemManager::NoError);

        QVERIFY(allowedDef == cm->detailDefinition(allowedDef.name()));

        /* and remove it */
        QVERIFY(cm->removeDetailDefinition(allowedDef.name()) == true);
        QVERIFY(cm->detailDefinition(allowedDef.name()) == QOrganizerItemDetailDefinition());
        QVERIFY(cm->error() == QOrganizerItemManager::DoesNotExistError);

    } else {
        /* Bad add class */
        QVERIFY(cm->saveDetailDefinition(QOrganizerItemDetailDefinition()) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::NotSupportedError);

        /* Make sure we can't add/remove/modify detail definitions */
        QVERIFY(cm->removeDetailDefinition(QString()) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::NotSupportedError);

        /* Try updating an existing definition */
        QVERIFY(cm->saveDetailDefinition(updatedDef) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::NotSupportedError);

        /* Try removing an existing definition */
        QVERIFY(cm->removeDetailDefinition(updatedDef.name()) == false);
        QVERIFY(cm->error() == QOrganizerItemManager::NotSupportedError);
    }
}

void tst_QOrganizerItemManager::changeSet()
{
    QOrganizerItemLocalId id(1);

    QOrganizerItemChangeSet changeSet;
    QVERIFY(changeSet.addedItems().isEmpty());
    QVERIFY(changeSet.changedItems().isEmpty());
    QVERIFY(changeSet.removedItems().isEmpty());

    changeSet.insertAddedItem(id);
    QVERIFY(!changeSet.addedItems().isEmpty());
    QVERIFY(changeSet.changedItems().isEmpty());
    QVERIFY(changeSet.removedItems().isEmpty());
    QVERIFY(changeSet.addedItems().contains(id));

    changeSet.insertChangedItem(id);
    changeSet.insertChangedItems(QList<QOrganizerItemLocalId>() << id);
    QVERIFY(changeSet.changedItems().size() == 1); // set, should only be added once.
    QVERIFY(!changeSet.addedItems().isEmpty());
    QVERIFY(!changeSet.changedItems().isEmpty());
    QVERIFY(changeSet.removedItems().isEmpty());
    QVERIFY(changeSet.changedItems().contains(id));
    changeSet.clearChangedItems();
    QVERIFY(changeSet.changedItems().isEmpty());

    changeSet.insertRemovedItems(QList<QOrganizerItemLocalId>() << id);
    QVERIFY(changeSet.removedItems().contains(id));
    changeSet.clearRemovedItems();
    QVERIFY(changeSet.removedItems().isEmpty());

    QVERIFY(changeSet.dataChanged() == false);
    QOrganizerItemChangeSet changeSet2;
    changeSet2 = changeSet;
    QVERIFY(changeSet.addedItems() == changeSet2.addedItems());
    changeSet.emitSignals(0);

    changeSet2.clearAddedItems();
    QVERIFY(changeSet2.addedItems().isEmpty());
    changeSet2.insertAddedItems(changeSet.addedItems().toList());
    QVERIFY(changeSet.addedItems() == changeSet2.addedItems());

    changeSet2.clearAll();
    QVERIFY(changeSet.addedItems() != changeSet2.addedItems());

    QOrganizerItemChangeSet changeSet3(changeSet2);
    QVERIFY(changeSet.addedItems() != changeSet3.addedItems());
    QVERIFY(changeSet2.addedItems() == changeSet3.addedItems());

    changeSet.setDataChanged(true);
    QVERIFY(changeSet.dataChanged() == true);
    QVERIFY(changeSet.dataChanged() != changeSet2.dataChanged());
    QVERIFY(changeSet.dataChanged() != changeSet3.dataChanged());
    changeSet.emitSignals(0);
}

void tst_QOrganizerItemManager::fetchHint()
{
    QOrganizerItemFetchHint hint;
    hint.setOptimizationHints(QOrganizerItemFetchHint::NoBinaryBlobs);
    QCOMPARE(hint.optimizationHints(), QOrganizerItemFetchHint::NoBinaryBlobs);
}

QList<QOrganizerItemDetail> tst_QOrganizerItemManager::removeAllDefaultDetails(const QList<QOrganizerItemDetail>& details)
{
    QList<QOrganizerItemDetail> newlist;
    foreach (const QOrganizerItemDetail d, details) {
        if (d.definitionName() != QOrganizerItemDisplayLabel::DefinitionName
                && d.definitionName() != QOrganizerItemType::DefinitionName
                && d.definitionName() != QOrganizerItemTimestamp::DefinitionName) {
            newlist << d;
        }
    }
    return newlist;
}

void tst_QOrganizerItemManager::detailOrders()
{
    QFETCH(QString, uri);
    QScopedPointer<QOrganizerItemManager> cm(QOrganizerItemManager::fromUri(uri));

    QOrganizerEvent a;

    // comments
    QOrganizerItemComment comment1, comment2, comment3;
    
    comment1.setComment("11111111");
    comment2.setComment("22222222");
    comment3.setComment("33333333");

    a.saveDetail(&comment1);
    a.saveDetail(&comment2);
    a.saveDetail(&comment3);

    QVERIFY(cm->saveItem(&a));
    a = cm->item(a.id().localId());
    
    QList<QOrganizerItemDetail> details = a.details(QOrganizerItemComment::DefinitionName);
    QVERIFY(details.count() == 3);
    
    QVERIFY(a.removeDetail(&comment2));
    QVERIFY(cm->saveItem(&a));
    a = cm->item(a.id().localId());
    details = a.details(QOrganizerItemComment::DefinitionName);
    QVERIFY(details.count() == 2);

    a.saveDetail(&comment2);
    QVERIFY(cm->saveItem(&a));
    a = cm->item(a.id().localId());
    
    details = a.details(QOrganizerItemComment::DefinitionName);
    QVERIFY(details.count() == 3);

    //addresses

    QOrganizerItemLocation address1, address2, address3;
    
    address1.setLocationName("Brandl St");
    address3 = address2 = address1;

    a.saveDetail(&address1);
    a.saveDetail(&address2);
    a.saveDetail(&address3);

    QVERIFY(cm->saveItem(&a));
    a = cm->item(a.id().localId());
    
    details = a.details(QOrganizerItemLocation::DefinitionName);
    QVERIFY(details.count() == 1); // 1 location - they're unique

    QVERIFY(a.removeDetail(&address3)); // remove the most recent.
    QVERIFY(cm->saveItem(&a));
    a = cm->item(a.id().localId());
    details = a.details(QOrganizerItemLocation::DefinitionName);
    QVERIFY(details.count() == 0); // unique, remove one means none left.

    a.saveDetail(&address2);
    QVERIFY(cm->saveItem(&a));
    a = cm->item(a.id().localId());
    
    details = a.details(QOrganizerItemLocation::DefinitionName);
    QVERIFY(details.count() == 1); // add one back.
}



void tst_QOrganizerItemManager::itemType()
{
    // XXX TODO!
}

QTEST_MAIN(tst_QOrganizerItemManager)
#include "tst_qorganizeritemmanager.moc"
