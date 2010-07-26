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


#ifndef __T_UTILS2_H__
#define __T_UTILS2_H__

#include <cntviewbase.h>
#include <coreappstest/testserver.h>

//
// Forward references.
//

class CContactItem;
class CContactDatabase;

//
// CLog.
//
class CTestRegister;

class CLog : public CBase
	{
public:
	static CLog* NewL(RTest& aTest,const TDesC& aLogFileName);
	~CLog();
	void Log(TRefByValue<const TDesC> aFmt,...);
	void LogLine(TRefByValue<const TDesC> aFmt,...);
	void LogLineNoEcho(TRefByValue<const TDesC> aFmt,...);
	void WriteLogToCommPortL();
	void IncVisualCounter();
	void EndVisualCounter();
private:
	CLog(RTest& aTest,const TDesC& aLogFileName);
	void ConstructL();
	TBool ReadLogLineL(TDes& aLine);
private:
	RTest& iTest;
	RFile iLogFile;
	TFileName iLogFileName;
	TInt iVisualCounter;
	CTestRegister* iFileRegister;
	};

class CContactChangeNotifier;
const TTimeIntervalMicroSeconds32 KDefaultEventConsumptionTimeout = 500000;
class CEventConsumer : public CTimer , MContactDbObserver
	{
public:
	static CEventConsumer* NewL(CContactDatabase& aDb, TCallBack aCallBack, TTimeIntervalMicroSeconds32 aEventTimeoutInterval = KDefaultEventConsumptionTimeout);
	~CEventConsumer();
	void Start();
private:
	void ConstructL(CContactDatabase& aDb);
	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
	CEventConsumer(TCallBack aCallBack,TTimeIntervalMicroSeconds32 aEventTimeoutInterval);
	TInt RunError(TInt aError);

	void RunL();
private:
	TCallBack iCallBack;
	CContactChangeNotifier* iChangeNotifier;
	TTimeIntervalMicroSeconds32 iEventTimeoutInterval;
	};

/** Used by CRandomContactGenerator for custom random words*/
class MRandomWordGenerator
	{
public:
	virtual HBufC* WordLC() = 0;
	virtual ~MRandomWordGenerator();
	};

//
// CRandomContactGenerator.
//

class CRandomContactGenerator : public CBase
	{
public:
	static CRandomContactGenerator* NewL();
	static CRandomContactGenerator* NewL(MRandomWordGenerator* aFirstname, MRandomWordGenerator* aLastname= NULL, MRandomWordGenerator* aCompany = NULL);
	~CRandomContactGenerator();
	void SetDbL(CContactDatabase& aDb);
	TContactItemId AddTypicalRandomContactL();
	void AddAtypicalRandomContactL();
	TContactItemId AddTypicalContactForFilterL(TInt aBitWiseFilter);
	TContactItemId AddTypicalContactForFilterL(TInt aBitWiseFilter, TInt& aLeaveCode);
	TContactItemId AddTypicalContactForLocalViewPreferencesL(TContactViewPreferences aContactTypes);//creates a random contact that matches to criteria to be included in C
	TContactItemId AddTypicalContactForFilterL(TInt aBitWiseFilter, TUid aIMProtocol);
	TContactItemId AddTypicalRandomContactWithNamesL(const RArray<TInt>& iNames, TBool aWithMapping = EFalse);

	TPtrC PhoneMatch() const;
	TPtrC NameMatch() const;
	TPtrC EmailMatch() const;
private:
	CRandomContactGenerator();
	CRandomContactGenerator(MRandomWordGenerator* aFirstname, MRandomWordGenerator* aLastname, MRandomWordGenerator* aCompany );
	void ConstructL();
	void SetRandomAlphaString(TDes& aBuf,TInt aLength);
	void SetRandomNumericString(TDes& aBuf,TInt aLength);
	void SetRandomAlphaNumericString(TDes& aBuf,TInt aLength);
	void SetRandomWordL(TDes& aBuf, TInt32 aType );
	TText RandomAlphaCharCode();
	TText RandomNumericCharCode();
	TText RandomAlphaNumbericCharCode();
	TText RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException);
	TText RandomCharCode(TText aLowerBound,TText aUpperBound);
private:
	CContactDatabase* iDb;
	CContactTemplate* iTemplate;
	MRandomWordGenerator* iFirstName;
	MRandomWordGenerator* iLastName;
	MRandomWordGenerator* iCoName;		// random company name generator
	TBuf<16> iPhoneMatch;
	TBuf<64> iNameMatch;
	TBuf<64> iEmailMatch;
	TInt64 iRandSeed;
	};


