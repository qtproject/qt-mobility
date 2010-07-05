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
const TInt KNumOfTables = 16;
const TInt KNumColInPrefTable = 6;
const TInt KNumColInContactTable = 16;
const TInt KNumColInGroupTable = 3;
const TInt KNumColInCommTable = 5;
const TInt KNumColInPredSearchTable = 7;
const TInt KNumColInPresenceTable = 5;
// TODO: qwerty-mail tables are not yet added here

// tables in the contact database 
_LIT(KSqlContactTableName, "contact");
_LIT(KSqlContactGroupTableName, "groups");
_LIT(KSqlContactPrefTableName, "preferences");
_LIT(KSqlContactCommAddrTableName, "comm_addr");
_LIT(KSqlContactPredSearchTable0, "predictivesearch0");
_LIT(KSqlContactPredSearchTable1, "predictivesearch1");
_LIT(KSqlContactPredSearchTable2, "predictivesearch2");
_LIT(KSqlContactPredSearchTable3, "predictivesearch3");
_LIT(KSqlContactPredSearchTable4, "predictivesearch4");
_LIT(KSqlContactPredSearchTable5, "predictivesearch5");
_LIT(KSqlContactPredSearchTable6, "predictivesearch6");
_LIT(KSqlContactPredSearchTable7, "predictivesearch7");
_LIT(KSqlContactPredSearchTable8, "predictivesearch8");
_LIT(KSqlContactPredSearchTable9, "predictivesearch9");
_LIT(KSqlContactPredSearchTable10, "predictivesearch10");
_LIT(KSqlContactPredSearchTable11, "predictivesearch11");
_LIT(KSqlContactPresenceTableName, "presence");
// There are also 44 qwerty-mail tables (qm0..qm43)


