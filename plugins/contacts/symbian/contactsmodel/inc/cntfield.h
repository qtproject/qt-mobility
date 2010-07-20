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
* Persistence layer exports
*
*/




#if !defined(__CNTFIELD_H__)
#define __CNTFIELD_H__

#include <cntdef.h>



/** Null field ID.
@publishedAll
@released
*/
const TInt KNullFieldId=-1;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/** @internalComponent */
const TInt KIntContactHintIsPhone=0x02;
/** @internalComponent */
const TInt KIntContactHintIsMsg=0x04;
/** @internalComponent */
const TInt KIntContactHintIsCompanyName=0x08;
/** @internalComponent */
const TInt KIntContactHintIsFamilyName=0x10;
/** @internalComponent */
const TInt KIntContactHintIsGivenName=0x20;
/** @internalComponent */
const TInt KIntContactHintIsAddress=0x40;
/** @internalComponent */
const TInt KIntContactHintIsAdditionalName=0x80;
/** @internalComponent */
const TInt KIntContactHintIsSuffixName=0x100;
/** @internalComponent */
const TInt KIntContactHintIsPrefixName=0x200;
/** @internalComponent */
const TInt KIntContactHintStorageInline=0x400;
/** @internalComponent */
const TInt KIntContactHintIsEmail=0x4000;
/** @internalComponent */
const TInt KIntContactHintIsPronunciation=0x800;
/** @internalComponent */
const TInt KIntContactHintIsCompanyNamePronunciation=KIntContactHintIsPronunciation|KIntContactHintIsCompanyName;
/** @internalComponent */
const TInt KIntContactHintIsGivenNamePronunciation=KIntContactHintIsPronunciation|KIntContactHintIsGivenName;
/** @internalComponent */
const TInt KIntContactHintIsFamilyNamePronunciation=KIntContactHintIsPronunciation|KIntContactHintIsFamilyName;

/** @internalComponent */
const TInt KHintTypeMask = 0x1FFFFF;

#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__

/** Number of additional type ids.
@internalComponent 
*/
const TInt KHintAdditionalMask    = 0x7F000000;
/** Bit flag to show if there is a vCard mapping uid.
@internalComponent 
*/
const TInt KHintVCardMappingMask  = 0x80000000;
/** Template field mask in low hint value.
@internalComponent 
*/
const TInt KHintTemplateFieldMask = 0x7FFFFFFF;

/** @internalComponent */
const TInt KHintAdditionalMaskShift = 24;

#else //_SYMBIAN_USE_SQLITE__

/** @internalComponent */
const TInt KHintAdditionalMask = 0x200000;
/** @internalComponent */
const TInt KHintIdMask = 0xFFC00000;

#endif //_SYMBIAN_USE_SQLITE__
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

class RWriteStream;
class RReadStream;
class CContactFieldStorage;
class CContactTextField;
class CContactStoreField;
class CContactAgentField;
class CContactDateField;
class TFieldHeader;
class CContactItemField;
class CContactItemFieldDef;
class CContactItemFieldSet;
class CContactItem;

class CContentType : public CBase
/** The content type for a contact item field. 

The content type identifies the field's type and the vCard property which 
the field maps to. It is owned by the field. Field types are defined as UIDs 
in cntdef.h.

The vCard mapping is optional. It should be specified if the field type alone 
is not enough to identify the mapping. The possible values for the vCard 
mapping are defined as UIDs in cntdef.h.

Each field is uniquely identified by the combination of UIDs contained in 
the content type. 
@publishedAll
@released
*/
    {
public:
    IMPORT_C ~CContentType();
	IMPORT_C static CContentType* NewL();
	IMPORT_C static CContentType* NewL(TFieldType aFieldType,TUid aMapping=KNullUid);
	IMPORT_C static CContentType* NewL(const CContentType &aContentType);
	static CContentType* NewLC(RReadStream& aStream);
	IMPORT_C void SetMapping(TUid aMapping);
	IMPORT_C void AddFieldTypeL(TFieldType aFieldType);
	IMPORT_C void RemoveFieldType(TFieldType aFieldType);
    IMPORT_C TFieldType FieldType(TInt aIndex) const;
    IMPORT_C TInt FieldTypeCount() const;
    IMPORT_C TUid Mapping() const;
	IMPORT_C TBool ContainsFieldType(TFieldType aFieldType) const;
	IMPORT_C TBool SupportsMultipleLines() const;
	IMPORT_C TBool operator==(const CContentType &aType) const;
	IMPORT_C TBool IsEqualForSyncUpdate(const CContentType& aType) const;
	IMPORT_C TBool IsEqual(const CContentType& aType) const;
	void Reset(); 
	void InternalizeAdditionalUidsL(TInt aCount,RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);		
private:
    CContentType();
	CContentType(TUid aMapping);
	void ConstructL();
	void CloneL(const CContentType &aContentType);
private:
    TUid iMapping;
	CArrayFix<TUid>* iFieldTypes;
    };

