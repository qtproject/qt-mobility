/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     This file contains private declarations for resources of PbkEng.dll.
*     The file can be included in C++ or resource file.
*
*/


#ifndef CNTMODELEXTUIDS_H
#define CNTMODELEXTUIDS_H

#include "cntmodelextuids.hrh"

//  CONSTANTS  

/**
 * Phonebook custom Field value.
 * Locationing privacy field type.
 */
const TUid KPbkUidContactFieldLocationPriv={KPbkUidContactFieldLocationPrivValue};

/**
 * Phonebook custom Field value.
 * Top Contact field type.
 */
const TUid KUidContactFieldTopContact={KUidContactFieldTopContactValue};

/**
 * Phonebook custom Field value.
 * IMPP field type.
 */
const TUid KUidContactFieldIMPP={KUidContactFieldIMPPValue};

/**
 * Phonebook custom Field value.
 * Video ringtone field type.
 */
const TUid KUidContactFieldVideoRingTone={KUidContactFieldVideoRingToneValue};

/**
 * Phonebook custom Field value.
 * Gender field type.
 */
const TUid KUidContactFieldGender={KUidContactFieldGenderValue};

// !! In case this is undefined on earlier platforms
#ifndef KUidContactFieldGEOValue
#define KUidContactFieldGEOValue            0x10274DB2
const TUid KUidContactFieldGEO = {KUidContactFieldGEOValue};
#endif

#endif // CNTMODELEXTUIDS_HRH

// End of File
