/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "mt_cntversitmycardplugin.h"

#include <QtTest/QtTest>
#include <qversitcontactexporter.h>
#include <qversitcontactimporter.h>
#include <qversitwriter.h>
#include <qversitreader.h>
#include <qversitproperty.h>
#include <qversitcontacthandler.h>

const QString MYCARD_DEFINTION_NAME = "MyCard";

#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 1000; \
        QTest::qWait(10); \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)


void MT_CntVersitMyCardPlugin::initTestCase()
{
    int error = qRegisterMetaType<QContactAction::State>();
}

void MT_CntVersitMyCardPlugin::cleanupTestCase()
{
}

void MT_CntVersitMyCardPlugin::init()
{}

void MT_CntVersitMyCardPlugin::cleanup()
{}

void MT_CntVersitMyCardPlugin::importContact()
{
    //import
    QBuffer input;
    input.open(QBuffer::ReadWrite);
    QByteArray inputVCard =
            "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nEND:VCARD\r\n";
    input.write(inputVCard);
    input.seek(0);
    
    QVersitReader reader;
    reader.setDevice(&input);
    reader.startReading();
    reader.waitForFinished();
    // Use the resulting document(s)...
    QVersitContactImporter importer;
    QList<QVersitDocument> versitDocuments = reader.results();
    importer.importDocuments(versitDocuments);
    QList<QContact> contacts = importer.contacts();
    QVERIFY(1 == contacts.count());
    
    // Check if MyCard detail is found
    QList<QContactDetail> details = contacts.first().details(MYCARD_DEFINTION_NAME);
    QVERIFY(details.count()==0);
    
}

void MT_CntVersitMyCardPlugin::importOwnContact()
{
    //import
    QBuffer input;
    input.open(QBuffer::ReadWrite);
    QByteArray inputVCard =
            "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John\r\nX-SELF:1\r\nEND:VCARD\r\n";
    input.write(inputVCard);
    input.seek(0);
    
    QVersitReader reader;
    reader.setDevice(&input);
    reader.startReading();
    reader.waitForFinished();
    // Use the resulting document(s)...
    QVersitContactImporter importer;
    QList<QVersitDocument> versitDocuments = reader.results();
    importer.importDocuments(versitDocuments);
    QList<QContact> contacts = importer.contacts();
    QVERIFY(1 == contacts.count());
    
    QList<QContactDetail> details = contacts.first().details(MYCARD_DEFINTION_NAME);
    // Check if MyCard detail is found
    QVERIFY(details.count()==1);
}


void MT_CntVersitMyCardPlugin::exportContact()
{
    //create contact
    QContact phonecontact;
    QContactName contactName;
    contactName.setFirstName("Jo");
    contactName.setLastName("Black");
    phonecontact.saveDetail(&contactName);
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("+02644424429");
    phonecontact.saveDetail(&number);
    
    //export
    QList<QContact> list;
    list.append(phonecontact);
    QVersitContactExporter exporter;
    QVERIFY(exporter.exportContacts(list, QVersitDocument::VCard21Type));
    QList<QVersitDocument> documents = exporter.documents();
    
    //X-SELF property is not exported if not MyCard
    QVersitDocument document  = documents.first();
    QVersitProperty property;
    property.setName(QLatin1String("X-SELF"));
    property.setValue("0");
    bool propertyFound = document.properties().contains(property);
    QVERIFY(!propertyFound);
}

void MT_CntVersitMyCardPlugin::exportOwnContact()
{
    //create contact
    QContact phonecontact;
    QContactName contactName;
    contactName.setFirstName("Jo");
    contactName.setLastName("Black");
    phonecontact.saveDetail(&contactName);
    QContactPhoneNumber number;
    number.setContexts("Home");
    number.setSubTypes("Mobile");
    number.setNumber("+02644424429");
    phonecontact.saveDetail(&number);
    
    //set MyCard detail
    QContactDetail myCard(MYCARD_DEFINTION_NAME);
    phonecontact.saveDetail(&myCard);
    
    //export
    QList<QContact> list;
    list.append(phonecontact);
    QVersitContactExporter exporter;
    QVERIFY(exporter.exportContacts(list, QVersitDocument::VCard21Type));
    QList<QVersitDocument> documents = exporter.documents();
    
    //X-SELF property is exported if MyCard
    QVersitDocument document  = documents.first();
    QVersitProperty property;
    property.setName(QLatin1String("X-SELF"));
    property.setValue("0");
    bool propertyFound = document.properties().contains(property);
    QVERIFY(propertyFound);
}

QTEST_MAIN(MT_CntVersitMyCardPlugin);
