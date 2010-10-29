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

#include "ut_cntfiltering.h"
#include "cntdbinfo.h"
#include "cntsymbiansrvconnection.h"
#include <qtcontacts.h>
#include <QContactDetailFilter.h>
#include <QtTest/QtTest>


#include "cntfilterdetail.h"
#include "cntfilterdefault.h"
#include "cntfilterunion.h"
#include "cntfilterintersection.h"
#include "cntfilterrelationship.h"

//#define WRITE_LOGS

#if defined(WRITE_LOGS)
#include <e32debug.h> // RDebug

#define LOG(a)              WriteTestLog(a, "")
#define LOG2(a, b)          WriteTestLog(a, b)
#define TEST_BEGIN_LOG(a)   LOG(a##" begins")
#define TEST_PASSED_LOG(a)  LOG(a##" passed")
void WriteTestLog(const QString a, const QString b);
void WriteTestPart(const TDesC& s);

#else
#define LOG(a)
#define LOG2(a, b)
#define TEST_BEGIN_LOG(a)
#define TEST_PASSED_LOG(a)
#endif


void TestFiltering::initTestCase()
{
    QContactManager::Error err;
    
    //create manager
    m_engine = new CntSymbianEngine(QMap<QString, QString>(), &err);
        
    //open symbian database
    m_database = 0;
        TRAPD(error, m_database = CContactDatabase::OpenL());
        QVERIFY(error == KErrNone);
    mFilters = new QHash<QContactFilter::FilterType, TFilter>;

    // Remove all contacts from the database
    QList<QContactLocalId> contacts = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    QMap<int, QContactManager::Error> errorMap;
    m_engine->removeContacts(contacts, &errorMap, &err);
        
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(), QList<QContactSortOrder>(), &err);
    QVERIFY(0 == cnt_ids.count());
    parseFilters();
    createContacts();
}

void TestFiltering::cleanupTestCase()
{
    delete m_engine;
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
        QContactFilter::MatchFlags fl = getMatchFlag(param[4]);
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
    createContact_1();
    createContact_2();
    createContact_3();
    createContact_4();
    createContact_5();
    createContact_6();
    createContact_7();
    createContact_8();
    createContact_9();
    
    QContactManager::Error err;
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    int j = cnt_ids.count();
    QVERIFY(9 == j);
}


void TestFiltering::createContact_1()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    QContact phonecontact;
    // Stefann Fedrernn +02644424423 ste.Fed@nokia.com
    // Contact details
    QContactName contactName;
    contactName.setFirstName("Stefann");
    contactName.setLastName("Fedrernn");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("+02644424423");
    phonecontact.saveDetail(&number);
    
    QContactEmailAddress email;
    email.setEmailAddress("ste.Fed@nokia.com");
    phonecontact.saveDetail(&email);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
   

}
void TestFiltering::createContact_2()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    // Johnn Lenonn +026434234423 Len.Jo@nokia.com
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("Johnn");
    contactName.setLastName("Lenonn");
    phonecontact.saveDetail(&contactName);
    
       

    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("+026434234423");
    phonecontact.saveDetail(&number);

    QContactEmailAddress email;
    email.setEmailAddress("Len.Jo@nokia.com");
    phonecontact.saveDetail(&email);
        
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
    
    

}
void TestFiltering::createContact_3()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    //Created contact 
    // Joronn Bondnn  29143225214423 Joronn.Bondnn@nokia.com
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("Joronn");
    contactName.setLastName("Bondnn");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number2;
    number2.setContexts("Work");
    number2.setSubTypes("Landline");
    number2.setNumber("+29143225214423");
    phonecontact.saveDetail(&number2);

    QContactEmailAddress email;
    email.setEmailAddress("Joronn.Bondnn@nokia.com");
    phonecontact.saveDetail(&email);
    
    QContactOnlineAccount accounturi;
    accounturi.setAccountUri("sip.com");
    accounturi.setSubTypes( QContactOnlineAccount::SubTypeSip );
    phonecontact.saveDetail(&accounturi);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
    


}
void TestFiltering::createContact_4()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    //Created contact 
    // Micheal Jack  +358035022423 micheal.jack@nokia.com
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("Micheal");
    contactName.setLastName("Jack");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number2;
    number2.setContexts("Work");
    number2.setSubTypes("Landline");
    number2.setNumber("+358035022423");
    phonecontact.saveDetail(&number2);

    QContactEmailAddress email;
    email.setEmailAddress("micheal.jack@nokia.com");
    phonecontact.saveDetail(&email);
    
    QContactOnlineAccount accounturi;
    accounturi.setAccountUri("voipsip.com");
    accounturi.setSubTypes( QContactOnlineAccount::SubTypeSipVoip );
    phonecontact.saveDetail(&accounturi);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
    
}
void TestFiltering::createContact_5()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    // Susan Daniel  +044035022423 dan.susa@nokia.com
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("Susan");
    contactName.setLastName("Daniel");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number2;
    number2.setContexts("Home");
    number2.setSubTypes("Mobile");
    number2.setNumber("+044035022423");
    phonecontact.saveDetail(&number2);

    QContactEmailAddress email;
    email.setEmailAddress("dan.susa@nokia.com");
    phonecontact.saveDetail(&email);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
}

