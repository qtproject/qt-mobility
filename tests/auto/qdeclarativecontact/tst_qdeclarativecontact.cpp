/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtTest/QSignalSpy>
#include <QMetaObject>
#include <QDateTime>
#include <QMap>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QDeclarativeListProperty>


#include "qdeclarativecontactmodel_p.h"
#include "qdeclarativecontactsortorder_p.h"
#include "qdeclarativecontact_p.h" 
#include "qdeclarativecontactfilters_p.h" 
#include "qdeclarativecontactdetails_p.h" 
#include "qdeclarativecontactfetchhint_p.h" 
#include "qdeclarativecontactrelationship_p.h" 
#include "qdeclarativecontactrelationshipmodel_p.h" 

// Eventually these will make it into qtestcase.h
// but we might need to tweak the timeout values here.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
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
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

#ifndef QTRY_WAIT
#define QTRY_WAIT(code, __expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 10000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)
#endif


#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif


QTM_USE_NAMESPACE


class tst_QDeclarativeContact : public QObject
{
    Q_OBJECT

public:
    tst_QDeclarativeContact();
    virtual ~tst_QDeclarativeContact();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

private slots:

    void contact();
    void contactModel();
    void filter();
    void details();
    void relationship();
    void relationshipModel();
    void sortOrder();
    void fetchHint();
    void construction();
    void construction_data();


private:
    QObject* createComponent(const QString& componentString);

private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine engine;
};

tst_QDeclarativeContact::tst_QDeclarativeContact() {}
tst_QDeclarativeContact::~tst_QDeclarativeContact() {}

void tst_QDeclarativeContact::initTestCase()
{
}

void tst_QDeclarativeContact::cleanupTestCase()
{
}

void tst_QDeclarativeContact::init()
{

}
void tst_QDeclarativeContact::construction()
{
    QFETCH(QString, componentString);
    QFETCH(QString, expectedClassName);
    QFETCH(bool, shouldSucceed);

    QObject* obj = createComponent(componentString);

    if (shouldSucceed) {
        if (obj == 0) {
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
            qWarning("--------- could not instantiate components from contact   -------------- ");
            qWarning("--------- declarative plugin. make sure it is built and found ------------");
            qWarning(" ---------under {QTDIR}/imports, or c:/sys/bin on Symbian     ----------- ");
            qWarning("--------- ------------- ------------- ---------------------- ------------ ");
        }
        QVERIFY(obj != 0);
        QCOMPARE(obj->metaObject()->className(), expectedClassName.toAscii().constData());
    } else {
        QVERIFY(obj == 0);
    }
    delete obj;
}

