/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Debug Macros used by AdvancedAudioController
*
*/

#include <QtCore/qdebug.h>

#ifndef __DEBUGMACROS_H__
#define __DEBUGMACROS_H__

// MACROS
#ifdef _DEBUG
#define DP0(string)                                 qDebug()<<string
#define DP1(string,arg1)                            qDebug()<<string<<arg1
#define DP2(string,arg1,arg2)                       qDebug()<<string<<arg1<<arg2
#define DP3(string,arg1,arg2,arg3)                  qDebug()<<string<<arg1<<arg2<<arg3
#define DP4(string,arg1,arg2,arg3,arg4)             qDebug()<<string<<arg1<<arg2<<arg3<<arg4
#define DP5(string,arg1,arg2,arg3,arg4,arg5)        qDebug()<<string<<arg1<<arg2<<arg3<<arg4<<arg5
#define DP6(string,arg1,arg2,arg3,arg4,arg5,arg6)   qDebug()<<string<<arg1<<arg2<<arg3<<arg4<<arg5<<arg6
#else
#define DP0(string)
#define DP1(string,arg1)
#define DP2(string,arg1,arg2)
#define DP3(string,arg1,arg2,arg3)
#define DP4(string,arg1,arg2,arg3,arg4)
#define DP5(string,arg1,arg2,arg3,arg4,arg5)
#define DP6(string,arg1,arg2,arg3,arg4,arg5,arg6)
#endif

#endif //__DEBUGMACROS_H__
