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


#include <cntitem.h>
#include <cntfilt.h>
#include <s32mem.h>
#include "ccntpackager.h"
#include <cntviewbase.h>


const TInt KGranularityRank = 8; //2^8 = 256 bytes
const TInt KDefaultPackagerSize = 3514; //Observed Techview Golden template size.


/**
First phase constructor.
*/ 
CCntPackager::CCntPackager()
	: transPtr(NULL,NULL,NULL), recPtr(NULL,NULL,NULL)
 	{
 	}


/**
Second phase constructor.

Allocate buffer used to hold packed objects.  Initializes iMaxBufferSize and
iBuffer, setting the latter's granularity.
*/
void CCntPackager::ConstructL()
	{	
	iBuffer = CBufFlat::NewL(1 << KGranularityRank);
	iMaxBufferSize = KDefaultPackagerSize;
	} 	


/**
The destructor frees all resources owned by the packager, prior to its
destruction. 
*/
EXPORT_C CCntPackager::~CCntPackager()
	{
	delete iBuffer;	
	}

	
/**
Allocates and constructs a new Packager. 

@return Pointer to the new CCntPackager object. 
*/
EXPORT_C CCntPackager* CCntPackager::NewL()
	{
	CCntPackager* packager=new(ELeave) CCntPackager;	
	CleanupStack::PushL(packager);
	packager->ConstructL();
	CleanupStack::Pop(packager);
	return packager;
	}


/** 
Clears iBuffer.  Calls CBufFlat's Reset(), deletes data and compresses the
buffer. 
*/
EXPORT_C void CCntPackager::Clear()
	{ 
	iBuffer->Reset();	
	}

	
/**
Clears iBuffer and resets pointer.  Does not free any memory.
*/
void CCntPackager::ResetPointer()
	{
	TInt bufferSize = iBuffer->Size();
	if(bufferSize)
		{
		iBuffer->Delete(0,bufferSize);
		}
	}			

	
/**
Checks if iBuffer is large enough to read the descriptor held in the message,
expanding it if necessary.  The data contained in the descriptor is then from
the message into the Packager's internal buffer.

@param aMessage The message containing descriptor to read. 
@param aMessageSlot The message slot containing the descriptor.
*/
EXPORT_C void CCntPackager::SetBufferFromMessageL(const RMessage2& aMessage, TUint aMessageSlot)
	{ 
	TInt bufflen = aMessage.GetDesLengthL(aMessageSlot);
	if (iBuffer->Size() < bufflen)
		{
		iBuffer->ExpandL(0,bufflen - iBuffer->Size());	
		}
	
	TPtr8 bufferPtr = iBuffer->Ptr(0);
	aMessage.ReadL(aMessageSlot, bufferPtr);
	}				


/** 
Returns descriptor pointing to the buffer with serialised object.  This
descriptor should be used as one of the IPC arguments when making an IPC call.

@return Descriptor pointing to the buffer with serialised object.
*/
EXPORT_C const TDesC8& CCntPackager::GetTransmittingBuffer() const
	{
	transPtr.Set(iBuffer->Ptr(0));
	return transPtr;	
	}


/** 
Returns descriptor pointing to the buffer allocated in order to receive a binary
object from the Server. 

The aSize parameter specifies the size of the receiving buffer.  If this
parameter is ommited then the Packager's buffer will be initialised to the
stored default size (kept in the iMaxBufferSize member variable).

@param aSize The desired size of the receiving buffer.

@return Descriptor pointing to the buffer receiving an object from the Server.
*/
EXPORT_C TDes8& CCntPackager::GetReceivingBufferL(TInt aSize)
	{
	if(aSize > iBuffer->Size())
		{
		// Find next divisable by granularity size value.
		(aSize >>= KGranularityRank)++;
		iMaxBufferSize = aSize <<= 8;

		iBuffer->ResizeL(iMaxBufferSize);
		}
	else if(!aSize && iBuffer->Size() < iMaxBufferSize)
		{
		// Use the stored default size.
		iBuffer->ResizeL(iMaxBufferSize);
		}
	
	// The location of the whole buffer may have changed, because reallocation
	// may have taken place.  Update both buffer pointers.
	transPtr.Set(iBuffer->Ptr(0)); 
	recPtr.Set(iBuffer->Ptr(0));

	return recPtr;	
	}


