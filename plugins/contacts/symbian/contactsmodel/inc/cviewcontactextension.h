/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CVIEWCONTACTEXTENSION_H__
#define __CVIEWCONTACTEXTENSION_H__

// System includes
#include <e32std.h>
#include <s32mem.h>

/** 
 * BC Extension class for CViewContact.
 */
NONSHARABLE_CLASS(CViewContactExtension) : public CBase
	{
public:
	static CViewContactExtension* NewL(TInt aLength = 0);
	~CViewContactExtension();   
	
public:
	inline void SetLengthZero();
	inline const TDesC& FieldText() const;
	inline void SetFieldText(HBufC* aFieldText);
	inline void CreateEmptyFieldTextL();
	void AppendToFieldTextL(const TDesC& aField);
	
	//
	inline TInt& HintBitField();
	inline TInt HintBitField() const;
	inline TUid ContactTypeUid() const;
	inline void SetContactTypeUid(TUid aUid);
private:
	CViewContactExtension();
	void ConstructL(TInt aLength);		

private:
	HBufC* iFieldTextBuf;
	TInt iHintBitField;
	TUid iContactTypeUid;
	};


// Inlines
inline void CViewContactExtension::SetLengthZero()
	{
	if(iFieldTextBuf)
	    {
    	iFieldTextBuf->Des().SetLength(0);
	    }
	}
inline const TDesC& CViewContactExtension::FieldText() const
	{
	if	(iFieldTextBuf)
		return *iFieldTextBuf;
	return KNullDesC;
	}
inline void CViewContactExtension::SetFieldText(HBufC* aFieldTextBuf)
	{
	delete iFieldTextBuf;
	iFieldTextBuf = aFieldTextBuf;
	}
inline void CViewContactExtension::CreateEmptyFieldTextL()
	{
	HBufC* nullBuf = KNullDesC().AllocL();
	SetFieldText(nullBuf);
	}
inline TInt& CViewContactExtension::HintBitField()
	{
	return iHintBitField;
	}
inline TInt CViewContactExtension::HintBitField() const
	{
	return iHintBitField;
	}

inline TUid CViewContactExtension::ContactTypeUid() const
	{return iContactTypeUid;}
	
inline void CViewContactExtension::SetContactTypeUid(TUid aUid)
	{iContactTypeUid = aUid;}

#endif
