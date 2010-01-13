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

#include "qtcontacts.h"
#include "qcontactchangeset.h"

QTM_USE_NAMESPACE

//TESTED_CLASS=
//TESTED_FILES=

/*!
 * This class if for testing symbian backend QContactManager API
 * implementation. Specifically those symbian specific cases that cannot be
 * tested with the generic auto tests on QtMobility Contacts level
 * (like tst_QContactManager).
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
    void avatarSubTypes();
    void avatarSubTypes_data();
    void avatarPixmap();
    void avatarPixmap_data();
    void avatarPathAndPixmap();

private:
    QContactManager* m_cm;
    QContactId m_contactId;
};

tst_QContactManagerSymbian::tst_QContactManagerSymbian()
{
	m_cm = QContactManager::fromUri("qtcontacts:symbian");
}

tst_QContactManagerSymbian::~tst_QContactManagerSymbian()
{
	delete m_cm;
}

void tst_QContactManagerSymbian::init()
{
    QContact contact;
    QContactName name;
    name.setFirst("James");
    name.setLast("Hunt");
    contact.saveDetail(&name);
    QVERIFY(m_cm->saveContact(&contact));
    m_contactId = contact.id();
}

void tst_QContactManagerSymbian::cleanup()
{
    //QVERIFY(m_cm->removeContact(m_contactId.localId()));
}

void tst_QContactManagerSymbian::avatarSubTypes_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("subType");

    QString emptyString;

    // TODO: file names
    QTest::newRow("Sub type image") << "C:\\Data\\Images\\contact.jpg" << "Image";
    //QTest::newRow("Sub type video") << "C:\\Data\\Videos\\video.mpg" << "Video";
    //QTest::newRow("Sub type textured mesh") << "C:\\Data\\" << "TexturedMesh";
    QTest::newRow("Sub type audio ringtone") << "C:\\Data\\Sounds\\Chat message tone.aac" << "AudioRingtone";
    QTest::newRow("Sub type video ringtone") << "C:\\Data\\Videos\\" << "VideoRingtone"; // TODO
    QTest::newRow("No sub type") << "C:\\Data\\Images\\contact.jpg" << emptyString;
}

void tst_QContactManagerSymbian::avatarSubTypes()
{
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
    if(subType.isEmpty()) {
        // Known issue: If the sub type of a QContactAvatar is left empty, sub type
        // image is used by default. A side effect is that after loading this kind
        // of an avatar, the sub type has been set to sub type image.
        // -> clear sub type to make the following compare pass
        retrievedAvatar.setSubType("");
    }
    QCOMPARE(retrievedAvatar, avatar);

    // Remove avatar
    QVERIFY(testContact.removeDetail(&avatar));
    QVERIFY(m_cm->saveContact(&testContact));
    QCOMPARE(testContact.details(QContactAvatar::DefinitionName).count(), 0);
}

void tst_QContactManagerSymbian::avatarPixmap_data()
{
    QTest::addColumn<QString>("fileName");

    // TODO: file names
    QTest::newRow("Small JPEG") << "C:\\Data\\Images\\contact.jpg";
}

void tst_QContactManagerSymbian::avatarPixmap()
{
    QFETCH(QString, fileName);

    QContact testContact = m_cm->contact(m_contactId.localId());

    // Set pixmap
    QContactAvatar avatar;
    // TODO: the file must exist
    QVERIFY(avatar.setPixmap(QPixmap(fileName))); 
    QVERIFY(testContact.saveDetail(&avatar));
    QVERIFY(m_cm->saveContact(&testContact));

    // Get pixmap
    testContact = m_cm->contact(m_contactId.localId());
    avatar = testContact.detail(QContactAvatar::DefinitionName);
    QVERIFY(!avatar.isEmpty());
    QPixmap pixmap = avatar.pixmap();
    QVERIFY(!pixmap.isNull());
}

void tst_QContactManagerSymbian::avatarPathAndPixmap()
{
    QString fileName("C:\\Data\\Images\\contact.jpg");
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
}

QTEST_MAIN(tst_QContactManagerSymbian)
#include "tst_qcontactmanagersymbian.moc"
