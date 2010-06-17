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


#ifndef __PLTABLES_H__
#define __PLTABLES_H__

#include "cntsqlprovider.h"
#include "persistencelayer.h"
#include "clplcontactproperties.h"
#include <cntdef.h>
#include <cntitem.h>
#include <cntfldst.h>

#include <sqldb.h>
#include <e32hashtab.h>
#include <QList>
#include <QStringList>

class CPcsKeyMap;


/**
The CPplTableBase class forms the base class for all SQLite tables in the
Persistence Layer. It implements default behaviour for some basic operations.

It holds member variables for the database and SQL statements representing the
4 CRUD operations. The ReadL function is virtual with a default implementation
that does nothing so that it can be overridden in the derived classes, as
necessary. This is to allow the read operation to be performed on all classes
in the same manner, even when in cases where there is nothing to be read. In
this way, all table classes can be treated the same. This is to facilitate the
use of the composite design pattern.
*/
NONSHARABLE_CLASS(CPplTableBase) : public CBase
	{
public:
	virtual void CreateInDbL(CContactItem& aItem) = 0;
	virtual void UpdateL(const CContactItem& aItem) = 0;
	virtual void DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred) = 0;
	virtual void CreateTableL() = 0;
	};


/**
The CPplContactTable class has a dual nature, caused by inheritance from two
interfaces: MPplContactItemPersistor and MLplPersistenceBroker.

MPplContactItemPersistor inheritance makes the CPplContactTable a normal table
capable of perfoming five basic operations (Create, Read, Update, Delete and
ChangeType) when called from the parent table.

MLplPersistenceBroker is a public interface responsible for carrying out the
basic operations.  It is exposed to the users of Persistence Layer making the
RPplContactTable an entry point for these operations.

As a result ContactTable appears twice in the tree structure of tables: once as
a root table (MLplPersistenceBroker inteface) and second time as a child of
Email table (MPplContactItemPersistor inteface).

The Contact table is responsible for saving header information for all the
fields in a contact and the values of most of the fields. Four C/BLOB fields
are used for this purpose.

Header information (including label and content type of the fields) is stored in
in the binary_fields_header and text_fields_header columns.

The binary_fields column accomodates the binary values of the fields (such as
pictures or sounds).

All human readable text values go to the text_fields column. This is particularly
useful for searching through all textual fields of contacts.
*/
NONSHARABLE_CLASS(CPplContactTable) : public CPplTableBase
	{
public:
	class THintField
		{
	friend class CPplContactTable;
	public:
		THintField();
		void UpdateHintL(const CContactItemField& aField, const RArray<TUid>& aCustFiltFields);


		TUint16 ExtHint();
		TInt8 Hint();
	private:
		THintField(TUint16 aExtHint, TUint8 aHint);

	private:
		TInt iValue;
		};


public:
	static CPplContactTable* NewL(RSqlDatabase& aDatabase, CLplContactProperties& aProperties);
	static CPplContactTable* NewLC(RSqlDatabase& aDatabase, CLplContactProperties& aProperties);
	void CreateInDbL(CContactItem& aItem);
	void CreateInDbL(CContactItem& aItem, TUint aSessionId);

	void UpdateL(const CContactItem& aItem);
	void DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred);
	CContactItem* DeleteLC(TContactItemId  aItemId, TBool& aLowDiskErrorOccurred);
	void CreateTableL();

	void ChangeTypeL(TContactItemId aItemId, TUid aNewType);
	TInt NameFieldUid(const CContactItemField& nameField);

	TBool IsTableEmptyL();
	CContactIdArray& CardTemplateIdsL();
	TContactItemId OwnCardIdL();
    void SetOwnCardIdL(TContactItemId aId, TBool aPersist = ETrue);	
    
	~CPplContactTable();
	

private:
	CPplContactTable(RSqlDatabase& aDatabase, CLplContactProperties& aProps);
	void ConstructL();
	void WriteContactItemL(const CContactItem& aItem, TCntSqlStatement aType);
	void UpdateTemplateAccessCounterL(TContactItemId aTemplateRefId, TBool aIncrement);
	void GetTypeFlagFields(TInt aTypeFlags, TUid& aType, TUint& aAttributes, TUint& aHintFields);
	TInt GenerateTypeFlags(TUid aType, TUint aAttributes, TUint aHintFields);
	TUint NumDigits(TInt aNum);
	
private:
	CLplContactProperties& iProperties;
	CCntSqlStatement* iInsertStmnt;
	CCntSqlStatement* iDeleteSelectStmnt;
	CCntSqlStatement* iAccessCountUpdateStmnt;
	CCntSqlStatement* iUpdateFlagsStmnt;
	CCntSqlStatement* iUpdateStmnt;
	CCntSqlStatement* iDeleteStmnt;
	RHashMap<TInt, TPtrC> iFieldMap;
	RSqlDatabase&  iDatabase;
	
	CContactIdArray* iCardTemplateIds;
	TContactItemId iOwnCardId;
	};

