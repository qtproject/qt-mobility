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

#ifndef DBSQLCONSTANTS_H
#define DBSQLCONSTANTS_H

#include <cntdef.hrh>


const TInt KInitialValue = -1;
const TInt KNumOfTables = 4;
const TInt KNumColInPrefTable = 6;
const TInt KNumColInContactTable = 17;
const TInt KNumColInGroupTable = 3;
const TInt KNumColInCommTable = 6;

// tables in the contact database 
_LIT(KSqlContactTableName, "contact");
_LIT(KSqlContactGroupTableName, "groups");
_LIT(KSqlContactPrefTableName, "preferences");
_LIT(KSqlContactCommAddrTableName, "comm_addr");

enum TDatabaseTables
	{
	EContactPrefTableName,
	EContactTableName,
	EContactGroupTableName,
	EContactCommAddressTableName
	};

// columns for contact table
_LIT(KContactId, "contact_id");
_LIT(KContactTemplateId, "template_id");
_LIT(KContactTypeFlags, "type_flags");
_LIT(KContactAccessCount, "access_count");
_LIT(KContactCreationDate, "creation_date");
_LIT(KContactLastModified, "last_modified");
_LIT(KContactGuidString, "guid_string");
_LIT(KContactFirstName, "first_name");
_LIT(KContactLastName, "last_name");
_LIT(KContactCompanyName, "company_name");
_LIT(KContactFirstNamePrn, "firstname_prn");
_LIT(KContactLastNamePrn, "lastname_prn");
_LIT(KContactCompanyNamePrn, "companyname_prn");
_LIT(KContactFavoriteIndex, "favorite_index");
_LIT(KContactTextFieldHeader, "text_fields_header");
_LIT(KContactBinaryFieldHeader, "binary_fields_header");
_LIT(KContactTextFields, "text_fields");
_LIT(KContactBinaryFields, "binary_fields");
//end columns for contact table

// parameters for contact table
_LIT(KContactIdParam, ":contact_id");
_LIT(KContactTemplateIdParam, ":template_id");
_LIT(KContactTypeFlagsParam, ":type_flags");
_LIT(KContactTypeParam, ":contact_type");
_LIT(KAttributesAndHintParam, ":attr_and_hint");
_LIT(KContactAccessCountParam, ":access_count");
_LIT(KContactCreationDateParam, ":creation_date");
_LIT(KContactLastModifiedParam, ":last_modified");
_LIT(KContactGuidStringParam, ":guid_string");
_LIT(KContactFirstNameParam, ":first_name");
_LIT(KContactLastNameParam, ":last_name");
_LIT(KContactCompanyNameParam, ":company_name");
_LIT(KContactFirstNamePrnParam, ":firstname_prn");
_LIT(KContactLastNamePrnParam, ":lastname_prn");
_LIT(KContactCompanyNamePrnParam, ":companyname_prn");
_LIT(KContactFavoriteIndexParam, ":favorite_index");
_LIT(KContactTextFieldHeaderParam, ":text_fields_header");
_LIT(KContactBinaryFieldHeaderParam, ":binary_fields_header");
_LIT(KContactTextFieldsParam, ":text_fields");
_LIT(KContactBinaryFieldsParam, ":binary_fields");
//end parameters for contact table

// create statements
// New Column should be added after the old columns. If the column has "Not NULL" constraint
// then Default Value should be provide. 
// New Column should be added to CCntSqlDbStructure::InitializeContactsTableInfoL() also 
_LIT(KContactCreateStmnt, "CREATE TABLE contact (contact_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, template_id INTEGER, type_flags INTEGER, access_count INTEGER, creation_date , last_modified , guid_string VARCHAR(255) NULL, first_name VARCHAR(255) NOT NULL DEFAULT \"\", last_name VARCHAR(255) NOT NULL DEFAULT \"\", company_name VARCHAR(255) NOT NULL DEFAULT \"\", firstname_prn VARCHAR(255) NULL, lastname_prn VARCHAR(255) NULL, companyname_prn VARCHAR(255) NULL, text_fields_header BLOB, binary_fields_header BLOB, text_fields TEXT, binary_fields BLOB, favorite_index INTEGER NULL); CREATE INDEX contact_guid_string_idx  ON contact (guid_string);");
// end contact table

