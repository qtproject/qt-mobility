/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 **  
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Solutions Commercial License Agreement provided
 ** with the Software or, alternatively, in accordance with the terms
 ** contained in a written agreement between you and Nokia.
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
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** Please note Third Party Software included with Qt Solutions may impose
 ** additional restrictions and it is the user's responsibility to ensure
 ** that they have met the licensing requirements of the GPL, LGPL, or Qt
 ** Solutions Commercial license and the relevant license of the Third
 ** Party Software they are using.
 **
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include "qlandmarkrequesthandler.h"
#include "qlandmarkmanagerengine_symbian_p.h"

// constant data
const TUint KOneSecond = 1000;

TWaitThrdData::TWaitThrdData()
{
    iThrdId = -1;
}

/***********************************************/

CLandmarkRequestAO::CLandmarkRequestAO(CPosLmOperation* aOperation,
    MLandmarkRequestObserver* aObserver) :
    CActive(EPriorityStandard), iObserver(aObserver), iOperation(aOperation)
{
    CActiveScheduler::Add(this);
}

void CLandmarkRequestAO::RunL()
{
    //    No CPosLmOperation. Cant be handled generically.
    if (iCancelRequest) {
        // Dont make a next call to NextStep
        // All processing is cancelled now. 
        return;
    }
    else if (iOperation) {
        // if operation is not complete or database is locked 
        // then request again to complete it.
        if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked) {
            iOperation->NextStep(iStatus, iProgress);
            SetActive();
        }
        // if request is complete or any error occured 
        // then complete the request with appropriate error code
        else {
            iIsComplete = ETrue;
            // All processing is done now. Notify observer.
            iParent->iErrorId = iStatus.Int();
            iObserver->HandleCompletion(iParent);
            WakeupThreads(iStatus.Int());
        }
    }
    else {
        iObserver->HandleExecution(iParent, iStatus);

        // if operation is not complete or database is locked 
        // then request again to complete it.
        if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked) {
            iStatus = KRequestPending;
            SetActive();
            // Only for the Landmark save and Category save. These dont have async operations.
            // Self completing ative object.
            TRequestStatus* Ptr = &iStatus;
            // TODO: Actual error code needed here??
            User::RequestComplete(Ptr, 0);
        }
        // if request is complete or any error occured 
        // then complete the request with appropriate error code
        else {
            iIsComplete = ETrue;
            iParent->iErrorId = iStatus.Int();
            // All processing is done now. Notify observer.
            iObserver->HandleCompletion(iParent);
            WakeupThreads(iStatus.Int());
        }
    }
}

/*
 * Do a wait for request for the particular request.
 * Loop until the request is complete and then call RunL.
 */
TBool CLandmarkRequestAO::WaitForRequest(TInt aTime, TRequestStatus& aRequest)
{
    while (!iIsComplete) {

        // Wait for the first request.
        if (aTime > 0) {
            User::WaitForRequest(iStatus, aRequest);
            if (aRequest != KRequestPending) {
                // Timer has expired. return false.
                return EFalse;
            }
        }
        else {
            User::WaitForRequest(iStatus);
        }

        // if request is cancel from outside.
        if (iCancelRequest) {
            iParent->iErrorId = KErrCancel;
            // All processing is cancelled now. Notify observer.
            iObserver->HandleCompletion(iParent);
            WakeupThreads(KErrCancel);
            return EFalse;
        }

        if (iOperation) {
            // if operation is not complete or database is locked 
            // then request again to complete it.
            if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked) {
                iOperation->NextStep(iStatus, iProgress);
            }
            // if request is complete or any error occured 
            // then complete the request with appropriate error code
            else {
                iIsComplete = ETrue;
                iParent->iErrorId = iStatus.Int();
                // All processing is done now. Notify observer.
                iObserver->HandleCompletion(iParent);
                WakeupThreads(iStatus.Int());
            }
        }
        // No CPosLmOperation. Cant be handled generically.
        else {
            iObserver->HandleExecution(iParent, iStatus);
            // if operation is not complete or database is locked 
            // then request again to complete it.
            if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked) {
                // Only for the Landmark save and Category save. These dont have async operations.
                // Self completing ative object.
                TRequestStatus* Ptr = &iStatus;
                // TODO: Actual error code needed here??
                User::RequestComplete(Ptr, 0);
            }
            // if request is complete or any error occured 
            // then complete the request with appropriate error code
            else {
                iIsComplete = ETrue;
                iParent->iErrorId = iStatus.Int();
                // All processing is done now. Notify observer.
                iObserver->HandleCompletion(iParent);
                WakeupThreads(iStatus.Int());
            }
        }
    }
    return ETrue;
}

