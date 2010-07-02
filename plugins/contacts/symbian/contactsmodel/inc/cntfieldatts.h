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

#ifndef __CNTFIELDATTS_H__
#define __CNTFIELDATTS_H__

class TContactFieldAtts
/**
@internalComponent
@released
*/
	{
public:
	inline TContactFieldAtts() : iStorage(0)
		{};

	void SetAttribs(TUint32 aAttribs);
	void SetExtendedAttribs(TUint32 aExtendedAttribs);
	void SetType(TStorageType aType);
	TUint32 Attribs() const;
	TUint32 ExtendedAttribs() const;
	TStorageType Type() const;
	void InternalizeL(RReadStream& aStream);
	void ExternalizeL(RWriteStream& aStream) const;

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TInt Count() const;
	void SetCount(TInt aCount);
	TInt TemplateFieldId() const;
	void SetTemplateFieldId(TInt aId);
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

		
private:
	TUint32 iStorage;
	
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__	
	TUint32 iExtendedAttribs;
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__
	};
	
#endif //__CNTFIELDATTS_H__
