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


#include <cntdef.h>
#include <cntfldst.h>
#include <txtetext.h>
#include <confndr.h>
#include <conlist.h>


#include "cntstd.h"
#include <s32mem.h>

const TUid KUidEtextToText={0x100040C8};
const TUid KUidTextToEtext={0x100040C7};


//
// class CContactFieldStorage
//


// for BC-proofing
EXPORT_C void CContactFieldStorage::CContactFieldStorage_Reserved1() 
	{  
	}



//
// class CContactTextField
//

// for BC-proofing
EXPORT_C /*virtual*/ void CContactTextField::CContactFieldStorage_Reserved1()
	{
	}

EXPORT_C CContactTextField::~CContactTextField()
/** Deletes the text owned by the object, prior to its destruction. */
    {
    delete iText;
    }

void CContactTextField::InternalizeL(RReadStream& aStream)
    {
    TInt maxlen =  aStream.ReadInt32L();
    if (maxlen > 0)
    	iText =  HBufC::NewL(aStream, maxlen);
	}

void CContactTextField::InternalizeL(HBufC* aTextStream,TInt aTextFieldIndex)
    {
	TInt pos=0;
	TPtrC des= *aTextStream;
	TInt totalLen=des.Length();
	// remove left part
	for(TInt i=1;i<=aTextFieldIndex && pos<totalLen;i++)
		{
		TInt offset=des.Right(totalLen-pos).Locate(KTextStreamSeparator);
		if (offset != KErrNotFound)
			pos+=offset+1;
		else
			break;
		}
	// remove right part
	if (totalLen>pos)
		{
		TInt len=des.Right(totalLen-pos).Locate(KTextStreamSeparator);
		if (len>0)
			iText=des.Mid(pos,len).AllocL();
		}
	}

void CContactTextField::ExternalizeL(RWriteStream& aStream) const
    {
	// Don't attempt to externalise an empty fieldset.
	if (iText == NULL)
		{
		aStream.WriteInt32L(0);	
		return;
		}

    TInt maxlen = iText->Length();
    aStream.WriteInt32L(maxlen);
    if(maxlen)
    	{
    	aStream << *iText;
    	}    
   }

void CContactTextField::ExternalizeL(RWriteStream& aStream,TBool aIsInlineBlob,TInt /*aTextFieldIndex*/) const
    {
	
	if (iText)
      aStream.WriteL(iText->Des());          
	if (aIsInlineBlob)
		{
		TBuf<1> buffer;
		buffer.Format(_L("%c"),KTextStreamSeparator);
		aStream.WriteL(buffer); 
		}
  }


TStreamId CContactTextField::StoreL(CStreamStore& aStore) const
    {

    RStoreWriteStream stream;
    TStreamId id=stream.CreateLC(aStore);
	ExternalizeL(stream);
    stream.CommitL();
    CleanupStack::PopAndDestroy();	// stream
    return id;
    }

void CContactTextField::RestoreL(CStreamStore& ,RReadStream& aStream)
    {
	InternalizeL(aStream);
	}

EXPORT_C void CContactTextField::SetTextL(const TDesC& aText)
/** Sets the field text. 

This function allocates a new HBufC descriptor, deleting any existing one, 
and copies the new text into it. The function can leave.

@param aText The new field text. */
	{
	HBufC* text=aText.AllocL();
	delete iText;
	iText=text;
	}

EXPORT_C void CContactTextField::SetText(HBufC *aHbuf)
/** Sets the field text. 

The text field object takes ownership of the specified descriptor.
The function cannot leave.

@param aHbuf The new field text. */
	{
	delete iText;
	iText=aHbuf;
	}

