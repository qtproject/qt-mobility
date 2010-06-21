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


/**
 @file
 @internalComponent
 @released
*/


#include <cntdb.h>
#include "cntstd.h"
#include "rcntmodel.h"
#include "ccontactprivate.h"
#include "cntviewprivate.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cntdb_internal.h"
#include "cntsyncecom.h"
#endif


const TInt KMaxItemsInTemplateCache = 8; // Maximum number of cached templates.
const TInt KInitialSortArraySize = 20; // Initial size of sort array.


/**
CTextFieldMinimal destructor.
*/
CTextFieldMinimal::~CTextFieldMinimal()
	{
	delete iText;
	}


/**
CTextFieldMinimal second phase constructor.
*/
void CTextFieldMinimal::ConstructL(const TDesC &aText, TContactItemId aId)
	{
	iText=aText.AllocL();
	iId=aId;
	}


/**
CSortArray first phase constructor.
*/
CSortArray::CSortArray()
	:
	iArray(KInitialSortArraySize)
	{
	}


/**
CSortArray destructor.
*/
CSortArray::~CSortArray()
	{
	iArray.ResetAndDestroy();
	iArray.Close();
	}


/**
Create a CTextFieldMinimal object using the given text and Contact ID and append
it to the sort array.

@param aText Text for CTextFieldMinimal object.
@param aId Contact ID for CTextFieldMinimal object.
*/
void CSortArray::AppendL(const TDesC &aText, TContactItemId aId)
	{
	CTextFieldMinimal *tm=new(ELeave) CTextFieldMinimal;
	CleanupStack::PushL(tm);
	tm->ConstructL(aText, aId);
	User::LeaveIfError(iArray.Append(tm));
	CleanupStack::Pop(); // tm
	}


/**
Sort the sort array using the given sort order.

@param aOrder EAsc for ascending order, EDesc for descending order.
*/
void CSortArray::SortL(CContactDatabase::TSortPref::TOrder aOrder)
	{
	// If nothing to sort then return immediately.
	if (iArray.Count()==0)
		{
		return;
		}
		
	// Sort the array in ascending sort order using the default algorithm to
	// compare the elements in the sort array.
	iArray.Sort(TLinearOrder<CTextFieldMinimal>(DefaultAlgorithmToCompareNames));
	
	// If descending sort order then reverse the elements in the sort array.
	if (aOrder==CContactDatabase::TSortPref::EDesc)
		{
		TInt count=Count();
		TInt end=count/2;
		TInt index2=count-1;
		for(TInt index=0;index<end;index++)
			{
			CTextFieldMinimal *tfm=(CTextFieldMinimal *)iArray[index];
			iArray[index]=iArray[index2];
			iArray[index2]=tfm;
			index2--;
			}
		}
	}


/**
Compare the text in the given CTextFieldMinimal objects.

The custom TCollation method is required to ensure that the collated comparison
will not ignore spaces and punctuation (which it does by default).

Mem::CollationMethodByIndex() is an exec call and is hence relatively expensive.
Although it is inefficient to create a collation method each time this method is
called, there is no benefit in storing TCollationMethod as a member of
CSortArray because this method must be static.

@param aName1
@param aName2

@return A positive value if text in aName1 is greater than text in aName2, a
negative value if text in aName1 is less than text in aName2 and zero if the
text in both aName1 and aName2 is the same.
*/
TInt CSortArray::DefaultAlgorithmToCompareNames(const CTextFieldMinimal& aName1, const CTextFieldMinimal& aName2)
	{
	TCollationMethod collateMethod;
	collateMethod = *Mem::CollationMethodByIndex(0);
	collateMethod.iFlags|=TCollationMethod::EIgnoreNone;
	return aName1.iText->CompareC(*aName2.iText,3,&collateMethod);
	}


/**
Get the Contact ID at the given index in the sort array.

@return The Contact ID at the given index.
*/
TContactItemId CSortArray::Id(TInt aIndex) const
	{
	return(((CTextFieldMinimal *)iArray[aIndex])->iId);
	}


/**
Get the text at the given index in the sort array.

@return The text at the given index.
*/
HBufC* CSortArray::Text(TInt aIndex) const
	{
	return(((CTextFieldMinimal *)iArray[aIndex])->iText);
	}


