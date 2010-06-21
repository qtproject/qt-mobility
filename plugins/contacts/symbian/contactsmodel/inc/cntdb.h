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


#ifndef __CNTDB_H__
#define __CNTDB_H__

// System includes
#include <e32base.h>
#include <cntdef.h>
#include <cntdbobs.h>
#include <cntsync.h>
#include <s32file.h>
#include <badesca.h>
#include <versit.h>
#include <collate.h>

#include <ecom/ecom.h>

// Classes referenced
//
class CContactViewDef;
class CFoundMatches;
class CContactTextDef;
class CContactItem;
class CContactItemViewDef;
class CContactItemFieldSet;
class CContactItemFieldDef;
class CContactActiveCompress;
class CContactActiveRecover;
class CContactDatabase;
class CContactTables;
class CCntFilter;
class CContentType;
class CContactItemField;
class CIdleFinder;
class CContactClientSession;
class CPrivConverter;
class RCntModel;
class CProxyFactory;
class CCntTemplateCache;
class CSortArray;
class CCntIdleSorter;
class MLplPersistenceLayerFactory;
class CContactOpenOperation;
class CDataBaseChangeObserver;
class CContactConverter;

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
class CContactSynchroniser;
#endif

// Constants

/** 
Maximum string length used to separate the fields in the text definition.
@see TContactTextDefItem
@publishedAll
@released
*/
const TInt KMaxContactTextSeperator=4;
/** 
Constant used during sorting and searching of contacts.
@publishedAll
@released
*/
const TInt KTextFieldMinimalLength=64;
/** 
Maximum number of fields that can be set as filterable fields by licensees.
@publishedAll
@released
*/
const TInt KMaxCustomFilterableFields=4;

/** 
Enables searching for best matching strategy: 
Leading zeros are removed then shorter number must be substring of second one.
@see CContactDatabase::MatchPhoneNumberL
@publishedAll
@released
*/
const TInt KBestMatchingPhoneNumbers=0;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

/** @internalComponent */
const TInt KMajorVersion=1;
/** @internalComponent */
const TInt KMinorVersion=0;
/** @internalComponent */
const TInt KBuildNumber=41;

#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

//
// Classes used for compact
//

class MContactStorageObserver
/**
Mixin used to observe low disk events

@publishedAll
@released
*/
	{
public:
	virtual void HandleDiskSpaceEvent(TInt aDrive) = 0;
	};

class MContactUiCompactObserver
/** Mixin used to observe contact database compaction status
@publishedAll
@released
*/
	{
public:
	virtual void Step(TInt aStep)=0;
	virtual void HandleError(TInt aError)=0;
	};

class MContactUiActive
/**  Mixin used to register the observer.
@publishedAll
@released
*/
	{
public:
	virtual void Cancel()=0;
	virtual void SetObserver(MContactUiCompactObserver *aObserver)=0;
	virtual TInt StepsTogo() const=0;
	virtual TInt Step()=0;
	virtual TInt Error() const=0;
private:
	IMPORT_C virtual void MContactUiActive_Reserved1();
	};

class CContactActiveBase : public CBase, public MContactUiActive
/**
@publishedAll
@released
*/
	{
public:
	~CContactActiveBase();
	void SetObserver(MContactUiCompactObserver *aObserver);
	void Cancel();
	TInt StepsTogo() const;
	TInt Step();
	TInt Error() const;
	void SetContactDatabase(CContactDatabase* aContactDatabase);
	void SetFileManagerL(RCntModel& aCntSvr);
	
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__ 	
protected:
	MContactUiCompactObserver *iObserver;
	RCntModel* iCntSvr; 
	CIdle *iIdle;
	TInt iStep;
	TInt iError;
	CContactDatabase* iContactDatabase;
#endif	
	};
	
class CContactActiveCompress : public CContactActiveBase
/**
@publishedAll
@released
*/
	{
public:
#if !( defined __SYMBIAN_CNTMODEL_HIDE_DBMS__ && defined __SYMBIAN_CNTMODEL_USE_SQLITE__ )
    void ConstructL(RDbNamedDatabase /* &aDataBase */) {};
#endif	
	};

class CContactActiveRecover : public CContactActiveBase
/**
@publishedAll
@released
*/
	{
public:
#if !( defined __SYMBIAN_CNTMODEL_HIDE_DBMS__ && defined __SYMBIAN_CNTMODEL_USE_SQLITE__ )
    void ConstructL(RDbNamedDatabase /* &aDataBase */) {};
#endif	
	};


class TContactTextDefItem
/** An item in a text definition.

A TContactTextDefItem has a field type and an optional separator string
with a maximum length of 4 characters.

@see CContactTextDef
@publishedAll
@released
*/
	{
public:
	IMPORT_C TContactTextDefItem();
	IMPORT_C TContactTextDefItem(TFieldType aFieldType);
	IMPORT_C TContactTextDefItem(TFieldType aFieldType, const TDesC &aSeperator);
public:
	/** The field type. */
	TFieldType iFieldType;
	/** The string used to separate the fields in the text definition. */
	TBuf<KMaxContactTextSeperator> iSeperator;
	};

class CContactTextDef : public CArrayFixFlat<TContactTextDefItem>
/** Defines a way of grouping fields to form a string which represents
a contact item.

A text definition is implemented as an array of text definition items
(TContactTextDefItems).

For example, CContactDatabase::ReadContactTextDefL() uses a text definition to
build up a string from a contact item. When creating the string, it searches
the item for fields whose type matches one of the fields specified in the text
definition. The contents of the first matching field of each type found in the
item are read into the string. These strings may be separated using a field
separator, which is also specified in the text definition. A separator is not
appended to the last field used to make up the string. It is only inserted when
there is a following non-empty field.

Additionally, the text definition may also contain a fallback field
type. This is used when none of the fields specified in the text
definition contain any text. If the item's fallback field contains no text
either, or if the fallback field type's value is KUidContactFieldNone, the
text is read from the first non-empty field in the item.

A contact database can have a default text definition which is set using CContactDatabase::SetTextDefinitionL().
This may be overridden using the variants of CContactDatabase::ReadContactTextDefL()
which take a CContactTextDef* argument.

CContactTextDef is derived from CArrayFixFlat<TContactTextDefItem>, so all
relevant functions can be used, e.g. to add and remove elements.

@publishedAll
@released
*/
	{
	friend class CContactDatabase;
	friend class CContactTables;
public:
	CContactTextDef();
	IMPORT_C static CContactTextDef* NewL();
	IMPORT_C static CContactTextDef* NewLC();
	static CContactTextDef* NewLC(RReadStream& aStream);
	IMPORT_C void SetFallbackField(TFieldType iFieldType);
	IMPORT_C TFieldType FallbackField() const;
	IMPORT_C TBool ExactMatchOnly();
	IMPORT_C void SetExactMatchOnly(TBool aExactMatchOnly);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);	
private:
	TFieldType iFallbackFieldType;
	TBool iExactMatchOnly;//Default to EFalse cause ReadInTextDef to only match the chosen fields and not attempt a matchall
	};

