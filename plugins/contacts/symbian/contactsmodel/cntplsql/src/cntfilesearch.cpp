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

#include "cntfilesearch.h"

const TInt KNameArrayGranularity = 32;
                         
//Define the patchable data for database cache size here but which is actually used
//in CPplContactsFile.cpp. This is a must-be so that the compiler cannot perform any
//"constant folding" with the value   
EXPORT_C extern const TInt KContactsModelSqliteDbCacheSize = 0;
                         
_LIT(KSqLiteFilePrefix, "SQLite__");
const TInt KPrefixNameLength = 8;   // length of above prefix string

CDesCArray* CCntFileScanner::ListFilesL(RFs& aFs, TDriveUnit* aDriveUnit)
	{
	CCntFileScanner* scanner = new (ELeave) CCntFileScanner(aFs);  // gets pushed in first lines of ScanLD
	// coverity [memory_leak]
	return scanner->ScanLD(aDriveUnit);
	}


CCntFileScanner::CCntFileScanner(RFs& aFs)
	:
	iFs(aFs)
	{
	}


CCntFileScanner::~CCntFileScanner()
	{
	delete iDirStack;
	}


CDesCArray* CCntFileScanner::ScanLD(TDriveUnit* aDriveUnit)
	{
	CleanupStack::PushL(this);

	iFs.PrivatePath(iPath);
	iDirStack = new(ELeave) CDesCArrayFlat(KNameArrayGranularity);

	if (aDriveUnit)
		{
		AddDirL(*aDriveUnit);
		}
	else
		{
		GenerateRootDirsL();
		}

	CDesCArray* results = new(ELeave) CDesCArrayFlat(KNameArrayGranularity);
	CleanupStack::PushL(results);
	ScanForFilesL(*results);
	CleanupStack::Pop(results);
	CleanupStack::PopAndDestroy(this);

	return results;
	}


void CCntFileScanner::AddDirL(TDriveUnit& aDriveUnit)
	{
	ASSERT(iDirStack);
	TBuf<3> root;
	root.Copy(aDriveUnit.Name());
	root.Append(TChar(KPathDelimiter));
	iDirStack->AppendL(root);
	}

	
void CCntFileScanner::GenerateRootDirsL()
	{
	ASSERT(iDirStack);
	// Generate list of root dirs.
	TBuf<3> root;
	TDriveList driveList;
	User::LeaveIfError(iFs.DriveList(driveList));
	TInt len = driveList.Length();
	for (TInt index = 0; index < len; ++index)
		{
		if(driveList[index]) // Check drive exists.
			{
			root.Copy(TDriveUnit(index).Name());
			root.Append(TChar(KPathDelimiter));
			iDirStack->AppendL(root);
			}
		}
	}


void CCntFileScanner::ScanForFilesL(CDesCArray& aResults)
	{
	ASSERT(iDirStack);
	// Traverse the directory stack until all directories have been examined.
	
	TFileName filename;
	while(iDirStack->Count())
		{
		// Pop the dir.
		filename.Copy(iDirStack->MdcaPoint(0));
		iDirStack->Delete(0);
		if(iPath.Length()!=0)
			{
			filename.Delete(2,1);
			filename.Append(iPath);
			}
		else if(filename[filename.Length() - 1] != KPathDelimiter)
			{ // Check it has a trailing '\' to identify it as a dir path.
			filename.Append(TChar(KPathDelimiter));
			}
		TRAPD(err,ScanDirectoryL(filename, aResults));
		if(err==KErrNoMemory)
			{ // Continue to search in each drive except for out of memory.
			User::Leave(err);
			}
		}
	}


/**
Get a list of all files and subdirectories in aDirPath (when the path is not
specified).
*/
void CCntFileScanner::ScanDirectoryL(const TDesC& aDirPath, CDesCArray& aResults)
	{
	CDir* files = NULL;
	CDir* subdirs = NULL;

	User::LeaveIfError(iFs.GetDir(aDirPath, KEntryAttAllowUid, ESortByName, files, subdirs));

	TRAPD(err,AppendFileNamesL(*files, aDirPath, aResults));

	delete files;
	delete subdirs;
	User::LeaveIfError(err);
	}


void CCntFileScanner::AppendFileNamesL(const CDir& aFileSet, const TDesC& aDirPath, CDesCArray& aFileNames) const
	{
	TParse parse;
	TInt len = aFileSet.Count();
	
	for(TInt idx = 0; idx < len ; ++idx)
		{
		const TEntry& entry = aFileSet[idx];
		User::LeaveIfError(parse.SetNoWild(entry.iName, &aDirPath, NULL));
			
		TInt found = parse.NameAndExt().FindF(KSqLiteFilePrefix);
				
		if (found == 0 && iPath.Length() > 0)
			{
			// add drive, remove prefix, add to list of files
			TFileName filename(parse.Drive());
			filename.Append(parse.NameAndExt().Right(parse.NameAndExt().Length() - KPrefixNameLength));	
						
			aFileNames.AppendL(filename);
			}
			
		}
	}