// comm-address table
// columns
_LIT(KCommAddrId, "comm_addr_id");
_LIT(KCommAddrType, "type");
_LIT(KCommAddrValue, "value");
_LIT(KCommAddrExtraValue, "extra_value");
_LIT(KCommAddrContactId, "contact_id");
_LIT(KCommAddrExtraTypeInfo, "extra_type_info");
// parameters
_LIT(KCommAddrIdParam, ":comm_addr_id");
_LIT(KCommAddrTypeParam, ":type");
_LIT(KCommAddrValueParam, ":value");
_LIT(KCommAddrExtraValueParam, ":extra_value");
_LIT(KCommAddrContactIdParam, ":contact_id");
_LIT(KCommAddrExtraTypeInfoParam, ":extra_type_info");
// create statements
// New Column should be added after the old columns. If the column has "Not NULL" constraint
// then Default Value should be provide. 
// New Column should be added to CCntSqlDbStructure::InitializeCommAddrTableInfoL() also 
_LIT(KCommAddrCreateStmnt, "CREATE TABLE comm_addr (comm_addr_id  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, value CHAR(255) NOT NULL, extra_value CHAR(255), contact_id INTEGER NOT NULL, extra_type_info INTEGER, FOREIGN KEY (contact_id) REFERENCES contact(contact_id)); CREATE INDEX comm_addr_val_idx  ON comm_addr (value); CREATE INDEX comm_addr_contact_id_idx  ON comm_addr (contact_id);");
// end comm_address table

// groups table
// columns
_LIT(KGroupId, "group_id");
_LIT(KGroupContactGroupId, "contact_group_id");
_LIT(KGroupContactGroupMemberId, "contact_group_member_id");
// parameters
_LIT(KGroupIdParam, ":group_id");
_LIT(KGroupContactGroupIdParam, ":contact_group_id");
_LIT(KGroupContactGroupMemberIdParam, ":contact_group_member_id");
// create statements
// New Column should be added after the old columns. If the column has "Not NULL" constraint
// then Default Value should be provide. 
// New Column should be added to CCntSqlDbStructure::InitializeGroupsTableInfoL() also 
_LIT(KGroupsCreateStmnt, "CREATE TABLE groups (group_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, contact_group_id INTEGER NOT NULL, contact_group_member_id INTEGER NOT NULL, FOREIGN KEY (contact_group_id) REFERENCES contact(contact_id), FOREIGN KEY (contact_group_member_id) REFERENCES contact(contact_id) ); CREATE INDEX contact_group_id_idx ON groups (contact_group_id); CREATE INDEX contact_group_member_id_idx ON groups (contact_group_member_id);");
// end groups table

// preferences table
// columns
_LIT(KPrefPrefId,"preference_id");
_LIT(KPrefSchemaVersion,"data_schema_version");
_LIT(KPrefDatabaseUid,"database_uid");
_LIT(KPrefPrefCreationDate,"creation_date");
_LIT(KPrefMachineId, "machine_id");
_LIT(KPrefPreferredOrder, "preferred_sortorder");
_LIT(KPrefferredTemplateId,"preferred_cardtemplate_id");

// parameters
_LIT(KPrefIdParam, ":preference_id");
_LIT(KPrefSchemaVersionParam, ":data_schema_version");
_LIT(KPrefDatabaseUidParam, ":database_uid");
_LIT(KPrefCreationDateParam, ":creation_date");
_LIT(KPrefMachineIdParam, ":machine_id");
_LIT(KPrefPreferredOrderParam, ":preferred_sortorder");
_LIT(KPrefferredTemplateIdParam,":preferred_cardtemplate_id");

// create statements
_LIT(KPreferencesCreateStmnt,  "CREATE TABLE preferences (preference_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, data_schema_version INTEGER NOT NULL, preferred_cardtemplate_id INTEGER, machine_id , creation_date , preferred_sortorder BLOB); ");
// end preferences table


// Condition strings for searching id
_LIT(KSelectCondition_SearchForEqual,   "%S == :SeekId");

// SQLITE_SEQUENCE table stuff for getting last row_id
_LIT(KSelectLastIdSqlStmnt, "SELECT seq FROM SQLITE_SEQUENCE WHERE name = 'contact';");


// SQLITE_SEQUENCE table stuff for getting last row_id
_LIT(KSqliteSequenceTableName, "SQLITE_SEQUENCE");
_LIT(KSqliteSequenceSeq, "seq");
_LIT(KSqliteSequenceName, "name");

_LIT(KSQLSelect, "SELECT ");
_LIT(KFrom, "FROM ");