class CContactItemViewDef : public CBase
/** A view definition for a contact item.

When reading or opening a contact item using the CContactDatabase class,
a view definition may be specified to indicate which field data should be
retrieved. See for instance CContactDatabase::ReadContactL().

A view definition for a contact item contains an array of field types, a use
(CContactItemViewDef::TUse) and a mode (CContactItemViewDef::TMode). The use
indicates whether the field types contained in the view definition should
be included in or excluded from the view. The mode indicates whether fields
with the hidden attribute should be included or excluded.

@publishedAll
@released
*/
	{
public:
	/** Include or exclude specified fields. */
	enum TUse
			{
			/** Include specified fields in the view. */
			EIncludeFields,
			/** Exclude specified fields from the view. */
			EMaskFields
			};
	/** Include or exclude hidden fields.*/
	enum TMode
			{
			/** Include hidden fields in the view. */
			EIncludeHiddenFields,
			/** Exclude hidden fields from the view. */
			EMaskHiddenFields
			};
public:
	IMPORT_C static CContactItemViewDef* NewL(TUse aUse, TMode aMode);
	IMPORT_C static CContactItemViewDef* NewLC(TUse aUse, TMode aMode);
	static CContactItemViewDef* NewLC(RReadStream& aStream);
    inline TUid operator[](TInt aIndex) const;
    IMPORT_C TInt Find(const CContentType &aContentType) const;
    IMPORT_C TInt Find(TFieldType aFieldType) const;
    inline TInt Count() const;
    inline void Reset();
	IMPORT_C void AddL(TFieldType aFieldType);
	IMPORT_C void Remove(TFieldType aFieldType);
	IMPORT_C void Remove(TInt aIndex);
	inline TUse Use() const;
	inline void SetUse(TUse aUse);
	inline TMode Mode() const;
	inline void SetMode(TMode aMode);
	IMPORT_C TBool Contains(const CContactItem& aItem);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TBool MatchesAll() const;
private:
	CContactItemViewDef(TUse aUse,TMode aMode);
private:
    CArrayFixFlat<TUid> iFieldTypes;
	TUse iUse;
	TMode iMode;
	};

class CContactViewDef : public CBase
/** Specifies a subset of fields to be loaded when reading a contact item.

A default view definition is owned by the contact database.
It is set using CContactDatabase::SetViewDefinitionL() and is used in calls
to CContactDatabase::ReadContactL(), ReadContactLC() and ReadContactAndAgentL()
when no view definition is specified.

The view definition owns the item view definition (see the CContactItemViewDef
class), which stores the view definition's field types, use and mode.

The following functions declared in class CContactViewDef have not been
implemented:- Groups(), AddL().

@publishedAll
@released
*/
	{
public:
	IMPORT_C static CContactViewDef* NewL();
	IMPORT_C static CContactViewDef* NewLC();
	IMPORT_C static CContactViewDef* NewL(CContactItemViewDef* aItemDef);
	IMPORT_C static CContactViewDef* NewLC(CContactItemViewDef* aItemDef);
	static CContactViewDef* NewLC(RReadStream& aStream);
	IMPORT_C ~CContactViewDef();
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C CContactItemViewDef& ItemDef() const;
	/** This function is not implemented    */
	IMPORT_C CContactIdArray& Groups() const;
	/** This function is not implemented. 
	@param aFieldType The field type to append to the array of field types. */
	inline void AddL(TFieldType aFieldType);
private:
	CContactViewDef();
	void ConstructL(CContactItemViewDef* aItemDef);
private:
	CContactItemViewDef* iItemDef;
//	CContactIdArray* iGroupIds; // unused for now
	};

class TContactIter
/** Iterates through the sorted contact items in a contact database.

A value of KNullContactId is returned by the iterator if a requested item cannot
be found.

@publishedAll
@released
*/
	{
public:
	IMPORT_C TContactIter(CContactDatabase &aDatabase);
	IMPORT_C TContactItemId FirstL();
	IMPORT_C TContactItemId NextL();
	IMPORT_C TContactItemId PreviousL();
	IMPORT_C TContactItemId LastL();
	IMPORT_C void GotoL(TContactItemId aContactId);
	IMPORT_C void Reset();
private:
	void GotoIndexL(TInt aPos);
private:
	CContactDatabase &iDatabase;
	TContactItemId iCursorId;
	};

class MIdleFindObserver
/** The interface for an asynchronous find observer.

An object which implements this interface is passed to the asynchronous find
functions defined in class CContactDatabase, (FindAsyncL() and FindInTextDefAsyncL()).
The observer would typically notify the user of the progress of the operation.

@publishedAll
@released
*/
	{
public:
	/** Asynchronous find observer callback.

	If an observer is supplied to CContactDatabase::FindAsyncL()
	or FindInTextDefAsyncL(), this callback function is called by CIdle::RunL()
	when nothing of a higher priority can be scheduled. It is called for every
	16 items searched to give the application a chance to update its search status.

	An implementation might test for and handle errors and retrieve information
	about the progress of the search. This information may be retrieved using
	functions provided by the CIdleFinder class. */
	virtual void IdleFindCallback()=0;
	};

struct SFindInTextDefWordParser
/** Search string parser struct.

An object of this type is used in the callback parser function passed to
CContactDatabase::FindInTextDefLC(). The parser function must walk
through and parse iSearchString, adding any words found to iWordArray.

@publishedAll
@released
*/
	{
public:
	inline SFindInTextDefWordParser(const TDesC *aSearchString, CDesCArray *aWordArray) : iSearchString(aSearchString), iWordArray(aWordArray)
	/** Initialises the search string and the word array.

	@param aSearchString Pointer to the input string to be parsed.
	@param aWordArray Pointer to the array to which words parsed from the input
	string should be added. */
		{};
public:
	/** Pointer to the input string to be parsed. */
	const TDesC *iSearchString;
	/** Pointer to the array to which words parsed from the input string should be
	added. */
	CDesCArray *iWordArray;
	};

class CIdleFinder : public CIdle
/** Provides information about the progress of an asynchronous contact database search,
and can be used to get the results of the search.

An instance of this class is returned by calls to CContactDatabase::FindAsyncL()
and CContactDatabase::FindInTextDefAsyncL().

@publishedAll
@released
*/
	{
public:
	~CIdleFinder();
	IMPORT_C TBool IsComplete() const;
	IMPORT_C CContactIdArray *TakeContactIds();
	IMPORT_C TInt Error() const;
	TInt RunError(TInt aError);
private:
	friend class CContactDatabase;
	static CIdleFinder *NewL(CContactDatabase &aContactDatabase, const TDesC& aText, const CContactItemFieldDef *aFieldDef, MIdleFindObserver *aObserver);
	static CIdleFinder *NewL(CContactDatabase &aContactDatabase, const MDesCArray* aFindWords,const CContactTextDef* aTextDef, MIdleFindObserver *aObserver, const TCallBack &aWordParserCallback);
	void DoCancel();
	void Start();
	static TInt CallbackL(TAny *aSelf);
	void ConstructL(const TDesC *aText, const MDesCArray* aFindWords, const TCallBack *aWordParserCallback);
	TBool ScanForMatchL(const TDesC &aFieldText,const TDesC &aFindText, CFoundMatches *aFoundMatches) const;
	TBool CheckFindL(TContactItemId aContactId) const;
	TBool CheckFindL() const;
	CIdleFinder(CContactDatabase &aContactDatabase, const CContactItemFieldDef *aFieldDef, const CContactTextDef* aTextDef, MIdleFindObserver *aObserver);
	TBool doFindL();

	enum TAsyncFindState
		{
		EFindInBlobFinished				=0x00000001,
		EFindInIdentityFinished			=0x00000002,
		EFindInEmailFinished			=0x00000004,
		EFindInTextDefFinished			=0x00000008
		};
private:
	CContactDatabase *iContactDatabase;
	CContactIdArray *iIdsFound;
	TBool iOwnsIds;
	CDesCArray*		iFindWords;
	TInt iReserved1;
	TInt iReserved2;
	TInt iReserved3;
	const CContactItemFieldDef* iFieldDef;
	const CContactTextDef* iTextDef;
	MIdleFindObserver *iObserver;
	TInt iError;
	TCallBack iWordParserCallback;
	TInt iReserved4;
	TInt iReserved5;
	TInt iReserved6;
	TInt iReserved7; //was 	TDbColNo
	TInt iReserved8; //was 	RDbView
	TInt iReserved9; //was 	RDbView
	TInt iReserved10; //was CIdentityTableColSet*
	};

