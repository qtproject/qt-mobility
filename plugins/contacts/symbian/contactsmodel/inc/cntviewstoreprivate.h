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
* Functions/APIs used only for testing purposes - these are not exported to 
* /epoc32/include
*
*/


#ifndef CNTVIEWSTOREPRIVATE_H_
#define CNTVIEWSTOREPRIVATE_H_

class CContactDefaultViewDefinition;

class TContactViewStorePrivate
	{
public:
	IMPORT_C static void GetDefinitionsOfExistingViewsL(const TDesC& aDbName, RPointerArray<CContactDefaultViewDefinition>& aViewDefs);
	};

#endif // CNTVIEWSTOREPRIVATE_H_
