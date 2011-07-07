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

#include <QtTest/QtTest>
#include <qversitproperty.h>

#include "ut_cntversitprefplugin.h"
#include "cntversitprefplugin.h"
#include "cntversitprefpluginfactory.h"

void TestCntVersitPrefPlugin::initTestCase()
{
	mPluginFactory = new CntVersitPrefPluginFactory;
}

void TestCntVersitPrefPlugin::cleanupTestCase()
{
    delete mPluginFactory;
}

void TestCntVersitPrefPlugin::init()
{
}

void TestCntVersitPrefPlugin::cleanup()
{
}

void TestCntVersitPrefPlugin::testFactory()
{
    QSet<QString> profilesSet = mPluginFactory->profiles();
    QVERIFY(profilesSet.count() ==  1);
    QVERIFY(profilesSet.contains(QVersitContactHandlerFactory::ProfileSync));
    
    QString name = mPluginFactory->name();
    QVERIFY(name == "com.nokia.symbian.contacts.cntversitprefplugin");
    
    int index = mPluginFactory->index();
    QVERIFY(index == -1);
    
    QVersitContactHandler* plugin = mPluginFactory->createHandler();
    QVERIFY(plugin);
    delete plugin;    
}

void TestCntVersitPrefPlugin::testImportMethods_data()
{
    QTest::addColumn<QString>("propertyString");
    QTest::addColumn<bool>("alreadyProcessed");
    QTest::addColumn<bool>("updatedDetailsListEmpty");
    QTest::addColumn<QString>("detailDefinitionName");
    QTest::addColumn<QString>("actionName");
    QTest::addColumn<int>("result");
    QTest::addColumn<bool>("resultIsPreferred");
    
    QTest::newRow("ImportTest1") << "TEL;PREF" << false << true << "PhoneNumber" << "call" << 0 << false;
    QTest::newRow("ImportTest2") << "TEL;PREF" << false << false << "PhoneNumber" << "call" << 0 << false;    
    QTest::newRow("ImportTest3") << "TEL;PREF" << true << true << "PhoneNumber" << "call" << 0 << false;
    QTest::newRow("ImportTest4") << "TEL;HOME" << true << false << "PhoneNumber" << "call" << 0 << false;
    QTest::newRow("ImportTest5") << "N;PREF" << true << false << "PhoneNumber" << "call" << 0 << false;
    QTest::newRow("ImportTest6") << "TEL;PREF" << true << false << "PhoneNumber" << "url" << 1 << false;
    QTest::newRow("ImportTest7") << "TEL;PREF" << true << false << "EmailAddress" << "call" << 1 << false;
    QTest::newRow("ImportTest8") << "TEL;PREF" << true << false << "PhoneNumber" << "call" << 1 << true;
    QTest::newRow("ImportTest9") << "X-ASSISTANT-TEL;PREF" << true << false << "PhoneNumber" << "call" << 1 << true;
    QTest::newRow("ImportTest10") << "EMAIL;PREF" << true << false << "EmailAddress" << "email" << 1 << true;
    QTest::newRow("ImportTest11") << "X-SIP;PREF" << true << false << "OnlineAccount" << "OnlineAccountActions" << 1 << true;
    QTest::newRow("ImportTest12") << "IMPP;PREF" << true << false << "OnlineAccount" << "OnlineAccountActions" << 1 << true;
    QTest::newRow("ImportTest13") << "X-IMPP;PREF" << true << false << "OnlineAccount" << "OnlineAccountActions" << 1 << true;
    QTest::newRow("ImportTest14") << "X-JABBER;PREF" << true << false << "OnlineAccount" << "OnlineAccountActions" << 1 << true;
    QTest::newRow("ImportTest15") << "URL;PREF" << true << false << "Url" << "url" << 1 << true;
}