class MContactDbPrivObserver
/** Provides notification of database changes to clients. 
* Provides information about database change event details and the type of changed event.
@publishedAll
@released
*/
	{
public:

#if !( defined __SYMBIAN_CNTMODEL_HIDE_DBMS__ && defined __SYMBIAN_CNTMODEL_USE_SQLITE__ )
	virtual void HandleDatabaseEventL(RDbNotifier::TEvent)=0;
#endif	
	virtual void HandleDatabaseEventL(const TContactDbObserverEvent &aEvent)=0;
	};


class MContactSortObserver
/**
@publishedAll
@deprecated
*/
	{
public:
    /**
    @deprecated
    */
	virtual void HandleSortEventL(TInt aContactsSorted, TInt aContactsTotal)=0;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
class MConverterCallBack
/**
Interface class for providing Intra-Contact Properties during a PBAP specific contacts export.
@released
@internalTechnology
*/
{
	public:
	/**
	PBAP clients are supposed to provide any information regarding the contact item represented by aContactId.
	This information should be in form of standard vCard property, all such properties should be appended to the array aPropertyList.
	*/
	virtual	void AddIntraContactPropertiesL(const TContactItemId& aContactId, CArrayPtr<CParserProperty>* aPropertyList) = 0;
};
#else
class MConverterCallBack;
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

enum TVCardVersion
/**
Specifies type of export of contact items.
PBAP clients should use EPBAPVCard21 and EPBAPVCard30.
@publishedAll
@released
*/
	{
	EVCardUDEF = -1,
	EVCard21 = 0,
	EVCard30,
	EPBAPVCard21,
	EPBAPVCard30
	};

class CContactDatabase : public CBase, public MContactDbPrivObserver, public MContactStorageObserver
/** A database of contact items.

Contact items can be added to and removed from the database and items in the
database can be opened or read. To edit an item, it must first be opened.
Changes to the item can then be committed using CommitContactL()
(committing can only take place to an open item).

If the available disk space is less than 128 kilobytes then changes to the
contact database cannot be started, and methods such as CreateL() or CommitContactL()
will leave with a KErrDiskFull error. Less obviously OpenL() can also fail with
KErrDiskFull when trying to update the system template.

Each item has an access count. An item cannot be fully deleted unless its
access count is zero. This is to prevent items which have been
synchronised with another contacts application from being deleted.
Also, if the item is a template, it cannot be deleted if other items are based
on it.

More than one contact database can exist on a phone, but there is a
default contact database. Its filename is contacts.cdb. It
can be moved between drives.

CContactDatabase implements searching and sorting and also provides
separate phone number matching functions. These provide a fast way of indexing
and looking up phone numbers, e.g. for looking up caller IDs on incoming
calls.

The database owns speed dial information. This is a mapping between a single
digit integer and a telephone number. There may be up to 9 speed dial telephone
numbers; each of them belongs to a contact item in the database. If a
contact item is deleted, its speed dial mapping is removed.

The following functions defined in class CContactDatabase are not
supported. If called, they leave with KErrNotSupported.

- ReplacePrefL()

- ReadPrefL()

- CreatePrefL()

@publishedAll
@released
*/
	{
	friend class TContactIter;
	friend class CIdleFinder;
	friend class CCntIdleSorter;
	friend class CContactLocalView;
	friend class CContactOpenOperation;
	friend class RContactRemoteView;
	friend class CViewIteratorProxyManager;
	friend class CCollectionProxy;
	friend class CContactActiveBase;
public:
	class TSortPref
	/** Specifies the field type and order preferences used for sorting items in the
	contacts database.

	The contents of the first field in a contact item matching the field type
	is used as the data to sort on. The order can either be ascending or descending.

	@see CContactDatabase::SortL()
	@see CContactDatabase::SortArrayL()
	@publishedAll
	@deprecated
	*/
		{
	public:
		/** The sort order. */
		enum TOrder
			{
			/** Ascending sort order. */
			EAsc,
			/** Descending sort order. */
			EDesc
			};
	public:
		inline TSortPref();
		inline TSortPref(TFieldType aFieldType,TOrder aOrder=EAsc);
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		IMPORT_C void InternalizeL(RReadStream& aStream);
	public:
		/** Specifies whether the sort should be ascending or descending. */
		TOrder iOrder;
		/** The first field matching this field type is used for the text to sort on. */
		TFieldType iFieldType;
		};

	/** Import and export format flags. Contacts can be imported into
	or exported from the contact database as vCards.

	The following options are available during import and export.

	@deprecated This enum is deprecated. It has been replaced by the TOptions enum in
	the plugin convertor class CContactVCardConverter, so that each plugin converter
	can define its own options. */
	enum TOptions
  		{
  		EDefault=0x0000,
		/** Handle Symbian's extended vCard format options. */
		EIncludeX =0x0001,
		/** Support non-standard extensions requried by Symbian PC connectivity software
		and Microsoft, amongst others. */
  		ETTFormat =0x0002,
		/** Don't export the contact ID (for exporting only). */
		EExcludeUid=0x0004,
		/** Decrease the contact's access count when importing and exporting. */
		EDecreaseAccessCount=0x0008,
		/** Only import the first contact in the read stream (for importing only). */
		EImportSingleContact=0x0010,
		/** Increase the contact's access count when importing and exporting. */
		EIncreaseAccessCount=0x0020,
		/** Sets a contact item to the local time when importing. */
		ELocalTime=0x0040,
		/** Allows you to set a template to a contact item that does not exist. */
		ENullTemplateId=0x0080,
		/** For use by implementor of converter plug-in. */
		EConverterDefined1=0x0100,
		/** For use by implementor of converter plug-in. */
		EConverterDefined2=0x0200,
		/** For use by implementor of converter plug-in. */
		EConverterDefined3=0x0400,
		/** For use by implementor of converter plug-in. */
		EConverterDefined4=0x0800,
		/** For use by implementor of converter plug-in. */
		EConverterDefined5=0x1000,
		/** For use by implementor of converter plug-in. */
		EConverterDefined6=0x2000,
		/** For use by implementor of converter plug-in. */
		EConverterDefined7=0x4000,
		/** For use by implementor of converter plug-in. */
		EConverterDefined8=0x8000,
		/** Reserved for use by Symbian. */
		EConverterReserved1=0x00010000, 
		/** Reserved for use by Symbian. */
		EConverterReserved2=0x00020000, 
		/** Reserved for use by Symbian. */
		EConverterReserved3=0x00040000, 
		/** Reserved for use by Symbian. */
		EConverterReserved4=0x00080000, 
		/** Reserved for use by Symbian. */
		EConverterReserved5=0x00100000, 
		/** Reserved for use by Symbian. */
		EConverterReserved6=0x00200000, 
		/** Reserved for use by Symbian. */
		EConverterReserved7=0x00400000, 
		/** Reserved for use by Symbian. */
		EConverterReserved8=0x00800000, 
		/** Reserved for use by Symbian. */
		EConverterReserved9=0x01000000, 
		/** Reserved for use by Symbian. */
		EConverterReserved10=0x02000000, 
		/** Reserved for use by Symbian. */
		EConverterReserved11=0x04000000, 
		/** Reserved for use by Symbian. */
		EConverterReserved12=0x08000000, 
		/** Reserved for use by Symbian. */
		EConverterReserved13=0x10000000, 
		/** Reserved for use by Symbian. */
		EConverterReserved14=0x20000000, 
		/** Reserved for use by Symbian. */
		EConverterReserved15=0x40000000, 
		/** Reserved for use by Symbian. */
		EConverterReserved16=0x80000000 
  		};
	/** Specifies whether the database can be accessed from single or multiple threads. */
	enum TThreadAccess
		{
		/** Allows access to the contacts database from a single thread only. */
		ESingleThread,
		/** Allows for a multi-threaded program to access the contact database from multiple
		threads but note that the contacts model provides no synchronisation support
		for such use. */
		EMultiThread
		};
		
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
/**
@internalComponent
*/
	enum TSortDataSource
		{
		ENoOrder,
		EUsesIdentityTableOnly,
		EUsesAllTables,
		EIdentityTableNotUsed //Not used
		};
#endif
public:
	IMPORT_C static CContactDatabase* OpenL(TThreadAccess aAccess=ESingleThread);
	IMPORT_C static CContactDatabase* OpenL(const TDesC& aFileName,TThreadAccess aAccess=ESingleThread);
	IMPORT_C static CContactDatabase* CreateL(TThreadAccess aAccess=ESingleThread);
	IMPORT_C static CContactDatabase* CreateL(const TDesC& aFileName,TThreadAccess aAccess=ESingleThread);
	IMPORT_C static CContactDatabase* ReplaceL(TThreadAccess aAccess=ESingleThread);
	IMPORT_C static CContactDatabase* ReplaceL(const TDesC& aFileName,TThreadAccess aAccess=ESingleThread);
	// asynchronous Open
	IMPORT_C static CContactOpenOperation* Open(TRequestStatus& aStatus, TThreadAccess aAccess=ESingleThread);
	IMPORT_C static CContactOpenOperation* Open(const TDesC& aFileName, TRequestStatus& aStatus, TThreadAccess aAccess=ESingleThread);
	IMPORT_C static TBool DatabaseDrive(TDriveUnit &aDriveUnit);
	inline static TInt NullUidValue();
	IMPORT_C static void SetDatabaseDriveL(TDriveUnit aDriveUnit, TBool aCopy=ETrue);
	IMPORT_C static void GetDefaultNameL(TDes &aDes);
	IMPORT_C static void DeleteDefaultFileL();
	// database file mangement
	IMPORT_C static void DeleteDatabaseL(const TDesC& aFileName);
	IMPORT_C static CDesCArray* ListDatabasesL();
	IMPORT_C static CDesCArray* ListDatabasesL(TDriveUnit aDriveUnit);
	IMPORT_C static TBool DefaultContactDatabaseExistsL();
	IMPORT_C static TBool ContactDatabaseExistsL(const TDesC& aFileName);
	IMPORT_C ~CContactDatabase();

	IMPORT_C void SetViewDefinitionL(CContactViewDef* aView);
	IMPORT_C void SetTextDefinitionL(CContactTextDef* aView);
	IMPORT_C const CContactTextDef* TextDefinition() const;
	IMPORT_C TInt CountL();
	IMPORT_C void SetDbViewContactType(const TUid aUid);
	IMPORT_C TUid GetDbViewContactType() const;


	IMPORT_C TContactItemId AddNewContactL(CContactItem& aContact);

	IMPORT_C CContactItem* CreateContactGroupL(TBool aInTransaction=EFalse);
	IMPORT_C CContactItem* CreateContactGroupLC(TBool aInTransaction=EFalse);
	IMPORT_C CContactItem* CreateContactGroupL(const TDesC& aGroupLabel,TBool aInTransaction=EFalse);
	IMPORT_C CContactItem* CreateContactGroupLC(const TDesC& aGroupLabel,TBool aInTransaction=EFalse);
	IMPORT_C CContactItem* CreateContactCardTemplateL(const TDesC& aTemplateLabel,TBool aInTransaction=EFalse);
	IMPORT_C CContactItem* CreateContactCardTemplateLC(const TDesC& aTemplateLabel,TBool aInTransaction=EFalse);
	IMPORT_C CContactItem* CreateContactCardTemplateL(const CContactItem* aTemplate,const TDesC& aTemplateLabel,TBool aInTransaction=EFalse);
	IMPORT_C CContactItem* CreateContactCardTemplateLC(const CContactItem* aTemplate,const TDesC& aTemplateLabel,TBool aInTransaction=EFalse);

	IMPORT_C CContactIdArray* GetCardTemplateIdListL() const;
	IMPORT_C CContactIdArray* GetGroupIdListL() const;
	IMPORT_C void AddContactToGroupL(TContactItemId aItemId, TContactItemId aGroupId);
	IMPORT_C void AddContactToGroupL(CContactItem& aItem, CContactItem& aGroup);
	IMPORT_C void AddContactToGroupL(TContactItemId aItemId, TContactItemId aGroupId,TBool aInTransaction);
	IMPORT_C void RemoveContactFromGroupL(CContactItem& aItem, CContactItem& aGroup);
	IMPORT_C void RemoveContactFromGroupL(TContactItemId aItemId, TContactItemId aGroupId);
	inline TInt GroupCount() const;
	inline TInt TemplateCount() const;

	// speed dial functions
	IMPORT_C void SetFieldAsSpeedDialL(CContactItem& aItem, TInt aFieldIndex, TInt aSpeedDialPosition);
	IMPORT_C TContactItemId GetSpeedDialFieldL(TInt aSpeedDialPosition, TDes& aPhoneNumber);
	IMPORT_C void RemoveSpeedDialFieldL(TContactItemId aContactId, TInt aSpeedDialPosition);

	// reading contacts from the db
	IMPORT_C CContactItem* ReadMinimalContactL(TContactItemId aContactId);
	IMPORT_C CContactItem* ReadMinimalContactLC(TContactItemId aContactId);
	IMPORT_C CContactItem* ReadContactL(TContactItemId aContactId);
	IMPORT_C CArrayPtr<CContactItem>* ReadContactAndAgentL(TContactItemId aContactId);
	IMPORT_C CContactItem* ReadContactL(TContactItemId aContactId,const CContactItemViewDef& aViewDef);
	IMPORT_C CContactItem* ReadContactLC(TContactItemId aContactId);
	IMPORT_C CContactItem* ReadContactLC(TContactItemId aContactId,const CContactItemViewDef& aViewDef);
	IMPORT_C void ReadContactTextDefL(const CContactItem &aItem, TDes &aResult);
	IMPORT_C void ReadContactTextDefL(const CContactItem &aItem, TDes &aResult,CContactTextDef* aTextDef);
	IMPORT_C void ReadContactTextDefL(TContactItemId aContactId, TDes &aResult);
	IMPORT_C void ReadContactTextDefL(TContactItemId aContactId, TDes &aResult,CContactTextDef* aTextDef);
	IMPORT_C CContactItem* OpenContactL(TContactItemId aContactId);
	IMPORT_C CContactItem* OpenContactL(TContactItemId aContactId,const CContactItemViewDef& aViewDef);
	IMPORT_C CContactItem* OpenContactLX(TContactItemId aContactId);
	IMPORT_C CContactItem* OpenContactLX(TContactItemId aContactId,const CContactItemViewDef& aViewDef);
	IMPORT_C void CloseContactL(TContactItemId aContactId);
	IMPORT_C void CommitContactL(const CContactItem& aContact);
	IMPORT_C void DeleteContactL(TContactItemId aContactId);
	IMPORT_C void DeleteContactsL(const CContactIdArray& aContactIds);
  	IMPORT_C CArrayPtr<CContactItem>* ImportContactsL(const TUid& aFormat,RReadStream& aReadStream,TBool& aImportSuccessful,TInt aOption);
  	IMPORT_C void ExportSelectedContactsL(const TUid& aFormat,const CContactIdArray& aSelectedContactIds,RWriteStream& aWriteStream,TInt aOption,TBool aExportPrivateFields=ETrue);
  	IMPORT_C void ExportSelectedContactsL(const TUid& aFormat,const CContactIdArray& aSelectedContactIds,RWriteStream& aWriteStream,TInt aOption,const Versit::TVersitCharSet aCharSet, TBool aExportPrivateFields=ETrue);
	IMPORT_C void CompactL();

	IMPORT_C CContactIdArray* FindLC(const TDesC& aText,const CContactItemFieldDef *aFieldDef);
	IMPORT_C CIdleFinder* FindAsyncL(const TDesC& aText,const CContactItemFieldDef *aFieldDef, MIdleFindObserver *aObserver);
	IMPORT_C CContactIdArray* FindInTextDefLC(const MDesCArray& aFindWords, const TCallBack &aWordParserCallback);
	IMPORT_C CContactIdArray* FindInTextDefLC(const MDesCArray& aFindWords,CContactTextDef* aTextDef, const TCallBack &aWordParserCallback);
	IMPORT_C CIdleFinder* FindInTextDefAsyncL(const MDesCArray& aFindWords, MIdleFindObserver *aObserver, const TCallBack &aWordParserCallback);
	IMPORT_C CIdleFinder* FindInTextDefAsyncL(const MDesCArray& aFindWords,const CContactTextDef* aTextDef, MIdleFindObserver *aObserver, const TCallBack &aWordParserCallback);
	IMPORT_C CContactIdArray* SortArrayL(const CContactIdArray* aIdArray, const CArrayFix<TSortPref>* aSortOrder);
	IMPORT_C CContactIdArray* SortArrayLC(const CContactIdArray* aIdArray, const CArrayFix<TSortPref>* aSortOrder);
	IMPORT_C void SortByTypeL(CArrayFix<TSortPref>* aSortOrder);
	IMPORT_C void SortL(CArrayFix<TSortPref>* aSortOrder);
	IMPORT_C void SortAsyncL(CArrayFix<TSortPref>* aSortOrder, TRequestStatus& aStatus);
	IMPORT_C void CancelAsyncSort();
	IMPORT_C void SortAsyncL(CArrayFix<TSortPref>* aSortOrder, TRequestStatus& aStatus, MContactSortObserver& aObserver);

	IMPORT_C void SetOperationTimeOutL(const TInt aMicroSeconds) const;
  	IMPORT_C TPtrC FileUid();
	IMPORT_C CContactActiveCompress* CreateCompressorLC();
	IMPORT_C CContactActiveRecover* CreateRecoverLC();
	IMPORT_C void RecoverL();
	IMPORT_C TBool IsDamaged() const;
	IMPORT_C TBool CompressRequired();
	IMPORT_C void CloseTables();
	IMPORT_C void OpenTablesL();

	//New Phone Matching function, takes a number as a TDesc& and will match from the right of the string
	//by the the number defined
	//use KBestMatchingPhoneNumbers as second parameter to enable best matching number strategy
	IMPORT_C CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, TInt aMatchLengthFromRight);


	IMPORT_C const CContactIdArray* SortedItemsL();
	IMPORT_C TContactItemId TemplateId() const;
	IMPORT_C CContactItem* CreateOwnCardLC();
	IMPORT_C CContactItem* CreateOwnCardL();
	IMPORT_C TContactItemId OwnCardId() const;
	IMPORT_C void SetOwnCardL(const CContactItem& aContact);

//	Preferred Template
	IMPORT_C TContactItemId PrefTemplateId() const;
	IMPORT_C void SetPrefTemplateL(const CContactItem& aContact);

//	Current item
	IMPORT_C void SetCurrentItem(const TContactItemId aContactId);
	IMPORT_C TContactItemId GetCurrentItem() const;

//	Connectivity
	IMPORT_C void SetDateFormatTextL(const TDesC& aFormat);
	IMPORT_C void FilterDatabaseL(CCntFilter& aFilter);
	IMPORT_C CContactIdArray* ContactsChangedSinceL(const TTime& aTime);
	IMPORT_C TContactSyncId SetLastSyncDateL(const TTime& aSyncDate);
	IMPORT_C void SetLastSyncDateL(TContactSyncId aSyncId, const TTime& aSyncDate);
	IMPORT_C void GetLastSyncDateL(TContactSyncId aSyncId, TTime& aSyncDate);

	IMPORT_C TInt FileSize() const;
	IMPORT_C TInt WastedSpaceInBytes() const;
	IMPORT_C TUint ConnectionId() const;
	IMPORT_C const CContentType &TemplateContentType(const CContactItemField &aField) const;
	IMPORT_C TVersion Version() const;
	IMPORT_C TInt64 MachineId() const;
	IMPORT_C TContactItemId ICCTemplateIdL();
	IMPORT_C TContactItemId ICCTemplateIdL(TUid aPhonebookUid);
	IMPORT_C TContactItemId PhonebookGroupIdL();

public:	// For test code only
	IMPORT_C void DamageDatabaseL(TInt aSecretCode);	// Don't use this, you don't really want to damage your database do you?
	IMPORT_C void OverrideMachineUniqueId(TInt64 aMachineUniqueId);
	IMPORT_C TInt CntServerResourceCount();
	IMPORT_C void SetCntServerHeapFailure(TInt aTAllocFailType,TInt aRate);
	IMPORT_C CContactIdArray* DeletedContactsLC();
	IMPORT_C void ResetServerSpeedDialsL();

public:	// For cnt server only
#if !( defined __SYMBIAN_CNTMODEL_HIDE_DBMS__ && defined __SYMBIAN_CNTMODEL_USE_SQLITE__ )
			 void HandleDatabaseEventL(RDbNotifier::TEvent) {};
#endif
	IMPORT_C void HandleDatabaseEventL(const TContactDbObserverEvent &aEvent);
	IMPORT_C TInt GetCurrentDatabase(TDes& aDatabase) const;
	IMPORT_C TInt SetCurrentDatabase(const TDesC& aDatabase) const;

	IMPORT_C void StoreSortOrderL();
	IMPORT_C void RestoreSortOrderL();
	IMPORT_C const CArrayFix<TSortPref>* SortOrder() const;
	//Contact Hint Field Access

	/** Contact view filter flags.

	These flags define the bits that can be set in filtered views (CContactFilteredView)
	and in the filter passed to CContactDatabase::ContactMatchesHintFieldL() for
	comparing against contact view items (CViewContact).

	If the item's bit field and the view's filter have the same settings for the
	CContactDatabase::EHome and CContactDatabase::EWork flags and any of the other
	flags are set in both, a match occurs and the item is included in the view.

	Filters only apply to non-hidden contact fields with content. */
	enum TContactViewFilter
		{
		/** The view is unfiltered; it includes all contact items. */
		EUnfiltered	=0x00000000,
		/** The filter includes items that have an email address. */
		EMailable	=0x00000001,
		/** The filter includes items that have a mobile telephone number. */
		ESmsable	=0x00000002,
		/** The filter includes items that have a landline telephone number. */
		ELandLine	=0x00000004,
		/** The filter includes items that have a fax number. */
		EFaxable	=0x00000008,
		/** The filter includes items that are phonable. (All items with any of the above
		three flags set also have this flag set). */
		EPhonable   =0x00000010,
		/** The filter excludes items without a work telephone number or email address. */
		EWork		=0x00000020,
		/** The filter excludes items without a home telephone number or email address. */
		EHome		=0x00000040,
		/** The filter includes items that have a non-empty ring tone field (a ring tone
		that is associated with the  item). */
		ERingTone	=0x00000080,
		/** The filter includes items that have a non-empty voice dial field (a voice
		recording associated with a telephone number field in the  item). */
		EVoiceDial	=0x00000100,
		/** The filter includes items that have any non empty instant messaging address field. */
		EIMAddress  =0x00000200,
		/** The filter includes items that have a non empty Wireless Village ID field.
		An item with this flag set will also have the EIMAddress flag set. */
		EWirelessVillage = 0x00000400,
		/** Reserved filters for future use. */
		ECustomFilter1	 = 0x00000800,
		/** Reserved filters for future use. */
		ECustomFilter2	 = 0x00001000,
		/** Reserved filters for future use. */
		ECustomFilter3	 = 0x00002000,
		/** Reserved filters for future use. */
		ECustomFilter4	 = 0x00004000
		};

	IMPORT_C TBool ContactMatchesHintFieldL(TInt aBitWiseFilter,TContactItemId aContactId);
public: // for vCard converter only
	IMPORT_C void DatabaseBeginLC(TBool aIsInTransaction);
	IMPORT_C void DatabaseCommitLP(TBool aIsInTransaction);
  	IMPORT_C CContactItem *UpdateContactLC(TContactItemId aContactId,CContactItem* aNewContact);
	IMPORT_C TContactItemId doAddNewContactL(CContactItem& aContact,TBool aIsTemplate,TBool aIsInTransaction);
	IMPORT_C void doCommitContactL(const CContactItem& aContact,TBool aIsInTransaction, TBool aSendChangedEvent);
	IMPORT_C TContactItemId ContactIdByGuidL(const TDesC& aGuid);
	IMPORT_C void doDeleteContactL(TContactItemId aContactId, TBool aIsInTransaction, TBool aSendChangedEvent, TBool aDecAccessCount=EFalse);
	IMPORT_C CContactItemViewDef* AllFieldsView();
public: // For Symbian use only
	IMPORT_C void DatabaseBeginL(TBool aIsInTransaction);
	IMPORT_C void DatabaseCommitL(TBool aIsInTransaction);
	IMPORT_C void DatabaseRollback();
	IMPORT_C void SetSortedItemsList(CContactIdArray* aSortedItems, CArrayFix<TSortPref>* aSortOrder);
	IMPORT_C CContactIdArray* UnfiledContactsL();
	IMPORT_C static void RecreateSystemTemplateL(const TDesC& aFileName);
public:	// For Contacts Lock Server use only
	IMPORT_C static CContactDatabase* LockServerConnectL(const TDesC& aFileName);
	IMPORT_C static CContactDatabase* LockServerConnectL(const TDesC& aFileName, TInt aOperation);
	IMPORT_C TInt LockServerCallBackL(TUint aServerOperation);
	IMPORT_C void LockServerCleanup();
public:
	IMPORT_C void ExportSelectedContactsL(const TUid& aFormat, const CContactIdArray& aSelectedContactIds, RWriteStream& aWriteStream, TInt aOption, const TInt64 aContactFieldFilter, MConverterCallBack* aCallback, const TVCardVersion aVersion, const TBool aExportTel, Versit::TVersitCharSet aCharSet = Versit::EUTF8CharSet, TBool aExportPrivateFields = ETrue);
private:
	//for cntiter
	TInt ContactPosL(TContactItemId aContactId); 
	TInt DoGotoL(TContactItemId aContactId);
	
	void DoReadContactTextDefL(const CContactItemFieldSet* aFieldSet,TDes& aResult,CContactTextDef* aTextDef);
	CContactItem* OpenNoMergeLCX(TContactItemId aContactId);

	void FetchGroupAndTemplateListsL();

public:
	void CancelNotifyRequestL();
	TBool IsICCSynchronisedL();

	void OpenDatabaseAsyncL(TRequestStatus& aStatus, const TDesC& aFileName = KNullDesC);
	static void CleanupLastLockedContact(TAny *aDatabase);

private:
	CContactConverter& ConverterL(const TUid& aFormat);
	CContactConverter& ConverterL(const TUid& aFormat, const TInt64 aContactFieldFilter, MConverterCallBack* aCallback, const TVCardVersion aVersion,const TBool aExportTel);
	static void CleanupDatabaseRollback(TAny *aDatabase);

	CContactIdArray* SortLC(const CArrayFix<TSortPref>* aSortOrder, const CContactIdArray* aIdArray);
	TBool CheckType(TUid aUid) const;
	void SortDuplicatesL(const CArrayFix<TSortPref>& aSortOrder,CSortArray& aList,TInt aPos);
	void SortDuplicatesL(const CArrayFix<TSortPref>& aSortOrder,CSortArray& aList,
								TInt aIndex,TInt aStartPos,TInt aEndPos);
	TBool AddContactToSortListL(TContactItemId aReqId, TContactItemId& aActualId,CBase* aItems, TUid& aFieldType, TBool aHasSortOrder);
	void RemoveFromSortArray(TContactItemId aContactId);
	void HandleDbObserverEventGroupDeletedL(const TContactItemId aContactId);
	void RemoveFromGroupIds(const TContactItemId aContactId);

public:
	static void CleanupTableCancel(TAny *aTable);
/** A text buffer of KTextFieldMinimalLength used during sorting and searching of contacts  */
	typedef TBuf<KTextFieldMinimalLength> TTextFieldMinimal;
	static TUid SpeedDialFieldUidFromSpeedDialPosition(TInt aSpeedDialPosition);
	void HandleDiskSpaceEvent(TInt aDrive);

	/** State of database connection
	*/
	enum TDbConnState
		{
		/** Database is open, connection is available for use. */
		EDbConnectionOpen = 0,
		/** Initial state. */
		EDbConnectionNotReady,
		/** Asynchronous Open failed. */
		EDbConnectionFailed,
		/** A Rollback has occurred and a Recover is required now. */
		EDbConnectionRecoverRequired,
		/** Write access is locked during backup. */
		EDbConnectionWriteLockedForBackup,
		/** The database has been closed whilst a Restore is in progress. */
		EDbConnectionClosedForRestore,
		/** Restore is in progress need to close after current transaction. */
		EDbConnectionNeedToCloseForRestore,
		};
	/** Currently unused    */
	enum TDirection {EAsc,EDesc}; //unused
	void SetDbConnectionState(TDbConnState aDbConnectionState);
private:
	void CheckDbConnForWriteL() const;
	void CheckDbConnForReadL() const;
	void CheckDbConnForRecoverL() const;
	void CheckTemplateField(CContactItem& aCnt);
	static void ValidateDatabaseNameL(const TParse& aParseName);
	void StartAsyncActivityL();
	void EndAsyncActivityL();
	TBool DatabaseReadyL() const;
	enum TSvrSessionType //unused
		{  
		// Server Session is persistent, for Open(L), CreateL or ReplaceL
		ESvrSessionPersistent,
		// Server Session is temporary, for a simple static API
		ESvrSessionTemporary,
		// Session is from the Contacts Lock Server
		ESvrSessionFromLockServer,
		};
private: // objec construction/destruction

	CContactDatabase();
	static CContactDatabase* NewLC();
	void ConstructL();
	void CreateViewDefL();
	MLplPersistenceLayerFactory* FactoryL();

private:
	CContactItem* doCreateContactGroupLC(const TDesC& aGroupLabel = KNullDesC);
	void AddCntToOpenedGroupL(TContactItemId aItemId, CContactItem& aGroup);
	void ReadTemplateIdsL();
	void AddToTemplateListL(const TContactItemId aNewTemplateId);
	void RemoveFromTemplateList(const TContactItemId aOldTemplateId);
	TBool SystemTemplateFieldsValid(const CContactItem& aContact);
	void RespondToEventL(const TContactDbObserverEventType aEventType, const TContactItemId aContactId);
	void HandleDbObserverEventGroupOrContactChangedL(const TContactItemId aContactId);
	void HandleDbObserverEventGroupOrContactAddedL(const TContactDbObserverEventType aEventType, const TContactItemId aContactId);
   	TBool CheckSortError(TInt aError);
   	TInt NextExistingL(TInt aIndex);
   	TInt PreviousExistingL(TInt aIndex);
   	void ReSortL(CArrayFix<TSortPref>* aSortOrder);
   	void InsertInSortArray(const CContactItem& aContact);
   	void MoveInSortArray(const CContactItem& aContact);
   	void InsertInSortArrayL(const CContactItem& aContact);
   	void MoveInSortArrayL(const CContactItem& aContact);
	TInt NewSortIndexL(const CContactItem& aContact,TInt aStartPos,TInt aEndPos);
	TInt CompareSortFieldsL(const CContactItem& aContact);
   	void ConstructTableUsageFlagsFromSortOrderL(TInt& aFlags);
   	void LoadSyncPluginL();
   	void DeleteContactSendEventActionL(TContactItemId aContactId, TCntSendEventAction aActionType);

public:
   	//for LocalView
	void IMPORT_C AddObserverL(MContactDbObserver& aChangeNotifier);
	void IMPORT_C RemoveObserver(const MContactDbObserver& aChangeNotifier);
	
	void IMPORT_C AddObserverV2L(MContactDbObserverV2& aChangeNotifier);
	void IMPORT_C RemoveObserverV2(const MContactDbObserverV2& aChangeNotifier);
   	
private:
	// Member variables
	RCntModel*					iCntSvr;			//was RDbNamedDatabase iDatabase;
	CProxyFactory*				iProxyFactory;		//was RDbs iDbsSession;
	TDbConnState				iDbConnectionState;	//was CContactClientSession* iContactClientSession;
	CDataBaseChangeObserver*	iDataBaseObserver;	//was CPrivateDbChangeNotifier* iDbChangeNotifier;
	CCntIdleSorter*				iIdleSorter;		//was CPrivateSvrSessionManager* iServerSessionManager;
	CPrivConverter*				iConv;				//was TContactItemId iLastLockedContactId;
	TBool						iTablesOpen;		//was TContactItemId iTemplateId;
	CContactItemViewDef*		iAllFieldsView;		//was TContactItemId iOwnCardId;
	TUid						iDbViewContactType;	//used to select which contact type to sort / search on
	
	//These two members are accessed via the inline functions!
	CContactIdArray* iCardTemplateIds;
	CContactIdArray* iGroupIds;

	CCntTemplateCache* iTemplateCache; //was CContactTables* iItemTable;
	CContactTextDef* iTextDef;		//remains
	CContactIdArray* iSortedItems;	//remains
	CContactViewDef* iView;			//remains
	
	// Used in SetLastSyncDateL() and GetLastSyncDateL()
	TTime iSyncDate;	//was TInt64 iMachineUniqueId;
	TCollationMethod iCollateMethod; //remains: used for sorting contacts

	TInt iAsyncActivityCount;
	CContactSynchroniser* iContactSynchroniser; //a handle to the synchroniser plugin
	CArrayFix<TSortPref>* iSortOrder;	// holds a sort order passed into SortL(), as in cntmodelv1, 
										// for delayed deletion to maintain backwards compatibility
	};


