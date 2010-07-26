/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CNTVIEWFINDCONFIG_H__
#define __CNTVIEWFINDCONFIG_H__

#include <e32base.h>
#include "ecom/ecom.h"		// for recomsession

class CContactViewFindConfigInterface : public CBase
/** An interface class that enables implementers to configure the way in which 
words are matched in CContactViewBase::ContactsMatchingCriteriaL() and CContactViewBase::ContactsMatchingPrefixL().

By default these two functions use TDesC16::MatchC() to do the matching, but 
where this is not appropriate, for instance in Chinese locales, a plug-in 
that implements this interface should be supplied. The plug-in's UID should 
be passed to CContactViewBase::SetViewFindConfigPlugin(), then the plug-in 
will be loaded when ContactsMatchingCriteriaL() or ContactsMatchingPrefixL() 
is called. Note that the plug-in is only loaded once per view, not each time 
a find is requested. Note also that the plug-in is optional. If no plug-in 
is supplied, TDesC16::MatchC() is used instead. 
@publishedPartner
@released
*/
	{
	public:
	static CContactViewFindConfigInterface* NewL(TUid aImplementationUid);
	inline virtual ~CContactViewFindConfigInterface();
	//pure virtual methods to be implemented by the plugin.
	/** May be used by the implementor of the interface to provide construction-like 
	behaviour.
	
	For example, it might be used to open a connection to a predictive text input 
	database, or might be used for reference counting of shared objects (using 
	TLS to store the data). */
	virtual void OpenL() = 0;
	/** May be used by the implementor of the interface to provide destruction-like 
	behaviour.
	
	For example, it might be used to close a connection to a predictive text input 
	database, or might be used for reference counting of shared objects (using 
	TLS to store the data). */
	virtual void Close() = 0;
	/** Searches for a string with wildcards in a single field in a contact item.
	
	@param aContactsField The contents of the contact item field to search.
	@param aWord The string to search for in aContactsField. Note that it contains 
	a '*' wildcard character at the end and optionally one at the beginning, depending 
	on whether ContactsMatchingCriteriaL() or ContactsMatchingPrefixL() was called.
	@return ETrue if aWord is found in aContactsField. EFalse if not. */
	virtual TBool Match(const TDesC& aContactsField, const TDesC& aWord) = 0;
	/** Tests whether a specified word is valid to be matched by the plug-in's implementation 
	of Match().
	
	For instance, in an implementation for a Chinese locale, this function would 
	return EFalse for non-Chinese words.
	
	Any words that are not valid to be matched by the plug-in will be matched 
	using TDesC16::MatchC() instead.
	
	@param aWord The word to be checked. Note that it contains a '*' wildcard 
	character at the end and optionally one at the beginning, depending on whether 
	ContactsMatchingCriteriaL() or ContactsMatchingPrefixL() was called.
	@return ETrue if the word is valid for matching by the plug-in, EFalse if not. */
	virtual TBool IsWordValidForMatching(const TDesC& aWord) = 0;
	/** An optimisation function that may be used to implement incremental find, i.e. 
	involving only the results of a previous search, rather than the entire view.
	
	This function is not called in v7.0s, but is provided for possible future use.
	
	@param aItemString The string that is being searched.
	@param aSearchText The string to search for.
	@return ETrue if aSearchText is found in aItemString. EFalse if not. */
	virtual TBool MatchRefineL( const TDesC& aItemString, const TDesC &aSearchText) = 0;
	private:
	//The uid is stored here,so that it can be used during destruction of the instance.
	TUid iDtor_ID_Key;
	};

#include <cntviewfindconfig.inl>

#endif
