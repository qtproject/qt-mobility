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




/**
 @file
 @internalComponent
 @released
*/


#ifndef __CLPLCONTACTPROPERTIES_H__
#define __CLPLCONTACTPROPERTIES_H__

#include "pltables.h"

class CPplContactItemManager;
class CPplPreferencesPersistor;

/**
This class derives from the phonebook synchroniser plugin interface, but
contains no implementation.  It is used to avoid the deadlock situation which
occurs when the phonebook synchroniser server, using the Contacts Model, tries
to call itself.

The contacts server analyses the SID of the server process and replaces the
real plugin with this class.  As a result the check is only required in one
place where the plugin reference is requested and the algorithms using the
plugin are unaware of the replacement.
*/
class NONSHARED CDummySynchroniser : public CBase, public MContactSynchroniser
	{
public:
	// From MContactSynchroniser
	TInt ValidateContact(TValidateOperation aOp, TContactItemId aId);
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
	};

/**
The CSystemTemplateManager class is a helper class for managing the System
template.

The System template is the special contact item that contains detailed 
information about every possible field in the contact card.

The System template is used when contact cards are read from the database.

When a new database is created the System template is built from the resource
file.
*/
class NONSHARED CSystemTemplateManager : CBase
	{
public:
	static CSystemTemplateManager* NewL(CPplContactItemManager& aContactItemManager, RFs& aFileSession);
	~CSystemTemplateManager();
	
	const CContactTemplate& TemplateL();
	void DeleteTemplate();
	void RecreateSystemTemplateL();
	CContactTemplate* SystemTemplateFromResourceL(const TDesC& aResouceFileName);
	
	RArray<TUid>& CustomFilterableFieldsL();

private:
	CSystemTemplateManager(CPplContactItemManager& aContactItemManager, RFs& aFileSession);

private:
	CContactTemplate* iSystemTemplate;
	CPplContactItemManager& iContactItemManager;
	RFs& iFileSession;
	RArray<TUid> iCustFiltFields;		   
	};

/**
The CLplContactProperties class:
	
- Provides the interface to the settings stored in the Preferences table.
- It loads and owns the ECOM plugins used in the Persistence Layer, making sure
that only one instance is reused by several users.
- Manages the System template.
*/
class NONSHARED CLplContactProperties : public CBase, public MLplContactProperties
	{
public:
	CLplContactProperties(RFs& aFs);
	~CLplContactProperties();

	void SetContactItemManagerL(CPplContactItemManager& aContactItemManager);
	
	CContactPhoneParser& ContactPhoneParserL();
	MContactSynchroniser& ContactSynchroniserL(TUint aSessionId);
	void DisableSynchroniser(TUint aSessionId);

	void SessionDeleted(TUint aSessionId);

	TInt CardTemplatePrefIdL() const;
	void SetCardTemplatePrefIdL(TInt aCardTemplatePrefId);

	CContactIdArray& CardTemplateIdsL();
	CContactIdArray& GroupIdListL();
    CBufSeg* DetailsListL(const TDesC& aSearchQuery) const;
    CBufSeg* DetailsListPredictiveL(const TDesC& aSearchPattern) const;

	TInt64 MachineIdL() const;
	void SetMachineIdL(TInt64 aMachineId);
	TPtrC UniqueIdL(); 

	const RArray<TUid>& CustomFilterableFieldsL();

	void SetDbViewContactType(TUid aDbViewContactType);
	TUid GetDbViewContactType() const;
	
	// Public for other Persistence Layer classes.
	TBool CheckType(TUid aUid) const;

	// System template methods.
	CContactTemplate* SystemTemplateFromResourceL(const TDesC& aResouceFileName) const;
	void RecreateSystemTemplateL();
	CSystemTemplateManager& SystemTemplateManager() const;
	const CContactTemplate& SystemTemplateL() const;
	
	TContactItemId OwnCardIdL();
	void SetOwnCardIdL(TContactItemId aId, TBool aPersist = ETrue);	

	TContactItemId ICCTemplateIdL(TUid aPhonebookUid, TUint aSessionId);
	TContactItemId PhonebookGroupIdL(TUint aSessionId);
	
	// Preferences methods
	const CArrayFix<CContactDatabase::TSortPref>& SortPrefsL();	
	void SetSortPrefsL(CArrayFix<CContactDatabase::TSortPref>* aPrefs);		
	
private:
	RFs& iFs;
	CPplContactItemManager* iContactItemManager;
	CContactPhoneParser* iPhoneParser;

	CContactSynchroniser* iSyncPlugin;
	CDummySynchroniser*	iSyncDummy; 
	CSystemTemplateManager* iSystemTemplate;
	TUint iDisabledPhonebookSessionId;

	TBuf<40> iUidString;
	TUid iDbViewContactType;
	CContactIdArray* iGroupIdList;
	
	};

#endif //__CLPLCONTACTPROPERTIES_H__