/**
Merge the given sort array with this sort array starting from the given position
in this sort array.

@param aDuplicates The sort array to be merged with this sort array.
@param aStartPos The starting position for the merge in this sort array.
*/
void CSortArray::Merge(CSortArray *aDuplicates,TInt aStartPos)
	{
	TInt numDuplicates=aDuplicates->Count();
	for (TInt loop=0;loop<numDuplicates;loop++)
		{
		const TInt index=aStartPos+loop;
		delete iArray[index];
		iArray[index]=aDuplicates->iArray[0];
		aDuplicates->iArray.Remove(0);
		}
	}


/**
CCntTemplateCache object factory.

@return CCntTemplateCache object.
*/
CCntTemplateCache* CCntTemplateCache::NewL(RCntModel& aCntSvr)
	{
	CCntTemplateCache* templateCache = new(ELeave) CCntTemplateCache(aCntSvr);
	return templateCache;		
	}
	

/**
CCntTemplateCache first phase constructor.

@param aCntSvr Client Contacts Model session object handle.
*/
CCntTemplateCache::CCntTemplateCache(RCntModel& aCntSvr)
	:
	iCache(KMaxItemsInTemplateCache),
	iCntSvr(aCntSvr),
	iTemplateId(KGoldenTemplateId)
	{
	}


/**
CCntTemplateCache destructor.
*/
CCntTemplateCache::~CCntTemplateCache()
	{
	iCache.ResetAndDestroy();
	delete iViewDef;
	delete iSystemTemplate;
	}


/**
Delete an entry from the template cache.

@param aIndex Index of entry in template cache to be deleted.
*/
void CCntTemplateCache::DeleteEntry(TInt aIndex)
	{
	delete iCache [aIndex];
	iCache.Remove(aIndex);
	}


/**
Remove a template from the template cache.

@param aId ID of the template which is to be removed from the template cache.
*/
void CCntTemplateCache::RemoveTemplate(TContactItemId aId)
	{
	for(TInt ii=iCache.Count() - 1; ii >= 0; --ii)
		{
		if (iCache[ii]->Id() == aId)
			{
			DeleteEntry(ii);	
			}
		}
	
	// Are we removing the system template?
	if (iSystemTemplate != NULL && iSystemTemplate->Id() == aId)	
		{
		delete iSystemTemplate;
		iSystemTemplate = NULL;	
		}
	}


/**
Get the system template.  If the system template does not yet exist then
create it (lazy initialisation).

@return CContactItem object reference for the system template.
*/
CContactItem& CCntTemplateCache::SystemTemplateL() 
	{
	if (!iSystemTemplate)
		{
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(	CContactItemViewDef::EMaskFields,
																CContactItemViewDef::EIncludeHiddenFields);
		iSystemTemplate = static_cast<CContactItem*>(iCntSvr.ReadContactL(matchAll, KGoldenTemplateId));
		CleanupStack::PopAndDestroy(matchAll);
		}

	return *iSystemTemplate;
	}


/**
Dispose of the system template.  The next time the system template is accessed
via the SystemTemplateL() method it will be fetched from the Server.
*/
void CCntTemplateCache::ResetSystemTemplate()
	{
	delete iSystemTemplate;
	iSystemTemplate = NULL;
	}


/**
Get a template from the template cache with the given ID.

@param Contact item ID of the template.

@return CContactItem object reference for the template.
*/
CContactItem& CCntTemplateCache::TemplateL(TContactItemId aId)
	{
	if (aId == KGoldenTemplateId)
		return SystemTemplateL();
	
	CContactItem *retVal = NULL;
	TInt maxCount 		 = iCache.Count();
	TInt ii				 = maxCount;
	
	// Try to find the template in the cache.
	while(ii) 
		{
		--ii;	
		if (iCache[ii]->Id() == aId)
			{
			retVal = iCache[ii];
			break; // Stop searching
			}
		}
		
	if (!retVal) // Template does not exist in cache.
		{
		// Read the template from the Server.
		CContactItemViewDef* matchAll = CContactItemViewDef::NewLC(	CContactItemViewDef::EMaskFields,
										CContactItemViewDef::EIncludeHiddenFields);
		retVal = iCntSvr.ReadContactL(matchAll, aId);
		CleanupStack::PopAndDestroy(matchAll);
		if (maxCount == KMaxItemsInTemplateCache)
			{
			// Remove the first item from the cache (cache is maintained in
			// using FIFO behaviour i.e. oldest template is removed when cache
			// size is exceeded.
			DeleteEntry(0);
			}
		iCache.Append(retVal);		
		}
		
	return *retVal;
	}


