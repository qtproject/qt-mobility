/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32math.h>
#include <e32test.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <cntfield.h>
#include "t_utils2.h"
#include <cntview.h>
#include <cntviewbase.h>
#include <coreappstest/testserver.h>
//
// Constants.
//

_LIT(KNewLine,"\n");
_LIT(KBackSpace,"\x08");
_LIT(KVisualCounterFormat,"%S%d");
_LIT8(KUnicodeStartFile,"\xFF\xFE");
_LIT8(KUnicodeLineEnd,"\x0D\x00\x0a\x00");
const TInt KMaxLogLength=0x100;




//
// CLog.
//

CLog* CLog::NewL(RTest& aTest,const TDesC& aLogFileName)
	{
	CLog* self=new(ELeave) CLog(aTest,aLogFileName);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CLog::CLog(RTest& aTest,const TDesC& aLogFileName)
	: iTest(aTest),iLogFileName(aLogFileName),iVisualCounter(-1)
	{
	}

CLog::~CLog()
	{
	iLogFile.Close();
	delete iFileRegister;
	}

void CLog::ConstructL()
	{
	iFileRegister = CTestRegister::NewLC();
	CleanupStack::Pop(iFileRegister);
	
	iFileRegister->CreateLogFileLC(iLogFile, iLogFileName);
	User::LeaveIfError(iLogFile.Write(KUnicodeStartFile));
	CleanupStack::Pop(&iLogFile); // This file stays open
	}

void CLog::Log(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<KMaxLogLength> buf;
	buf.FormatList(aFmt,list);

	iLogFile.Write(TPtrC8((TUint8 *)buf.Ptr(),buf.Size()));
	iLogFile.Flush();
	iTest.Printf(buf);
	}

void CLog::LogLine(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<KMaxLogLength> buf;
	buf.FormatList(aFmt,list);

	iLogFile.Write(TPtrC8((TUint8 *)buf.Ptr(),buf.Size()));
	iLogFile.Write(KUnicodeLineEnd);
	iLogFile.Flush();
	buf.Append(KNewLine);
	iTest.Printf(buf);
	}

void CLog::LogLineNoEcho(TRefByValue<const TDesC> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<KMaxLogLength> buf;
	buf.FormatList(aFmt,list);

	iLogFile.Write(TPtrC8((TUint8 *)buf.Ptr(),buf.Size()));
	iLogFile.Write(KUnicodeLineEnd);
	iLogFile.Flush();
	}

void CLog::WriteLogToCommPortL()
	{
	iLogFile.Close();
	iFileRegister->OpenLogFileLC(iLogFile, iLogFileName);
	CleanupStack::Pop(&iLogFile);
	TInt pos=2;
	iLogFile.Seek(ESeekStart,pos); // Ignore Unicode header.
	TBuf<KMaxLogLength> line;
	while (ReadLogLineL(line))
		{
		RDebug::Print(line);
		}
	}

TBool CLog::ReadLogLineL(TDes& aLine)
	{
	TBuf8<2> character=_L8("  ");
	TBuf8<4> lineEndTestBuf=_L8("    ");

	aLine.SetLength(0);
	while (lineEndTestBuf!=KUnicodeLineEnd)
		{
		User::LeaveIfError(iLogFile.Read(character,2));
		if (character.Length()>0)
			{
			lineEndTestBuf[0]=lineEndTestBuf[2];
			lineEndTestBuf[1]=lineEndTestBuf[3];
			lineEndTestBuf[2]=character[0];
			lineEndTestBuf[3]=character[1];

			const TPtrC16 widePtr((TUint16*)character.Ptr(),1);
			aLine.Append(widePtr);
			}
		else
			{
			return EFalse;
			}
		}

	aLine=aLine.Left(aLine.Length()-2); // Strip off \n\r.
	return ETrue;
	}

void CLog::IncVisualCounter()
	{
	TBuf<8> backSpaceBuf;

	if (iVisualCounter>=0)
		{
		TInt numDigits=1;
		TInt divisor=10;

		FOREVER
			{
			if (iVisualCounter/divisor>0)
				{
				divisor*=10;
				++numDigits;
				}
			else
				{
				break;
				}
			}

		for (TInt ii=0;ii<numDigits;++ii)
			{
			backSpaceBuf.Append(KBackSpace);
			}
		}

	iTest.Printf(KVisualCounterFormat,&backSpaceBuf,++iVisualCounter);
	}

void CLog::EndVisualCounter()
	{
	iTest.Printf(KNewLine);
	iVisualCounter=-1;
	}




//
// CRandomContactGenerator.
//

CRandomContactGenerator::CRandomContactGenerator()
	{
	}

CRandomContactGenerator::CRandomContactGenerator(MRandomWordGenerator* aFirstname, MRandomWordGenerator* aLastname, MRandomWordGenerator* aCompany ) :
	iFirstName(aFirstname)
	{
	if(aLastname)
		{
		iLastName = aLastname;
		}
	else
		{
		iLastName = iFirstName;
		}
	if(aCompany)
		{
		iCoName = aCompany;
		}
	else
		{
		iCoName = iLastName;
		}
	}

CRandomContactGenerator::~CRandomContactGenerator()
	{
	delete iTemplate;
	delete iFirstName;
	if (iLastName != iFirstName)
		delete iLastName;
	if (iCoName != iFirstName && iCoName != iLastName)
		delete iCoName;
	}

CRandomContactGenerator* CRandomContactGenerator::NewL()
	{
	CRandomContactGenerator* self=new(ELeave) CRandomContactGenerator();
	CleanupStack::PushL(self);
	//self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

/** Takes ownership of the args. If this Leaves the args will be deleted. Args can be duplicates
	If aCompany is NULL, this uses aLastname to generate company names. 
	If aLastname is NULL, this uses aFirstname to generate last names (and, potentially, company names as well) */
CRandomContactGenerator* CRandomContactGenerator::NewL(MRandomWordGenerator* aFirstname, MRandomWordGenerator* aLastname, MRandomWordGenerator* aCompany )
	{
	CRandomContactGenerator* self=new(ELeave) CRandomContactGenerator(aFirstname, aLastname, aCompany);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

void CRandomContactGenerator::ConstructL()
	{}


void CRandomContactGenerator::SetDbL(CContactDatabase& aDb)
	{
	iDb=&aDb;
	iTemplate=STATIC_CAST(CContactTemplate*,iDb->ReadContactL(iDb->TemplateId()));
	}

TPtrC CRandomContactGenerator::PhoneMatch() const
	{
	return TPtrC(iPhoneMatch);
	}

TPtrC CRandomContactGenerator::NameMatch() const
	{
	return TPtrC(iNameMatch);
	}

TPtrC CRandomContactGenerator::EmailMatch() const
	{
	return TPtrC(iEmailMatch);
	}
	
void CRandomContactGenerator::SetRandomWordL(TDes& aBuf, TInt32 aType )
	{
	MRandomWordGenerator* word = iFirstName;
	TInt len = 16;
	switch (aType)
		{
		case KUidContactFieldFamilyNameValue:
		case KUidContactFieldFamilyNamePronunciationValue:
			word = iLastName;
			break;
		case KUidContactFieldCompanyNameValue:
		case KUidContactFieldCompanyNamePronunciationValue:
			word = iCoName;
			break;
		case KUidContactFieldEMailValue:
			len = 32;
			break;
		}
	if(!word)
		{
		SetRandomAlphaString(aBuf,len);
		return;
		}
	aBuf = *word->WordLC();
	CleanupStack::PopAndDestroy();
	
	// the following are to make these look a bite more realistic
	if(aType==KUidContactFieldEMailValue)	
		{
		aBuf.Append('@');
		aBuf.Append(*word->WordLC());
		CleanupStack::PopAndDestroy();
		aBuf.Append(_L(".co.uk"));
		}
	if((aType==KUidContactFieldCompanyNameValue || aType==KUidContactFieldCompanyNamePronunciationValue) 
			&& word==iLastName)
		{
		aBuf.Append(' ');
		aBuf.Append(*word->WordLC());
		CleanupStack::PopAndDestroy();
		aBuf.Append(_L(" Ltd."));
		}
	}

TContactItemId CRandomContactGenerator::AddTypicalRandomContactL()
	{
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	HBufC* buf=HBufC::NewLC(64);
	TPtr bufPtr=buf->Des();
	SetRandomWordL(bufPtr,KUidContactFieldGivenNameValue);
	contact->SetFirstNameL(bufPtr);
	iNameMatch=bufPtr;
	SetRandomWordL(bufPtr,KUidContactFieldFamilyNameValue);
	contact->SetLastNameL(bufPtr);
	SetRandomWordL(bufPtr,KUidContactFieldCompanyNameValue);
	contact->SetCompanyNameL(bufPtr);
	SetRandomNumericString(bufPtr,16);
	contact->SetWorkPhoneL(bufPtr);
	iPhoneMatch=bufPtr;
	SetRandomNumericString(bufPtr,16);
	contact->SetWorkMobileL(bufPtr);
	SetRandomNumericString(bufPtr,16);
	contact->SetHomePhoneL(bufPtr);
	SetRandomWordL(bufPtr,KUidContactFieldEMailValue);
	contact->SetWorkEmailAddressL(bufPtr);
	iEmailMatch=bufPtr;

	TContactItemId id = iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(2); // buf, contact.
	return id;
	}

TContactItemId CRandomContactGenerator::AddTypicalRandomContactWithNamesL(const RArray<TInt>& iNames, TBool aWithMapping)
	{
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	HBufC* buf=HBufC::NewLC(64);
	TPtr bufPtr=buf->Des();
	TInt i;
	for(i=iNames.Count()-1;i>=0;i--) 
		{
		SetRandomWordL(bufPtr,iNames[i]);
		if(!aWithMapping )
			{
			contact->SetTextFieldL(TUid::Uid(iNames[i]),bufPtr);
			}
		else
			{
			switch (iNames[i])
				{
				case KUidContactFieldCompanyNameValue: 
					contact->SetCompanyNameL(bufPtr);
					break;
				case KUidContactFieldCompanyNamePronunciationValue:
					contact->SetCompanyNameL(bufPtr, ETrue);
					break;
				case KUidContactFieldGivenNameValue:
					contact->SetFirstNameL(bufPtr);
					break;
				case KUidContactFieldGivenNamePronunciationValue:
					contact->SetFirstNameL(bufPtr, ETrue);
					break;
				case KUidContactFieldFamilyNameValue:
					contact->SetLastNameL(bufPtr);
					break;
				case KUidContactFieldFamilyNamePronunciationValue:
					contact->SetLastNameL(bufPtr, ETrue);
					break;
				default:
					contact->SetTextFieldL(TUid::Uid(iNames[i]),bufPtr);
				}
			}
		iNameMatch=bufPtr;
	}
	
	SetRandomNumericString(bufPtr,16);
	contact->SetWorkPhoneL(bufPtr);
	iPhoneMatch=bufPtr;
	SetRandomNumericString(bufPtr,16);
	contact->SetWorkMobileL(bufPtr);
	SetRandomNumericString(bufPtr,16);
	contact->SetHomePhoneL(bufPtr);
	SetRandomWordL(bufPtr,KUidContactFieldEMailValue);
	contact->SetWorkEmailAddressL(bufPtr);
	iEmailMatch=bufPtr;

	TContactItemId id = iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(2); // buf, contact.
	return id;
	}

void CRandomContactGenerator::AddAtypicalRandomContactL()
	{
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	HBufC* buf=HBufC::NewLC(256);
	TPtr bufPtr=buf->Des();
	SetRandomAlphaString(bufPtr,32);
	contact->SetFirstNameL(bufPtr);
	SetRandomAlphaString(bufPtr,32);
	contact->SetLastNameL(bufPtr);
	SetRandomAlphaString(bufPtr,32);
	contact->SetCompanyNameL(bufPtr);
	SetRandomAlphaString(bufPtr,32);
	contact->SetJobTitleL(bufPtr);
	SetRandomNumericString(bufPtr,24);
	contact->SetWorkPhoneL(bufPtr);
	SetRandomNumericString(bufPtr,24);
	contact->SetWorkMobileL(bufPtr);
	SetRandomNumericString(bufPtr,24);
	contact->SetHomePhoneL(bufPtr);
	SetRandomNumericString(bufPtr,24);
	contact->SetHomeMobileL(bufPtr);
	SetRandomNumericString(bufPtr,24);
	contact->SetFaxL(bufPtr);
	SetRandomAlphaNumericString(bufPtr,48);
	contact->SetWorkEmailAddressL(bufPtr);
	SetRandomAlphaNumericString(bufPtr,48);
	contact->SetInternetAddressL(bufPtr);
	SetRandomAlphaNumericString(bufPtr,32);
	contact->SetStreetL(bufPtr);
	SetRandomAlphaString(bufPtr,32);
	contact->SetTownL(bufPtr);
	SetRandomAlphaString(bufPtr,32);
	contact->SetCountyL(bufPtr);
	SetRandomAlphaNumericString(bufPtr,16);
	contact->SetPostCodeL(bufPtr);
	SetRandomAlphaString(bufPtr,32);
	contact->SetCountryL(bufPtr);
	SetRandomAlphaNumericString(bufPtr,256);
	contact->SetNotesL(bufPtr);

	iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(2); // buf, contact.
	}

TContactItemId CRandomContactGenerator::AddTypicalContactForFilterL(TInt aBitWiseFilter, TInt& aLeaveCode)
	{
	TContactItemId retval = 0;
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	HBufC* buf=HBufC::NewLC(32);
	TPtr bufPtr=buf->Des();
	SetRandomAlphaString(bufPtr,16);
	contact->SetFirstNameL(bufPtr);
	SetRandomAlphaString(bufPtr,16);
	contact->SetLastNameL(bufPtr);
	SetRandomAlphaString(bufPtr,16);
	contact->SetCompanyNameL(bufPtr);

	if(aBitWiseFilter&CContactDatabase::ELandLine)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetWorkPhoneL(bufPtr);
		}
	if(aBitWiseFilter&CContactDatabase::ESmsable)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetWorkMobileL(bufPtr);
		}
	if(aBitWiseFilter&CContactDatabase::EMailable)
		{
		SetRandomAlphaNumericString(bufPtr,32);
		if (aBitWiseFilter&CContactDatabase::EHome)
			contact->SetHomeEmailAddressL(bufPtr);
		else
			contact->SetWorkEmailAddressL(bufPtr);
		}
	if(aBitWiseFilter&CContactDatabase::EFaxable)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetFaxL(bufPtr);
		}


	TRAP( aLeaveCode, { retval = iDb->AddNewContactL(contact->ContactItem()); } );
	CleanupStack::PopAndDestroy(2); // buf, contact.
	return retval;
	}

