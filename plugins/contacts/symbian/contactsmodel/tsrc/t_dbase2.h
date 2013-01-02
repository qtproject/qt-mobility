/*
* Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
* Contact: http://www.qt-project.org/legal
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


GLREF_C void FindAsyncL();
GLREF_C void FindL();
GLDEF_C void FindAsyncInTextDefL();
GLREF_C void FindDefectL();
GLREF_C void FindScandinavianLettersL();
GLREF_C TContactItemId AddContactForFindL(TDesC& aName);
//added to address IN049017 where Identitiy fields were not being searched when using KUidContactFieldmatchAll
GLREF_C void FindUsingKUidContactFieldMatchAllL();

GLREF_D RTest test;
GLREF_D CCntTest* CntTest;

const TPtrC KTestName=_L("Test Name No%d");
const TPtrC KTestAddress=_L("Address %d");
const TInt KTotalNumRecords=20;

