/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Macros for performing logging 
*/

#ifndef __PREDICTIVESEARCHLOG_H__
#define __PREDICTIVESEARCHLOG_H__


// To avoid logging in debug builds, define NO_PRED_SEARCH_LOGS macro in the
// file that includes this header file
#if !defined(NO_PRED_SEARCH_LOGS) && defined(_DEBUG)
#define WRITE_PRED_SEARCH_LOGS
#endif


#if defined(WRITE_PRED_SEARCH_LOGS)

#include <e32debug.h> // RDebug

#define PRINT(x)					RDebug::Print(x)
#define PRINT1(x, y)				RDebug::Print(x, y)
#define PRINT2(x, y, z)				RDebug::Print(x, y, z)
#define PRINT3(w, x, y, z)			RDebug::Print(w, x, y, z)
#define PRINT4(v, w, x, y, z)		RDebug::Print(v, w, x, y, z)
#define PRINT5(u, v, w, x, y, z)	RDebug::Print(u, v, w, x, y, z)

#else // #if defined(WRITE_PRED_SEARCH_LOGS)

#define PRINT(x)
#define PRINT1(x, y)
#define PRINT2(x, y, z)
#define PRINT3(w, x, y, z)
#define PRINT4(v, w, x, y, z)
#define PRINT5(u, v, w, x, y, z)

#endif // #if defined(WRITE_PRED_SEARCH_LOGS)


#endif // __PREDICTIVESEARCHLOG_H__

// End of file
