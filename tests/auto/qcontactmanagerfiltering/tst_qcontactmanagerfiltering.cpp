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

#define QT_STATICPLUGIN
#include <QtTest/QtTest>

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

/*
 * This test is mostly just for testing sorting and filtering -
 * having it in tst_QContactManager makes maintenance more
 * difficult!
 */

Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QContactManager*)
Q_DECLARE_METATYPE(QList<QUniqueId>)

/*
 * Global variables:
 * These are the definition and field names used by the actions for their matching.
 */
QMap<QString, QPair<QString, QString> > defAndFieldNamesForTypeForActions;


class tst_QContactManagerFiltering : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerFiltering();
    virtual ~tst_QContactManagerFiltering();

private:
    void dumpContactDifferences(const QContact& a, const QContact& b);
    void dumpContact(const QContact &c);
    void dumpContacts();
    bool isSuperset(const QContact& ca, const QContact& cb);

    QPair<QString, QString> definitionAndField(QContactManager *cm, QVariant::Type type, bool *nativelyFilterable);
    QList<QUniqueId> prepareModel(QContactManager* cm); // add the standard contacts
    QString convertIds(QList<QUniqueId> allIds, QList<QUniqueId> ids); // convert back to "abcd"

    QMap<QContactManager*, QMap<QString, QPair<QString, QString> > > defAndFieldNamesForTypePerManager;
    QMultiMap<QContactManager*, QUniqueId> contactsAddedToManagers;
    QMultiMap<QContactManager*, QString> detailDefinitionsAddedToManagers;
    QList<QContactManager*> managers;

private slots:
    void rangeFiltering(); // XXX should take all managers
    void rangeFiltering_data();

    void detailStringFiltering(); // XXX should take all managers
    void detailStringFiltering_data();

    void actionPlugins();
    void actionFiltering();
    void actionFiltering_data();

    void detailVariantFiltering();
    void detailVariantFiltering_data();

    void groupMembershipFiltering();
    void groupMembershipFiltering_data();

    void intersectionFiltering();
    void intersectionFiltering_data();

    void unionFiltering();
    void unionFiltering_data();

    void changelogFiltering();
    void changelogFiltering_data();

    void idListFiltering();
    void idListFiltering_data();

    void sorting(); // XXX should take all managers
    void sorting_data();

    void multiSorting();
    void multiSorting_data();

    void invalidFiltering_data();
    void invalidFiltering();

    void allFiltering_data();
    void allFiltering();
};

tst_QContactManagerFiltering::tst_QContactManagerFiltering()
{
    // firstly, build a list of the managers we wish to test.
    QStringList managerNames = QContactManager::availableManagers();

    /* Known one that will not pass */
    managerNames.removeAll("invalid");

    foreach(QString mgr, managerNames) {
        QMap<QString, QString> params;
        QString mgrUri = QContactManager::buildUri(mgr, params);
        QContactManager* cm = QContactManager::fromUri(mgrUri);
        managers.append(cm);

        if (mgr == "memory") {
            params.insert("id", "tst_QContactManager");
            mgrUri = QContactManager::buildUri(mgr, params);
            cm = QContactManager::fromUri(mgrUri);
            managers.append(cm);
        }
    }

    // for each manager that we wish to test, prepare the model.
    foreach (QContactManager* cm, managers) {
        QList<QUniqueId> addedContacts = prepareModel(cm);
        if (addedContacts != contactsAddedToManagers.values(cm)) {
            qDebug() << "prepareModel returned:" << addedContacts;
            qDebug() << "contactsAdded are:    " << contactsAddedToManagers.values(cm);
            Q_ASSERT_X(false, "prepareModel", "returned list different from saved contacts list!");
        }
    }

    qDebug() << "Finished preparing each manager for test!";
}

tst_QContactManagerFiltering::~tst_QContactManagerFiltering()
{
    // first, remove any contacts that we've added to any managers.
    foreach (QContactManager* manager, managers) {
        QList<QUniqueId> contactIds = contactsAddedToManagers.values(manager);
        foreach (const QUniqueId& cid, contactIds) {
            manager->removeContact(cid);
        }
    }
    contactsAddedToManagers.clear();

    // then, remove any detail definitions that we've added.
    foreach (QContactManager* manager, managers) {
        QStringList definitionNames = detailDefinitionsAddedToManagers.values(manager);
        foreach (const QString& definitionName, definitionNames) {
            manager->removeDetailDefinition(definitionName);
        }
    }
    detailDefinitionsAddedToManagers.clear();

    // finally, we can delete all of our manager instances
    qDeleteAll(managers);
    managers.clear();
    defAndFieldNamesForTypePerManager.clear();
}

QString tst_QContactManagerFiltering::convertIds(QList<QUniqueId> allIds, QList<QUniqueId> ids)
{
    QString ret;
    /* Expected is of the form "abcd".. */
    for (int i = 0; i < ids.size(); i++) {
        ret += ('a' + allIds.indexOf(ids.at(i)));
    }

    return ret;
}

void tst_QContactManagerFiltering::detailStringFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<int>("matchflags");
    QTest::addColumn<QString>("expected");

    QVariant ev; // empty variant
    QString es; // empty string

    QString name = QContactName::DefinitionName;
    QString firstname = QContactName::FieldFirst;

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);
        QTest::newRow("Name == Aaron") << manager << name << firstname << QVariant("Aaron") << 0 << "a";
        QTest::newRow("Name == aaron") << manager << name << firstname << QVariant("aaron") << 0 << "a";
        QTest::newRow("Name == Aaron, case sensitive") << manager << name << firstname << QVariant("Aaron") << (int)(Qt::MatchCaseSensitive) << "a";
        QTest::newRow("Name == aaron, case sensitive") << manager << name << firstname << QVariant("aaron") << (int)(Qt::MatchCaseSensitive) << es;

        QTest::newRow("Name == Aaron, begins") << manager << name << firstname << QVariant("Aaron") << (int)(Qt::MatchStartsWith) << "a";
        QTest::newRow("Name == aaron, begins") << manager << name << firstname << QVariant("aaron") << (int)(Qt::MatchStartsWith) << "a";
        QTest::newRow("Name == Aaron, begins, case sensitive") << manager << name << firstname << QVariant("Aaron") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << "a";
        QTest::newRow("Name == aaron, begins, case sensitive") << manager << name << firstname << QVariant("aaron") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << es;

        QTest::newRow("Name == Aar, begins") << manager << name << firstname << QVariant("Aar") << (int)(Qt::MatchStartsWith) << "a";
        QTest::newRow("Name == aar, begins") << manager << name << firstname << QVariant("aar") << (int)(Qt::MatchStartsWith) << "a";
        QTest::newRow("Name == Aar, begins, case sensitive") << manager << name << firstname << QVariant("Aar") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << "a";
        QTest::newRow("Name == aar, begins, case sensitive") << manager << name << firstname << QVariant("aar") << (int)(Qt::MatchStartsWith | Qt::MatchCaseSensitive) << es;

        QTest::newRow("Name == aro, contains") << manager << name << firstname << QVariant("aro") << (int)(Qt::MatchContains) << "a";
        QTest::newRow("Name == ARO, contains") << manager << name << firstname << QVariant("ARO") << (int)(Qt::MatchContains) << "a";
        QTest::newRow("Name == aro, contains, case sensitive") << manager << name << firstname << QVariant("aro") << (int)(Qt::MatchContains | Qt::MatchCaseSensitive) << "a";
        QTest::newRow("Name == ARO, contains, case sensitive") << manager << name << firstname << QVariant("ARO") << (int)(Qt::MatchContains | Qt::MatchCaseSensitive) << es;

        QTest::newRow("Name == ron, ends") << manager << name << firstname << QVariant("ron") << (int)(Qt::MatchEndsWith) << "a";
        QTest::newRow("Name == ARON, ends") << manager << name << firstname << QVariant("ARON") << (int)(Qt::MatchEndsWith) << "a";
        QTest::newRow("Name == aron, ends, case sensitive") << manager << name << firstname << QVariant("aron") << (int)(Qt::MatchEndsWith | Qt::MatchCaseSensitive) << "a";
        QTest::newRow("Name == ARON, ends, case sensitive") << manager << name << firstname << QVariant("ARON") << (int)(Qt::MatchEndsWith | Qt::MatchCaseSensitive) << es;

        QTest::newRow("Name == Aaron, fixed") << manager << name << firstname << QVariant("Aaron") << (int)(Qt::MatchFixedString) << "a";
        QTest::newRow("Name == aaron, fixed") << manager << name << firstname << QVariant("aaron") << (int)(Qt::MatchFixedString) << "a";
        QTest::newRow("Name == Aaron, fixed, case sensitive") << manager << name << firstname << QVariant("Aaron") << (int)(Qt::MatchFixedString | Qt::MatchCaseSensitive) << "a";
        QTest::newRow("Name == aaron, fixed, case sensitive") << manager << name << firstname << QVariant("aaron") << (int)(Qt::MatchFixedString | Qt::MatchCaseSensitive) << es;

        /* Converting other types to strings */
        QPair<QString, QString> defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Integer");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("integer == 20") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("20") << 0 << es;
            QTest::newRow("integer == 20, as string") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("20") << (int)(Qt::MatchFixedString) << "b";
            QTest::newRow("integer == 20, begins with, string") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("20") << (int)(Qt::MatchFixedString | Qt::MatchStartsWith) << "b";
            QTest::newRow("integer == 2, begins with, string") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("2") << (int)(Qt::MatchFixedString | Qt::MatchStartsWith) << "b";
            QTest::newRow("integer == 20, ends with, string") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("20") << (int)(Qt::MatchFixedString | Qt::MatchEndsWith) << "bc";
            QTest::newRow("integer == 0, ends with, string") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("0") << (int)(Qt::MatchFixedString | Qt::MatchEndsWith) << "abc";
            QTest::newRow("integer == 20, contains, string") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("20") << (int)(Qt::MatchFixedString | Qt::MatchContains) << "bc";
            QTest::newRow("integer == 0, contains, string") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant("0") << (int)(Qt::MatchFixedString | Qt::MatchContains) << "abc";
        }
    }
}

void tst_QContactManagerFiltering::detailStringFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(QVariant, value);
    QFETCH(QString, expected);
    QFETCH(int, matchflags);

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> ids;

    QContactDetailFilter df;
    df.setDetailDefinitionName(defname, fieldname);
    df.setValue(value);
    df.setMatchFlags(Qt::MatchFlags(matchflags));

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QVERIFY(cm->information()->filterSupported(df) == false);

    ids = cm->contacts(df);

    QString output = convertIds(contacts, ids);
    QEXPECT_FAIL("integer == 20", "Not sure if this should pass or fail", Continue);
    QCOMPARE(output, expected);
}