void TestFiltering::createContact_6()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    // Susan Daniel  +044035022423 dan.susa@nokia.com
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("6079949400404");
    contactName.setLastName("Douglas");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number2;
    number2.setContexts("Home");
    number2.setSubTypes("Mobile");
    number2.setNumber("+12126093344");
    phonecontact.saveDetail(&number2);

    QContactEmailAddress email;
    email.setEmailAddress("michael.douglas@mga.com");
    phonecontact.saveDetail(&email);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
}

void TestFiltering::createContact_7()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    // Susan Daniel  +12126093344 kirk.douglas@mga.com
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("Frady");
    contactName.setLastName("Mercury");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number2;
    number2.setContexts("Home");
    number2.setSubTypes("Mobile");
    number2.setNumber("+12126093344");
    phonecontact.saveDetail(&number2);

    QContactEmailAddress email;
    email.setEmailAddress("kirk.douglas@mga.com");
    phonecontact.saveDetail(&email);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
}

void TestFiltering::createContact_8()
{
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    // Susan Daniel  +35891 aulis.kekkonen@ak.org
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("Aulis");
    contactName.setLastName("Kekkonen");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number2;
    number2.setContexts("Home");
    number2.setSubTypes("Mobile");
    number2.setNumber("+35891");
    phonecontact.saveDetail(&number2);

    QContactEmailAddress email;
    email.setEmailAddress("aulis.kekkonen@ak.org");
    phonecontact.saveDetail(&email);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
}

void TestFiltering::createContact_9()
{
    TEST_BEGIN_LOG("createContact_9");
    //Currenlty we can only fetch firstname,lastname,companyname and sip/email/phone from the databse
    // so create contact with only these details
    // +plus 000000007  +044035022423 jack.kekkonen@ukk.com
    QContact phonecontact;
    
    // Contact details
    QContactName contactName;
    contactName.setFirstName("+plus");
    contactName.setLastName("000000007");
    QVERIFY(phonecontact.saveDetail(&contactName));
    
    QContactPhoneNumber number2;
    number2.setContexts("Home");
    number2.setSubTypes("Mobile");
    number2.setNumber("+12126093344");
    phonecontact.saveDetail(&number2);

    QContactEmailAddress email;
    email.setEmailAddress("jack.kekkonen@ukk.com");
    phonecontact.saveDetail(&email);
    
    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
    TEST_PASSED_LOG("createContact_9");
}
QContactFilter::MatchFlags TestFiltering::getMatchFlag(QString& inputflag)
    {

    if(inputflag.compare("MatchExactly") == 0)
        return QContactFilter::MatchExactly ;
    else if (inputflag.compare("MatchStartsWith") == 0)
        return QContactFilter::MatchStartsWith ;
    else if (inputflag.compare("MatchEndsWith") == 0)
	        return QContactFilter::MatchEndsWith ;
    else if (inputflag.compare("MatchContains") == 0)
		return QContactFilter::MatchContains;
    else if (inputflag.compare("MatchFixedString") == 0)
		return QContactFilter::MatchFixedString;
    else if (inputflag.compare("MatchCaseSensitive") == 0)
            return QContactFilter::MatchCaseSensitive;
    else if (inputflag.compare("MatchKeypadCollation") == 0)
               return QContactFilter::MatchKeypadCollation;
    else 
        return QContactFilter::MatchPhoneNumber ;
    
    }

void TestFiltering::testContactDetailFilter()
{
    testContactDetailFilter_1();
    testContactDetailFilter_2();
    testContactDetailFilter_3();
    testContactDetailFilter_4();
    testContactDetailFilter_5();
	testContactDetailFilter_6();
	testContactDetailFilter_7();
	testContactDetailFilter_8();
	testContactDetailFilter_9();
}

void TestFiltering::testContactDetailFilter_1()
{
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);

    QList<TFilter> fs = mFilters->values(QContactFilter::ContactDetailFilter);
    int cnt = fs.count();
    for(int i=0; i< cnt; i++) {
        qDebug() << fs[i].name;
        const QContactDetailFilter cdf(fs[i].filter);
        cnt_ids = m_engine->contactIds(cdf, sortOrder, &error);
        QVERIFY(fs[i].error == error);
        
        // check counts 
        int seachedcontactcount = cnt_ids.count();
        int expectedCount =fs[i].result;  
        QVERIFY(expectedCount == seachedcontactcount);
        // Verify contacts
        QString value = cdf.value().toString();
        QString field = cdf.detailFieldName();
        QString defname = cdf.detailDefinitionName();
        for(int j=0; j<seachedcontactcount; j++) {
            QContact sc = m_engine->contact(cnt_ids[j], QContactFetchHint(), &error);
            QContactDetail scDetail = sc.detail(defname);
            QString scDetailValue = scDetail.value(field);
            QVERIFY(scDetail.definitionName() == defname);
            // Not testing equal to due to MatchFlags definitions
            // in the filter.
            QVERIFY(scDetailValue.contains(value));
        }
    }
}