/** 
Packs a CContactItem object. 

@param aItem The contact item to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CContactItem& aItem)
	{ 	
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	aItem.ExternalizeL(writeStream);	

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}


/**
Packs a CContactItem object. 

@param aItem The contact item to be packed.
@param aSize The size of the packed contact item.

@return Pointer to iBuffer. 
*/	
EXPORT_C TPtr8 CCntPackager::PackL(const CContactItem& aItem, TInt& aSize) 
	{ 	
	TPtr8 packaged = PackL(aItem);
	aSize = iBuffer->Ptr(0).Size();
	return packaged;
	}	


/**
Packs a CContentType object. 

@param aItem The content type to be packed.

@return Pointer to iBuffer. 
*/	
EXPORT_C TPtr8 CCntPackager::PackL(const CContentType& aItem) 
	{ 	
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	aItem.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close(); 

	return iBuffer->Ptr(0);
	}

	
/**
Packs a CContactTextDef object. 

@param aItem The contact textdef to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CContactTextDef& aItem)
	{
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	aItem.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}		


/**
Packs a CContactItemViewDef object. 

@param aItem The contact item view def to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CContactItemViewDef& aItem)
	{ 
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);
	
	// Externalize aItem.Use() and aItem.Mode() so we can call the constructor
	// for CContactItemViewDef with them as parameters when unpacking/
	// internalizing later.
	writeStream.WriteInt32L(aItem.Use());
	writeStream.WriteInt32L(aItem.Mode());
	aItem.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}	

	
/**
Packs a CContactViewDef object. 

@param aItem The contact view def to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CContactViewDef& aItem) 
	{ 
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	// Externalize aItem.Use() and aItem.Mode() so we can call the constructor
	// for CContactItemViewDef with them as parameters when unpacking/
	// internalizing later.
	writeStream.WriteInt32L(aItem.ItemDef().Use());
	writeStream.WriteInt32L(aItem.ItemDef().Mode());	
	aItem.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}	
		
/**
Packs a CContactIdArray object. 

@param aItem The contact ID array to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CContactIdArray& aItem)
	{
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	aItem.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}

	
/**
Packs a CCntFilter object. 

@param aItem The contact item filter to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CCntFilter& aItem)
	{
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	aItem.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}	


/**
Packs a CViewContact item. 

@param aItem The CViewContact to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CViewContact& aViewContact)
	{
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	aViewContact.ExternalizeL(writeStream);

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}	


/**
Packs a CDesCArray object. 

@param aItem The descriptor array to be packed.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CDesCArray& aItem)
	{
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	TInt count = aItem.Count();
	writeStream.WriteInt32L(count);
	
	TInt length = 0;
	for(TInt i=0; i<count;++i)
		{
		length = aItem[i].Length();
		writeStream.WriteInt32L(length);		
		writeStream.WriteL(aItem[i],length);
		}

	writeStream.CommitL();
	writeStream.Close();
	return iBuffer->Ptr(0);

	}


/**
Packs an array of sort preferences.

@param aItem The array of sort preferences.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CArrayFix<CContactDatabase::TSortPref>& aItem)
	{
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);

	TInt count = aItem.Count();
	writeStream.WriteInt32L(count);

	for(TInt i=0; i<count;++i)
		{
		aItem[i].ExternalizeL(writeStream);
		}

	writeStream.CommitL();
	writeStream.Close();

	return iBuffer->Ptr(0);
	}


/**
Packs an array of UIDs.

@param aItem The array of TUid objects.

@return Pointer to iBuffer. 
*/
EXPORT_C TPtr8 CCntPackager::PackL(const CArrayFixFlat<TUid>& aItem)
	{
	ResetPointer();
	RBufWriteStream writeStream;
	writeStream.Open(*iBuffer);
	TInt count = aItem.Count();
	writeStream.WriteInt32L(count);
	//TInt length = 0;
	for(TInt i=0; i<count;++i)
		{
		writeStream.WriteInt32L(aItem[i].iUid);
		}
	writeStream.CommitL();
	writeStream.Close();
	return iBuffer->Ptr(0);
	}


/**
Unpacks CContactItem object from the internal buffer. 

@return Pointer to the new contact item. 
*/
EXPORT_C CContactItem* CCntPackager::UnpackCntItemLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);	
	return CContactItem::NewLC(readStream);
	}			

	