/**
Get the default view definition for the template cache.  If the default view 
definition does not yet exist then create it (lazy initialisation).

@return CContactItemViewDef object reference.

@leave KErrNoMemory Out of memory.
*/
CContactItemViewDef& CCntTemplateCache::DefaultViewDefL()
	{
	if (!iViewDef)
		{
		iViewDef = CContactItemViewDef::NewL(CContactItemViewDef::EIncludeFields, CContactItemViewDef::EIncludeHiddenFields);
		iViewDef->AddL(KUidContactFieldMatchAll);
		}
	return *iViewDef;	
	}

	
/**
Merge the given Contact Item with the template used to create the Contact Item.
If no view definition is supplied then use the default view definition.

@param aContact Contact Item to be merged with template.
@param aViewDef View definition to use.
*/
void CCntTemplateCache::MergeWithTemplateL(CContactItem& aContact, const CContactItemViewDef* aViewDef)
	{
	if (aContact.TemplateRefId()!= KNullContactId && aContact.Type() != KUidContactCardTemplate)
		{
		CContactItemViewDef* viewDef = NULL;
		if(aViewDef == NULL) // Only create a viewDef if none has been given.
			{
			viewDef = &(DefaultViewDefL());
			}
		else 
			{
			viewDef = const_cast<CContactItemViewDef*>(aViewDef);	
			}
	
		// Restore template fields.
		if (aContact.TemplateRefId() != KNullContactId && aContact.Type() != KUidContactCardTemplate)
			{
			// Get the template ID used to create the Contact Item.
			CContactItem* contactTemplate=&(static_cast<CContactTemplate&>(TemplateL(aContact.TemplateRefId())));
			aContact.RestoreTemplateFieldsL(SystemTemplateL().CardFields(), contactTemplate->CardFields(), const_cast<CContactItemViewDef&> (*viewDef));
			}
		}
	else if (aContact.Type()==KUidContactCardTemplate)
		{
		aContact.AddLabelFieldL();
		}
	}


/**
CPrivConverter object factory.

@param aImplementationUid UID of the converter plugin.

@return CPrivConverter object.
*/
CPrivConverter* CPrivConverter::NewL(TUid aImplementationUid)
	{
	CPrivConverter* self=new(ELeave) CPrivConverter;
	CleanupStack::PushL(self);
	CVersitTlsData::VersitTlsDataL();
	self->LoadEComPluginL(aImplementationUid);
	CleanupStack::Pop(self); 
	return self;
	}
/**
CPrivConverter object factory.
Loads PBAP contact converter plug-in.
@param aImplementationUid UID of the converter plugin.
@param aContactFieldFilter Specifies contact fields to be exported.
@param aCallback Callback to client for providing intra-contact properties.
@param aVersion Denotes the vCard version for contact export.
@return CPrivConverter object.
*/
CPrivConverter* CPrivConverter::NewL(TUid aUid, TInt64 aContactFieldFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel)
	{
	CPrivConverter* self = new(ELeave) CPrivConverter;
	CleanupStack::PushL(self);
	CVersitTlsData::VersitTlsDataL();
	self->LoadEComPluginL(aUid, aContactFieldFilter, aCallback, aVersion, aExportTel);
	CleanupStack::Pop(self); 
	return self;
	}

TInt64 CPrivConverter::GetPBAPFilter() const
	{
	return iPBAPFilter;
	}

TBool CPrivConverter::GetExportTel() const
	{
	return iExportTel;
	}

TVCardVersion CPrivConverter::GetCurrentVersion() const
	{
	return iVersion;
	}

/**
CPrivConverter destructor.
*/
CPrivConverter::~CPrivConverter()
	{
	delete iConverter;
	CVersitTlsData::CloseVersitTlsData();
	}


