/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedAll
 @released
*/
#ifndef __CNT_BASE_STEP_H__
#define __CNT_BASE_STEP_H__

#include <testexecutestepbase.h>
#include <cntdb.h>
#include <cntitem.h>
#include "cntperfserver.h"
#include "rcntlist.h"

namespace TestConstants
	{
	//strings to use for labels/values
	_LIT(KLongString, "longstartkla sjd324flk jsak  ldjflk asjdfka lsj34fdlkj   salalk  fjkajsdf lska1djfl   ksajflkasldjlongend");
	_LIT(KShortString, "a");
	_LIT(KMediumString, "ajkldjKJIlk30aksdjjfei");
	
	_LIT(KTime,"22000101:235959.999999");// time to use for datetime storage
	static const TContactItemId KContactId = 1;//contact id to use for agent storage
	static const TTime KDateTime(KTime);
	static const TInt KAltUID = 1900;
	static const TInt KInitialUID = 1100;
	}

#define CLEAR(x) { delete x; x = NULL; }

//necessary, so that __FILE__ macro is executed
#define	FILE(file) _S(file)

#define STRINGCOMPARE( compareLeft, compareOperation, compareRight, iteration, subiteration )\
	StringComparisonTestPrint(compareLeft, _S(#compareOperation), compareRight,\
	(compareLeft compareOperation compareRight), iteration, subiteration, __LINE__, FILE(__FILE__))

#define INTCOMPARE( compareLeft, compareOperation, compareRight, iteration, subiteration )\
	IntegerComparisonTestPrint(compareLeft, _S(#compareOperation), compareRight,\
	(compareLeft compareOperation compareRight), iteration, subiteration, __LINE__, FILE(__FILE__))

#define SINGLECOMPARE( compareOperation, iteration, subiteration )\
	IntegerComparisonTestPrint(ETrue, _S(#compareOperation), ETrue,\
	(compareOperation), iteration, subiteration, __LINE__, FILE(__FILE__))

class CCntBaseStep : public CTestStep
	{
public:
	CCntBaseStep(const TInt aContacts):iContacts(aContacts), iParent(NULL)
		{
		}
	CCntBaseStep(const TInt aContacts, CCntPerfServer &aParent):iContacts(aContacts), iParent(&aParent)
		{
		}
	virtual ~CCntBaseStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
protected:
	void AddUIDsL(CContactItemFieldSet &aFields, const TInt32 aFieldType) const;
	TBool CheckUIDsL(const CContactItemFieldSet &aFields, const TInt32 aFieldType) const;
	void SetLabelL(	CContactItemFieldSet &aFields, const TInt aPos,
					const TDesC &aLabel) const;
	void AddNewFieldL(	CContactItemFieldSet &aFields,const TDesC& aLabel,
						const TFieldType aFieldType, const TBool aEmpty=EFalse) const;	
	TBool CheckNewFieldL(	const CContactItemFieldSet &aFields, const TInt aPos, 
							const TFieldType aFieldType, const TBool aEmpty=EFalse) const;
	void SetFieldL(	CContactItemFieldSet &aFields,const TInt aPos, 
					const TDesC &aText) const;
	TBool CompareFieldsL(	const CContactItemFieldSet &aFields, const TInt aPos, 
							const TDesC &aText) const;
	void SetContactFieldsL(CContactItem &aContact) const;
	void SetContactFieldsL(CContactItem &aContact, const TInt aCount) const;
	TBool CheckContactFieldsL(const CContactItem &aContact) const;
	void SetContactLabelsL(CContactItem &aContact) const;
	TBool CheckContactLabelsL(const CContactItem &aContact);
	virtual void InitializeL();
	virtual void Cleanup();
	virtual void PreTestL() =0;
	static TInt CountEmpty(const CContactItemFieldSet &aFields);
	void PrintCurrentContactL(const CContactItem &aContact) const;
	void PrintCurrentContactL(const TContactItemId aCid) const;
	TInt CountHiddenFields(const CContactItemFieldSet &aFields) const;
	void OpenL(	const TContactItemId aCid, const CContactItemViewDef &aView, 
				const TBool aDefault=EFalse);
	void CommitL();
	void ReadL(	const TContactItemId aCid, const CContactItemViewDef &aView, 
				const TBool aDefault=EFalse);
	void ReadMinimalL(	const TContactItemId aCid, const CContactItemViewDef &aView, 
						const CContactItemViewDef &aDefView, const TBool aDefault=EFalse);
	void Close();
	void AddEmptyContactsL(const TInt aNumber);
	void MissngTestPanic() const;
	void WaitForServerToExitL();
	
	TBool StringComparisonTestPrint(	const TDesC &aCompareL,
										const TText *aCompareOperator,  
										const TDesC	&aCompareR, 
										const TBool aResult,
										const TInt 	aIteration,  
										const TInt 	aSubIteration, 
										const TInt 	aLine, 
										const TText *aFile);
									
	TBool IntegerComparisonTestPrint(	const TInt	aCompareL,
										const TText *aCompareOperator,  
										const TInt 	aCompareR, 
										const TBool aResult,
										const TInt 	aIteration, 
										const TInt 	aSubIteration,  
										const TInt 	aLine, 
										const TText *aFile);

private:
	TBool CompareLabels(const CContactItemFieldSet &aFields, const TInt aPos, 
						const TDesC &aLabel);
	void GetExistingUIDsL(RArray<TInt32> &aArray) const;
	CContactItemViewDef *CopyViewL(const CContactItemViewDef &aView);
	void CloseProcessL(const TDesC& aProcessName, const TInt aIteration);

	
protected:
	TInt iContacts;
	CContactDatabase *iContactsDatabase;
	CContactItemViewDef *iViewAll;
	RArray<TInt32> *iExistingUidsArray;
	CContactItem *iContactItem;
	CContactItemFieldSet *iFields;
	RCntList *iIterate;
	CCntPerfServer *iParent;
	CContactTemplate *iTemplate;
	
	};
#endif