class CContactItemField : public CBase
/** A field in a contact item.

Fields are grouped into a field set (CContactItemFieldSet), which is owned by 
the contact item.

The contact item field stores the field data. The data may be text, a date/time 
value, or an agent, as indicated by the storage type (TStorageType). The storage 
type is also owned by the field. Note that numeric field data is not supported. 
All numbers, for instance telephone or fax numbers are stored as text.

Fields also have a content type, attributes and a label. The label is a string 
which identifies the field to a user e.g. First name, Last name.

The following attributes are supported: 

- hidden: the field is not displayed if the view definition masks hidden 
fields

- disabled: the field cannot be deleted from the original template and thus 
is marked as disabled

- read only: the field cannot be written to

- synchronise: used by Connectivity applications; set by default for all 
contact item fields

- user added: the field was added by the user and was not taken from the 
template

- template: indicates that the field is part of a template. Template fields 
differ from other fields in that they are persisted when they do not contain 
any data.

- speed dial: the field contains a telephone number which maps to one of 
the nine possible speed dial positions.

These attributes can be set using member functions of this class, but they 
are normally set automatically by other functions, for instance when 
setting a speed dial field, CContactDatabase automatically sets the field's 
speed dial attribute. 
@publishedAll
@released
*/
	{
private:
	friend class TFieldHeader;
	friend class CContactItemFieldSet;
	friend class CContactDatabase;      
	friend class CContactTables;
	friend class RPplContactTable;
	friend class CPackagerTests;
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
    class THint;
#else
	class THint
	/**
	@internalComponent
	@released
	*/
		{
	public:
		THint();
		THint(TInt aValue);
		TBool operator==(const THint& aHint) const;
		TBool operator!=(const THint& aHint) const;
		TInt  HintType() const;
		inline void SetIsMatchAll();
		inline void SetIsPhone();
		inline void SetIsMsg();
		inline void SetIsCompanyName();
		inline void SetIsFamilyName();
		inline void SetIsGivenName();
		inline void SetIsCompanyNamePronunciation();
		inline void SetIsFamilyNamePronunciation();
		inline void SetIsGivenNamePronunciation();
		inline void SetIsAddress();
		inline void SetIsAdditionalName();
		inline void SetIsSuffixName();
		inline void SetIsPrefixName();
		inline void SetStorageIsInline();
		inline void SetIsEmail();
		inline TBool IsMatchAll() const;
		inline TBool IsDefault() const;
		inline TBool IsPhone() const;
		inline TBool IsMsg() const;
		inline TBool IsCompanyName() const;
		inline TBool IsFamilyName() const;
		inline TBool IsGivenName() const;
		inline TBool IsCompanyNamePronunciation() const;
		inline TBool IsFamilyNamePronunciation() const;
		inline TBool IsGivenNamePronunciation() const;
		inline TBool IsAddress() const;
		inline TBool IsAdditionalName() const;
		inline TBool IsSuffixName() const;
		inline TBool IsPrefixName() const;
		inline TBool IsStorageInline() const;
		inline TBool IsEmail() const;
		inline TBool Contains(const THint& aHint) const;
		
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
		inline void SetHasAdditionalUids();
		TBool HasAdditionalUids() const;
		
		TInt FieldId() const;
		void SetFieldId(TInt aId);
		
#else //__SYMBIAN_CNTMODEL_USE_SQLITE__
        TInt  TemplateFieldId() const;
        void  SetTemplateFieldId(TInt aTemplateFieldId);
        
        inline void SetHasVCardMappingUid();
        inline TBool HasVCardMappingUid() const;
        
        inline TInt  AdditionalUidsNum() const;
        inline void  SetAdditionalUidsNum(TInt aNumber);
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__		
	public:
		TInt iHintValue;

		};
#endif
		
public:
	IMPORT_C static CContactItemField* NewLC();
	IMPORT_C static CContactItemField* NewL(TStorageType aType);
	IMPORT_C static CContactItemField* NewLC(TStorageType aType);
	IMPORT_C static CContactItemField* NewL(TStorageType aType, TFieldType aFieldType);
	IMPORT_C static CContactItemField* NewLC(TStorageType aType, TFieldType aFieldType);
	IMPORT_C static CContactItemField* NewL(const CContactItemField &aField);
	IMPORT_C static CContactItemField* NewLC(const CContactItemField &aField);
	IMPORT_C static CContactItemField* NewL(TStorageType aType, const CContentType &aContentType);
	IMPORT_C static CContactItemField* NewLC(TStorageType aType, const CContentType &aContentType);
	IMPORT_C ~CContactItemField();

	IMPORT_C TStorageType StorageType() const;
	IMPORT_C const CContentType &ContentType() const;
	inline TBool IsHidden() const;
	inline TBool IsReadOnly() const;
	inline TBool IsDisabled() const;
	inline TBool IsTemplate() const;
	inline TBool IsPrivate() const;
	inline TBool IsSpeedDial() const;
	inline TBool DoSynchronize() const;
	inline TBool OverRidesLabel() const;
	inline TBool HasExtraStream() const;
	inline TBool UsesTemplateTypes() const;
	inline TBool LabelUnspecified() const;
	inline TBool UserAddedField() const;
	inline TBool IsCustomFilterable() const;
	IMPORT_C void SetHintType(TInt aType);
	IMPORT_C TPtrC Label() const;
	IMPORT_C void ResetStore();
	IMPORT_C CContactFieldStorage* Storage() const;
	IMPORT_C CContactTextField * TextStorage() const;
	IMPORT_C CContactStoreField * StoreStorage() const;
	IMPORT_C CContactAgentField * AgentStorage() const;
	IMPORT_C CContactDateField * DateTimeStorage() const;
	IMPORT_C void AddFieldTypeL(TFieldType aFieldType);
	IMPORT_C void RemoveFieldType(TFieldType aFieldType);
	IMPORT_C void SetMapping(TUid aMapping);
	IMPORT_C void SetHidden(TBool aHidden);
	IMPORT_C void SetReadOnly(TBool aReadOnly);
	IMPORT_C void SetSynchronize(TBool aSynchronize);
	IMPORT_C void SetDisabled(TBool aDisabled);
	IMPORT_C void SetLabelL(const TDesC& aLabel);
	IMPORT_C void SetLabel(HBufC* aLabel);
	IMPORT_C void SetUserAddedField(TBool aUserAddedField);
	IMPORT_C void SetTemplateField(TBool aTemplateField);
	IMPORT_C void SetPrivate(TBool aTemplateField);
	IMPORT_C void SetSpeedDial(TBool aSpeedDialField);
	IMPORT_C void SetId(TInt aId);
	IMPORT_C TInt Id() const;
	IMPORT_C TUint UserFlags() const;
	IMPORT_C void SetUserFlags(TUint aFlags);
	IMPORT_C static TBool IsValidLabel(const TDesC& aLabel,TInt& aInvalidPos);
	IMPORT_C TBool IsTemplateLabelField() const;
	IMPORT_C void GetFieldText(TDes &aText) const;
	IMPORT_C void RestoreHeaderL(RReadStream& aStream, const CContactItemFieldSet *aSystemTemplateFields);
	IMPORT_C TStreamId RestoreFieldTypesL(RReadStream &aRootStream, const CContactItemFieldSet *aSystemTemplateFields);
	IMPORT_C void RestoreTextL(HBufC *aTextStream,TInt aTextFieldIndex);
	IMPORT_C TBool RestoreIfMatchL(RReadStream& aStream,const CContactItemFieldDef *aFieldDef, const CContactItemFieldSet *aSystemTemplateFields,HBufC *aTextStream,TInt aTextIndex);

public:
	IMPORT_C void SetCustomFilterable(EContactFieldFlags aContactFilterType);
	IMPORT_C void Reset();
	void SetUsesTemplateTypes(TBool aUsesTemplateTypes);
	void SetOverRidesLabel(TBool aValue);
	TInt TemplateFieldId() const;
	void SetDeleted(TBool aDeleted);
	inline TBool IsDeleted() const;
	void CopyStorageL(const CContactItemField &aField);
	void UpdateFieldFlags(const CContactItemFieldSet& aTemplateFieldSet);
	void PopulateStoreL(RStoreWriteStream& aRootStream, TInt aCount, CArrayFix<TFieldHeader>& aFieldHeaderArray) const;
	void PrepareFieldAsTemplateL(CContactItemFieldSet& aSystemTemplateFieldSet);
	IMPORT_C TBool HasCustomFilter(EContactFieldFlags& contactFieldFlag) const;
	TBool RestoreIfMatchL(RReadStream& aStream,TFieldType aFieldType, const CContactItemFieldSet *aSystemTemplateFields,HBufC *aTextStream,TInt aTextIndex);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);		
	
