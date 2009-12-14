// MPBUTIL.H
//
// Copyright (c) 1997-2000 Symbian Ltd.  All rights reserved.
//

#ifndef __MPBUTIL_H__
#define	__MPBUTIL_H__

#include <e32base.h>
#include "etelmm_etel_test_server.h"		// for RMobilePhoneBookStore

const TInt KFieldEntrySize = 8;
const TInt KErrStartOfNewEntry = -1;
const TInt KMaxUint16Val = 0xFFFF;

class CPhoneBookBuffer : public CBase
	{
	public:
		CPhoneBookBuffer();
		void Set(TDes8* aData);
		TInt AddNewEntryTag();
        TInt AddNewNumberTag();
		TInt PutTagAndValue(TUint8 aTagValue, TUint8 aInteger);
		TInt PutTagAndValue(TUint8 aTagValue, TUint16 aInteger);
		TInt PutTagAndValue(TUint8 aTagValue, TUint32 aInteger);
		TInt PutTagAndValue(TUint8 aTagValue, const TDesC8 &aData);
		TInt PutTagAndValue(TUint8 aTagValue, const TDesC16 &aData);
		TInt RemovePartialEntry();
		void StartRead();

		enum TPhBkTagType
			{
			EPhBkTypeNoData,
			EPhBkTypeInt8,
			EPhBkTypeInt16,
			EPhBkTypeInt32,
			EPhBkTypeDes8,
			EPhBkTypeDes16,
			};

		TInt GetTagAndType(TUint8 &aTagValue, TPhBkTagType &aDataType);
		TInt GetValue(TUint8 &aInteger);
		TInt GetValue(TUint16 &aInteger);
		TInt GetValue(TUint32 &aInteger);
		TInt GetValue(TPtrC8 &aData);
		TInt GetValue(TPtrC16 &aData);
		void SkipValue(TPhBkTagType aDataType);
		TInt BufferLength();
		TInt RemainingReadLength();

	private:
		TInt AppendInt8(TUint8 aInteger);
		TInt AppendInt16(TUint16 aInteger);
		TInt AppendInt32(TUint32 aInteger);

	private:
		TDes8*  iPtr;   // Points to supplied Data buffer
		TPtrC8 iRead;   // Points to populated TLV buffer
		TPtr8 iMonitor; // Used to remove partially populated entries from TLV buffer

		TInt iBytesInBuffer;
	};

#endif
