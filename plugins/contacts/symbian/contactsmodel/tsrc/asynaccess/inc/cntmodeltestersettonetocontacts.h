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

#ifndef __CCntModelTesterSetToneToContacts_H__
#define __CCntModelTesterSetToneToContacts_H__

// INCLUDES
#include <e32base.h>
//#include "mpbkbackgroundprocess.h"
#include <cntdef.h>

// FORWARD DECLARATIONS
class CContactIdArray;
class CContactDatabase;
class CContactItem;

// CLASS DECLARATION

/**
 * Assign ringing tone to group members process.
 */
class CCntModelTesterSetToneToContacts  :
        public CBase        
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine   reference to a contact engine
         * @param aContacts contacts to delete
		 * @param aRingingToneName ringing tone file name
         */
        static CCntModelTesterSetToneToContacts* NewL
            ( const CContactIdArray& aContacts,
              const TDesC& aRingingToneName);

        /**
         * Destructor.
         */
        ~CCntModelTesterSetToneToContacts ();

    public: 
        void StepL();
        TInt TotalNumberOfSteps();
        TBool IsProcessDone() const;
        void ProcessFinished();
        TInt HandleStepError(TInt aError);
        void ProcessCanceled();
        TInt GetCount();
		TInt iSetCount;
    private:  // Implementation
        CCntModelTesterSetToneToContacts ();
        void ConstructL(const CContactIdArray& aContacts, const TDesC& aRingingToneName);
        TBool MoreContactsToSet() const;
        void Cancel();
        void SetRingingToneL(CContactItem &aContact);

    private:  // Data
    
        CContactDatabase* iDb;
        /// Own: initial number of contacts to assign a tone
        TInt iTotalContactsToSet;
        /// Own: ids of contacts to set ringing tone to
        CContactIdArray* iContacts;
        /// Own: count of contacts succesfully set

        /// Own: ringing tone file name
        HBufC* iRingingToneFile;
        
    };

_LIT(KDbFileName,"c:t_asynaccess.cdb");
#endif // __CCntModelTesterSetToneToContacts _H__

// End of File