class CContactChangeNotifier : public CBase
/** Receives events reporting changes to a contact database.

After the change notifier has been created, it notifies the specified
observer whenever a change takes place to the database.

@publishedAll
@released
*/
	{
public:
	IMPORT_C ~CContactChangeNotifier();
	IMPORT_C static CContactChangeNotifier* NewL(CContactDatabase& aDatabase, MContactDbObserver *aObserver);
private:
	CContactChangeNotifier(CContactDatabase& aDatabase, MContactDbObserver *aObserver);
	void ConstructL();
private:
	CContactDatabase &iDatabase;
	MContactDbObserver* iObserver;
	};


/** The UID of the default vCard converter implemented by an ECom plugin.
@publishedAll
@released
*/
#define KUidEComCntVCardConverterDefaultImplementation  0x102035F9
/** The UID of the default vCard converter plugin implementation. This
implementation is independent of the plugin framework used. 
@publishedAll
@released
*/
#define KUidVCardConvDefaultImpl 	KUidEComCntVCardConverterDefaultImplementation

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS

/** The UID of the vCard converter ECom plugin interface. 
@released
@internalTechnology
*/
const TUid KUidEcomCntVCardConverterInterface = {0x102035F7};

/** The UID of the phone number parser ECom plugin interface. 
@released
@internalTechnology
*/
const TUid KUidEcomCntPhoneNumberParserInterface = {0x102035FA};