/**
Load the ECOM plugin with the given UID.

@param aUid UID of the plugin.
@param aContactFieldFilter Specifies contact fields to be exported.
@param aCallback Callback to client for providing intra-contact properties.
@param aVersion Denotes the vCard version for contact export.
*/
void CPrivConverter::LoadEComPluginL(TUid aUid, TInt64 aContactFieldFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel)
	{
	if(IsImplementationAvailableL(aUid))
		{
		iVersion = aVersion;
		iPBAPFilter = aContactFieldFilter;
		iExportTel = aExportTel;
		iConverter = CContactEcomConverter::NewL(aUid, aContactFieldFilter, aCallback, aVersion, aExportTel);
		}
	__ASSERT_DEBUG(ETrue, User::Invariant());
	}

/**
Load the ECOM plugin with the given UID.

@param aUid UID of the plugin.
*/
void CPrivConverter::LoadEComPluginL(TUid aUid)
	{
	if(IsImplementationAvailableL(aUid))
		{
		iVersion = EVCard21;
		iConverter = CContactEcomConverter::NewL(aUid);	
		}
	__ASSERT_DEBUG(ETrue, User::Invariant());
	}

/**
Finds out if implementation for a given plug-in uid is present as part of vCard Interface.

@param aUid UID of the plugin.
@return TBool ETrue if implementation exists.

@leave KErrNoMemory 
@leave KErrNotConnected 
@leave KErrArgument 

*/
TBool CPrivConverter::IsImplementationAvailableL(TUid aUid)
	{
	RImplInfoPtrArray	implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);

	REComSession::ListImplementationsL(KUidEcomCntVCardConverterInterface, implInfoArray);

	// Check that aImplementationUid is an implementation of
	// KUidEcomCnVCardConverterInterface by checking it is included in
	// implInfoArray.
	const TInt count = implInfoArray.Count();
	__ASSERT_ALWAYS( count > 0, User::Leave(KErrNotFound));

	TBool isAvailable = EFalse;

	for(TInt i = 0 ; i < count ; ++i)
		{
		if(	aUid == implInfoArray[i]->ImplementationUid())
			{
			switch(aUid.iUid)
				{
				case KUidVCardConvDefaultImpl:
					isAvailable = ETrue;
				break;
				case KUidPBAPVCardConvImpl:
					isAvailable = ETrue;
				break;
				default:
				break;
				}
			break;
			}
		}
	CleanupStack::PopAndDestroy(&implInfoArray);
	
	return isAvailable;
	}

/**
CContactPlugin constructor.
*/
CContactPlugin::CContactPlugin()
	{
	}

/**
CContactPlugin destructor.
*/
CContactPlugin::~CContactPlugin()
	{
	iDll.Close();
	}


/**
Initialise this TSortPref object from the given stream.

@param aStream Stream to read TSortPref object state from.
*/
EXPORT_C void CContactDatabase::TSortPref::InternalizeL(RReadStream& aStream)
	{
	iFieldType.iUid=aStream.ReadInt32L();
	iOrder=STATIC_CAST(CContactDatabase::TSortPref::TOrder,aStream.ReadInt32L());
	}

/**
Store this TSortPref object in the given stream.

@param aStream Stream to write TSortPref object state to.
*/
EXPORT_C void CContactDatabase::TSortPref::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iFieldType.iUid);
	aStream.WriteInt32L(iOrder);
	}


/**
Determine if the asynchronous find is complete.
	
@return ETrue If asynchronous find is complete, EFalse otherwise.
*/
EXPORT_C  TBool CIdleFinder::IsComplete() const
/** Checks whether the contact database search is complete.
@return ETrue if search is complete, otherwise EFalse.  */
	{
	return(iContactDatabase==NULL);
	}


/**
Get and take ownership of the Contact IDs.

@return Array of Contact IDs.
*/
EXPORT_C  CContactIdArray* CIdleFinder::TakeContactIds()
/** Gets an array of contact item IDs where the match is found.
@return Array of contact item IDs.  */
	{
	iOwnsIds=EFalse;
	return(iIdsFound);
	}


/**
Get the last asynchronous find error.

@return The last asynchronous find error.
*/
EXPORT_C  TInt CIdleFinder::Error() const
/** Signals an error while searching the database.
@return The error code, if any error occurs during the search operation.  */
	{
	return iError;
	}