TContactItemId CRandomContactGenerator::AddTypicalContactForFilterL(TInt aBitWiseFilter)
	{
	TUid fakeUid = TUid::Null();
	return AddTypicalContactForFilterL(aBitWiseFilter, fakeUid);
	}
	
TContactItemId CRandomContactGenerator::AddTypicalContactForFilterL(TInt aBitWiseFilter, TUid aIMProtocol)
	{
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	HBufC* buf=HBufC::NewLC(32);
	TPtr bufPtr=buf->Des();
	SetRandomAlphaString(bufPtr,16);
	contact->SetFirstNameL(bufPtr);
	SetRandomAlphaString(bufPtr,16);
	contact->SetLastNameL(bufPtr);
	SetRandomAlphaString(bufPtr,16);
	contact->SetCompanyNameL(bufPtr);

	if(aBitWiseFilter&CContactDatabase::ELandLine)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetWorkPhoneL(bufPtr);
		}
	if(aBitWiseFilter&CContactDatabase::ESmsable)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetWorkMobileL(bufPtr);
		}
	if(aBitWiseFilter&CContactDatabase::EMailable)
		{
		SetRandomAlphaNumericString(bufPtr,32);
		if (aBitWiseFilter&CContactDatabase::EHome)
			contact->SetHomeEmailAddressL(bufPtr);
		else
			contact->SetWorkEmailAddressL(bufPtr);
		}
	if(aBitWiseFilter&CContactDatabase::EFaxable)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetFaxL(bufPtr);
		}

	if(aBitWiseFilter&CContactDatabase::ERingTone)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetRingToneL(bufPtr);
		}
	if(aBitWiseFilter&CContactDatabase::EVoiceDial)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetVoiceDialL(bufPtr);
		}

	if(aBitWiseFilter&CContactDatabase::EWirelessVillage)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetIMAddressL(bufPtr, aIMProtocol);
		}		

	TContactItemId id = iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(2); // buf, contact.
	return id;
	}
	

