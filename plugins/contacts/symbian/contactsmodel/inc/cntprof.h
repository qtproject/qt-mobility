/*
* Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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


#if !defined(__PROFILE_H__)
#define __PROFILE_H__

#include <e32std.h>

#if defined(_DEBUG)			// Alter to this to turn on/off profiling in non-debug/debug builds
#define _PROFILING
#else
//#define _PROFILING
#endif

#if defined(_PROFILING)

#define __START_PROFILE(n) 
#define __END_PROFILE(n)  

#else

#define __START_PROFILE(n)
#define __END_PROFILE(n)

#endif

#endif
