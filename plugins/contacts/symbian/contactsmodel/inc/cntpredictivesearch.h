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
* Description: Class to hold the character to key mappings and provide
*   conversion of strings to their numerical mappings. 
*
*/

#ifndef __CNTPREDICTIVESEARCH_H__
#define __CNTPREDICTIVESEARCH_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION
NONSHARABLE_CLASS (CntPredictiveSearch) : public CBase
{
public:
    
    /**
     * Two phase construction
     */
    IMPORT_C static  CntPredictiveSearch* NewL();
    
    /**
     * Destructor
     */
    IMPORT_C virtual ~CntPredictiveSearch();

public:

    /**
    * Construct SQL query.
    * aKeypad ETrue indicates key pad type.
    *                        
    */
    IMPORT_C  HBufC* CreateSQLQueryL(HBufC& searchQuery, int aKeypad);
    
private:
    /**
    * Constructor
    */
    CntPredictiveSearch();
      
    /**
    * Second phase constructor
    */
    void ConstructL();
    
};
#endif // __CNTPREDICTIVESEARCH_H__

// End of file