TContactItemId CRandomContactGenerator::AddTypicalContactForLocalViewPreferencesL(TContactViewPreferences aContactTypes)
	{
	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	HBufC* buf=HBufC::NewLC(32);
	TPtr bufPtr=buf->Des();
				
	if(aContactTypes&EUnSortedAtBeginning || aContactTypes&EUnSortedAtEnd)
		{
		SetRandomNumericString(bufPtr,16);
		contact->SetWorkPhoneL(bufPtr);
		}

	if(aContactTypes&ESingleWhiteSpaceIsEmptyField)
		{
		contact->SetFirstNameL(_L(" "));
		contact->SetCompanyNameL(_L(" "));

		SetRandomAlphaString(bufPtr,16);
		contact->SetLastNameL(bufPtr);

		SetRandomNumericString(bufPtr,16);
		contact->SetWorkPhoneL(bufPtr);
		}


	TContactItemId newId = iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(2); // buf, contact.
	return newId;
	}

void CRandomContactGenerator::SetRandomAlphaString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt ii=0;ii<aLength;++ii)
		{
		aBuf[ii]=RandomAlphaCharCode();
		}
	}

void CRandomContactGenerator::SetRandomNumericString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt ii=0;ii<aLength;++ii)
		{
		aBuf[ii]=RandomNumericCharCode();
		}
	}

