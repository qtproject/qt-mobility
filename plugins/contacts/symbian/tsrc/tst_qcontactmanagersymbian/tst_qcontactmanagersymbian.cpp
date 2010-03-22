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
#include <QObject>

#include <qtcontacts.h>
#include <cntdb.h>
#include <cntitem.h>

QTM_USE_NAMESPACE

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        QTest::qWait(__step); \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

// A macro to help verifications done in signalEmission test case
#define QTRY_COMPARE_SIGNAL_COUNTS() \
    QTRY_COMPARE(spyContactsAdded.count(), contactsAdded); \
    QTRY_COMPARE(spyContactsChanged.count(), contactsChanged); \
    QTRY_COMPARE(spyContactsRemoved.count(), contactsRemoved); \
    QTRY_COMPARE(spyRelationshipsAdded.count(), relationshipsAdded); \
    QTRY_COMPARE(spyRelationshipsRemoved.count(), relationshipsRemoved); \
    QTRY_COMPARE(spyContactsAdded2.count(), contactsAdded); \
    QTRY_COMPARE(spyContactsChanged2.count(), contactsChanged); \
    QTRY_COMPARE(spyContactsRemoved2.count(), contactsRemoved); \
    QTRY_COMPARE(spyRelationshipsAdded2.count(), relationshipsAdded); \
    QTRY_COMPARE(spyRelationshipsRemoved2.count(), relationshipsRemoved);

//TESTED_CLASS=
//TESTED_FILES=

/*!
 * This class if for testing symbian backend QContactManager API
 * implementation. Specifically those symbian specific cases that cannot be
 * tested with the generic auto tests on QtMobility Contacts level
 * (like tst_QContactManager). Generic test cases that are expected to pass on
 * all contact backends should not be here, they should be included as
 * QtMobility auto tests instead.
 */
class tst_QContactManagerSymbian : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerSymbian();
    virtual ~tst_QContactManagerSymbian();

public slots:
    //void initTestCase();
    //void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void signalEmission();
    void filtering();
    void avatarSubTypes();
    void avatarSubTypes_data();
    void avatarPixmap();
    void avatarPixmap_data();
    void avatarPathAndPixmap();
    void displayLabel_data();
    void displayLabel();
    void invalidContactItems();

private:
    QContact createContact(QString type, QString firstName, QString lastName);
    void addContactItemWithInvalidFieldsL(TContactItemId& itemId);

    CContactDatabase* m_contactDatabase;
    QContactManager* m_cm;
    QContactId m_contactId;
};

tst_QContactManagerSymbian::tst_QContactManagerSymbian()
{
    m_cm = QContactManager::fromUri("qtcontacts:symbian");
    TRAPD(err, m_contactDatabase = CContactDatabase::OpenL());
    QVERIFY(err == KErrNone);
}

tst_QContactManagerSymbian::~tst_QContactManagerSymbian()
{
    delete m_contactDatabase;
	delete m_cm;
}

void tst_QContactManagerSymbian::init()
{
    QContact contact;
    QContactName name;
    name.setFirstName("James");
    name.setLastName("Hunt");
    contact.saveDetail(&name);
    QContactPhoneNumber number;
    number.setNumber("+44752222222");
    contact.saveDetail(&number);
    QVERIFY(m_cm->saveContact(&contact));
    m_contactId = contact.id();
}

void tst_QContactManagerSymbian::cleanup()
{
    // Commented out => leave generated contacts into database
    QVERIFY(m_cm->removeContact(m_contactId.localId()));
}

