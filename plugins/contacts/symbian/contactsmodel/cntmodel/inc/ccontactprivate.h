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


#ifndef __CCONTACTPRIVATE_H__
#define __CCONTACTPRIVATE_H__

#include <cntitem.h>


const TInt KNoValueSet = -1; // Used in CContactDatabase - not published.
const TInt KFiveMins = 300000000; // Used in CContactDatabase - not published.


class CPrivConverter;
class CContactConverter;
class CTextFieldMinimal;


/** 
Base class for Contacts Model plug-ins.
*/
NONSHARABLE_CLASS(CContactPlugin) : public CBase
	{
public:
	~CContactPlugin();

protected:
	TUid LoadEComPluginFromInterface(TUid aInterfaceUid);
	CContactPlugin();

private:
	RLibrary iDll;
	};


/**
Class for all template related functionality.
*/
NONSHARABLE_CLASS(CCntTemplateCache) : public CBase
	{
public:
	static CCntTemplateCache* NewL(RCntModel& aCntSvr);
	~CCntTemplateCache();
	inline TContactItemId TemplateId()const {return iTemplateId;}
	void MergeWithTemplateL(CContactItem& aContact, const CContactItemViewDef* aViewDef = NULL);
	void RemoveTemplate(TContactItemId aId);
	CContactItem& SystemTemplateL();

private:
	CContactItem& TemplateL(TContactItemId aId);
	CCntTemplateCache(RCntModel& aCntSvr);
	void DeleteEntry(TInt aIndex);
	CContactItemViewDef& DefaultViewDefL();
	void ResetSystemTemplate();
	
private:
	RPointerArray <CContactItem> iCache;
	RCntModel& iCntSvr;
	CContactItem* iSystemTemplate;
	TContactItemId iTemplateId;
	CContactItemViewDef* iViewDef;
	};


/** 
Contact item to vCard converter plug-in.
*/
NONSHARABLE_CLASS(CPrivConverter) : public CContactPlugin
	{
public:
	static CPrivConverter* NewL(TUid aUid);
	static CPrivConverter* NewL(TUid aUid, TInt64 aContactFieldFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel);
	TVCardVersion GetCurrentVersion() const;
	TInt64 GetPBAPFilter() const;
	TBool GetExportTel()const;
	void LoadEComPluginL(TUid aUid, TInt64 aFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel);
	void LoadEComPluginL(TUid aUid);
	~CPrivConverter();
	inline CContactConverter* Converter() const;
private:
	TBool IsImplementationAvailableL(TUid aUid);
private:
	CContactConverter* iConverter;
	TVCardVersion iVersion;
	TInt64 iPBAPFilter;
	TBool iExportTel;
	};


inline CContactConverter* CPrivConverter::Converter() const { return iConverter;}


NONSHARABLE_CLASS(CSortArray) : public CBase
	{
public:
	CSortArray();
	~CSortArray();
	void AppendL(const TDesC &Text, TContactItemId aId);
	void SortL(CContactDatabase::TSortPref::TOrder aOrder);
	inline TInt Count() const;
	TContactItemId Id(TInt aIndex) const;
	HBufC *Text(TInt aIndex) const;
	void Merge(CSortArray *aDuplicates,TInt aStartPos);

private:
	static TInt DefaultAlgorithmToCompareNames(const CTextFieldMinimal& aName1, const CTextFieldMinimal& aName2);

private:
	RPointerArray<CTextFieldMinimal> iArray;
	};


inline TInt CSortArray::Count() const {return(iArray.Count());}


NONSHARABLE_CLASS(CTextFieldMinimal) : public CBase
	{
public:
	~CTextFieldMinimal();
	inline CTextFieldMinimal() : iId(KNullContactId) {};
	void ConstructL(const TDesC &aText, TContactItemId aId);

public:
	TContactItemId iId;
	HBufC *iText;
	};


NONSHARABLE_CLASS(CCntIdleSorter) : public CBase
	{
public:
	static CCntIdleSorter* NewL(CContactDatabase& aContactDatabase);
	~CCntIdleSorter();

public:
	// Access methods.
	void ResetL();
	void Cancel();
	void StartSortingL(CArrayFix<CContactDatabase::TSortPref>* aSortOrder, TRequestStatus& aStatus);
	void StartSortingL(CArrayFix<CContactDatabase::TSortPref>* aSortOrder, TRequestStatus& aStatus, MContactSortObserver& aObserver);

private:
	// Internal construction methods.
	CCntIdleSorter();
	CCntIdleSorter(CContactDatabase& aContactDatabase);
	void ConstructL();

private:
	// Sort state.
	enum TState {EReadContacts, ESortContacts};

private:
	static TInt SortCallBack(TAny* aThis);
	TInt PeformSortStep();
	TBool ReadContactsL(TInt aNumber = 4);
	void SortListAndSaveL();
	void CompleteRequest(TInt aError = KErrNone);
	void ReportProgressL();

private:
	inline TInt& TotalCount()			{ return iTotalCount; }
	inline TInt TotalCount() const		{ return iTotalCount; }
	inline TInt& ReadSoFar()			{ return iReadSoFar; }
	inline TInt ReadSoFar() const		{ return iReadSoFar; }
	inline TBool& HasObserver()			{ return iHasObserver; }
	inline TBool HasObserver() const	{ return iHasObserver; }

private:
	TBool iHasObserver;
	TInt iTotalCount;
	TInt iReadSoFar;

	MContactSortObserver* iObserver;
	CContactDatabase& iDb;
	CArrayFix<CContactDatabase::TSortPref>* iSortOrder;
	TRequestStatus* iStatus;

	CIdle* iIdle;
	TState iState;
	CContactIdArray* iFinalItems;
	CSortArray* iSortedList;
	TContactItemId iCurrentId;
	TBool iSortStarted;
	};


NONSHARABLE_CLASS (CDataBaseChangeObserver) :  public CBase, public MContactDbObserver
	{
public:
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	static CDataBaseChangeObserver* NewL(MContactDbPrivObserver& aPrivateObserver);
	~CDataBaseChangeObserver(); 

private:
	CDataBaseChangeObserver (MContactDbPrivObserver& aPrivateObserver);
	MContactDbPrivObserver& iPrivateObserver;
	}; 


#endif //__CCONTACTPRIVATE_H__
