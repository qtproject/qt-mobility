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

#include "qlandmarkrequesthandler.h"
#include "qlandmarkmanagerengine_symbian_p.h"
#include <qlandmarkidfetchrequest.h>
#include <qlandmarkfetchrequest.h>
#include <qdebug.h>

// constant data
const TUint KOneSecond = 1000;

TWaitThrdData::TWaitThrdData()
{
    iThrdId = -1;
}

/***********************************************/

CLandmarkRequestAO::CLandmarkRequestAO(CPosLmOperation *aOperation,
    MLandmarkRequestObserver *aObserver) :
    CActive(EPriorityStandard), iObserver(aObserver), iOperation(aOperation),
        iIsDestructing(EFalse)
{
    CActiveScheduler::Add(this);
}

void CLandmarkRequestAO::RunL()
{
    // if canceled then return.
    if (iCancelRequest) {
        // Dont make a next call to NextStep
        // All processing is cancelled now. 
        //qDebug() << "RunL : request is canceled";

        return;
    }
    else if (iOperation) {
        // if operation is not complete or database is locked 
        // then request again to complete it.
        if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked || iStatus
            == KErrAccessDenied) {
            iOperation->NextStep(iStatus, iProgress);
            SetActive();
        }
        else if (iStatus == KErrNone && IsMultiOperationRequest()) {
            TRAPD(err, iObserver->HandleExecutionL(iParent, iStatus);)
            if (err == KErrNone) {
                //qDebug() << "iStatus = " << iStatus.Int();
                iStatus = KRequestPending;
                if (iOperation) {
                    iOperation->NextStep(iStatus, iProgress);
                    SetActive();
                }
                else {
                    //qDebug() << "starting multi-operation";
                    SetActive();
                    // Only for the Landmark save and Category save. These dont have async operations.
                    // Self completing ative object.
                    TRequestStatus *Ptr = &iStatus;
                    User::RequestComplete(Ptr, KErrNone);
                }
            }
            else {

                iIsComplete = ETrue;
                // All processing is done now. Notify observer.
                iParent->iErrorId = err;
                iObserver->HandleCompletionL(iParent);
                WakeupThreads(iStatus.Int());
                iIsRequestRunning = EFalse;

            }
        }
        // if request is complete or any error occurred 
        // then complete the request with appropriate error code
        else {
            iIsComplete = ETrue;
            // All processing is done now. Notify observer.
            iParent->iErrorId = iStatus.Int();
            iObserver->HandleCompletionL(iParent);
            WakeupThreads(iStatus.Int());
            iIsRequestRunning = EFalse;
        }
    }
    // if operation object is not defined.
    else {
        TRAPD(err,iObserver->HandleExecutionL(iParent, iStatus);)
        if (err != KErrNone) {
            iStatus = err;
        }

        // prepare next request.
        if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked || iStatus
            == KErrAccessDenied) {

            iStatus = KRequestPending;
            if (iOperation) {
                iOperation->NextStep(iStatus, iProgress);
                SetActive();
            }
            else {
                SetActive();
                // Only for the Landmark save and Category save. These dont have async operations.
                // Self completing ative object.
                TRequestStatus *Ptr = &iStatus;
                User::RequestComplete(Ptr, KErrNone);
            }
        }
        // if request is complete or any error occurred 
        // then complete the request with appropriate error code
        else {
            iIsComplete = ETrue;
            iParent->iErrorId = iStatus.Int();
            // All processing is done now. Notify observer.
            iObserver->HandleCompletionL(iParent);
            WakeupThreads(iStatus.Int());
            iIsRequestRunning = EFalse;
        }
    }
}

/*
 * Do a wait for request for the particular request.
 * Loop until the request is complete and then call RunL.
 */