void CRandomContactGenerator::SetRandomAlphaNumericString(TDes& aBuf,TInt aLength)
	{
	aBuf.SetLength(aLength);

	for (TInt ii=0;ii<aLength;++ii)
		{
		aBuf[ii]=RandomAlphaNumbericCharCode();
		}
	}

TText CRandomContactGenerator::RandomAlphaCharCode()
	{
	return RandomCharCode('A','z',' ');
	}

TText CRandomContactGenerator::RandomNumericCharCode()
	{
	return RandomCharCode('0','9',' ');
	}

TText CRandomContactGenerator::RandomAlphaNumbericCharCode()
	{
	return RandomCharCode('0','z',' ');
	}

TText CRandomContactGenerator::RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException)
	{
	TText charCode=0;
	
	do
		{
		charCode=RandomCharCode(aLowerBound,aUpperBound);
		}
		while (charCode==aException);

	return charCode;
	}

TText CRandomContactGenerator::RandomCharCode(TText aLowerBound,TText aUpperBound)
	{
	TText charCode=STATIC_CAST(TText,(Math::Rand(iRandSeed)%(aUpperBound-aLowerBound))+aLowerBound);
	ASSERT(charCode>=aLowerBound && charCode<=aUpperBound);
	return charCode;
	}


//
// CContactNameGenerator.
//
/*
CContactNameGenerator* CContactNameGenerator::NewL(RTest& aTest,CLog& aLog)
	{
	CContactNameGenerator* self=new(ELeave) CContactNameGenerator(aTest,aLog);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self.
	return self;
	}

CContactNameGenerator::CContactNameGenerator(RTest& aTest,CLog& aLog)
	: iTest(aTest),iLog(aLog)
	{
	}

CContactNameGenerator::~CContactNameGenerator()
	{
	}

void CContactNameGenerator::ConstructL()
	{
	}

void CContactNameGenerator::SetDbL(CContactDatabase& aDb)
	{
	iDb=&aDb;
	iTemplate=STATIC_CAST(CContactTemplate*,iDb->ReadContactL(iDb->TemplateId()));
	}
*/

