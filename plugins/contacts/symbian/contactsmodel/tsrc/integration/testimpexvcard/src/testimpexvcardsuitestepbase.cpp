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


#include "testimpexvcardsuitestepbase.h"
#include "testimpexvcardsuitedefs.h"

CTestImpExvCardSuiteStepBase::~CTestImpExvCardSuiteStepBase()
	{
	}

CTestImpExvCardSuiteStepBase::CTestImpExvCardSuiteStepBase()
	{
	}

void CTestImpExvCardSuiteStepBase::OpenVCFAndImportItemL(TBuf<80> aPath, RFs aFsSession, TInt aIndex)
	{
	User::LeaveIfError(aFsSession.Connect());
	CleanupClosePushL(aFsSession);
	RFileReadStream readVCFStream;
	
	// Format buf to hold relevant vcf file
	TBuf<100> buf;
	buf.Zero();
#ifdef __WINS__	
    buf.Append(_L("c:"));
#else
    TFileName processFileName = RProcess().FileName();
    TParsePtrC parse(processFileName);
    buf.Append(parse.Drive());
#endif 
	buf.Append(aPath);
	buf.AppendNum(aIndex);
	buf.Append(_L(".vcf"));
	
	// Open vcf file
	User::LeaveIfError(readVCFStream.Open(aFsSession, buf, EFileRead));
	
	TBuf<MAX_TEXT_MESSAGE> message;
	message.Format(KOpenVCFFile, &buf);
	INFO_PRINTF1(message);
	
	CleanupClosePushL(readVCFStream);
	
	// Imports the vCard
	ImportCardL(readVCFStream);
	
	CleanupStack::PopAndDestroy(); // pop readVCFStream
	aFsSession.Close();
	CleanupStack::PopAndDestroy(); // pop aFsSession
	}
	
void CTestImpExvCardSuiteStepBase::ImportCardL(RReadStream& aReadStream)
	{
	INFO_PRINTF1(_L("Importing vCard....."));
	
	CVCardToContactsAppConverter* converter=new(ELeave)	CVCardToContactsAppConverter;
	CleanupStack::PushL(converter); // converter

	CParserVCard* vCard= CParserVCard::NewL();
	CleanupStack::PushL(vCard);
	TInt err = KErrNone;
	TRAP(err, vCard->InternalizeL(aReadStream));
	User::LeaveIfError(err);
	
	CContactItem* contactItem = NULL;
		
	// the CContactVCardConverter::ELocalTime must be used, so the program expects a local time
	contactItem = converter->GetVCardAsContactItemLC(*vCard, CVCardToContactsAppConverter::EPreserveAllProperties, 0);
	
	// the time would be automatically converted to UTC by the production codes
	iTimeFromImport = (*contactItem).LastModified();
	
	CleanupStack::PopAndDestroy(3); // GetVCardAsContactItemLC, vCard & converter
	INFO_PRINTF1(_L("Imported to Contact Entry"));
	}
	
void CTestImpExvCardSuiteStepBase::OpenBDAYVCFAndImportItemL(TBuf<80> aPath,TInt aIndex)
	{
	// Format buf to hold relevant vcf file
	TFileName fileName;
#ifdef __WINS__
	fileName.Append(_L("c:"));
#else
	TFileName processFileName = RProcess().FileName();
	TParsePtrC parse(processFileName);
	fileName.Append(parse.Drive());
#endif
	fileName.Append(aPath);
	fileName.AppendNum(aIndex);
	fileName.Append(_L(".vcf"));
	
	RFs fs; 
	User::LeaveIfError(fs.Connect());  // push fs
	CleanupClosePushL(fs);
	RFileReadStream readVCFStream;
	User::LeaveIfError(readVCFStream.Open(fs, fileName, EFileRead)); // opens vcf file
	
	TBuf<MAX_TEXT_MESSAGE> message;
	message.Format(KOpenVCFFile, &fileName);
	INFO_PRINTF1(message);
	
	CleanupClosePushL(readVCFStream);   // readVCFStream

	ImportBDAYCardL(readVCFStream);
		
	CleanupStack::PopAndDestroy(); // pop readVCFStream
	fs.Close();
	CleanupStack::PopAndDestroy(); // pop fs
	}
	