void TestFiltering::testContactDetailFilter_2()
{
    // Test Contact Sip field support
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);
    
    QContactDetailFilter cdf1;
    cdf1.setDetailDefinitionName(QContactOnlineAccount::DefinitionName, QContactOnlineAccount::FieldAccountUri);
    cdf1.setValue("sip");
    cdf1.setMatchFlags(QContactFilter::MatchStartsWith);
    cnt_ids = m_engine->contactIds(cdf1, sortOrder, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    QContactDetailFilter cdf2;
    cdf2.setDetailDefinitionName(QContactOnlineAccount::DefinitionName, QContactOnlineAccount::FieldAccountUri);
    cdf2.setValue("voip");
    cdf2.setMatchFlags(QContactFilter::MatchStartsWith);
    cnt_ids = m_engine->contactIds(cdf2, sortOrder, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    // Test Not supported fields (by our SQL-based filtering, so it rolls
    // back to slow filtering inside Qt Contacts Mobility)
    QContactDetailFilter cdf3;
    cdf3.setDetailDefinitionName(QContactNickname::DefinitionName, QContactNickname::FieldNickname);
    cdf3.setValue("aba");
    cdf3.setMatchFlags(QContactFilter::MatchFixedString);
    cnt_ids = m_engine->contactIds(cdf3, sortOrder, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    QVERIFY(error == QContactManager::NoError);
    
    QContactDetailFilter cdf4;
    cdf4.setDetailDefinitionName(QContactNickname::DefinitionName, QContactNickname::FieldNickname);
    cdf4.setValue("aba");
    cdf4.setMatchFlags(QContactFilter::MatchExactly);
    cnt_ids = m_engine->contactIds(cdf4, sortOrder, &error);
    // check counts 
    seachedcontactcount = cnt_ids.count();
    expectedCount =0;  
    QVERIFY(expectedCount == seachedcontactcount);
    QVERIFY(error == QContactManager::NoError);
}   

void TestFiltering::testContactDetailFilter_3()
{
    // Test for fetching Mobile number
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);
    
    QContactDetailFilter mobileFilter;
    mobileFilter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, 
            QContactPhoneNumber::FieldSubTypes); 
    mobileFilter.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    
    
    cnt_ids = m_engine->contactIds(mobileFilter, sortOrder, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =7;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestFiltering::testContactDetailFilter_4()
{
	//Test for checking Grp conference number filtering
    //Create contact with phone number 1234567
    QContact phonecontact;

    QContactName contactName;
    contactName.setFirstName("Stefann");
    contactName.setLastName("Fedrernn");
    phonecontact.saveDetail(&contactName);
    
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("1234567");
    phonecontact.saveDetail(&number);

    QContactManager::Error err;
    QVERIFY(m_engine->saveContact(&phonecontact, &err));
    
    //Create Group with Conference number 1234567
    QContact groupcontact;
    groupcontact.setType(QContactType::TypeGroup);
    QContactName groupName;
    groupName.setCustomLabel("groupname");
    groupcontact.saveDetail(&groupName);
    
    QContactPhoneNumber confnumber ;
    confnumber.setNumber("1234567");
    groupcontact.saveDetail(&confnumber);
    
 
    QVERIFY(m_engine->saveContact(&groupcontact, &err));
    
    // Test Contact Sip field support
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);
    
    QContactDetailFilter phoneFilter; 
    phoneFilter.setDetailDefinitionName( QContactPhoneNumber::DefinitionName, 
                                             QContactPhoneNumber::FieldNumber);
    QString phnumber("1234567");
    phoneFilter.setValue(phnumber);
    
    
    cnt_ids = m_engine->contactIds(phoneFilter, sortOrder, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    
    QVERIFY(m_engine->removeContact(phonecontact.localId(), &err));
    QVERIFY(m_engine->removeContact(groupcontact.localId(), &err));
    
}

void TestFiltering::testContactDetailFilter_5()
{    
    // Test Contact with email 
    // Filter with Empty Field value
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);
    
    QContactDetailFilter emailFilter; 
    emailFilter.setDetailDefinitionName( QContactEmailAddress::DefinitionName);
   
    
    
    cnt_ids = m_engine->contactIds(emailFilter, sortOrder, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =9;  
    QVERIFY(expectedCount == seachedcontactcount);
    QVERIFY(error == QContactManager::NoError);
}

void TestFiltering::testContactDetailFilter_6()
{    
    // Test Contact with phone number 
    // Filter with Empty Field value
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName);
    
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =9;  
    QVERIFY(expectedCount == seachedcontactcount);
    QVERIFY(error == QContactManager::NoError);
}

void TestFiltering::testContactDetailFilter_7()
{    
    // Test Contact with Online account 
    // Filter with Empty Field value
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactOnlineAccount::DefinitionName);
    
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
    QVERIFY(error == QContactManager::NoError);
}