void CLandmarkRequestAO::DoCancel()
{
    if (iOperation) {
        // Only way to cancel the operation is to delete the CPosLmOperation object.
        // Will also take care of cancelling any request which may be pending.
        delete iOperation;
        iOperation = NULL;
    }
    // All processing is cancelled now. Notify observer.
    iParent->iErrorId = KErrCancel;
    iObserver->HandleCompletion(iParent);
    WakeupThreads(KErrCancel);
}

CLandmarkRequestAO* CLandmarkRequestAO::NewL(MLandmarkRequestObserver* aObserver,
    CPosLmOperation* aOperation)
{
    CLandmarkRequestAO* self = new (ELeave) CLandmarkRequestAO(aOperation, aObserver);
    return self;
}

CLandmarkRequestAO::~CLandmarkRequestAO()
{
    Cancel();
    if (iOperation) {
        delete iOperation;
        iOperation = NULL;
    }
}

TBool CLandmarkRequestAO::StartRequest(CPosLandmarkSearch *aLandmarkSearch)
{
    if (aLandmarkSearch)
        iParent->iLandmarkSearch = aLandmarkSearch;

    iCancelRequest = iIsComplete = EFalse;

    // Case where owned CPosLmOperation.
    if (iOperation) {
        iOperation->NextStep(iStatus, iProgress);
        SetActive();
    }
    // Case where manually do operation on single item out of a list.
    else {
        iObserver->HandleExecution(iParent, iStatus);
        iStatus = KRequestPending;
        SetActive();
        // Only for the Landmark save and Category save. 
        // Use a self completing AO
        TRequestStatus* Ptr = &iStatus;
        User::RequestComplete(Ptr, 0);
    }
    return ETrue;
}

TBool CLandmarkRequestAO::CancelRequest()
{
    iCancelRequest = ETrue;
    Cancel();
    return ETrue;
}

TBool CLandmarkRequestAO::WaitForFinished(TInt aTime)
{
    if (iIsComplete || iCancelRequest) {
        return ETrue;
    }

    RTimer timer;
    TRequestStatus timestatus;
    if (aTime > 0) {
        if (timer.CreateLocal() != KErrNone) {
            return EFalse; // ???
        }
        timer.After(timestatus, aTime * KOneSecond);
    }
    TBool IsOperationComplete = EFalse;

    // Check the Thread ID in the list against current.
    TInt currThrdId = RThread().Id();
    if (currThrdId == iParent->iOwnerThrd) {
        IsOperationComplete = WaitForRequest(aTime, timestatus);
        // TODO: Modify this hack if required
        if (IsActive() && IsOperationComplete)
            memset(&iStatus, 0, sizeof(TRequestStatus));

    }
    else {
        qDebug() << "WFF for a non - AO owner Id is" << currThrdId;
        TWaitThrdData Data;
        Data.iThrdId = currThrdId;
        Data.iThrdRequest = KRequestPending;
        iParent->iWaitThrds.AddFirst(Data);
        qDebug() << "Wait for request started";
        if (aTime > 0) {
            User::WaitForRequest(Data.iThrdRequest, timestatus);
            if (timestatus == KRequestPending) {
                IsOperationComplete = ETrue;
            }
        }
        else
            User::WaitForRequest(Data.iThrdRequest);
        qDebug() << "Out of Wait for request in" << currThrdId;
    }
    if (timestatus == KRequestPending) {
        timer.Cancel();
    }
    timer.Close();
    return IsOperationComplete;
}

void CLandmarkRequestAO::WakeupThreads(TInt aCompletion)
{
    qDebug() << "In wakeup threads by " << RThread().Id();
    TWaitThrdData* Ptr = NULL;
    TSglQueIter<TWaitThrdData> Iter(iParent->iWaitThrds);

    iParent->iLock.Wait();
    Iter.SetToFirst();
    while ((Ptr = Iter++) != NULL) {
        RThread waiter;
        if (waiter.Open(Ptr->iThrdId) == KErrNone) {
            TRequestStatus* reqptr = &Ptr->iThrdRequest;
            qDebug() << "Waking up thread" << Ptr->iThrdId;
            waiter.RequestComplete(reqptr, aCompletion);
            waiter.Close();
        }
        iParent->iWaitThrds.Remove(*Ptr);
    }
    iParent->iLock.Signal();
}

/************************************/

