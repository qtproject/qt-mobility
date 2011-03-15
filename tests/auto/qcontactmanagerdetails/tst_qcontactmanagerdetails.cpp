/****************************************************************************
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QDateTime>
#include "qtcontacts.h"
#include "qcontactmanagerdataholder.h"

QTM_USE_NAMESPACE

// to get QFETCH to work with the template expression...
typedef QMap<QString,QString> tst_QContactManagerDetails_QStringMap;
Q_DECLARE_METATYPE(tst_QContactManagerDetails_QStringMap)
Q_DECLARE_METATYPE(QList<QContactLocalId>)


class tst_QContactManagerDetails : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerDetails();
    virtual ~tst_QContactManagerDetails();

private:
    void addManagers();

    template <typename T>
    void removeDetail( QContact& contact )
    {
        T detail = contact.detail<T>();
        contact.removeDetail( &detail );
    }

    bool saveAndLoadContact( QContactManager *cm, QContact &original, QContact &loaded );
    void saveAndVerifyContact( QContactManager *cm, QContact &original );

    QScopedPointer<QContactManagerDataHolder> managerDataHolder;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testAddress();
    void testAddress_data() {addManagers();}
    void testAnniversary();
    void testAnniversary_data() {addManagers();}
    void testAvatar();
    void testAvatar_data() {addManagers();}
    void testBirthday();
    void testBirthday_data() {addManagers();}
    void testEmailAddress();
    void testEmailAddress_data() {addManagers();}
    void testName();
    void testName_data() {addManagers();}
    void testNickName();
    void testNickName_data() {addManagers();}
    void testOrganisation();
    void testOrganisation_data() {addManagers();}
    void testOnlineAccount();
    void testOnlineAccount_data() {addManagers();}
    void testPhoneNumber();
    void testPhoneNumber_data() {addManagers();}
    void testUrl();
    void testUrl_data() {addManagers();}
    void testRingtone();
    void testRingtone_data() {addManagers();}

private slots:

};

tst_QContactManagerDetails::tst_QContactManagerDetails()
{
}

tst_QContactManagerDetails::~tst_QContactManagerDetails()
{
}

void tst_QContactManagerDetails::addManagers()
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
    }
}

void tst_QContactManagerDetails::initTestCase()
{
    managerDataHolder.reset(new QContactManagerDataHolder());
}

void tst_QContactManagerDetails::cleanupTestCase()
{
    managerDataHolder.reset(0);
}

bool tst_QContactManagerDetails::saveAndLoadContact( QContactManager *cm, QContact &original, QContact &loaded )
{
    // Save contact
    if( cm->saveContact(&original) == false )
        return false;

    // Check the id
    if( original.localId() == 0 )
        return false;

    // Load same contact from database
    loaded = cm->contact( original.localId() );
    if( cm->error() )
        return false;

    // Ignore some details which are not relevant and will mess
    // up direct comparison between two contacts.
    removeDetail<QContactDisplayLabel>(original);
    removeDetail<QContactDisplayLabel>(loaded);
    removeDetail<QContactGuid>(original);
    removeDetail<QContactGuid>(loaded);
    removeDetail<QContactTimestamp>(original);
    removeDetail<QContactTimestamp>(loaded);

    return true;
}

void tst_QContactManagerDetails::saveAndVerifyContact( QContactManager *cm, QContact &original )
{
    QContact loaded;
    QVERIFY( saveAndLoadContact( cm, original, loaded ) );
    if (original != loaded) {
        qDebug() << "expected: " << original;
        qDebug() << "loaded: " << loaded;
        QCOMPARE( loaded.details().count(), original.details().count() );
        QCOMPARE( loaded, original );
    }
}

void tst_QContactManagerDetails::testAddress()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    QContactDetailDefinition def = cm->detailDefinition(QContactAddress::DefinitionName);
    if (def.isEmpty()
            || !def.fields().contains(QContactAddress::FieldStreet)
            || !def.fields().contains(QContactAddress::FieldLocality)
            || !def.fields().contains(QContactAddress::FieldRegion)
            || !def.fields().contains(QContactAddress::FieldPostcode)
            || !def.fields().contains(QContactAddress::FieldCountry)
            || !def.fields().contains(QContactAddress::FieldPostOfficeBox))
        QSKIP("This backend does not support the required detail!", SkipSingle);

    // general address
    {
        QContact c;

        QContactAddress a;
        a.setStreet("street");
        a.setLocality("locality");
        a.setRegion("region");
        a.setPostcode("postcode");
        a.setCountry("country");
        a.setPostOfficeBox("POBox");
        c.saveDetail( &a );

        saveAndVerifyContact( cm.data(), c );
    }

    if (def.isUnique())
        QSKIP("This backend does not support more than one of this detail", SkipSingle);

    // general address *2
    {
        QContact c1;

        QContactAddress a1;
        a1.setStreet("street1");
        a1.setLocality("locality1");
        a1.setRegion("region1");
        a1.setPostcode("postcode1");
        a1.setCountry("country1");
        a1.setPostOfficeBox("POBox1");
        c1.saveDetail( &a1 );

        QContactAddress a2;
        a2.setStreet("street2");
        a2.setLocality("locality2");
        a2.setRegion("region2");
        a2.setPostcode("postcode2");
        a2.setCountry("country2");
        a2.setPostOfficeBox("POBox2");
        c1.saveDetail( &a2 );

        QContact c2;
        saveAndLoadContact( cm.data(), c1, c2 );

        QList<QContactAddress> addresses = c2.details<QContactAddress>();
        QVERIFY( addresses.count() == 2 );
        QVERIFY( a1 == addresses[0] );
        QVERIFY( a2 == addresses[1] );
    }

    if (!def.fields().contains(QContactDetail::FieldContext)) {
        QSKIP("This backend does not support contexts", SkipSingle);
    }

    // general + home + work address
    {
        QContact c;

        // General address
        QContactAddress a1;
        a1.setStreet("street1");
        c.saveDetail( &a1 );

        // home address
        QContactAddress a2;
        a2.setStreet("street2");
        a2.setContexts( QContactDetail::ContextHome );
        c.saveDetail( &a2 );

        // work address
        QContactAddress a3;
        a3.setStreet("street3");
        a3.setContexts( QContactDetail::ContextWork );
        c.saveDetail( &a3 );

        saveAndVerifyContact( cm.data(), c );
    }
}
void tst_QContactManagerDetails::testAnniversary()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->detailDefinition(QContactAnniversary::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactAnniversary a;
    a.setOriginalDate( QDate(2009,9,9) );
    c.saveDetail( &a );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testAvatar()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->detailDefinition(QContactAvatar::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactAvatar a;
    a.setImageUrl( QUrl::fromLocalFile("c:\\foo\\bar.jpeg") );
    c.saveDetail( &a );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testBirthday()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->detailDefinition(QContactBirthday::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactBirthday b;
    b.setDate( QDate(2001,1,1) );
    c.saveDetail( &b );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testEmailAddress()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->detailDefinition(QContactEmailAddress::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactEmailAddress e;
    e.setEmailAddress( "foo@bar.com" );
    c.saveDetail( &e );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testName()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));
    QContact c;

    QContactDetailDefinition def = cm->detailDefinition(QContactName::DefinitionName);
    if (def.isEmpty()
            || !def.fields().contains(QContactName::FieldPrefix)
            || !def.fields().contains(QContactName::FieldFirstName)
            || !def.fields().contains(QContactName::FieldMiddleName)
            || !def.fields().contains(QContactName::FieldLastName)
            || !def.fields().contains(QContactName::FieldSuffix))
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContactName n;
    n.setPrefix( "prefix" );
    n.setFirstName( "first" );
    n.setMiddleName( "middle" );
    n.setLastName( "last" );
    n.setSuffix( "suffix" );
    c.saveDetail( &n );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testNickName()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->detailDefinition(QContactNickname::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactNickname n1;
    n1.setNickname("nickname1");
    c.saveDetail( &n1 );

    saveAndVerifyContact( cm.data(), c );

    QContactNickname n2;
    n2.setNickname("nickname2");
    c.saveDetail( &n2 );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testOrganisation()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->detailDefinition(QContactOrganization::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactOrganization o;
    o.setName( "Foreign legion" );
    o.setTitle( "Bicycle mechanic" );
    c.saveDetail( &o );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testOnlineAccount()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    QContactDetailDefinition def = cm->detailDefinition(QContactOnlineAccount::DefinitionName);
    if (def.isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactOnlineAccount o;
    o.setAccountUri( "john@example.com" );

    if (def.fields().contains(QContactOnlineAccount::FieldProtocol))
        o.setProtocol(QContactOnlineAccount::ProtocolJabber);
    else
        qDebug() << "Manager doesn't support Protocol";

    c.saveDetail( &o );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testPhoneNumber()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    QContactDetailDefinition def = cm->detailDefinition(QContactPhoneNumber::DefinitionName);

    // general landline number
    {
        QContact c;

        // General landline number
        QContactPhoneNumber n2;
        n2.setNumber( "2" );
        n2.setSubTypes( QContactPhoneNumber::SubTypeLandline );
        c.saveDetail( &n2 );

        saveAndVerifyContact( cm.data(), c );
    }

    if (def.isUnique()
            || !def.fields().contains(QContactPhoneNumber::FieldSubTypes)
            || !def.fields().contains(QContactDetail::FieldContext))
        QSKIP("This backend does not support multiple, subtyped or contexted phone numbers", SkipSingle);

    // home mobile number + work landline number
    {
        QContact c;

        // home mobile number
        QContactPhoneNumber n1;
        n1.setNumber( "1" );
        n1.setSubTypes( QContactPhoneNumber::SubTypeMobile );
        n1.setContexts( QContactDetail::ContextHome );
        c.saveDetail( &n1 );

        // work landline number
        QContactPhoneNumber n2;
        n2.setNumber( "2" );
        n2.setSubTypes( QContactPhoneNumber::SubTypeLandline );
        n2.setContexts( QContactDetail::ContextWork );
        c.saveDetail( &n2 );

        saveAndVerifyContact( cm.data(), c );
    }
}

void tst_QContactManagerDetails::testUrl()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    QContactDetailDefinition def = cm->detailDefinition(QContactUrl::DefinitionName);

    if (cm->detailDefinition(QContactUrl::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

    QContact c;

    QContactUrl u;
    u.setUrl("http://failblog.org");
    if (def.fields().value(QContactUrl::FieldSubType).allowableValues()
            .contains(QLatin1String(QContactUrl::SubTypeHomePage))) {
        u.setSubType(QContactUrl::SubTypeHomePage);
    }
    c.saveDetail( &u );

    saveAndVerifyContact( cm.data(), c );
}

void tst_QContactManagerDetails::testRingtone()
{
    QFETCH(QString, uri);
    QScopedPointer<QContactManager> cm(QContactManager::fromUri(uri));

    if (cm->detailDefinition(QContactRingtone::DefinitionName).isEmpty())
        QSKIP("This backend does not support the required detail!", SkipSingle);

#if defined(Q_OS_SYMBIAN)
    QUrl rtUrl(QUrl::fromLocalFile("C:\\data\\Sounds\\sample1.wav"));
#else
    QUrl rtUrl(QUrl::fromLocalFile("/home/user/sample.wav"));
#endif

    QContact c;
    QContactRingtone rt;
    rt.setAudioRingtoneUrl(rtUrl);
    c.saveDetail(&rt);

    saveAndVerifyContact( cm.data(), c );
}
QTEST_MAIN(tst_QContactManagerDetails)
#include "tst_qcontactmanagerdetails.moc"
