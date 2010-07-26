/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/* If __VERBOSE_DEBUG__ is set the following DEBUG_PRINT macros' will enable debug logs
   else DEBUG_PRINT macros' do nothing! (empty #define)
*/

#ifdef __VERBOSE_DEBUG__
#define DEBUG_PRINT1(define,p1)  if(define) RDebug::Print(p1)
#define DEBUG_PRINT2(define,p1,p2)  RDebug::Print(p1,p2)
#define DEBUG_PRINTDN2(define,p1,p2)  DebugLogNotification(p1,p2)
#define DEBUG_PRINTVN2(define,p1,p2)  DebugLogViewNotification(p1,p2)
#define DEBUG_PRINTF1(define,p1)  RDebug::Printf(p1)
#define DEBUG_PRINTF2(define,p1,p2)  RDebug::Printf(p1,p2)
#define  DEBUG_PRINT4(define,p1,p2,p3,p4)  DebugLogIPC(p1,p2,p3,p4)
#endif

#ifndef __VERBOSE_DEBUG__
#define DEBUG_PRINT1(define,p1) 
#define DEBUG_PRINT2(define,p1,p2) 
#define DEBUG_PRINTDN2(define,p1,p2) 
#define DEBUG_PRINTVN2(define,p1,p2) 
#define DEBUG_PRINTF1(define,p1) 
#define DEBUG_PRINTF2(define,p1,p2) 
#define  DEBUG_PRINT4(define,p1,p2,p3,p4)
#endif
