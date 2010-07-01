// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CNTHINT_H__
#define __CNTHINT_H__

class CContactItemField::THint
	/**
	@internalComponent
	@released
	*/
	{
	public:
		THint();
		THint(TInt aValue);
		TBool operator==(const THint& aHint) const;
		TBool operator!=(const THint& aHint) const;
		TInt  HintType() const;
		inline void SetIsMatchAll();
		inline void SetIsPhone();
		inline void SetIsMsg();
		inline void SetIsCompanyName();
		inline void SetIsFamilyName();
		inline void SetIsGivenName();
		inline void SetIsCompanyNamePronunciation();
		inline void SetIsFamilyNamePronunciation();
		inline void SetIsGivenNamePronunciation();
		inline void SetIsAddress();
		inline void SetIsAdditionalName();
		inline void SetIsSuffixName();
		inline void SetIsPrefixName();
		inline void SetStorageIsInline();
		inline void SetIsEmail();
		inline TBool IsMatchAll() const;
		inline TBool IsDefault() const;
		inline TBool IsPhone() const;
		inline TBool IsMsg() const;
		inline TBool IsCompanyName() const;
		inline TBool IsFamilyName() const;
		inline TBool IsGivenName() const;
		inline TBool IsCompanyNamePronunciation() const;
		inline TBool IsFamilyNamePronunciation() const;
		inline TBool IsGivenNamePronunciation() const;
		inline TBool IsAddress() const;
		inline TBool IsAdditionalName() const;
		inline TBool IsSuffixName() const;
		inline TBool IsPrefixName() const;
		inline TBool IsStorageInline() const;
		inline TBool IsEmail() const;
		inline TBool Contains(const THint& aHint) const;
		
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
		inline void SetHasAdditionalUids();
		TBool HasAdditionalUids() const;
		
		TInt FieldId() const;
		void SetFieldId(TInt aId);
		
#else //__SYMBIAN_CNTMODEL_USE_SQLITE__
        TInt  TemplateFieldId() const;
        void  SetTemplateFieldId(TInt aTemplateFieldId);
        
        inline void SetHasVCardMappingUid();
        inline TBool HasVCardMappingUid() const;
        
        inline TInt  AdditionalUidsNum() const;
        inline void  SetAdditionalUidsNum(TInt aNumber);
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__		
	public:
		TInt iHintValue;

	};
		
#endif