/**
Unpacks CContentType object from the internal buffer. 

@return Pointer to the new CContentType item. 
*/
EXPORT_C CContentType* CCntPackager::UnpackCntContentTypeLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);
	return CContentType::NewLC(readStream);
	}	


/**
Unpacks CContactTextDef object from the internal buffer.

@return Pointer to the new CContactTextDef item. 
*/
EXPORT_C CContactTextDef* CCntPackager::UnpackCntTextDefLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);	
	return CContactTextDef::NewLC(readStream);	
	}


/**
Unpacks CContactItemViewDef object from the internal buffer.

@return Pointer to the new CContactItemViewDef item. 
*/
EXPORT_C CContactItemViewDef* CCntPackager::UnpackCntItemViewDefLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);
	return CContactItemViewDef::NewLC(readStream);	
	}

	
/**
Unpacks CContactViewDef object from the internal buffer.

@return Pointer to the new CContactViewDef item. 
*/
EXPORT_C CContactViewDef* CCntPackager::UnpackCntViewDefLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);	
	return CContactViewDef::NewLC(readStream);	
	}	

	
/**
Unpacks CContactIdArray object from the internal buffer.

@return Pointer to the new CContactIdArray item. 
*/
EXPORT_C CContactIdArray* CCntPackager::UnpackCntIdArrayLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);	
	return CContactIdArray::NewLC(readStream);	
	}	

	
/**
Unpacks CCntFilter object from the internal buffer.

@return Pointer to the new CCntFilter item. 
*/
EXPORT_C CCntFilter* CCntPackager::UnpackCntFilterLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);
	return CCntFilter::NewLC(readStream);
	}

		
/**
Unpacks CViewContact object from the internal buffer.

@return Pointer to the new CViewContact item. 
*/
EXPORT_C CViewContact* CCntPackager::UnpackViewContactLC() const
	{
	RBufReadStream readStream;
	readStream.Open(*iBuffer);
	return CViewContact::NewLC(readStream);
	}


/**
Unpacks CDesCArray object from the internal buffer.

@return Pointer to the new CDesCArray object. 
*/
EXPORT_C CDesCArray* CCntPackager::UnpackDesCArrayLC() const
	{
	CDesCArray* unpackedCDesCArray = NULL;
	
	RBufReadStream readStream;
	readStream.Open(*iBuffer);
		
	TInt count = readStream.ReadInt32L();
	if(count!=0)
		{
		unpackedCDesCArray = new(ELeave) CDesCArrayFlat(count);
		CleanupStack::PushL(unpackedCDesCArray);
		TInt length = 0;
		for (TInt i=0; i<count; ++i)
			{
			TFileName fileName;
			length = readStream.ReadInt32L();		
			readStream.ReadL(fileName,length);
			unpackedCDesCArray->AppendL(fileName);
			}
		}	

	return unpackedCDesCArray;
	}


/**
Unpacks an array of sort preferences from the internal buffer.

@return Pointer to the new array of sort preferences.
*/
EXPORT_C CArrayFix<CContactDatabase::TSortPref>* CCntPackager::UnpackCArrayFixLC() const
	{
	CArrayFix<CContactDatabase::TSortPref>* unpacked = NULL;
	RBufReadStream readStream;
	readStream.Open(*iBuffer);
		
	unpacked = new (ELeave) CArrayFixFlat<CContactDatabase::TSortPref>(2);
	CleanupStack::PushL(unpacked);

	TInt count = readStream.ReadInt32L();
	if(count!=0)
		{
		for (TInt i=0; i<count; ++i)
			{
			CContactDatabase::TSortPref pref;
			pref.InternalizeL(readStream);
			unpacked->AppendL(pref);
			}
		}

	return unpacked;
	}


/**
Unpacks an array of TUid objects from the internal buffer.

@return Pointer to the new array of TUid objects.
*/
EXPORT_C CArrayFixFlat<TUid>* CCntPackager::UnpackTUidArrayLC() const
	{
	CArrayFixFlat<TUid>* unpacked = NULL;
	RBufReadStream readStream;
	readStream.Open(*iBuffer);
		
	unpacked = new (ELeave) CArrayFixFlat<TUid>(2);
	CleanupStack::PushL(unpacked);

	TInt count = readStream.ReadInt32L();
	if(count!=0)
		{
		for (TInt i=0; i<count; ++i)
			{
			TUid id;
			id.iUid = readStream.ReadInt32L();
			unpacked->AppendL(id);
			}
		}

	return unpacked;
	}
