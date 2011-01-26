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

#ifndef NEARFIELDTAGASYNCREQUEST_H
#define NEARFIELDTAGASYNCREQUEST_H

#include <qnearfieldtarget.h>
#include <e32base.h>
#include "nearfieldtargetoperation_symbian.h"

QTM_USE_NAMESPACE

class QNearFieldTagImplCommon;

class MNearFieldTagAsyncRequest
    {
public:
    enum TRequestType
        {
        ENdefRequest,
        ETagCommandRequest,
        ETagCommandsRequest,
        ENull
        };

public:
    MNearFieldTagAsyncRequest(MNearFieldTargetOperation& aOperator);

    virtual ~MNearFieldTagAsyncRequest();
    virtual void IssueRequest() = 0;
    virtual void ProcessResponse(TInt aError);

    // inline to get fast speed since this function is used internally
    // to convert async ndef request to sync.
    virtual void ProcessTimeout() = 0;

    virtual void ProcessWaitRequestCompleted(TInt aError);

    // emit signal defined in QNearFieldTarget
    virtual void ProcessEmitSignal(TInt aError) = 0;

    // should call iOperator->handleResponse(id, response)
    virtual void HandleResponse(TInt aError) = 0;

    virtual TRequestType Type() = 0;

    virtual bool WaitRequestCompleted(int aMsec);
    virtual int WaitRequestCompletedNoSignal(int aMsec);

    void SetRequestId(QNearFieldTarget::RequestId aId);
    QNearFieldTarget::RequestId RequestID();
    static TInt TimeoutCallback(TAny * aObj);
protected:
    // Current async request ID.
    QNearFieldTarget::RequestId iId;
    // Not own.
    QNearFieldTagImplCommon& iOperator;

    // Own.
    CActiveSchedulerWait * iWait;
    // Own.
    CPeriodic * iTimer;
    TBool iRequestIssued;

    int iMsecs;
    bool iSendSignal;
    volatile int * iRequestResult;

    volatile bool * iCurrentRequestResult;
    };

#endif