/**
CIdleFinder constructor.
*/
CIdleFinder::CIdleFinder(CContactDatabase &aContactDatabase, const CContactItemFieldDef *aFieldDef, const CContactTextDef* aTextDef, MIdleFindObserver *aObserver)
	:
	CIdle(CActive::EPriorityIdle),
	iContactDatabase(&aContactDatabase),
	iFieldDef(aFieldDef), 
	iTextDef(aTextDef),
	iObserver(aObserver)
	{
	}


/**
CIdleFinder destructor.
*/
CIdleFinder::~CIdleFinder()
	{
	// Only delete the found Contact IDs if ownership has not been taken with
	// a call to TakeContactIds().
	if (iOwnsIds)
		{
		delete iIdsFound;
		}
	delete iFindWords;
	}


void CIdleFinder::DoCancel()
	{
	// Setting to NULL indicates complete.
	iContactDatabase=NULL;
	}


/**
CIdleFinder object factory.  This form of NewL() should be used when performing
an asynchronous find using text and a field definition.

@return CIdleFinder object.
*/
CIdleFinder* CIdleFinder::NewL(CContactDatabase &aContactDatabase, const TDesC& aText, const CContactItemFieldDef *aFieldDef, MIdleFindObserver *aObserver)
	{
	CIdleFinder *finder=new(ELeave) CIdleFinder(aContactDatabase, aFieldDef,NULL,aObserver);
	CleanupStack::PushL(finder);
	finder->ConstructL(&aText,NULL,NULL);
	CleanupStack::Pop(); // finder
	return(finder);
	}


/**
CIdleFinder object factory.  This form of NewL() should be used when performing
an asynchronous find using "find words"/"find words parser" and a text
definition.

@return CIdleFinder object.
*/
CIdleFinder* CIdleFinder::NewL(CContactDatabase &aContactDatabase, const MDesCArray* aFindWords,const CContactTextDef* aTextDef, MIdleFindObserver *aObserver, const TCallBack &aWordParserCallback)
	{
	CIdleFinder *finder=new(ELeave) CIdleFinder(aContactDatabase,NULL,aTextDef,aObserver);
	CleanupStack::PushL(finder);
	finder->ConstructL(NULL,aFindWords,&aWordParserCallback);
	CleanupStack::Pop(); // finder
	return(finder);
	}


/**
Set the CIdle object going.
*/
void CIdleFinder::Start()
	{
	CIdle::Start(TCallBack(CIdleFinder::CallbackL, this));
	}


TInt CIdleFinder::CallbackL(TAny *aSelf)
	{
	return ((CIdleFinder *)aSelf)->doFindL();
	}


TInt CIdleFinder::RunError(TInt aError)
	{
	iError = aError;
	iContactDatabase = NULL;
	if (iObserver)
		{
		iObserver->IdleFindCallback();
		}
	return KErrNone;
	}


/**
CIdle callback method.  Makes an IPC call into the Server to perform a find
iteration.

On return from the find iteration from the Server if "find words" were
specified when initialising the find then the client supplied "find words
parser" is invoked to search the returned Contact IDs.  The search cannot be
performed on the server because it is not possible to provide the client
supplied callback to the server.

@return ETrue if further find iterations are required otherwise EFalse.
*/
TBool CIdleFinder::doFindL()
	{
	iError = KErrNone;
	TBool moreToGo = EFalse;
	iOwnsIds = ETrue;
	
	// Perform a find iteration in the Server.
	TRAP(iError, moreToGo = iContactDatabase->iCntSvr->FindAsyncL(iIdsFound));

	if (iError != KErrNone)
		{
		CContactIdArray* newIdArray = CContactIdArray::NewLC();
		CleanupStack::Pop(newIdArray);
		delete iIdsFound;
		iIdsFound = newIdArray;		
		}
	
	if(iFindWords != NULL)
		{
		// Construct a new Contacts ID array by calling CheckFindL().  This
		// method then calls ScanForMatchL() which calls the client's function
		// pointer (supplied when the find was initialised) for the "find
		// words parser".
		CContactIdArray* newIdArray = CContactIdArray::NewLC();
		for(TInt i=0;i<iIdsFound->Count();++i)
			{
			if(CheckFindL((*iIdsFound)[i]))
				{
				newIdArray->AddL((*iIdsFound)[i]);
				}
			}
		CleanupStack::Pop(newIdArray);

		// Dispose of the existing found Contacts IDs and replace with the fully
		// filtered Contacts IDs.
		delete iIdsFound;
		iIdsFound = newIdArray;
		}
	
	if(!moreToGo)
		{
		// Set to NULL to indicate complete.
		iContactDatabase=NULL;
		}
	
	if (iObserver)
		{
		iObserver->IdleFindCallback();
		}
	
	return moreToGo;
	}