/**
The CPplCommAddrTable class is used to store all communication addresses in a
contact item. This includes telephone/fax numbers, email addresses and SIP
addresses.

Email and SIP addresses are stored as-is but telephone/fax numbers are hashed
in this table. This is because in the text field in the contact table, telephone
numbers are stored as text. This provides great flexibility for the users as they
can use any symbol in their telephone number fields, such as '+','(',')', spaces
or even words like 'ext.' or 'emergency only'. However, this form of information
storage present a big problem in the phone matching use case i.e. when the
Contacts Model should quickly find a contact with the given phone number in it.

This problem is solved with the hashing mechanism.  Each time the user creates a
new phone field or changes the existing one the hash value is calculated and
saved in the phone table.  The hash value is calculated with the help of the
cntphone plugin, which parses the text containing the telephone number and tries
to find all the informative digits and symbols.

Originally the hash value covered only last seven digits of the phone number and
the TInt column was enough to store the hash.  Later the value was extended and
now it is stored in two TInt columns to provide data backward compatibility with
previous versions of the Contacts Model.

The comm_addr table contains a private helper class TMatch that represents the
hash value and an enumeration to specify type of communication address.
*/
NONSHARABLE_CLASS(CPplCommAddrTable) : public CPplTableBase
	{
private:
	class TMatch
		{
	public:
		TMatch();

		static TInt32 CreateHashL(const TDesC& aPhoneNumberString, TInt aMatchLength, TInt& aNumPhoneDigits);
		static void StripOutNonDigitChars(TDes& aText);
		static TInt32 PadOutPhoneMatchNumber(TInt32& aPhoneNumber,TInt aPadOutLength);
		static TBool Equals (const TMatch& aRMatch ,const TMatch& aLMatch);
		inline TBool operator==(const TMatch &aMatch) const;

	public:
		TInt32 iLowerSevenDigits;
		TInt32 iUpperDigits;
		TInt iNumLowerDigits;
		TInt iNumUpperDigits;
		};

public:
	// defines type of communication address.
	// !! the values these represent are persisted in the database  !!
	// !!  do not change the order -- add new ones to the bottom    !!
	// !!   changing these values could break data compatibility    !!
	enum TCommAddrType
		{
		EPhoneNumber,
		EEmailAddress,
		ESipAddress
		};

public:
	static CPplCommAddrTable* NewL(RSqlDatabase& aDatabase, CLplContactProperties& aProperties);
	static CPplCommAddrTable* NewLC(RSqlDatabase& aDatabase, CLplContactProperties& aProperties);
	void CreateInDbL(CContactItem& aItem);
	void UpdateL(const CContactItem& aItem);
	void DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred);
	void CreateTableL();

	CContactIdArray* MatchPhoneNumberL(const TDesC& aNumber, const TInt aMatchLengthFromRight);
	CContactIdArray* BestMatchingPhoneNumberL(const TDesC& aNumber);
	CContactIdArray* MatchEmailAddressL(const TDesC& aEmailAddr);
	CContactIdArray* MatchSipAddressL(const TDesC& aSipAddr);

	~CPplCommAddrTable();

private:
	void ConstructL();
	CPplCommAddrTable(RSqlDatabase& aDatabase, CLplContactProperties& iProperties);
	void RemoveNonUpdatedAddrsL(RArray<TMatch>& aNewPhones, RArray<TPtrC>& aNewEmails, RArray<TPtrC>& aNewSips, 
							RArray<TInt>& aFreeCommAddrIds, const TInt aItemId);
	void DoUpdateCommAddrsL(RArray<TMatch>& aNewPhones, RArray<TPtrC>& aNewEmails, RArray<TPtrC>& aNewSips, 
						    RArray<TInt>& aFreeCommAddrIds, const TInt aItemId);
	void DeleteSingleCommAddrL(TInt aCommAddrId, TBool& aLowDiskErrorOccurred);
	void DoPhoneNumWriteOpL(const CPplCommAddrTable::TMatch& aPhoneNum, TCntSqlStatement aType, TInt aCntId);
	void DoPhoneNumWriteOpL(const CPplCommAddrTable::TMatch& aPhoneNum, TCntSqlStatement aType, TInt aCntId, 
							TInt aCommAddrId);
	void DoNonPhoneWriteOpL(const TDesC& aAddress, TCntSqlStatement aType, TInt aCntId, 
							TCommAddrType aAddrType);
	void DoNonPhoneWriteOpL(const TDesC& aAddress, TCntSqlStatement aType, TInt aCntId, 
							TCommAddrType aAddrType, TInt aCommAddrId);
	CContactIdArray* MatchNonPhoneAddrL(const TDesC& aCommAddr, TCommAddrType aAddrType);
	CPplCommAddrTable::TMatch CreatePaddedPhoneDigitsL(const TDesC& aNumber, const TInt aNumLowerDigits, 
							const TInt aNumUpperDigits);
	CPplCommAddrTable::TMatch CreatePhoneMatchNumberL(const TDesC& aText, TInt aLowerMatchLength, 
							TInt aUpperMatchLength);

