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
    QTRY_COMPARE(spySelfContactIdChanged.count(), selfContactIdChanged); \
    QTRY_COMPARE(spyContactsAdded2.count(), contactsAdded); \
    QTRY_COMPARE(spyContactsChanged2.count(), contactsChanged); \
    QTRY_COMPARE(spyContactsRemoved2.count(), contactsRemoved); \
    QTRY_COMPARE(spyRelationshipsAdded2.count(), relationshipsAdded); \
    QTRY_COMPARE(spyRelationshipsRemoved2.count(), relationshipsRemoved); \
    QTRY_COMPARE(spySelfContactIdChanged2.count(), selfContactIdChanged);

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
    void init();
    void cleanup();

private slots:
    void signalEmission();
    void filtering();
    void avatarImage();
    void avatarImage_data();
    void thumbnail_data();
    void thumbnail();
    void ringTone();
    void displayLabel_data();
    void displayLabel();
    void timestamp();
    void onlineAccount_data();
    void onlineAccount();
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
    // If the following is commented out => the generated contacts are left into
    // the database
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
    int selfContactIdChanged(0);
    
    // Signal spys for verifying signal emissions
    qRegisterMetaType<QContactLocalId>("QContactLocalId");
    qRegisterMetaType<QList<QContactLocalId> >("QList<QContactLocalId>");
    QSignalSpy spyContactsAdded(m_cm, SIGNAL(contactsAdded(QList<QContactLocalId>)));
    QSignalSpy spyContactsChanged(m_cm, SIGNAL(contactsChanged(QList<QContactLocalId>)));
    QSignalSpy spyContactsRemoved(m_cm, SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsAdded(m_cm, SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsRemoved(m_cm, SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));
    QSignalSpy spySelfContactIdChanged(m_cm, SIGNAL(selfContactIdChanged(QContactLocalId, QContactLocalId)));
    QSignalSpy spyContactsAdded2(cm2.data(), SIGNAL(contactsAdded(QList<QContactLocalId>)));
    QSignalSpy spyContactsChanged2(cm2.data(), SIGNAL(contactsChanged(QList<QContactLocalId>)));
    QSignalSpy spyContactsRemoved2(cm2.data(), SIGNAL(contactsRemoved(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsAdded2(cm2.data(), SIGNAL(relationshipsAdded(QList<QContactLocalId>)));
    QSignalSpy spyRelationshipsRemoved2(cm2.data(), SIGNAL(relationshipsRemoved(QList<QContactLocalId>)));
    QSignalSpy spySelfContactIdChanged2(cm2.data(), SIGNAL(selfContactIdChanged(QContactLocalId, QContactLocalId)));

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

    // Self contact
    QContact memyself = createContact(QContactType::TypeContact, "Kimi", "Raikkonen");
    QVERIFY(m_cm->saveContact(&memyself));
    contactsAdded++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QVERIFY(m_cm->setSelfContactId(memyself.localId()));
    selfContactIdChanged++;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QVERIFY(m_cm->removeContact(memyself.localId()));
    contactsRemoved++;
    selfContactIdChanged++;
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

void tst_QContactManagerSymbian::avatarImage_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("ExtraSmall JPEG") << "C:\\Data\\Images\\avatar_sharks_xs.jpg";
    QTest::newRow("Small JPEG") << "C:\\Data\\Images\\avatar_sharks_s.jpg";
    QTest::newRow("Medium JPEG") << "C:\\Data\\Images\\avatar_sharks_m.jpg";
    QTest::newRow("XXLarge JPEG") << "C:\\Data\\Images\\avatar_sharks_xxl.jpg";
}

/*
 * Special avatar cases that cannot be covered in QtMobility system level
 * test cases.
 */
void tst_QContactManagerSymbian::avatarImage()
{
    QFETCH(QString, fileName);

    QContact testContact = m_cm->contact(m_contactId.localId());

    // Set image, the image file may or may not actually exist
    QContactAvatar avatar;
    QUrl url(fileName);
    QVERIFY(url.isValid());
    avatar.setImageUrl(url);
    QVERIFY(testContact.saveDetail(&avatar));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get image
    testContact = m_cm->contact(m_contactId.localId());
    avatar = testContact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!avatar.isEmpty());
    QCOMPARE(url, avatar.imageUrl());
}

void tst_QContactManagerSymbian::thumbnail_data()
{
    QTest::addColumn<QString>("fileName");

    QTest::newRow("ExtraSmall JPEG") << "C:\\Data\\Images\\avatar_sharks_xs.jpg";
    QTest::newRow("Small JPEG") << "C:\\Data\\Images\\avatar_sharks_s.jpg";
    QTest::newRow("Medium JPEG") << "C:\\Data\\Images\\avatar_sharks_m.jpg";
    QTest::newRow("XXLarge JPEG") << "C:\\Data\\Images\\avatar_sharks_xxl.jpg";
}

/*
 * Special thumbnail cases that cannot be covered in QtMobility system level
 * test cases.
 */
void tst_QContactManagerSymbian::thumbnail()
{
    QFETCH(QString, fileName);

    QContact testContact = m_cm->contact(m_contactId.localId());

    // Set
    QContactThumbnail thumb = testContact.detail(QContactThumbnail::DefinitionName);
    QImage image(fileName);
    QVERIFY(!image.isNull());
    thumb.setThumbnail(image);
    QVERIFY(testContact.saveDetail(&thumb));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get pixmap
    testContact = m_cm->contact(m_contactId.localId());
    thumb = testContact.detail(QContactThumbnail::DefinitionName);
    QVERIFY(!thumb.isEmpty());
    QVERIFY(!thumb.thumbnail().isNull());
}

/*
 * Special ringing tone cases that cannot be covered in QtMobility system level
 * test cases.
 */
void tst_QContactManagerSymbian::ringTone()
{
    QContact testContact = m_cm->contact(m_contactId.localId());

    // these files are not actually included to the test data
    QString audio("C:\\Data\\Sounds\\tone.wav");
    QString video("C:\\Data\\Videos\\video.3gp");
    
    // Set audio ringtone
    QContactRingtone tone = testContact.detail(QContactRingtone::DefinitionName);
    QUrl audioRingtone(audio);
    tone.setAudioRingtoneUrl(audioRingtone);
    QVERIFY(testContact.saveDetail(&tone));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get and verify ringtone
    testContact = m_cm->contact(m_contactId.localId());
    tone = testContact.detail(QContactRingtone::DefinitionName);
    QVERIFY(!tone.isEmpty());
    QCOMPARE(tone.audioRingtoneUrl(), audioRingtone);
    QCOMPARE(tone.videoRingtoneUrl(), QUrl());
    QCOMPARE(tone.vibrationRingtoneUrl(), QUrl());

    // Set video ringtone
    QUrl videoRingtone(video);
    tone.setVideoRingtoneUrl(videoRingtone);
    QVERIFY(testContact.saveDetail(&tone));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get and verify ringtone
    testContact = m_cm->contact(m_contactId.localId());
    tone = testContact.detail(QContactRingtone::DefinitionName);
    QVERIFY(!tone.isEmpty());
    QCOMPARE(tone.audioRingtoneUrl(), audioRingtone);
    QCOMPARE(tone.videoRingtoneUrl(), videoRingtone);
    QCOMPARE(tone.vibrationRingtoneUrl(), QUrl());
}

void tst_QContactManagerSymbian::displayLabel_data()
{
    // Expected display label
    QTest::addColumn<QString>("contactType");
    QTest::addColumn<QString>("displayLabel");
    // A string list containing the detail fields in format <detail definition name>:<field name>:<value>
    // For example first name: Name:First:James
    // Note: With the current implementation the value must not contain a ':' character
    QTest::addColumn<QStringList>("details");

    QString typeContact = QContactType::TypeContact;
    QString typeGroup = QContactType::TypeGroup;

    QTest::newRow("first name")
        << typeContact
        << "James"
        << (QStringList()
            << "Name:FirstName:James");

    QTest::newRow("last name")
        << typeContact
        << "Hunt"
        << (QStringList()
            << "Name:LastName:Hunt");

    QTest::newRow("first and last name") // fail
        << typeContact
        << "James Hunt"
        << (QStringList()
            << "Name:FirstName:James"
            << "Name:LastName:Hunt");

    QTest::newRow("multi-part first name and last name") // fail
        << typeContact
        << "James Simon Wallis Hunt"
        << (QStringList()
            << "Name:FirstName:James Simon Wallis"
            << "Name:LastName:Hunt");

    QTest::newRow("all names")
        << typeContact
        << "James Hunt"
        << (QStringList()
            << "Name:FirstName:James"
            << "Name:LastName:Hunt"
            << "Name:MiddleName:Simon Wallis"
            << "Name:Suffix:Suffix"
            << "Name:Prefix:Pre");

    QTest::newRow("first name, organization")
        << typeContact
        << "James"
        << (QStringList()
            << "Name:FirstName:James"
            << "Organization:Name:McLaren");

    QTest::newRow("last name, organization")
        << typeContact
        << "Hunt"
        << (QStringList()
            << "Name:LastName:Hunt"
            << "Organization:Name:McLaren");

    QTest::newRow("first name, last name, organization")
        << typeContact
        << "James Hunt"
        << (QStringList()
            << "Name:FirstName:James"
            << "Name:LastName:Hunt"
            << "Organization:Name:McLaren");

    QTest::newRow("organization")
        << typeContact
        << "McLaren"
        << (QStringList()
            << "Organization:Name:McLaren");

    QTest::newRow("nick name")
        << typeContact
        << ""
        << (QStringList()
            << "Nickname:Nickname:The Shunt");

    QTest::newRow("phone number")
        << typeContact
        << ""
        << (QStringList()
            << "PhoneNumber:PhoneNumber:+44759999999");

    QTest::newRow("no details")
        << typeContact
        << ""
        << QStringList();

    QTest::newRow("group, custom label")
        << typeGroup
        << "McLaren"
        << (QStringList()
            << "Name:CustomLabel:McLaren");

    QTest::newRow("group, no details")
        << typeGroup
        << ""
        << QStringList();
}

/*
 * Special display label test cases for testing symbian backend specific
 * display label generation.
 */
void tst_QContactManagerSymbian::displayLabel()
{
    //qDebug() << QTest::currentDataTag();
    QFETCH(QString, contactType);
    QFETCH(QString, displayLabel);
    QFETCH(QStringList, details);

    QContact contact;
    if (contactType == QContactType::TypeGroup) {
        contact.setType(QContactType::TypeGroup);
    }

    // Parse details and add them to the contact
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

void tst_QContactManagerSymbian::timestamp()
{
    // Save a contact
    QContact contact = createContact(QContactType::TypeContact, "Jame", "Hunt");
    QVERIFY(m_cm->saveContact(&contact));

    // Wait a second to make the contact's timestamp a little older
    QTest::qWait(1001);

    // Modify the contact
    QContactName name = contact.detail(QContactName::DefinitionName);
    name.setFirstName("James");
    contact.saveDetail(&name);
    QVERIFY(m_cm->saveContact(&contact));

    // Verify
    QContactTimestamp timestamp = contact.detail(QContactTimestamp::DefinitionName);
    QDateTime current = QDateTime::currentDateTime();
    // assume one contact save operation takes less than one second
    QVERIFY(timestamp.created().secsTo(current) <= 2);
    QVERIFY(timestamp.created().secsTo(current) >= 1);
    QVERIFY(timestamp.lastModified().secsTo(current) <= 1);
    QVERIFY(timestamp.lastModified().secsTo(current) >= 0);

    // Delete the contact
    QVERIFY(m_cm->removeContact(contact.localId()));
}

void tst_QContactManagerSymbian::onlineAccount_data()
{
    QTest::addColumn<QString>("accountUri");
    QTest::addColumn<QStringList>("subTypes");

    QTest::newRow("SubTypeSip")
        << QString("+44728888888")
        << (QStringList()
            << QContactOnlineAccount::SubTypeSip);
    
    QTest::newRow("SubTypeSipVoip")
        << QString("+44727777777")
        << (QStringList()
            << QContactOnlineAccount::SubTypeSipVoip);

    QTest::newRow("SubTypeVideoShare")
        << QString("+44726666666")
        << (QStringList()
            << QContactOnlineAccount::SubTypeVideoShare);
}

void tst_QContactManagerSymbian::onlineAccount()
{
    QFETCH(QString, accountUri);
    QFETCH(QStringList, subTypes);

    // Save a contact
    QContact contact = createContact(QContactType::TypeContact, "James", "Hunt");
    QVERIFY(m_cm->saveContact(&contact));
    QContactOnlineAccount onlineAccount;
    onlineAccount.setAccountUri(accountUri);
    onlineAccount.setSubTypes(subTypes);
    contact.saveDetail(&onlineAccount);
    QVERIFY(m_cm->saveContact(&contact));

    // verify by reading the saved contact and comparing online account data
    QContact retrievedContact = m_cm->contact(contact.localId());
    QContactOnlineAccount retrievedOnlineAccount = contact.detail(QContactOnlineAccount::DefinitionName);
    QCOMPARE(retrievedOnlineAccount.accountUri(), accountUri);
    QCOMPARE(retrievedOnlineAccount.subTypes(), subTypes);

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
