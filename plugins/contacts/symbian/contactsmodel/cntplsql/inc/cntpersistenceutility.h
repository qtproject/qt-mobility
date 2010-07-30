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

#ifndef __PERSISTENCEUTILITY_H__
#define __PERSISTENCEUTILITY_H__

#include <cntitem.h>
#include <sqldb.h>

NONSHARABLE_CLASS(TCntPersistenceUtility)
	{
public:
	static void ReadBlobL(CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate, RSqlDatabase& aDatabase);
	static void ReadTextBlobL(CEmbeddedStore& aTextHeaderStore, TPtrC& aTextValues, CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate);
	static void ReadBinaryBlobL(CEmbeddedStore& aBinaryHeaderStore, CEmbeddedStore& aBinaryBlobStore, CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate);
	static void WriteBlobL(CEmbeddedStore& aTextEmbeddedStore, RWriteStream& aTextValues, CEmbeddedStore& aBinaryEmbeddedStore, CEmbeddedStore& aBinaryEmbeddedBlobStore, const CContactItem& aItem, const CContactTemplate* aSysTemplate);

    static void ReadTextBlobL(RReadStream& aHeaderStream, const HBufC* aTextBuffer, const CContactTextDef& aTextDef, const CContactTemplate& aSystemTemplate, RPointerArray<CContactItemField>& aFields, TBool& aSearchFastAccessFields);	
	            
	static TBool FindTxtFieldInTextBlobL(RReadStream& aHeaderStream, HBufC* aTextFieldsBuf, const CContactTemplate& aSystemTemplate, const TFieldType& aFieldType, TDes& aText);
	
	static TBool CopyMinFieldText(TPtrC aSrc,TDes& aDest);
	static TBool IsFastAccessField(TFieldType aFieldType);	
	
	static const TDesC& GetFastAccessColumnNameById(TInt32 aFieldUid);
	static void  ResetAndDestroyRPointerArray(TAny *aArray);
	
	//Functions for type flags
	static TInt  TypeFlagsToHint(TInt aTypeFlags);
	static TUid  TypeFlagsToContactTypeUid(TInt aTypeFlags);
    static TInt  ContactTypeUidToTypeFlags(TUid aContactTypeUid);

private:
	static void ReadTextBlobL(CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate, RSqlDatabase& aDatabase);
	static void ReadBinaryBlobL(CContactItem& aItem, const CContactItemViewDef& aView, const CContactItem* aTemplate, RSqlDatabase& aDatabase);
	
	static HBufC* LoadTextStreamLC(RReadStream& aStream);
	static void RestoreTextL(CContactItemFieldSet& aFieldSet, CStreamStore& aHeaderStore, TStreamId aId, HBufC* textStream, const CContactItemViewDef& aViewDef, const CContactItem* aTemplate);
	static void RestoreBinaryL(CContactItemFieldSet& aFieldSet, CStreamStore& aHeaderStore, TStreamId aId, CStreamStore* aBlobStore, const CContactItemViewDef& aViewDef, const CContactItem* aTemplate);
	};

#endif