private:
	CLplContactProperties& iProperties;
	CCntSqlStatement* iInsertStmnt;
	CCntSqlStatement* iWholeSelectStmnt;
	CCntSqlStatement* iMatchSelectStmnt;
	CCntSqlStatement* iUpdateStmnt;
	CCntSqlStatement* iAllForItemDeleteStmnt;
	CCntSqlStatement* iSingleDeleteStmnt;
	RSqlDatabase&  iDatabase;
	};


/**
The CPplGroupsTable class provides a mapping between contact groups and members
of those groups. The groups table provides two fields: contact_group_id and
contact_group_member_id. Both of these are foreign keys referencing the contact_id
field of the contact table. Using these two fields, lookup can be performed in 
either direction: members of a group; groups of which item is a member.
*/
NONSHARABLE_CLASS(CPplGroupsTable) : public CPplTableBase
	{
public:
	static CPplGroupsTable* NewL(RSqlDatabase& aDatabase);
	static CPplGroupsTable* NewLC(RSqlDatabase& aDatabase);
	void CreateInDbL(CContactItem& aItem);
	void ReadL(CContactItem& aItem);
	void UpdateL(const CContactItem& aItem);
	void DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred);
	void CreateTableL();
	~CPplGroupsTable();

private:
	void ConstructL();
	CContactIdArray* GetListForItemL(TContactItemId aItemId, TBool aIsGroup);
	void DeleteItemL(TContactItemId aItemId, TBool& aLowDiskErrorOccurred);
	CPplGroupsTable(RSqlDatabase& aDatabase);
	void WriteGroupMembersL(const CContactItem& aGroup);

private:
	CCntSqlStatement* iInsertStmnt;
	CCntSqlStatement* iSelectGroupsStmnt;
	CCntSqlStatement* iSelectMembersStmnt;
	CCntSqlStatement* iDeleteStmnt;
	CCntSqlStatement* iCountContactsStmnt;
	
	RSqlDatabase&  iDatabase;
	};


/**
The CPplPredictiveSearchTableBase is a base class for keymap-specific predictive
search tables that contain numeric representation of the fields that are checked
in predictive search.
*/
NONSHARABLE_CLASS(CPplPredictiveSearchTableBase) : public CPplTableBase
	{
public:
	virtual ~CPplPredictiveSearchTableBase();

public: // From CPplTableBase
	void CreateInDbL(CContactItem& aItem);
	void UpdateL(const CContactItem& aItem);
	void DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred);
	virtual void CreateTableL() = 0;

public: // New pure virtual functions
	virtual QList<QChar> FillAllTables() const = 0;

private: // New pure virtual functions
	virtual HBufC* TableNameL(const QChar aCh) const = 0;
	virtual TBool IsValidChar(const QChar aChar) const = 0;

	virtual void FillKeyboardSpecificFieldsL(RSqlStatement& aSqlStatement,
											 QStringList aTokens) = 0;

private: // New virtual functions
	virtual QStringList
		GetTableSpecificFields(const CContactItem& aItem,
							   TBool& aMandatoryFieldsPresent) const;

public:
	// Return next table's name, ownership is transferred
	HBufC* GetNextTableNameL(QList<QChar>& aTables) const;

protected:
	void ConstructL();
	CPplPredictiveSearchTableBase(RSqlDatabase& aDatabase,
								  TInt aMaxTokens,
								  TInt aMaxTokenLength);

	QList<QChar> DetermineTables(QStringList aTokens) const;

	// aFirstName ownership is not transferred
	// aLastName ownership is not transferred
	QStringList GetTokens(QStringList aNonTokenizedFields,
						  HBufC* aFirstName,
						  HBufC* aLastName) const;

private:
	void WriteToDbL(const CContactItem& aItem);

	// aFirstNameAsNbr OUT: Pointer to first name converted to numbers,
	//				  		pushed to cleanupstack. Ownership is transferred.
	// aLastNameAsNbr OUT: Pointer to last name converted to numbers,
	//				       pushed to cleanupstack. Ownership is transferred.
	// aFirstName OUT: Pointer to the first N characters of first name,
	//			  	   pushed to cleanupstack. Ownership is transferred.
	// aLastName OUT: Pointer to the first N characters of last name,
	//			  	  pushed to cleanupstack. Ownership is transferred.
	void GetFieldsLC(const CContactItem& aItem,
					 HBufC** aFirstNameAsNbr,
					 HBufC** aLastNameAsNbr,
					 HBufC** aFirstName,
					 HBufC** aLastName) const;

	// aString ownership is not transferred
	void AddTokens(HBufC* aString, QStringList& aTokens) const;

	void GetNextToken(QStringList& aSource, QStringList& aDestination) const;
	void DeleteFromAllTablesL(TContactItemId aContactId,
							  TBool& aLowDiskErrorOccurred) const;

