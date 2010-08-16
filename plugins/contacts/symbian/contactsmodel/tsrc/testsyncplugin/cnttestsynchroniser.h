/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cntsyncchecker.h"
#include <cntsync.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntsyncecom.h"
#endif

class CContactActiveTestSync : public CActive
	{
public:
	CContactActiveTestSync(CContactSyncChecker& aSyncChecker, TUid aPhoneBookUid);
	//static CContactActiveTestSync* NewL(TRequestStatus& aStatus);
	~CContactActiveTestSync();
	void StartL(TRequestStatus& aStatus);
	TUid PhoneBookUid();

private:
	//CContactActiveTestSync(TRequestStatus& aStatus);
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
private:
	//TRequestStatus* iTheStatus;
	CArrayPtrFlat<TRequestStatus> iStatusArray;
	CContactSyncChecker& iSyncChecker;
	TUid iPhoneBookUid;	
	};


class CContactTestSynchroniser : public CContactSynchroniser 
	{
public: 
	static CContactTestSynchroniser* NewL();
	void ConstructL();
	//from MContactSynchroniser
	virtual ~CContactTestSynchroniser();
	TInt ValidateContact(MContactSynchroniser::TValidateOperation aOp, TContactItemId aId);
	TInt ValidateWriteContact(CContactICCEntry& aContactItem);
	TInt DeleteContact(TContactItemId aId);
	void Release();
	void UpdatePostWriteL(const CContactICCEntry& aContactItem);
	TContactItemId ICCTemplateIdL(TUid aPhonebookUid);
	TContactItemId GroupIdL(TUid aPhonebookUid);
	void NotifySyncStateChange(TRequestStatus& aStatus, TUid aPhonebookUid);
	void CancelNotifyRequest(TUid aPhonebookUid);
	TBool IsSynchronisedL(TUid aPhonebookUid);
	TInt PhonebookList(RArray<TUid>& aPhonebookList);

	CContactSyncChecker* iContactSyncChecker;

	RPointerArray<CContactActiveTestSync> iActiveTestSync;
	};

	