void TestFiltering::testContactDetailFilter_8()
{    
    // Fetch contacts list using display label
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
        
    QContactDetailFilter filter;
    QString filterString("J");
    QStringList searchList = filterString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    filter.setDetailDefinitionName(QContactDisplayLabel::DefinitionName,
        QContactDisplayLabel::FieldLabel);
    filter.setMatchFlags(QContactFilter::MatchStartsWith);
    filter.setValue(searchList);
    
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =2;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestFiltering::testContactDetailFilter_9()
{    
    // Fetch all contacts having phonenumbers
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    // Empty field name
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName); 
    
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =9;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestFiltering::testRelationshipFilter()
{
    // create a group contact
    QContactManager::Error err;
    QContact groupContact;
    groupContact.setType(QContactType::TypeGroup);
    m_engine->saveContact(&groupContact, &err);

    // create a local contact
    QContact aContact;
    aContact.setType(QContactType::TypeContact);
    m_engine->saveContact(&aContact, &err);

    // create a relationship
    QContactRelationship relationship;
    relationship.setRelationshipType(QContactRelationship::HasMember);
    relationship.setFirst(groupContact.id());
    relationship.setSecond(aContact.id());
    
    // save relationship
    m_engine->saveRelationship(&relationship, &err);
    
    QList<QContactLocalId> cnt_ids;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;
    
    QContactRelationshipFilter groupFilter;                   
    groupFilter.setRelationshipType(QContactRelationship::HasMember);
    groupFilter.setRelatedContactId(groupContact.id());                
    groupFilter.setRelatedContactRole(QContactRelationship::First);

    
    cnt_ids = m_engine->contactIds(groupFilter, sortOrder, &error);
    
    // check counts 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount =1;  
    QVERIFY(expectedCount == seachedcontactcount);
}

void TestFiltering::testIntersectionFilter()
{
    testIntersectionFilter_1();
    testIntersectionFilter_2();
    testIntersectionFilter_3();
    testIntersectionFilter_4();
}

void TestFiltering::testIntersectionFilter_1()
{
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("John");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    f2.setValue("Len.Jo@nok");
    f2.setMatchFlags(QContactFilter::MatchStartsWith);
    
    //Create an intersection filter with the above created filters 
    QList<QContactLocalId> cnt_ids;
    QContactIntersectionFilter filter;
    filter.append(f1);
    filter.append(f2);
    QList<QContactSortOrder> sortOrder;
    QContactManager::Error error;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);

    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);

    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 1; 
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
    
    for(int j=0; j<seachedcontactcount; j++) 
    {
        QContactManager::Error err;
        QContact sc = m_engine->contact(cnt_ids[j], QContactFetchHint(), &err);
        
        //Get the first name
        QContactName name = sc.detail(QContactName::DefinitionName);
        QString firstname = name.firstName();
        QVERIFY(firstname.contains("John"));        
        //get the Email address
        QContactEmailAddress eaddr = sc.detail(QContactEmailAddress::DefinitionName);
        QString email = eaddr.emailAddress();
        // Not testing equal to due to MatchFlags definitions
        // in the filter.
        QVERIFY(email.contains("Len.Jo@nok"));     
    }
}

void TestFiltering::testIntersectionFilter_2()
{
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("John");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    f2.setValue("Len.Jo@nok");
    f2.setMatchFlags(QContactFilter::MatchStartsWith);
    
    //Create third filter
    QContactDetailFilter f3;
    f3.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f3.setValue("Mic");
    f3.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create fourth filter
    QContactDetailFilter f4;
    f4.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    f4.setValue("ste.Fed@nok");
    f4.setMatchFlags(QContactFilter::MatchStartsWith);
    
    //Create first union filter
    QContactUnionFilter ufilter1;
    ufilter1.append(f1);
    ufilter1.append(f3);
    
    //Create second union filter
    QContactUnionFilter ufilter2;
    ufilter2.append(f2);
    ufilter2.append(f4);
    
    //Create an intersection filter with the above union filters 
    QList<QContactLocalId> cnt_ids;
    QContactIntersectionFilter filter;
    filter.append(ufilter1);
    filter.append(ufilter2);
    QList<QContactSortOrder> sortOrder;
    QContactManager::Error error;

    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    
    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 1; 
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
    
    for(int j=0; j<seachedcontactcount; j++) 
    {
        QContactManager::Error err;
        QContact sc = m_engine->contact(cnt_ids[j], QContactFetchHint(), &err);
        //Get the first name
        QContactName name = sc.detail(QContactName::DefinitionName);
        QString firstname = name.firstName();
        QVERIFY(firstname.contains("John"));
    }
}

void TestFiltering::testIntersectionFilter_3()
{
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("Mic");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f2.setValue("035022423");
    
    //Create an intersection filter with the above created filters 
    QList<QContactLocalId> cnt_ids;
    QContactIntersectionFilter filter;
    filter.append(f1);
    filter.append(f2);
    QList<QContactSortOrder> sortOrder;
    QContactManager::Error error;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);

    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);

    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 1; 
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
    
    for(int j=0; j<seachedcontactcount; j++) 
    {
        QContact sc = m_engine->contact(cnt_ids[j],QContactFetchHint(),&error);
        //Get the first name
        QContactName name = sc.detail(QContactName::DefinitionName);
        QString firstname = name.firstName();
        //get the Phone number
        QContactPhoneNumber phoneDetail = sc.detail(QContactPhoneNumber::DefinitionName);
        QString number = phoneDetail.number();
        // Not testing equal to due to MatchFlags definitions
        // in the filter.
        QVERIFY(firstname.contains("Mic") && (number.contains("5022423")));
    }
}