//
// CTestContact.
//

CTestContact* CTestContact::NewL(CContactItem& aSeedContact)
	{
	CTestContact* self=CTestContact::NewLC(aSeedContact);
	CleanupStack::Pop();
	return self;
	}

CTestContact* CTestContact::NewLC(CContactItem& aSeedContact)
	{
	CTestContact* self=new(ELeave) CTestContact();
	CleanupStack::PushL(self);
	self->ConstructL(aSeedContact);
	return self;
	}

CTestContact::CTestContact()
	{
	}

void CTestContact::ConstructL(CContactItem& aSeedContact)
	{
	iContactItem=CContactCard::NewL(&aSeedContact);
	}

CTestContact::~CTestContact()
	{
	delete iContactItem;
	}

CContactItem& CTestContact::ContactItem()
	{
	return *iContactItem;
	}

const TPtrC CTestContact::FirstNameL()
	{
	return TextFieldL(KUidContactFieldGivenName);
	}

const TPtrC CTestContact::LastNameL()
	{
	return TextFieldL(KUidContactFieldFamilyName);
	}

const TPtrC CTestContact::CompanyNameL()
	{
	return TextFieldL(KUidContactFieldCompanyName);
	}

const TPtrC CTestContact::JobTitleL()
	{
	return TextFieldL(KUidContactFieldJobTitle);
	}

