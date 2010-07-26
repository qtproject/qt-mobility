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



#include "cfindtext.h"


CFindText* CFindText::NewLC
        (const TDesC& aText)
    {
    CFindText* self = new(ELeave) CFindText;
    CleanupStack::PushL(self);
    self->ConstructL(aText);
    return self;
    }

CFindText::~CFindText()
    {
    iWords.Close();
    delete iText;
    }

TInt CFindText::MdcaCount() const
    {
    return iWords.Count();
    }

TPtrC16 CFindText::MdcaPoint(TInt aIndex) const
    {
    return iWords[aIndex];
    }

void CFindText::ConstructL(const TDesC& aText)
    {
    for (TInt beg=0; beg < aText.Length(); ++beg)
        {
        // Skip space before next word
        if (!TChar(aText[beg]).IsSpace())
            {
            // Scan the end of the word
            TInt end = beg;
            for (; end < aText.Length() && !TChar(aText[end]).IsSpace(); ++end)
                {
                }
            const TInt len = end-beg;
            // Append found word to the array
            User::LeaveIfError(iWords.Append(aText.Mid(beg,len)));
            // Scan for next word
            beg = end;
            }
        }

    iText = aText.AllocL();
    }