_LIT(KSpace, " ");
_LIT(KComma, ",");
_LIT(KSelectTwoFields, "SELECT %S, %S FROM %S WHERE %S=%d");
_LIT(KSelectTwoFieldsWithGreater, "SELECT %S, %S FROM %S WHERE %S>=%d");
_LIT(KSelectFiveFields, "SELECT %S, %S, %S, %S, %S FROM %S");
_LIT(KCountSelect, "SELECT count(*) FROM %S");
_LIT(KCountTypeSelect, "SELECT count(*) FROM %S WHERE (%S>>%d)=%d AND ((%S>>%d)&%d)=0");
_LIT(KOneTypeField, "SELECT %S FROM %S WHERE (%S>>%d)=%d");
_LIT(KTwoTypeField, "SELECT %S, %S FROM %S WHERE (%S>>%d)=%d");
_LIT(KJoinConditionClause, "%S=%S and %S=%d");
_LIT(KConditionClause, "%S=%d");
_LIT(KTypeConditionClause, "(%S>>%d)=%d");
_LIT(KWhereStringEqualsStringFormatText, "%S = %S");
_LIT(KSelectFlagsSqlStmntFormat, "SELECT type_flags FROM contact WHERE contact_id = %d");
_LIT(KVar, ":var");
_LIT(KIntValue, "%d");
_LIT(KSelectCondition_SearchForContactId, "contact_id == :SeekId");


// COUNT function
_LIT(KCount, " COUNT(%S) ");

// NULL
_LIT(KNullText, "NULL");    //think this should be deleted

// AND
_LIT(KSqlAnd, " AND ");

// OR
_LIT(KSqlOr, " OR ");

// COUNT
_LIT(KSqlCount, "count(*)");

const TInt  KFirstParam = 0;

/* constants defined for type_flags in contact table */

// definition for contact type flags

//Bit flags in the byte of Contact Type
enum TContactTypeFlags 
	{
	EContactTypeFlags_ContactCard,   
	EContactTypeFlags_OwnCard,
	EContactTypeFlags_ICCEntry,
	EContactTypeFlags_Group,
	EContactTypeFlags_Template,
	EContactTypeFlags_CardTemplate,
	EContactTypeFlags_UnknownType
	};

const TInt KContactTypeShiftBits = 24; //ContactType byte stored in highest(left most) byte in the type flags integer

/* definition for contact attributes flags */
enum TContactAttrsFlags 
	{
	EContactAttrsFlags_System  			= 1,
	EContactAttrsFlags_Hidden  			= 2, // 1 << 1
	EContactAttrsFlags_CompressedGUID 	= 4, // 1 << 2
	EContactAttrsFlags_Deleted    		= 8, // 1 << 3
	EContactAttrsFlags_Mask  		    = 0x00FF0000
	};
const TInt KContactAttrsShiftBits = 16; //Contact Attributes byte stored in second highest byte in the type flags integer

/* definition for contact hint flags */ 
const TInt KFastAccessFieldUids[] = 
	{
	KUidContactFieldGivenNameValue, 
	KUidContactFieldFamilyNameValue,
	KUidContactFieldCompanyNameValue,
	KUidContactFieldGivenNamePronunciationValue,
	KUidContactFieldFamilyNamePronunciationValue,
	KUidContactFieldCompanyNamePronunciationValue
	};
	
/* definition for contact hint flags */
enum TContactTypeFlagsMasks 
	{
	EContactType_Mask 		= 0xFF000000,
	EContactAttributes_Mask	= 0x00FF0000,
	EContactHintFlags_Mask 	= 0x0000FFFF // Hint value put lowest 2 bytes of the type flags integer
	};

enum TContactTypeFlagsShitfs
	{
	EContactAttributes_Shift	= 16,
	EContactType_Shift 			= 24	
	};	

enum TFastAccessField
	{
	EFirstFastAccessField = 0,
	EGivenName = EFirstFastAccessField,
	EFamilyName,
	ECompanyName,
	EGivenNamePrn,
	EFamilyNamePrn,
	ECompanyNamePrn,
	ENumberOfFastAccessFields,
	ENoSuchFastAccessField = -1
	};

const TInt KFirstIndex = 0;

// For Database compatability . Used in cntsqldbstructure
_LIT(KInteger, "INTEGER");
_LIT(KVarchar255,"VARCHAR(255)");
_LIT(KChar255,"CHAR(255)");
_LIT(KBlob,"BLOB");
_LIT(KText,"TEXT");
_LIT(KDefault, "DEFAULT \"\"");
_LIT(KNotNull, "NOT NULL");
_LIT(KNull, "NULL");
_LIT(KAutoIncrement, "PRIMARY KEY AUTOINCREMENT");
_LIT(KPragmaName, "Name");
_LIT(KPragmaType, "Type");
_LIT(KPragmaStmnt, "PRAGMA table_info(%S);");
_LIT(KContactTableAddColumn,"ALTER TABLE %S ADD COLUMN %S %S %S %S");

#endif // #ifndef DBSQLCONSTANTS_H