const TPtrC CTestContact::WorkPhoneL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::WorkMobileL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::HomePhoneL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::HomeMobileL()
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	const TPtrC text=TextFieldL(*contentType);
	CleanupStack::PopAndDestroy(); // contentType.
	return text;
	}

const TPtrC CTestContact::FaxL()
	{
	return TextFieldL(KUidContactFieldFax);
	}

const TPtrC CTestContact::EmailAddressL()
	{
	return TextFieldL(KUidContactFieldEMail);
	}

const TPtrC CTestContact::InternetAddressL()
	{
	return TextFieldL(KUidContactFieldUrl);
	}

const TPtrC CTestContact::StreetL()
	{
	return TextFieldL(KUidContactFieldAddress);
	}

const TPtrC CTestContact::TownL()
	{
	return TextFieldL(KUidContactFieldLocality);
	}

const TPtrC CTestContact::CountyL()
	{
	return TextFieldL(KUidContactFieldRegion);
	}

const TPtrC CTestContact::PostCodeL()
	{
	return TextFieldL(KUidContactFieldPostcode);
	}

const TPtrC CTestContact::CountryL()
	{
	return TextFieldL(KUidContactFieldCountry);
	}

const TPtrC CTestContact::NotesL()
	{
	return TextFieldL(KUidContactFieldNote);
	}

void CTestContact::SetFirstNameL(const TDesC& aFirstName, TBool aIsPronunciation)
	{
	const TUid id = aIsPronunciation ? KUidContactFieldGivenNamePronunciation : KUidContactFieldGivenName;
	SetTextFieldL(id,aFirstName, KUidContactFieldVCardMapUnusedN);
	}

void CTestContact::SetLastNameL(const TDesC& aLastName, TBool aIsPronunciation)
	{
	const TUid id = aIsPronunciation ? KUidContactFieldFamilyNamePronunciation : KUidContactFieldFamilyName;
	SetTextFieldL(id,aLastName, KUidContactFieldVCardMapUnusedN);
	}

void CTestContact::SetCompanyNameL(const TDesC& aCompanyName, TBool aIsPronunciation)
	{	
	if ( aIsPronunciation )
		SetTextFieldL(KUidContactFieldCompanyNamePronunciation,aCompanyName, KUidContactFieldVCardMapORGPronunciation);
	else
		SetTextFieldL(KUidContactFieldCompanyName,aCompanyName, KUidContactFieldVCardMapORG);
	}

void CTestContact::SetJobTitleL(const TDesC& aJobTitle)
	{
	SetTextFieldL(KUidContactFieldJobTitle,aJobTitle);
	}

void CTestContact::SetWorkPhoneL(const TDesC& aWorkPhone)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(*contentType,aWorkPhone);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetWorkMobileL(const TDesC& aWorkMobile)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(*contentType,aWorkMobile);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetHomePhoneL(const TDesC& aHomePhone)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	SetTextFieldL(*contentType,aHomePhone);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetHomeMobileL(const TDesC& aHomeMobile)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldPhoneNumber);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	SetTextFieldL(*contentType,aHomeMobile);
	CleanupStack::PopAndDestroy(); // contentType.
	}

void CTestContact::SetFaxL(const TDesC& aFax)
	{
	SetTextFieldL(KUidContactFieldFax,aFax);
	}

void CTestContact::SetHomeEmailAddressL(const TDesC& aEmailAddress)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapHOME);
	SetTextFieldL(*contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType);
	}

void CTestContact::SetWorkEmailAddressL(const TDesC& aEmailAddress)
	{
	CContentType* contentType=CContentType::NewL();
	CleanupStack::PushL(contentType);
	contentType->AddFieldTypeL(KUidContactFieldEMail);
	contentType->AddFieldTypeL(KUidContactFieldVCardMapWORK);
	SetTextFieldL(*contentType,aEmailAddress);
	CleanupStack::PopAndDestroy(contentType); 
	}


void CTestContact::SetInternetAddressL(const TDesC& aInternetAddress)
	{
	SetTextFieldL(KUidContactFieldUrl,aInternetAddress);
	}

