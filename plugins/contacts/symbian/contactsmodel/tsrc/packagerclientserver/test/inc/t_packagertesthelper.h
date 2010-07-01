/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "t_packagertestdef.h"

#ifndef __PACKAGER_TEST_HELPER_H__
#define __PACKAGER_TEST_HELPER_H__

#include <e32base.h>
#include <badesca.h>
#include <cntitem.h>
#include <cntfilt.h>
#include <cntfield.h>
#include <cntviewbase.h>
#include "persistencelayer.h"
#include "t_cnttestimplementor.h"
#include "nbcnttestlib/nbcnttestlib.h" 

class CPackagerCntFactory : CBase
	{
public:	
	~CPackagerCntFactory();
	static CPackagerCntFactory* NewL();
	
	CContactItem& 			CreateDefaultCContactItemL() const;
	CContentType& 			CreateDefaultCContentTypeL() const;
	CContactTextDef& 		CreateDefaultCContactTextDefL() const;
	CContactItemViewDef& 	CreateDefaultCContactItemViewDefL() const;
	CContactViewDef& 		CreateDefaultCContactViewDefL() const;
	CContactIdArray& 		CreateDefaultCContactIdArrayL() const;
	CCntFilter& 			CreateDefaultCCntFilterL() const;
	CViewContact& 			CreateDefaultCViewContactL() const;
	CDesCArray& 			CreateDefaultCDesCArrayL() const;
	
private:
	CPackagerCntFactory();
	CContactItem* 			doCreateDefaultCContactItemL() const; 
	CContentType*			doCreateDefaultCContentTypeL() const;	
	CContactTextDef*		doCreateDefaultCContactTextDefL() const;
	CContactItemViewDef* 	doCreateDefaultCContactItemViewDefL() const;		
	CContactIdArray* 		doCreateDefaultCContactIdArrayL() const;
	CCntFilter*				doCreateDefaultCCntFilterL() const;
	CViewContact*			doCreateDefaultCViewContactL() const;
	CDesCArray*				doCreateDefaultCDesCArrayL() const;
	
private:
	mutable CContactItem* 			iContactItem;
	mutable CContentType* 			iContentType;
	mutable CContactTextDef* 		iContactTextDef;
	mutable CContactItemViewDef* 	iContactItemViewDef;
	mutable CContactViewDef* 		iContactViewDef;
	mutable CContactIdArray* 		iContactIdArray;
	mutable CCntFilter* 			iContactFilter;
	mutable CViewContact* 			iViewContact;
	mutable CDesCArray* 			iDesCArray;		

	};

class CPackagerCntComparator : CBase
	{
public:	
	~CPackagerCntComparator();
	static CPackagerCntComparator* NewL();
	
	TBool Compare(const CContactItem& anItem1, const CContactItem& anItem2) const;	
	TBool Compare(const CContentType& anItem1, const CContentType& anItem2) const;
	TBool Compare(const CContactTextDef& anItem1, const CContactTextDef& anItem2) const;
	TBool Compare(const CContactItemViewDef& anItem1, const CContactItemViewDef& anItem2) const;
	TBool Compare(const CContactViewDef& anItem1, const CContactViewDef& anItem2) const;
	TBool Compare(const CContactIdArray& anItem1, const CContactIdArray& anItem2) const;	
	TBool Compare(const CCntFilter& anItem1, const CCntFilter& anItem2) const;
	TBool Compare(const CViewContact& anItem1, const CViewContact& anItem2) const;
	TBool Compare(const CDesCArray& anItem1, const CDesCArray& anItem2) const;
	
private:
	CPackagerCntComparator();
	TBool DoCompareCContactItemField(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const;
	TBool DoCompareCContactItemFieldStorageTypeL(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const;
	TBool DoCompareCContactItemFieldSet(const CContactItemFieldSet& anItemFieldSet1, const CContactItemFieldSet& anItemFieldSet2) const;
	TBool DoCompareTUid(const TUid& aTUid1, const TUid& aTUid2) const;
	TBool DoCompareTContactItemId(const TContactItemId& aCntId1, const TContactItemId& aCntId2) const;
	TBool DoCompareTTime(const TTime& aTime1, const TTime& aTime2) const;
	TBool DoCompareTInt(const TInt& anInt1, const TInt& anInt2) const;
	TBool DoCompareTUint(const TUint& anInt1, const TUint& anInt2) const;	
	TBool DoCompareTStorageType(const TStorageType& aStorageType1, const TStorageType& aStorageType2) const;
	TBool DoCompareHBufC(const HBufC& aBuf1, const HBufC& aBuf2) const;
	TBool DoCompareTPtrC(const TPtrC& aPtr1, const TPtrC& aPtr2) const;
	
	TBool DoCompareCContactTextDefArray(const CContactTextDef& aTextDefArray1, const CContactTextDef& aTextDefArray2) const;
	TBool DoCompareCContactItemViewDefArray(const CContactItemViewDef& anItemViewDefArray1, const CContactItemViewDef& anItemViewDefArray2) const;
	TBool DoCompareCCntFilterInclude(const CCntFilter& aFilter1, const CCntFilter& aFilter2) const;
	TBool DoCompareCCntFilterContactTypeL(const CCntFilter& aFilter1, const CCntFilter& aFilter2) const;
	TBool DoCompareCContactIdArray(const CContactIdArray& anArray1, const CContactIdArray& anArray2) const;
	TBool DoCompareCDesCArray(const CDesCArray& anArray1, const CDesCArray& anArray2) const;
	TBool DoCompareCContactItemType(const CContactItem& anItem1, const CContactItem& anItem2) const;
	TBool DoCompareCViewContactField(const CViewContact& anItem1, const CViewContact& anItem2) const;
	TBool DoCompareCContactItemFieldAttribute(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const;
	TBool DoCompareCContactItemExtendedFieldAttribute(const CContactItemField& anItemField1, const CContactItemField& anItemField2) const;
	TBool DoCompareCContactItemAttribute(const CContactItem& anItem1, const CContactItem& anItem2) const;
	};



#endif // __PACKAGER_TEST_HELPER_H__