/** The UID of PBAP vCard Converter plugin Implementation.
@internalTechnology
@released
*/
#define KUidPBAPVCardConvImpl 0xA00015C1

#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

class TPluginParameters
/**
Class used to pack the extra arguments required for a PBAP conveter,
PBAP client provides these arguments using overloaded CContactDatabase::ExportSelectedContacts.
@publishedAll
@released
*/
{
public:
	TPluginParameters(TInt64 aContactFieldFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel);
	TInt64 GetFilter()const;
	MConverterCallBack* GetCallback()const;
	TVCardVersion GetExportVersion()const;
	TBool IsExportTel()const;
private:
	TInt64 iFilter;
	MConverterCallBack* iCallback;
	TVCardVersion iVersion;
	TBool iExportTel;
};

inline TBool TPluginParameters::IsExportTel()const
	{
	return iExportTel;
	}
inline TInt64 TPluginParameters::GetFilter()const
	{
	return iFilter;
	}

inline MConverterCallBack* TPluginParameters::GetCallback()const
	{
	return iCallback;
	}

inline TVCardVersion TPluginParameters::GetExportVersion()const
	{
	return iVersion;
	}

inline	TPluginParameters::TPluginParameters(TInt64 aContactFieldFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel):iFilter(aContactFieldFilter), iCallback(aCallback), iVersion(aVersion), iExportTel(aExportTel)
	{
	}