void CTestContact::SetStreetL(const TDesC& aStreet)
	{
	SetTextFieldL(KUidContactFieldAddress,aStreet);
	}

void CTestContact::SetTownL(const TDesC& aTown)
	{
	SetTextFieldL(KUidContactFieldLocality,aTown);
	}

void CTestContact::SetCountyL(const TDesC& aCounty)
	{
	SetTextFieldL(KUidContactFieldRegion,aCounty);
	}

void CTestContact::SetPostCodeL(const TDesC& aPostCode)
	{
	SetTextFieldL(KUidContactFieldPostcode,aPostCode);
	}

void CTestContact::SetCountryL(const TDesC& aCountry)
	{
	SetTextFieldL(KUidContactFieldCountry,aCountry);
	}

void CTestContact::SetNotesL(const TDesC& aNotes)
	{
	SetTextFieldL(KUidContactFieldNote,aNotes);
	}

void CTestContact::SetIMAddressL(const TDesC& aIMAddress, TUid aAddOnContentType)
	{
	SetTextFieldL(KUidContactFieldIMAddress, aIMAddress, aAddOnContentType);
	}

void CTestContact::InsertTextFieldL(TFieldType aID, const TDesC& aStringOfStuff )
	{
	CContactItemField* itemFieldPtr = NULL;
	CContactTextField* textStoragePtr = NULL;

	itemFieldPtr = CContactItemField::NewLC( KStorageTypeText, aID );
	textStoragePtr = itemFieldPtr->TextStorage(); // we don't have ownership here, hence no stack.

	textStoragePtr->SetTextL( aStringOfStuff );
	textStoragePtr = NULL; // we've finished with this ptr now.

	iContactItem->AddFieldL( *itemFieldPtr ); // hand ownership over here
	CleanupStack::Pop( itemFieldPtr ); // and pop,
	}

void CTestContact::SetVoiceDialL(const TDesC& aVoiceDial)
	{
	SetTextFieldL( KUidContactsVoiceDialField, aVoiceDial );
	}

void CTestContact::SetRingToneL(const TDesC& aRingTone)
	{
	SetTextFieldL( KUidContactFieldRingTone, aRingTone );
	}

const TPtrC CTestContact::TextFieldL(TFieldType aFieldType) const
	{
	CContactItemField& field=FindFieldL(aFieldType);
	ASSERT(field.StorageType()==KStorageTypeText);
	return STATIC_CAST(CContactTextField*,field.Storage())->Text();
	}

const TPtrC CTestContact::TextFieldL(const CContentType& aTypesToMatch) const
	{
	CContactItemField& field=FindFieldL(aTypesToMatch);
	ASSERT(field.StorageType()==KStorageTypeText);
	return STATIC_CAST(CContactTextField*,field.Storage())->Text();
	}

// CHANGE, the original SetTextFieldL is const ?!
// ----- Seems really daft since even the very name of the function
// ----- implies that it's modifying data... "SetXXXXX"
void CTestContact::SetTextFieldL(TFieldType aFieldType,const TDesC& aText)
	{
	CContactItemFieldSet& fields = iContactItem->CardFields();
	TInt fieldIndex = 0;

	fieldIndex = FindField( aFieldType );

	if ( fieldIndex > KErrNotFound )
		{
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType()==KStorageTypeText);
		STATIC_CAST(CContactTextField*,field.Storage())->SetText(aText.AllocL());
		}
	else
		{
		InsertTextFieldL(aFieldType, aText);
		}
	}

void CTestContact::SetTextFieldL(const CContentType& aTypesToMatch,const TDesC& aText) const
	{
	CContactItemField& field=FindFieldL(aTypesToMatch);
	ASSERT(field.StorageType()==KStorageTypeText);
	STATIC_CAST(CContactTextField*,field.Storage())->SetText(aText.AllocL());
	}