NONSHARABLE_CLASS(TKeyCmpTextLength) : public TKeyArrayFix
	{
public:
	TKeyCmpTextLength();
protected:
	// From TKey.
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
	};


TKeyCmpTextLength::TKeyCmpTextLength()
	:
	// Doesn't matter using ECmpNormal, it's ignored.
	TKeyArrayFix(0,ECmpNormal,0)
	{
	}


/**
Compare the lengths of the words.
*/
TInt TKeyCmpTextLength::Compare(TInt aLeft,TInt aRight) const
	{
	TDesC* left=(*((TDesC**)At(aLeft)));
	TDesC* right=(*((TDesC**)At(aRight)));
	return right->Length()-left->Length();
	}


/**
Initialise and start the asynchronous find.
*/
void CIdleFinder::ConstructL(const TDesC *aText, const MDesCArray* aFindWords, const TCallBack *aWordParserCallback)
	{
	if(aFindWords != NULL) // Find using a CContactTextDef.
		{
		// Construct "find words" from the client supplied aFindWords.
		iFindWords = new (ELeave) CDesCArrayFlat(5);
		for(TInt loop=0;loop<aFindWords->MdcaCount();loop++)
			{
			// Construct iFindWords which contains all the strings in the search
			// surrounded by a *.
			TPtrC findWord(aFindWords->MdcaPoint(loop));
			TKeyCmpTextLength key;
			HBufC* bufPtr=findWord.AllocLC();
			iFindWords->CArrayFixBase::InsertIsqAllowDuplicatesL(&bufPtr,key);
			CleanupStack::Pop(); // bufPtr
			}

		// This is the client supplied "find words parser" callback method.
		iWordParserCallback = *aWordParserCallback;

		// Initialise the asynchronous find in the Server.
		iContactDatabase->iCntSvr->FindAsyncTextDefInitL(*iFindWords,const_cast<CContactTextDef*>(iTextDef));
		}
	else // Find using text and a CContactItemFieldDef.
		{
		// Initialise the asynchronous find in the Server.
		iContactDatabase->iCntSvr->FindAsyncInitL(*aText,const_cast<CContactItemFieldDef*>(iFieldDef));		
		}

	// Start the asynchronous find.
	CActiveScheduler::Add(this);
	Start();
	}


class TFoundMatch
	{
public:
	inline TFoundMatch(TFieldType aFieldType,TInt aPos) : iFieldType(aFieldType),iPos(aPos) {};
	inline TBool operator==(const TFoundMatch &aMatch) const;

private:
	TFieldType iFieldType;
	TInt iPos;
	};


inline TBool TFoundMatch::operator==(const TFoundMatch &aMatch) const
	{
	return(aMatch.iFieldType==iFieldType && aMatch.iPos==iPos);
	}


/**
List of matches.
*/
class CFoundMatches : public CArrayFixFlat<TFoundMatch>
	{
public:
	inline CFoundMatches() : CArrayFixFlat<TFoundMatch>(8) {};
	inline void SetCurrentMatchField(TFieldType aFieldType) {iFieldType=aFieldType;};
	TBool AddMatchL(TInt aPos);

private:
	TFieldType iFieldType;
	};


TBool CFoundMatches::AddMatchL(TInt aPos)
	{
	TFoundMatch newMatch(iFieldType,aPos);
	for(TInt loop=0;loop<Count();loop++)
		{
		if (At(loop)==newMatch)
			{
			return(EFalse);
			}
		}
	AppendL(newMatch);
	return(ETrue);
	}


