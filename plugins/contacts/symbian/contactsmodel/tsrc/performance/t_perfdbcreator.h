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


#ifndef __T_PERFDBCREATOR_H__
#define __T_PERFDBCREATOR_H__



//************************************************************************************************************
#include <e32std.h>
#include <e32test.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>
#include <cntviewbase.h>
#include <ecom.h>



//************************************************************************************************************
enum TContactLevel
   {
   ELevel1=0x00,
   ELevel2=0x01,
   ELevel3=0x02,
   ELevel4=0x03	   
   };
   
enum TUserProfile
   {
   ESimple=0x00,
   EPower=0x01,
   ECorporate=0x02,
   EL1Only=0x03,
   EL2Only=0x04,
   EL3Only=0x05,
   EL4Only=0x06            
   };	   



//************************************************************************************************************
const TInt   KProfiles=7;    //7 profiles and 4 levels
const TInt   KLevels=4;
const TInt   KRatios[KProfiles][KLevels]=
   {{80 , 20 , 0  , 0  },
    {0  , 20 , 20 , 60 },
    {0  , 10 , 80 , 10 },
    {100, 0  , 0  , 0  },
    {0  , 100, 0  , 0  },
    {0  , 0  , 100, 0  },
    {0  , 0  , 0  , 100}                
   };
      
//const TInt64 KSeed=0x1234567890123456;
const TInt64 KSeed=MAKE_TINT64(0x12345678, 0x90123456);
const TInt   KCompactEvery=0x0f;



//************************************************************************************************************
//   CPerfTestContact
//************************************************************************************************************
class CPerfTestContact : public CBase
	{
public:
	static CPerfTestContact* NewL(CContactItem& aSeedContact);
	static CPerfTestContact* NewLC(CContactItem& aSeedContact);
	~CPerfTestContact();
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
	void SetWorkFaxL(const TDesC& aFax);
	void SetHomeEmailAddressL(const TDesC& aEmailAddress);
	void SetWorkEmailAddressL(const TDesC& aEmailAddress);

	void SetInternetAddressL(const TDesC& aInternetAddress);
	void SetHomeStreetL(const TDesC& aStreet);
	void SetWorkStreetL(const TDesC& aStreet);
	void SetHomeTownL(const TDesC& aTown);
	void SetWorkTownL(const TDesC& aTown);
	void SetHomeCountyL(const TDesC& aCounty);
	void SetWorkCountyL(const TDesC& aCounty);	
	void SetHomePostCodeL(const TDesC& aPostCode);
	void SetWorkPostCodeL(const TDesC& aPostCode);	
	void SetHomeCountryL(const TDesC& aCountry);
	void SetWorkCountryL(const TDesC& aCountry);	
	void SetNotesL(const TDesC& aNotes);
	void SetPictureL(const TDesC8& aPicture);
	void SetVoiceDialL(const TDesC& aVoiceDial);
	void SetRingToneL(const TDesC& aRingTone);
	void SetIMAddressL(const TDesC& aIMAddress, TUid aAddOnContentType);
	
	void SetTextFieldL(TFieldType aFieldType, const TDesC& aText);
   void SetBinaryFieldL(TFieldType aFieldType,const TDesC8& aBuf);

private:
	CPerfTestContact();
	void ConstructL(CContactItem& aSeedContact);
	const TPtrC TextFieldL(TFieldType aFieldType) const;
	const TPtrC TextFieldL(const CContentType& aTypesToMatch) const;
	void SetTextFieldL(const CContentType& aTypesToMatch,const TDesC& aText) const;
	void SetTextFieldL(TFieldType aFieldType, const TDesC& aText, TUid aAddOnContentType);
	CContactItemField& FindFieldL(TFieldType aFieldType) const;
	CContactItemField& FindFieldL(const CContentType& aTypesToMatch) const;
	TInt FindField(TFieldType aFieldType) const;
	void InsertTextFieldL(TUid aID, const TDesC& aStringOfStuff );
	void InsertBinaryFieldL(TUid aID, const TDesC8& aBufOfStuff );	
private:
	CContactItem* iContactItem;
	};







//************************************************************************************************************
//   CPerfDbCreator
//************************************************************************************************************
class CPerfDbCreator : public CBase
   {
   public:
	   static CPerfDbCreator* NewL();
	   ~CPerfDbCreator();
	   
	public:
	   void GenDbL(TUserProfile aProfile, TInt aCount, TInt aDupRatio, TBool aAddSeedPrefix, const TDesC &aFileName, TBool aWithGroups=EFalse);
      void GenAllTheSameDbL(TContactLevel aLevel, TInt aCount, TBool aAddSeedPrefix, const TDesC &aFileName);	   
	   
   protected:
      CPerfDbCreator();
	   void ConstructL();
	   
	   void GenContactsL(TContactLevel aLevel, TInt aSingles, TInt aDuplicates);
      TContactItemId AddContactL(TContactLevel aLevel);
      void AddGroupL(const TDesC & aGroupLabel);
      
	   void SetRandomAlphaString(TDes& aBuf,TInt aLength, TBool aWithSeed=false);
	   void SetRandomNumericString(TDes& aBuf,TInt aLength, TBool aWithSeed=false);
	   void SetRandomAlphaNumericString(TDes& aBuf,TInt aLength, TBool aWithSeed=false);	   

      TText RandomAlphaCharCode();
      TText RandomNumericCharCode();
      TText RandomAlphaNumbericCharCode();
      TText RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException);
      TText RandomCharCode(TText aLowerBound,TText aUpperBound);	   
	   
   protected:
      CContactDatabase*       iContactsDb;
	   CContactTemplate*       iTemplate;      
      TInt64                  iSeed;
      TBuf<16>                iTextSeed;
      TBool                   iAddSeedPrefix;
   };
   
   
   
//************************************************************************************************************
//   TPerfTimer
//************************************************************************************************************
class TPerfTimer
   {
   public:
	   TPerfTimer();
	   //~TPerfTimer();
	   
	   void GetTime(TDes &aTimeDes);
	   void GetTime(TInt aTotal, TInt aDelta);
	   void Reset();	   
	   
	private:
      TTime    iStartTime;
      TTime    iPrevTime;
   };
   

#endif  //__T_PERFDBCREATOR_H__
