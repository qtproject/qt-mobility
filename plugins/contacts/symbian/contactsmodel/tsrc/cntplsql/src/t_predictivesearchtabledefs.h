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
* Description: Common definitions for test code for predictive search tables
*
*/

#ifndef __UT_PREDICTIVESEARCHTABLEDEFS_H__
#define __UT_PREDICTIVESEARCHTABLEDEFS_H__

//  EXTERNAL INCLUDES
#include "cntdef.h" // TContactItemId


// Database file
_LIT(KDBFile, "c:\\unittest.db");

_LIT(KDBFileWithoutPredSearch, "c:\\contacts_without_pred_search_tables.db");
_LIT(KDBFile12keyButNoQwerty, "c:\\contacts_just_12key_tables.db");
_LIT(KDBFileOtherLanguage, "c:\\contacts_language_45.db");


const TContactItemId KTestContactId = 20;
const TContactItemId KTestContactId2 = 85;
const TContactItemId KTestContactId3 = 7001;
const TContactItemId KTestContactId4 = 56030;

_LIT(KTestFirstName, "123");
_LIT(KTestLastName, "45678");

#endif      //  __UT_PREDICTIVESEARCHTABLEDEFS_H__

// End of file
