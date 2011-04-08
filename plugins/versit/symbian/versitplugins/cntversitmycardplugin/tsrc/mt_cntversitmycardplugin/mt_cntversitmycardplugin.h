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

#include <QObject>

#include <qtcontacts.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QContactAction::State)

class MT_CntVersitMyCardPlugin : public QObject
{
    Q_OBJECT

private slots:
    //Init/cleanup the test class
    void initTestCase();
    void cleanupTestCase();
	
    //Init/cleanup each test method
    void init();
    void cleanup();
	
    void importContact();
    void exportContact();
    void importOwnContact();
    void exportOwnContact();
};
