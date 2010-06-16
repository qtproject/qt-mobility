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
* Description: Retrieves the character map for each of the numeric keys.
*/

// INCLUDE FILES
#include "cntpredictivesearch.h"
#include "cntsqlsearch.h"

#include <QtGlobal>
#include <QString>
#include <QChar>

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CntPredictiveSearch::NewL
// ----------------------------------------------------------------------------
EXPORT_C CntPredictiveSearch* CntPredictiveSearch::NewL()
    {
    CntPredictiveSearch* self = new (ELeave) CntPredictiveSearch();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
// ----------------------------------------------------------------------------
// CntPredictiveSearch::NewL
// ----------------------------------------------------------------------------
EXPORT_C CntPredictiveSearch::~CntPredictiveSearch()
{

}   
 
// ----------------------------------------------------------------------------
// CntPredictiveSearch::CreateSQLQuery
// Construct SQL query.
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CntPredictiveSearch::CreateSQLQueryL(HBufC& searchQuery, int aKeypad)
{   
    QString qString;
    QString queryString;
    QT_TRYCATCH_LEAVING({
        qString = QString((QChar*)searchQuery.Des().Ptr(),searchQuery.Length());
        CntSqlSearch SqlSearch;
        queryString = SqlSearch.CreatePredictiveSearch(qString);
        });
    TPtrC myDescriptor (reinterpret_cast<const TText*>(queryString.constData()),queryString.length());   
    return myDescriptor.AllocL();
}
    
// ----------------------------------------------------------------------------
// CntPredictiveSearch::CntPredictiveSearch
// ----------------------------------------------------------------------------
CntPredictiveSearch::CntPredictiveSearch()
{
}
      
// ----------------------------------------------------------------------------
// CntPredictiveSearch::ConstructL
// ----------------------------------------------------------------------------
void CntPredictiveSearch::ConstructL()
{
}
// End of file