protected:
	void UsesTemplateData(TInt aTemplateFieldId);
private:
	void UsesTemplateLabel();
	void SetLabelUnspecified(TBool aUnspecified);
	const CContentType &TemplateContentType(const CContactItemFieldSet &aSystemTemplateFields) const;
	TFieldHeader StoreL(RWriteStream& aTextStream,CStreamStore& aBlobStore,TInt aTextFieldIndex);
	TStreamId StoreAdditionalUidsL(CStreamStore& aStore,TStreamId aId, CArrayFixFlat<TUid> *aAdditionalFields);
  	TStreamId StoreDataL(CStreamStore& aStore);

   	void RestoreL(CStreamStore& aStore,RReadStream& aStream);
    void RestoreDataL(CStreamStore& aStore,TStreamId aId);
	
 	TStreamId RestoreHintL(CStreamStore& aStore,RReadStream& aStream);
	TBool AddFieldToHint(TFieldType aFieldType, CContactItemField::THint &aHint) const;
	void MapHintsToFieldTypesL(THint aHint);
	CContactItemField();
	CContactItemField(TStorageType aType);
	void CloneL(const CContactItemField &aField);
	void ConstructStorageL();
private:
	enum {EUserMaskShift=4};
	enum // er5 hard coded enum most of which have been used but not documented ;o(
		{
		EHidden				=0x00000001,
		EReadOnly			=0x00000002,
		ESynchronize		=0x00000004,
		EDisabled			=0x00000008,
		/* 0x00000010,-> 0x00000080 are used for 
			the user defined attribs / categories like work /
			Personal / Other / None - used in er5 UI */
		EUserMask			=0x000000F0,
		ETemplateMask		=EUserMask|ESynchronize|EReadOnly|EHidden,
		EOverRidesLabel		=0x00000100,
		EUsesTemplateData	=0x00000200,
		EUserAddedField		=0x00000400,
		ETemplate			=0x00000800,
		/* 0x00001000,-> 0x0000F000 are used for 
			the storage type of the contact field */
		ELabelUnspecified	=0x40000000,
		EDeleted			=0x80000000
		};
	enum 
		// new extended enum to allow cntmodel to have
		// more attributes per field
		{
		EPrivate			=0x00000001,
		ESpeedDial			=0x00000002,
		EUserDefinedFilter	=0x00000004,
		EUserDefinedFilter1 =0x00000008,
		EUserDefinedFilter2	=0x00000010,
		EUserDefinedFilter3	=0x00000020,
		EUserDefinedFilter4	=0x00000040
		};
	
