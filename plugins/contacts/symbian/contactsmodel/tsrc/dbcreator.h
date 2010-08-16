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
* Summary:
* A class for creating a CContactDatabase object with random data.
* The contact items take the form of the following example:
* [KUidContactFieldFamilyName]	Fasiokljno	<5-10 chars>
* [KUidContactFieldGivenName]		Liasidwq	<5-10 chars>
* [KUidContactFieldCompanyName]	Xwjweiohj	<5-10 chars>	
* [KUidContactFieldPhoneNumber]	9874234891	<10 digits>
* Usage example:
* #include "dbcreator.h"
* void MyFunctionL()
* // create a database populated with 500 randomly created contact items
* const TInt KNumContacts(500);
* _LIT(KDbName, "C:mydb.cbd");
* CContactDatabase* db  DbCreator::CreateDbL(KDbName, KNumContacts);
* CleanupStack::PushL(db);
* // do something interesting...
* CleanupStack::PopAndDestroy(db);
* CContactDatabase::DeleteDatabaseL(KDbName);
* Written by:
* James Clarke
*
*/




#ifndef __DBCREATOR_H__
#define __DBCREATOR_H__

#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <e32math.h>
#include <e32cmn.h>


class DbCreator
	{
public:
	static CContactDatabase* CreateDbL(const TDesC &aFileName, TInt aNumContacts);
private:
	static TUint RandomNum(TUint aMin, TUint aMax);
	static TChar RandomAlphaChar(TBool aIsUpper);
	static HBufC* RandomNameL();
	static HBufC* RandomPhoneNumL();
	static void PopulateDatabaseL(CContactDatabase& aDb, TInt aNumContacts);
	};



#endif //__DBCREATOR_H__
