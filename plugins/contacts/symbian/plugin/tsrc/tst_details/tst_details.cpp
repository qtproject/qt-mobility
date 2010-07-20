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

//TESTED_CLASS=
//TESTED_FILES=

// to get QFETCH to work with the template expression...
typedef QMap<QString,QString> tst_details_QStringMap;
Q_DECLARE_METATYPE(tst_details_QStringMap)
Q_DECLARE_METATYPE(QList<QContactLocalId>)


class tst_details : public QObject
{
Q_OBJECT

public:
    tst_details();
    virtual ~tst_details();

private:
    void dumpContact(const QContact &c);
    
    template <typename T>
    void removeDetail( QContact& contact )
    {
        T detail = contact.detail<T>();
        contact.removeDetail( &detail );
    }    
    
    bool saveAndLoadContact( QContact &original, QContact &loaded );
    void saveAndVerifyContact( QContact &original );

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testAddress();
    void testAnniversary();
    void testAvatar();
    void testBirthday();
    void testEmailAddress();
    void testName();
    void testNickName();
    void testOrganisation();
    void testPhoneNumber();
    void testUrl();    
    void testPresence();

private slots:

};

tst_details::tst_details()
{
}

tst_details::~tst_details()
{
}

void tst_details::dumpContact(const QContact& contact)
{
    qDebug() << "Contact: " << contact.localId();
    QList<QContactDetail> details = contact.details();
    foreach(const QContactDetail& d, details) {
        qDebug() << "  " << d.definitionName() << ":";
        foreach( const QString& key, d.variantValues().keys() )
            qDebug() << "    " << key << d.variantValue(key);
    }
}

void tst_details::initTestCase()
{
    qDebug() << "Available managers:";
    foreach( const QString& manager, QContactManager::availableManagers() )
        qDebug() << manager;
    QVERIFY(QContactManager::availableManagers().contains("symbian"));

    QContactManager cm("symbian");
    QList<QContactLocalId> ids = cm.contactIds();
    cm.removeContacts( &ids );
}

void tst_details::cleanupTestCase()
{
    QContactManager cm("symbian");
    QList<QContactLocalId> ids = cm.contacts();
    cm.removeContacts( &ids );
}

bool tst_details::saveAndLoadContact( QContact &original, QContact &loaded )
{
    QContactManager cm("symbian");

    // Save contact
    if( cm.saveContact(&original) == false )
        return false;
    
    // Check the id
    if( original.localId() == 0 )
        return false;

    // Load same contact from database
    loaded = cm.contact( original.localId() );
    if( cm.error() )
        return false;

    // Ignore some details which are not relevant and will mess
    // up direct comparison between two contacts.
    removeDetail<QContactDisplayLabel>(original);
    removeDetail<QContactDisplayLabel>(loaded);
    removeDetail<QContactGuid>(original);
    removeDetail<QContactGuid>(loaded);
    removeDetail<QContactTimestamp>(original);    
    removeDetail<QContactTimestamp>(loaded);
    
    // Dump to log for debugging
    //qDebug() << "Original contact:";
    //dumpContact( original );
    //qDebug() << "Loaded contact:";
    //dumpContact( loaded );
    
    return true;
}

void tst_details::saveAndVerifyContact( QContact &original )
{
    QContact loaded;
    QVERIFY( saveAndLoadContact( original, loaded ) );
    QVERIFY( original.details().count() == loaded.details().count() );
    QCOMPARE( original, loaded );
}

void tst_details::testAddress()
{
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

        saveAndVerifyContact( c );
    }

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
        saveAndLoadContact( c1, c2 );
        
        // TODO: Should we now have two addresses or one?
        QVERIFY( c2.details<QContactAddress>().count() == 1 );
        QVERIFY( a2 == c2.detail<QContactAddress>() );
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

        saveAndVerifyContact( c );
    }
}
void tst_details::testAnniversary()
{
    QContact c;

    QContactAnniversary a;
    a.setOriginalDate( QDate(2009,9,9) );
    c.saveDetail( &a );

    saveAndVerifyContact( c );
}

void tst_details::testAvatar()
{
    QContact c;

    QContactAvatar a;
    a.setAvatar( "c:/foo/bar.jpeg" );
    a.setSubType( QContactAvatar::SubTypeImage );
    c.saveDetail( &a );

    saveAndVerifyContact( c );
}

void tst_details::testBirthday()
{
    QContact c;

    QContactBirthday b;
    b.setDate( QDate(2001,1,1) );
    c.saveDetail( &b );

    saveAndVerifyContact( c );
}

void tst_details::testEmailAddress()
{
    QContact c;

    QContactEmailAddress e;
    e.setEmailAddress( "foo@bar.com" );
    c.saveDetail( &e );

    saveAndVerifyContact( c );
}

void tst_details::testName()
{
    QContact c;

    QContactName n;
    n.setPrefix( "prefix" );
    n.setFirstName( "first" );
    n.setMiddleName( "middle" );
    n.setLastName( "last" );
    n.setSuffix( "suffix" );
    c.saveDetail( &n );

    saveAndVerifyContact( c );
}

void tst_details::testNickName()
{
    QContact c;

    QContactNickname n1;
    n1.setNickname("nickname1");
    c.saveDetail( &n1 );

    saveAndVerifyContact( c );

    QContactNickname n2;
    n2.setNickname("nickname2");
    c.saveDetail( &n2 );

    saveAndVerifyContact( c );
}

void tst_details::testOrganisation()
{
    QContact c;

    QContactOrganization o;
    o.setName( "Foreign legion" );
    o.setTitle( "Bicycle mechanic" );
    c.saveDetail( &o );

    saveAndVerifyContact( c );
}

void tst_details::testPhoneNumber()
{
    // general landline number
    {
        QContact c;

        // General landline number
        QContactPhoneNumber n2;
        n2.setNumber( "2" );
        n2.setSubTypes( QContactPhoneNumber::SubTypeLandline );
        c.saveDetail( &n2 );

        saveAndVerifyContact( c );
    }

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

        saveAndVerifyContact( c );
    }

    // fax number + dtmf number
    {
        QContact c;

        QContactPhoneNumber n2;
        n2.setNumber( "2" );
        n2.setSubTypes( QContactPhoneNumber::SubTypeFacsimile );
        c.saveDetail( &n2 );

        QContactPhoneNumber n3;
        n3.setNumber( "3" );
        n3.setSubTypes( QContactPhoneNumber::SubTypeDtmfMenu );
        c.saveDetail( &n3 );

        saveAndVerifyContact( c );
    }
}

void tst_details::testUrl()
{
    QContact c;

    QContactUrl u;
    u.setUrl("http://failblog.org");
    u.setSubType(QContactUrl::SubTypeHomePage);
    c.saveDetail( &u );

    saveAndVerifyContact( c );
}

void tst_details::testPresence()
{
    QContact c;
    
    QContactPresence presence;
    presence.setNickname("Tom");
    presence.setTimestamp(QDateTime::currentTime());
    presence.setPresenceState(QContactPresence::PresenceAvailable);
    presence.setPresenceStateText("Available");
    presence.setPresenceStateImageUrl("http://example.com/example.jpg");
    presence.setCustomMessage("MSN");
    c.saveDetail(&presence);
   
    saveAndVerifyContact( c );
}
QTEST_MAIN(tst_details)
#include "tst_details.moc"
