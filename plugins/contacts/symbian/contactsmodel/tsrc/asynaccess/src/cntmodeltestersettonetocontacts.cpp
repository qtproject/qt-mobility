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

// INCLUDE FILES
#include "cntmodeltestersettonetocontacts.h"
#include <cntdef.h>
#include <cntitem.h>
#include <cntdb.h>
#include <cntview.h>
#include <cntfldst.h>


// LOCAL CONSTANTS AND MACROS




// ================= MEMBER FUNCTIONS =======================

inline TBool CCntModelTesterSetToneToContacts ::MoreContactsToSet() const
    {
    return (iContacts && iContacts->Count() > 0);
    }

inline void CCntModelTesterSetToneToContacts ::Cancel()
    {
    delete iContacts;
    iContacts = NULL;
    }

inline CCntModelTesterSetToneToContacts ::CCntModelTesterSetToneToContacts ()    
    {    
    }

inline void CCntModelTesterSetToneToContacts ::ConstructL
        (const CContactIdArray& aContacts,
        const TDesC& aRingingToneName)
    {
    iRingingToneFile = aRingingToneName.AllocL();
    iTotalContactsToSet = aContacts.Count();
    iSetCount=0;
    iContacts = CContactIdArray::NewL(&aContacts);
    iContacts->ReverseOrder();
    iDb= CContactDatabase::OpenL(KDbFileName);    
    }
inline TInt CCntModelTesterSetToneToContacts::GetCount()
	{
	return iSetCount;
	}

EXPORT_C CCntModelTesterSetToneToContacts * CCntModelTesterSetToneToContacts ::NewL
        ( const CContactIdArray& aContacts,
          const TDesC& aRingingToneName)
    {
    CCntModelTesterSetToneToContacts * self = new(ELeave) CCntModelTesterSetToneToContacts ();
    CleanupStack::PushL(self);
    self->ConstructL(aContacts, aRingingToneName);
    CleanupStack::Pop(self);
    return self;
    }

CCntModelTesterSetToneToContacts ::~CCntModelTesterSetToneToContacts ()
    {
    delete iDb;
    delete iContacts;
    delete iRingingToneFile;
    }

void CCntModelTesterSetToneToContacts ::StepL()
    {
    if (MoreContactsToSet())
        {
        const TInt index = iContacts->Count() - 1;
        const TContactItemId id = (*iContacts)[index];

        CContactItem* item = iDb->OpenContactLX(id);
        CleanupStack::PushL(item);
        
        SetRingingToneL(*item);
        
        iDb->CommitContactL(*item);
        CleanupStack::PopAndDestroy(2); // item, lock

        iContacts->Remove(index);
        ++iSetCount;                
        }
    }

TInt CCntModelTesterSetToneToContacts ::TotalNumberOfSteps()
    {
    return iTotalContactsToSet;
    }

TBool CCntModelTesterSetToneToContacts ::IsProcessDone() const
    {
    return !MoreContactsToSet();
    }

void CCntModelTesterSetToneToContacts ::ProcessFinished()
    {
    Cancel();
    }

TInt CCntModelTesterSetToneToContacts ::HandleStepError(TInt aError)
    {
    if (aError != KErrNone)
        {
        Cancel();
        }

    return aError;
    }

void CCntModelTesterSetToneToContacts ::ProcessCanceled()
    {
    Cancel();
    }

void CCntModelTesterSetToneToContacts ::SetRingingToneL
        (CContactItem &aContact)
    {    
    TInt index = aContact.CardFields().Find(KUidContactFieldRingTone);
    
    CContactItemField* field = NULL;
    
    if (index != KErrNotFound)
        {
        field = &aContact.CardFields()[index];
        }

    if (iRingingToneFile->Compare(KNullDesC) == 0)
        {
        // remove ringing tone field
        if (field)
            {
            aContact.RemoveField(index);
            }
        }
    else
        {
        // ringing tone selected
        if (field)
            {
            field->TextStorage()->SetTextL(*iRingingToneFile);
            }
        else
            {
            field = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldRingTone);    
            field->SetMapping(KUidContactFieldVCardMapUnknown);
            field->TextStorage()->SetTextL(*iRingingToneFile);
            aContact.AddFieldL(*field);
            CleanupStack::Pop();
            }
        }
    }

//  End of File  