TBool CLandmarkRequestAO::WaitForRequestL(TInt aTime, TRequestStatus &aRequest)
{
    //qDebug() << "WaitForRequestL start";
    
    while (!iIsComplete) {

        // if request is cancel from outside.
        if (iCancelRequest) {
            iParent->iErrorId = KErrCancel;
            // All processing is cancelled now. Notify observer.
            iObserver->HandleCompletionL(iParent);
            WakeupThreads(KErrCancel);
            iIsRequestRunning = EFalse;
            return EFalse;
        }
        // Wait for the first request.
        if (aTime > 0) {
            
            //qDebug() << "wait for aTime = " << aTime;
            User::WaitForRequest(iStatus, aRequest);
            //qDebug() << "aRequest = " << aRequest.Int();
            
            if (aRequest != KRequestPending) {
                // Timer has expired. return false.
                return EFalse;
            }
        }
        else {
            //qDebug() << "wait for iStatus";
            User::WaitForRequest(iStatus);
        }

        if (iOperation) {
            // if operation is not complete or database is locked 
            // then request again to complete it.
            if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked || iStatus
                == KErrAccessDenied) {
                iOperation->NextStep(iStatus, iProgress);
            }
            else if (iStatus == KErrNone && IsMultiOperationRequest()) {
                iObserver->HandleExecutionL(iParent, iStatus);
                TRequestStatus *Ptr = &iStatus;
                User::RequestComplete(Ptr, 0);
            }
            // if request is complete or any error occurred 
            // then complete the request with appropriate error code
            else {
                iIsComplete = ETrue;
                iParent->iErrorId = iStatus.Int();
                // All processing is done now. Notify observer.
                iObserver->HandleCompletionL(iParent);
                WakeupThreads(iStatus.Int());
                iIsRequestRunning = EFalse;
            }
        }
        // if operation object is not defined.
        else {
            TRAPD(err,iObserver->HandleExecutionL(iParent, iStatus);)
            if (err != KErrNone) {
                iStatus = err;
            }
            // prepare next request.
            if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked || iStatus
                == KErrAccessDenied) {

                if (iOperation) {
                    iOperation->NextStep(iStatus, iProgress);
                }
                else {
                    // Only for the Landmark save and Category save. These dont have async operations.
                    // Self completing ative object.
                    TRequestStatus *Ptr = &iStatus;
                    User::RequestComplete(Ptr, KErrNone);
                }
            }
            // if request is complete or any error occurred 
            // then complete the request with appropriate error code
            else {
                iIsComplete = ETrue;
                iParent->iErrorId = iStatus.Int();
                // All processing is done now. Notify observer.
                iObserver->HandleCompletionL(iParent);
                WakeupThreads(iStatus.Int());
                iIsRequestRunning = EFalse;
            }
        }
    }
    //qDebug() << "WaitForRequestL end";
    return ETrue;
}

void CLandmarkRequestAO::DoCancel()
{
    //qDebug() << "DoCancel start";
    
    if (iOperation) {
        //qDebug() << "a operational request is canceled";

        // Only way to cancel the operation is to delete the CPosLmOperation object.
        // Will also take care of cancelling any request which may be pending.
        delete iOperation;
        iOperation = NULL;
    }
    else {
        if (IsActive())
            memset(&iStatus, 0, sizeof(TRequestStatus));
    }
    
    // All processing is cancelled now. Notify observer.
    iParent->iErrorId = KErrCancel;
    TRAPD(err,iObserver->HandleCompletionL(iParent);)
    if (err != KErrNone) {
        //qDebug() << "DoCancel failed with = " << err;
    }
    
    WakeupThreads(KErrCancel);
    iIsRequestRunning = EFalse;
    
    //qDebug() << "DoCancel end";
}

CLandmarkRequestAO * CLandmarkRequestAO::NewL(MLandmarkRequestObserver *aObserver,
    CPosLmOperation *aOperation)
{
    CLandmarkRequestAO *self = new (ELeave) CLandmarkRequestAO(aOperation, aObserver);
    return self;
}

CLandmarkRequestAO::~CLandmarkRequestAO()
{
    //qDebug() << "~CLandmarkRequestAO start";
    iIsDestructing = ETrue;
    CancelRequest();
    //qDebug() << "~CLandmarkRequestAO end";
}

TBool CLandmarkRequestAO::StartRequest(CPosLandmarkSearch *aLandmarkSearch)
{
    //qDebug() << "CLandmarkRequestAO::StartRequest -> Request Started";

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
        TRAPD(err, iObserver->HandleExecutionL(iParent, iStatus);)

        if (err != KErrNone) {
            iStatus = err;
        }

        // prepare next request.
        if (iStatus == KPosLmOperationNotComplete || iStatus == KErrLocked || iStatus
            == KErrAccessDenied) {

            iStatus = KRequestPending;
            if (iOperation) {
                iOperation->NextStep(iStatus, iProgress);
                SetActive();
            }
            else {
                SetActive();
                // Only for the Landmark save and Category save. These dont have async operations.
                // Self completing ative object.
                TRequestStatus *Ptr = &iStatus;
                User::RequestComplete(Ptr, KErrNone);
            }
        }
        // if request is complete or any error occurred 
        // then complete the request with appropriate error code
        else {
            iIsComplete = ETrue;
            iParent->iErrorId = iStatus.Int();
            // All processing is done now. Notify observer.
            iObserver->HandleCompletionL(iParent);
            WakeupThreads(iStatus.Int());
            iIsRequestRunning = EFalse;
        }

    }

    iIsRequestRunning = ETrue;

    return ETrue;
}