void TestFiltering::testIntersectionFilter_4()
{
    //Create first filter to match all contacts with name field
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("Jo");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create second filter to fetch all mobile numbers
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    f2.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));

    
    //Create an intersection filter with the above created filters 
    QList<QContactLocalId> cnt_ids;
    QContactIntersectionFilter filter;
    filter.append(f1);
    filter.append(f2);
    QContactManager::Error error;
    
    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &error);

    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 1; 
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
    
    for(int j=0; j<seachedcontactcount; j++) 
    {
        QContact sc = m_engine->contact(cnt_ids[j],QContactFetchHint(),&error);
        //Get the first name
        QContactName name = sc.detail(QContactName::DefinitionName);
        QString firstname = name.firstName();
        //get the Phone number
        QContactPhoneNumber phoneDetail = sc.detail(QContactPhoneNumber::DefinitionName);
        QString number = phoneDetail.number();
        // Not testing equal to due to MatchFlags definitions
        // in the filter.
        QVERIFY(firstname.contains("Jo"));
    }
}


void TestFiltering::testUnionFilter()
    {
    testUnionFilter_1();
    testUnionFilter_2();
    testUnionFilter_3();
    testUnionFilter_4();
    }

void TestFiltering::testUnionFilter_1()
{
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("Mic");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    f2.setValue("Len.Jo@nok");
    f2.setMatchFlags(QContactFilter::MatchStartsWith);
    
    //Create an union filter with the above created filters 
    QList<QContactLocalId> cnt_ids;
    QContactUnionFilter filter;
    filter.append(f1);
    filter.append(f2);
    QList<QContactSortOrder> sortOrder;
    QContactManager::Error error;
    
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);

    QContactSortOrder sortOrderLastName;
    sortOrderLastName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldLastName);

    sortOrder.append(sortOrderFirstName);
    sortOrder.append(sortOrderLastName);

    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    
    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 2; 
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
        
    for(int j=0; j<seachedcontactcount; j++) 
    {
        QContactManager::Error err;
        QContact sc = m_engine->contact(cnt_ids[j], QContactFetchHint(), &err);
        //Get the first name
        QContactName name = sc.detail(QContactName::DefinitionName);
        QString firstname = name.firstName();
        //get the Email address
        QContactEmailAddress eaddr = sc.detail(QContactEmailAddress::DefinitionName);
        QString email = eaddr.emailAddress();
        // Not testing equal to due to MatchFlags definitions
        // in the filter.
        QVERIFY(firstname.contains("Mic") || (email.contains("Len.Jo@nok")));     
    }
}


void TestFiltering::testUnionFilter_2()
{
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("n");
    f1.setMatchFlags(QContactFilter::MatchContains);
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    f2.setValue("@nok");
    f2.setMatchFlags(QContactFilter::MatchContains);
    
    //Create an union filter with the above created filters 
    QList<QContactLocalId> cnt_ids;
    QContactUnionFilter filter;
    filter.append(f1);
    filter.append(f2);
    QList<QContactSortOrder> sortOrder;
    QContactManager::Error error;

    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    
    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 5; 
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
        
    for(int j=0; j<seachedcontactcount; j++) 
    {
        QContactManager::Error err;
        QContact sc = m_engine->contact(cnt_ids[j], QContactFetchHint(), &err);
        //Get the first name
        QContactName name = sc.detail(QContactName::DefinitionName);
        QString firstname = name.firstName();
        //get the Email address
        QContactEmailAddress eaddr = sc.detail(QContactEmailAddress::DefinitionName);
        QString email = eaddr.emailAddress();
        // Not testing equal to due to MatchFlags definitions
        // in the filter.
        QVERIFY(firstname.contains("n") || (email.contains("@nok")));     
    }
}
void TestFiltering::testUnionFilter_3()
{
    //Create first filter
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("J");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create second filter
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    f2.setValue("035022423");
    
    //Create an union filter
    QList<QContactLocalId> cnt_ids;
    QContactUnionFilter filter;
    filter.append(f1);
    filter.append(f2);
    QList<QContactSortOrder> sortOrder;
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    sortOrder.append(sortOrderFirstName);
    QContactManager::Error error;

    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    
    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 4; 
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
        
    for(int j=0; j<seachedcontactcount; j++) 
    {
        QContact sc = m_engine->contact(cnt_ids[j],QContactFetchHint(),&error);
        //Get the first name
        QContactName name = sc.detail(QContactName::DefinitionName);
        QString firstname = name.firstName();
        //get the Phone number
        QContactPhoneNumber phoneDetail = sc.detail(QContactPhoneNumber::DefinitionName);
        QString number = phoneDetail.number();
        // Not testing equal to due to MatchFlags definitions
        // in the filter.
        QVERIFY(firstname.contains("J") || (number.contains("5022423")));     
    }
}