void tst_QContactManagerFiltering::detailVariantFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<bool>("setValue");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("expected");

    QVariant ev; // empty variant
    QString es; // empty string

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);

        /* Nothings */
        QTest::newRow("no name") << manager << es << es << false << ev << es;
        QTest::newRow("no def name") << manager << es << "value" << false << ev << es;

        /* Strings (name) */
        QTest::newRow("first name presence") << manager << "Name" << QString(QLatin1String(QContactName::FieldFirst)) << false << ev << "abcdefg";
        QTest::newRow("first name == Aaron") << manager << "Name" << QString(QLatin1String(QContactName::FieldFirst)) << true << QVariant("Aaron") << "a";

        /*
         * Doubles
         * B has double(4.0)
         * C has double(4.0)
         * D has double(-128.0)
         */
        QPair<QString, QString> defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Double");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("double presence") << manager << defAndFieldNames.first << es << false << ev << "bcd";
            QTest::newRow("double presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "bcd";
            QTest::newRow("double presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("double value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("double value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(QDateTime()) << es;
            QTest::newRow("double value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(3.5) << es;
            QTest::newRow("double value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(4.0) << "bc";
            QTest::newRow("double value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(4.0) << es;
            QTest::newRow("double value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(-128.0) << "d";
            QTest::newRow("double value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(-128.0) << es;
        }

        /*
         * Integers
         * A has 10
         * B has 20
         * C has -20
         */
        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Integer");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("integer presence") << manager << defAndFieldNames.first << es << false << ev << "abc";
            QTest::newRow("integer presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "abc";
            QTest::newRow("integer presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("integer value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(50) << es;
            QTest::newRow("integer value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("integer value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(50) << es;
            QTest::newRow("integer value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(10) << "a";
            QTest::newRow("integer value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(10) << es;
            QTest::newRow("integer value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(-20) << "c";
            QTest::newRow("integer value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(-20) << es;
        }

        /*
         * Date times
         * A has QDateTime(QDate(2009, 06, 29), QTime(16, 52, 23, 0))
         * C has QDateTime(QDate(2009, 06, 29), QTime(16, 54, 17, 0))
         */
        const QDateTime adt(QDate(2009, 06, 29), QTime(16, 52, 23, 0));
        const QDateTime cdt(QDate(2009, 06, 29), QTime(16, 54, 17, 0));

        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("DateTime");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("datetime presence") << manager << defAndFieldNames.first << es << false << ev << "ac";
            QTest::newRow("datetime presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "ac";
            QTest::newRow("datetime presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("datetime value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(QDateTime(QDate(2100,5,13), QTime(5,5,5))) << es;
            QTest::newRow("datetime value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("datetime value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(QDateTime(QDate(2100,5,13), QTime(5,5,5))) << es;
            QTest::newRow("datetime value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(adt) << "a";
            QTest::newRow("datetime value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(adt) << es;
            QTest::newRow("datetime value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(cdt)<< "c";
            QTest::newRow("datetime value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(cdt) << es;
        }

        /*
         * Dates
         * A has QDate(1988, 1, 26)
         * B has QDate(1492, 5, 5)
         * D has QDate(1770, 10, 1)
         */
        const QDate ad(1988, 1, 26);
        const QDate bd(1492, 5, 5);
        const QDate dd(1770, 10, 1);

        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Date");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("date presence") << manager << defAndFieldNames.first << es << false << ev << "abd";
            QTest::newRow("date presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "abd";
            QTest::newRow("date presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("date value (no match)") << manager << defAndFieldNames.first <<defAndFieldNames.second << true << QVariant(QDate(2100,5,13)) << es;
            QTest::newRow("date value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("date value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(QDate(2100,5,13)) << es;
            QTest::newRow("date value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(ad) << "a";
            QTest::newRow("date value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(ad) << es;
            QTest::newRow("date value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(bd)<< "b";
            QTest::newRow("date value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(bd) << es;
            QTest::newRow("date value 3") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(dd)<< "d";
            QTest::newRow("date value 3 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(dd) << es;
        }

        /*
         * Times
         * A has QTime(16,52,23,0)
         * B has QTime(15,52,23,0)
         */
        const QTime at = QTime(16,52,23,0);
        const QTime bt = QTime(15,52,23,0);

        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Time");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("time presence") << manager << defAndFieldNames.first << es << false << ev << "ab";
            QTest::newRow("time presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "ab";
            QTest::newRow("time presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("time value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(QTime(5,5,5)) << es;
            QTest::newRow("time value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("time value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(QTime(5,5,5)) << es;
            QTest::newRow("time value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(at) << "a";
            QTest::newRow("time value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(at) << es;
            QTest::newRow("time value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(bt)<< "b";
            QTest::newRow("time value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(bt) << es;
        }


        /*
         * Bool
         * A has bool(true)
         * B has bool(false)
         * C has bool(false)
         */
        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Bool");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("bool presence") << manager << defAndFieldNames.first << es << false << ev << "abc";
            QTest::newRow("bool presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "abc";
            QTest::newRow("bool presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("bool value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(4.0) << es;
            QTest::newRow("bool value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(true) << "a";
            QTest::newRow("bool value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(true) << es;
            QTest::newRow("bool value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(false) << "bc";
            QTest::newRow("bool value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(false) << es;
        }

        /*
         * LongLong
         * C has LongLong(8000000000LL)
         * D has LongLong(-14000000000LL)
         */
        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("LongLong");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("longlong presence") << manager << defAndFieldNames.first << es << false << ev << "cd";
            QTest::newRow("longlong presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "cd";
            QTest::newRow("longlong presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("longlong value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(50000000000LL) << es;
            QTest::newRow("longlong value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("longlong value (wrong field, no match)") << manager << defAndFieldNames.first<< "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(50000000000LL) << es;
            QTest::newRow("longlong value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(8000000000LL) << "c";
            QTest::newRow("longlong value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(8000000000LL) << es;
            QTest::newRow("longlong value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(-14000000000LL) << "d";
            QTest::newRow("longlong value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(-14000000000LL) << es;
        }

        /*
         * ULongLong
         * A has ULongLong(120000000000ULL)
         * B has ULongLong(80000000000ULL)
         * C has ULongLong(80000000000ULL)
         */
        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("ULongLong");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("ulonglong presence") << manager << defAndFieldNames.first << es << false << ev << "abc";
            QTest::newRow("ulonglong presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "abc";
            QTest::newRow("ulonglong presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("ulonglong value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(50000000000ULL) << es;
            QTest::newRow("ulonglong value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("ulonglong value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(50000000000ULL) << es;
            QTest::newRow("ulonglong value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(120000000000ULL) << "a";
            QTest::newRow("ulonglong value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(120000000000ULL) << es;
            QTest::newRow("ulonglong value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(80000000000ULL) << "bc";
            QTest::newRow("ulonglong value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(80000000000ULL) << es;
        }

        /*
         * UInt
         * B has UInt(4000000000u)
         * D has UInt(3000000000u)
         */
        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("UInt");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("unsigned integer presence") << manager << defAndFieldNames.first << es << false << ev << "bd";
            QTest::newRow("unsigned integer presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "bd";
            QTest::newRow("unsigned integer presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("unsigned integer value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3500000000u) << es;
            QTest::newRow("unsigned integer value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("unsigned integer value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(3500000000u) << es;
            QTest::newRow("unsigned integer value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(4000000000u) << "b";
            QTest::newRow("unsigned integer value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(4000000000u) << es;
            QTest::newRow("unsigned integer value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3000000000u) << "d";
            QTest::newRow("unsigned integer value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(3000000000u) << es;
        }

        /*
         * Char
         * B has QChar('b')
         * C has QChar('c')
         */
        const QChar bchar('b');
        const QChar cchar('c');
        defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Char");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("char presence") << manager << defAndFieldNames.first << es << false << ev << "bc";
            QTest::newRow("char presence (inc field)") << manager << defAndFieldNames.first << defAndFieldNames.second << false << ev << "bc";
            QTest::newRow("char presence (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << false << ev << es;
            QTest::newRow("char value (no match)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(QChar('a')) << es;
            QTest::newRow("char value (wrong type)") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(3.5) << es;
            QTest::newRow("char value (wrong field, no match)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(QChar('a')) << es;
            QTest::newRow("char value") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(bchar) << "b";
            QTest::newRow("char value (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(bchar) << es;
            QTest::newRow("char value 2") << manager << defAndFieldNames.first << defAndFieldNames.second << true << QVariant(cchar)<< "c";
            QTest::newRow("char value 2 (wrong field)") << manager << defAndFieldNames.first << "x-nokia-mobility-contacts-test-invalidFieldName" << true << QVariant(cchar) << es;
        }
    }
}

void tst_QContactManagerFiltering::detailVariantFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(bool, setValue);
    QFETCH(QVariant, value);
    QFETCH(QString, expected);

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> ids;

    QContactDetailFilter df;
    df.setDetailDefinitionName(defname, fieldname);
    if (setValue)
        df.setValue(value);

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QVERIFY(cm->information()->filterSupported(df) == false);

    ids = cm->contacts(df);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
}

void tst_QContactManagerFiltering::rangeFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<QVariant>("minrange");
    QTest::addColumn<QVariant>("maxrange");
    QTest::addColumn<bool>("setrfs");
    QTest::addColumn<int>("rangeflagsi");
    QTest::addColumn<bool>("setmfs");
    QTest::addColumn<int>("matchflagsi");
    QTest::addColumn<QString>("expected");

    QVariant ev; // empty variant
    QString es; // empty string

    QString namedef = QContactName::DefinitionName;
    QString firstname = QContactName::FieldFirst;
    QString lastname = QContactName::FieldLast;

    QString phonedef = QContactPhoneNumber::DefinitionName;
    QString phonenum = QContactPhoneNumber::FieldNumber;

    int csflag = (int)Qt::MatchCaseSensitive;

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);

        /* First, cover the "empty defname / fieldname / ranges" cases */
        QTest::newRow("invalid defname") << manager << es << firstname << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;
        QTest::newRow("defn presence test") << manager << namedef << es << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << "abcdefg";
        QTest::newRow("field presence test") << manager << phonedef << phonenum << QVariant() << QVariant() << false << 0 << true << 0 << "ab";
        QTest::newRow("good def, bad field") << manager << namedef << "Bongo" << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;
        QTest::newRow("bad def") << manager << "Bongo" << es << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << es;

        /* Presence for fields that aren't there */
        QTest::newRow("defn presence test negative") << manager << "Burgers" << es << ev << ev << false << 0 << false << 0 << es;
        QTest::newRow("field presence test negative") << manager << "Burgers" << "Beef" << ev << ev << false << 0 << false << 0 << es;
        QTest::newRow("defn yes, field no presence test negative") << manager << namedef << "Burger" << ev << ev << false << 0 << false << 0 << es;

        QTest::newRow("no max, all results") << manager << namedef << firstname << QVariant("a") << QVariant() << false << 0 << true << 0 << "abcdefg";
        QTest::newRow("no max, some results") << manager << namedef << firstname << QVariant("BOB") << QVariant() << false << 0 << true << 0 << "bcdefg";
        QTest::newRow("no max, no results") << manager << namedef << firstname << QVariant("ZamBeZI") << QVariant() << false << 0 << true << 0 << es;
        QTest::newRow("no min, all results") << manager << namedef << firstname << QVariant() << QVariant("zambezi") << false << 0 << true << 0 << "abcdefg";
        QTest::newRow("no min, some results") << manager << namedef << firstname << QVariant() << QVariant("bOb") << false << 0 << true << 0 << "a";
        QTest::newRow("no min, no results") << manager << namedef << firstname << QVariant() << QVariant("aardvark") << false << 0 << true << 0 << es;

        /* now case sensitive */
        QTest::newRow("no max, cs, all results") << manager << namedef << firstname << QVariant("A") << QVariant() << false << 0 << true << csflag << "abcdefg";
        QTest::newRow("no max, cs, some results") << manager << namedef << firstname << QVariant("Bob") << QVariant() << false << 0 << true << csflag << "bcdefg";
        QTest::newRow("no max, cs, no results") << manager << namedef << firstname << QVariant("Zambezi") << QVariant() << false << 0 << true << csflag << es;
        QTest::newRow("no min, cs, all results") << manager << namedef << firstname << QVariant() << QVariant("Zambezi") << false << 0 << true << csflag << "abcdefg";
        QTest::newRow("no min, cs, some results") << manager << namedef << firstname << QVariant() << QVariant("Bob") << false << 0 << true << csflag << "a";
        QTest::newRow("no min, cs, no results") << manager << namedef << firstname << QVariant() << QVariant("Aardvark") << false << 0 << true << csflag << es;

        /* due to ascii sorting, most lower case parameters give all results, which is boring */
        QTest::newRow("no max, cs, badcase, all results") << manager << namedef << firstname << QVariant("A") << QVariant() << false << 0 << true << csflag << "abcdefg";
        QTest::newRow("no max, cs, badcase, some results") << manager << namedef << firstname << QVariant("BOB") << QVariant() << false << 0 << true << csflag << "bcdefg";
        QTest::newRow("no max, cs, badcase, no results") << manager << namedef << firstname << QVariant("ZAMBEZI") << QVariant() << false << 0 << true << csflag << es;
        QTest::newRow("no min, cs, badcase, all results") << manager << namedef << firstname << QVariant() << QVariant("ZAMBEZI") << false << 0 << true << csflag << "abcdefg";
        QTest::newRow("no min, cs, badcase, some results") << manager << namedef << firstname << QVariant() << QVariant("BOB") << false << 0 << true << csflag << "a";
        QTest::newRow("no min, cs, badcase, no results") << manager << namedef << firstname << QVariant() << QVariant("AARDVARK") << false << 0 << true << csflag << es;

        /* 'a' has phone number ("555-1212") */
        QTest::newRow("range1") << manager << phonedef << phonenum << QVariant("555-1200") << QVariant("555-1220") << false << 0 << false << 0 << "a";

        /* A(Aaron Aaronson), B(Bob Aaronsen), C(Boris Aaronsun), D(Dennis FitzMacyntire) */
        // string range matching - no matchflags set.
        QTest::newRow("string range - no matchflags - 1") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << false << 0 << true << 0 << "a";
        QTest::newRow("string range - no matchflags - 2") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "a";
        QTest::newRow("string range - no matchflags - 3") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "a";
        QTest::newRow("string range - no matchflags - 4") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "ab";
        QTest::newRow("string range - no matchflags - 5") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "ab";
        QTest::newRow("string range - no matchflags - 6") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "c";
        QTest::newRow("string range - no matchflags - 7") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "bc";
        QTest::newRow("string range - no matchflags - 8") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << 0 << "bc";
        QTest::newRow("string range - no matchflags - 9") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "c";
        QTest::newRow("string range - no matchflags - 10") << manager << namedef << firstname << QVariant("Barry") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << 0 << "bc";

        // string range matching - Qt::MatchStartsWith should produce the same results as without matchflags set.
        QTest::newRow("string range - startswith - 1") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "a";
        QTest::newRow("string range - startswith - 2") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "a";
        QTest::newRow("string range - startswith - 3") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";
        QTest::newRow("string range - startswith - 4") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";
        QTest::newRow("string range - startswith - 5") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "c";
        QTest::newRow("string range - startswith - 6") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";
        QTest::newRow("string range - startswith - 7") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";
        QTest::newRow("string range - startswith - 8") << manager << namedef << firstname << QVariant("Bob") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "c";
        QTest::newRow("string range - startswith - 9") << manager << namedef << firstname << QVariant("Barry") << QVariant("C") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "bc";

        // Open ended starts with
        QTest::newRow("string range - startswith open top - 1") << manager << namedef << firstname << QVariant("A") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchStartsWith) << "abcdefg";
        QTest::newRow("string range - startswith open top - 2") << manager << namedef << firstname << QVariant("A") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchStartsWith) << "abcdefg";
        QTest::newRow("string range - startswith open top - 3") << manager << namedef << firstname << QVariant("Aaron") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchStartsWith) << "abcdefg";
        QTest::newRow("string range - startswith open top - 4") << manager << namedef << firstname << QVariant("Aaron") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchStartsWith) << "bcdefg";
        QTest::newRow("string range - startswith open bottom - 1") << manager << namedef << firstname << ev << QVariant("Borit") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "abc";
        QTest::newRow("string range - startswith open bottom - 2") << manager << namedef << firstname << ev << QVariant("Borit") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "abc";
        QTest::newRow("string range - startswith open bottom - 3") << manager << namedef << firstname << ev << QVariant("Boris") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchStartsWith) << "abc";
        QTest::newRow("string range - startswith open bottom - 4") << manager << namedef << firstname << ev << QVariant("Boris") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchStartsWith) << "ab";

        // Qt::MatchContains with range is invalid
        QTest::newRow("string range - contains - 1") << manager << namedef << firstname << QVariant("A") << QVariant("Bob") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchContains) << es;

        // Check EndsWith with range: A == son, B == sen, C == sun
        QTest::newRow("string range - endswith - 1") << manager << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << es;
        QTest::newRow("string range - endswith - 2") << manager << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "b";
        QTest::newRow("string range - endswith - 3") << manager << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "a";
        QTest::newRow("string range - endswith - 4") << manager << namedef << lastname << QVariant("sen") << QVariant("son") << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "ab";
        QTest::newRow("string range - endswith - 5") << manager << namedef << lastname << QVariant("sen") << QVariant("sun") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "ac";
        QTest::newRow("string range - endswith - 6") << manager << namedef << lastname << QVariant("sen") << QVariant("sun") << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "a";

        // Endswith with open ends
        QTest::newRow("string range - endswith no max - 1") << manager << namedef << lastname << QVariant("sen") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchEndsWith) << "acdg";
        QTest::newRow("string range - endswith no max - 2") << manager << namedef << lastname << QVariant("sen") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchEndsWith) << "abcdg";
        QTest::newRow("string range - endswith no max - 3") << manager << namedef << lastname << QVariant("sem") << ev << true << (int)(QContactDetailRangeFilter::ExcludeLower) << true << (int)(Qt::MatchEndsWith) << "abcdg";
        QTest::newRow("string range - endswith no max - 4") << manager << namedef << lastname << QVariant("sem") << ev << true << (int)(QContactDetailRangeFilter::IncludeLower) << true << (int)(Qt::MatchEndsWith) << "abcdg";
        QTest::newRow("string range - endswith no min - 1") << manager << namedef << lastname << ev << QVariant("sen") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "ef";
        QTest::newRow("string range - endswith no min - 2") << manager << namedef << lastname << ev << QVariant("sen") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "bef";
        QTest::newRow("string range - endswith no min - 3") << manager << namedef << lastname << ev << QVariant("seo") << true << (int)(QContactDetailRangeFilter::ExcludeUpper) << true << (int)(Qt::MatchEndsWith) << "bef";
        QTest::newRow("string range - endswith no min - 4") << manager << namedef << lastname << ev << QVariant("seo") << true << (int)(QContactDetailRangeFilter::IncludeUpper) << true << (int)(Qt::MatchEndsWith) << "bef";

        /* A(10), B(20), C(-20) */
        // Now integer range testing
        QPair<QString, QString> defAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Integer");
        if (!defAndFieldNames.first.isEmpty() && !defAndFieldNames.second.isEmpty()) {
            QTest::newRow("int range - no rangeflags - 1") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(9) << QVariant(9) << false << 0 << false << 0 << es;
            QTest::newRow("int range - no rangeflags - 2") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(9) << QVariant(10) << false << 0 << false << 0 << es;
            QTest::newRow("int range - no rangeflags - 3") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(9) << QVariant(11) << false << 0 << false << 0 << "a";
            QTest::newRow("int range - no rangeflags - 4") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(10) << QVariant(10) << false << 0 << false << 0 << es;
            QTest::newRow("int range - rangeflags - 1") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper) << false << 0 << es;
            QTest::newRow("int range - rangeflags - 2") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::ExcludeUpper) << false << 0 << es;
            QTest::newRow("int range - rangeflags - 3") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << es;
            QTest::newRow("int range - rangeflags - 4") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(10) << QVariant(10) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "a";
            QTest::newRow("int range - rangeflags - 5") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(10) << QVariant(11) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "a";
            QTest::newRow("int range - rangeflags - 6") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(11) << QVariant(11) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << es;
            QTest::newRow("int range - rangeflags - 7") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(-30) << QVariant(-19) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "c";
            QTest::newRow("int range - rangeflags - 8") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(-20) << QVariant(-30) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << es;
            QTest::newRow("int range - rangeflags - variant - 1") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant(9) << QVariant() << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "ab";
            QTest::newRow("int range - rangeflags - variant - 2") << manager << defAndFieldNames.first << defAndFieldNames.second << QVariant() << QVariant(11) << true << (int)(QContactDetailRangeFilter::IncludeLower | QContactDetailRangeFilter::IncludeUpper) << false << 0 << "ac";
        }
    }
}

void tst_QContactManagerFiltering::rangeFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(QVariant, minrange);
    QFETCH(QVariant, maxrange);
    QFETCH(bool, setrfs);
    QFETCH(int, rangeflagsi);
    QFETCH(bool, setmfs);
    QFETCH(int, matchflagsi);
    QFETCH(QString, expected);

    QContactDetailRangeFilter::RangeFlags rangeflags = (QContactDetailRangeFilter::RangeFlags)rangeflagsi;
    Qt::MatchFlags matchflags = (Qt::MatchFlags) matchflagsi;

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> ids;

    /* Build the range filter */
    QContactDetailRangeFilter drf;
    drf.setDetailDefinitionName(defname, fieldname);
    if (setrfs)
        drf.setRange(minrange, maxrange, rangeflags);
    else
        drf.setRange(minrange, maxrange);
    if (setmfs)
        drf.setMatchFlags(matchflags);

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QContactManagerInfo *info = cm->information();
    QVERIFY(info->filterSupported(drf) == false);

    ids = cm->contacts(drf);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
}

void tst_QContactManagerFiltering::groupMembershipFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QString>("expectedone");
    QTest::addColumn<QString>("expectedtwo");

    QString es; // empty string.

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);
        QTest::newRow("1") << manager << "abcd" << es;
        QTest::newRow("2") << manager << "abcd" << "abcd";
        QTest::newRow("3") << manager << "ad" << "ab";
        QTest::newRow("4") << manager << es << "c";
    }
}

void tst_QContactManagerFiltering::groupMembershipFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(QString, expectedone);
    QFETCH(QString, expectedtwo);

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> idsone, idstwo;

    QContactGroup g1, g2;
    g1.setName("GroupOne");
    g2.setName("GroupTwo");

    // add the specified members to the specified groups
    for (int i = 0; i < expectedone.size(); i++)
        g1.addMember(contacts.at(expectedone.at(i).toLower().toAscii() - 'a'));
    for (int i = 0; i < expectedtwo.size(); i++)
        g2.addMember(contacts.at(expectedtwo.at(i).toLower().toAscii() - 'a'));

    // save them to the manager.
    cm->saveGroup(&g1);
    cm->saveGroup(&g2);

    // build the group membership filters
    QContactGroupMembershipFilter cg1f, cg2f;
    cg1f.setGroupId(g1.id());
    cg2f.setGroupId(g2.id());

    /* At this point, since we're using memory, assume the filter isn't really supported */
    QVERIFY(cm->information()->filterSupported(cg1f) == false);

    idsone = cm->contacts(cg1f);
    idstwo = cm->contacts(cg2f);

    QString output = convertIds(contacts, idsone);
    QCOMPARE(output, expectedone);

    output = convertIds(contacts, idstwo);
    QCOMPARE(output, expectedtwo);
}

void tst_QContactManagerFiltering::intersectionFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<bool>("firstfilter");
    QTest::addColumn<int>("fftype"); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QTest::addColumn<QString>("ffdefname");
    QTest::addColumn<QString>("fffieldname");
    QTest::addColumn<bool>("ffsetvalue");
    QTest::addColumn<QVariant>("ffvalue");
    QTest::addColumn<QVariant>("ffminrange");
    QTest::addColumn<QVariant>("ffmaxrange");
    QTest::addColumn<bool>("secondfilter");
    QTest::addColumn<int>("sftype");
    QTest::addColumn<QString>("sfdefname");
    QTest::addColumn<QString>("sffieldname");
    QTest::addColumn<bool>("sfsetvalue");
    QTest::addColumn<QVariant>("sfvalue");
    QTest::addColumn<QVariant>("sfminrange");
    QTest::addColumn<QVariant>("sfmaxrange");
    QTest::addColumn<QString>("order");
    QTest::addColumn<QString>("expected");

    QString es; // empty string.

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);

        // for the following tests, terminology:
        // X will be an (empty) intersection filter created in the test
        // Y will be the first filter defined here
        // Z will be the second filter defined here

        // WITH Y AND Z AS DETAIL FILTERS (with no overlap between Y and Z results)
        // For these tests, Y matches "bc" and Z matches "a"
        // X && Y - X empty so es
        QPair<QString, QString> integerDefAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Integer");
        QPair<QString, QString> booleanDefAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Bool");
        if (!integerDefAndFieldNames.first.isEmpty() && !integerDefAndFieldNames.second.isEmpty() && !booleanDefAndFieldNames.first.isEmpty() && !booleanDefAndFieldNames.second.isEmpty()) {
            QTest::newRow("A1") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(10) << QVariant() << QVariant()
                                << "XY" << es;
            // Y && X - X empty so es
            QTest::newRow("A2") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(10) << QVariant() << QVariant()
                                << "YX" << es;
            // Y && Z  - matches "a" and "bc" - so intersected = es
            QTest::newRow("A3") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "YZ" << es;
            // Z && Y - matches "bc" and "a" - so intersected = es
            QTest::newRow("A4") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "ZY" << es;
            // X && Z - X empty so es
            QTest::newRow("A5") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "XZ" << es;
            // Z && X - X empty so es
            QTest::newRow("A6") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "ZX" << es;
            // X && Y && Z - X empty so es
            QTest::newRow("A7") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "XYZ" << es;
            // X && Z && Y - X empty so es
            QTest::newRow("A8") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "XZY" << es;
            // Y && X && Z - X empty so es
            QTest::newRow("A9") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "YXZ" << es;
            // Z && X && Y - X empty so es
            QTest::newRow("A10") << manager
                                 << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                 << "ZXY" << es;
            // Y && Z && X - X empty so es
            QTest::newRow("A11") << manager
                                 << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                 << "YZX" << es;
            // Z && Y && X - X empty so es
            QTest::newRow("A12") << manager
                                 << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                 << "ZYX" << es;

            // WITH Y AND Z AS DETAIL FILTERS (with some overlap between Y and Z results)
            // For these tests, Y matches "bc", Z matches "b"
            // X && Y - X empty so es
            QTest::newRow("B1") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(20) << QVariant() << QVariant()
                                << "XY" << es;
            // Y && X - X empty so es
            QTest::newRow("B2") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(20) << QVariant() << QVariant()
                                << "YX" << es;
            // Y && Z  - matches "b" and "bc" - so intersected = "b"
            QTest::newRow("B3") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "YZ" << "b";
            // Z && Y - matches "bc" and "b" - so intersected = "b"
            QTest::newRow("B4") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "ZY" << "b";
            // X && Z - X empty so es
            QTest::newRow("B5") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "XZ" << es;
            // Z && X - X empty so es
            QTest::newRow("B6") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "ZX" << es;
            // X && Y && Z - X empty so es
            QTest::newRow("B7") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "XYZ" << es;
            // X && Z && Y - X empty so es
            QTest::newRow("B8") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "XZY" << es;
            // Y && X && Z - X empty so es
            QTest::newRow("B9") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "YXZ" << es;
            // Z && X && Y - X empty so es
            QTest::newRow("B10") << manager
                                 << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                 << "ZXY" << es;
            // Y && Z && X - X empty so es
            QTest::newRow("B11") << manager
                                 << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                 << "YZX" << es;
            // Z && Y && X - X empty so es
            QTest::newRow("B12") << manager
                                 << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                 << "ZYX" << es;
        }

        //---------------------------

        // WITH Y AND Z AS RANGE FILTERS (with no overlap between Y and Z results)
        // For these tests, Y matches "a", Z matches "b"
        // X && Y - X empty so es
        if (!integerDefAndFieldNames.first.isEmpty() && !integerDefAndFieldNames.second.isEmpty()) {
            QTest::newRow("C1") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XY" << es;
            // Y && X - X empty so es
            QTest::newRow("C2") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YX" << es;
            // Y && Z - no overlap so es
            QTest::newRow("C3") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YZ" << es;
            // Z && Y - no overlap so es
            QTest::newRow("C4") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZY" << es;
            // X && Z - X empty so es
            QTest::newRow("C5") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZ" << es;
            // Z && X - X empty so es
            QTest::newRow("C6") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZX" << es;
            // X && Y && Z - X empty so es
            QTest::newRow("C7") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XYZ" << es;
            // X && Z && Y - X empty so es
            QTest::newRow("C8") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZY" << es;
            // Y && X && Z - X empty so es
            QTest::newRow("C9") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YXZ" << es;
            // Z && X && Y - X empty so es
            QTest::newRow("C10") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZXY" << es;
            // Y && Z && X - X empty so es
            QTest::newRow("C11") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "YZX" << es;
            // Z && Y && X - X empty so es
            QTest::newRow("C12") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZYX" << es;

            // WITH Y AND Z AS RANGE FILTERS (with some overlap between Y and Z results)
            // For these tests, Y matches "ab", Z matches "b"
            // X && Y - X empty so es
            QTest::newRow("D1") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XY" << es;
            // Y && X - X empty so es
            QTest::newRow("D2") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YX" << es;
            // Y && Z - Y matches "ab", Z matches "b", intersection = "b"
            QTest::newRow("D3") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YZ" << "b";
            // Z && Y - Y matches "ab", Z matches "b", intersection = "b"
            QTest::newRow("D4") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZY" << "b";
            // X && Z - X empty so es
            QTest::newRow("D5") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZ" << es;
            // Z && X - X empty so es
            QTest::newRow("D6") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZX" << es;
            // X && Y && Z - X empty so es
            QTest::newRow("D7") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XYZ" << es;
            // X && Z && Y - X empty so es
            QTest::newRow("D8") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZY" << es;
            // Y && X && Z - X empty so es
            QTest::newRow("D9") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YXZ" << es;
            // Z && X && Y - X empty so es
            QTest::newRow("D10") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZXY" << es;
            // Y && Z && X - X empty so es
            QTest::newRow("D11") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "YZX" << es;
            // Z && Y && X - X empty so es
            QTest::newRow("D12") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZYX" << es;
        }
    }
}

void tst_QContactManagerFiltering::intersectionFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(bool, firstfilter);
    QFETCH(int, fftype); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QFETCH(QString, ffdefname);
    QFETCH(QString, fffieldname);
    QFETCH(bool, ffsetvalue);
    QFETCH(QVariant, ffvalue);
    QFETCH(QVariant, ffminrange);
    QFETCH(QVariant, ffmaxrange);
    QFETCH(bool, secondfilter);
    QFETCH(int, sftype);
    QFETCH(QString, sfdefname);
    QFETCH(QString, sffieldname);
    QFETCH(bool, sfsetvalue);
    QFETCH(QVariant, sfvalue);
    QFETCH(QVariant, sfminrange);
    QFETCH(QVariant, sfmaxrange);
    QFETCH(QString, order);
    QFETCH(QString, expected);

    QContactFilter *x = new QContactIntersectionFilter();
    QContactFilter *y = 0, *z = 0;

    if (firstfilter) {
        switch (fftype) {
            case 1: // detail filter
                y = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                if (ffsetvalue)
                    static_cast<QContactDetailFilter*>(y)->setValue(ffvalue);
                break;
            case 2: // range filter
                y = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                static_cast<QContactDetailRangeFilter*>(y)->setRange(ffminrange, ffmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    if (secondfilter) {
        switch (sftype) {
            case 1: // detail filter
                z = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                if (sfsetvalue)
                    static_cast<QContactDetailFilter*>(z)->setValue(sfvalue);
                break;
            case 2: // range filter
                z = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                static_cast<QContactDetailRangeFilter*>(z)->setRange(sfminrange, sfmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    // control variables - order: starts, ends, mids
    bool sX = false;
    bool sY = false;
    bool sZ = false;
    bool eX = false;
    bool eY = false;
    bool eZ = false;
    bool mX = false;
    bool mY = false;
    bool mZ = false;

    if (order.startsWith("X"))
        sX = true;
    if (order.startsWith("Y"))
        sY = true;
    if (order.startsWith("Z"))
        sZ = true;
    if (order.endsWith("X"))
        eX = true;
    if (order.endsWith("Y"))
        eY = true;
    if (order.endsWith("Z"))
        eZ = true;
    if (order.size() > 2) {
        if (order.at(1) == 'X')
            mX = true;
        if (order.at(1) == 'Y')
            mY = true;
        if (order.at(1) == 'Z')
            mZ = true;
    }

    // now perform the filtering.
    QContactIntersectionFilter resultFilter;
    if (sX) {
        if (mY && eZ)
            resultFilter = *x && *y && *z;
        else if (mZ && eY)
            resultFilter = *x && *z && *y;
        else if (eY)
            resultFilter = *x && *y;
        else if (eZ)
            resultFilter = *x && *z;
    } else if (sY) {
        if (mX && eZ)
            resultFilter = *y && *x && *z;
        else if (mZ && eX)
            resultFilter = *y && *z && *x;
        else if (eX)
            resultFilter = *y && *x;
        else if (eZ)
            resultFilter = *y && *z;
    } else if (sZ) {
        if (mX && eY)
            resultFilter = *z && *x && *y;
        else if (mY && eX)
            resultFilter = *z && *y && *x;
        else if (eX)
            resultFilter = *z && *x;
        else if (eY)
            resultFilter = *z && *y;
    }

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> ids;

    ids = cm->contacts(resultFilter);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    delete x;
    if (y) delete y;
    if (z) delete z;
}

void tst_QContactManagerFiltering::unionFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<bool>("firstfilter");
    QTest::addColumn<int>("fftype"); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QTest::addColumn<QString>("ffdefname");
    QTest::addColumn<QString>("fffieldname");
    QTest::addColumn<bool>("ffsetvalue");
    QTest::addColumn<QVariant>("ffvalue");
    QTest::addColumn<QVariant>("ffminrange");
    QTest::addColumn<QVariant>("ffmaxrange");
    QTest::addColumn<bool>("secondfilter");
    QTest::addColumn<int>("sftype");
    QTest::addColumn<QString>("sfdefname");
    QTest::addColumn<QString>("sffieldname");
    QTest::addColumn<bool>("sfsetvalue");
    QTest::addColumn<QVariant>("sfvalue");
    QTest::addColumn<QVariant>("sfminrange");
    QTest::addColumn<QVariant>("sfmaxrange");
    QTest::addColumn<QString>("order");
    QTest::addColumn<QString>("expected");

    QString es; // empty string.

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);

        // for the following tests, terminology:
        // X will be an (empty) union filter created in the test
        // Y will be the first filter defined here
        // Z will be the second filter defined here

        // WITH Y AND Z AS DETAIL FILTERS (with no overlap between Y and Z results)
        // For these tests, Y matches "bc" and Z matches "a"
        // X || Y - X empty, Y matches "bc" so union = "bc"
        QPair<QString, QString> integerDefAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Integer");
        QPair<QString, QString> booleanDefAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Bool");
        if (!integerDefAndFieldNames.first.isEmpty() && !integerDefAndFieldNames.second.isEmpty() && !booleanDefAndFieldNames.first.isEmpty() && !booleanDefAndFieldNames.second.isEmpty()) {
            QTest::newRow("A1") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(10) << QVariant() << QVariant()
                                << "XY" << "bc";
            // Y || X - Y matches "bc", X empty so union = "bc"
            QTest::newRow("A2") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(10) << QVariant() << QVariant()
                                << "YX" << "bc";
            // Y || Z  - Y matches "bc" and Z matches "a" - so union = "abc"
            QTest::newRow("A3") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "YZ" << "abc";
            // Z || Y - Y matches "bc" and Z matches "a" - so union = "abc"
            QTest::newRow("A4") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "ZY" << "abc";
            // X || Z - X empty, Z matches "a" so "a"
            QTest::newRow("A5") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << false << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "XZ" << "a";
            // Z || X - X empty, Z matches "a" so "a"
            QTest::newRow("A6") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << false << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "ZX" << "a";
            // X || Y || Z - X empty, Y matches "bc", Z matches "a" so "abc"
            QTest::newRow("A7") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "XYZ" << "abc";
            // X || Z || Y - X empty, Y matches "bc", Z matches "a" so "abc"
            QTest::newRow("A8") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "XZY" << "abc";
            // Y || X || Z - X empty, Y matches "bc", Z matches "a" so "abc"
            QTest::newRow("A9") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                << "YXZ" << "abc";
            // Z || X || Y - X empty, Y matches "bc", Z matches "a" so "abc"
            QTest::newRow("A10") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                 << "ZXY" << "abc";
            // Y || Z || X - X empty, Y matches "bc", Z matches "a" so "abc"
            QTest::newRow("A11") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                 << "YZX" << "abc";
            // Z || Y || X - X empty, Y matches "bc", Z matches "a" so "abc"
            QTest::newRow("A12") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(10) << QVariant() << QVariant()
                                 << "ZYX" << "abc";

            // WITH Y AND Z AS DETAIL FILTERS (with some overlap between Y and Z results)
            // For these tests, Y matches "bc", Z matches "b"
            // X || Y - X empty, Y matches "b", so "bc"
            QTest::newRow("B1") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(20) << QVariant() << QVariant()
                                << "XY" << "bc";
            // Y || X - X empty, Y matches "bc", so "bc"
            QTest::newRow("B2") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << false << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(20) << QVariant() << QVariant()
                                << "YX" << "bc";
            // Y || Z  - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B3") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "YZ" << "bc";
            // Z || Y - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B4") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "ZY" << "bc";
            // X || Z - X empty, Z matches "b" so "b"
            QTest::newRow("B5") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "XZ" << "b";
            // Z || X - X empty, Z matches "b" so "b"
            QTest::newRow("B6") << manager
                                << false << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "ZX" << "b";
            // X || Y || Z - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B7") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "XYZ" << "bc";
            // X || Z || Y - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B8") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "XZY" << "bc";
            // Y || X || Z - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B9") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                << "YXZ" << "bc";
            // Z || X || Y - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B10") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                 << "ZXY" << "bc";
            // Y || Z || X - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B11") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                 << "YZX" << "bc";
            // Z || Y || X - X empty, Y matches "bc", Z matches "b" so "bc"
            QTest::newRow("B12") << manager
                                << true << 1 << booleanDefAndFieldNames.first << booleanDefAndFieldNames.second << true << QVariant(false) << QVariant() << QVariant()
                                 << true << 1 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << true << QVariant(20) << QVariant() << QVariant()
                                 << "ZYX" << "bc";
        }

        //---------------------------

        // WITH Y AND Z AS RANGE FILTERS (with no overlap between Y and Z results)
        // For these tests, Y matches "a", Z matches "b"
        // X || Y - X empty, Y matches "a" so "a"
        if (!integerDefAndFieldNames.first.isEmpty() && !integerDefAndFieldNames.second.isEmpty()) {
            QTest::newRow("C1") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XY" << "a";
            // Y || X - X empty, Y matches "a" so "a"
            QTest::newRow("C2") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YX" << "a";
            // Y || Z - Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C3") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YZ" << "ab";
            // Z || Y - Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C4") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZY" << "ab";
            // X || Z - X empty, Z matches "b" so "b"
            QTest::newRow("C5") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZ" << "b";
            // Z || X - X empty, Z matches "b" so "b"
            QTest::newRow("C6") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZX" << "b";
            // X || Y || Z - X empty, Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C7") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XYZ" << "ab";
            // X || Z || Y - X empty, Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C8") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZY" << "ab";
            // Y || X || Z - X empty, Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C9") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YXZ" << "ab";
            // Z || X || Y - X empty, Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C10") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZXY" << "ab";
            // Y || Z || X - X empty, Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C11") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "YZX" << "ab";
            // Z || Y || X - X empty, Y matches "a", Z matches "b" so "ab"
            QTest::newRow("C12") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(15)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZYX" << "ab";

            // WITH Y AND Z AS RANGE FILTERS (with some overlap between Y and Z results)
            // For these tests, Y matches "ab", Z matches "b"
            // X || Y - X empty, Y matches "ab" so "ab"
            QTest::newRow("D1") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XY" << "ab";
            // Y || X - X empty, Y matches "ab" so "ab"
            QTest::newRow("D2") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YX" << "ab";
            // Y || Z - Y matches "ab", Z matches "b", union = "ab"
            QTest::newRow("D3") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YZ" << "ab";
            // Z || Y - Y matches "ab", Z matches "b", union = "ab"
            QTest::newRow("D4") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZY" << "ab";
            // X || Z - X empty, Z matches "b" so "b"
            QTest::newRow("D5") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZ" << "b";
            // Z || X - X empty, Z matches "b" so "b"
            QTest::newRow("D6") << manager
                                << false << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "ZX" << "b";
            // X || Y || Z - X empty, Y matches "ab", Z matches "b" so "ab"
            QTest::newRow("D7") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XYZ" << "ab";
            // X || Z || Y - X empty, Y matches "ab", Z matches "b" so "ab"
            QTest::newRow("D8") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "XZY" << "ab";
            // Y || X || Z - X empty, Y matches "ab", Z matches "b" so "ab"
            QTest::newRow("D9") << manager
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                << "YXZ" << "ab";
            // Z || X || Y - X empty, Y matches "ab", Z matches "b" so "ab"
            QTest::newRow("D10") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZXY" << "ab";
            // Y || Z || X - X empty, Y matches "ab", Z matches "b" so "ab"
            QTest::newRow("D11") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "YZX" << "ab";
            // Z || Y || X - X empty, Y matches "ab", Z matches "b" so "ab"
            QTest::newRow("D12") << manager
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(5) << QVariant(25)
                                 << true << 2 << integerDefAndFieldNames.first << integerDefAndFieldNames.second << false << QVariant(0) << QVariant(15) << QVariant(25)
                                 << "ZYX" << "ab";
        }
    }
}

void tst_QContactManagerFiltering::unionFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(bool, firstfilter);
    QFETCH(int, fftype); // 1 = detail, 2 = detailrange, 3 = groupmembership, 4 = union, 5 = intersection
    QFETCH(QString, ffdefname);
    QFETCH(QString, fffieldname);
    QFETCH(bool, ffsetvalue);
    QFETCH(QVariant, ffvalue);
    QFETCH(QVariant, ffminrange);
    QFETCH(QVariant, ffmaxrange);
    QFETCH(bool, secondfilter);
    QFETCH(int, sftype);
    QFETCH(QString, sfdefname);
    QFETCH(QString, sffieldname);
    QFETCH(bool, sfsetvalue);
    QFETCH(QVariant, sfvalue);
    QFETCH(QVariant, sfminrange);
    QFETCH(QVariant, sfmaxrange);
    QFETCH(QString, order);
    QFETCH(QString, expected);

    QContactFilter *x = new QContactUnionFilter();
    QContactFilter *y = 0, *z = 0;

    if (firstfilter) {
        switch (fftype) {
            case 1: // detail filter
                y = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                if (ffsetvalue)
                    static_cast<QContactDetailFilter*>(y)->setValue(ffvalue);
                break;
            case 2: // range filter
                y = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(y)->setDetailDefinitionName(ffdefname, fffieldname);
                static_cast<QContactDetailRangeFilter*>(y)->setRange(ffminrange, ffmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    if (secondfilter) {
        switch (sftype) {
            case 1: // detail filter
                z = new QContactDetailFilter();
                static_cast<QContactDetailFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                if (sfsetvalue)
                    static_cast<QContactDetailFilter*>(z)->setValue(sfvalue);
                break;
            case 2: // range filter
                z = new QContactDetailRangeFilter();
                static_cast<QContactDetailRangeFilter*>(z)->setDetailDefinitionName(sfdefname, sffieldname);
                static_cast<QContactDetailRangeFilter*>(z)->setRange(sfminrange, sfmaxrange);
                break;
            case 3: // group membership filter
            case 4: // union filter
            case 5: // intersection filter
                break;

            default:
                QVERIFY(false); // force fail.
            break;
        }
    }

    // control variables - order: starts, ends, mids
    bool sX = false;
    bool sY = false;
    bool sZ = false;
    bool eX = false;
    bool eY = false;
    bool eZ = false;
    bool mX = false;
    bool mY = false;
    bool mZ = false;

    if (order.startsWith("X"))
        sX = true;
    if (order.startsWith("Y"))
        sY = true;
    if (order.startsWith("Z"))
        sZ = true;
    if (order.endsWith("X"))
        eX = true;
    if (order.endsWith("Y"))
        eY = true;
    if (order.endsWith("Z"))
        eZ = true;
    if (order.size() > 2) {
        if (order.at(1) == 'X')
            mX = true;
        if (order.at(1) == 'Y')
            mY = true;
        if (order.at(1) == 'Z')
            mZ = true;
    }

    // now perform the filtering.
    QContactUnionFilter resultFilter;
    if (sX) {
        if (mY && eZ)
            resultFilter = *x || *y || *z;
        else if (mZ && eY)
            resultFilter = *x || *z || *y;
        else if (eY)
            resultFilter = *x || *y;
        else if (eZ)
            resultFilter = *x || *z;
    } else if (sY) {
        if (mX && eZ)
            resultFilter = *y || *x || *z;
        else if (mZ && eX)
            resultFilter = *y || *z || *x;
        else if (eX)
            resultFilter = *y || *x;
        else if (eZ)
            resultFilter = *y || *z;
    } else if (sZ) {
        if (mX && eY)
            resultFilter = *z || *x || *y;
        else if (mY && eX)
            resultFilter = *z || *y || *x;
        else if (eX)
            resultFilter = *z || *x;
        else if (eY)
            resultFilter = *z || *y;
    }

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> ids;

    ids = cm->contacts(resultFilter);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    delete x;
    if (y) delete y;
    if (z) delete z;
}

void tst_QContactManagerFiltering::sorting_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QString>("defname");
    QTest::addColumn<QString>("fieldname");
    QTest::addColumn<int>("directioni");
    QTest::addColumn<bool>("setbp");
    QTest::addColumn<int>("blankpolicyi");
    QTest::addColumn<QString>("expected");

    QString firstname = QContactName::FieldFirst;
    QString lastname = QContactName::FieldLast;
    QString namedef = QContactName::DefinitionName;
    QString urldef = QContactUrl::DefinitionName;
    QString urlfield = QContactUrl::FieldUrl;

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);

        QPair<QString, QString> integerDefAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("Integer");
        QPair<QString, QString> stringDefAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("String");

        QTest::newRow("first ascending") << manager << namedef << firstname << (int)(Qt::AscendingOrder) << false << 0 << "abcdefg";
        QTest::newRow("first descending") << manager << namedef << firstname << (int)(Qt::DescendingOrder) << false << 0 << "efgdcba";
        QTest::newRow("last ascending") << manager << namedef << lastname << (int)(Qt::AscendingOrder) << false << 0 << "bacdefg";
        QTest::newRow("last descending") << manager << namedef << lastname << (int)(Qt::DescendingOrder) << false << 0 << "gfedcab";
        if (!integerDefAndFieldNames.first.isEmpty() && !integerDefAndFieldNames.second.isEmpty()) {
            QTest::newRow("integer ascending, blanks last") << manager << integerDefAndFieldNames.first << integerDefAndFieldNames.second << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "cabgfed";
            QTest::newRow("integer descending, blanks last") << manager << integerDefAndFieldNames.first << integerDefAndFieldNames.second << (int)(Qt::DescendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "bacgfed";
            QTest::newRow("integer ascending, blanks first") << manager << integerDefAndFieldNames.first << integerDefAndFieldNames.second << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "defgcab";
            QTest::newRow("integer descending, blanks first") << manager << integerDefAndFieldNames.first << integerDefAndFieldNames.second << (int)(Qt::DescendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "defgbac";
        }
        if (!stringDefAndFieldNames.first.isEmpty() && !stringDefAndFieldNames.second.isEmpty()) {
            QTest::newRow("string ascending (null value), blanks first") << manager << stringDefAndFieldNames.first << stringDefAndFieldNames.second << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksFirst) << "fgeabcd";
            QTest::newRow("string ascending (null value), blanks last") << manager << stringDefAndFieldNames.first << stringDefAndFieldNames.second << (int)(Qt::AscendingOrder) << true << (int)(QContactSortOrder::BlanksLast) << "eabcdgf";
        }
    }
}

void tst_QContactManagerFiltering::sorting()
{
    QFETCH(QContactManager*, cm);
    QFETCH(QString, defname);
    QFETCH(QString, fieldname);
    QFETCH(int, directioni);
    QFETCH(bool, setbp);
    QFETCH(int, blankpolicyi);
    QFETCH(QString, expected);

    Qt::SortOrder direction = (Qt::SortOrder)directioni;
    QContactSortOrder::BlankPolicy blankpolicy = (QContactSortOrder::BlankPolicy)blankpolicyi;

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> ids;

    /* Build the sort order */
    QContactSortOrder s;
    s.setDetailDefinitionName(defname, fieldname);
    s.setDirection(direction);
    if (setbp)
        s.setBlankPolicy(blankpolicy);

    ids = cm->contacts(s);
    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);

    /* Now do a check with a filter involved; the filter should not affect the sort order */
    QContactDetailFilter presenceName;
    presenceName.setDetailDefinitionName(QContactName::DefinitionName);
    ids = cm->contacts(presenceName, s);
    output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
}

void tst_QContactManagerFiltering::multiSorting_data()
{
    QTest::addColumn<QContactManager *>("cm");

    QTest::addColumn<bool>("firstsort");
    QTest::addColumn<QString>("fsdefname");
    QTest::addColumn<QString>("fsfieldname");
    QTest::addColumn<int>("fsdirectioni");

    QTest::addColumn<bool>("secondsort");
    QTest::addColumn<QString>("ssdefname");
    QTest::addColumn<QString>("ssfieldname");
    QTest::addColumn<int>("ssdirectioni");

    QTest::addColumn<QString>("expected");


    QString es;

    QString firstname = QContactName::FieldFirst;
    QString lastname = QContactName::FieldLast;
    QString namedef = QContactName::DefinitionName;
    QString phonedef = QContactPhoneNumber::DefinitionName;
    QString numberfield = QContactPhoneNumber::FieldNumber;

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);
        QPair<QString, QString> stringDefAndFieldNames = defAndFieldNamesForTypePerManager.value(manager).value("String");

        QTest::newRow("1") << manager
                           << true << namedef << firstname << (int)(Qt::AscendingOrder)
                           << true << namedef << lastname << (int)(Qt::AscendingOrder)
                           << "abcdefg";
        QTest::newRow("2") << manager
                           << true << namedef << firstname << (int)(Qt::AscendingOrder)
                           << true << namedef << lastname << (int)(Qt::DescendingOrder)
                           << "abcdgfe";
        QTest::newRow("3") << manager
                           << true << namedef << firstname << (int)(Qt::DescendingOrder)
                           << true << namedef << lastname << (int)(Qt::AscendingOrder)
                           << "efgdcba";
        QTest::newRow("4") << manager
                           << true << namedef << firstname << (int)(Qt::DescendingOrder)
                           << true << namedef << lastname << (int)(Qt::DescendingOrder)
                           << "gfedcba";

        QTest::newRow("5") << manager
                           << true << namedef << firstname << (int)(Qt::AscendingOrder)
                           << false << namedef << lastname << (int)(Qt::AscendingOrder)
                           << "abcdefg";

        QTest::newRow("5b") << manager
                           << true << namedef << firstname << (int)(Qt::AscendingOrder)
                           << true << es << es << (int)(Qt::AscendingOrder)
                           << "abcdefg";

        QTest::newRow("6") << manager
                           << false << namedef << firstname << (int)(Qt::AscendingOrder)
                           << true << namedef << lastname << (int)(Qt::AscendingOrder)
                           << "bacdefg";

        QTest::newRow("7") << manager
                           << false << namedef << firstname << (int)(Qt::AscendingOrder)
                           << false << namedef << lastname << (int)(Qt::AscendingOrder)
                           << "abcdefg";

        if (!stringDefAndFieldNames.first.isEmpty() && !stringDefAndFieldNames.second.isEmpty()) {
            QTest::newRow("8") << manager
                               << true << stringDefAndFieldNames.first << stringDefAndFieldNames.second << (int)(Qt::AscendingOrder)
                               << false << stringDefAndFieldNames.first << stringDefAndFieldNames.second << (int)(Qt::DescendingOrder)
                               << "eabcdgf";

            QTest::newRow("8b") << manager
                               << true << stringDefAndFieldNames.first << stringDefAndFieldNames.second << (int)(Qt::AscendingOrder)
                               << false << es << es << (int)(Qt::DescendingOrder)
                               << "eabcdgf";
        }

        QTest::newRow("9") << manager
                           << true << phonedef << numberfield << (int)(Qt::AscendingOrder)
                           << true << namedef << lastname << (int)(Qt::DescendingOrder)
                           << "abgfedc";

        QTest::newRow("10") << manager
                            << true << namedef << firstname << (int)(Qt::AscendingOrder)
                            << true << namedef << firstname << (int)(Qt::DescendingOrder)
                            << "abcdefg";

    }
}

void tst_QContactManagerFiltering::multiSorting()
{
    QFETCH(QContactManager*, cm);
    QFETCH(bool, firstsort);
    QFETCH(QString, fsdefname);
    QFETCH(QString, fsfieldname);
    QFETCH(int, fsdirectioni);
    QFETCH(bool, secondsort);
    QFETCH(QString, ssdefname);
    QFETCH(QString, ssfieldname);
    QFETCH(int, ssdirectioni);
    QFETCH(QString, expected);

    Qt::SortOrder fsdirection = (Qt::SortOrder)fsdirectioni;
    Qt::SortOrder ssdirection = (Qt::SortOrder)ssdirectioni;

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);

    /* Build the sort orders */
    QContactSortOrder fs;
    fs.setDetailDefinitionName(fsdefname, fsfieldname);
    fs.setDirection(fsdirection);
    QContactSortOrder ss;
    ss.setDetailDefinitionName(ssdefname, ssfieldname);
    ss.setDirection(ssdirection);
    QList<QContactSortOrder> sortOrders;
    if (firstsort)
        sortOrders.append(fs);
    if (secondsort)
        sortOrders.append(ss);

    QList<QUniqueId> ids = cm->contacts(sortOrders);
    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
}

void tst_QContactManagerFiltering::actionPlugins()
{
    QStringList actions = QContactAction::availableActions();
    QVERIFY(actions.contains("Boolean"));
    QVERIFY(actions.contains("Number"));

    /* Ignore the version if the vendor is not set */
    actions = QContactAction::availableActions(QString(), 555);
    QVERIFY(actions.contains("Boolean"));
    QVERIFY(actions.contains("Number"));

    actions = QContactAction::availableActions("NumberCo");
    QVERIFY(actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactAction::availableActions("IntegerCo");
    QVERIFY(actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactAction::availableActions("BooleanCo");
    QVERIFY(!actions.contains("Number"));
    QVERIFY(actions.contains("Boolean"));

    actions = QContactAction::availableActions("IntegerCo", 5);
    QVERIFY(actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactAction::availableActions("IntegerCo", 3);
    QVERIFY(!actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));

    actions = QContactAction::availableActions("BooleanCo", 3);
    QVERIFY(!actions.contains("Number"));
    QVERIFY(actions.contains("Boolean"));

    actions = QContactAction::availableActions("BooleanCo", 555);
    QVERIFY(!actions.contains("Number"));
    QVERIFY(!actions.contains("Boolean"));
}

void tst_QContactManagerFiltering::actionFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QString>("actionName");
    QTest::addColumn<QString>("vendorName");
    QTest::addColumn<int>("version");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("expected");

    QString es;
    QVariant ev;

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);

        QTest::newRow("empty (any action matches)") << manager << es << es << -1 << ev << "abcd";
        QTest::newRow("bad actionname") << manager << "No such action" << es << -1 << ev << es;
        QTest::newRow("bad vendor") << manager << es << "Vendor missing" << -1 << ev << es;
        /* versions are ignored if vendors are not specified */
        QTest::newRow("ignored version") << manager << es << es << 793434 << ev << "abcd";

        QTest::newRow("Number") << manager << "Number" << es << -1 << ev << "abcd";
        QTest::newRow("Number (IntegerCo)") << manager << "Number" << "IntegerCo" << -1 << ev << "abc";
        QTest::newRow("Number (NumberCo)") << manager << "Number" << "NumberCo" << -1 << ev << "abcd";
        QTest::newRow("Number (BooleanCo)") << manager << "Number" << "BooleanCo" << -1 << ev << es;

        QTest::newRow("Number (IntegerCo, good version)") << manager << "Number" << "IntegerCo" << 5 << ev << "abc";
        QTest::newRow("Number (NumberCo, good version)") << manager << "Number" << "NumberCo" << 42 << ev << "abcd";

        QTest::newRow("Number (IntegerCo, bad version)") << manager << "Number" << "IntegerCo" << 345345 << ev << es;
        QTest::newRow("Number (NumberCo, bad version)") << manager << "Number" << "NumberCo" << 7547544 << ev << es;

        /* versions are ignored if vendors are not specified */
        QTest::newRow("Number (ignored version)") << manager << "Number" << es << 345345 << ev << "abcd";

        /* Vendor specific */
        QTest::newRow("NumberCo") << manager << es << "NumberCo" << -1 << ev << "abcd";
        QTest::newRow("NumberCo (good version)") << manager << es << "NumberCo" << 42 << ev << "abcd";
        QTest::newRow("NumberCo (bad version)") << manager << es << "NumberCo" << 41 << ev << es;

        QTest::newRow("IntegerCo") << manager << es << "IntegerCo" << -1 << ev << "abc";
        QTest::newRow("IntegerCo (good version)") << manager << es << "IntegerCo" << 5 << ev << "abc";
        QTest::newRow("IntegerCo (bad version)") << manager << es << "IntegerCo" << 41 << ev << es;


        /* Boolean testing */
        QTest::newRow("Boolean action") << manager << "Boolean" << es << -1 << ev << "a";
        QTest::newRow("BooleanCo") << manager << es << "BooleanCo" << -1 << ev << "a";
        QTest::newRow("BooleanCo (good version)") << manager << es << "BooleanCo" << 3 << ev << "a";
        QTest::newRow("BooleanCo (bad version)") << manager << es << "BooleanCo" << 3234243 << ev << es;

        /* Value filtering */
        QTest::newRow("Any action matching 20") << manager << es << es << -1 << QVariant(20) << "b";
        QTest::newRow("Any action matching 4.0") << manager << es << es << -1 << QVariant(4.0) << "bc";
        QTest::newRow("NumberCo with 20") << manager << es << "NumberCo" << -1 << QVariant(20) << "b";
        QTest::newRow("NumberCo with 4.0") << manager << es << "NumberCo" << -1 << QVariant(4.0) << "bc";
        QTest::newRow("IntegerCo with 20") << manager << es << "IntegerCo" << -1 << QVariant(20) << "b";
        QTest::newRow("IntegerCo with 4.0") << manager << es << "IntegerCo" << -1 << QVariant(4.0) << es;
        QTest::newRow("Boolean action matching true") << manager << es << "BooleanCo" << -1 << QVariant(true) << "a";
        QTest::newRow("Boolean action matching false") << manager << es << "BooleanCo" << -1 << QVariant(false) << es;

        /* Recursive filtering */
        QTest::newRow("Recursive action 1") << manager << "IntersectionRecursive" << es << -1 << QVariant(false) << es;
        QTest::newRow("Recursive action 2") << manager << "UnionRecursive" << es << -1 << QVariant(false) << es;
        QTest::newRow("Recursive action 3") << manager << "PairRecursive" << es << -1 << QVariant(false) << es;
        QTest::newRow("Recursive action 4") << manager << "AnotherPairRecursive" << es << -1 << QVariant(false) << es;
        QTest::newRow("Recursive action 5") << manager << "Recursive" << es << -1 << QVariant(false) << es;
    }
}

void tst_QContactManagerFiltering::actionFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(QString, actionName);
    QFETCH(QString, vendorName);
    QFETCH(int, version);
    QFETCH(QVariant, value);
    QFETCH(QString, expected);

    /* Load the definition and field names for the various variant types for the current manager */
    defAndFieldNamesForTypeForActions = defAndFieldNamesForTypePerManager.value(cm);

    QContactActionFilter af;
    af.setActionName(actionName);
    af.setValue(value);
    af.setVendor(vendorName, version);

    QList<QUniqueId> ids = cm->contacts(af);
    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
}

void tst_QContactManagerFiltering::idListFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expected");

    QString es;

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);
        QTest::newRow("empty") << manager << es << es;
        QTest::newRow("a") << manager << "a" << "a";
        QTest::newRow("ab") << manager << "ab" << "ab";
        QTest::newRow("aa") << manager << "aa" << "a";
        QTest::newRow("ba") << manager << "ba" << "ab";
        QTest::newRow("abcd") << manager << "abcd" << "abcd";
        QTest::newRow("abcdefg") << manager << "abcdefg" << "abcd";
    }
}

void tst_QContactManagerFiltering::idListFiltering()
{
    QFETCH(QContactManager*, cm);
    QFETCH(QString, input);
    QFETCH(QString, expected);

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QList<QUniqueId> ids;

    // 3 extra ids that (hopefully) won't exist
    QUniqueId e = 0x54555657;
    QUniqueId f = 0x96969696;
    QUniqueId g = 0x44335566;

    /* Convert the input to a list of ids */
    foreach(QChar c, input) {
        if (c == 'a')
            ids << contacts.at(0);
        else if (c == 'b')
            ids << contacts.at(1);
        else if (c == 'c')
            ids << contacts.at(2);
        else if (c == 'd')
            ids << contacts.at(3);
        else if (c == 'e')
            ids << e;
        else if (c == 'f')
            ids << f;
        else if (c == 'g')
            ids << g;
    }

    /* And do the search */
    QContactIdListFilter idf;
    idf.setIds(ids);

    // now reuse ids
    ids = cm->contacts(idf);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
}

void tst_QContactManagerFiltering::invalidFiltering_data()
{
    QTest::addColumn<QContactManager*>("cm");

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);
        QTest::newRow(manager->managerName().toAscii().constData()) << manager;
    }
}

void tst_QContactManagerFiltering::invalidFiltering()
{
    QFETCH(QContactManager*, cm);

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QContactInvalidFilter f; // invalid
    QList<QUniqueId> ids = cm->contacts(f);
    QVERIFY(ids.count() == 0);

    // Try unions/intersections of invalids too
    ids = cm->contacts(f || f);
    QVERIFY(ids.count() == 0);

    ids = cm->contacts(f && f);
    QVERIFY(ids.count() == 0);
}

void tst_QContactManagerFiltering::allFiltering_data()
{
    QTest::addColumn<QContactManager*>("cm");

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);
        QTest::newRow(manager->managerName().toAscii().constData()) << manager;
    }
}

void tst_QContactManagerFiltering::allFiltering()
{
    QFETCH(QContactManager*, cm);

    QList<QUniqueId> contacts = contactsAddedToManagers.values(cm);
    QContactFilter f; // default = permissive
    QList<QUniqueId> ids = cm->contacts(f);
    QVERIFY(ids.count() == contacts.size());

    // Try unions/intersections of defaults
    ids = cm->contacts(f || f);
    QVERIFY(ids.count() == contacts.size());

    ids = cm->contacts(f && f);
    QVERIFY(ids.count() == contacts.size());
}

void tst_QContactManagerFiltering::changelogFiltering_data()
{
    QTest::addColumn<QContactManager *>("cm");
    QTest::addColumn<QList<QUniqueId> >("contacts");
    QTest::addColumn<int>("changeType");
    QTest::addColumn<QDateTime>("since");
    QTest::addColumn<QString>("expected");

    int added = (int)QContactChangeLogFilter::Added;
    int changed = (int)QContactChangeLogFilter::Changed;
    int removed = (int)QContactChangeLogFilter::Removed;

    for (int i = 0; i < managers.size(); i++) {
        QContactManager *manager = managers.at(i);
        QList<QUniqueId> contacts = contactsAddedToManagers.values(manager);
        QContact a,b,c,d;
        a = manager->contact(contacts.at(0));
        b = manager->contact(contacts.at(1));
        c = manager->contact(contacts.at(2));
        d = manager->contact(contacts.at(3));

        QDateTime ac = a.detail<QContactTimestamp>().created();
        QDateTime bc = b.detail<QContactTimestamp>().created();
        QDateTime cc = c.detail<QContactTimestamp>().created();
        QDateTime dc = d.detail<QContactTimestamp>().created();

        QDateTime am = a.detail<QContactTimestamp>().lastModified();
        QDateTime bm = b.detail<QContactTimestamp>().lastModified();
        QDateTime cm = c.detail<QContactTimestamp>().lastModified();
        QDateTime dm = d.detail<QContactTimestamp>().lastModified();

        QTest::newRow("Added since before start") << manager << contacts << added << ac.addSecs(-1) << "abcdefg";
        QTest::newRow("Added since first") << manager << contacts << added << ac << "abcdefg";
        QTest::newRow("Added since second") << manager << contacts << added << bc << "bcdefg";
        QTest::newRow("Added since third") << manager << contacts << added << cc << "cdefg";
        QTest::newRow("Added since fourth") << manager << contacts << added << dc << "defg";
        QTest::newRow("Added since after fourth") << manager << contacts << added << dc.addSecs(1) << "efg";
        QTest::newRow("Added since first changed") << manager << contacts << added << am << "";
        QTest::newRow("Added since second changed") << manager << contacts << added << bm << "";
        QTest::newRow("Added since third changed") << manager << contacts << added << cm << "";
        QTest::newRow("Added since fourth changed") << manager << contacts << added << cm << "";

        QTest::newRow("Changed since before start") << manager << contacts << changed << ac.addSecs(-1) << "abcdefg";
        QTest::newRow("Changed since first") << manager << contacts << changed << ac << "abcdefg";
        QTest::newRow("Changed since second") << manager << contacts << changed << bc << "abcdefg";
        QTest::newRow("Changed since third") << manager << contacts << changed << cc << "abcdefg";
        QTest::newRow("Changed since fourth") << manager << contacts << changed << dc << "abcdefg";
        QTest::newRow("Changed since after fourth") << manager << contacts << changed << dc.addSecs(1) << "abcefg";
        QTest::newRow("Changed since first changed") << manager << contacts << changed << am << "a";
        QTest::newRow("Changed since second changed") << manager << contacts << changed << bm << "ab";
        QTest::newRow("Changed since third changed") << manager << contacts << changed << cm << "abc";
        QTest::newRow("Changed since fourth changed") << manager << contacts << changed << dm << "abcdefg";

        // These are currently useless..
        QTest::newRow("Removed since before start") << manager << contacts << removed << ac.addSecs(-1) << "";
        QTest::newRow("Removed since first") << manager << contacts << removed << ac << "";
        QTest::newRow("Removed since second") << manager << contacts << removed << bc << "";
        QTest::newRow("Removed since third") << manager << contacts << removed << cc << "";
        QTest::newRow("Removed since fourth") << manager << contacts << removed << dc << "";
        QTest::newRow("Removed since after fourth") << manager << contacts << removed << dc.addSecs(1) << "";
    }
}

void tst_QContactManagerFiltering::changelogFiltering()
{
    QFETCH(int, changeType);
    QFETCH(QDateTime, since);
    QFETCH(QString, expected);
    QFETCH(QContactManager*, cm);
    QFETCH(QList<QUniqueId>, contacts);

    QList<QUniqueId> ids;

    QContactChangeLogFilter clf((QContactChangeLogFilter::ChangeType)changeType);
    clf.setSince(since);

    ids = cm->contacts(clf);

    QString output = convertIds(contacts, ids);
    QCOMPARE(output, expected);
}

QPair<QString, QString> tst_QContactManagerFiltering::definitionAndField(QContactManager *cm, QVariant::Type type, bool *nativelyFilterable)
{
    QPair<QString, QString> result;
    QString definitionName, fieldName;

    // step one: search for an existing definition with a field of the specified type
    QContactManagerInfo *info = cm->information();
    QMap<QString, QContactDetailDefinition> allDefs = cm->detailDefinitions();
    QStringList defNames = allDefs.keys();
    bool found = false;
    bool isNativelyFilterable = false;
    foreach (const QString& defName, defNames) {
        // check the current definition.
        QContactDetailDefinition def = allDefs.value(defName);

        // if unique or read/create only, we cannot use this definition.
        if (def.isUnique() || def.accessConstraint() != QContactDetailDefinition::Any) {
            continue;
        }

        // grab the fields and search for a field of the required type
        // we only consider the definition if it only has a SINGLE FIELD, and
        // if that field is of the required type.  This avoids nasty presence test
        // failures which aren't.
        QMap<QString, QContactDetailDefinition::Field> allFields = def.fields();
        QList<QString> fNames = allFields.keys();
        if (fNames.size() > 1)
            break;
        foreach (const QString& fName, fNames) {
            QContactDetailDefinition::Field field = allFields.value(fName);
            if (field.dataType == type) {
                // this field of the current definition is of the required type.
                definitionName = defName;
                fieldName = fName;
                found = true;

                // step two: check to see whether the definition/field is natively filterable
                QContactDetailFilter filter;
                filter.setDetailDefinitionName(definitionName, fieldName);
                bool isNativelyFilterable = info->filterSupported(filter);

                if (isNativelyFilterable) {
                    // we've found the optimal definition + field for our test.
                    break;
                }
            }
        }

        if (found && isNativelyFilterable) {
            // we've found the optimal definition + field for our test.
            break;
        }
    }

    if (found) {
        // whether it is natively filterable or not, we found a definition that matches our requirements.
        result.first = definitionName;
        result.second = fieldName;
        *nativelyFilterable = isNativelyFilterable;
        return result;
    }

    // step three (or, if not step one): check to see whether the manager allows mutable definitions
    // no existing definition matched our requirements, but we might be able to add one that does.
    if (info->supportedDataTypes().contains(type) && info->hasFeature(QContactManagerInfo::MutableDefinitions)) {
        // ok, the manager does not have a definition matching our criteria, but we could probably add it.
        int defCount = detailDefinitionsAddedToManagers.values(cm).count();
        QString generatedDefinitionName = QString("x-nokia-mobility-contacts-test-definition-") + QString::number((defCount+1));

        // build a definition that matches the criteria.
        QContactDetailDefinition generatedDefinition;
        generatedDefinition.setName(generatedDefinitionName);
        QContactDetailDefinition::Field generatedField;
        generatedField.dataType = type;
        QMap<QString, QContactDetailDefinition::Field> fields;
        fields.insert("generatedField", generatedField);
        generatedDefinition.setFields(fields);
        generatedDefinition.setAccessConstraint(QContactDetailDefinition::Any);
        generatedDefinition.setUnique(false);

        // attempt to save it to the manager.
        if (cm->saveDetailDefinition(generatedDefinition)) {
            // successfully added definition.
            definitionName = generatedDefinitionName;
            fieldName = "generatedField";
            detailDefinitionsAddedToManagers.insert(cm, definitionName); // cleanup stack.
        }
    } else {
        qWarning() << "Unable to perform tests involving detail values of the" << type << "type: not supported by manager.";
    }

    result.first = definitionName;
    result.second = fieldName;
    *nativelyFilterable = false;
    return result;
}

QList<QUniqueId> tst_QContactManagerFiltering::prepareModel(QContactManager *cm)
{
    /* Discover the definition and field names required for testing */
    QMap<QString, QPair<QString, QString> > definitionDetails; // per value type string
    QPair<QString, QString> defAndFieldNames;
    bool nativelyFilterable;

    /* String */
    defAndFieldNames = definitionAndField(cm, QVariant::String, &nativelyFilterable);
    definitionDetails.insert("String", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* Integer */
    defAndFieldNames = definitionAndField(cm, QVariant::Int, &nativelyFilterable);
    definitionDetails.insert("Integer", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* Date time detail */
    defAndFieldNames = definitionAndField(cm, QVariant::DateTime, &nativelyFilterable);
    definitionDetails.insert("DateTime", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* double detail */
    defAndFieldNames = definitionAndField(cm, QVariant::Double, &nativelyFilterable);
    definitionDetails.insert("Double", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* bool */
    defAndFieldNames = definitionAndField(cm, QVariant::Bool, &nativelyFilterable);
    definitionDetails.insert("Bool", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* long long */
    defAndFieldNames = definitionAndField(cm, QVariant::LongLong, &nativelyFilterable);
    definitionDetails.insert("LongLong", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* unsigned long long */
    defAndFieldNames = definitionAndField(cm, QVariant::ULongLong, &nativelyFilterable);
    definitionDetails.insert("ULongLong", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* date */
    defAndFieldNames = definitionAndField(cm, QVariant::Date, &nativelyFilterable);
    definitionDetails.insert("Date", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* time */
    defAndFieldNames = definitionAndField(cm, QVariant::Time, &nativelyFilterable);
    definitionDetails.insert("Time", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* uint */
    defAndFieldNames = definitionAndField(cm, QVariant::UInt, &nativelyFilterable);
    definitionDetails.insert("UInt", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* char */
    defAndFieldNames = definitionAndField(cm, QVariant::Char, &nativelyFilterable);
    definitionDetails.insert("Char", defAndFieldNames);
    defAndFieldNamesForTypePerManager.insert(cm, definitionDetails);
    defAndFieldNames.first = QString();
    defAndFieldNames.second = QString();

    /* Add some contacts */
    QContact a, b, c, d;
    QContactName name;
    QContactPhoneNumber number;
    QContactDetail string(definitionDetails.value("String").first);
    QContactDetail integer(definitionDetails.value("Integer").first);
    QContactDetail datetime(definitionDetails.value("DateTime").first);
    QContactDetail dubble(definitionDetails.value("Double").first);
    QContactDetail boool(definitionDetails.value("Bool").first);
    QContactDetail llong(definitionDetails.value("LongLong").first);
    QContactDetail ullong(definitionDetails.value("ULongLong").first);
    QContactDetail date(definitionDetails.value("Date").first);
    QContactDetail time(definitionDetails.value("Time").first);
    QContactDetail uintt(definitionDetails.value("UInt").first);
    QContactDetail charr(definitionDetails.value("Char").first);

    name.setFirst("Aaron");
    name.setLast("Aaronson");
    number.setNumber("555-1212");
    string.setValue(definitionDetails.value("String").second, "Aaron Aaronson");
    integer.setValue(definitionDetails.value("Integer").second, 10);
    datetime.setValue(definitionDetails.value("DateTime").second, QDateTime(QDate(2009, 06, 29), QTime(16, 52, 23, 0)));
    boool.setValue(definitionDetails.value("Bool").second, true);
    ullong.setValue(definitionDetails.value("ULongLong").second, (qulonglong)120000000000LL); // 120B
    date.setValue(definitionDetails.value("Date").second, QDate(1988, 1, 26));
    time.setValue(definitionDetails.value("Time").second, QTime(16,52,23,0));

    a.saveDetail(&name);
    a.saveDetail(&number);
    if (!definitionDetails.value("String").first.isEmpty() && !definitionDetails.value("String").second.isEmpty())
        a.saveDetail(&string);
    if (!definitionDetails.value("Integer").first.isEmpty() && !definitionDetails.value("Integer").second.isEmpty())
        a.saveDetail(&integer);
    if (!definitionDetails.value("DateTime").first.isEmpty() && !definitionDetails.value("DateTime").second.isEmpty())
        a.saveDetail(&datetime);
    if (!definitionDetails.value("Bool").first.isEmpty() && !definitionDetails.value("Bool").second.isEmpty())
        a.saveDetail(&boool);
    if (!definitionDetails.value("ULongLong").first.isEmpty() && !definitionDetails.value("ULongLong").second.isEmpty())
        a.saveDetail(&ullong);
    if (!definitionDetails.value("Date").first.isEmpty() && !definitionDetails.value("Date").second.isEmpty())
        a.saveDetail(&date);
    if (!definitionDetails.value("Time").first.isEmpty() && !definitionDetails.value("Time").second.isEmpty())
        a.saveDetail(&time);

    name.setFirst("Bob");
    name.setLast("Aaronsen");
    number.setNumber("555-3456");
    string.setValue(definitionDetails.value("String").second, "Bob Aaronsen");
    integer.setValue(definitionDetails.value("Integer").second, 20);
    dubble.setValue(definitionDetails.value("Double").second, 4.0);
    boool.setValue(definitionDetails.value("Bool").second, false);
    ullong.setValue(definitionDetails.value("ULongLong").second, (qulonglong) 80000000000LL); // 80B
    uintt.setValue(definitionDetails.value("UInt").second, 4000000000u); // 4B
    date.setValue(definitionDetails.value("Date").second, QDate(1492, 5, 5));
    time.setValue(definitionDetails.value("Time").second, QTime(15,52,23,0));
    charr.setValue(definitionDetails.value("Char").second, QVariant(QChar('b')));

    b.saveDetail(&name);
    b.saveDetail(&number);
    if (!definitionDetails.value("String").first.isEmpty() && !definitionDetails.value("String").second.isEmpty())
        b.saveDetail(&string);
    if (!definitionDetails.value("Integer").first.isEmpty() && !definitionDetails.value("Integer").second.isEmpty())
        b.saveDetail(&integer);
    if (!definitionDetails.value("Double").first.isEmpty() && !definitionDetails.value("Double").second.isEmpty())
        b.saveDetail(&dubble);
    if (!definitionDetails.value("Bool").first.isEmpty() && !definitionDetails.value("Bool").second.isEmpty())
        b.saveDetail(&boool);
    if (!definitionDetails.value("ULongLong").first.isEmpty() && !definitionDetails.value("ULongLong").second.isEmpty())
        b.saveDetail(&ullong);
    if (!definitionDetails.value("UInt").first.isEmpty() && !definitionDetails.value("UInt").second.isEmpty())
        b.saveDetail(&uintt);
    if (!definitionDetails.value("Date").first.isEmpty() && !definitionDetails.value("Date").second.isEmpty())
        b.saveDetail(&date);
    if (!definitionDetails.value("Time").first.isEmpty() && !definitionDetails.value("Time").second.isEmpty())
        b.saveDetail(&time);
    if (!definitionDetails.value("Char").first.isEmpty() && !definitionDetails.value("Char").second.isEmpty())
        b.saveDetail(&charr);

    name.setFirst("Boris");
    name.setLast("Aaronsun");
    string.setValue(definitionDetails.value("String").second, "Boris Aaronsun");
    integer.setValue(definitionDetails.value("Integer").second, -20);
    datetime.setValue(definitionDetails.value("DateTime").second, QDateTime(QDate(2009, 06, 29), QTime(16, 54, 17, 0)));
    llong.setValue(definitionDetails.value("LongLong").second, (qlonglong)8000000000LL); // 8B
    charr.setValue(definitionDetails.value("Char").second, QVariant(QChar('c')));

    c.saveDetail(&name);
    if (!definitionDetails.value("String").first.isEmpty() && !definitionDetails.value("String").second.isEmpty())
        c.saveDetail(&string);
    if (!definitionDetails.value("Integer").first.isEmpty() && !definitionDetails.value("Integer").second.isEmpty())
        c.saveDetail(&integer);
    if (!definitionDetails.value("DateTime").first.isEmpty() && !definitionDetails.value("DateTime").second.isEmpty())
        c.saveDetail(&datetime);
    if (!definitionDetails.value("Double").first.isEmpty() && !definitionDetails.value("Double").second.isEmpty())
        c.saveDetail(&dubble);
    if (!definitionDetails.value("Bool").first.isEmpty() && !definitionDetails.value("Bool").second.isEmpty())
        c.saveDetail(&boool);
    if (!definitionDetails.value("LongLong").first.isEmpty() && !definitionDetails.value("LongLong").second.isEmpty())
        c.saveDetail(&llong);
    if (!definitionDetails.value("ULongLong").first.isEmpty() && !definitionDetails.value("ULongLong").second.isEmpty())
        c.saveDetail(&ullong);
    if (!definitionDetails.value("Char").first.isEmpty() && !definitionDetails.value("Char").second.isEmpty())
        c.saveDetail(&charr);

    name.setFirst("Dennis");
    name.setLast("FitzMacintyre");
    string.setValue(definitionDetails.value("String").second, "Dennis FitzMacintyre");
    dubble.setValue(definitionDetails.value("Double").second, -128.0);
    llong.setValue(definitionDetails.value("LongLong").second, (qlonglong)-14000000000LL);
    uintt.setValue(definitionDetails.value("UInt").second, 3000000000u); // 3B
    date.setValue(definitionDetails.value("Date").second, QDate(1770, 10, 1));

    d.saveDetail(&name);
    if (!definitionDetails.value("String").first.isEmpty() && !definitionDetails.value("String").second.isEmpty())
        d.saveDetail(&string);
    if (!definitionDetails.value("Double").first.isEmpty() && !definitionDetails.value("Double").second.isEmpty())
        d.saveDetail(&dubble);
    if (!definitionDetails.value("LongLong").first.isEmpty() && !definitionDetails.value("LongLong").second.isEmpty())
        d.saveDetail(&llong);
    if (!definitionDetails.value("UInt").first.isEmpty() && !definitionDetails.value("UInt").second.isEmpty())
        d.saveDetail(&uintt);
    if (!definitionDetails.value("Date").first.isEmpty() && !definitionDetails.value("Date").second.isEmpty())
        d.saveDetail(&date);

    qDebug() << "Generating contacts with different timestamps, please wait..";
    int originalContactCount = cm->contacts().count();
    Q_ASSERT(cm->saveContact(&a));
    QTest::qSleep(2000);
    Q_ASSERT(cm->saveContact(&b));
    QTest::qSleep(2000);
    Q_ASSERT(cm->saveContact(&c));
    QTest::qSleep(2000);
    Q_ASSERT(cm->saveContact(&d));
    QTest::qSleep(2000);

    /* Now add some contacts specifically for multisorting */
    QContact e,f,g;
    QContactName n;
    n.setFirst("John");
    n.setLast("Smithee");
    string.setValue(definitionDetails.value("String").second, "");
    if (!definitionDetails.value("String").first.isEmpty() && !definitionDetails.value("String").second.isEmpty())
        e.saveDetail(&string);
    e.saveDetail(&n);
    n.setLast("Smithey");
    f.saveDetail(&n);
    n.setLast("Smithy");
    g.saveDetail(&n);
    Q_ASSERT(cm->saveContact(&e));
    Q_ASSERT(cm->saveContact(&f));
    Q_ASSERT(cm->saveContact(&g));
    originalContactCount += 7;
    Q_ASSERT(cm->contacts().count() == originalContactCount);

    /* Ensure the last modified times are different */
    QTest::qSleep(2000);
    c.setDisplayLabel("Clarence");
    cm->saveContact(&c);
    QTest::qSleep(2000);
    b.setDisplayLabel("Boris");
    cm->saveContact(&b);
    QTest::qSleep(2000);
    a.setDisplayLabel("Albert");
    cm->saveContact(&a);
    QTest::qSleep(2000);

    /* Add our newly saved contacts to our internal list of added contacts */
    contactsAddedToManagers.insert(cm, g.id());
    contactsAddedToManagers.insert(cm, f.id());
    contactsAddedToManagers.insert(cm, e.id());
    contactsAddedToManagers.insert(cm, d.id());
    contactsAddedToManagers.insert(cm, c.id());
    contactsAddedToManagers.insert(cm, b.id());
    contactsAddedToManagers.insert(cm, a.id());

    /* Reload the contacts to pick up any changes */
    a = cm->contact(a.id());
    b = cm->contact(b.id());
    c = cm->contact(c.id());
    d = cm->contact(d.id());
    e = cm->contact(e.id());
    f = cm->contact(f.id());
    g = cm->contact(g.id());

    QList<QUniqueId> list;
    if (!a.isEmpty())
        list << a.id();
    if (!b.isEmpty())
        list << b.id();
    if (!c.isEmpty())
        list << c.id();
    if (!d.isEmpty())
        list << d.id();
    if (!e.isEmpty())
        list << e.id();
    if (!f.isEmpty())
        list << f.id();
    if (!g.isEmpty())
        list << g.id();
    return list;
}

/* ============ Utility functions ============= */

void tst_QContactManagerFiltering::dumpContactDifferences(const QContact& ca, const QContact& cb)
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

    QCOMPARE(b, a);
}

bool tst_QContactManagerFiltering::isSuperset(const QContact& ca, const QContact& cb)
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

void tst_QContactManagerFiltering::dumpContact(const QContact& contact)
{
    QContactManager m;
    qDebug() << "Contact: " << contact.id() << "(" << m.synthesiseDisplayLabel(contact) << ")";
    QList<QContactDetail> details = contact.details();
    foreach(QContactDetail d, details) {
        qDebug() << "  " << d.definitionName() << ":";
        qDebug() << "    Vals:" << d.values();
    }
}

void tst_QContactManagerFiltering::dumpContacts()
{
    QContactManager m;
    QList<QUniqueId> ids = m.contacts();

    foreach(QUniqueId id, ids) {
        QContact c = m.contact(id);
        dumpContact(c);
    }
}

/* Static actions for testing matching */
class QIntegerAction : public QContactAction
{
    Q_OBJECT

public:
    QIntegerAction() {}
    ~QIntegerAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Number", "IntegerCo", 5); }
    QVariantMap metadata() const {return QVariantMap();}

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        QPair<QString, QString> defAndFieldName = defAndFieldNamesForTypeForActions.value("Integer");
        df.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
        df.setValue(value);
        return df;
    }
    bool supportsDetail(const QContactDetail& detail) const
    {
        return detail.definitionName() == defAndFieldNamesForTypeForActions.value("Integer").first
                && !detail.variantValue(defAndFieldNamesForTypeForActions.value("Integer").second).isNull();
    }

    void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
    {
        Q_UNUSED(contact);
        Q_UNUSED(detail);
        // Well, do something
    }
};

class QNumberAction : public QContactAction
{
    Q_OBJECT

public:
    QNumberAction() {}
    ~QNumberAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Number", "NumberCo", 42); }
    QVariantMap metadata() const {return QVariantMap();}

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        QPair<QString, QString> defAndFieldName = defAndFieldNamesForTypeForActions.value("Double");
        df.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
        df.setValue(value);

        QContactDetailFilter df2;
        defAndFieldName = defAndFieldNamesForTypeForActions.value("Integer");
        df2.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
        df2.setValue(value);

        /* We like either doubles or integers */
        return df || df2;
    }
    bool supportsDetail(const QContactDetail& detail) const
    {
        if (detail.definitionName() == defAndFieldNamesForTypeForActions.value("Double").first
                && !detail.variantValue(defAndFieldNamesForTypeForActions.value("Double").second).isNull()) {
            return true;
        }

        if (detail.definitionName() == defAndFieldNamesForTypeForActions.value("Integer").first
                && !detail.variantValue(defAndFieldNamesForTypeForActions.value("Integer").second).isNull()) {
            return true;
        }

        return false;
    }

    void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
    {
        Q_UNUSED(contact);
        Q_UNUSED(detail);
        // Well, do something
    }
};

class QBooleanAction : public QContactAction
{
    Q_OBJECT

public:
    QBooleanAction() {}
    ~QBooleanAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Boolean", "BooleanCo", 3); }
    QVariantMap metadata() const {return QVariantMap();}

    QContactFilter contactFilter(const QVariant& value) const
    {
        if (value.isNull() || (value.type() == QVariant::Bool && value.toBool() == true)) {
            /* This only likes bools that are true */
            QContactDetailFilter df;
            QPair<QString, QString> defAndFieldName = defAndFieldNamesForTypeForActions.value("Bool");
            df.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
            df.setValue(true);
            return df;
        } else {
            return QContactInvalidFilter();
        }
    }
    bool supportsDetail(const QContactDetail& detail) const
    {
        return detail.definitionName() == defAndFieldNamesForTypeForActions.value("Bool").first
                && (detail.value<bool>(defAndFieldNamesForTypeForActions.value("Bool").second) == true);
    }

    void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail())
    {
        Q_UNUSED(contact);
        Q_UNUSED(detail);
        // Well, do something
    }
};

class RecursiveAction : public QContactAction
{
    Q_OBJECT

public:
    RecursiveAction() {}
    ~RecursiveAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Recursive", "RecursiveCo", 3); }
    QVariantMap metadata() const {return QVariantMap();}

    QContactFilter contactFilter(const QVariant& value) const
    {
        /* Return a filter that selects us again.. */
        QContactActionFilter af;
        af.setActionName("Recursive");
        af.setVendor("RecursiveCo", 3);
        af.setValue(value);
        return af;
    }
    bool supportsDetail(const QContactDetail&) const
    {
        return false;
    }
    void performAction(const QContact&, const QContactDetail&)
    {

    }
};

class AnotherRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    int implementationVersion() const {return 4;}
    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("Recursive");
        return af;
    }
};

/* A pair that reference each other */
class PairRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("PairRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("AnotherPairRecursive");
        return af;
    }
};

class AnotherPairRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("AnotherPairRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af;
    }
};

class IntersectionRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("IntersectionRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af && QContactFilter() && af;
    }
};

class UnionRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("UnionRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af || QContactInvalidFilter() || af;
    }
};

class FilterActionFactory : public QContactActionFactory
{
    Q_OBJECT
    Q_INTERFACES(QContactActionFactory)

public:
    FilterActionFactory() {}
    ~FilterActionFactory() {}

    QString name() const
    {
        return QString("FilterActionFactory");
    }

    QList<QContactActionDescriptor> actionDescriptors() const
    {
        QList<QContactActionDescriptor> ret;

        ret << QContactActionDescriptor("Number", "NumberCo", 42)
                << QContactActionDescriptor("Number", "IntegerCo", 5)
                << QContactActionDescriptor("Boolean", "BooleanCo", 3)
                << QContactActionDescriptor("Recursive", "RecursiveCo", 3)
                << QContactActionDescriptor("Recursive", "RecursiveCo", 4)
                << QContactActionDescriptor("PairRecursive", "RecursiveCo", 3)
                << QContactActionDescriptor("AnotherPairRecursive", "RecursiveCo", 3)
                << QContactActionDescriptor("IntersectionRecursive", "RecursiveCo", 3)
                << QContactActionDescriptor("UnionRecursive", "RecursiveCo", 3);

        return ret;
    }

    QContactAction* instance(const QContactActionDescriptor& descriptor) const
    {
        if (descriptor.actionName() == "Number") {
            if (descriptor.vendorName() == "IntegerCo")
                return new QIntegerAction;
            else
                return new QNumberAction;
        } else if (descriptor.actionName() == "Boolean") {
            return new QBooleanAction;
        } else if (descriptor.actionName() == "Recursive") {
            if (descriptor.implementationVersion() == 3)
                return new RecursiveAction;
            else
                return new AnotherRecursiveAction;
        } else if (descriptor.actionName() == "PairRecursive") {
            return new PairRecursiveAction;
        } else if (descriptor.actionName() == "AnotherPairRecursive") {
            return new AnotherPairRecursiveAction;
        } else if (descriptor.actionName() == "IntersectionRecursive") {
            return new IntersectionRecursiveAction;
        } else {
            return new UnionRecursiveAction;
        }
    }

    QVariantMap actionMetadata(const QContactActionDescriptor& descriptor) const
    {
        Q_UNUSED(descriptor);
        return QVariantMap();
    }
};

/* Statically import it (and a duplicate copy of it, purely for testing purposes) */
Q_EXPORT_PLUGIN2(contacts_testFilterActionFactory, FilterActionFactory)
Q_IMPORT_PLUGIN(contacts_testFilterActionFactory)

QTEST_MAIN(tst_QContactManagerFiltering)
#include "tst_qcontactmanagerfiltering.moc"
