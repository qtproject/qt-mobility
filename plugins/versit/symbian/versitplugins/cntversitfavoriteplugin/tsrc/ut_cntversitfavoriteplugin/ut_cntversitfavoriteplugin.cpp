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

#include "ut_cntversitfavoriteplugin.h"
#include "cntversitfavoriteplugin.h"
#include "cntversitfavoritepluginfactory.h"


void TestCntVersitFavoritePlugin::initTestCase()
{
	mPluginFactory = new CntVersitFavoritePluginFactory;
}

void TestCntVersitFavoritePlugin::cleanupTestCase()
{
    delete mPluginFactory;
}

void TestCntVersitFavoritePlugin::init()
{
}

void TestCntVersitFavoritePlugin::cleanup()
{
}

void TestCntVersitFavoritePlugin::testFactory()
{
    QString name = mPluginFactory->name();
    QVERIFY(name == "com.nokia.symbian.contacts.cntversitfavoriteplugin");
    
    QVersitContactHandler* plugin = mPluginFactory->createHandler();
    QVERIFY(plugin);
    delete plugin;    
}

void TestCntVersitFavoritePlugin::testImportMethods_data()
{
    QTest::addColumn<QString>("propertyString");
    QTest::addColumn<int>("result");
    
    QTest::newRow("ImportTest1") << "X-FAVORITE" << 1;
    QTest::newRow("ImportTest2") << "TEL" << 0;
}

void TestCntVersitFavoritePlugin::testImportMethods()
{
    CntVersitFavoritePlugin* plugin =  static_cast<CntVersitFavoritePlugin*>(mPluginFactory->createHandler());
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
    plugin->documentProcessed(document, &contact);
    QList<QContactDetail> details = contact.details(QContactFavorite::DefinitionName);
    QVERIFY(details.count() == result);
    delete plugin; 
}

void TestCntVersitFavoritePlugin::testExportMethods_data()
{
    QTest::addColumn<QString>("detailDefinitionName");
    QTest::addColumn<bool>("toBeAddedIsEmpty");
        
    QTest::newRow("ExportTest1") << "Favorite" << false;
    QTest::newRow("ExportTest2") << "PhoneNumber" << true;
}

void TestCntVersitFavoritePlugin::testExportMethods()
{
    CntVersitFavoritePlugin* plugin =  static_cast<CntVersitFavoritePlugin*>(mPluginFactory->createHandler());
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
        const QContactFavorite &favorite(static_cast<const QContactFavorite&>(detail));
        QVersitProperty resultProperty(toBeAdded.last());
        QVERIFY(resultProperty.name() == QLatin1String("X-FAVORITE"));
        QVERIFY(resultProperty.value() == QString("0"));
    }    
    plugin->contactProcessed(contact, &document);
    delete plugin; 
}

QTEST_MAIN(TestCntVersitFavoritePlugin)

// End of File