void TestFiltering::testUnionFilter_4()
{
    //Create first filter to match all contacts with name field
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldFirstName);
    f1.setValue("J");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    //Create second filter to fetch all mobile numbers
    QContactDetailFilter f2;
    f2.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldSubTypes); 
    f2.setValue(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    
    //Create an union filter
    QList<QContactLocalId> cnt_ids;
    QContactUnionFilter filter;
    filter.append(f1);
    filter.append(f2);
    QList<QContactSortOrder> sortOrder;
    QContactSortOrder sortOrderFirstName;
    sortOrderFirstName.setDetailDefinitionName(QContactName::DefinitionName,
        QContactName::FieldFirstName);
    sortOrder.append(sortOrderFirstName);
    QContactManager::Error error;

    //Search for contacts 
    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);
    
    // Now check the results 
    int seachedcontactcount = cnt_ids.count();
    int expectedCount = 8;
    QVERIFY(seachedcontactcount == expectedCount);
    QVERIFY(error == QContactManager::NoError);
}

void TestFiltering::testDefaultFilter()
{
    QList<QContactLocalId> cnt_ids;
    QContactFilter filter;
    QContactManager::Error error;
    QList<QContactSortOrder> sortOrder;

    cnt_ids = m_engine->contactIds(filter, sortOrder, &error);

    QVERIFY(error == QContactManager::NoError);
}

void TestFiltering::testDefaultFilterWithPredictiveSearch()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    testString << "Micheal" << "6079949400404";
    QString pattern = "6";
    bool isPredSearch = false;
    QContactManager::Error err;
    
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    for( int i=0;i<cnt_ids.count();i++ ) {
            QContactLocalId cid = cnt_ids.at( i );    
            QContact contact = m_engine->contact( cid,  QContactFetchHint(), &err );
            QContactName contactName = contact.detail( QContactName::DefinitionName );
            testString.removeAll( contactName.value( QContactName::FieldFirstName ) );
            testString.removeAll( contactName.value( QContactName::FieldLastName ) );
            }
    QVERIFY( testString.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch");
   
}

void TestFiltering::testDefaultFilterWithPredictiveSearch2()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch2");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    testString << "Frady" << "6079949400404";  
    QString pattern = "603";
       
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    for( int i=0;i<cnt_ids.count();i++ ) {
                QContactLocalId cid = cnt_ids.at( i );    
                QContact contact = m_engine->contact( cid, QContactFetchHint(), &err );
                QContactName contactName = contact.detail( QContactName::DefinitionName );
                testString.removeAll( contactName.value( QContactName::FieldFirstName ) );
                testString.removeAll( contactName.value( QContactName::FieldLastName ) );
                } 
   QVERIFY( testString.count() == 0 );
   TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch2");
}

void TestFiltering::testDefaultFilterWithPredictiveSearch3()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch3");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    testString << "Fedrernn"; 
    QString pattern = "3307";
       
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    for( int i=0;i<cnt_ids.count();i++ ) {
                QContactLocalId cid = cnt_ids.at( i );    
                QContact contact = m_engine->contact( cid, QContactFetchHint(), &err );
                QContactName contactName = contact.detail( QContactName::DefinitionName );
                testString.removeAll( contactName.value( QContactName::FieldFirstName ) );
                testString.removeAll( contactName.value( QContactName::FieldLastName ) );
                } 
    QVERIFY( testString.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch3");
}

void TestFiltering::testDefaultFilterWithPredictiveSearch4()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch4");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    testString << "Johnn" ; 
    QString pattern = "505";
       
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    for( int i=0;i<cnt_ids.count();i++ ) {
                    QContactLocalId cid = cnt_ids.at( i );    
                    QContact contact = m_engine->contact( cid, QContactFetchHint(), &err );
                    QContactName contactName = contact.detail( QContactName::DefinitionName );
                    testString.removeAll( contactName.value( QContactName::FieldFirstName ) );
                    testString.removeAll( contactName.value( QContactName::FieldLastName ) );
                    } 
    QVERIFY( testString.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch4");
}

void TestFiltering::testDefaultFilterWithPredictiveSearch5()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch5");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    QString pattern = "78";
    testString << "Stefann" << "Susan"; 
    
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    for( int i=0;i<cnt_ids.count();i++ ) {
                    QContactLocalId cid = cnt_ids.at( i );    
                    QContact contact = m_engine->contact( cid, QContactFetchHint(), &err );
                    QContactName contactName = contact.detail( QContactName::DefinitionName );
                    testString.removeAll( contactName.value( QContactName::FieldFirstName ) );
                    testString.removeAll( contactName.value( QContactName::FieldLastName ) );
                    } 
    QVERIFY( testString.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch5");
}

void TestFiltering::testDefaultFilterWithPredictiveSearch6()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch6");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    QString pattern = "502";
    testString << "Kekkonen" << "Bondnn"; 
    
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    for( int i=0;i<cnt_ids.count();i++ ) {
            QContactLocalId cid = cnt_ids.at( i );    
            QContact contact = m_engine->contact( cid, QContactFetchHint(), &err );
            QContactName contactName = contact.detail( QContactName::DefinitionName );
            testString.removeAll( contactName.value( QContactName::FieldFirstName ) );
            testString.removeAll( contactName.value( QContactName::FieldLastName ) );
            }
    QVERIFY( testString.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch6");
}

