/*
* Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
* Contact: http://www.qt-project.org/legal
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


#include <e32std.h>
#include "cntstd.h"

GLDEF_C void Panic(TCntModelPanic aPanic)
//
// Panic the thread with CNTMODEL as the category
//
	{
	User::Panic(_L("CNTMODEL"),aPanic);
	}


