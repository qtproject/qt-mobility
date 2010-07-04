/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CFindText Test module
*
*/



#ifndef CFindText_H
#define CFindText_H

#include <e32base.h>
#include <bamdesca.h>

/**
 * Small helper class for using CContactFindView. Takes in
 * a string in constructor and breaks the string into words.
 * Implements MDesCArray interface which provides access to the words
 * found in the string.
 */
class CFindText : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor
        static CFindText* NewLC(const TDesC& aText);
        ~CFindText();
        
    public:  // New functions
        const TDesC& Text() const;

    public:  // from MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;

    private:  // Implementation
        CFindText();
        void ConstructL(const TDesC& aText);

    private:
        HBufC* iText;
        RArray<TPtrC> iWords;
    };

inline CFindText::CFindText()
    {
    }

inline const TDesC& CFindText::Text() const
    {
    if (iText)
        return(*iText);
    else
        return KNullDesC;
    }

#endif