// zzz this should be binned or tested (and re-written)
EXPORT_C void CContactTextField::SetTextArray(MDesCArray* anArray)
/** Sets the field text from a descriptor array. Each descriptor 
in the array is appended to the text field storage. They are separated by 
paragraph delimiters (CEditableText::EParagraphDelimiter). Any existing text 
is replaced.

@param anArray Pointer to array of descriptors to set as the field text. */
    {
    TInt count = anArray->MdcaCount();
    delete (iText);
	TInt size=0;
	TInt ii=0;
    for (;ii<count;ii++)
    	{
    	size+=anArray->MdcaPoint(ii).Size();
    	if (ii<(count-1))
	    	size++; //for semicolon
		}
    iText=HBufC::NewL(size);
    for (ii=0;ii<count;ii++)
    	{
    	TPtrC temp(anArray->MdcaPoint(ii));
    	TPtr ptr = iText->Des();
    	ptr.Append(temp);
    	if (ii<(count-1))
			ptr.Append(CEditableText::EParagraphDelimiter);
    	}
    }

EXPORT_C TPtrC CContactTextField::Text() const
/** Gets a copy of the text which is stored in the field.

@return Descriptor pointing to a copy of the field data. */
    {
    TPtrC text;
    if (iText)
    	text.Set(*iText);
    return text;
    }

TBool CContactTextField::IsFull() const
	{
	return (iText && iText->Size()!=0);
	}

HBufC *CContactTextField::EncodeL(const TDesC& aText,TUid aConversion) const
	{
	HBufC8* text=HBufC8::NewLC(aText.Length()*2);
	TPtr8 ptr = text->Des();
	TInt i;
	for (i=0; i < aText.Length(); i++)
		{
		ptr.Append(aText[i] & 0x00FF);
		ptr.Append((aText[i] >> 8) & 0x00FF);
		}
	CCnaConverterList* convList=CCnaConverterList::NewLC(); 
	CConverterBase* conv = convList->NewConverterL(aConversion); 
	if (!conv)
		{
		CleanupStack::PopAndDestroy();          // convList 
		User::Leave(KErrNotSupported);
		}
	CleanupStack::PushL(conv);
	CBufFlat* decodeBuffer = CBufFlat::NewL(256); 
	CleanupStack::PushL(decodeBuffer);
	CBufFlat* encodedBuffer = CBufFlat::NewL(256); 
	CleanupStack::PushL(encodedBuffer);
	decodeBuffer->InsertL(0,ptr);
	RBufReadStream readStream;
	RBufWriteStream writeStream;
	readStream.Open(*decodeBuffer);
	writeStream.Open(*encodedBuffer);
	conv->ConvertObjectL(readStream, writeStream); 
	readStream.Close();
	TInt size=encodedBuffer->Size();
	HBufC* writeBuf=HBufC::NewLC(size); 
	TPtr resulttext = writeBuf->Des();
	for(i = 0; i < (size - 1); i += 2)
		{
		resulttext.Append((encodedBuffer->Ptr(0)[i + 1] << 8) | 
			encodedBuffer->Ptr(0)[i]);
		}

	writeStream.CommitL();
	writeStream.Close();
	CleanupStack::Pop(); // writebuf
	CleanupStack::PopAndDestroy(2); // buffers 
	CleanupStack::PopAndDestroy(2); //conv+convList 
	CleanupStack::PopAndDestroy();  //text
	return (writeBuf);
	}


EXPORT_C TPtrC CContactTextField::StandardTextLC() const
/** Converts a copy of the text stored in the field from Symbian editable text 
format into plain text and returns it as a pointer descriptor.

@return Descriptor pointing to a copy of the field data as plain text. */
    {
    TPtrC text;
    if (iText)
		text.Set(*iText);
	HBufC *hbuf=EncodeL(text,KUidEtextToText);
	CleanupStack::PushL(hbuf);
	return(hbuf->Des());
	}

EXPORT_C void CContactTextField::SetStandardTextL(const TDesC& aText)
/** Converts a text string from plain text into Symbian editable text, and sets 
this as the text which is stored in the field.  The text is truncated to
KCntMaxTextFieldLength characters if necessary.

@param aText The new field data. */
	{
	// Make a copy of aText and truncate if necessary.
	TPtr truncText(const_cast<TUint16*>(aText.Ptr()),KCntMaxTextFieldLength);
	truncText.SetLength(aText.Length()>KCntMaxTextFieldLength ? KCntMaxTextFieldLength : aText.Length());

	HBufC* text=EncodeL(truncText,KUidTextToEtext);
	delete iText;
	iText=text;
	}

