// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTDBCONSTS_INTERNAL_H_
#define CNTDBCONSTS_INTERNAL_H_

#include <e32def.h>

// Maximum size of an image
const TInt KMaxImageSize = 100000; // 100KB

// Internal folder to store contact thumbnail images
#ifdef UNIT_TEST
_LIT(KImagesFolder, "imagerescale_test\\");
#else
_LIT(KImagesFolder, "10003A73\\");
#endif

// S60 specific contact field type containing image call object data
#define KUidContactFieldCodImageValue 0x101F8841
const TUid KUidContactFieldCodImage={KUidContactFieldCodImageValue};

#endif /* CNTDBCONSTS_INTERNAL_H_ */