private:
	CContentType *iContentType;
	TStorageType iStorageType;
	HBufC* iLabel;
	TInt iId;
	TUint32 iAttributes;
	TUint32 iExtendedAttributes;
	CContactFieldStorage* iStorage;
	TInt iTemplateFieldId;
	};

class CContactItemFieldDef : public CArrayFixFlat<TUid>
/** A field definition. 

This is an array of field types. Field definitions are used by 
CContactDatabase to specify a subset of fields when searching.

The CContactItemFieldDef class is derived from CArrayFixFlat<TUid> so all 
relevant functions can be used, e.g. to add and remove elements. It adds no 
functions other than a constructor. 
@publishedAll
@released
*/
	{
public:
	inline CContactItemFieldDef() : CArrayFixFlat<TUid>(8) 
	/** Constructs the CContactItemFieldDef object, with an array granularity of 8. */
		{};
	};
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
class TContactFieldAtts
/**
@internalComponent
@released
*/
	{
public:
	inline TContactFieldAtts() : iStorage(0)
		{};

	void SetAttribs(TUint32 aAttribs);
	void SetExtendedAttribs(TUint32 aExtendedAttribs);
	void SetType(TStorageType aType);
	TUint32 Attribs() const;
	TUint32 ExtendedAttribs() const;
	TStorageType Type() const;
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TInt Count() const;
	void SetCount(TInt aCount);
	TInt TemplateFieldId() const;
	void SetTemplateFieldId(TInt aId);
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

		
private:
	TUint32 iStorage;
	
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TUint32 iExtendedAttribs;
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__
	};


