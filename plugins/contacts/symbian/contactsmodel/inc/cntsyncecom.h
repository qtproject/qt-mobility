// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CNTSYNCECOM_H__
#define __CNTSYNCECOM_H__

#include <cntsync.h>

/** The UID for the default phone book synchroniser ECom plugin interface.
@publishedPartner
*/
const TUid KUidEcomCntPhBkSyncInterface = {0x102035FD};
	
class CContactSynchroniser : public CBase, public MContactSynchroniser
/** 
Abstract interface for a ECom contacts phonebook synchroniser plugin

It should be inherited by classes which implement the phonebook 
synchroniser API in a platform secured environment. The ECom plug-in interface uid is 
KUidEcomCntPhBkSyncInterface
@publishedPartner
*/
	{
public:
	// From MContactSynchroniser
	virtual TInt ValidateContact(TValidateOperation aOp, TContactItemId aId) = 0;
	virtual TInt ValidateWriteContact(CContactICCEntry& aContactItem) = 0;
	virtual TInt DeleteContact(TContactItemId aId) = 0;
	virtual void Release() = 0;
	virtual void UpdatePostWriteL(const CContactICCEntry& aContactItem) = 0;
	virtual TContactItemId ICCTemplateIdL(TUid aPhonebookUid) = 0;
	virtual TContactItemId GroupIdL(TUid aPhonebookUid) = 0;
	virtual void NotifySyncStateChange(TRequestStatus& aStatus, TUid aPhonebookUid) = 0;
	virtual void CancelNotifyRequest(TUid aPhonebookUid) = 0;
	virtual TBool IsSynchronisedL(TUid aPhonebookUid) = 0;
	virtual TInt PhonebookList(RArray<TUid>& aPhonebookList) = 0;
public:
	static CContactSynchroniser* NewL();
	IMPORT_C virtual ~CContactSynchroniser();
private:
	TUid iDtor_ID_Key;
	};

#endif