void TestFiltering::testDefaultFilterWithPredictiveSearch7()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch7");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    QString pattern = "000";
    testString << "000000007"; 
    
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);
    
    QString count;
    QString contact_id;
    count.setNum(cnt_ids.count());
    
    LOG2(pattern, count);

    for( int i=0;i<cnt_ids.count();i++ ) {
            QContactLocalId cid = cnt_ids.at( i );
            contact_id.setNum(cid);
            LOG2(pattern,contact_id);
            QContact contact = m_engine->contact( cid, QContactFetchHint(), &err );
            QContactName contactName = contact.detail( QContactName::DefinitionName );
            testString.removeAll( contactName.value( QContactName::FieldFirstName ) );
            testString.removeAll( contactName.value( QContactName::FieldLastName ) );
            }
    QVERIFY( testString.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch7");
}

void TestFiltering::testDefaultFilterWithPredictiveSearch8()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch8");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    QString pattern = "99999099999";
    
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    QVERIFY( cnt_ids.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch8");
}

void TestFiltering::testDefaultFilterWithPredictiveSearch9()
{
    TEST_BEGIN_LOG("testDefaultFilterWithPredictiveSearch9");
    QList<QContactLocalId> cnt_ids;
    QContactDetailFilter df;
    QList<QString> testString;
    QContactManager::Error err;

    bool isPredSearch = false;
    QString pattern = "12+22";
    
    
    df.setDetailDefinitionName(QContactName::DefinitionName);
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( pattern );
    cnt_ids = m_engine->contactIds(df, QList<QContactSortOrder>(), &err);

    QVERIFY( testString.count() == 0 );
    TEST_PASSED_LOG("testDefaultFilterWithPredictiveSearch9");
}
void TestFiltering::testFilterSupported()
    {
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactNickname::DefinitionName, QContactNickname::FieldNickname);
    f1.setValue("Mic");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    bool flag(false);
      
    //Supported cases
    CntFilterDefault filterDefault(*m_database,srvConnection,dbInfo);
    flag = filterDefault.filterSupported(QContactFilter());
    QVERIFY(flag ==true);
    
    CntFilterRelationship filterRlationship(*m_database,srvConnection,dbInfo);
    QContactRelationshipFilter relationFilter;                   
    relationFilter.setRelationshipType(QContactRelationship::HasMember);
    flag = filterRlationship.filterSupported(relationFilter);
    QVERIFY(flag ==true);
       
    CntFilterUnion filterUnion(*m_database,srvConnection,dbInfo);
    flag = filterUnion.filterSupported(QContactUnionFilter());
    QVERIFY(flag ==true);   
               
    CntFilterIntersection filterIntersection(*m_database,srvConnection,dbInfo);
    flag = filterIntersection.filterSupported(QContactIntersectionFilter());
    QVERIFY(flag ==true);
        
    //Not supported cases
    CntFilterDetail filterDetail(*m_database,srvConnection,dbInfo);
    flag = filterDetail.filterSupported(QContactDetailFilter());
    QVERIFY(flag ==false);
    
    flag = filterDefault.filterSupported(f1);
    QVERIFY(flag ==false);
        
    flag = filterRlationship.filterSupported(f1);
    QVERIFY(flag ==false);   
       
    flag = filterUnion.filterSupported(f1);
    QVERIFY(flag ==false);   
               
    flag = filterIntersection.filterSupported(f1);
    QVERIFY(flag ==false);
    }

void TestFiltering::testCreateSelectQuery()
    {
    QContactDetailFilter f1;
    f1.setDetailDefinitionName(QContactNickname::DefinitionName, QContactNickname::FieldNickname);
    f1.setValue("Mic");
    f1.setMatchFlags(QContactFilter::MatchStartsWith);
    CntSymbianSrvConnection srvConnection(m_engine);
    CntDbInfo dbInfo(m_engine);
    QString sqlquery;
    QContactManager::Error error;
    
    //Not supported cases
    error = QContactManager::NoError;
    CntFilterDetail filterDtl(*m_database,srvConnection,dbInfo);
    filterDtl.createSelectQuery(QContactFilter(),sqlquery,&error);
    QVERIFY(error == QContactManager::NotSupportedError);
    
    CntFilterDefault filterDefault(*m_database,srvConnection,dbInfo);
    error = QContactManager::NoError;
    filterDefault.createSelectQuery(f1,sqlquery,&error);
    QVERIFY(error == QContactManager::NotSupportedError);
        
    CntFilterRelationship filterRlationship(*m_database,srvConnection,dbInfo);
    error = QContactManager::NoError;
    filterRlationship.createSelectQuery(f1,sqlquery,&error);
    QVERIFY(error == QContactManager::NotSupportedError); 
       
    CntFilterUnion filterUnion(*m_database,srvConnection,dbInfo);
    error = QContactManager::NoError;
    filterUnion.createSelectQuery(f1,sqlquery,&error);
    QVERIFY(error == QContactManager::NotSupportedError);
               
    CntFilterIntersection filterIntersection(*m_database,srvConnection,dbInfo);
    error = QContactManager::NoError;
    filterIntersection.createSelectQuery(f1,sqlquery,&error);
    QVERIFY(error == QContactManager::NotSupportedError);
    
    }

