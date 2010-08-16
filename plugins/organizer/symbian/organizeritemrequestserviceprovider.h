/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CORGANIZERITEMREQUESTSSERVICEPROVIDER_H_
#define CORGANIZERITEMREQUESTSSERVICEPROVIDER_H_

#include <e32base.h>
#include <qtorganizer.h>

// Forward declarations
class QOrganizerItemSymbianEngine; // Symbian Plugin

QTM_USE_NAMESPACE 

class COrganizerItemRequestsServiceProvider : public CActive
    {
public: 
    // Static two phase constructor
    static COrganizerItemRequestsServiceProvider* NewL(
            QOrganizerItemSymbianEngine& aEntryView);
    // Destruction, cleanup
    ~COrganizerItemRequestsServiceProvider();
    // To start an asynchronous request aReq
    TBool StartRequest(QOrganizerItemAbstractRequest* aReq);
    // To cancel ongoing iReq request
    TBool CancelRequest();
    // Wait for request to complete 
    TBool waitForRequestFinished(TTimeIntervalMicroSeconds32 aInterval);
    
private:
    // Private first phase basic constructor
    COrganizerItemRequestsServiceProvider(
            QOrganizerItemSymbianEngine& aEntryView);
    // Second phase constructor
    void ConstructL();
    // From CActive
    void RunL();
    // From CActive
    void DoCancel();
    // From CActive
    TInt RunError(TInt aError);
    // Complete asynchronous request on self
    void SelfComplete();
    // Initialize/reset member variables so that these 
	// can be used by the next request
    void Cleanup();
    
private: // Worker functions
    // Save item/entry in agenda server
    void SaveItemL();
    // Fetch items/entries from agenda server
    void FetchItemsL();
    // Fetch items/entries by local Ids
    void FetchItemsByLocalIdsL();
    // Fetch items/entries by details
    void FetchItemsandFilterL(QOrganizerItemFilter& filter, 
            QList<QOrganizerItemSortOrder>& sortOrder, 
            QOrganizerItemFetchHint& fetchHint);
    // Fetch items ids
    void FetchItemIdsL();
    // Remove items/entries
    void RemoveItemL();
    // Fetch detail definition
    void FetchDetailDefinitionL();
    // Remove detail definition
    void RemoveDetailDefinitionL();
    // Save detail definition
    void SaveDetailDefinitionL();
    
private:
    QOrganizerItemSymbianEngine&            iOrganizerItemManagerEngine;
    QOrganizerItemAbstractRequest*          iReq; // Request to be processed
    QList<QOrganizerItem>                   iItemList;
    QList<QOrganizerItemLocalId>            iItemIds;
    QMap<int, QOrganizerItemManager::Error> iErrorMap; // Error map
    QOrganizerItemManager::Error            iError; // Error
    TInt                                    iNoOfItems;
    TInt                                    iIndex;
    };

#endif /* CORGANIZERITEMREQUESTSSERVICEPROVIDER_H_ */
