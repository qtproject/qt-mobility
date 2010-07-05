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



#ifndef CNTSYNCPHONEDATA_H
#define CNTSYNCPHONEDATA_H

#include "cntphonecontact.h"

#include <cntdef.h>
#include <cntdb.h>

#include <centralrepository.h>  // crepository.
#include <e32property.h>
#include <phbksync.h>
#include <e32base.h>
#include <e32cmn.h>

class CContactSyncData;

NONSHARABLE_CLASS(CContactSyncPhonebookData) : public CBase
	{
public:
	static CContactSyncPhonebookData* NewLC(TUid aPhonebookUid, CRepository* aRepository);
	static CContactSyncPhonebookData* NewL(TUint32& aCRKey, CRepository* aRepository);

	~CContactSyncPhonebookData();

	void AddPhonebookContactL(const TDesC& aName, const TDesC& aNumber);


	// high level synchronisation control
	TInt SynchronisePhonebook(CContactDatabase& aDb, TInt aCompletionError, TInt aLeaveError);
	void ResetSynchronised(TBool aResetTemplateAndGroupIds);
	

	// low level twiddling
	void SetTemplateId(TInt aPhonebookTemplateId);
	void SetGroupId(TInt aPhonebookGroupId);
	void SetSynchronised(TInt aSynchronised);
	void SetSynchronisedLeaveCode(TInt aLeaveError);
	void CompleteNotificationRequest(TInt aError);
	void CancelNotificationRequestL();

	// metrics
	TInt NotificationPendingCount();
	

	// directly support Phonebook plug-in interfaces
	TBool IsSynchronisedL();
	TInt StoreNotificationRequest(TInt aError);
	const TUid& PhonebookUid();
	TInt TemplateId();
	TInt GroupId();
	TInt ErrorCode();

	TBool SynchroniseRequired();
	void InternaliseFromCRL(TUint32& aCRKey);
	void ExternaliseToCRL(TUint32& aCRKey);
private:
	CContactSyncPhonebookData(TUid aPhonebookUid, CRepository* aRepository);
	void ValidateTemplateIdAndLabelL(CContactDatabase& aDb);
	void CreateTemplateIdL(CContactDatabase &aDb);
	void ValidateGroupIdAndTemplateL(CContactDatabase& aDb);
	void CreateGroupIdL(CContactDatabase &aDb);
	const TDesC& TemplateLabel();
	void SynchroniseContactsL(CContactDatabase &aDb);

private:
	TUid			iPhonebookUid;	// UID of this Phonebook
	TContactItemId 	iPhonebookTemplateId;
	TContactItemId	iPhonebookGroupId;

	// owned data
	RArray<TInt> iStatusArray;
	//RArray<CPhonebookContact> iPhonebookContacts;
	RPointerArray<CPhonebookContact> iPhonebookContacts;
	CContactItem* iIccTemplate;

	// state
	TBool iIsSynchronised;
	TInt iSynchronisedLeaveCode;
	TInt iNotificationPending;
	TInt iErrorCode;

	CRepository* iRepository;
	};

#endif

