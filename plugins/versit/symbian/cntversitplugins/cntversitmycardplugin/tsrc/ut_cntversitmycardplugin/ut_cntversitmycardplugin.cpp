/*
* Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include <QtTest/QtTest>
#include <qversitproperty.h>

#include "ut_cntversitmycardplugin.h"
#include "cntversitmycardplugin.h"
#include "cntversitmycardpluginfactory.h"

const QString MYCARD_DEFINTION_NAME = "MyCard";

void TestCntVersitMyCardPlugin::initTestCase()
{
	mPluginFactory = new CntVersitMyCardPluginFactory;
}

void TestCntVersitMyCardPlugin::cleanupTestCase()
{
    delete mPluginFactory;
}

void TestCntVersitMyCardPlugin::init()
{
}

void TestCntVersitMyCardPlugin::cleanup()
{
}

void TestCntVersitMyCardPlugin::testFactory()
{
    QString name = mPluginFactory->name();
    QVERIFY(name == "com.nokia.symbian.contacts.cntversitmycardplugin");
    
    QVersitContactHandler* plugin = mPluginFactory->createHandler();
    QVERIFY(plugin);
    delete plugin;    
}

void TestCntVersitMyCardPlugin::testImportMethods_data()
{
    QTest::addColumn<QString>("propertyString");
    QTest::addColumn<int>("result");
    
    QTest::newRow("ImportTest1") << "X-SELF" << 1;
    QTest::newRow("ImportTest2") << "TEL" << 0;
}

void TestCntVersitMyCardPlugin::testImportMethods()
{
    CntVersitMyCardPlugin* plugin =  static_cast<CntVersitMyCardPlugin*>(mPluginFactory->createHandler());
    QVERIFY(plugin);
    
    QFETCH(QString, propertyString);
    QFETCH(int, result);
    
    QVersitDocument document;
    QVersitProperty property;
    QContact contact;
    QList<QContactDetail> updatedDetails;
   
    QStringList propertyData = propertyString.split(';');
    property.setName(propertyData.at(0));
    propertyData.removeFirst();
    foreach(QString paramValue, propertyData)
    {
        property.insertParameter(QLatin1String("TYPE"), paramValue);
    }
    bool alreadyProcessed = true;
    plugin->propertyProcessed(document, property, contact, &alreadyProcessed, &updatedDetails);
    QVERIFY(plugin->mIsMyCard == result);
    plugin->documentProcessed(document, &contact);
    QList<QContactDetail> details = contact.details(MYCARD_DEFINTION_NAME);
    QVERIFY(details.count() == result);
    QVERIFY(plugin->mIsMyCard == false);
    delete plugin; 
}

void TestCntVersitMyCardPlugin::testExportMethods_data()
{
    QTest::addColumn<QString>("detailDefinitionName");
    QTest::addColumn<bool>("toBeAddedIsEmpty");
        
    QTest::newRow("ExportTest1") << "MyCard" << false;
    QTest::newRow("ExportTest2") << "PhoneNumber" << true;
}

void TestCntVersitMyCardPlugin::testExportMethods()
{
    CntVersitMyCardPlugin* plugin =  static_cast<CntVersitMyCardPlugin*>(mPluginFactory->createHandler());
    QVERIFY(plugin);
    
    QFETCH(QString, detailDefinitionName);
    QFETCH(bool, toBeAddedIsEmpty);
    
    QContact contact;
    QContactDetail detail(detailDefinitionName);
    QVersitDocument document;
    QSet<QString> processedFields;
    QVersitProperty property;
    QList<QVersitProperty> toBeRemoved;
    QList<QVersitProperty> toBeAdded;

    contact.saveDetail(&detail);
    
    plugin->detailProcessed(contact, detail, document, &processedFields, &toBeRemoved, &toBeAdded);
    QVERIFY(toBeAdded.isEmpty() == toBeAddedIsEmpty);
    
    if (!toBeAdded.isEmpty())
    {
        QVersitProperty resultProperty(toBeAdded.last());
        QVERIFY(resultProperty.name() == QLatin1String("X-SELF"));
        QVERIFY(resultProperty.value() == QString("0"));
    }    
    plugin->contactProcessed(contact, &document);
    delete plugin; 
}

QTEST_MAIN(TestCntVersitMyCardPlugin)

// End of File
