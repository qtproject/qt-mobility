/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __PACKAGER_TEST_CLIENT_H__
#define __PACKAGER_TEST_CLIENT_H__

#include <e32std.h>
#include "ccntpackager.h"


class RPackagerTestClient : public RSessionBase
	{
public:
	IMPORT_C void ConnectL();
	IMPORT_C void Close();
	
	IMPORT_C void PackAndSendL(const CContactItem& anItem, TRequestStatus& aStatus);
	IMPORT_C void PackAndSendL(const CContentType& anItem, TRequestStatus& aStatus);
	IMPORT_C void PackAndSendL(const CContactTextDef& anItem, TRequestStatus& aStatus);	
	IMPORT_C void PackAndSendL(const CContactItemViewDef& anItem, TRequestStatus& aStatus);
	IMPORT_C void PackAndSendL(const CContactViewDef& anItem, TRequestStatus& aStatus);
	IMPORT_C void PackAndSendL(const CContactIdArray& anItem, TRequestStatus& aStatus);
	IMPORT_C void PackAndSendL(const CCntFilter& anItem, TRequestStatus& aStatus);	
	IMPORT_C void PackAndSendL(const CViewContact& anItem, TRequestStatus& aStatus);	
	IMPORT_C void PackAndSendL(const CDesCArray& anItem, TRequestStatus& aStatus);

	IMPORT_C CContactItem* 			GetAndUnpackCntItemLC() const;
	IMPORT_C CContentType* 			GetAndUnpackCntContentTypeLC() const;
	IMPORT_C CContactTextDef* 		GetAndUnpackCntTextDefLC() const;
	IMPORT_C CContactItemViewDef* 	GetAndUnpackCntItemViewDefLC() const;
	IMPORT_C CContactViewDef* 		GetAndUnpackCntViewDefLC() const;
	IMPORT_C CContactIdArray* 		GetAndUnpackCntIdArrayLC() const;
	IMPORT_C CCntFilter* 			GetAndUnpackCntFilterLC() const;
	IMPORT_C CViewContact* 			GetAndUnpackViewCntLC() const;
	IMPORT_C CDesCArray* 			GetAndUnpackDesCArrayLC() const;
	
private:
	void PrepareArgumentsL(TIpcArgs& anArgs) const;
	IMPORT_C CContactItem* DoUnpackCntItemLC			(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CContentType* DoUnpackCntContentTypeLC	(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CContactTextDef* DoUnpackCntTextDefLC	(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CContactItemViewDef* DoUnpackCntItemViewDefLC(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CContactViewDef* DoUnpackCntViewDefLC	(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CContactIdArray* DoUnpackCntIdArrayLC	(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CCntFilter* DoUnpackCntFilterLC			(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CViewContact* DoUnpackViewCntLC			(TInt aBufferSize, TIpcArgs& anArgs) const;
	IMPORT_C CDesCArray* DoUnpackDesCArrayLC			(TInt aBufferSize, TIpcArgs& anArgs) const;	
	
private:
	CCntPackager* iPackager;	
	
	};


#endif //__PACKAGER_TEST_CLIENT_H__