//
// CContactNameGenerator.
//
/*
class CContactNameGenerator : public CBase
	{
public:
	enum TSortOrder
		{
		ELastNameFirstName,
		EFirstNameLastName
		};
public:
	static CContactNameGenerator* NewL(RTest& aTest,CLog& aLog);
	~CContactNameGenerator();
	void SetDbL(CContactDatabase& aDb);
	void AddInitialContactsL();
	TInt AddSubsequentContactsL(TInt aWhich);
	void CheckSort(TSortOrder aOrder,TContactItemId aId,TInt aIndex);
	void CheckSortLowSubView(TSortOrder aOrder,TContactItemId aId,TInt aIndex,const TDesC& aHighBoundary);
	void CheckSortHighSubView(TSortOrder aOrder,TContactItemId aId,TInt aIndex,const TDesC& aLowBoundary);
	void CheckSortSubView(TSortOrder aOrder,TContactItemId aId,TInt aIndex,const TDesC& aLowBoundary,const TDesC& aHighBoundary);
private:
	CContactNameGenerator(RTest& aTest,CLog& aLog);
	void ConstructL();
	TInt AddContactsL(const TDesC& aFile);
	TBool ReadNamePairL(RFile aFile,TDes& aFirstName,TDes& aLastName) const;
	void SortLists();
	static TInt CompareAscending(const TContact& aFirst, const TContact& aSecond);
	static TInt CompareDescending(const TContact& aFirst, const TContact& aSecond);
	TInt FindLow(TSortOrder aOrder,const TDesC& aLowBoundary);
	static TInt StaticAddContact(TAny* aSelf);
	TInt AddContactL();
private:
	RTest& iTest;
	CLog& iLog;
	RFs iFs;
	CContactDatabase* iDb;
	};
*/

//
// CTestContact.
//

class CTestContact : public CBase
	{
public:
	static CTestContact* NewL(CContactItem& aSeedContact);
	static CTestContact* NewLC(CContactItem& aSeedContact);
	~CTestContact();
	CContactItem& ContactItem();
	const TPtrC FirstNameL();
	const TPtrC LastNameL();
	const TPtrC CompanyNameL();
	const TPtrC JobTitleL();
	const TPtrC WorkPhoneL();
	const TPtrC WorkMobileL();
	const TPtrC HomePhoneL();
	const TPtrC HomeMobileL();
	const TPtrC FaxL();
	const TPtrC EmailAddressL();
	const TPtrC InternetAddressL();
	const TPtrC StreetL();
	const TPtrC TownL();
	const TPtrC CountyL();
	const TPtrC PostCodeL();
	const TPtrC CountryL();
	const TPtrC NotesL();
	void SetFirstNameL(const TDesC& aFirstName, TBool aIsPronunciation = EFalse);
	void SetLastNameL(const TDesC& aLastName, TBool aIsPronunciation = EFalse);
	void SetCompanyNameL(const TDesC& aCompanyName, TBool aIsPronunciation = EFalse);
	void SetJobTitleL(const TDesC& aJobTitle);
	void SetWorkPhoneL(const TDesC& aWorkPhone);
	void SetWorkMobileL(const TDesC& aWorkMobile);
	void SetHomePhoneL(const TDesC& aHomePhone);
	void SetHomeMobileL(const TDesC& aHomeMobile);
	void SetFaxL(const TDesC& aFax);
	void SetHomeEmailAddressL(const TDesC& aEmailAddress);
	void SetWorkEmailAddressL(const TDesC& aEmailAddress);

	void SetInternetAddressL(const TDesC& aInternetAddress);
	void SetStreetL(const TDesC& aStreet);
	void SetTownL(const TDesC& aTown);
	void SetCountyL(const TDesC& aCounty);
	void SetPostCodeL(const TDesC& aPostCode);
	void SetCountryL(const TDesC& aCountry);
	void SetNotesL(const TDesC& aNotes);
	void SetVoiceDialL(const TDesC& aVoiceDial);
	void SetRingToneL(const TDesC& aRingTone);
	void SetIMAddressL(const TDesC& aIMAddress, TUid aAddOnContentType);
	
	void SetTextFieldL(TFieldType aFieldType, const TDesC& aText);

private:
	CTestContact();
	void ConstructL(CContactItem& aSeedContact);
	const TPtrC TextFieldL(TFieldType aFieldType) const;
	const TPtrC TextFieldL(const CContentType& aTypesToMatch) const;
	void SetTextFieldL(const CContentType& aTypesToMatch,const TDesC& aText) const;
	void SetTextFieldL(TFieldType aFieldType, const TDesC& aText, TUid aAddOnContentType);
	CContactItemField& FindFieldL(TFieldType aFieldType) const;
	CContactItemField& FindFieldL(const CContentType& aTypesToMatch) const;
	TInt FindField(TFieldType aFieldType) const;
	void InsertTextFieldL(TUid aID, const TDesC& aStringOfStuff );
private:
	CContactItem* iContactItem;
	};

#endif