void CTestImpExvCardSuiteStepBase::ImportBDAYCardL(RReadStream& aReadStream)
	{
	INFO_PRINTF1(_L("Importing vCard....."));
	

	CVCardToContactsAppConverter* converter=new(ELeave)	CVCardToContactsAppConverter;
	CleanupStack::PushL(converter); // converter

	CParserVCard* vCard= CParserVCard::NewL();
	CleanupStack::PushL(vCard);
	TInt err = KErrNone;
	TRAP(err, vCard->InternalizeL(aReadStream));
	User::LeaveIfError(err);
	
	CContactItem* contactItem = NULL;
	
	// set to local time
	contactItem = converter->GetVCardAsContactItemLC(*vCard, CVCardToContactsAppConverter::EPreserveAllProperties, CContactVCardConverter::ELocalTime);
	
	CContactItemFieldSet& cardFields = contactItem->CardFields();
	
	INFO_PRINTF1(_L("Looking for Birthday field in Contact entry....."));
	TInt pos = cardFields.Find(KUidContactFieldBirthday);
	
	if (pos==KErrNotFound)
		{
		INFO_PRINTF1(_L("Birthday field not found in Contact entry"));
		iBDayFromImport = NULL;
		}
	else
		{
		INFO_PRINTF1(_L("Birthday field found in Contact entry"));
		CContactItemField& itemField = cardFields[pos];
	
		CContactDateField* dateTimeField = itemField.DateTimeStorage();
		CleanupStack::PushL(dateTimeField);
	
		iBDayFromImport = dateTimeField->Time();
		
		CleanupStack::Pop();
		}
	
	INFO_PRINTF1(_L("Imported to Contact Entry"));
		
	CleanupStack::PopAndDestroy(3); 
	}
	
void CTestImpExvCardSuiteStepBase::ExportItemL(TBuf<80> aPath, TBool aAddBDay)
	{
	OpenDBL();
	
	TContactItemId itemId;
	RFs fsSession;
	RFileWriteStream fileStream;

	CContactIdArray* idArray = NULL;
	CContactCard* contactAdded = NULL;
	TTime now;
	
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	
	idArray = CContactIdArray::NewL();

	CleanupStack::PushL(idArray);   // idArray

	fsSession.MkDirAll(aPath);
	User::LeaveIfError(fileStream.Replace(fsSession, aPath, EFileWrite));

	contactAdded = CContactCard::NewL();

	CleanupStack::PushL(contactAdded);   // contactAdded
	
	contactAdded->IncAccessCount();
	
	// ADD BDAY
	if (aAddBDay)
		{
		CContactItemFieldSet& fieldSet=contactAdded->CardFields();
		const TInt pos=fieldSet.Find(KUidContactFieldBirthday);
	
		if (pos!=KErrNotFound)
			{
			fieldSet[pos].DateTimeStorage()->SetTime(iBDayLocal);
			}
		else
			{
			CContactItemField* field=CContactItemField::NewLC(KStorageTypeDateTime,KUidContactFieldBirthday);
   			field->SetMapping(KUidContactFieldVCardMapBDAY);
			field->DateTimeStorage()->SetTime(iBDayLocal);
			contactAdded->AddFieldL(*field);
	
			CleanupStack::Pop(); // field
			}
		}
	
	SetNameL(*contactAdded, KUidContactFieldGivenName, _L("toby"), ETrue);
	SetNameL(*contactAdded, KUidContactFieldPhoneNumber, _L("6041233214"), ETrue);

	User::After(3000000);

	// There is no need to call SetLastModifed because
	// That function will be call by ExportSelectedContactsL below
	now.UniversalTime();
	iRecordedTime = now;
	
	INFO_PRINTF1(_L("Adding contact to Database....."));
	itemId = iDb->AddNewContactL(*contactAdded);
	INFO_PRINTF1(_L("Contact Added to Database"));
	CleanupStack::PopAndDestroy(contactAdded);  // contactAdded
	contactAdded = NULL;

	idArray->InsertL(0, itemId);

	INFO_PRINTF1(_L("Exporting Contact....."));
	iDb->ExportSelectedContactsL(TUid::Uid(KUidVCardConvDefaultImpl), *idArray, fileStream, CContactVCardConverter::EDefault, ETrue);
	INFO_PRINTF1(_L("Exported Contact"));
	fileStream.CommitL();
	fileStream.Close();
	
	CleanupStack::PopAndDestroy(idArray);  // idArray
	fsSession.Close();
	CleanupStack::PopAndDestroy();  // fsSession
	CloseDB();
	}
	
