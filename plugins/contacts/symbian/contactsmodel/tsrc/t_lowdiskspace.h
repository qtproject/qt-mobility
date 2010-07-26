/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__T_LOWDISKSPACE_H__)
#define __T_LOWDISKSPACE_H__
   
#include <e32base.h>
#include <f32file.h>

class CContactDatabase;


class CLowDiskSpaceTest : public CActive
	{
public:
	static CLowDiskSpaceTest* NewL();
	~CLowDiskSpaceTest();
	void Start();
private:
	CLowDiskSpaceTest();
	void ConstructL();
	void FillDiskL();
	void ClearDiskL();
	void DatabaseTestL();
private: //From CActive
	void RunL();
	void DoCancel();
	void AddContactToDatabaseL();
	void Cleanup();
private:
	enum TTest
		{
		EFillDisk,
		EDatabaseTest,
		EClearDisk
		};
private:
	CContactDatabase* iContactDatabase;
	RFs iFileSession;
	RFile* iFile;
	TInt iCount;
	TInt iManyFiles;
	};

#endif