void tst_QContactManagerSymbian::signalEmission()
{
    QScopedPointer<QContactManager> cm2(QContactManager::fromUri("qtcontacts:symbian"));

    // Wait a moment to make sure there are no pending database observer events
    QTest::qWait(500);

    // counters to keep track of the expected signal counts
    int contactsAdded(0);
    int contactsChanged(0);
    int contactsRemoved(0);
    int relationshipsAdded(0);
    int relationshipsRemoved(0);
    
    // Signal spys for verifying signal emissions
    qRegisterMetaType<QContactLocalId>("QContactLocalId");
    qRegisterMetaType<QList<QContactLocalId> >("QList<QContactLocalId>");
    QSignalSpy spyContactsAdded(m_cm, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    QSignalSpy spyContactsChanged(m_cm, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    QSignalSpy spyContactsRemoved(m_cm, SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsAdded(m_cm, SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsRemoved(m_cm, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));
    QSignalSpy spyContactsAdded2(cm2.data(), SIGNAL(contactsAdded(QList<QContactLocalId>)));
    QSignalSpy spyContactsChanged2(cm2.data(), SIGNAL(contactsChanged(QList<QContactLocalId>)));
    QSignalSpy spyContactsRemoved2(cm2.data(), SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsAdded2(cm2.data(), SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsRemoved2(cm2.data(), SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));

    // create a group
    QContact group = createContact(QContactType::TypeGroup, "Hesketh", "");
    QVERIFY(m_cm->saveContact(&group));
    contactsAdded++;
    QTRY_COMPARE_SIGNAL_COUNTS();

    // change the group
    QContactName name = group.detail(QContactName::DefinitionName);
    name.setCustomLabel("McLaren");
    group.saveDetail(&name);
    QVERIFY(m_cm->saveContact(&group));
    contactsChanged++;
    QTRY_COMPARE_SIGNAL_COUNTS();

    // remove the group
    QVERIFY(m_cm->removeContact(group.localId()));
    contactsRemoved++;
    QTRY_COMPARE_SIGNAL_COUNTS();

    // Add two contacts
    QContact contact1 = createContact(QContactType::TypeContact, "James", "Hunt");
    QVERIFY(m_cm->saveContact(&contact1));
    contactsAdded++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QContact contact2 = createContact(QContactType::TypeContact, "Jochen", "Mass");
    QVERIFY(m_cm->saveContact(&contact2));
    contactsAdded++;
    QTRY_COMPARE_SIGNAL_COUNTS();

    // Add group 2
    QContact group2 = createContact(QContactType::TypeGroup, "McLaren", "");
    QVERIFY(m_cm->saveContact(&group2));
    contactsAdded++;
    QTRY_COMPARE_SIGNAL_COUNTS();

    // Add a relationship
    QContactRelationship r;
    r.setFirst(group2.id());
    r.setSecond(contact1.id());
    r.setRelationshipType(QContactRelationship::HasMember);
    QVERIFY(m_cm->saveRelationship(&r));
    relationshipsAdded++;
    QTRY_COMPARE_SIGNAL_COUNTS();

    // Create one more contact manager instance
    QScopedPointer<QContactManager> cm3(QContactManager::fromUri("qtcontacts:symbian"));
    QSignalSpy spyRelationshipsAdded3(cm3.data(), SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsRemoved3(cm3.data(), SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));

    // Add a relationship
    QContactRelationship r2;
    r2.setFirst(group2.id());
    r2.setSecond(contact2.id());
    r2.setRelationshipType(QContactRelationship::HasMember);
    QVERIFY(m_cm->saveRelationship(&r2));
    relationshipsAdded++;
    QTRY_COMPARE_SIGNAL_COUNTS();

    // Remove relationship 1
    QVERIFY(m_cm->removeRelationship(r));
    relationshipsRemoved++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE(spyRelationshipsAdded3.count(), 1);
    QTRY_COMPARE(spyRelationshipsRemoved3.count(), 1);

    // Remove relationship 2
    QVERIFY(m_cm->removeRelationship(r2));
    relationshipsRemoved++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE(spyRelationshipsAdded3.count(), 1);
    QTRY_COMPARE(spyRelationshipsRemoved3.count(), 2);

    // Remove contacts
    QVERIFY(m_cm->removeContact(contact1.localId()));
    contactsRemoved++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QVERIFY(m_cm->removeContact(contact2.localId()));
    contactsRemoved++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QVERIFY(m_cm->removeContact(group2.localId()));
    contactsRemoved++;
    QTRY_COMPARE_SIGNAL_COUNTS();
}

/*
 * Special filtering cases that cannot be covered in QtMobility system level
 * test cases.
 */
void tst_QContactManagerSymbian::filtering()
{
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
    df.setValue("222");

    // Ends with
    df.setMatchFlags(QContactFilter::MatchEndsWith);

    QList<QContactLocalId> ids = m_cm->contactIds(df);
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QCOMPARE(ids.count(), 1);

    df.setValue("333");
    ids = m_cm->contactIds(df);
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QCOMPARE(ids.count(), 0);

    df.setValue("222222");
    ids = m_cm->contactIds(df);
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QCOMPARE(ids.count(), 1);

    df.setValue("2222222");
    ids = m_cm->contactIds(df);
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QCOMPARE(ids.count(), 1);

    // Match phone number
    df.setMatchFlags(QContactFilter::MatchPhoneNumber);

    df.setValue("2222222");
    ids = m_cm->contactIds(df);
    QCOMPARE(m_cm->error(), QContactManager::NoError);
    QCOMPARE(ids.count(), 1);

    df.setValue("2222");
    ids = m_cm->contactIds(df);
    // Should fail with not supported error because symbian's phone number
    // match algorithm cannot be applied with less than 7 digit search strings
    QCOMPARE(m_cm->error(), QContactManager::NotSupportedError);
    QCOMPARE(ids.count(), 0);

    df.setValue("3333");
    ids = m_cm->contactIds(df);
    // Should fail with not supported error because symbian's phone number
    // match algorithm cannot be applied with less than 7 digit search strings
    QCOMPARE(m_cm->error(), QContactManager::NotSupportedError);
    QCOMPARE(ids.count(), 0);
}

void tst_QContactManagerSymbian::avatarSubTypes_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("subType");

    QString emptyString;

    // TODO: file names
    QTest::newRow("Sub type image") << "C:\\Data\\Images\\avatar_sharks_s.jpg" << "Image";
    //QTest::newRow("Sub type video") << "C:\\Data\\Videos\\video.mpg" << "Video";
    //QTest::newRow("Sub type textured mesh") << "C:\\Data\\" << "TexturedMesh";
    QTest::newRow("Sub type audio ringtone") << "C:\\Data\\Sounds\\avatar_sound.aac" << "AudioRingtone";
    QTest::newRow("Sub type video ringtone") << "C:\\Data\\Videos\\avatar_video.3gp" << "VideoRingtone"; // TODO
    QTest::newRow("No sub type") << "C:\\Data\\Images\\avatar_sharks_s.jpg" << emptyString;
}

/*
 * Special avatar cases that cannot be covered in QtMobility system level
 * test cases.
 */
void tst_QContactManagerSymbian::avatarSubTypes()
{
    QFAIL("Refactor test to match new API!");
    /*
    QFETCH(QString, fileName);
    QFETCH(QString, subType);
    QContact testContact = m_cm->contact(m_contactId.localId());

    // Add avatar with sub type
    QContactAvatar avatar;
    avatar.setAvatar(fileName);

    if(!subType.isEmpty()) {
        avatar.setSubType(subType);
    }
    QVERIFY(testContact.saveDetail(&avatar));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get avatar
    testContact = m_cm->contact(m_contactId.localId());
    QCOMPARE(testContact.details(QContactAvatar::DefinitionName).count(), 1);
    QContactAvatar retrievedAvatar = testContact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!retrievedAvatar.isEmpty());
    QCOMPARE(retrievedAvatar.avatar(), fileName);
    if(subType.isEmpty()) {
        // Known issue: If the sub type of a QContactAvatar is left empty, sub type
        // image is used by default. A side effect is that after loading this kind
        // of an avatar, the sub type has been set to sub type image.
        // -> clear sub type to make the following compare pass
        QVERIFY(retrievedAvatar.removeValue(QContactAvatar::FieldSubType));
    }
    QCOMPARE(retrievedAvatar, avatar);

    // Remove avatar
    retrievedAvatar = testContact.detail(QContactAvatar::DefinitionName);
    QVERIFY(testContact.removeDetail(&retrievedAvatar));
    QVERIFY(m_cm->saveContact(&testContact));
    QCOMPARE(testContact.details(QContactAvatar::DefinitionName).count(), 0);
    */
}

void tst_QContactManagerSymbian::avatarPixmap_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("ExtraSmall JPEG") << "C:\\Data\\Images\\avatar_sharks_xs.jpg";
    QTest::newRow("Small JPEG") << "C:\\Data\\Images\\avatar_sharks_s.jpg";
    QTest::newRow("Medium JPEG") << "C:\\Data\\Images\\avatar_sharks_m.jpg";
    QTest::newRow("XXLarge JPEG") << "C:\\Data\\Images\\avatar_sharks_xxl.jpg";
}

void tst_QContactManagerSymbian::avatarPixmap()
{
    QFAIL("Refactor test to match new API!");
    /*
    QFETCH(QString, fileName);

    QContact testContact = m_cm->contact(m_contactId.localId());

    // Set pixmap
    QContactAvatar avatar;
    QPixmap pixmap(fileName);
    QVERIFY(!pixmap.isNull());
    QVERIFY(avatar.setPixmap(pixmap)); 
    QVERIFY(testContact.saveDetail(&avatar));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get pixmap
    testContact = m_cm->contact(m_contactId.localId());
    avatar = testContact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!avatar.isEmpty());
    pixmap = avatar.pixmap();
    QVERIFY(!pixmap.isNull());
    */
}

/*
 * Special avatar cases that cannot be covered in QtMobility system level
 * test cases.
 */
void tst_QContactManagerSymbian::avatarPathAndPixmap()
{
    QFAIL("Refactor test to match new API!");
    /*
    QString fileName("C:\\Data\\Images\\avatar_sharks_s.jpg");
    QContact testContact = m_cm->contact(m_contactId.localId());

    // Set
    QContactAvatar avatar;
    avatar.setAvatar(fileName);
    QVERIFY(avatar.setPixmap(QPixmap(fileName))); 
    QVERIFY(testContact.saveDetail(&avatar));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get pixmap
    testContact = m_cm->contact(m_contactId.localId());
    avatar = testContact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!avatar.isEmpty());
    QCOMPARE(avatar.avatar(), fileName);
    QPixmap pixmap = avatar.pixmap();
    QVERIFY(!pixmap.isNull());
    */
}

void tst_QContactManagerSymbian::displayLabel_data()
{
    // A string list containing the detail fields in format <detail definition name>:<field name>:<value>
    // For example first name: Name:First:James
    QTest::addColumn<QString>("displayLabel");
    // Note: With the current implementation the value must not contain a ':' character
    QTest::addColumn<QStringList>("details");

    QTest::newRow("first name")
        << "James"
        << (QStringList()
            << "Name:FirstName:James");

    QTest::newRow("last name")
        << "Hunt"
        << (QStringList()
            << "Name:LastName:Hunt");

    QTest::newRow("first and last name") // fail
        << "James Hunt"
        << (QStringList()
            << "Name:FirstName:James"
            << "Name:LastName:Hunt");

    QTest::newRow("multi-part first name and last name") // fail
        << "James Simon Wallis Hunt"
        << (QStringList()
            << "Name:FirstName:James Simon Wallis"
            << "Name:LastName:Hunt");

    QTest::newRow("all names")
        << "James Hunt"
        << (QStringList()
            << "Name:FirstName:James"
            << "Name:LastName:Hunt"
            << "Name:MiddleName:Simon Wallis"
            << "Name:Suffix:Suffix"
            << "Name:Prefix:Pre");

    QTest::newRow("first name, organization")
        << "James"
        << (QStringList()
            << "Name:FirstName:James"
            << "Organization:Name:McLaren");

    QTest::newRow("last name, organization")
        << "Hunt"
        << (QStringList()
            << "Name:LastName:Hunt"
            << "Organization:Name:McLaren");

    QTest::newRow("first name, last name, organization")
        << "James Hunt"
        << (QStringList()
            << "Name:FirstName:James"
            << "Name:LastName:Hunt"
            << "Organization:Name:McLaren");

    QTest::newRow("organization")
        << "McLaren"
        << (QStringList()
            << "Organization:Name:McLaren");
}

/*
 * Special display label test cases for testing symbian backend specific
 * display label generation.
 */
void tst_QContactManagerSymbian::displayLabel()
{
    qDebug() << QTest::currentDataTag();
    QFETCH(QString, displayLabel);
    QFETCH(QStringList, details);

    // Parse details and add them to the contact
    QContact contact;
    foreach(const QString& detail, details) {
        // the expected format is <detail definition name>:<field name>:<value>
        QStringList detailParts = detail.split(QChar(':'), QString::KeepEmptyParts, Qt::CaseSensitive);
        QVERIFY(detailParts.count() == 3);
        QContactDetail contactDetail(detailParts[0]);
        // use existing detail if available
        if(!contact.detail(detailParts[0]).isEmpty()) {
            contactDetail = contact.detail(detailParts[0]);
        }
        contactDetail.setValue(detailParts[1], detailParts[2]);
        QVERIFY(contact.saveDetail(&contactDetail));
    }

    // Save contact
    QVERIFY(m_cm->saveContact(&contact));
    QCOMPARE(m_cm->error(), QContactManager::NoError);

    // Check the generated display label
    if(contact.displayLabel() != displayLabel) {
        //qDebug() << Wrong display label;
        QCOMPARE(contact.displayLabel(), displayLabel);
    }

    // Remove contact
    QVERIFY(m_cm->removeContact(contact.localId()));
}

/*
 * Special contact handling test cases that cannot be covered in QtMobility
 * system level test cases.
 */
void tst_QContactManagerSymbian::invalidContactItems()
{
    // 1. Empty contact
    QContact empty;
    QVERIFY(m_cm->saveContact(&empty));
    empty = m_cm->contact(empty.localId());
    QVERIFY(m_cm->error() == QContactManager::NoError);
    QVERIFY(empty.id() != QContactId());
    QVERIFY(m_cm->removeContact(empty.localId()));
    
    // 2. CContactItem with invalid fields 
    TContactItemId itemId(0);
    TRAPD(err, addContactItemWithInvalidFieldsL(itemId));
    QCOMPARE(err, KErrNone);
    QContact invalidContact = m_cm->contact(itemId);
    QVERIFY(invalidContact.id() != QContactId());
    // TODO: check that 3 fields are removed during save?
    QVERIFY(m_cm->saveContact(&invalidContact));
    QVERIFY(m_cm->removeContact(invalidContact.localId()));
}

QContact tst_QContactManagerSymbian::createContact(QString type, QString firstName, QString lastName)
{
    QContact contact;
    contact.setType(type);
    QContactName name;
    if(type == QContactType::TypeContact) {
        name.setFirstName(firstName);
        name.setLastName(lastName);
    } else {
        name.setCustomLabel(firstName);
    }
    contact.saveDetail(&name);
    return contact;
}

void tst_QContactManagerSymbian::addContactItemWithInvalidFieldsL(TContactItemId& itemId)
{
    // Create a contact with invalid item fields to the database
    CContactItem* contactItem = CContactCard::NewLC();
    // Note! No TFieldType defined!
    CContactItemField* field = CContactItemField::NewLC(KStorageTypeContactItemId);
    contactItem->AddFieldL(*field);
    CleanupStack::Pop(field);
    CContactItemField* field2 = CContactItemField::NewLC(KStorageTypeStore);
    contactItem->AddFieldL(*field2);
    CleanupStack::Pop(field2);
    CContactItemField* field3 = CContactItemField::NewLC(KStorageTypeDateTime);
    contactItem->AddFieldL(*field3);
    CleanupStack::Pop(field3);

    itemId = m_contactDatabase->AddNewContactL(*contactItem);
    CleanupStack::PopAndDestroy(contactItem);
}
QTEST_MAIN(tst_QContactManagerSymbian)
#include "tst_qcontactmanagersymbian.moc"