TBool CLandmarkRequestAO::CancelRequest()
{
    //qDebug() << "CancelRequest start";

    iCancelRequest = ETrue;

    Cancel();
    if (iIsRequestRunning) {
        //qDebug() << "Cancelling request forcefully";
        DoCancel();
    }

    //qDebug() << "CancelRequest end";

    if (iParent->iErrorId == KErrCancel)
        return ETrue;
    else
        return EFalse;
}

TBool CLandmarkRequestAO::WaitForFinished(TInt aTime)
{
    //qDebug() << "WaitForFinished start, aTime = " << aTime;
    if (iIsComplete || iCancelRequest) {
        return ETrue;
    }

    RTimer timer;
    TRequestStatus timestatus = KErrNone;
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
        TRAPD(err,IsOperationComplete = WaitForRequestL(aTime, timestatus);)
        if (err != KErrNone) {
            //qDebug() << "WaitForRequestL error = " << err;
        }
        // TODO: Modify this hack if required
        if (IsActive() && IsOperationComplete)
            memset(&iStatus, 0, sizeof(TRequestStatus));
    }
    else {
        //qDebug() << "WFF for a non - AO owner Id is" << currThrdId;
        TWaitThrdData Data;
        Data.iThrdId = currThrdId;
        Data.iThrdRequest = KRequestPending;
        iParent->iWaitThrds.AddFirst(Data);

        //qDebug() << "Wait for request started";
        if (aTime > 0) {
            User::WaitForRequest(Data.iThrdRequest, timestatus);
            if (timestatus == KRequestPending) {
                IsOperationComplete = ETrue;
            }
        }
        else {
            IsOperationComplete = ETrue;
            User::WaitForRequest(Data.iThrdRequest);
        }
        //qDebug() << "Out of Wait for request in" << currThrdId;
    }
    if (timestatus == KRequestPending) {
        timer.Cancel();
    }
    timer.Close();

    iIsRequestRunning = EFalse;

    //qDebug() << "WaitForFinished end";
    return IsOperationComplete;
}

void CLandmarkRequestAO::SetOperation(CPosLmOperation *aOp)
{
    iOperation = aOp;
}

void CLandmarkRequestAO::SetExportData(CPosLandmarkEncoder *aEncoder, RFs &aFs, HBufC *aExportPath,
    CBufBase *aExportBuffer, QList<QLandmarkId> lmIds)
{
    if (iParent) {

        if (aEncoder)
            iParent->iLandmarkEncoder = aEncoder;
        if (aExportBuffer)
            iParent->iExportBuffer = aExportBuffer;
        if (&lmIds != 0 && lmIds.size() > 0)
            iParent->iLandmarkIds = lmIds;
        if (aExportPath)
            iParent->iExportPath = aExportPath;

        iParent->iFileSystem = aFs;
    }
}

/**
 * This is used only incase of import landmarks async request
 * in hanlde completion
 */
CPosLmOperation * CLandmarkRequestAO::GetOperation()
{
    return iOperation;
}

void CLandmarkRequestAO::WakeupThreads(TInt aCompletion)
{
    //qDebug() << "In wakeup threads start, thread id = " << RThread().Id();
    TWaitThrdData* Ptr = NULL;
    TSglQueIter<TWaitThrdData> Iter(iParent->iWaitThrds);

    //qDebug() << "iIsRequestRunning = " << iIsRequestRunning << "\tiIsDestructing = " << iIsDestructing;
    
    if (!iIsDestructing)
        iParent->iLock.Wait();

    Iter.SetToFirst();
    while ((Ptr = Iter++) != NULL) {

        RThread waiter;
        if (waiter.Open(Ptr->iThrdId) == KErrNone) {
            TRequestStatus* reqptr = &Ptr->iThrdRequest;
            //qDebug() << "Waking up thread" << Ptr->iThrdId;
            waiter.RequestComplete(reqptr, aCompletion);
            waiter.Close();

        }

        iParent->iWaitThrds.Remove(*Ptr);
    }
    iParent->iLock.Signal();
    //qDebug() << "In wakeup threads end ";
}

