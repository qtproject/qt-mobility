/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QObject>
#include <qtcontacts.h>

QTM_USE_NAMESPACE

class CntVersitPrefPluginFactory;
class CntVersitPrefPlugin;

class TestCntVersitPrefPlugin : public QObject
{
    Q_OBJECT

private slots:
    //Init/cleanup the test class
    void initTestCase();
    void cleanupTestCase();
	
    //Init/cleanup each test method
    void init();
    void cleanup();
	
    void testFactory();
    
    void testImportMethods_data();
    void testImportMethods();
    
    void testExportMethods_data();
    void testExportMethods();

private:
    QContactManager *m_manager; 
    CntVersitPrefPluginFactory* mPluginFactory;
    CntVersitPrefPlugin* mVersitPrefPlugin;
};
