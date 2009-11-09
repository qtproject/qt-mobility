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

#include "ut_cntfiltering.h"
#include "cntsymbianfiltersqlhelper.h"

#include <qtcontacts.h>
#include <QtTest/QtTest>

void TestFiltering::initTestCase()
{
    mCntMng = new QContactManager("symbian");
    mSqlFilter = new CntSymbianFilterSqlHelper;
    mFilters = new QHash<QContactFilter::FilterType, TFilter>;

    // Remove all contacts from the database
    QList<QContactLocalId> cnt_ids = mCntMng->contacts();
    mCntMng->removeContacts(&cnt_ids);

    parseFilters();
    createContacts();
}

void TestFiltering::cleanupTestCase()
{
    delete mCntMng;
    delete mSqlFilter;
    delete mFilters;

}

void TestFiltering::parseFilters()
{
    QFile file("c:/filtering/test_data.txt");
    QByteArray texts;
    QVector<QString> param(7);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        texts = file.readAll();
    } else
        QSKIP("Could not open test_data.txt", SkipAll);

    char lf = '\n';
    char cr = '\r';
    char tab = '\t';
    char space = ' ';
    char open_brace = '[';
    char close_brace = ']';
    int brace_state = 0;
    int no_of_commas = 0;
    char slash = '/';
    bool wait_for_return = false;
    char comma = ',';
    char colon = ':';
    bool found_colon = false;
    int count = texts.size();

    // Parse the text file
    for(int i=0; i<count; i++) {
        char c = texts[i];
        // Comments
        if (c == slash) {
            if (i+1 < count && texts[i+1] == slash) {
            wait_for_return = true;
                continue;
            }
        }

        if (c == lf) {
            wait_for_return = false;
            continue;
        }

        if (wait_for_return)
            continue;

        // Enter braces
        if (c == open_brace) {
            brace_state = 1;
            no_of_commas = 0;
            found_colon = false;
            continue;
        }
        // Leave braces
        if (c == close_brace) {
            brace_state = 2;
        }

        if (c == colon) {
            found_colon = true;
            continue;
        }

        if (c == comma) {
            no_of_commas++;
            continue;
        }

        // Append filter data
        if (brace_state == 1 && c != tab && c != space && c != lf && c != cr) {
            if (!found_colon) {
                // Filter type
                param[0].append(c);
            } else {
                if (no_of_commas == 0)
                    // Definition name
                    param[1].append(c);
                if (no_of_commas == 1)
                    // Field name
                    param[2].append(c);
                if (no_of_commas == 2)
                    // Value
                    param[3].append(c);
                if (no_of_commas == 3)
                    // Match flags
                    param[4].append(c);
                if (no_of_commas == 4)
                    // Results
                    param[5].append(c);
                if (no_of_commas == 5)
                    // Error expected
                    param[6].append(c);
            }
        }
        // Create the filters
        if (brace_state == 2) {
            addFilter(param);
            brace_state = 0;
            qDebug() << "[" << param[0] << param[1] << param[2] << param[3] << param[4] << param[5] << param[6] << "]";
            param.clear();
            param = QVector<QString>(7);
        }
    }
}
void TestFiltering::addFilter(QVector<QString> param)
{
    if (param[0].contains("ContactDetailFilter", Qt::CaseInsensitive)) {
        QContactDetailFilter df;

        df.setDetailDefinitionName(param[1], param[2]);
        QContactFilter::MatchFlags fl = param[4].toInt();
        df.setMatchFlags(fl);
        df.setValue(param[3]);

        TFilter f;
        f.filter = df;
        if (!param[5].isEmpty())
            f.result = param[5].toInt();
        else
            f.result = 0;
        f.name = param[0];
        if (!param[6].isEmpty())
            f.error = param[6].toInt();
        else
            f.error = 0;
        mFilters->insertMulti(QContactFilter::ContactDetailFilter, f);
    }
}

void TestFiltering::createContacts()
{

    QContact alice;

    // Contact details
    QContactName aliceName;
    aliceName.setFirst("Alice");
    aliceName.setLast("Jones");
    alice.saveDetail(&aliceName);

    QString label("Ally Jones");
    QContactDisplayLabel aliceDisplay;
    aliceDisplay.setLabel(label);
    alice.saveDetail(&aliceDisplay);

    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("12345678");
    alice.saveDetail(&number);
    alice.setPreferredDetail("DialAction", number);

    QContactPhoneNumber number2;
    number2.setContexts("Work");
    number2.setSubTypes("Landline");
    number2.setNumber("555-4444");
    alice.saveDetail(&number2);

    QContactAddress add;
    add.setStreet("Leeds West Yorkshire");
    add.setPostcode("10087");
    add.setRegion("New York");
    add.setCountry("United States");
    add.setSubTypes(QContactAddress::SubTypeDomestic);
    alice.saveDetail(&add);

    QContactGender gender;
    gender.setGender("Female");
    alice.saveDetail(&gender);

    QContactBirthday bday;
    bday.setDate(QDate(25,10,1978));
    alice.saveDetail(&bday);

    QContactOnlineAccount acc;
    acc.setAccountUri("sips:alice.jones@nokia.com");
    acc.setSubTypes(QContactOnlineAccount::SubTypeSip);
    alice.saveDetail(&acc);

    QContactEmailAddress email;
    email.setEmailAddress("mailto:alice.jones@nokia.com");
    alice.saveDetail(&email);

    QContactOrganization org;
    org.setDepartment(QStringList("Services"));
    org.setTitle("Assistant Manager");
    org.setLocation("Nokia Cyber Park");
    alice.saveDetail(&email);

    mCntMng->saveContact(&alice);

    // Empty contact
    QContact empty;
    mCntMng->saveContact(&empty);

}

Qt::MatchFlags TestFiltering::flag(int f)
{
    switch(f) {
        case 0:
            return Qt::MatchExactly;
        case 1:
            return Qt::MatchContains;
        case 2:
            return Qt::MatchStartsWith;
        case 3:
            return Qt::MatchEndsWith;
        default:
            return Qt::MatchRecursive;
    }
}

void TestFiltering::testInvalidFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testContactDetailFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;

    QList<TFilter> fs = mFilters->values(QContactFilter::ContactDetailFilter);

    for(int i=0; i<fs.count(); i++) {
        qDebug() << fs[i].name;
        const QContactDetailFilter cdf(fs[i].filter);
        cnt_ids = mSqlFilter->searchContacts(cdf, error);
        QVERIFY(error == fs[i].error);
        QVERIFY(cnt_ids.count() == fs[i].result);

        // Verify contacts
        QString value = cdf.value().toString();
        QString field = cdf.detailFieldName();
        QString defname = cdf.detailDefinitionName();
        for(int j=0; j<cnt_ids.count(); i++) {
            QContact c = mCntMng->contact(cnt_ids[j]);
            QContactDetail d = c.detail(defname);
            QVERIFY(d.definitionName() == defname);
            // Not testing equal to due to MatchFlags definitions
            // in the filter.
            QVERIFY(d.value(field).contains(value));
        }
    }
}

void TestFiltering::testContactDetailRangeFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactDetailRangeFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testChangeLogFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactChangeLogFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testActionFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactActionFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testRelationshipFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactRelationshipFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testIntersectionFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactIntersectionFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testUnionFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactUnionFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testLocalIdFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactLocalIdFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

void TestFiltering::testDefaultFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactFilter filter;
    QContactManager::Error error;

    cnt_ids = mSqlFilter->searchContacts(filter, error);

    QVERIFY(error == QContactManager::NotSupportedError);
}

QTEST_MAIN(TestFiltering);
