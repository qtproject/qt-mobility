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
    QVERIFY(detailDefinitions.contains(QContactDisplayLabel::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactType::DefinitionName));
    QVERIFY(detailDefinitions.contains(QContactSyncTarget::DefinitionName));

    // TODO: QContactGuid?
}

void tst_QContactManagerSymbianSim::addContact_data()
{
    // A string list containing the detail fields in format <detail definition name>:<field name>:<value>
    // For example first name: Name:First:James
    QTest::addColumn<QString>("expectedDisplayLabel");
    QTest::addColumn<QStringList>("details");
    QString unnamedLabel("Unnamed");
    QString es = QString();

    // TODO: what name field to use for a sim contact name?
    // Note: With the current implementation the value must not contain a ':' character
    QTest::newRow("custom label")
        << "James"
        << (QStringList() << "Name:CustomLabel:James");

    QTest::newRow("custom label 2")
        << "James Hunt"
        << (QStringList() << "Name:CustomLabel:James Hunt");

    QTest::newRow("custom label and nick name")
        << "James Hunt"
        << (QStringList() << "Name:CustomLabel:James Hunt" << "Nickname:Nickname:Hunt the Shunt");

    QTest::newRow("phone number")
        << unnamedLabel
        << (QStringList() << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("custom label and phone number")
        << "James Hunt"
        << (QStringList() << "Name:CustomLabel:James Hunt" << "PhoneNumber:PhoneNumber:+44752222222");

    QTest::newRow("custom label and multiple phone numbers")
        << "James Hunt"
        << (QStringList() << "Name:CustomLabel:James Hunt" << "PhoneNumber:PhoneNumber:+44752222222" << "PhoneNumber:PhoneNumber:+44751111111");

    QTest::newRow("custom label and email")
        << "James Hunt"
        << (QStringList() << "Name:CustomLabel:James Hunt" << "EmailAddress:EmailAddress:james.hunt@mclaren.com");

    QTest::newRow("non-supported field")
        << es
        << (QStringList() << "Name:IllegalNameDetailFieldName:James");

    QTest::newRow("non-supported detail")
        << es
        << (QStringList() << "NotSupportedDetailDefinitionName:IllegalFieldName:FieldValue");

    QTest::newRow("empty, non-supported detail")
        << es
        << (QStringList() << "NotSupportedDetailDefinitionName:IllegalFieldName:");
}

void tst_QContactManagerSymbianSim::addContact()
{
    // Parse details and add them to the contact
    QFETCH(QString, expectedDisplayLabel);
    QFETCH(QStringList, details);    

    QContact contact;
    QList<QContactDetail> detailsUnderTest;

    foreach (QString detail, details) {
        // the expected format is <detail definition name>:<field name>:<value>
        QStringList detailParts = detail.split(QChar(':'), QString::KeepEmptyParts, Qt::CaseSensitive);
        QVERIFY(detailParts.count() == 3);

        // Use existing detail if available
        QContactDetail contactDetail = contact.detail(detailParts[0]);
        if(contactDetail.isEmpty()) {
            contactDetail = QContactDetail(detailParts[0]);
        }

        // Set the field value only if not empty (do not add empty fields)  
        if (!detailParts[2].isEmpty()) {
            QVERIFY(contactDetail.setValue(detailParts[1], detailParts[2]));
        }

        detailsUnderTest.append(contactDetail);
        contact.saveDetail(&contactDetail);
    }

    if (isContactSupported(contact))
    {
        // verify contact can be saved
        QVERIFY(m_cm->saveContact(&contact));
        QCOMPARE(m_cm->error(), QContactManager::NoError);

        // verify contact id
        QVERIFY(contact.id() != QContactId());

        // verify display label
        QCOMPARE(contact.displayLabel(), expectedDisplayLabel);

        // verify that all the details were actually saved
        foreach (QContactDetail detail, detailsUnderTest) {
            QContactDetail savedDetail = contact.detail(detail.definitionName());
            QCOMPARE(savedDetail, detail);
        }

        // TODO: verify that no extra details were added
        //?QCOMPARE(contact.details().count(), detailsUnderTest.count() + 2);

        // verify contact removal
        QVERIFY(m_cm->removeContact(contact.localId()));
        QCOMPARE(m_cm->error(), QContactManager::NoError);
    } else {
        // verify that the contact cannot be saved
        QVERIFY(!m_cm->saveContact(&contact));
        // TODO: what is the expected error code? does it depend on the case?

        // verify contact id is untouched 
        QVERIFY(contact.id() == QContactId());
    }
}

bool tst_QContactManagerSymbianSim::isContactSupported(QContact contact)
{
    QMap<QString, QContactDetailDefinition> detailDefinitions = m_cm->detailDefinitions();

    if(!m_cm->supportedContactTypes().contains(contact.type()))
        return false;
        
    foreach(QContactDetail detail, contact.details()) {
        QString definitionName = detail.definitionName();

        // TODO: should we save a contact that has empty, non-supported details?
        // The current implementation is to ignore empty details here which
        // means that the backend should also ignore the empty details, even
        // if the detail in question is not supported.
        if (detail.isEmpty()) {
            continue;
        }

        // check if the detail is supported by the SIM
        if (detailDefinitions.contains(detail.definitionName())) {
            // check the fields of the detail
            foreach (QString fieldKey, detail.variantValues().keys()) {
                QContactDetailDefinition detaiDef = detailDefinitions.value(detail.definitionName());
                if (!detaiDef.fields().contains(fieldKey)) {
                    return false;
                }
            }
            // TODO: uniquenes of the detail? (for example, create a test case with multiple names)
        } else {
            qDebug() << "Detail" << definitionName << "Not supported";
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
