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
*/
class tst_QContactManagerSymbianSim : public QObject
{
Q_OBJECT

public:
    tst_QContactManagerSymbianSim();
    virtual ~tst_QContactManagerSymbianSim();

public slots:
    void init();
    void cleanup();

private slots:
    /* Test cases that take no data */
    void hasFeature();
    void supportedContactTypes();
    void detailDefinitions();

    void addContact();
    void updateContact();
    void removeContact();

    /* data */
    void addContact_data();

private:
    bool isContactSupported(QContact contact);

    QContactManager* m_cm;
    /* data providers? */
};

tst_QContactManagerSymbianSim::tst_QContactManagerSymbianSim()
{
	m_cm = QContactManager::fromUri("qtcontacts:symbiansim");
}

tst_QContactManagerSymbianSim::~tst_QContactManagerSymbianSim()
{
	delete m_cm;
}

void tst_QContactManagerSymbianSim::init()
{
}

void tst_QContactManagerSymbianSim::cleanup()
{
}

void tst_QContactManagerSymbianSim::hasFeature()
{
    // TODO: Groups may be supported by some SIM cards?
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups));
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Groups, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences));
    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ActionPreferences, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions));
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships));
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes));
    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ArbitraryRelationshipTypes, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering));
    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::RelationshipOrdering, QContactType::TypeGroup));

    // TODO: self contact may be supported? (so called "own number store")
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::SelfContact, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous));
    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::Anonymous, QContactType::TypeGroup));

    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs));
    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs, QContactType::TypeContact));
    QVERIFY(!m_cm->hasFeature(QContactManager::ChangeLogs, QContactType::TypeGroup));
}

void tst_QContactManagerSymbianSim::supportedContactTypes()
{
    QStringList types = m_cm->supportedContactTypes();
    QVERIFY(types.count() > 0);
    // Assuming that contact type of TypeContact is always supported
    QVERIFY(types.contains(QContactType::TypeContact));
}

void tst_QContactManagerSymbianSim::detailDefinitions()
{
    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();
    // TODO: what are the allowed counts?
    QVERIFY(detailDefinitions.count() >= 2);

    // check that at least definitions for name and phone number exist
    QVERIFY(detailDefinitions.contains(QContactName::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactPhoneNumber::DefinitionName));
}

void tst_QContactManagerSymbianSim::addContact_data()
{
    // A string list containing the detail fields in format <detail definition name>:<field name>:<value>
    // For example first name: Name:First:James
    QTest::addColumn<QStringList>("details");

    // TODO: what name field to use for a sim contact name? first name is not very logical choice...
    // Note: With the current implementation the value must not contain a ':' character
    QTest::newRow("first name") << 
        (QStringList() << "Name:First:James");

    QTest::newRow("first name and nick name") <<
        (QStringList() << "Name:First:James" << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("first name and phone number") <<
        (QStringList() << "Name:First:James" << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("first name and email") <<
        (QStringList() << "Name:First:James" << "EmailAddress:EmailAddress:james.hunt@mclaren.com");
}

void tst_QContactManagerSymbianSim::addContact()
{
    QFETCH(QStringList, details);
    QContact contact;

    // Parse details and add them to the contact
    foreach(QString detail, details) {
        // the expected format is <detail definition name>:<field name>:<value>
        QStringList detailParts = detail.split(QChar(':'), QString::KeepEmptyParts, Qt::CaseSensitive);
        QVERIFY(detailParts.count() == 3);
        QContactDetail contactDetail(detailParts[0]);
        contactDetail.setValue(detailParts[1], detailParts[2]);
        contact.saveDetail(&contactDetail);
    }

    if(isContactSupported(contact))
    {
        bool result = m_cm->saveContact(&contact);
        QCOMPARE(result, true);
        QCOMPARE(m_cm->error(), QContactManager::NoError);
    } else {
        QSKIP("Manager does not support all the contact details", SkipSingle);
    }
}

bool tst_QContactManagerSymbianSim::isContactSupported(QContact contact)
{
    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();

    if(!m_cm->supportedContactTypes().contains(contact.type()))
        return false;
        
    foreach(QContactDetail detail, contact.details()) {
        if(detailDefinitions.contains(detail.definitionName())) {
            // TODO: check the "writability" of the detail?
        } else {
            return false;
        }
    }

    return true;
}

void tst_QContactManagerSymbianSim::updateContact()
{
    // TODO
}

void tst_QContactManagerSymbianSim::removeContact()
{
    // TODO
}

QTEST_MAIN(tst_QContactManagerSymbianSim)
#include "tst_qcontactmanagersymbiansim.moc"
