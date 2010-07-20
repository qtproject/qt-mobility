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


#include <f32file.h> // for rfs

#include "ccntlowdiskmanager.h"

#include "ccntlogger.h"

/** Object factory method for CCntLowDiskManager.
*/
CCntLowDiskManager* CCntLowDiskManager::NewL(MContactLowDiskObserver& aObserver,
	TInt64 aThreshold, TInt aDrive, RFs& aFs)
	{
	CCntLowDiskManager* self = new(ELeave) CCntLowDiskManager(aObserver,
		aThreshold, aDrive, aFs);
	return self;
	}


/** Destructor. 
*/
CCntLowDiskManager::~CCntLowDiskManager()
	{
	Cancel();
	}


/** Request notification of disk space events from File Server.  This method
	covers the scenario where we start with low disk (i.e. already below the
	threshold).  We won't get a notification from the File Server session in
	this case (need to cross threshold).  Rather than repeat the code in RunL(),
	manually complete the request here to force a call of RunL().  The actual
	registration for disk space events takes place at the end of RunL().
*/
void CCntLowDiskManager::Start()
	{
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}


TBool CCntLowDiskManager::LowDiskL()
	{
	TVolumeInfo volumeInfo;	
	User::LeaveIfError(iFs.Volume(volumeInfo, iDrive));
	return (volumeInfo.iFree <= iThreshold);
	}


/** First phase default constructor.
*/
CCntLowDiskManager::CCntLowDiskManager(MContactLowDiskObserver& aObserver,
	TInt64 aThreshold, TInt aDrive, RFs& aFs)
	:
	CActive(EPriorityHigh), iObserver(aObserver), iThreshold(aThreshold),
	iDrive(aDrive), iFs(aFs), iWasLowDisk(EFalse)
	{
	CActiveScheduler::Add(this);
	}


/** Receives a disk space event and notifies the observer.
	@see CActive
*/
void CCntLowDiskManager::RunL()
	{
	TInt ret = iStatus.Int();
	if (ret != KErrNone)
		{
		return;
		}

	if (LowDiskL())
		{
		iWasLowDisk = ETrue;
		iObserver.HandleLowDiskL(ETrue);
		}
	else if (iWasLowDisk)
		{
		iWasLowDisk = EFalse;
		iObserver.HandleLowDiskL(EFalse);
		}
		
	// Request notification again.
	iFs.NotifyDiskSpace(iThreshold, iDrive, iStatus);
	SetActive();
	}


/** Recover if RunL() leaves.
	@see CActive
*/
TInt CCntLowDiskManager::RunError(TInt /*aError*/)
	{
	
	DEBUG_PRINT1(__VERBOSE_DEBUG__,_L("[CNTMODEL] CCntLowDiskManager::RunError(): RunL() has left!\n"));

	return (KErrNone);
	}


/**
	@see CActive
*/
void CCntLowDiskManager::DoCancel()
	{
	iFs.NotifyDiskSpaceCancel(iStatus);
	}