EXPORT_C void CContactTextField::SetStandardTextArray(MDesCArray* anArray)
/** Converts an array of text strings from plain text into Symbian editable text, 
appends them to a single descriptor, separating them with the new line character, 
and sets this as the text which is stored in the field. Any existing field 
text is replaced.  The text is truncated to KCntMaxTextFieldLength characters if
necessary.

@param anArray Array of descriptors to set as the field data. */
    {
    TInt count = anArray->MdcaCount();
    HBufC *txt=HBufC::NewL(0);
	CleanupStack::PushL(txt);
    for (TInt ii=0;ii<count;ii++)
    	{
		TInt mdcaLen=anArray->MdcaPoint(ii).Size();
		if (mdcaLen>0)
			{
    		if (txt->Length()>0)
				txt->Des().Append(TChar('\n'));
    		txt=txt->ReAllocL(txt->Length()+mdcaLen+1);	// +1 In case we add '\n'
			CleanupStack::Pop();	// txt(old value)
			CleanupStack::PushL(txt);
    		TPtrC temp(anArray->MdcaPoint(ii));
    		txt->Des().Append(temp);
			}
    	}

	// Truncate if necessary.
	if (txt->Length()>KCntMaxTextFieldLength)
		{
		txt->Des().SetLength(KCntMaxTextFieldLength);
		}
		
	HBufC *text=EncodeL(txt->Des(),KUidTextToEtext);
	CleanupStack::PopAndDestroy();	// txt
	delete iText;
    iText=text;
    }

//
// class CContactDateField
//


// for BC-proofing
EXPORT_C /*virtual*/ void CContactDateField::CContactFieldStorage_Reserved1()
	{
	}

CContactDateField::CContactDateField()
	{
	iTime=Time::NullTTime();
	}

TStreamId CContactDateField::StoreL(CStreamStore& aStore) const
    {
    RStoreWriteStream stream;
    TStreamId id=stream.CreateLC(aStore);
	ExternalizeL(stream);
    stream.CommitL();
    CleanupStack::PopAndDestroy();	// stream
    return id;
    }

