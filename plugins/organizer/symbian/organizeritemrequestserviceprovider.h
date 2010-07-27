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

//forward declarations
class CCalEntryView; // calentryview to store/update the entries in the data base
class OrganizerItemTransform; // organizeritemtransform to transform entries/items to and from

QTM_USE_NAMESPACE 

class COrganizerItemRequestsServiceProvider : public CActive
    {
public: 
    // Static two phase constructor
    static COrganizerItemRequestsServiceProvider* NewL(QOrganizerItemManagerEngine& aEntryView);
    // Destruction, cleanup
    ~COrganizerItemRequestsServiceProvider();
    // To start an asynch request aReq
    void StartRequest(QOrganizerItemAbstractRequest* aReq);
    // To stop asynch request iReq
    void StopRequest();
    // To cancel ongoing iReq request
    void CancelRequest();
    
private:
    // Private first phase basic constructor
    COrganizerItemRequestsServiceProvider(QOrganizerItemManagerEngine& aEntryView);
    // Second phase constructor
    void ConstructL();
    // From CActive
    void RunL();
    // From CActive
    void DoCancel();
    // From CActive
    TInt RunError(TInt aError);
    
private: // worker functions
    // Save Items/Entries in agenda server
    void SaveItemsL();

private:
    QOrganizerItemManagerEngine&        iOrganizerItemManagerEngine;
    QOrganizerItemAbstractRequest*      iReq; // Request to be processed
    RPointerArray<CCalEntry>            iCalEntryList; // CalEntryView
    OrganizerItemTransform*             iOrganizerItemTransform; // transform utility to transform item/entry to & from
    };

#endif /* CORGANIZERITEMREQUESTSSERVICEPROVIDER_H_ */