class CContactConverter : public CBase
/** Provides functionality to import and export vCards.
One or more vCards can be imported from a read stream (the vCards are converted 
into contact items and added to the database). Also, contact items can be exported as vCards.
@publishedAll
@released
*/
	{
public:
	virtual CArrayPtr<CContactItem>*  ImportL(CContactDatabase& aDb,RReadStream& aReadStream,TBool& aImportSuccessful,TInt aOptions,TBool aImportSingle)=0;
	virtual void ExportL(CContactDatabase& aDb,const CContactIdArray& aSelectedContactIds,RWriteStream& aWriteStream,TInt aOptions,const Versit::TVersitCharSet aCharSet,TBool aExportPrivateFields, TInt aCommitNumber=10)=0;
	};


class CContactEcomConverter : public CContactConverter
/**
It provides Ecom Framework based facilities to resolve and load the appropriate implementations at run-time.
The framework supplies a default resolver for selecting appropriate implementations.
@publishedAll
@released
*/
	{
public:
	static CContactEcomConverter* NewL(TUid aImplementationUid);
	static CContactEcomConverter* NewL(TUid aImplementationUid, TInt64 aContactFieldFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel);
	inline virtual ~CContactEcomConverter();
private:
	TUid iDtor_ID_Key;
	};

inline CContactEcomConverter* CContactEcomConverter::NewL(TUid aImplementationUid)
/** Allocates and instantiates an interface implementation to satisfy the specified interface.
@param aImplementationUid Denotes the type of implemetation that needs to be loaded.
@return A pointer to a newly allocated and initialised object of type CContactEcomConverter.   */
	
