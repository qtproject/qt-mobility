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


#ifndef __CNTVCARD_H__
#define __CNTVCARD_H__

// System includes
#include <cntdb.h>
#include <vcard.h>

// Classes referenced
class CVCardAddress;
class CVCardItemAndLabel;
class CVCardToContactsAppConverter;

// Enumerations
/**
@internalComponent
@released
*/
enum TCntVCardImportType
	{
	ECntVCardImportTypeFirstSync,
	ECntVCardImportTypeMerge
	};
		
class CContactVCardConverter : public CContactEcomConverter
/**
Plug-in vCard to CContactItem converter.
@publishedAll
@released
*/
	{
public:
	// Ideally each converter should be able to define its own flags relevant 
	// to its own mode of operation. Due to the legacy design (defining flags 
	// in CContactDatabase) they have to be redeclared here. Long term we hope 
	// to deprecate the CContactDatabase flags and rely on those declared in 
	// the converter implementation.
	/** Import and export format flags. Contacts can be imported into
	or exported from the contact database as vCards.

	The following options are available during import and export.
               
	@publishedAll
	@released
	*/
	enum TOptions 
	   	{
  		EDefault = CContactDatabase::EDefault,
        /** Handle Symbian's extended vCard format options. */
		EIncludeX = CContactDatabase::EIncludeX,
		/** Support non-standard extensions requried by Symbian PC connectivity software
		and Microsoft, amongst others. */
  		ETTFormat = CContactDatabase::ETTFormat ,
		/** Don't export the contact ID (for exporting only). */
		EExcludeUid = CContactDatabase::EExcludeUid,
		/** Decrease the contact's access count when importing and exporting. */
		EDecreaseAccessCount = CContactDatabase::EDecreaseAccessCount,
		/** Only import the first contact in the read stream (for importing only). */
		EImportSingleContact = CContactDatabase::EImportSingleContact,
		/** Increase the contact's access count when importing and exporting. */
		EIncreaseAccessCount = CContactDatabase::EIncreaseAccessCount,
		/** Sets a contact item to the local time when importing. */
		ELocalTime = CContactDatabase::ELocalTime,
		/** Allows to set a template to a contact item that does not exist. */
		ENullTemplateId = CContactDatabase::ENullTemplateId,
		/** Allows to set white spaces as value for an empty field.  */
		EConnectWhitespace=0x0100,		// Used to correctly manage the TimeIS protocol for syncing.
		/**
		During import ignore the UID property value of the vCard.  By ignoring
		the UID the client is indicating that it knows that the contact does not
		already exist in the database (i.e. the contact is being added rather
		than being updated).  Within the CContactVCardConverter::ImportL()
		method no attempt to search for the UID in the database will be made.
		For a series of addition of vCards with UIDs use of this option will
		give a performance improvement since the search for the UID in the
		Contacts database can become very costly as the number of contacts
		grows.
		*/
		EIgnoreUid = CContactDatabase::EConverterReserved1,		
		/**
		During import, if it is determined that the contact item being imported 
		already exists in the database then the contact item in the database will 
		be deleted before being replaced by the vCard being imported. This option
		should be used when the client does not want a merge type behaviour for
		existing contact items. This option will typically be used in a sync use case
		where the vCard data that the client is provided with has already been through a process 
		of conflict resolution and so the existing contact item should be replaced not merged.
		*/		
		EReplaceIfExists = CContactDatabase::EConverterReserved2,
  		};
  		
	/**
	@internalComponent
	@released
	*/
	enum TVersitPropertyType
		{
		EPropertyValueSingle,
		EPropertyValueComposite,
		EPropertyValueHandlingNotImplemented
		};
public: // from CContactConverter
	static CContactVCardConverter* NewL();
	CArrayPtr<CContactItem>* ImportL(CContactDatabase& aDb,RReadStream& aReadStream,TBool& aImportSuccessful,TInt aOptions,TBool aImportSingle);
	void ExportL(CContactDatabase& aDb,const CContactIdArray& aSelectedContactIds,RWriteStream& aWriteStream,TInt aOptions,const Versit::TVersitCharSet aCharSet, TBool aExportPrivateFields, TInt aCommitNumber);
public: // Utility
	static TBool ContainsExportableData(const TDesC& aText);
	static TBool ContainsImportableData(const TDesC& aText, TVersitPropertyType aType, TCntVCardImportType aImportType);
private:
	static TBool ContainsData(const TDesC& aText);
	void doImportL(CVCardToContactsAppConverter& aConverter, CParserVCard& aVCard, CContactDatabase& aDb, TInt aOption, TBool aIncAccessCount, TBool aDecAccessCount, TBool& aImportSuccessful, CArrayPtr<CContactItem>* aContactItems, TBool aIsInTransaction, TContactItemId aIdForUpdate);
	TContactItemId IsVCardMergeNeededL(CVCardToContactsAppConverter& aConverter, CParserVCard& aVCard, CContactDatabase& aDb, TInt aOption);
	void ModifyAccessCountL(CContactItem& aContact,  TBool aIncAccessCount, TBool aDecAccessCount);
	void HandleAgentsInVCardL(CVCardToContactsAppConverter& aConverter, CArrayPtr<CParserProperty>* aAgentProperties, CContactItem& aContact, CContactDatabase& aDb, TInt aOption, TBool aIncAccessCount, TBool aDecAccessCount, CArrayPtr<CContactItem>* aContactItemArray, TBool aMerge);
 	};

#endif

