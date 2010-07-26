/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CNTTESTIMPLDEF_H__
#define __CNTTESTIMPLDEF_H__

#include <e32base.h>

/**
The Contact Test Implementor namespace contains all the contact model and 
persistence layer database specific constants and enums.

*/
namespace nsCntTestImpl
	{
	_LIT(KSourceDBName, "c:\\System\\data\\Contacts.saved");
	_LIT(KUnsecureDBName, "c:\\System\\data\\Contacts.cdb");
	_LIT(KSecureDBLocation, "c:\\private\\10003A73\\CONTACTS.CDB");
	_LIT(KSecureDBName, "c:CONTACTS.CDB");

	_LIT(KCntTestImplErrNullPointer, 		"Pointer is null");


	_LIT(KPl, "Pl");
	_LIT(KCnt,"Cnt");
	
	
	}

#endif