void TestFiltering::testFavoriteDetailFilter()
    {
    QContactManager::Error err;
    QContactFetchHint hint;
    //save one contact with favorite detail
    QContact c;
    c.setType(QContactType::TypeContact);
    QContactName name;
    name.setFirstName("firstname");
    c.saveDetail(&name);
    QContactFavorite fav;
    fav.setFavorite(true);
    fav.setIndex(100);
    c.saveDetail(&fav);
    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);
    
    // find all favorite contacts
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactFavorite::DefinitionName, QContactFavorite::FieldFavorite); 
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(err == QContactManager::NoError);
    int ccc = cnt_ids.count();
    QVERIFY(cnt_ids.count() == 1);
    
    //fetch found contact and check favorite detail
    QContact savedContact = m_engine->contact(cnt_ids.at(0), hint, &err);
    QVERIFY(err == QContactManager::NoError);
    QContactFavorite savedFav = savedContact.detail<QContactFavorite>();
    QVERIFY(savedFav.isFavorite());
    QVERIFY(savedFav.index() == 100); 
    }

void TestFiltering::testMatchFlags()
{
    QContactManager::Error err;
    QContactFetchHint hint;
    
    //save a contact without favorite detail
    QContact c1;
    c1.setType(QContactType::TypeContact);
    QContactName name1;
    name1.setFirstName("somename");
    c1.saveDetail(&name1);
    QContactEmailAddress email;
    email.setEmailAddress("EMAIL123");
    c1.saveDetail(&email);
    QVERIFY(m_engine->saveContact(&c1, &err));
    QVERIFY(err == QContactManager::NoError);
    
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
    filter.setValue("email123");
    filter.setMatchFlags(QContactFilter::MatchExactly);
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(cnt_ids.count() == 0);

    filter.setMatchFlags(QContactFilter::MatchFixedString);
    cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(cnt_ids.count() == 1);
    
    filter.setValue("EMAIL123");
    filter.setMatchFlags(QContactFilter::MatchExactly);
    cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(cnt_ids.count() == 1);
}

void TestFiltering::testContactTypeFilter()
{    
    QContactManager::Error err;
    
    // Remove all contacts from the database
    QList<QContactLocalId> contacts = m_engine->contactIds(QContactFilter(),QList<QContactSortOrder>(), &err);
    QMap<int, QContactManager::Error> errorMap;
    m_engine->removeContacts(contacts, &errorMap, &err);
    
    //check amount of contacts
    QContactDetailFilter filter;
    filter.setDetailDefinitionName(QContactType::DefinitionName, QContactType::FieldType);
    filter.setValue(QContactType::TypeContact);
    QList<QContactLocalId> cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(cnt_ids.count() == 0);
    
    //save a new contact
    QContact c;
    c.setType(QContactType::TypeContact);
    QContactName name;
    name.setFirstName("somename");
    c.saveDetail(&name);
    QVERIFY(m_engine->saveContact(&c, &err));
    QVERIFY(err == QContactManager::NoError);
    
    //check amount of contacts
    cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(cnt_ids.count() == 1);
    
    //check amount of groups
    filter.setValue(QContactType::TypeGroup);
    cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(cnt_ids.count() == 0);
    
    //save a new group
    QContact group;
    group.setType(QContactType::TypeGroup);
    QContactName groupName;
    groupName.setCustomLabel("group1");
    group.saveDetail(&groupName);
    QVERIFY(m_engine->saveContact(&group, &err));
    QVERIFY(err == QContactManager::NoError);
    
    //check amount of groups
    cnt_ids = m_engine->contactIds(filter, QList<QContactSortOrder>(), &err);
    QVERIFY(err == QContactManager::NoError);
    QVERIFY(cnt_ids.count() == 1);
}

//QTEST_MAIN(TestFiltering);
/*int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");
    
    QApplication app(argc, argv);
    //TestResultXmlParser parser;
    
    TestFiltering testFiltering;
    QString resultFileName = "c:/testFiltering.xml";
    QStringList args_logsTestFiltering( "testFiltering");
    args_logsTestFiltering << "-xml" << "-o" << resultFileName;
    QTest::qExec(&testFiltering, args_logsTestFiltering);
    
    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}
*/

#if defined(WRITE_LOGS)
void WriteTestLog(const QString a, const QString b)
    {
    TPtrC16 ptr(reinterpret_cast<const TUint16*>(a.utf16()));
    WriteTestPart(ptr);
    
    if (b.size() > 0)
        {
        TPtrC16 ptr2(reinterpret_cast<const TUint16*>(b.utf16()));
        WriteTestPart(ptr2);
        }
    }

void WriteTestPart(const TDesC& s)
    {
    // RDebug::Print() only writes first 256 chars
    const TInt KMaxLength = 255;
        
    TInt pos(0);
    TInt len = s.Length();  
    while (pos < len)
        {
        TInt partLength = KMaxLength;
        if (pos + partLength > len)
            {
            partLength = len - pos;
            }
        TPtrC16 part = s.Mid(pos, partLength);
        RDebug::Print(part);
        pos += KMaxLength;
        }
    }
#endif // #if defined(WRITE_LOGS)