class TFieldHeader
/**
@internalComponent
@released
*/
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__

	{
public:
    TFieldHeader();
	TFieldHeader(TContactFieldAtts aAtts, TUint32 aFieldUid, TStreamId aId);
	
	inline TContactFieldAtts FieldAtts() const;
	inline void SetFieldAtts(TContactFieldAtts aAtts);
	
	inline TInt FieldId() const;
    inline void SetFieldId(TInt aId);
    
    inline TStreamId StreamId() const;
    inline void SetStreamId(TStreamId aId);
    
private:
	TContactFieldAtts iAtts;
    TInt      iFieldUid;
	TStreamId iStreamId;
	};

#else //__SYMBIAN_CNTMODEL_USE_SQLITE__

	{
public:
	inline TFieldHeader(TStreamId aId,TContactFieldAtts aAtts);
	
public:
	TStreamId iStreamId;
	TContactFieldAtts iAtts;
	};

inline TFieldHeader::TFieldHeader(TStreamId aId,TContactFieldAtts aAtts)
	{ iStreamId=aId; iAtts=aAtts; }
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

inline TBool CContactItemField::IsHidden() const
/** Gets the value of the field's hidden attribute.

@return ETrue if hidden attribute is set, EFalse if not set. */
    { return iAttributes&EHidden; }

inline TBool CContactItemField::IsReadOnly() const
/** Gets the value of the field's read only attribute.

@return ETrue if read only attribute set, EFalse if not set. */
    { return iAttributes&EReadOnly; }

inline TBool CContactItemField::IsDisabled() const
/** Gets the value of the field's disabled attribute.

@return ETrue if disabled attribute set, EFalse if not set. */
	{ return iAttributes&EDisabled; }

inline TBool CContactItemField::IsTemplate() const
/** Tests whether the field is in a template, as set by SetTemplateField().

@return ETrue if field is a template field. EFalse if not. */
	{ return iAttributes&ETemplate; }

inline TBool CContactItemField::IsPrivate() const
/** Tests whether the field's private attribute is set.

@return True if the field's private attribute is set, false if not. */
	{ return iExtendedAttributes&EPrivate; }

inline TBool CContactItemField::IsSpeedDial() const
/** Tests whether the field is a speed dial field.

@return True if the field is a speed dial field, false if not. */
	{ return iExtendedAttributes&ESpeedDial; }

inline TBool CContactItemField::DoSynchronize() const
/** Gets the value of the field's synchronise attribute.

@return ETrue if synchronise attribute is set, EFalse if not set. */
    { return iAttributes&ESynchronize; }

inline TBool CContactItemField::IsDeleted() const
/** Gets the value of the field's deleted  attribute.

@return ETrue if the field is deleted, otherwise EFalse.                            */
	{ return iAttributes&EDeleted; }
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
inline void CContactItemField::THint::SetIsPhone()
	{iHintValue|=KIntContactHintIsPhone;}
inline void CContactItemField::THint::SetIsMsg()
	{iHintValue|=KIntContactHintIsMsg;}
// turn off Pronunciation bit too?
inline void CContactItemField::THint::SetIsCompanyName()
	{iHintValue|=KIntContactHintIsCompanyName;}
inline void CContactItemField::THint::SetIsFamilyName()
	{iHintValue|=KIntContactHintIsFamilyName;}
inline void CContactItemField::THint::SetIsGivenName()
	{iHintValue|=KIntContactHintIsGivenName;}
inline void CContactItemField::THint::SetIsCompanyNamePronunciation()
	{iHintValue|=KIntContactHintIsCompanyName|KIntContactHintIsPronunciation;}
inline void CContactItemField::THint::SetIsFamilyNamePronunciation()
	{iHintValue|=KIntContactHintIsFamilyName|KIntContactHintIsPronunciation;}
inline void CContactItemField::THint::SetIsGivenNamePronunciation()
	{iHintValue|=KIntContactHintIsGivenName|KIntContactHintIsPronunciation;}
inline void CContactItemField::THint::SetIsAddress()
	{iHintValue|=KIntContactHintIsAddress;}
inline void CContactItemField::THint::SetIsAdditionalName()
	{iHintValue|=KIntContactHintIsAdditionalName;}
inline void CContactItemField::THint::SetIsSuffixName()
	{iHintValue|=KIntContactHintIsSuffixName;}
inline void CContactItemField::THint::SetIsPrefixName()
	{iHintValue|=KIntContactHintIsPrefixName;}
inline void CContactItemField::THint::SetStorageIsInline()
	{iHintValue|=KIntContactHintStorageInline;}