void CTestContact::SetTextFieldL(TFieldType aFieldType, const TDesC& aText, TUid aAddOnContentType) 
	{
	CContactItemFieldSet& fields = iContactItem->CardFields();
	CContactTextField* textField = NULL;
	TInt fieldIndex = 0;

	fieldIndex = FindField( aFieldType );

	if (fieldIndex == KErrNotFound )
		{
		InsertTextFieldL( aFieldType, aText );
		fieldIndex = FindField( aFieldType );
		CContactItemField& field = fields[ fieldIndex ];
		field.AddFieldTypeL( aAddOnContentType );
		}
	else if (fieldIndex >= KErrNone )
		{
		
		CContactItemField& field = fields[ fieldIndex ];
		ASSERT(field.StorageType()==KStorageTypeText);
		textField = static_cast<CContactTextField*>(field.Storage());
		textField->SetText( aText.AllocL() );
		field.AddFieldTypeL( aAddOnContentType );
		}
	}

CContactItemField& CTestContact::FindFieldL(TFieldType aFieldType) const
	{
	CContactItemFieldSet& fields=iContactItem->CardFields();
	TInt index=fields.Find(aFieldType);
	User::LeaveIfError(index);
	return fields[index];
	}

TInt CTestContact::FindField(TFieldType aFieldType) const
	{
	return iContactItem->CardFields().Find( aFieldType );
	}

CContactItemField& CTestContact::FindFieldL(const CContentType& aTypesToMatch) const
	{
	CContactItemFieldSet& fields=iContactItem->CardFields();
	const TInt numTypesToMatch(aTypesToMatch.FieldTypeCount());
	const TFieldType mainFieldType(aTypesToMatch.FieldType(0));
	TInt index=-1;
	FOREVER
		{
		index=fields.FindNext(mainFieldType,index+1);
		User::LeaveIfError(index);

		CContactItemField& thisPhoneField=fields[index];
		const CContentType& thisPhoneFieldTypes=thisPhoneField.ContentType();
		const TInt numFieldTypes(thisPhoneFieldTypes.FieldTypeCount());

		if (numFieldTypes!=numTypesToMatch)
			{
			continue;
			}

		TInt numTypesLeftToMatch=numTypesToMatch;
		for (TInt ii=0;ii<numFieldTypes;++ii)
			{
			TFieldType thisFieldType(thisPhoneFieldTypes.FieldType(ii));
			for (TInt jj=0;jj<numTypesToMatch;++jj)
				{
				TFieldType thisFieldTypeToMatch(aTypesToMatch.FieldType(jj));
				if (thisFieldType==thisFieldTypeToMatch)
					{
					if (--numTypesLeftToMatch<=0)
						{
						return thisPhoneField;
						}

					break;
					}
				}
			}
		}
	}

MRandomWordGenerator::~MRandomWordGenerator()
	{}

///////
// Class that consumes Database events up to a timeout period.
// At timeout it calls back to a parent class.
// Timer is reset each time a database event is received.
CEventConsumer* CEventConsumer::NewL(CContactDatabase& aDb,TCallBack aCallBack, TTimeIntervalMicroSeconds32 aEventTimeoutInterval)
	{
	CEventConsumer* self = new (ELeave) CEventConsumer(aCallBack,aEventTimeoutInterval);
	CleanupStack::PushL(self);
	self->ConstructL(aDb);
	CleanupStack::Pop(self);
	return self;
	}

// From MContactDbObserver - Ignores/consumes the event.
void CEventConsumer::HandleDatabaseEventL(TContactDbObserverEvent /*aEvent*/)
	{
	// Timer base class cancel actually cancels the timer
	Cancel();
	// Start the timer again
	After(iEventTimeoutInterval);
	}

CEventConsumer::CEventConsumer(TCallBack aCallBack, TTimeIntervalMicroSeconds32 aEventTimeoutInterval) : CTimer(EPriorityNormal), iCallBack(aCallBack), iEventTimeoutInterval(aEventTimeoutInterval)
	{
	CActiveScheduler::Add(this);
	}

void CEventConsumer::RunL()
	{
	// Callback to parent on timer expiry.
	iCallBack.CallBack();
	}
	
TInt CEventConsumer::RunError(TInt aError)
    {
	//Problem, RunL left
    return aError;
    }


void CEventConsumer::Start()
	{
	After(iEventTimeoutInterval);	
	}

CEventConsumer::~CEventConsumer()
	{
	// Timer base class cancel actually cancels the timer
	Cancel();
	delete iChangeNotifier;
	}

void CEventConsumer::ConstructL(CContactDatabase& aDb)
	{
	// Must call base class construction.
	CTimer::ConstructL();
	// Class enables us to be a database observer.
	iChangeNotifier = CContactChangeNotifier::NewL(aDb,this);
	}
