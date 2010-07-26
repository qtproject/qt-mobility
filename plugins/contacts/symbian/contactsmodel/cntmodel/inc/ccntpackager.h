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




/**
 @file
 @internalComponent
 @released
*/


#ifndef __CCNTPACKAGER_H__
#define __CCNTPACKAGER_H__

#include <cntdb.h>
#include <badesca.h>

class CContactItem;
class CCntFilter;
class CContactItemViewDef;
class CContactViewDef;
class CContactTextDef;
class CContentType;
class CContactIdArray;
class CViewContact;


/**
The CCntPackager class provides methods for packing and unpacking objects used
in IPC (inter-process communications) messages.

The class contains a dynamically allocated buffer for accommodating different 
kinds of object that will be sent over the process boundaries.
*/
NONSHARABLE_CLASS(CCntPackager) : public CBase
	{
public:
	IMPORT_C static CCntPackager* NewL();
	IMPORT_C ~CCntPackager();
	IMPORT_C TPtr8 PackL(const CContactItem& aItem);
	IMPORT_C TPtr8 PackL(const CContactItem& aItem, TInt& aSize);
	IMPORT_C TPtr8 PackL(const CContentType& aItem);
	IMPORT_C TPtr8 PackL(const CContactTextDef& aItem);	
	IMPORT_C TPtr8 PackL(const CContactItemViewDef& aItem);
	IMPORT_C TPtr8 PackL(const CContactViewDef& aItem);
	IMPORT_C TPtr8 PackL(const CContactIdArray& aItem);
	IMPORT_C TPtr8 PackL(const CCntFilter& aItem);	
	IMPORT_C TPtr8 PackL(const CViewContact& aItem);
	IMPORT_C TPtr8 PackL(const CDesCArray& aItem);
	IMPORT_C TPtr8 PackL(const CArrayFix<CContactDatabase::TSortPref>& aItem);
	IMPORT_C TPtr8 PackL(const CArrayFixFlat<TUid>& aItem);
	
	IMPORT_C CContactItem* UnpackCntItemLC() const;
	IMPORT_C CContentType* UnpackCntContentTypeLC() const;
	IMPORT_C CContactTextDef* UnpackCntTextDefLC() const;
	IMPORT_C CContactItemViewDef* UnpackCntItemViewDefLC() const;
	IMPORT_C CContactViewDef* UnpackCntViewDefLC() const;
	IMPORT_C CContactIdArray* UnpackCntIdArrayLC() const;
	IMPORT_C CCntFilter* UnpackCntFilterLC() const;
	IMPORT_C CViewContact* UnpackViewContactLC() const;
	IMPORT_C CDesCArray* UnpackDesCArrayLC() const;
	IMPORT_C CArrayFix<CContactDatabase::TSortPref>* UnpackCArrayFixLC() const;
	IMPORT_C CArrayFixFlat<TUid>* UnpackTUidArrayLC() const;
	
	IMPORT_C void Clear();
	IMPORT_C void SetBufferFromMessageL(const RMessage2& aMessage, TUint aMessageSlot=0);

	IMPORT_C const TDesC8& GetTransmittingBuffer() const;
	IMPORT_C TDes8& GetReceivingBufferL(TInt aSize=0);

private:
	CCntPackager();
	void ConstructL();
	void ResetPointer();	
	
private:
	CBufFlat* iBuffer;
	TInt iMaxBufferSize;

	mutable TPtr8 transPtr;
	TPtr8 recPtr;
	};

	
#endif // __CCNTPACKAGER_H__
