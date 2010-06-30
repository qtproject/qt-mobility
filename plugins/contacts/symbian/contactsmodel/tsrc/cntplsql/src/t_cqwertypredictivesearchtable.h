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
* Description: Unit test class for QWERTY tables
*
*/

#ifndef __UT_CQWERTYPREDICTIVESEARCHTABLE_H__
#define __UT_CQWERTYPREDICTIVESEARCHTABLE_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>
#include <sqldb.h>
#include "cntdef.h" // TContactItemId
#include <QVector>

//  FORWARD DECLARATIONS
class CQwertyPredictiveSearchTable;
class CContactItem;

//  CONSTANTS

//  CLASS DEFINITION
/**
 * Tester class for CQwertyPredictiveSearchTable.
 */
NONSHARABLE_CLASS( UT_CQwertyPredictiveSearchTable ): public CEUnitTestSuiteClass
    {
    public: // Constructors and destructors

        /**
         * Two phase construction
         */
        static UT_CQwertyPredictiveSearchTable* NewL();
        static UT_CQwertyPredictiveSearchTable* NewLC();
        
        /**
         * Destructor
         */
        ~UT_CQwertyPredictiveSearchTable();

    private: // Constructors

        UT_CQwertyPredictiveSearchTable();
        void ConstructL();
        
    private: // Test case setup and teardown

        void SetupL();
        void Teardown();

    private: // Test functions

        void UT_CreateInDbLL();
		void UT_UpdateLL();
		void UT_DeleteLL();
        
    private: // New functions
        QVector<TInt> InitTableVector() const;
        
        void AddContactL(TContactItemId aContactId,
                        const TDesC& aFirstName = KNullDesC,
                        const TDesC& aLastName = KNullDesC,
                        const TDesC& aMail = KNullDesC,
                        const TDesC& aMail2 = KNullDesC,
                        const TDesC& aMail3 = KNullDesC);

		CContactItem* FillContactItemLC(TContactItemId aContactId,
                                      const TDesC& aFirstName = KNullDesC,
                                      const TDesC& aLastName = KNullDesC,
                                      const TDesC& aMail = KNullDesC,
                                      const TDesC& aMail2 = KNullDesC,
                                      const TDesC& aMail3 = KNullDesC) const;

        void CheckItemCountL(QVector<TInt> aCountInTables);

    private:    // Data

		CQwertyPredictiveSearchTable* iTable;
        
        RSqlDatabase iDB;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CQWERTYPREDICTIVESEARCHTABLE_H__

// End of file