inline void CContactItemField::THint::SetIsEmail()
	{iHintValue|=KIntContactHintIsEmail;}
inline TBool CContactItemField::THint::IsPhone() const
	{return (iHintValue&KIntContactHintIsPhone);}
inline TBool CContactItemField::THint::IsMsg() const
	{return (iHintValue&KIntContactHintIsMsg);}
inline TBool CContactItemField::THint::IsCompanyName() const
	{return ((iHintValue&KIntContactHintIsCompanyNamePronunciation) == KIntContactHintIsCompanyName);}
inline TBool CContactItemField::THint::IsFamilyName() const
	{return ((iHintValue&KIntContactHintIsFamilyNamePronunciation)==KIntContactHintIsFamilyName);}
inline TBool CContactItemField::THint::IsGivenName() const
	{return ((iHintValue&KIntContactHintIsGivenNamePronunciation)==KIntContactHintIsGivenName);}
inline TBool CContactItemField::THint::IsCompanyNamePronunciation() const
	{return ((iHintValue&KIntContactHintIsCompanyNamePronunciation) == KIntContactHintIsCompanyNamePronunciation);}
inline TBool CContactItemField::THint::IsFamilyNamePronunciation() const
	{return ((iHintValue&KIntContactHintIsFamilyNamePronunciation)==KIntContactHintIsFamilyNamePronunciation);}
inline TBool CContactItemField::THint::IsGivenNamePronunciation() const
	{return ((iHintValue&KIntContactHintIsGivenNamePronunciation)==KIntContactHintIsGivenNamePronunciation);}
inline TBool CContactItemField::THint::IsAddress() const
	{return (iHintValue&KIntContactHintIsAddress);}
inline TBool CContactItemField::THint::IsAdditionalName() const
	{return (iHintValue&KIntContactHintIsAdditionalName);}
inline TBool CContactItemField::THint::IsSuffixName() const
	{return (iHintValue&KIntContactHintIsSuffixName);}
inline TBool CContactItemField::THint::IsPrefixName() const
	{return (iHintValue&KIntContactHintIsPrefixName);}
inline TBool CContactItemField::THint::IsStorageInline() const
	{return (iHintValue&KIntContactHintStorageInline);}
inline TBool CContactItemField::THint::IsEmail() const
	{return (iHintValue&KIntContactHintIsEmail);}
	
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
inline void CContactItemField::THint::SetHasAdditionalUids()
	{iHintValue|=KHintAdditionalMask;}
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__ 
inline TBool CContactItemField::THint::Contains(const THint& aHint) const
	{return (iHintValue&aHint.iHintValue);}
#endif
inline TBool CContactItemField::OverRidesLabel() const
/** Tests whether the field's label (as set by SetLabel()) overrides the label 
specified in the field's template.

@return True if the field's label overrides the label specified in the template; 
false if not. */
	{return iAttributes&EOverRidesLabel;}

inline TBool CContactItemField::HasExtraStream() const
/** Tests whether the field needs an extra stream to store information not contained 
in the template. This is the case if the field's content type is not based 
on a template, or if the field has a label which overrides the label specified 
in the template.

@return True if the field uses an extra stream; false if not. */
	{return(!(iAttributes&EUsesTemplateData) || iAttributes&EOverRidesLabel);}

inline TBool CContactItemField::UsesTemplateTypes() const
/** Tests whether or not the field's content type is based on a template.

@return True if the field's content type is based on a template; false if not. */
	{return iAttributes&EUsesTemplateData;}

inline TBool CContactItemField::LabelUnspecified() const
/** Tests whether the field's label has been set, either by a call to SetLabel() 
or by inheriting a label specified in the template.

@return True if the field's label has been set, false if not. */
	{return iAttributes&ELabelUnspecified;}

inline TBool CContactItemField::UserAddedField() const
/** Gets the value of the user added field attribute.

@return ETrue if the user added attribute is set, EFalse if not. */
	{return iAttributes&EUserAddedField;}

inline TBool CContactItemField::IsCustomFilterable() const
/** Tests the value of the user defined filter attribute.

@return ETrue if the user defined filter attribute is set, EFalse if not. */
	{
	return (iExtendedAttributes&EUserDefinedFilter || iExtendedAttributes&EUserDefinedFilter1 ||
	       iExtendedAttributes&EUserDefinedFilter2 || iExtendedAttributes&EUserDefinedFilter3 
	       || iExtendedAttributes&EUserDefinedFilter4);
	}
	
#endif
