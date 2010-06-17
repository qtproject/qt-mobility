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
* A simple find implementation of the interface CFindUtilInterface
*
*/


#include <cntviewfindconfig.h>
#include <implementationproxy.h>

// Static DLL functions

// ____________________________________________________________________________
// 
/**
	@class CFindUtilBase
	Intended usage: This class implements the functionality promised by 
	the CFindUtilInterface defintion class. It does little apart from 
	implementing a simple find,as a demonsration of using ECOM to load a test
	plugin.
 */
class CFindUtilBase : public CContactViewFindConfigInterface
{
// Methods
public:
/**
	@fn				NewL(TAny* aInitParams)
	Intended Usage	: Standardised safe construction which leaves nothing the cleanup stack.
	Error Condition	: Leaves with error code.
	@leave KErrNoMemory.
	@since			7.0
	@param			None
	@return			CFindUtilBase* The class instance.
	@pre 			None
	@post			CFindUtilBase has been constructed,
					and initialised.
 */
	static CFindUtilBase* NewL();

/**
	@fn				~CFindUtilBase()
	Intended Usage	: Default Destructor	
	Error Condition	: None	
	@since			7.0
	@pre 			CFindUtilBase has been constructed
	@post			CFindUtilBase has been completely destroyed.
 */
	virtual ~CFindUtilBase();

public:
	//Methods that implement the pure virtual functions defined as part of the interface.
	virtual void OpenL() ;
	virtual void Close() ;
	virtual TBool Match(const TDesC& aContactsField, const TDesC& aWord);	
	virtual TBool IsWordValidForMatching(const TDesC& aWord);
	virtual TBool MatchRefineL( const TDesC& aItemString, const TDesC &aSearchText);

private:
/**
	@fn				CFindUtilBase()
	Intended Usage	: Default Constructor : usable only by derived classes	
	Error Condition	: None	
	@since			7.0
	@pre 			None
	@post			CFindUtilBase has been constructed
 */
	CFindUtilBase();

};  // End of CFindUtilBase definition

// __________________________________________________________________________
// Implementation

CFindUtilBase* CFindUtilBase::NewL()
// Intended Usage	: Safe construction which leaves nothing upon the cleanup stack	
// Error Condition	: Will leave with an appropriate error code	
// Dependencies	: CBase
// @param			" "
// @return			CFindUtilBase* a pointer to the fully instantiated CFindUtilBase object
// @pre 			None
// @post			The object has been fully instantiated
// Static member
	{
	CFindUtilBase* self=new(ELeave) CFindUtilBase();  // calls c'tor
	return self;
	}

CFindUtilBase::~CFindUtilBase()
// Default virtual d'tor
	{
	}

CFindUtilBase::CFindUtilBase()
// Default c'tor for use by derived and 
// static construction methods only
: CContactViewFindConfigInterface()
	{
	}

void CFindUtilBase::OpenL()
	{
	}

void CFindUtilBase::Close()
	{
	}

TBool CFindUtilBase::Match(const TDesC& aContactsField, const TDesC& aWord)
	{
	TInt srchLen = aWord.Length();
	if (!aContactsField.Length() || (srchLen > aContactsField.Length()))
		{
		return EFalse;
		}
	TPtrC matchStr(aContactsField.Left(srchLen));
	return (matchStr.MatchC(aWord) != KErrNotFound);
	}

TBool CFindUtilBase::IsWordValidForMatching(const TDesC& aWord)
	{
		if(aWord == aWord)
			return ETrue;
		return EFalse;
	}


TBool CFindUtilBase::MatchRefineL( const TDesC& aItemString, const TDesC &aSearchText)
	{
	if(aItemString.MatchC(aSearchText))
		{
		return ETrue;
		}
	return EFalse;
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x101F8012,	CFindUtilBase::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

