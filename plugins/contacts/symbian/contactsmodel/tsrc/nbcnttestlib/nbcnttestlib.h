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



#ifndef __NBCNTTESTLIB_H__
#define __NBCNTTESTLIB_H__

#include <cntitem.h>

IMPORT_C void PrintContactL(const TDesC& aFilename, const CContactItem& aContact);

const TInt KEmailAddrLen = 4;

class CCntItemModifier;

class CCntItemBuilder : public CBase
{
public:
	IMPORT_C static CCntItemBuilder* NewLC(const CContactTemplate& aTemplate);
	IMPORT_C ~CCntItemBuilder(); 
	IMPORT_C CContactItem* GetCntItemLC();// Returning ownership.

private:
	void ConstructL();
	inline CCntItemBuilder  (const CContactTemplate& aTemplate);
	void FillWithRandomDataL(CContactItem& aContactItem);
	void SetRandomWordL		(TDes& aBuf, TInt32 aType);
	void SetRandomAlphaString(TDes& aBuf,TInt aLength);
	
	TText RandomAlphaCharCode	();
	TText RandomNumericCharCode ();
	TText RandomAlphaNumbericCharCode();
	TText RandomCharCode		(TText aLowerBound,TText aUpperBound,TText aException);
	TText RandomCharCode		(TText aLowerBound,TText aUpperBound);
	void SetRandomNumericString (TDes& aBuf, TInt aLength);
	void SetRandomAlphaNumericString(TDes& aBuf,TInt aLength);

private:
	const CContactTemplate& 	 iTemplate;	
	CCntItemModifier*			 iCntModifier;
};


class CCntItemModifier : public CBase
{
public:
	IMPORT_C static CCntItemModifier* NewLC();
	IMPORT_C ~CCntItemModifier(); 
	IMPORT_C void SetTextFieldL		(TFieldType aFieldType, const TDesC& aText, CContactItem& aContactItem);
	IMPORT_C void SetTextFieldL		(const CContentType& aTypesToMatch,const TDesC& aText, CContactItem& aContactItem) const;
	IMPORT_C void SetTextFieldL		(TFieldType aFieldType, TFieldType a2ndFieldType,
							TFieldType a3rdFieldType, const TDesC& aText, 
							CContactItem& aContactItem) const;
	IMPORT_C void SetTextFieldL		(TFieldType aFieldType, TFieldType a2ndFieldType,
							const TDesC& aText, CContactItem& aContactItem) const;
	IMPORT_C void SetTextFieldL		(TFieldType aFieldType, TFieldType a2ndFieldType,
							TFieldType a3rdFieldType, TFieldType a4thFieldType, 
							const TDesC& aText, CContactItem& aContactItem) const;
	
	IMPORT_C TBool FindTextFieldL	(TFieldType aFieldType, TFieldType a2ndFieldType,
							const TDesC& aText, 
							CContactItem& aContactItem) const;
	IMPORT_C TBool FindTextFieldL(TFieldType aFieldType, 
							const TDesC& aText, 
							CContactItem& aContactItem) const;
	IMPORT_C TBool FindTextFieldL(TFieldType aFieldType, TFieldType a2ndFieldType,
							TFieldType a3rdFieldType, TFieldType a4thFieldType, 
							const TDesC& aText, CContactItem& aContactItem) const;
							

private:	
	TInt FindField			(TFieldType aFieldType, CContactItem& aContactItem) const;
	
	CContactItemField& FindFieldL(TFieldType aFieldType, CContactItem& aContactItem) const;
	CContactItemField& FindFieldL(const CContentType& aTypesToMatch, CContactItem& aContactItem) const;
	
	void InsertTextFieldL	(TFieldType aID, const TDesC& aStringOfStuff, CContactItem& aContactItem);
	
private:
	inline CCntItemModifier  (){};
	
};


class TContact
	{
private:
	class TProp
		{
	public:
		TProp(TUid aUid1, TUid aUid2, CContactItem* aCnt) :  
		  iUid1(aUid1), iUid2(aUid2), iCnt(aCnt)
			{};
		void operator= (const TDesC& aStr);
		operator const TPtrC();

	private:
		CContactItemField& Find();

		TUid iUid1;
		TUid iUid2;
		CContactItem* iCnt;
		};

	class TWorkHome
		{
	public:
		TWorkHome(TUid aUid, CContactItem* aCnt)
		  : Work(aUid, KUidContactFieldVCardMapWORK, aCnt),
			Home(aUid, KUidContactFieldVCardMapHOME, aCnt)
			{};

		TProp Work;
		TProp Home;
		};


public:
	TContact(CContactItem* aCnt);


	//Entries 

	//TProp Title;
	TProp FirstName;
	//TProp MiddleName;
	//TProp LastName;
	//TProp SuffixName;


	TWorkHome Email;
	TWorkHome Address;

private:
	CContactItem* iCnt;
	};



class TCnt
	{
private:
	class TGetSet
		{
		friend class TCnt;
	public:
		TGetSet(CContactItem*& aCnt) : iCnt(aCnt), iIndex(NULL) {};

		IMPORT_C operator const TPtrC();
		IMPORT_C CContactItemField& Field();


		IMPORT_C void operator= (const TDesC& aStr);
		IMPORT_C TBool operator==(const TDesC& aStr);
		IMPORT_C TGetSet& operator & (TUid aUid);

		//Array functionality
		IMPORT_C void operator+= (const TDesC& aStr);
		IMPORT_C TGetSet& operator[](TInt aIndex);
		IMPORT_C TInt Count();

	private:			
		CContactItemField* Find();
	private:
		CContentType* iContType;
		CContactItem*& iCnt;
		TInt iIndex;
		};

public: 

	TCnt(CContactItem*& aContact)
		:iTaker(aContact) {};

	IMPORT_C TGetSet& operator[](TUid aUid);

private:
	TGetSet iTaker;
	};



inline CCntItemBuilder::CCntItemBuilder  (const CContactTemplate& aTemplate):iTemplate(aTemplate) {};



#define KWork	KUidContactFieldVCardMapWORK
#define KHome	KUidContactFieldVCardMapHOME
#define KVoice  KUidContactFieldVCardMapVOICE
#define KCell	KUidContactFieldVCardMapCELL
#define KPager	KUidContactFieldVCardMapPAGER
#define KWv		KUidContactFieldVCardMapWV

#define KPoc	KUidContactFieldVCardMapPOC
#define KSwis	KUidContactFieldVCardMapSWIS
#define KVoip	KUidContactFieldVCardMapVOIP

#endif
