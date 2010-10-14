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
#include <calcommon.h>
#include "qorganizeritemchangeset.h"
#include "qorganizercollectionchangeset.h"

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
    bool StartRequest(QOrganizerAbstractRequest* aReq);
    // To cancel ongoing iReq request
    bool CancelRequest();
    
private:
    // Private first phase basic constructor
    COrganizerItemRequestsServiceProvider(
            QOrganizerItemSymbianEngine& aEntryView);
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
    void Reset();
    
private: // Worker functions
    void FetchItems();
    void SaveItem();
    void FetchItemsForExport();
    void FetchItemsByLocalIds();
    void FetchItemsandFilter();
    void FetchItemIds();
    void RemoveItem();
    void FetchDetailDefinition();
    void FetchCollections();
    void SaveCollections();
    void RemoveCollections();
    
private:
    QOrganizerItemSymbianEngine&            iOrganizerItemManagerEngine;
    QOrganizerAbstractRequest*          iReq; // Request to be processed
    QList<QOrganizerItemLocalId>            iItemIds;
    QMap<int, QOrganizerManager::Error> iErrorMap; // Error map
    TInt                                    iIndex;
};

#endif /* CORGANIZERITEMREQUESTSSERVICEPROVIDER_H_ */
