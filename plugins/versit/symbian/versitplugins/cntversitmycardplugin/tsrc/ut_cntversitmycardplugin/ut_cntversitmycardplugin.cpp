/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