TBool CLandmarkRequestAO::IsMultiOperationRequest()
{
    if (iParent->iQtRequest->type() == QLandmarkAbstractRequest::LandmarkIdFetchRequest
        || iParent->iQtRequest->type() == QLandmarkAbstractRequest::LandmarkFetchRequest) {

        QLandmarkIdFetchRequest *fetchRequest =
            static_cast<QLandmarkIdFetchRequest *> (iParent->iQtRequest);

        if (fetchRequest->filter().type() == QLandmarkFilter::IntersectionFilter
            || fetchRequest->filter().type() == QLandmarkFilter::UnionFilter) {
            return ETrue;
        }
    }

    return EFalse;
}

TInt CLandmarkRequestAO::RunError(TInt aError)
{
    iIsComplete = ETrue;
    iParent->iErrorId = aError;
    // All processing is done now. Notify observer.
    TRAP_IGNORE(iObserver->HandleCompletionL(iParent);)
    WakeupThreads(iStatus.Int());

    //qDebug() << "RunL failed with = " << aError;
    return KErrNone;
}

/************************************/

CLandmarkRequestData::CLandmarkRequestData(QLandmarkAbstractRequest *aQtReq,
    CLandmarkRequestAO *aAO) :
    iOwnerAO(aAO), iQtRequest(aQtReq), iLandmarkSearch(NULL)
{
    iWaitThrds.SetOffset(_FOFF(TWaitThrdData, iLink));
    iOwnerThrd = RThread().Id();
    iOwnerAO->SetParent(this);
}

CLandmarkRequestData * CLandmarkRequestData::NewL(QLandmarkAbstractRequest *aQtReq,
    CLandmarkRequestAO *aAO)
{
    CLandmarkRequestData *self = new (ELeave) CLandmarkRequestData(aQtReq, aAO);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

void CLandmarkRequestData::ConstructL()
{
    User::LeaveIfError(iLock.CreateLocal());
    error = QLandmarkManager::NoError;
    errorString.clear();
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

    if (iLandmarkSearch) {
        delete iLandmarkSearch;
        iLandmarkSearch = NULL;
    }

    if (iOwnerAO) {
        delete iOwnerAO;
        iOwnerAO = NULL;
    }

    Reset();

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
    iOpCount = 0;
    error = QLandmarkManager::NoError;
    errorString.clear();

    iAddedLandmarkIds.clear();
    iChangedLandmarkIds.clear();
    iRemovedLandmarkIds.clear();
    iFetchedLandmarkIds.clear();

    iAddedCategoryIds.clear();
    iChangedCategoryIds.clear();
    iRemovedCategoryIds.clear();
    iFetchedCategoryIds.clear();
}

/**
 * LandmarkRequestHandler implementations
 */

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
        current = iRequestList[idx];
        if (current) {
            delete current;
            current = NULL;
        }
    }
    iRequestList.Close();
    iRequestListLock.Signal();
    iRequestListLock.Close();
}

TInt LandmarkRequestHandler::AddAsyncRequest(QLandmarkAbstractRequest *aQtReq,
    CLandmarkRequestAO *aAO)
{
    CLandmarkRequestData *currentRequestData = NULL;
    iRequestListLock.Wait();
    for (TInt idx = 0; idx < iRequestList.Count(); idx++) {
        if (iRequestList[idx]->iQtRequest == aQtReq) {
            currentRequestData = iRequestList[idx];
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
    TInt ret = iRequestList.Append(currentRequestData);
    iRequestListLock.Signal();
    return ret;
}

TBool LandmarkRequestHandler::RemoveAsyncRequest(QLandmarkAbstractRequest *aQtReq)
{
    TBool flag = false;
    CLandmarkRequestData* Current = NULL;
    iRequestListLock.Wait();
    for (TInt idx = 0; idx < iRequestList.Count(); idx++) {
        if (iRequestList[idx]->iQtRequest == aQtReq) {
            Current = iRequestList[idx];
            if (Current) {
                delete Current;
                Current = NULL;
                flag = true;
            }
            iRequestList.Remove(idx);
            break;
        }
    }
    iRequestListLock.Signal();
    return flag;
}

CLandmarkRequestData* LandmarkRequestHandler::FetchAsyncRequest(QLandmarkAbstractRequest *aQtReq)
{
    CLandmarkRequestData *current = NULL;
    iRequestListLock.Wait();
    for (TInt idx = 0; idx < iRequestList.Count(); idx++) {
        if (iRequestList[idx]->iQtRequest == aQtReq) {
            current = iRequestList[idx];
            break;
        }
    }
    iRequestListLock.Signal();
    return current;
}

// EOF
