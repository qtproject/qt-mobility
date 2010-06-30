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




#ifndef __ADD_MODIFY_DELETE_CONTACTS__
#define __ADD_MODIFY_DELETE_CONTACTS__

// System Includes
#include <testexecutestepbase.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntviewbase.h>
#include <cntfldst.h>
#include <cntfilt.h>
#include <e32math.h>

// User Includes
#include "clientserver.h"

/**
 * This class supports
 * 1) Determination of desired contact field Uids
 * 2) Adding contact item fields with random alpha, numeric or alpha numeric content
 * 3) Addition of contact item fields which meet the desired contact view filter criteria
 */ 
class TAddModifyDeleteContactFields
	{
	public:
	  TStorageType 	GetStorageType(const TFieldType& aFieldType);
	  void 			SetDataL(const TFieldType& aFieldType, CContactItemField& aField);
	  void 			UpdateFieldsL(CContactItem& aContact);	  
	  TUid 			GetContactFieldType(const TPtrC& aContactFieldString);
	  void 			AddFieldsSpecificToFilterL(CContactItem& aContact);
	  void 			AddFilterBasedFieldsL(CContactItem& aContact, TInt aBitWiseFilter);
	  void 			SetWorkPhoneL(CContactItem& aContact, const TDesC& aWorkPhone);
	  void 			SetWorkMobileL(CContactItem& aContact, const TDesC& aWorkMobile);
	  void 			SetHomePhoneL(CContactItem& aContact, const TDesC& aHomePhone);
	  void 			SetHomeMobileL(CContactItem& aContact, const TDesC& aHomeMobile);
	  void 			SetHomeEmailAddressL(CContactItem& aContact, const TDesC& aEmailAddress);
	  void 			SetWorkEmailAddressL(CContactItem& aContact, const TDesC& aEmailAddress);
	  void 			SetFaxL(CContactItem& aContact, const TDesC& aFax);
	  void 			SetIMAddressL(CContactItem& aContact, const TDesC& aIMAddress);
	  void 			SetVoiceDialL(CContactItem& aContact, const TDesC& aVoiceDial);
	  void 			SetRingToneL(CContactItem& aContact, const TDesC& aRingTone);
	  void 			SetTextFieldL(CContactItem& aContact, const CContentType& aTypesToMatch, const TDesC& aText);
	  void 			SetRandomAlphaString(TDes& aBuf,TInt aLength);
	  void 			SetRandomNumericString(TDes& aBuf,TInt aLength);
	  void 			SetRandomAlphaNumericString(TDes& aBuf,TInt aLength);
	  TText			RandomAlphaCharCode();
	  TText 		RandomNumericCharCode();
	  TText 		RandomAlphaNumbericCharCode();
	  TText 		RandomCharCode(TText aLowerBound,TText aUpperBound,TText aException);
	  TText 		RandomCharCode(TText aLowerBound,TText aUpperBound);

	private:
		TInt64 		iRandSeed;
	};
	
#endif	