void TestCntVersitPrefPlugin::testImportMethods()
{
    CntVersitPrefPlugin* plugin =  static_cast<CntVersitPrefPlugin*>(mPluginFactory->createHandler());
    QVERIFY(plugin);
    
    QFETCH(QString, propertyString);
    QFETCH(bool, alreadyProcessed);
    QFETCH(bool, updatedDetailsListEmpty);
    QFETCH(QString, detailDefinitionName);
    QFETCH(QString, actionName);
    QFETCH(int, result);
    QFETCH(bool, resultIsPreferred);
    
    QVersitDocument document;
    QVersitProperty property;
    QContact contact;
    QList<QContactDetail> updatedDetails;
    QContactDetail detail(detailDefinitionName);
    
    QStringList propertyData = propertyString.split(';');
    property.setName(propertyData.at(0));
    propertyData.removeFirst();
    foreach(QString paramValue, propertyData)
    {
        property.insertParameter(QLatin1String("TYPE"), paramValue);
    }
    
    if (!updatedDetailsListEmpty)
    {
        updatedDetails.append(detail);
        contact.saveDetail(&detail);
    }

    plugin->propertyProcessed(document, property, contact, &alreadyProcessed, &updatedDetails); 
    QVERIFY(plugin->mPrefDetailList.count() == result);
    plugin->documentProcessed(document, &contact);
    QVERIFY(plugin->mPrefDetailList.count() == 0);
    QVERIFY(contact.isPreferredDetail(actionName, detail) == resultIsPreferred);
    delete plugin; 
}

void TestCntVersitPrefPlugin::testExportMethods_data()
{
    QTest::addColumn<QString>("detailDefinitionName");
    QTest::addColumn<bool>("isPreferredDetail");
    QTest::addColumn<QString>("action");
    QTest::addColumn<bool>("toBeAddedIsEmpty");
    QTest::addColumn<QString>("propertyString");
    QTest::addColumn<bool>("prefInResultProperty");
        
    QTest::newRow("ExportTest1") << "PhoneNumber" << true << "call" << false << "TEL;PREF" << true;
    QTest::newRow("ExportTest1") << "PhoneNumber" << false << "call" << false << "TEL;PREF" << true;
    QTest::newRow("ExportTest2") << "PhoneNumber" << true << "call" << false << "TEL;HOME" << true;
    QTest::newRow("ExportTest2") << "PhoneNumber" << false << "call" << false << "TEL;HOME" << false;
    QTest::newRow("ExportTest2") << "PhoneNumber" << true << "email" << false << "TEL;HOME" << false;
    QTest::newRow("ExportTest2") << "PhoneNumber" << true << "call" << true << "TEL;HOME" << false;
    QTest::newRow("ExportTest2") << "PhoneNumber" << true << "call" << false << "EMAIL;HOME" << false;
    QTest::newRow("ExportTest2") << "EmailAddress" << true << "email" << false << "EMAIL;HOME" << true;
    QTest::newRow("ExportTest2") << "OnlineAccount" << true << "OnlineAccountActions" << false << "X-SIP;VOIP" << true;
    QTest::newRow("ExportTest2") << "Url" << true << "url" << false << "URL;HOME" << true;
    
}

void TestCntVersitPrefPlugin::testExportMethods()
{
    CntVersitPrefPlugin* plugin =  static_cast<CntVersitPrefPlugin*>(mPluginFactory->createHandler());
    QVERIFY(plugin);
    
    QFETCH(QString, detailDefinitionName);
    QFETCH(bool, isPreferredDetail);
    QFETCH(QString, action);
    QFETCH(bool, toBeAddedIsEmpty);
    QFETCH(QString, propertyString);
    QFETCH(bool, prefInResultProperty);
    
    QContact contact;
    QContactDetail detail(detailDefinitionName);
    QVersitDocument document;
    QSet<QString> processedFields;
    QVersitProperty property;
    QList<QVersitProperty> toBeRemoved;
    QList<QVersitProperty> toBeAdded;

    if (isPreferredDetail)
    {
        contact.saveDetail(&detail);
        contact.setPreferredDetail(action,detail);
    }
    
    if (!toBeAddedIsEmpty)
    {
        QStringList propertyData = propertyString.split(';');
        property.setName(propertyData.at(0));
        propertyData.removeFirst();
        foreach(QString paramValue, propertyData)
        {
            property.insertParameter(QLatin1String("TYPE"), paramValue);
        }
        toBeAdded.append(property);
     }    

    plugin->detailProcessed(contact, detail, document, &processedFields, &toBeRemoved, &toBeAdded);
    if (prefInResultProperty)
    {
        QVERIFY(!toBeAdded.isEmpty());
        QVersitProperty resultProperty(toBeAdded.last());
        QStringList typeParameters = resultProperty.parameters().values(QLatin1String("TYPE"));
        QVERIFY(typeParameters.contains(QLatin1String("PREF"), Qt::CaseInsensitive));
     }
    
    plugin->contactProcessed(contact, &document);
    
    delete plugin; 
}

QTEST_MAIN(TestCntVersitPrefPlugin)

// End of File