void CContactDateField::RestoreL(CStreamStore& ,RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

void CContactDateField::InternalizeL( RReadStream& aStream)
    {
    TInt64 timevalue;
    aStream>>timevalue;
	iTime=TTime(timevalue);
    }

void CContactDateField::ExternalizeL(RWriteStream& aStream) const
    {
	aStream<<iTime.Int64();
    }

EXPORT_C void CContactDateField::SetTime(TTime aTime)
/** Sets the date/time stored in the field from a TTime value.

@param aTime The field's new date/time value. */
    {
    iTime=aTime;
    }

EXPORT_C void CContactDateField::SetTime(TDateTime aDateTime)
/** Sets the date/time stored in the field from a TDateTime value.

@param aDateTime The field's new date/time value. */
    {
    iTime=TTime(aDateTime);
    }

EXPORT_C TTime CContactDateField::Time() const
/** Gets the date/time stored in the field as a TTime value.

@return The field's date/time value. */
    {
    return iTime;
    }

TBool CContactDateField::IsFull() const
	{
	return(iTime!=Time::NullTTime());
	}

//
// class CContactStoreField
//

// for BC-proofing
EXPORT_C /*virtual*/ void CContactStoreField::CContactFieldStorage_Reserved1()
	{
	}

CContactStoreField::~CContactStoreField()
	{
	delete iThing;
	}

TStreamId CContactStoreField::StoreL(CStreamStore& aStore) const
    {
    RStoreWriteStream stream;
    TStreamId id=stream.CreateLC(aStore);
	ExternalizeL(stream);
    stream.CommitL();
    CleanupStack::PopAndDestroy();	// stream
    return id;
    }

void CContactStoreField::RestoreL(CStreamStore&, RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

void CContactStoreField::InternalizeL(RReadStream& aStream)
    {
    TInt32 length;
    aStream>>length;
    if (iThing)
    	{
    	delete (iThing);
    	}
    if (length)
    	{
		// stream the HBufC descriptor in from the the stream store
    	iThing=HBufC8::NewLC(aStream,length);
    	CleanupStack::Pop();// buf
    	}
    }

void CContactStoreField::ExternalizeL(RWriteStream& aStream) const
    {
    if (IsFull())
    	{
    	aStream<<TInt32(iThing->Size());
    	aStream<<*iThing;
    	}
    else
    	{
    	aStream<<TInt32(0);
    	}
    }


EXPORT_C void CContactStoreField::SetThingL(const TDesC8& aDes)
/** Sets the field data from an 8-bit descriptor.

@param aDes 8-bit descriptor containing the binary data to copy into the 
field. */
    {
    if (iThing)
    	{
    	delete (iThing);
    	}
	iThing = aDes.AllocL();
    }

EXPORT_C void CContactStoreField::SetThingL(const HBufC8& aBuf)
/** Sets the field data from an 8-bit heap descriptor.

@param aDes 8-bit heap descriptor containing the binary data to copy into the field. */
    {
    if (iThing)
    	{
    	delete (iThing);
    	}
	iThing = aBuf.AllocL();
    }

EXPORT_C void CContactStoreField::SetThingL(const CBufBase* aBuf)
/** Sets the field data from a buffer.

@param aBuf Buffer containing the binary data to copy into the field. */
	{
	TInt len=aBuf->Size();
	HBufC8* buf=HBufC8::NewLC(len);
	TPtr8 ptr=buf->Des();
	TInt pos=0;
	while (pos<len)
		{
		ptr.Append(CONST_CAST(CBufBase*,aBuf)->Ptr(pos));
		pos=ptr.Length();
		}
	CleanupStack::Pop(buf);
    if (iThing)
    	{
    	delete (iThing);
    	}
	iThing=buf;
	}

EXPORT_C HBufC8* CContactStoreField::Thing() const
/** Gets a pointer to the binary data stored in the field.

@return Pointer to the binary data stored in the field. */
    {
    return iThing;
    }

TBool CContactStoreField::IsFull() const
	{
	return (iThing && iThing->Size()!=0);
	}

//
// class CContactAgentField
//

// for BC-proofing
EXPORT_C /*virtual*/ void CContactAgentField::CContactFieldStorage_Reserved1()
	{
	}

TStreamId CContactAgentField::StoreL(CStreamStore& aStore) const
    {
    RStoreWriteStream stream;
    TStreamId id=stream.CreateLC(aStore);
	ExternalizeL(stream);
    stream.CommitL();
    CleanupStack::PopAndDestroy();	// stream
    return id;
    }

void CContactAgentField::RestoreL(CStreamStore& ,RReadStream& aStream)
    {
	InternalizeL(aStream);
	}

void CContactAgentField::InternalizeL( RReadStream& aStream)
    {
    iAgentId=TContactItemId(aStream.ReadInt32L());
    }

void CContactAgentField::ExternalizeL(RWriteStream& aStream) const
    {
	aStream.WriteInt32L(iAgentId);
    }

EXPORT_C void CContactAgentField::SetAgentId(TContactItemId aId)
/** 
* Sets the agent ID field data.
* @param aId The agent ID. Must be > 0 otherwise it won't be set.
*/
    {
    if (aId > 0)
        {
        iAgentId = aId;
        }
    }

EXPORT_C TContactItemId CContactAgentField::Value() const
/** Gets the agent ID field data.

@return The agent ID. */
    {
    return iAgentId;
    }

TBool CContactAgentField::IsFull() const
	{
	return (iAgentId!=KNullContactId);
	}