void CTestImpExvCardSuiteStepBase::ImportItemL(TBuf<80> aPath, TBool aImportBDay)
	{
	RFs fs; 
	User::LeaveIfError(fs.Connect());  // push fs
	CleanupClosePushL(fs);
	RFileReadStream readVCFStream;
	
	INFO_PRINTF1(_L("Opening exported vcf file to import, to check REV property"));
	
	User::LeaveIfError(readVCFStream.Open(fs, aPath, EFileRead));
	
	TBuf<MAX_TEXT_MESSAGE> message;
	message.Format(KOpenVCFFile, &KExportRevUTCFile);
	INFO_PRINTF1(message);
	
	CleanupClosePushL(readVCFStream);  // readVCFStream

	if (aImportBDay)
		{
		ImportBDAYCardL(readVCFStream);
		}
	else
		{
		ImportCardL(readVCFStream);
		}

	CleanupStack::PopAndDestroy();  // readVCFStream
	CleanupStack::PopAndDestroy(); // pop fs
	}

void CTestImpExvCardSuiteStepBase::OpenDBL()
	{
	if(iDb == NULL)
		{
		INFO_PRINTF1(_L("Opening Contacts Database....."));
		iDb = CContactDatabase::ReplaceL();
		INFO_PRINTF1(_L("Opened Contacts Database"));
		}
	}
	
void CTestImpExvCardSuiteStepBase::CloseDB()
	{
	delete iDb;
	iDb = NULL;
	}

void CTestImpExvCardSuiteStepBase::SetNameL(CContactItem& aItem,TUid aType,const TDesC& aName, TBool aAddField)
//
// Set the contents of a text field, creating the field if required
//
	{
	CContactItemFieldSet& fieldSet=aItem.CardFields();
	const TInt pos=fieldSet.Find(aType);
	if (!aAddField && pos!=KErrNotFound)
		{
		fieldSet[pos].TextStorage()->SetTextL(aName);
		}
	else
		{
		CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,aType);
   		field->SetMapping(KUidContactFieldVCardMapUnusedN);
		field->TextStorage()->SetTextL(aName);
		aItem.AddFieldL(*field);
		CleanupStack::Pop(); // field
		}
	}
	
TTime CTestImpExvCardSuiteStepBase::FormatDateTime(const TDesC& aParamString)
	{
	// Format of buffer to construct a TTime is YYYYMMDD:HHMMSS (15 characters).
	// TTime uses zero-based values for month and day - which is confusing for scripting.
	// In our script, we use actual month and day numbers to make things simpler so we
	// must modify the string here to take account of this.
	TBuf<32> buf;
	TInt m, d;

	buf.Zero();
	buf.Copy(aParamString.Left(4));		// The year
	TLex lexMonth = aParamString.Mid(4, 2);
	lexMonth.Val(m);
	TLex lexDay = aParamString.Mid(6, 2);
	lexDay.Val(d);
	buf.AppendFormat(_L("%02d%02d"), m - 1, d - 1);	// The month and day
	buf.Append(aParamString.Right(7));

	return TTime(buf);
	}
	
TPtrC CTestImpExvCardSuiteStepBase::GetExpectedUTCFromIniL(TInt numberOfCases,TBuf<80> aConfig, TBool aExpectedBDay)
	{
	TPtrC expectedUTC;
	TPtrC expectedBDAY;
	
	TPtrC config = aConfig;
	HBufC *configval = HBufC::NewLC(config.Length()+2);
	
	TPtr configSection = configval->Des();
	
	configSection.Append(config);
	configSection.AppendNum(numberOfCases);
	
	if (aExpectedBDay)
		{
		GetStringFromConfig(configSection,KExpectedBDay,expectedBDAY);
		CleanupStack::PopAndDestroy(configval);
		return expectedBDAY;
		}
	else
		{
		GetStringFromConfig(configSection,KExpectedUTC,expectedUTC);
		CleanupStack::PopAndDestroy(configval);
		return expectedUTC;
		}
	}