CLandmarkRequestData::CLandmarkRequestData(QLandmarkAbstractRequest* aQtReq,
    CLandmarkRequestAO* aAO) :
    iOwnerAO(aAO), iQtRequest(aQtReq), iLandmarkSearch(NULL)
{
    iWaitThrds.SetOffset(_FOFF(TWaitThrdData, iLink));
    iOwnerThrd = RThread().Id();
    iOwnerAO->SetParent(this);
}

CLandmarkRequestData* CLandmarkRequestData::NewL(QLandmarkAbstractRequest* aQtReq,
    CLandmarkRequestAO* aAO)
{
    CLandmarkRequestData* self = new (ELeave) CLandmarkRequestData(aQtReq, aAO);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CLandmarkRequestData::ConstructL()
{
    User::LeaveIfError(iLock.CreateLocal());
}

CLandmarkRequestData::~CLandmarkRequestData()
{
    iLock.Wait();

    TSglQueIter<TWaitThrdData> Iter(iWaitThrds);
    Iter.SetToFirst();
    TWaitThrdData* Ptr;
    while ((Ptr = Iter++) != NULL) {
        iWaitThrds.Remove(*Ptr);
        delete Ptr;
    }

    if (iLandmarkSearch)
        delete iLandmarkSearch;

    if (iOwnerAO)
        delete iOwnerAO;

    iLock.Signal();
    iLock.Close();
}

/**
 * Resetting the data objects.
 */
void CLandmarkRequestData::Reset()
{
    if (iLandmarkSearch) {
        delete iLandmarkSearch;
        iLandmarkSearch = NULL;
    }
    iCategoryIds.clear();
    iLandmarkIds.clear();
    iLandmarks.clear();
    iCategories.clear();
    iErrorMap.clear();
    iErrorId = 0;
    iOpCount = 0;
}

// Asynchronous Framework
LandmarkRequestHandler::LandmarkRequestHandler()
{
    //Handle error here
    LandmarkRequestHandler::iRequestListLock.CreateLocal();
}

LandmarkRequestHandler::~LandmarkRequestHandler()
{
    CLandmarkRequestData *current = NULL;
    iRequestListLock.Wait();
    for (TInt idx = 0; idx < iRequestList.Count(); idx++) {
        current = const_cast<CLandmarkRequestData*> (&iRequestList[idx]);
        if (current)
            delete current;
    }
    iRequestList.Close();
    iRequestListLock.Signal();
}

TInt LandmarkRequestHandler::AddAsyncRequest(QLandmarkAbstractRequest* aQtReq,
    CLandmarkRequestAO* aAO)
{
    CLandmarkRequestData *currentRequestData = NULL;
    iRequestListLock.Wait();
    for (TInt idx = 0; idx < iRequestList.Count(); idx++) {
        if (iRequestList[idx].iQtRequest == aQtReq) {
            currentRequestData = const_cast<CLandmarkRequestData*> (&iRequestList[idx]);
            delete currentRequestData;
            currentRequestData = NULL;
            iRequestList.Remove(idx);
            break;
        }
    }
    TRAPD(err, currentRequestData = CLandmarkRequestData::NewL(aQtReq, aAO));
    if (err) {
        iRequestListLock.Signal();
        return err;
    }
    TInt ret = iRequestList.Append(*currentRequestData);
    iRequestListLock.Signal();
    return ret;
}

TBool LandmarkRequestHandler::RemoveAsyncRequest(QLandmarkAbstractRequest* aQtReq)
{
    TBool flag = false;
    CLandmarkRequestData* Current = NULL;
    iRequestListLock.Wait();
    for (TInt idx = 0; idx < iRequestList.Count(); idx++) {
        if (iRequestList[idx].iQtRequest == aQtReq) {
            Current = const_cast<CLandmarkRequestData*> (&iRequestList[idx]);
            break;
        }
    }
    // This does not delete the QAR.
    if (Current) {
        delete Current;
        Current = NULL;
        flag = true;
    }
    iRequestListLock.Signal();
    return flag;
}

CLandmarkRequestData* LandmarkRequestHandler::FetchAsyncRequest(QLandmarkAbstractRequest* aQtReq)
{
    CLandmarkRequestData *current = NULL;
    iRequestListLock.Wait();
    for (TInt idx = 0; idx < iRequestList.Count(); idx++) {
        if (iRequestList[idx].iQtRequest == aQtReq) {
            current = const_cast<CLandmarkRequestData*> (&iRequestList[idx]);
            break;
        }
    }
    iRequestListLock.Signal();
    return current;
}

// EOF
