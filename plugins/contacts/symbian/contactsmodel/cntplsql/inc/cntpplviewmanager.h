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

#ifndef __CNTPPLVIEWMANAGER_H__
#define __CNTPPLVIEWMANAGER_H__

#include "cntsqlprovider.h"
#include "persistencelayer.h" // for interface definitions.
#include "persistencelayerimpl.h"
#include <sqldb.h>

class CViewContact;
class CCntPplViewSession;
class CCntSqlStatement;

/**
The CCntPplViewManager class provides the data source for the Contacts Model
view framework.  The view item manager provides support for multiple
iterators (used for view population) as well as the random access to the
Contacts database (used for keeping the views synchronised with the changing
database).
*/
class NONSHARED CCntPplViewManager : public CBase, public MLplViewIteratorManager
	{
public:
	static CCntPplViewManager* NewL(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties);
	~CCntPplViewManager();
	
	//Methods defined in MLplViewIteratorManager
	TInt OpenViewL(const CContactTextDef& aTextDef, const TContactViewPreferences aViewPrefs);
	void CloseView(TInt aViewId);
	void ChangeSortOrderL(TInt aViewId, const CContactTextDef& aTextDef);
	void BeginIterateL(TInt aViewId);
	void EndIterateL(TInt aViewId);
	CViewContact* NextItemL(TInt aViewId, TContactViewPreferences aViewPrefs);
	CViewContact* ItemAtL(TContactItemId aContactId, TInt aViewId);
	
	TUid ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,const CContactTextDef& aTextDef);
    void TextFieldL(TInt aCntItemId,TFieldType aFieldType, TDes& aText);

private:
	void ConstructL();
	CCntPplViewManager(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties);
	
	TInt FindViewSessionIndexById(TInt aViewId) const;
	CCntPplViewSession* FindViewSessionByIdL(TInt aViewId) const;
	
private:
	CPplContactsFile& iContactsFile;
	const CLplContactProperties& iContactProperties;
	
	TInt  iNextViewId;
	RPointerArray<CCntPplViewSession> iViewSessions;
	
	CCntSqlStatement* 		iSelectFullFieldsStmt;
	};

class NONSHARED CCntPplViewSession : public CTimer
	{
public:	
	static CCntPplViewSession* NewL(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties, CCntSqlStatement& aSelectAllFields, TInt aViewId, const CContactTextDef& aTextDef, TContactViewPreferences aViewPrefs);
	~CCntPplViewSession();
	
	void ChangeSortOrderL(const CContactTextDef& aTextDef);
	
	void BeginIterateL();
	void EndIterate();
	CViewContact* NextItemL(TContactViewPreferences aViewPrefs);
	CViewContact* ItemAtL(TContactItemId aContactId);
	
	TInt ViewId() const;
	
	static void  TextFieldL(RSqlStatement& aSqlStatement, const CCntSqlStatement& aCntSqlStmt, const CContactTemplate& aSystemTemplate, TFieldType aFieldType, TDes& aText);
	
private:
	void ConstructL(const CContactTextDef& aTextDef);
	CCntPplViewSession(CPplContactsFile& aContactsFile, const CLplContactProperties& aContactProperties, CCntSqlStatement& aSelectAllFields, TInt aViewId, TContactViewPreferences aViewPrefs);
	
	CViewContact* CreateViewItemL(RSqlStatement& aSqlStmt, const CCntSqlStatement& aCntSqlStmt, const TContactViewPreferences& aViewPrefs);
	void  FillViewItemL(CViewContact& aViewContact, RSqlStatement& aSqlStmt, const TContactViewPreferences& aViewPrefs);
	
	static CViewContact* InitialiseViewItemL(RSqlStatement& aSqlStmt, const CCntSqlStatement& aCntSqlStmt, const TContactViewPreferences& aViewPrefs);
	static TBool HasTxtFieldInFastAccessFieldsL(RSqlStatement& aSelectStmt, const CCntSqlStatement& aCntSqlStmt, TDes& aText);
	static TBool SpecificTxtFieldInFastAccessFieldsL(RSqlStatement& aSelectStmt, const CCntSqlStatement& aCntSqlStmt, const TFieldType aFieldType, TDes& aText);
	static TUid  GetContactFieldMatchUid(const CViewContact& aViewContact, const TContactViewPreferences& aViewPreferences);
	static void  AddFieldInViewContactL(CViewContact& aViewContact, TPtrC& aFieldPtrC, const TContactViewPreferences& aViewPreferences);
    static TBool ContactCorrectType(TUid aContactTypeUid, TContactViewPreferences aTypeToInclude);	
    
    void RunL();  //interface for CTimer
   
    CViewContact* doItemAtL(TContactItemId aContactId);
    void CleanupCachedPrepareStatement();
private:
	const TInt 				iViewId;
	const CLplContactProperties& iContactProperties;
	
	CPplContactsFile& 		iContactsFile;
	CCntSqlStatement&       iSqlSmtSelectAllFieldsById;
	
	TContactViewPreferences iViewPrefs;
	
	CContactTextDef*		iTextDef;			
	CCntSqlStatement*		iCntSqlStatement;
	RSqlStatement*			iRSqlStatement;
	RSqlStatement*			iCachedSqlStatement;
	TBool 					iIsFastAccessFieldsOnly;
	};
	
	
#endif //__CNTPPLVIEWMANAGER_H__
