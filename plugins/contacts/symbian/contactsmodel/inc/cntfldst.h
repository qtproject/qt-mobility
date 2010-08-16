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


#if !defined(__CNTFLDST_H__)
#define __CNTFLDST_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__BAMDESCA_H__)
#include <bamdesca.h>
#endif

#if !defined(__S32STD_H__)
#include <s32std.h>
#endif

#if !defined(__CNTDEF_H__)
#include <cntdef.h>
#endif
#define KTextStreamSeparator 0x00



class CStreamStore;

class CContactFieldStorage : public CBase
/** Abstract base class for the different types of contact field data storage. 

CContactTextField, CContactStoreField, CContactAgentField, CContactDateField 
and CContactNumberField are all derived from this class.

A pointer to the base class can be obtained by calling CContactItemField::Storage(). 
Pointers to the derived classes can be retrieved by calling CContactItemField::TextStorage(), 
CContactItemField::StoreStorage() etc. 
@publishedAll
@released
*/
	{
public:
	/** Internalises the field data.
	
	@param aStream Stream from which the the contents of the field should be internalised. */
    virtual void InternalizeL( RReadStream& aStream)=0;
	/** Externalises the field data.
	
	@param aStream Write stream to which the contents of the field should be externalised. */
    virtual void ExternalizeL(RWriteStream& aStream) const=0;
	/** Stores the field data.
	
	@param aStore Reference to the stream store to which the field data is written.
	@return The ID of the stream store. */
	virtual TStreamId StoreL(CStreamStore& aStore) const=0;
	/** Restores the field data.
	
	@param aStore Reference to the store from which the object is to be restored.
	@param aStream Reference to the stream which should be passed to InternalizeL(). */
	virtual void RestoreL(CStreamStore& aStore,RReadStream& aStream)=0;
	/** Tests whether the field storage contains data.
	
	@return True if the field storage contains data, false if not. */
	virtual TBool IsFull() const=0;
private:
	IMPORT_C virtual void CContactFieldStorage_Reserved1();
	TAny* CContactFieldStorage_Reserved;
	};

class CContactTextField : public CContactFieldStorage
/** Provides access to the text stored in a contact item field. 

An object of this class can be retrieved using CContactItemField::TextStorage(). 
@publishedAll
@released
*/
	{
public:
	IMPORT_C ~CContactTextField();
public: 
	void InternalizeL(HBufC *aTextStream,TInt aTextFieldIndex);
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
    void ExternalizeL(RWriteStream& aStream,TBool aIsInlineBlob,TInt aTextFieldIndex) const;
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(CStreamStore& aStore,RReadStream& aStream);
	IMPORT_C void SetTextL(const TDesC& aText);
	IMPORT_C void SetText(HBufC *aHbuf);
	IMPORT_C void SetTextArray(MDesCArray* anArray);
	IMPORT_C TPtrC Text() const;
	TBool IsFull() const;
	IMPORT_C TPtrC StandardTextLC() const;
	IMPORT_C void SetStandardTextL(const TDesC& aText);
	IMPORT_C void SetStandardTextArray(MDesCArray* anArray);
private:
	HBufC *EncodeL(const TDesC& aText,TUid aConversion) const;
	IMPORT_C /*virtual*/ void CContactFieldStorage_Reserved1();
private:
	HBufC* iText;
	};

class CContactStoreField : public CContactFieldStorage
/** Provides access to the binary field data stored in an 8-bit descriptor. 

An object of this class can be retrieved using CContactItemField::StoreStorage(). 
@publishedAll
@released
*/
	{
public:
	~CContactStoreField();
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(CStreamStore& aStore,RReadStream& aStream);
	IMPORT_C void SetThingL(const TDesC8& aDes);
	IMPORT_C void SetThingL(const HBufC8& aDes);
	IMPORT_C void SetThingL(const CBufBase* aBuf);
	IMPORT_C HBufC8* Thing() const; 
	void InternalizeL(RReadStream& aStream);
    void ExternalizeL(RWriteStream& aStream) const;
	TBool IsFull() const;
private:
	IMPORT_C /*virtual*/ void CContactFieldStorage_Reserved1();
private:
	HBufC8* iThing;
	};

class CContactAgentField : public CContactFieldStorage
/** Provides access to an agent ID. 

An agent is a field in a contact item which contains another person's 
contact details. It corresponds to the agent property in the vCard 
specification (version 2.1). The agent is identified by an agent ID 
(a TContactItemId).

An object of this class can be retrieved using CContactItemField::AgentStorage(). 
@publishedAll
@released
*/
	{
public:
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(CStreamStore& aStore,RReadStream& aStream);
	IMPORT_C void SetAgentId(TContactItemId aId);
	IMPORT_C TContactItemId Value() const;
	void InternalizeL( RReadStream& aStream);
    void ExternalizeL(RWriteStream& aStream) const;
	TBool IsFull() const;
private:
	IMPORT_C /*virtual*/ void CContactFieldStorage_Reserved1();
private:
	TContactItemId iAgentId;
	};

class CContactDateField : public CContactFieldStorage
/** Provides access to the date/time value stored in a contact item field. 

An object of this class can be retrieved using CContactItemField::DateTimeStorage(). 
@publishedAll
@released
*/
	{
public:
	CContactDateField();
	TStreamId StoreL(CStreamStore& aStore) const;
	void RestoreL(CStreamStore& aStore,RReadStream& aStream);
	IMPORT_C void SetTime(TTime aTime);
	IMPORT_C void SetTime(TDateTime aDateTime);
	IMPORT_C TTime Time() const;
	void InternalizeL( RReadStream& aStream);
    void ExternalizeL(RWriteStream& aStream) const;
	TBool IsFull() const;
private:
	IMPORT_C /*virtual*/ void CContactFieldStorage_Reserved1();
private:
	TTime iTime;
	};



#endif
