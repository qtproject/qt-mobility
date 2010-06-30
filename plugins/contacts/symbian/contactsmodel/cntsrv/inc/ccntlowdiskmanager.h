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


#ifndef __CCNTLOWDISKMANAGER_H__
#define __CCNTLOWDISKMANAGER_H__


#include <e32base.h> // for cactive.


// Forward class references.
class RFs;


/**
	This interace is implemented by the disk space event observer.

	@internalComponent
	@released
*/
class MContactLowDiskObserver
	{
public:
	virtual void HandleLowDiskL(TBool aLowDisk) = 0;
	};


/**
	This class watches for disk space events from the File Server and then
	notifies the registered observer.

	@internalComponent
	@released
*/
class CCntLowDiskManager : public CActive
	{
public:
	static CCntLowDiskManager* NewL(MContactLowDiskObserver& aObserver,
		TInt64 aThreshold, TInt aDrive, RFs& aFs);
	~CCntLowDiskManager();

	void Start();
	
	TBool LowDiskL();
	
private:
	CCntLowDiskManager(MContactLowDiskObserver& aObserver, TInt64 aThreshold,
		TInt aDrive, RFs& aFs);
	
	// From CActive.
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	MContactLowDiskObserver& iObserver;
	TInt64 iThreshold;
	TInt iDrive;
	RFs& iFs;
	TBool iWasLowDisk;
	};

	
#endif //__CCCNTLOWDISKMANAGER_H__