{
	TAny* ptr = REComSession::CreateImplementationL(aImplementationUid,
													_FOFF(CContactEcomConverter,
														  iDtor_ID_Key));
	return reinterpret_cast<CContactEcomConverter*>(ptr);
}

inline CContactEcomConverter* CContactEcomConverter::NewL(TUid aImplementationUid, TInt64 aContactFieldFilter, MConverterCallBack* aCallback, TVCardVersion aVersion, TBool aExportTel)
/** Allocates and instantiates an interface implementation for PBAP export.
@param aImplementationUid Denotes the type of implemetation that needs to be loaded.
@param aContactFieldFilter Specifies contact fields to be exported.
@param aCallback Callback to client for providing intra-contact properties.
@param aVersion Denotes the vCard version for contact export.
@return A pointer to a newly allocated and initialised object of type CContactEcomConverter.   */
	
{	
	TPluginParameters partr(aContactFieldFilter,aCallback,aVersion,aExportTel);
	
	TAny* ptr = REComSession::CreateImplementationL(aImplementationUid,
													_FOFF(CContactEcomConverter,
													  iDtor_ID_Key), &partr);
	return reinterpret_cast<CContactEcomConverter*>(ptr);
}

inline CContactEcomConverter::~CContactEcomConverter()
/** The destruction of the interface implementation referred to by iDtor_ID_Key */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
class CContactPhoneNumberParser : public CBase
/** Provides functionality to extract the real phone number from a contact's phone number field.
@publishedPartner
@released
*/
	{
public:
	virtual void ExtractRawNumber(const TDesC& aTextualNumber, TDes& aRawNumber)=0;
	};


