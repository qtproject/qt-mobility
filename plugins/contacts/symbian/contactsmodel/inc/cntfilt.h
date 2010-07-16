/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CNTFILT_H__
#define __CNTFILT_H__

#include <e32base.h>

#if !( defined __SYMBIAN_CNTMODEL_HIDE_DBMS__ )
#include <d32dbms.h>
#endif

#include <cntdb.h>

class CCntFilter : public CBase
/** A contact database filter.

This is used to get a subset of the items in a contact database. 
There are three criteria used when filtering a database:

- the contact item's state

- the date/time the contact item was created/last modified/deleted

- the contact item's type

The state can be one of: deleted, modified or created (since 
a specified date/time). The filter can be set to include contact items of 
only one state, or it can be set to include contact items of all states, using 
the TInclude enumeration. 

The date/time value is used in combination with the contact item's state.

The contact item's type is one of contact card, contact card group, contact 
card template, or own card. Any combination of contact types may be specified 
in the filter, in addition to the contact item's state.

After the filter has been set up, it should be passed to the function 
CContactDatabase::FilterDatabaseL(). 
@publishedAll
@released
*/
	{
	friend class CPackagerTests; //Comparison test
public:	
	/** Flags to identify which contact items should be included in the filter.

	Only one of the following values can be set in the filter at a time. However 
	the client may select to filter contacts in all states using EIncludeAllContacts. */
	enum TInclude 
		{ 
		/** Contact items in all states should be included. */
		EIncludeAllContacts,
		/** Only contact items which have been added since the date/time should be included. */
		EIncludeNewContacts,
		/** Only contact items which have been modified since the date/time should be included. */
		EIncludeModifiedContacts,
		/** Only contact items which have been deleted since the date/time should be included. */
		EIncludeDeletedContacts	
		};
	/** Contact item types.*/
	enum TContactType
		{
		/** All contact item types should be included. */
		EIncludeAllTypes = 0x01,
		/** Contact cards should be included in the filter. */
		EContactCards = 0x02,
		/** Contact card groups should be included in the filter. */
		EContactGroups = 0x04, 
		/** Contact card templates should be included in the filter. */
		EContactTemplates = 0x08,
		/** Own cards should be included in the filter. */
		EContactOwnCard = 0x10 
		};

	IMPORT_C static CCntFilter* NewL();
	IMPORT_C static CCntFilter* NewLC();
	IMPORT_C static CCntFilter* NewL(const CCntFilter* aFilter);
	IMPORT_C static CCntFilter* NewLC(const CCntFilter* aFilter);
	static CCntFilter* NewLC(RReadStream& aStream);
	IMPORT_C ~CCntFilter();
public:
	/** A pointer to an array which stores the filtered list of contact IDs. */
	CContactIdArray* iIds;
//
	IMPORT_C void SetIncludeModifiedContacts(TBool aIncludeModified); 
	IMPORT_C void SetIncludeNewContacts(TBool aIncludeNew); 
	IMPORT_C void SetIncludeDeletedContacts(TBool aIncludeDeleted);
//
	IMPORT_C TBool TestContactFilterType(TUid aTypeUid);
	IMPORT_C void SetContactFilterTypeALL(TBool aFilterAllContacts);
	IMPORT_C void SetContactFilterTypeCard(TBool aFilterCards);
	IMPORT_C void SetContactFilterTypeGroup(TBool aFilterGroups);
	IMPORT_C void SetContactFilterTypeTemplate(TBool aFilterTemplates);
	IMPORT_C void SetContactFilterTypeOwnCard(TBool aFilterOwnCard);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);	
//
	inline TBool ContactFilterTypeALL() const;
	inline TBool ContactFilterTypeCard() const;
	inline TBool ContactFilterTypeGroup() const;
	inline TBool ContactFilterTypeOwnCard() const;
	inline TBool ContactFilterTypeTemplate() const;
//
	inline TBool IncludeModifiedContacts();
	inline TBool IncludeNewContacts();
	inline TBool IncludeDeletedContacts();
//
	inline TTime GetFilterDateTime();
	inline void SetFilterDateTime(TTime aTime);
//
	IMPORT_C void Reset();
//
/**	
    Intended usage: Reserved to preserve future BC  */ 
	IMPORT_C void Reserved1();
/**	
    Intended usage: Reserved to preserve future BC  */
  	IMPORT_C void Reserved2();

private:
	TTime            iSinceDateTime;
	TInclude		 iInclude;
	TInt32			 iContactType;
	TAny*	         iReserved1;
	TAny*			 iReserved2;
	CCntFilter();
	CCntFilter(const CCntFilter* aFilter);
	};


inline TBool CCntFilter::IncludeModifiedContacts()
/** Tests whether the filter includes only contacts modified since the filter's 
date/time.

@return ETrue if the filter only includes modified contacts. EFalse if not. */
	{return iInclude == EIncludeModifiedContacts;};

inline TBool CCntFilter::IncludeNewContacts()
/** Tests whether the filter includes only contacts created since the filter's 
date/time.

@return ETrue if the filter only includes new contacts. EFalse if not. */
	{return iInclude == EIncludeNewContacts;};

inline TBool CCntFilter::IncludeDeletedContacts()
/** Tests whether the filter includes only contacts deleted since the filter's 
date/time.

@return ETrue if the filter only includes deleted contacts. EFalse if not. */
	{return iInclude == EIncludeDeletedContacts;};

inline TTime CCntFilter::GetFilterDateTime()
/** Gets the date and time used by the filter, as set by SetFilterDateTime().

@return The filter's date and time value. */
	{return iSinceDateTime;};

inline void CCntFilter::SetFilterDateTime(TTime aTime)
/** Sets the date and time used by the filter in combination with the TInclude 
value to test contact items against.

@param aTime The new date and time value. */
	{iSinceDateTime = aTime;};

// CONTACT TYPE
inline TBool CCntFilter::ContactFilterTypeALL() const
/** Tests whether all contact item types are included in the filter, as set by 
SetContactFilterTypeALL().

@return ETrue if all contact item types are included in the filter, EFalse 
if not. */
	{return iContactType & EIncludeAllTypes;}

inline TBool CCntFilter::ContactFilterTypeCard() const
/** Tests whether contact cards are included in the filter, as set by SetContactFilterTypeCard().

@return ETrue if contact cards are included in the filter, EFalse if not. */
	{return iContactType & EContactCards;}

inline TBool CCntFilter::ContactFilterTypeGroup() const
/** Tests whether contact card groups are included in the filter, as set by SetContactFilterTypeGroup().

@return ETrue if contact card groups are included in the filter, EFalse if 
not. */
	{return iContactType & EContactGroups;}

inline TBool CCntFilter::ContactFilterTypeOwnCard() const
/** Tests whether own cards are included in the filter, as set by SetContactFilterTypeOwnCard().

@return ETrue if own cards are included in the filter, EFalse if not. */
	{return iContactType & EContactOwnCard;}

inline TBool CCntFilter::ContactFilterTypeTemplate() const
/** Tests whether contact card templates are included in the filter, as set by 
SetContactFilterTypeTemplate().

@return ETrue if contact card templates are included in the filter, EFalse 
if not. */
	{return iContactType & EContactTemplates;}

#endif
