/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalComponent
 @released
*/

#ifndef __HTMLTAGS_H__
#define __HTMLTAGS_H__

// Html Table Tag
_LIT8(KTable, "<Table border=1>");
// Html Table End Tag
_LIT8(KTableEnd, "</Table><p>\r\n");
// Html Table Row Tag
_LIT8(KRow, "<TR>");
// Html Table Row Tag with Gray Background 
_LIT8(KHdRow, "<TR bgcolor='#CCCCCC'>");
// Html Table End Row Tag
_LIT8(KRowEnd, "</TR>\r\n");
// Html Table Cell Tag
_LIT8(KCell, "<TD>");
// Html Table Cell End Tag
_LIT8(KCellEnd, "</TD>");
// Html Bold Text Tag
_LIT8(KBold, "<B>");
// Html End Bold Text Tag
_LIT8(KBoldEnd, "</B>");

// Output Formatting Constants
// Format for String Output    
_LIT8(KRealString, "%F");
// Format for Date Output
_LIT(KFormatDate, "%D%M%Y%/0%1%/1%2%/2%3%/3(%H:%T:%S)");
// Format for Decimal Output
_LIT8(KInt32String, "%d");
// Format for Unsigned 32 Decimal Output
_LIT8(KUInt32String, "%u");
// Format for Unsigned 64 Decimal Output
_LIT8(KUInt64String, "[%u,%u]");
// Format Descriptor for Long Text 8 Output
_LIT8(KLongText8, "Long Text 8");
// Format Descriptor for Long Text 16 Output
_LIT8(KLongText16, "Long Text 16");
// Format Descriptor for DB NULL Output
_LIT8(KNullCol, "null");
// Format Descriptor for output of Data of an Unknown Type 
_LIT8(KUnknownMessage, "unknown");
// Format Descriptor for Long Binary Output
_LIT8(KLongTextBinary, "Long Binary");

_LIT8(KDumpFont,"<font face=\"courier\" size=2>");
_LIT8(KDumpFontEnd,"</font>");
	
#endif	// __HTMLTAGS_H__
