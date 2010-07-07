// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CNTFIELD_INTERNAL_H__
#define __CNTFIELD_INTERNAL_H__

/** @internalComponent */
const TInt KIntContactHintIsPhone=0x02;
/** @internalComponent */
const TInt KIntContactHintIsMsg=0x04;
/** @internalComponent */
const TInt KIntContactHintIsCompanyName=0x08;
/** @internalComponent */
const TInt KIntContactHintIsFamilyName=0x10;
/** @internalComponent */
const TInt KIntContactHintIsGivenName=0x20;
/** @internalComponent */
const TInt KIntContactHintIsAddress=0x40;
/** @internalComponent */
const TInt KIntContactHintIsAdditionalName=0x80;
/** @internalComponent */
const TInt KIntContactHintIsSuffixName=0x100;
/** @internalComponent */
const TInt KIntContactHintIsPrefixName=0x200;
/** @internalComponent */
const TInt KIntContactHintStorageInline=0x400;
/** @internalComponent */
const TInt KIntContactHintIsEmail=0x4000;
/** @internalComponent */
const TInt KIntContactHintIsPronunciation=0x800;
/** @internalComponent */
const TInt KIntContactHintIsCompanyNamePronunciation=KIntContactHintIsPronunciation|KIntContactHintIsCompanyName;
/** @internalComponent */
const TInt KIntContactHintIsGivenNamePronunciation=KIntContactHintIsPronunciation|KIntContactHintIsGivenName;
/** @internalComponent */
const TInt KIntContactHintIsFamilyNamePronunciation=KIntContactHintIsPronunciation|KIntContactHintIsFamilyName;

/** @internalComponent */
const TInt KHintTypeMask = 0x1FFFFF;

#ifdef __SYMBIAN_CNTMODEL_USE_SQLITE__

/** Number of additional type ids.
@internalComponent 
*/
const TInt KHintAdditionalMask    = 0x7F000000;
/** Bit flag to show if there is a vCard mapping uid.
@internalComponent 
*/
const TInt KHintVCardMappingMask  = 0x80000000;
/** Template field mask in low hint value.
@internalComponent 
*/
const TInt KHintTemplateFieldMask = 0x7FFFFFFF;

/** @internalComponent */
const TInt KHintAdditionalMaskShift = 24;

#else //_SYMBIAN_USE_SQLITE__

/** @internalComponent */
const TInt KHintAdditionalMask = 0x200000;
/** @internalComponent */
const TInt KHintIdMask = 0xFFC00000;

#endif //_SYMBIAN_USE_SQLITE__


#endif //__CNTFIELD_INTERNAL_H__