void tst_QDeclarativeContact::construction_data()
{
    QTest::addColumn<QString>("expectedClassName");
    QTest::addColumn<QString>("componentString");
    QTest::addColumn<bool>("shouldSucceed");
    // ContactModel
    QTest::newRow("ContactModel: No properties") <<  "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {}" << true;
    QTest::newRow("ContactModel: Only id property") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId}" << true;
    QTest::newRow("ContactModel: Valuetype properties") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId; manager:'memory'}" << true;
    QTest::newRow("ContactModel: With filter") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId; filter: DetailFilter{} }" << true;
    QTest::newRow("ContactModel: With fetchHint") << "QDeclarativeContactModel" << "import Qt 4.7 \n import QtMobility.contacts 1.1 \n ContactModel {id: contactModelId; fetchHint:FetchHint {id:hint; optimizationHints:FetchHint.AllRequired} }" << true;
}
void tst_QDeclarativeContact::contact()
{
    QObject* obj = createComponent("import Qt 4.7\n"
                                   "import QtMobility.contacts 1.1\n"
                                   "Contact {id:contact; }");
    QVERIFY(obj != 0);
    QVERIFY(obj->property("contactId").toInt() == 0);
    QVERIFY(obj->property("manager").toString() == "");
    QVERIFY(obj->property("modified").toBool() == false);
    delete obj;

    obj = createComponent("import Qt 4.7\n"
                           "import QtMobility.contacts 1.1\n"
                           "Contact {"
                              "id:contact;"
                              "address.street : '53 Brandl St';"
                              "address.locality : 'Brisbane';"
                              "address.region : 'QLD';"
                              "address.postcode : '4111';"
                              "address.country : 'Australia';"
                              "address.postOfficeBox : '1111';"
                              "address.subTypes : [Address.Parcel, Address.Domestic, Address.Postal, Address.International];"
                              "Anniversary {"
                                 "calendarId: 'birthday';"
                                 "originalDate : '2010-08-12';"
                                 "event : 'party';"
                                 "subType : Anniversary.Memorial;"
                              "}\n"
                           "}");

    QVERIFY(obj != 0);
    delete obj;
}
void tst_QDeclarativeContact::contactModel()
{
    //TODO
//    QDeclarativeComponent component(&engine);
//    component.loadUrl(QUrl::fromLocalFile(SRCDIR "./data/contactmodel.qml"));

//    QDeclarativeContactModel *contactModel = static_cast<QDeclarativeContactModel*>(component.create());
//    QVERIFY(contactModel != 0);
//    //other qverfies
//    delete contactModel;
}
void tst_QDeclarativeContact::filter()
{
    QObject* filter = 0;

    //action filter
    {
        filter = createComponent(
                    "import Qt 4.7\n"
                    "import QtMobility.contacts 1.1\n"
                    "ActionFilter{"
                    "actionName : 'email';"
                    "}");
        QVERIFY(filter != 0);
        QVERIFY(filter->property("actionName").toString() == QLatin1String("email"));
        QDeclarativeContactFilter* df = static_cast<QDeclarativeContactFilter*>(filter);
        QVERIFY(df != 0);
        QVERIFY(df->filter().type() == QContactFilter::ActionFilter);
        QContactActionFilter af = df->filter();
        QVERIFY(af.actionName() == QLatin1String("email"));

        QSignalSpy spy(filter, SIGNAL(filterChanged()));

        filter->setProperty("actionName", "call");
        QVERIFY(filter->property("actionName").toString() == QLatin1String("call"));
        QTRY_COMPARE(spy.count(), 1);
        delete filter;
        filter = 0;
    }

     //changelog filter
    {
        filter = createComponent(
                    "import Qt 4.7\n"
                    "import QtMobility.contacts 1.1\n"
                    "ChangeLogFilter{"
                    "since : '2010-11-16 12:12:12';"
                    "eventType : 'EventAdded';"
                    "}");
        QVERIFY(filter != 0);
        QVERIFY(filter->property("since").toDateTime() == QDateTime(QDate(2010, 11, 16), QTime(12, 12, 12)));
        QVERIFY(filter->property("eventType").toInt() == QDeclarativeContactChangeLogFilter::EventAdded);
        QDeclarativeContactChangeLogFilter* df = static_cast<QDeclarativeContactChangeLogFilter*>(filter);
        QVERIFY(df != 0);
        QVERIFY(df->filter().type() == QContactFilter::ChangeLogFilter);
        QContactChangeLogFilter cf = df->filter();
        QVERIFY(cf.since() == QDateTime(QDate(2010, 11, 16), QTime(12, 12, 12)));
        QVERIFY(cf.eventType() == QContactChangeLogFilter::EventAdded);

        QSignalSpy spy(filter, SIGNAL(filterChanged()));

        filter->setProperty("eventType", "EventChanged");
        filter->setProperty("since", QDateTime(QDate(2010, 12, 17), QTime(12, 12, 12)));
        cf = df->filter();
        QVERIFY(cf.since() == QDateTime(QDate(2010, 12, 17), QTime(12, 12, 12)));
        QVERIFY(cf.eventType() == QContactChangeLogFilter::EventChanged);

        QTRY_COMPARE(spy.count(), 2);
        delete filter;
        filter = 0;
    }

    //detail filter
   {
       filter = createComponent(
                   "import Qt 4.7\n"
                   "import QtMobility.contacts 1.1\n"
                   "DetailFilter{"
                   "detail : ContactDetail.Address;"
                   "field : Address.Street;"
                   "value : 'Brandl St';"
                   "matchFlags : Filter.MatchContains | Filter.MatchCaseSensitive;"
                   "}");
       QVERIFY(filter != 0);
       QVERIFY(filter->property("detail").toString() == QLatin1String("Address"));
       QVERIFY(filter->property("field").toString() == QLatin1String("Street"));
       QVERIFY(filter->property("value").toString() == QLatin1String("Brandl St"));
       QDeclarativeContactFilter::MatchFlags flags;

       flags = ~flags & filter->property("matchFlags").toInt();

       QVERIFY(flags.testFlag(QDeclarativeContactFilter::MatchContains) &&
               flags.testFlag(QDeclarativeContactFilter::MatchCaseSensitive));

       QDeclarativeContactDetailFilter* df = static_cast<QDeclarativeContactDetailFilter*>(filter);
       QVERIFY(df != 0);
       QVERIFY(df->filter().type() == QContactFilter::ContactDetailFilter);
       QContactDetailFilter cdf = df->filter();
       QVERIFY(cdf.detailDefinitionName() == QLatin1String("Address"));
       QVERIFY(cdf.detailFieldName() == QLatin1String("Street"));
       QVERIFY(cdf.value().toString() == QLatin1String("Brandl St"));
       QVERIFY(cdf.matchFlags().testFlag(QContactFilter::MatchContains));
       QVERIFY(cdf.matchFlags().testFlag(QContactFilter::MatchCaseSensitive));

       QSignalSpy spy(filter, SIGNAL(filterChanged()));

       filter->setProperty("detail", "PhoneNumber");
       filter->setProperty("field", "number");
       filter->setProperty("value", "123456");
       filter->setProperty("matchFlags", (int)(QDeclarativeContactFilter::MatchFlags(QDeclarativeContactFilter::MatchExactly)));

       cdf = df->filter();
       QVERIFY(cdf.detailDefinitionName() == QLatin1String("PhoneNumber"));
       QVERIFY(cdf.detailFieldName() == QLatin1String("number"));
       QVERIFY(cdf.value().toString() == QLatin1String("123456"));
       QVERIFY(cdf.matchFlags().testFlag(QContactFilter::MatchExactly));
       QVERIFY(!cdf.matchFlags().testFlag(QContactFilter::MatchCaseSensitive));


       QTRY_COMPARE(spy.count(), 4);
       delete filter;
       filter = 0;
    }

    //detail range filter
   {
       filter = createComponent(
                   "import Qt 4.7\n"
                   "import QtMobility.contacts 1.1\n"
                   "DetailRangeFilter{"
                   "detail : ContactDetail.Address;"
                   "field : Address.Postcode;"
                   "min:'4000';"
                   "max:'4222';"
                   "matchFlags : Filter.MatchCaseSensitive | Filter.MatchFixedString;"
                   "rangeFlags : DetailRangeFilter.ExcludeLower | DetailRangeFilter.IncludeUpper;"
                   "}");
       QVERIFY(filter != 0);
       QVERIFY(filter->property("detail").toString() == QLatin1String("Address"));

       QVERIFY(filter->property("field").toString() == QLatin1String("Postcode"));
       QVERIFY(filter->property("min").toString() == QLatin1String("4000"));
       QVERIFY(filter->property("max").toString() == QLatin1String("4222"));
       QDeclarativeContactFilter::MatchFlags flags;
       flags = ~flags & filter->property("matchFlags").toInt();
       QVERIFY(flags.testFlag(QDeclarativeContactFilter::MatchCaseSensitive) &&
               flags.testFlag(QDeclarativeContactFilter::MatchFixedString));

       QDeclarativeContactDetailRangeFilter::RangeFlags rflags;
       rflags = ~rflags & filter->property("rangeFlags").toInt();

       QVERIFY(rflags.testFlag(QDeclarativeContactDetailRangeFilter::ExcludeLower) &&
               rflags.testFlag(QDeclarativeContactDetailRangeFilter::IncludeUpper));

       QDeclarativeContactDetailFilter* df = static_cast<QDeclarativeContactDetailFilter*>(filter);
       QVERIFY(df != 0);
       QVERIFY(df->filter().type() == QContactFilter::ContactDetailRangeFilter);
       QContactDetailRangeFilter rf = df->filter();
       QVERIFY(rf.detailDefinitionName() == QLatin1String("Address"));
       QVERIFY(rf.detailFieldName() == QLatin1String("Postcode"));
       QVERIFY(rf.minValue().toString() == QLatin1String("4000"));
       QVERIFY(rf.maxValue().toString() == QLatin1String("4222"));
       QVERIFY(rf.matchFlags().testFlag(QContactFilter::MatchFixedString));
       QVERIFY(rf.matchFlags().testFlag(QContactFilter::MatchCaseSensitive));
       QVERIFY(rf.rangeFlags().testFlag(QContactDetailRangeFilter::ExcludeLower));
       QVERIFY(rf.rangeFlags().testFlag(QContactDetailRangeFilter::IncludeUpper));


       QSignalSpy spy(filter, SIGNAL(filterChanged()));

       filter->setProperty("detail", "PhoneNumber");
       filter->setProperty("field", "number");
       filter->setProperty("min", "111111");
       filter->setProperty("max", "999999");
       filter->setProperty("matchFlags", (int)(QDeclarativeContactFilter::MatchFlags(QDeclarativeContactFilter::MatchExactly)));
       filter->setProperty("rangeFlags", (int)(QDeclarativeContactDetailRangeFilter::RangeFlags(QDeclarativeContactDetailRangeFilter::ExcludeUpper)));

       rf = df->filter();
       QVERIFY(rf.detailDefinitionName() == QLatin1String("PhoneNumber"));
       QVERIFY(rf.detailFieldName() == QLatin1String("number"));
       QVERIFY(rf.minValue().toString() == QLatin1String("111111"));
       QVERIFY(rf.maxValue().toString() == QLatin1String("999999"));
       QVERIFY(rf.matchFlags().testFlag(QContactFilter::MatchExactly));
       QVERIFY(!rf.matchFlags().testFlag(QContactFilter::MatchCaseSensitive));
       QVERIFY(rf.rangeFlags().testFlag(QContactDetailRangeFilter::ExcludeUpper));
       QVERIFY(!rf.rangeFlags().testFlag(QContactDetailRangeFilter::IncludeUpper));


       QTRY_COMPARE(spy.count(), 6);
       delete filter;
       filter = 0;
    }

    //local id filter
    {
        filter = createComponent(
                    "import Qt 4.7\n"
                    "import QtMobility.contacts 1.1\n"
                    "IdFilter{"
                    "ids : ['1111','2222','3333'];"
                    "}");
        QVERIFY(filter != 0);
        QVERIFY(filter->property("ids").toStringList().contains(QLatin1String("1111")));
        QVERIFY(filter->property("ids").toStringList().contains(QLatin1String("2222")));
        QVERIFY(filter->property("ids").toStringList().contains(QLatin1String("3333")));
        QDeclarativeContactLocalIdFilter* df = static_cast<QDeclarativeContactLocalIdFilter*>(filter);
        QVERIFY(df != 0);
        QVERIFY(df->filter().type() == QContactFilter::LocalIdFilter);
        QContactLocalIdFilter lif = df->filter();
        QVERIFY(lif.ids().size() == 3);
        QVERIFY(lif.ids().contains(1111));
        QVERIFY(lif.ids().contains(2222));
        QVERIFY(lif.ids().contains(3333));

        QSignalSpy spy(filter, SIGNAL(filterChanged()));

        filter->setProperty("ids", QStringList() << QLatin1String("5555"));
        QVERIFY(filter->property("ids").toStringList().contains(QLatin1String("5555")));
        QTRY_COMPARE(spy.count(), 1);
        delete filter;
        filter = 0;
    }

    //intersection filter
    {
        filter = createComponent(
                    "import Qt 4.7\n"
                    "import QtMobility.contacts 1.1\n"
                    "IntersectionFilter{"
                    "  filters: [IdFilter {ids : ['1111','2222','3333']},"
                    "  ActionFilter {actionName : 'email'}]"
                    "}");
        QVERIFY(filter != 0);
        QDeclarativeContactIntersectionFilter* df = static_cast<QDeclarativeContactIntersectionFilter*>(filter);
        QVERIFY(df != 0);
        QVERIFY(df->filter().type() == QContactFilter::IntersectionFilter);
        QContactIntersectionFilter cif = df->filter();

        QVERIFY(cif.filters().size() == 2);
        QVERIFY(cif.filters().front().type() == QContactFilter::LocalIdFilter);
        QVERIFY(cif.filters().back().type() == QContactFilter::ActionFilter);
        QContactLocalIdFilter idf = cif.filters().front();
        QVERIFY(idf.ids().contains(1111));
        QVERIFY(idf.ids().contains(2222));
        QVERIFY(idf.ids().contains(3333));

        QContactActionFilter caf = cif.filters().back();
        QVERIFY(caf.actionName() == QLatin1String("email"));

        delete filter;
        filter = 0;
    }

    //union filter
    {
        filter = createComponent(
                    "import Qt 4.7\n"
                    "import QtMobility.contacts 1.1\n"
                    "UnionFilter{"
                    "  IdFilter {ids : ['1111','2222','3333']}"
                    "  ActionFilter {actionName : 'email'}"
                    "}");
        QVERIFY(filter != 0);
        QDeclarativeContactUnionFilter* df = static_cast<QDeclarativeContactUnionFilter*>(filter);
        QVERIFY(df != 0);
        QVERIFY(df->filter().type() == QContactFilter::UnionFilter);
        QContactUnionFilter cuf = df->filter();
        QVERIFY(cuf.filters().size() == 2);
        QVERIFY(cuf.filters().front().type() == QContactFilter::LocalIdFilter);
        QVERIFY(cuf.filters().back().type() == QContactFilter::ActionFilter);
        QContactLocalIdFilter idf = cuf.filters().front();
        QVERIFY(idf.ids().contains(1111));
        QVERIFY(idf.ids().contains(2222));
        QVERIFY(idf.ids().contains(3333));

        QContactActionFilter caf = cuf.filters().back();
        QVERIFY(caf.actionName() == QLatin1String("email"));

        delete filter;
        filter = 0;
    }

    //relationship filter
    {
        filter = createComponent(
                    "import Qt 4.7\n"
                    "import QtMobility.contacts 1.1\n"
                    "RelationshipFilter{\n"
                    "relationshipType:Relationship.HasMember;\n"
                    "relatedContactId:1111;\n"
                    "relatedContactRole:Relationship.Either;\n"
                    "}");
        QVERIFY(filter != 0);
        QDeclarativeContactRelationshipFilter* df = static_cast<QDeclarativeContactRelationshipFilter*>(filter);
        QVERIFY(df != 0);
        QVERIFY(df->filter().type() == QContactFilter::RelationshipFilter);
        QContactRelationshipFilter rf = df->filter();
        QVERIFY(rf.relationshipType() == QContactRelationship::HasMember);
        QVERIFY(rf.relatedContactId().localId() == 1111);
        QVERIFY(rf.relatedContactRole() == QContactRelationship::Either);

        QSignalSpy spy(filter, SIGNAL(filterChanged()));

        filter->setProperty("relationshipType", QContactRelationship::HasManager);
        filter->setProperty("relatedContactId", 2222 );
        filter->setProperty("relatedContactRole", QLatin1String("First"));

        QTRY_COMPARE(spy.count(), 3);

        rf = df->filter();
        QVERIFY(rf.relationshipType() == QContactRelationship::HasManager);
        QVERIFY(rf.relatedContactId().localId() == 2222);
        QVERIFY(rf.relatedContactRole() == QContactRelationship::First);

        delete filter;
        filter = 0;
    }
}
void tst_QDeclarativeContact::details()
{
    QObject* obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Address{street : '53 Brandl St';"
                "locality : 'Brisbane';"
                "region : 'QLD';"
                "postcode : '4111';"
                "country : 'Australia';"
                "postOfficeBox : '1111';"
                "subTypes : [Address.Parcel, Address.Domestic, Address.Postal, Address.International];}"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("street").toString() == QLatin1String("53 Brandl St"));
    QVERIFY(obj->property("locality").toString() == QLatin1String("Brisbane"));
    QVERIFY(obj->property("region").toString() == QLatin1String("QLD"));
    QVERIFY(obj->property("postcode").toString() == QLatin1String("4111"));
    QVERIFY(obj->property("country").toString() == QLatin1String("Australia"));
    QVERIFY(obj->property("postOfficeBox").toString() == QLatin1String("1111"));
    QVERIFY(obj->property("subTypes").toStringList().contains(QLatin1String("Parcel")));
    QVERIFY(obj->property("subTypes").toStringList().contains(QLatin1String("Domestic")));
    QVERIFY(obj->property("subTypes").toStringList().contains(QLatin1String("Postal")));
    QVERIFY(obj->property("subTypes").toStringList().contains(QLatin1String("International")));
    delete obj;
    obj = 0;
    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Anniversary {"
                   "calendarId: 'birthday';"
                   "originalDate : '2010-08-12';"
                   "event : 'party';"
                   "subType : Anniversary.Memorial;"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("calendarId").toString() == QLatin1String("birthday"));
    QVERIFY(obj->property("originalDate").toDate() == QDate(2010, 8, 12));
    QVERIFY(obj->property("event").toString() == QLatin1String("party"));
    QVERIFY(obj->property("subType").toInt() == QDeclarativeContactAnniversary::Memorial);
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Avatar {"
                   "imageUrl: 'http://www.example.com/avatar.jpg';"
                   "videoUrl : 'http://www.example.com/avatar.avi';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("imageUrl").toUrl() == QUrl("http://www.example.com/avatar.jpg"));
    QVERIFY(obj->property("videoUrl").toUrl() == QUrl("http://www.example.com/avatar.avi"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Birthday {"
                   "birthday: '1980-12-31';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("birthday").toDate() == QDate(1980, 12, 31));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "EmailAddress {"
                   "emailAddress: 'tom@example.com';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("emailAddress").toString() == QLatin1String("tom@example.com"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Family {"
                   "spouse: 'Tom';"
                   "children: ['Ben', 'Sophie'];"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("spouse").toString() == QLatin1String("Tom"));
    QVERIFY(obj->property("children").toStringList().contains(QLatin1String("Ben")));
    QVERIFY(obj->property("children").toStringList().contains(QLatin1String("Sophie")));
    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Favorite {"
                   "favorite: true;"
                   "index: 1;"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("favorite").toBool());
    QVERIFY(obj->property("index").toInt() == 1);

    obj->setProperty("favorite", false);
    obj->setProperty("index", 2);
    QVERIFY(!obj->property("favorite").toBool());
    QVERIFY(obj->property("index").toInt() == 2);
    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Gender {"
                   "gender: Gender.Male;"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("gender").toInt() == QDeclarativeContactGender::Male);

    obj->setProperty("gender", "Female");
    QVERIFY(obj->property("gender").toInt() == QDeclarativeContactGender::Female);
    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Location {"
                   "label: 'Office';"
                   "latitude: 121.111;"
                   "longitude: 121.111;"
                   "accuracy:0.2;"
                   "altitude:23.2;"
                   "altitudeAccuracy:2.3;"
                   "heading:3234.1;"
                   "speed:33.5;"
                   "timestamp:'2010-11-13';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("label").toString() == QLatin1String("Office"));
    obj->setProperty("label", "Home");
    QVERIFY(obj->property("label").toString() == QLatin1String("Home"));

    QVERIFY(qFuzzyCompare(obj->property("latitude").toDouble(), 121.111));
    obj->setProperty("latitude", 222.222);
    QVERIFY(qFuzzyCompare(obj->property("latitude").toDouble(), 222.222));

    QVERIFY(qFuzzyCompare(obj->property("longitude").toDouble(), 121.111));
    obj->setProperty("longitude", 333.222);
    QVERIFY(qFuzzyCompare(obj->property("longitude").toDouble(), 333.222));

    QVERIFY(qFuzzyCompare(obj->property("accuracy").toDouble(), 0.2));
    obj->setProperty("accuracy", 111.11);
    QVERIFY(qFuzzyCompare(obj->property("accuracy").toDouble(), 111.11));

    QVERIFY(qFuzzyCompare(obj->property("altitude").toDouble(), 23.2));
    obj->setProperty("altitude", 12.2);
    QVERIFY(qFuzzyCompare(obj->property("altitude").toDouble(), 12.2));

    QVERIFY(qFuzzyCompare(obj->property("altitudeAccuracy").toDouble(), 2.3));
    obj->setProperty("altitudeAccuracy", 3.2);
    QVERIFY(qFuzzyCompare(obj->property("altitudeAccuracy").toDouble(), 3.2));

    QVERIFY(qFuzzyCompare(obj->property("heading").toDouble(), 3234.1));
    obj->setProperty("heading", 322.2);
    QVERIFY(qFuzzyCompare(obj->property("heading").toDouble(), 322.2));

    QVERIFY(qFuzzyCompare(obj->property("speed").toDouble(), 33.5));
    obj->setProperty("speed", 21.2);
    QVERIFY(qFuzzyCompare(obj->property("speed").toDouble(), 21.2));

    QVERIFY(obj->property("timestamp").toDate() == QDate(2010,11,13));
    obj->setProperty("timestamp", QDateTime(QDate(2010, 12, 24), QTime(8, 23, 21)));
    QVERIFY(obj->property("timestamp").toDateTime().time() == QTime(8, 23, 21));
    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Guid {"
                   "guid: '1121233322';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("guid").toString() == QLatin1String("1121233322") );

    obj->setProperty("guid", "xyzcds");
    QVERIFY(obj->property("guid").toString() == QLatin1String("xyzcds"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Hobby {"
                   "hobby: 'swimming';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("hobby").toString() == QLatin1String("swimming") );

    obj->setProperty("hobby", "sleeping");
    QVERIFY(obj->property("hobby").toString() == QLatin1String("sleeping"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Nickname {"
                   "nickname: 'Tommy';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("nickname").toString() == QLatin1String("Tommy") );

    obj->setProperty("nickname", "Charlie");
    QVERIFY(obj->property("nickname").toString() == QLatin1String("Charlie"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Note {"
                   "note: 'some notes';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("note").toString() == QLatin1String("some notes") );

    obj->setProperty("note", "more notes");
    QVERIFY(obj->property("note").toString() == QLatin1String("more notes"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "SyncTarget {"
                   "syncTarget: 'skype';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("syncTarget").toString() == QLatin1String("skype") );

    obj->setProperty("syncTarget", "msn");
    QVERIFY(obj->property("syncTarget").toString() == QLatin1String("msn"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Tag {"
                   "tag: 'tag1';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("tag").toString() == QLatin1String("tag1") );

    obj->setProperty("tag", "tag2");
    QVERIFY(obj->property("tag").toString() == QLatin1String("tag2"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Url {"
                   "url: 'http://www.example.com';"
                   "subType:'HomePage';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("url").toString() == QLatin1String("http://www.example.com") );
    QVERIFY(obj->property("subType").toString() == QLatin1String("HomePage"));

    obj->setProperty("url", "http://www.nokia.com");
    obj->setProperty("subType", "Favourite");
    QVERIFY(obj->property("url").toString() == QLatin1String("http://www.nokia.com"));
    QVERIFY(obj->property("subType").toString() == QLatin1String("Favourite"));
    delete obj;
    obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Timestamp {"
                   "lastModified: '2010-11-10 12:12:12';"
                   "created:'2010-10-10 13:54:34';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("lastModified").toDateTime() == QDateTime(QDate(2010, 11, 10), QTime(12, 12, 12)) );
    QVERIFY(obj->property("created").toDateTime() == QDateTime(QDate(2010, 10, 10), QTime(13, 54, 34)) );

    obj->setProperty("lastModified", QDateTime(QDate(2011, 12, 12), QTime(13, 13, 13)));
    obj->setProperty("created", QDateTime(QDate(2009, 11, 11), QTime(10, 10, 10)));
    QVERIFY(obj->property("lastModified").toDateTime() == QDateTime(QDate(2011, 12, 12), QTime(13, 13, 13)) );
    QVERIFY(obj->property("created").toDateTime() == QDateTime(QDate(2009, 11, 11), QTime(10, 10, 10)) );
    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Ringtone {"
                   "audioRingtoneUrl: 'http://www.example.com/audio.wav';"
                   "videoRingtoneUrl : 'http://www.example.com/video.avi';"
                   "vibrationRingtoneUrl : 'http://www.example.com/vibration.wav';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("audioRingtoneUrl").toUrl() == QUrl("http://www.example.com/audio.wav"));
    QVERIFY(obj->property("videoRingtoneUrl").toUrl() == QUrl("http://www.example.com/video.avi"));
    QVERIFY(obj->property("vibrationRingtoneUrl").toUrl() == QUrl("http://www.example.com/vibration.wav"));
    obj->setProperty("audioRingtoneUrl", QUrl());
    obj->setProperty("videoRingtoneUrl", QUrl());
    obj->setProperty("vibrationRingtoneUrl", QUrl());
    QVERIFY(obj->property("audioRingtoneUrl").toUrl().isEmpty());
    QVERIFY(obj->property("videoRingtoneUrl").toUrl().isEmpty());
    QVERIFY(obj->property("vibrationRingtoneUrl").toUrl().isEmpty());

    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Presence {"
                   "timestamp: '2010-11-15 11:59:12';"
                   "nickname : 'Charles';"
                   "state :'Busy';"
                   "stateText :'working' ;"
                   "imageUrl: 'http://www.example.com/me.png';"
                   "customMessage:'qt everywhere';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("timestamp").toDateTime() == QDateTime(QDate(2010, 11, 15), QTime(11, 59, 12)));
    QVERIFY(obj->property("nickname").toString() == QLatin1String("Charles"));
    QVERIFY(obj->property("state").toInt() == QDeclarativeContactPresence::Busy);
    QVERIFY(obj->property("stateText").toString() == QLatin1String("working"));
    QVERIFY(obj->property("imageUrl").toString() == QLatin1String("http://www.example.com/me.png"));
    QVERIFY(obj->property("customMessage").toString() == QLatin1String("qt everywhere"));

    obj->setProperty("timestamp",  QDateTime(QDate(2010, 11, 16), QTime(12, 04, 12)));
    obj->setProperty("nickname",  "Charlie");
    obj->setProperty("state",  "Away");
    obj->setProperty("stateText",  "away for lunch");
    obj->setProperty("imageUrl",  "http://www.example.com/me2.png");
    obj->setProperty("customMessage",  "nice food today");

    QVERIFY(obj->property("timestamp").toDateTime() == QDateTime(QDate(2010, 11, 16), QTime(12, 04, 12)));
    QVERIFY(obj->property("nickname").toString() == QLatin1String("Charlie"));
    QVERIFY(obj->property("state").toInt() == QDeclarativeContactPresence::Away);
    QVERIFY(obj->property("stateText").toString() == QLatin1String("away for lunch"));
    QVERIFY(obj->property("imageUrl").toString() == QLatin1String("http://www.example.com/me2.png"));
    QVERIFY(obj->property("customMessage").toString() == QLatin1String("nice food today"));


    delete obj;
    obj = 0;



    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "PhoneNumber {"
                   "number: '12345678';"
                   "subTypes : ['Mobile', 'Voice'];"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("number").toString() == QLatin1String("12345678"));

    QVERIFY(obj->property("subTypes").toStringList().contains("Mobile"));
    obj->setProperty("number", "87654321");
    obj->setProperty("subTypes", QStringList() << "Fax" << "Voice");
    QVERIFY(obj->property("number").toString() == QLatin1String("87654321"));
    QVERIFY(obj->property("subTypes").toStringList().contains("Voice"));

    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Name {"
                   "prefix:'Mr.';"
                   "firstName :'First';"
                   "middleName : 'Middle';"
                   "lastName : 'Last';"
                   "suffix : 'Suffix';"
                   "customLabel: 'Mr. Example'"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("prefix").toString() == QLatin1String("Mr."));
    QVERIFY(obj->property("firstName").toString() == QLatin1String("First"));
    QVERIFY(obj->property("middleName").toString() == QLatin1String("Middle"));
    QVERIFY(obj->property("lastName").toString() == QLatin1String("Last"));
    QVERIFY(obj->property("suffix").toString() == QLatin1String("Suffix"));
    QVERIFY(obj->property("customLabel").toString() == QLatin1String("Mr. Example"));

    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "OnlineAccount {"
                   "accountUri:'gtalk';"
                   "serviceProvider :'Google';"
                   "capabilities : ['text', 'audio', 'video'];"
                   "subTypes : [OnlineAccount.SipVoip, OnlineAccount.Impp];"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("accountUri").toString() == QLatin1String("gtalk"));
    QVERIFY(obj->property("serviceProvider").toString() == QLatin1String("Google"));
    QVERIFY(obj->property("capabilities").toStringList().contains("text"));
    QVERIFY(obj->property("subTypes").toStringList().contains("SipVoip"));

    delete obj;
    obj = 0;


    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "Organization {"
                   "name:'Nokia';"
                   "logoUrl :'http://www.nokia.com/logo.gif';"
                   "department : ['mobility', 'web'];"
                   "location : 'Brisbane';"
                   "role :'R&D';"
                   "title :'programmer';"
                   "assistantName : 'assistant';"
                "}\n"
                );
    QVERIFY(obj != 0);
    QVERIFY(obj->property("name").toString() == QLatin1String("Nokia"));
    QVERIFY(obj->property("logoUrl").toUrl() == QUrl("http://www.nokia.com/logo.gif"));
    QVERIFY(obj->property("department").toStringList().contains("mobility"));
    QVERIFY(obj->property("role").toString() == QLatin1String("R&D"));
    QVERIFY(obj->property("title").toString() == QLatin1String("programmer"));
    QVERIFY(obj->property("assistantName").toString() == QLatin1String("assistant"));

    delete obj;
    obj = 0;
}
void tst_QDeclarativeContact::relationship()
{
    //TODO
}
void tst_QDeclarativeContact::relationshipModel()
{
    //TODO
}
void tst_QDeclarativeContact::sortOrder()
{
    QObject* obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "SortOrder{"
                "  detail :ContactDetail.Address;"
                "  field : Address.Street;"
                "  direction : Qt.AscendingOrder;"
                "  blankPolicy : SortOrder.BlanksFirst;"
                "  caseSensitivity:Qt.CaseInsensitive;"
                "}");
    QVERIFY(obj != 0);
    qDebug() << obj->property("detail");
    QVERIFY(obj->property("detail").toString() == QLatin1String("Address"));
    QVERIFY(obj->property("field").toString() == QLatin1String("Street"));
    QVERIFY(obj->property("direction").toInt() == Qt::AscendingOrder);
    QVERIFY(obj->property("blankPolicy").toInt() == QContactSortOrder::BlanksFirst);
    QVERIFY(obj->property("caseSensitivity").toInt() == Qt::CaseInsensitive);

    QSignalSpy spy(obj, SIGNAL(sortOrderChanged()));
    obj->setProperty("detail", QLatin1String("PhoneNumber"));
    obj->setProperty("field", QLatin1String("number"));
    obj->setProperty("direction", Qt::DescendingOrder);
    obj->setProperty("blankPolicy", QContactSortOrder::BlanksLast);
    obj->setProperty("caseSensitivity", Qt::CaseSensitive);

    QTRY_COMPARE(spy.count(), 5);

    QDeclarativeContactSortOrder* cso = static_cast<QDeclarativeContactSortOrder*>(obj);
    QVERIFY(cso != 0);

    delete obj;
}
void tst_QDeclarativeContact::fetchHint()
{
    QObject* obj = 0;

    obj = createComponent(
                "import Qt 4.7\n"
                "import QtMobility.contacts 1.1\n"
                "FetchHint{"
                "  detailDefinitionsHint :['PhoneNumber', 'Address'];"
                "  relationshipTypesHint : ['HasMember', 'HasManager'];"
                "  optimizationHints : FetchHint.NoRelationships | FetchHint.NoBinaryBlobs;"
                "  imageWidth : 90;"
                "  imageHeight : 80;"
                "}");
    QVERIFY(obj != 0);
    QVERIFY(obj->property("detailDefinitionsHint").toStringList().contains("PhoneNumber"));
    QVERIFY(obj->property("detailDefinitionsHint").toStringList().contains("Address"));
    QVERIFY(obj->property("detailDefinitionsHint").toStringList().size() == 2);
    QVERIFY(obj->property("relationshipTypesHint").toStringList().size() == 2);
    QVERIFY(obj->property("relationshipTypesHint").toStringList().contains("HasMember"));
    QVERIFY(obj->property("relationshipTypesHint").toStringList().contains("HasManager"));
    QVERIFY(obj->property("optimizationHints").toInt() == (int)(QContactFetchHint::NoRelationships | QContactFetchHint::NoBinaryBlobs));
    QVERIFY(obj->property("imageWidth").toInt() == 90);
    QVERIFY(obj->property("imageHeight").toInt() == 80);


    QSignalSpy spy(obj, SIGNAL(fetchHintChanged()));
    obj->setProperty("detailDefinitionsHint", QStringList() << "EmailAddress" << "Avatar");
    obj->setProperty("relationshipTypesHint", QStringList() << "IsSameAs");
    obj->setProperty("optimizationHints", (int)(QContactFetchHint::AllRequired));
    obj->setProperty("imageWidth", 100);
    obj->setProperty("imageHeight", 200);


    QTRY_COMPARE(spy.count(), 5);
    QVERIFY(obj->property("detailDefinitionsHint").toStringList().contains("EmailAddress"));
    QVERIFY(obj->property("detailDefinitionsHint").toStringList().contains("Avatar"));
    QVERIFY(obj->property("detailDefinitionsHint").toStringList().size() == 2);
    QVERIFY(obj->property("relationshipTypesHint").toStringList().size() == 1);
    QVERIFY(obj->property("relationshipTypesHint").toStringList().contains("IsSameAs"));
    QVERIFY(obj->property("optimizationHints").toInt() == (int)(QContactFetchHint::AllRequired));
    QVERIFY(obj->property("imageWidth").toInt() == 100);
    QVERIFY(obj->property("imageHeight").toInt() == 200);

    delete obj;
}

/*
    Helper function to create components from given string.
*/
QObject* tst_QDeclarativeContact::createComponent(const QString& componentString)
{
    QDeclarativeComponent component(&engine);
    component.setData(componentString.toLatin1(), QUrl::fromLocalFile(""));
    QObject* source_obj = component.create();
    if (!source_obj)
        qDebug() << component.errorString();
    return source_obj;
}

QTEST_MAIN(tst_QDeclarativeContact)
#include "tst_qdeclarativecontact.moc"
