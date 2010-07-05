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


#include <cntdb.h>

////////////////////////////////
// CSqlSearcher class declaration
////////////////////////////////

class CSqlSearcher : public CBase, public MIdleFindObserver
	{
	public:
		static CSqlSearcher * NewLC();
		void IdleFindCallback();
		void DoSearchL(const TDesC& aString, TUid aFieldId);
	private:
		CSqlSearcher();
		~CSqlSearcher();
		void ConstructL();
	protected:
		CContactDatabase* iDb;
		HBufC* iBuffer;
	};