TBool CIdleFinder::ScanForMatchL(const TDesC &aFieldText,const TDesC &aFindText, CFoundMatches *aFoundMatches) const
	{
	CDesCArray *wordArray=new(ELeave) CDesCArrayFlat(10);
	CleanupStack::PushL(wordArray);

	SFindInTextDefWordParser parser(&aFieldText,wordArray);
	(*iWordParserCallback.iFunction)(&parser);

	TBool matched=EFalse;

	TInt findLen=aFindText.Length();

	for(TInt loop=0;loop<wordArray->Count();loop++)
		{
		TPtrC fieldWord((*wordArray)[loop]);
		if (findLen<=fieldWord.Length() && fieldWord.Left(findLen).CompareF(aFindText)==0)
			{
			if (aFoundMatches->AddMatchL(loop))
				{
				matched=ETrue;
				break;
				}
			}
		}

	CleanupStack::PopAndDestroy(); // wordArray

	return(matched);
	}


/**
Use the client supplied "find words parser" to check for a match of the "find
words" within the text fields in the given Contact Item.

@param aContactID Contact Item to check for match.
*/
TBool CIdleFinder::CheckFindL(TContactItemId aContactId) const
	{
	// If iTextDef is NULL can't check so return EFalse to indicate no match.
	if (!iTextDef)
		{
		return(EFalse);
		}

	// Use ReadMininalContactLC() to read the Contact Item.  We don't need the
	// template and groups data for the find check.
	CContactItem* item = iContactDatabase->ReadMinimalContactLC(aContactId);
	const CContactItemFieldSet& fieldSet = item->CardFields();

	TBool found=ETrue;
	CFoundMatches *foundWords=new(ELeave) CFoundMatches;
	CleanupStack::PushL(foundWords);

	// Iterate through iFindWords.
	for(TInt searchWord=0;found && searchWord<iFindWords->Count();searchWord++)
		{
		const TPtrC findMe((*iFindWords)[searchWord]);
		TBool foundInTextDef=EFalse;
		
		// Iterate through iTextDef field types then fallback field type.
		for(TInt loop=0;!foundInTextDef && loop<=iTextDef->Count();loop++)
			{
			TFieldType fieldType;

			if (loop==iTextDef->Count())
				{
				fieldType=iTextDef->FallbackField();
				}
			else
				{
				fieldType=(*iTextDef)[loop].iFieldType;
				}

			// If field type is KUidContactFieldNone, skip this field.
			if (fieldType==KUidContactFieldNone)
				{
				continue;
				}

			TBuf<KTextFieldMinimalLength> textFieldMin;

			// While that field type is found in the Contact Item field text...
			TInt pos=0;
	        while (pos!=KErrNotFound)
				{
				// Get the field text for this field type.
				pos=fieldSet.FieldText(fieldType,textFieldMin,pos);				
				
				// Scan for match in this field text.
				foundWords->SetCurrentMatchField(fieldType);
				if (ScanForMatchL(textFieldMin,findMe,foundWords))
					{
					foundInTextDef=ETrue;
					break;
					}
				if (pos!=KErrNotFound)
					{
					pos++;
					}
				}
			}

		// If there wasn't a match then set found to EFalse.
		if (!foundInTextDef)
			{
			found=EFalse;
			}
		}

	CleanupStack::PopAndDestroy(2,item); // + foundWords

	return(found);
	}


/**
Reserved (private) virtual function to allow further extension of the
MContactSynchroniser interface without requiring changes to all derived classes.
This can be used to return a pointer to an additional extension class.
*/
EXPORT_C void MContactSynchroniser::MContactSynchroniser_Reserved_1()
	{
	}


EXPORT_C CContactSynchroniser::~CContactSynchroniser()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}


/**
Panic the current thread with CNTMODEL as the category
*/
GLDEF_C void Panic(TCntModelPanic aPanic)
	{
	User::Panic(_L("CNTMODEL"),aPanic);
	}


#ifdef __VC32__
extern "C" void __E32Dll(void)
	{
	}
#endif 


/**
 Standard Epoc32 DLL entry point.
*/
#ifndef EKA2
GLDEF_C TInt E32Dll(TInt /*aReason*/)
	{
	return KErrNone;
	}
#endif