enum TDatabaseTables
	{
	EContactPrefTableName,
	EContactTableName,
	EContactGroupTableName,
	EContactCommAddressTableName,
	KContactPredSearchTable0Name,
	KContactPredSearchTable1Name,
	KContactPredSearchTable2Name,
	KContactPredSearchTable3Name,
	KContactPredSearchTable4Name,
	KContactPredSearchTable5Name,
	KContactPredSearchTable6Name,
	KContactPredSearchTable7Name,
	KContactPredSearchTable8Name,
	KContactPredSearchTable9Name,
	KContactPredSearchTable10Name,
	KContactPredSearchTable11Name
	// TODO: qwerty-mail tables are not yet added here
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
_LIT(KContactTextFieldHeaderParam, ":text_fields_header");
_LIT(KContactBinaryFieldHeaderParam, ":binary_fields_header");
_LIT(KContactTextFieldsParam, ":text_fields");
_LIT(KContactBinaryFieldsParam, ":binary_fields");
//end parameters for contact table

// create statements
_LIT(KContactCreateStmnt, "CREATE TABLE contact (contact_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, template_id INTEGER, type_flags INTEGER, access_count INTEGER, creation_date , last_modified , guid_string VARCHAR(255) NULL, first_name VARCHAR(255) NOT NULL DEFAULT \"\", last_name VARCHAR(255) NOT NULL DEFAULT \"\", company_name VARCHAR(255) NOT NULL DEFAULT \"\", firstname_prn VARCHAR(255) NULL, lastname_prn VARCHAR(255) NULL, companyname_prn VARCHAR(255) NULL, text_fields_header BLOB, binary_fields_header BLOB, text_fields TEXT, binary_fields BLOB); CREATE INDEX contact_guid_string_idx  ON contact (guid_string);");
// end contact table

// comm-address table
// columns
_LIT(KCommAddrId, "comm_addr_id");
_LIT(KCommAddrType, "type");
_LIT(KCommAddrValue, "value");
_LIT(KCommAddrExtraValue, "extra_value");
_LIT(KCommAddrContactId, "contact_id");
// parameters
_LIT(KCommAddrIdParam, ":comm_addr_id");
_LIT(KCommAddrTypeParam, ":type");
_LIT(KCommAddrValueParam, ":value");
_LIT(KCommAddrExtraValueParam, ":extra_value");
_LIT(KCommAddrContactIdParam, ":contact_id");
// create statements
_LIT(KCommAddrCreateStmnt, "CREATE TABLE comm_addr (comm_addr_id  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, value CHAR(255) NOT NULL, extra_value CHAR(255), contact_id INTEGER NOT NULL, FOREIGN KEY (contact_id) REFERENCES contact(contact_id)); CREATE INDEX comm_addr_val_idx  ON comm_addr (value); CREATE INDEX comm_addr_contact_id_idx  ON comm_addr (contact_id);");
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

// predictive search table
// columns
_LIT(KPredSearchContactId, "contact_id");
_LIT(KPredSearchNameAsNumber, "nbr");
_LIT(KPredSearchNameAsNumber2, "nbr2");
_LIT(KPredSearchNameAsNumber3, "nbr3");
_LIT(KPredSearchNameAsNumber4, "nbr4");
_LIT(KPredSearchFirstName, "first_name");
_LIT(KPredSearchLastName, "last_name");

// parameters
_LIT(KPredSearchContactIdParam, ":contact_id");
_LIT(KPredSearchNameAsNumberParam, ":nbr");
_LIT(KPredSearchNameAsNumber2Param, ":nbr2");
_LIT(KPredSearchNameAsNumber3Param, ":nbr3");
_LIT(KPredSearchNameAsNumber4Param, ":nbr4");
_LIT(KPredSearchFirstNameParam, ":first_name");
_LIT(KPredSearchLastNameParam, ":last_name");

// create statements
_LIT(KPredSearchCreateTable0Stmnt,
"CREATE TABLE predictivesearch0 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable1Stmnt,
"CREATE TABLE predictivesearch1 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable2Stmnt,
"CREATE TABLE predictivesearch2 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable3Stmnt,
"CREATE TABLE predictivesearch3 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable4Stmnt,
"CREATE TABLE predictivesearch4 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable5Stmnt,
"CREATE TABLE predictivesearch5 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable6Stmnt,
"CREATE TABLE predictivesearch6 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable7Stmnt,
"CREATE TABLE predictivesearch7 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable8Stmnt,
"CREATE TABLE predictivesearch8 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable9Stmnt,
"CREATE TABLE predictivesearch9 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable10Stmnt,
"CREATE TABLE predictivesearch10 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");
_LIT(KPredSearchCreateTable11Stmnt,
"CREATE TABLE predictivesearch11 (contact_id INTEGER PRIMARY KEY,\
 nbr BIGINT NULL, nbr2 BIGINT NULL, nbr3 BIGINT NULL, nbr4 BIGINT NULL,\
 first_name CHAR(16) NULL, last_name CHAR(16) NULL);");


// create table indexes
_LIT(KPredSearchCreateNbrIndexTable0, "CREATE INDEX index0_nbr on predictivesearch0 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable0, "CREATE INDEX index0_nbr2 on predictivesearch0 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable0, "CREATE INDEX index0_nbr3 on predictivesearch0 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable0, "CREATE INDEX index0_nbr4 on predictivesearch0 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable1, "CREATE INDEX index1_nbr on predictivesearch1 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable1, "CREATE INDEX index1_nbr2 on predictivesearch1 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable1, "CREATE INDEX index1_nbr3 on predictivesearch1 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable1, "CREATE INDEX index1_nbr4 on predictivesearch1 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable2, "CREATE INDEX index2_nbr on predictivesearch2 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable2, "CREATE INDEX index2_nbr2 on predictivesearch2 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable2, "CREATE INDEX index2_nbr3 on predictivesearch2 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable2, "CREATE INDEX index2_nbr4 on predictivesearch2 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable3, "CREATE INDEX index3_nbr on predictivesearch3 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable3, "CREATE INDEX index3_nbr2 on predictivesearch3 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable3, "CREATE INDEX index3_nbr3 on predictivesearch3 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable3, "CREATE INDEX index3_nbr4 on predictivesearch3 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable4, "CREATE INDEX index4_nbr on predictivesearch4 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable4, "CREATE INDEX index4_nbr2 on predictivesearch4 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable4, "CREATE INDEX index4_nbr3 on predictivesearch4 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable4, "CREATE INDEX index4_nbr4 on predictivesearch4 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable5, "CREATE INDEX index5_nbr on predictivesearch5 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable5, "CREATE INDEX index5_nbr2 on predictivesearch5 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable5, "CREATE INDEX index5_nbr3 on predictivesearch5 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable5, "CREATE INDEX index5_nbr4 on predictivesearch5 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable6, "CREATE INDEX index6_nbr on predictivesearch6 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable6, "CREATE INDEX index6_nbr2 on predictivesearch6 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable6, "CREATE INDEX index6_nbr3 on predictivesearch6 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable6, "CREATE INDEX index6_nbr4 on predictivesearch6 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable7, "CREATE INDEX index7_nbr on predictivesearch7 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable7, "CREATE INDEX index7_nbr2 on predictivesearch7 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable7, "CREATE INDEX index7_nbr3 on predictivesearch7 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable7, "CREATE INDEX index7_nbr4 on predictivesearch7 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable8, "CREATE INDEX index8_nbr on predictivesearch8 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable8, "CREATE INDEX index8_nbr2 on predictivesearch8 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable8, "CREATE INDEX index8_nbr3 on predictivesearch8 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable8, "CREATE INDEX index8_nbr4 on predictivesearch8 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable9, "CREATE INDEX index9_nbr on predictivesearch9 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable9, "CREATE INDEX index9_nbr2 on predictivesearch9 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable9, "CREATE INDEX index9_nbr3 on predictivesearch9 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable9, "CREATE INDEX index9_nbr4 on predictivesearch9 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable10, "CREATE INDEX index10_nbr on predictivesearch10 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable10, "CREATE INDEX index10_nbr2 on predictivesearch10 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable10, "CREATE INDEX index10_nbr3 on predictivesearch10 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable10, "CREATE INDEX index10_nbr4 on predictivesearch10 (nbr4);");

_LIT(KPredSearchCreateNbrIndexTable11, "CREATE INDEX index11_nbr on predictivesearch11 (nbr);");
_LIT(KPredSearchCreateNbr2IndexTable11, "CREATE INDEX index11_nbr2 on predictivesearch11 (nbr2);");
_LIT(KPredSearchCreateNbr3IndexTable11, "CREATE INDEX index11_nbr3 on predictivesearch11 (nbr3);");
_LIT(KPredSearchCreateNbr4IndexTable11, "CREATE INDEX index11_nbr4 on predictivesearch11 (nbr4);");

// create table indexes for readable names for ordering results alphabetically
_LIT(KPredSearchCreateFNIndexInTable0,
"CREATE INDEX index_last_name0 on predictivesearch0 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable0,
"CREATE INDEX index_first_name0 on predictivesearch0 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable1,
"CREATE INDEX index_last_name1 on predictivesearch1 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable1,
"CREATE INDEX index_first_name1 on predictivesearch1 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable2,
"CREATE INDEX index_last_name2 on predictivesearch2 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable2,
"CREATE INDEX index_first_name2 on predictivesearch2 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable3,
"CREATE INDEX index_last_name3 on predictivesearch3 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable3,
"CREATE INDEX index_first_name3 on predictivesearch3 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable4,
"CREATE INDEX index_last_name4 on predictivesearch4 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable4,
"CREATE INDEX index_first_name4 on predictivesearch4 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable5,
"CREATE INDEX index_last_name5 on predictivesearch5 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable5,
"CREATE INDEX index_first_name5 on predictivesearch5 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable6,
"CREATE INDEX index_last_name6 on predictivesearch6 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable6,
"CREATE INDEX index_first_name6 on predictivesearch6 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable7,
"CREATE INDEX index_last_name7 on predictivesearch7 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable7,
"CREATE INDEX index_first_name7 on predictivesearch7 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable8,
"CREATE INDEX index_last_name8 on predictivesearch8 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable8,
"CREATE INDEX index_first_name8 on predictivesearch8 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable9,
"CREATE INDEX index_last_name9 on predictivesearch9 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable9,
"CREATE INDEX index_first_name9 on predictivesearch9 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable10,
"CREATE INDEX index_last_name10 on predictivesearch10 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable10,
"CREATE INDEX index_first_name10 on predictivesearch10 (first_name);");

_LIT(KPredSearchCreateFNIndexInTable11,
"CREATE INDEX index_last_name11 on predictivesearch11 (last_name);");
_LIT(KPredSearchCreateLNIndexInTable11,
"CREATE INDEX index_first_name11 on predictivesearch11 (first_name);");



_LIT(KPresenceContactId, "contact_id");
_LIT(KPresenceAccountUri, "account_uri");
_LIT(KPresenceServiceName, "service_name");
_LIT(KPresenceStatus, "status");
_LIT(KPresenceStatusMsg, "status_message");
_LIT(KPresenceContactIdParam, ":contact_id");
_LIT(KPresenceAccountUriParam, ":account_uri");
_LIT(KPresenceServiceNameParam, ":service_name");
_LIT(KPresenceStatusParam, ":status");
_LIT(KPresenceStatusMsgParam, ":status_message");
_LIT(KPresenceCreateStmnt,
"CREATE TABLE presence (contact_id INTEGER NOT NULL,\
account_uri VARCHAR(255),service_name VARCHAR(255),\
status INTEGER, status_msg VARCHAR(255),\
CONSTRAINT contactid_fk FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE);");


// Predictive search for qwerty mail tables
// columns
_LIT(KPredSearchQwertyMailContactId, "contact_id");
_LIT(KPredSearchQwertyMailNameAsNumber, "n");
_LIT(KPredSearchQwertyMailNameAsNumber2, "n2");
_LIT(KPredSearchQwertyMailNameAsNumber3, "n3");
_LIT(KPredSearchQwertyMailNameAsNumber4, "n4");
_LIT(KPredSearchQwertyMailNameAsNumber5, "n5");
_LIT(KPredSearchQwertyMailNameAsNumber6, "n6");
_LIT(KPredSearchQwertyMailNameAsNumber7, "n7");
_LIT(KPredSearchQwertyMailFirstName, "first_name");
_LIT(KPredSearchQwertyMailLastName, "last_name");

// parameters
_LIT(KPredSearchQwertyMailContactIdParam, ":contact_id");
_LIT(KPredSearchQwertyMailNameAsNumberParam, ":n");
_LIT(KPredSearchQwertyMailNameAsNumberParam2, ":n2");
_LIT(KPredSearchQwertyMailNameAsNumberParam3, ":n3");
_LIT(KPredSearchQwertyMailNameAsNumberParam4, ":n4");
_LIT(KPredSearchQwertyMailNameAsNumberParam5, ":n5");
_LIT(KPredSearchQwertyMailNameAsNumberParam6, ":n6");
_LIT(KPredSearchQwertyMailNameAsNumberParam7, ":n7");
_LIT(KPredSearchQwertyMailFirstNameParam, ":first_name");
_LIT(KPredSearchQwertyMailLastNameParam, ":last_name");


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

#endif // #ifndef DBSQLCONSTANTS_H