protected:
	// Owned
	CCntSqlStatement* iInsertStmnt;
	// Owned
	CCntSqlStatement* iDeleteStmnt;
	// Owned
	CPcsKeyMap*		  iKeyMap;

	RSqlDatabase&	  iDatabase;

	// Max amount of tokens that can be stored into predictive search table
	const TInt		  iMaxTokens;

	// Max length of a single token that can be stored into predictive search table
	const TInt		  iMaxTokenLength;
	};


NONSHARABLE_CLASS(CPplPresenceTable) : public CPplTableBase
    {
public:
    static CPplPresenceTable* NewL(RSqlDatabase& aDatabase);
    static CPplPresenceTable* NewLC(RSqlDatabase& aDatabase);
    ~CPplPresenceTable();
private:
    void ConstructL();
    CPplPresenceTable(RSqlDatabase& aDatabase);
public: // From CPplTableBase
    void CreateTableL();
    void CreateInDbL(CContactItem& aItem);
    void UpdateL(const CContactItem& aItem);
    void DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred);
private:
    RSqlDatabase&  iDatabase;
    };
/**
This class holds a set of contact database preferences and is used in conjunction
with the CPplPreferencesPersistor class.
*/
NONSHARABLE_CLASS(RCntPreferences)
	{
public:
	TUint DataSchemaVersion() const;
	TUint DatabaseUid() const;
	TTime CreationDate() const;
	TUint PreferCardTemplateId() const;	
	RContactViewSortOrder& PreferredSortOrder();
	RCntPreferences();
	void Close();
	void SetDataSchemaVersion(TUint aDbSchemaVersion);
	void SetDatabaseUid(TUint aDbUid);
	void SetCreationDate(TTime aCreationDate);
	void SetFieldTypeFixsize(TUint aFieldTypeFixsize);
	void SetPreferredSortOrderL(const RContactViewSortOrder& aSortOrder);
	void SetPreferCardTemplateId(TUint aTemplateId);	

private:
	TUint iDbSchemaVersion;
	TUint iDbUid;
	TTime iCreationDate;
	TUint iPreferCardTemplateId;	
	RContactViewSortOrder iSortOrder;
	};

/**
The CPplPreferencesPersistor class is used to store contact database preferences,
as represented by the RCntPreferences class and, currently, stored in the 
preferences table in the SQLite database. As this information is different from
the rest of the data in that it is not contacts data but metadata about the 
database itself, it is treated differently. Hence, this class is called a persistor
and not a table class to emphasise the differences and to provide the flexibility
to change the way the preferences information is persist in the future if this is
desired.

*/
NONSHARABLE_CLASS(CPplPreferencesPersistor) : public CBase
	{
public:
	static CPplPreferencesPersistor* NewL(RSqlDatabase& aDatabase);
	~CPplPreferencesPersistor();
	
	void CreateTableL();

    // getters
	TDesC DataSchemaVersion() const;
	TInt64 MachineIdL();
	TTime CreationDateL();
	TContactItemId PreferredCardTemplateIdL();	
	const CArrayFix<CContactDatabase::TSortPref>& PreferredSortOrderL();
	TPtrC DatabaseUidL();	
		
	// setters 	
	void SetMachineIdL(TInt64 aMachineId);
	void SetPreferredCardTemplateIdL(TContactItemId aTemplateId);
	void SetPreferredSortOrderL(CArrayFix<CContactDatabase::TSortPref>* aSortOrder);	 
	
	// utility methods
	void SetGuidL(CContactItem& aContact, TBool& aCompressed);
	
private:
	CPplPreferencesPersistor(RSqlDatabase& aDatabase);
	void ConstructL();
	void FirstWriteToTableL();
	void GenerateMachineUniqueID();
	void ReadInStateL();
	void PersistStateL(const TDesC& aParam, TInt aValue);

private:
	RSqlDatabase& iDatabase;
	CCntSqlStatement* iUpdateStmnt;
	TUint iMachineId;        // machine id    
	TTime iCreationDate;     // creation data
	TBuf<40> iUidString;     //unique id  
	TContactItemId iPreferCardTemplateId;   // template	
	CArrayFix<CContactDatabase::TSortPref>* iSortOrderPrefs;		 // sort order
	};

#endif
