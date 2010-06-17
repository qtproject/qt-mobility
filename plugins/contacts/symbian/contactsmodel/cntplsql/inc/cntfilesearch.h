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




/**
 @file
 @internalComponent
 @released
*/


#ifndef __CNTFILESEARCH_H__
#define __CNTFILESEARCH_H__ 

#include <e32base.h>
#include <badesca.h>
#include <f32file.h>


class RFs;
class CDir;


/**
The CCntFileScanner class is used to scan the Contacts Model private directories
for database files.  This has to be done in the server since the users of the
Contacts Model do not have access to the private directory.
*/
NONSHARABLE_CLASS(CCntFileScanner) : public CBase
	{
public:
	static CDesCArray* ListFilesL(RFs& aFs, TDriveUnit* aDriveUnit = NULL);

private:
	CCntFileScanner(RFs& aFs);
	~CCntFileScanner();

	CDesCArray* ScanLD(TDriveUnit* aDriveUnit = NULL);
	void GenerateRootDirsL();
	void AddDirL(TDriveUnit& aDriveUnit);
	void ScanForFilesL(CDesCArray& aArray);
	void ScanDirectoryL(const TDesC& aDirPath, CDesCArray& aArray);
	void AppendFileNamesL(const CDir& aFileSet, const TDesC& aDirPath, CDesCArray& aArray) const;

private:
	RFs& iFs;
	CDesCArray* iDirStack;
	TFileName iPath;
	};


#endif // __CNTFILESEARCH_H__
