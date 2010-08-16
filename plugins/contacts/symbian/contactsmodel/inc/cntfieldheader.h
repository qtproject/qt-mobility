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

#ifndef __CNTFIELDHEADER_H__
#define __CNTFIELDHEADER_H__
#include "cntfieldatts.h"

class RReadStream;
class RWriteStream;

class TFieldHeader
/**
@internalComponent
@released
*/
#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__

	{
public:
    TFieldHeader();
	TFieldHeader(TContactFieldAtts aAtts, TUint32 aFieldUid, TStreamId aId);
	
	inline TContactFieldAtts FieldAtts() const;
	inline void SetFieldAtts(TContactFieldAtts aAtts);
	
	inline TInt FieldId() const;
    inline void SetFieldId(TInt aId);
    
    inline TStreamId StreamId() const;
    inline void SetStreamId(TStreamId aId);
    
private:
	TContactFieldAtts iAtts;
    TInt      iFieldUid;
	TStreamId iStreamId;
	};

#else //__SYMBIAN_CNTMODEL_USE_SQLITE__

	{
public:
	inline TFieldHeader(TStreamId aId,TContactFieldAtts aAtts);
	
public:
	TStreamId iStreamId;
	TContactFieldAtts iAtts;
	};

inline TFieldHeader::TFieldHeader(TStreamId aId,TContactFieldAtts aAtts)
	{ iStreamId=aId; iAtts=aAtts; }
#endif //__SYMBIAN_CNTMODEL_USE_SQLITE__

#endif //__CNTFIELDHEADER_H__