class CContactEcomPhoneNumberParser : public CContactPhoneNumberParser
/** It provides Ecom Framework based facilities to resolve and load the appropriate implementations at run-time.
The framework supplies a default resolver for selecting appropriate implementations.
@publishedPartner
@released
*/
	{
public:
	static CContactEcomPhoneNumberParser* NewL(TUid aImplementationUid);
	inline virtual ~CContactEcomPhoneNumberParser();
private:
	TUid iDtor_ID_Key;
	};

inline CContactEcomPhoneNumberParser* CContactEcomPhoneNumberParser::NewL(TUid aImplementationUid)
/** Allocates and Instantiates an interface implementation to satisfy the specified interface. 
@param aImplementationUid denotes the type of implemetation that need to be loaded.
@return A pointer to a newly allocated and initialised object of type CContactEcomPhoneNumberParser.   */
	{
	TAny* ptr = REComSession::CreateImplementationL(aImplementationUid,
													_FOFF(CContactEcomPhoneNumberParser,
														  iDtor_ID_Key));
	return reinterpret_cast<CContactEcomPhoneNumberParser*>(ptr);
	}

inline CContactEcomPhoneNumberParser::~CContactEcomPhoneNumberParser()
/** The destruction of the interface implementation referred to by iDtor_ID_Key */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

inline TInt CContactDatabase::TemplateCount() const
/** Gets the number of contact card templates that exist in the database.
This does not include the system template.
@deprecated
@return The number of contact card templates that exist in the database. */
	{ return iCardTemplateIds->Count(); }

inline TInt CContactDatabase::GroupCount() const
/** Gets the number of groups that exist in the database.
@deprecated
@return The number of groups that exist in the database. */
	{ return iGroupIds->Count(); }

inline TInt CContactDatabase::NullUidValue()
/** Gets the NULL contact ID value.

@return KNullContactId. */
	{ return KNullContactId; }

inline TUid CContactItemViewDef::operator[](TInt aIndex) const
/** Gets the field type located at a specified index position within the field type array.

@param aIndex The position of the field type in the array, relative to zero. It
must be non-negative and less than the number of objects in the array otherwise the
function raises a panic.
@return The field type located at index aIndex within the array (this is equivalent to a
TFieldType). */
    { return iFieldTypes[aIndex]; }

inline TInt CContactItemViewDef::Count() const
/** Gets the number of field types in the view definition.

@return The number of field types in the view definition. */
    { return iFieldTypes.Count(); }

inline void CContactItemViewDef::Reset()
/** Deletes all field types from the view definition's field type array. */
    { iFieldTypes.Reset(); }

inline CContactItemViewDef::TUse CContactItemViewDef::Use() const
/** Gets the view definition's use.

@return The current TUse setting. */
	{ return iUse; }

inline void CContactItemViewDef::SetUse(CContactItemViewDef::TUse aUse)
/** Sets the view definition's use.

@param aUse The new TUse setting. */
	{ iUse=aUse; }

inline CContactItemViewDef::TMode CContactItemViewDef::Mode() const
/** Gets the view definition's mode.

@return The current TMode setting. */
	{ return iMode; }

inline void CContactItemViewDef::SetMode(CContactItemViewDef::TMode aMode)
/** Sets the view definition's mode.

@param aMode The new TMode setting. */
	{ iMode=aMode; }

inline CContactDatabase::TSortPref::TSortPref()
	: iOrder(EAsc), iFieldType(KNullUid)
/** Constructs a TSortPref object. The order is initialised to EAsc and the field
type to KNullUid.
@deprecated
*/
	{}

inline CContactDatabase::TSortPref::TSortPref(TFieldType aFieldType,TOrder aOrder)
	: iOrder(aOrder), iFieldType(aFieldType)
/** Constructs the TSortPref object with a field type and an order.
@param aFieldType Specifies the field type to sort on.
@param aOrder Specifies the sort order.
@deprecated
*/
	{}


/** A class that manages the asynchronous open operation for a CContactDatabase.

The object can be deleted before the asynchronous open completes. This will set
the state of the client supplied TRequestStatus to KErrCancel.

When the client supplied TRequestStatus is completed with KErrNone the TakeDatabase()
method is called to pass ownership of the open database to the client.

@publishedAll
@released

@see CContactDatabase::OpenL()
*/
class CContactOpenOperation : private CActive
	{
public:
	IMPORT_C CContactDatabase* TakeDatabase();
	IMPORT_C ~CContactOpenOperation();

	static CContactOpenOperation* NewL(TRequestStatus& aPtrStatus);
	static CContactOpenOperation* NewL(const TDesC& aFileName, TRequestStatus& aPtrStatus);

private:
	// from CActive (for BC reasons)
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

	CContactOpenOperation (TRequestStatus& aClientStatus);
	void ConstructL		  (const TDesC& aFileName = KNullDesC);

private:
	TRequestStatus*  	iClientStatus;
	CContactDatabase*	iContactDatabase;
	};


#endif //__CNTDB_H__
