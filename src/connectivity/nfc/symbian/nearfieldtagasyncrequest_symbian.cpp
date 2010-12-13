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
#include "nearfieldtagasyncrequest_symbian.h"
#include <e32std.h>

TInt MNearFieldTagAsyncRequest::TimeoutCallback(TAny * aObj)
{
    MNearFieldTagAsyncRequest * obj = static_cast<MNearFieldTagAsyncRequest *>(aObj);
    obj->ProcessTimeout();
    return KErrNone;
}

TInt MNearFieldTagAsyncRequest::DeleteLater(TAny * aObj)
{
    MNearFieldTagAsyncRequest * obj = static_cast<MNearFieldTagAsyncRequest *>(aObj);
    delete obj;
}

MNearFieldTagAsyncRequest::MNearFieldTagAsyncRequest()
{
    iOperator = 0;
    iWait = 0;
    iTimer = 0;
}

MNearFieldTagAsyncRequest::~MNearFieldTagAsyncRequest()
{
    if (iTimer)
    {
        delete iTimer;
        iTimer = 0;
    }

    if (iWait)
    {
        if (iWait->IsStarted())
        {
            iWait->AsyncStop();
        }
        delete iWait;
    }
}
 
void MNearFieldTagAsyncRequest::SetOperator(MNearFieldTargetOperation * aOperator)
{
    iOperator = aOperator;
}

void MNearFieldTagAsyncRequest::SetRequestId(QNearFieldTarget::RequestId aId)
{
    iId = aId;
}
    
QNearFieldTarget::RequestId MNearFieldTagAsyncRequest::GetRequestId()
{
    return iId;
}

bool MNearFieldTagAsyncRequest::WaitRequestCompleted(int aMsecs)
{
    if (iWait)
    {
        if (iWait->IsStarted())
        {
            // the request is already waited, return false.
            return false;
        }
    }
    else
    {
        iWait = new(ELeave) CActiveSchedulerWait();
    }

    if (iTimer)
    {
        iTimer->Cancel();
    } 
    else
    {
        iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    }

    TCallBack callback(MNearFieldTagAsyncRequest::TimeoutCallback, this);
    iTimer->Start(0, aMsecs, callback);
    iWait->Start();
    return true;
}
        
void MNearFieldTagAsyncRequest::ProcessResponse(TInt aError)
{
    iOperator->IssueNextRequest();
    this->HandleResponse(aError);

    if (iWait) 
    {
        ProcessWaitRequestCompleted(aError);
    }
    else
    {
        ProcessEmitSignal(aError);
    }
    
    iOperator->RemoveRequestFromQueue(iId);
    delete this;
} 
            
void MNearFieldTagAsyncRequest::ProcessTimeout()
{
    if (iWait)
    {
        if (iWait->IsStarted())
        {
            ProcessResponse(KErrTimedOut);
        }
    }
}
void MNearFieldTagAsyncRequest::ProcessWaitRequestCompleted(TInt aError)
{
    if (iTimer)
    {
        delete iTimer;
        iTimer = 0;
    }
    if (iWait)
    {
        if (iWait->IsStarted())
        {
            iWait->AsyncStop();
        }
    }
}
