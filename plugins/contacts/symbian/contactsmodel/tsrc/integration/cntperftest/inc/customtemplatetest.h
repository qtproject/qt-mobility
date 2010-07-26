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

#ifndef __CUSTOM_TEMPLATE_TEST_H__
#define __CUSTOM_TEMPLATE_TEST_H__

#include "cntbasestep.h"

class CCustomTemplateTest : public CCntBaseStep
	{
public:
	CCustomTemplateTest();
	virtual ~CCustomTemplateTest();
	virtual TVerdict doTestStepL();
	virtual void PreTestL()
		{
		}
private:
	void EmptyTemplateTestL(const TContactItemId aCid);
	void CreateCustomTemplateL();
	void LoadContactL(const TContactItemId aCid, const TBool aReadMinimal = EFalse);
	void Clean();
	void FullTemplateTestL(const TContactItemId aCid);
	TContactItemId AddTemplatedContactL(const CContactItem &aTemplate);
	TContactItemId AddTemplatedContactL(const TContactItemId aTemplate);
	void OpenL(const TContactItemId aCid);
	void CommitL();
	void ModifiedTemplateTestL(const TContactItemId aCid, const TBool aReadMinimal);
	TBool CompareToTemplateL( 	const TContactItemId aCid, const CContactItemFieldSet &aFields, 
								const TInt aStart=1, const TBool aNogroup=EFalse);
	TInt FindCtype( const CContentType &aCtype, const CContactItemFieldSet &aFields );
	void NewFieldsTemplateTestL(const TContactItemId aCid, const TBool aNewAndOldFields);
	void doDynamicLoadingL(const TContactItemId aCid, const TContactItemId aTemplate, 
							const TInt aCount, const TBool aSystemTemplate = EFalse);
	void TestDynamicLoadingL(const TContactItemId aCid);
	void CustomFieldsTemplateTestL(const TContactItemId aCid, TBool aSetCustomLabels, const TBool aUids);
	CContactItemFieldSet *CopyFieldSetLC(const CContactItemFieldSet &aFields);
	void DeleteCustomTemplateTestL(const TContactItemId aCid);
	void DeleteSystemTemplateTestL(const TContactItemId aCid);
	TBool CheckDeleteItemL(	const TContactItemId aCid, 
							const TInt aFieldCountPreDelete, 
							const TInt aHiddenCountPreDelete );
	TBool CompareFields(const CContactItemField &aField1, const CContactItemField &aField2) const;
	
private:
	CContactItem *iCustomTemplate;
	CContactItemFieldSet *iFields;//fieldset of open contact item
	CContactItem *iContactRead;//contact opened using readcontact
	CContactItem *iContactOpened;//contact opened using opencontact
	TContactItemId iCustomTemplateId;//contact id of custom template
	};

#endif
