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

#ifndef MT_CONTACTVERSITLANDLINEPLUGIN_H
#define MT_CONTACTVERSITLANDLINEPLUGIN_H

#include <QtTest/QtTest>
#include <qtcontacts.h>
#include <QVersitDocument>
#include <qversitcontacthandler.h>


QTM_USE_NAMESPACE



 class MT_CntVersitLandlinePlugin: public QObject
 {
     Q_OBJECT
     QContact importVCard(QByteArray inputVCard );
     QVersitDocument exportVCard(QContact contact );   
     QVersitContactHandlerFactory* loadPlugin();
 private slots:
         
     // module tests through QVersit 
     void testImportMobile();  
     void testImportLandline();
     void testImportCombo();  
     void testExportLandline();
     
     // module tests loadin dll directly  
     void testDetailProcessed();
     void testPluginFactory();
     
 };
 
#endif // MT_CONTACTVERSITLANDLINEPLUGIN_H
