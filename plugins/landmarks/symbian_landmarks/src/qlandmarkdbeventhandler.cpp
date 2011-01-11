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

#include "qlandmarkdbeventhandler.h"
#include <qdebug.h>

CLandmarkDbEventHandler::CLandmarkDbEventHandler() :
    CActive(EPriorityStandard)
{
    // No implementation
}

/*
 * This should be called before any use, it initializes all the required objects
 */
void CLandmarkDbEventHandler::InitializeL()
{
    TInt err = KErrNone;
    iDatabase = CPosLandmarkDatabase::OpenL();
    err = iObserverLock.CreateLocal(EOwnerProcess);
    User::LeaveIfError(err);
    CActiveScheduler::Add(this);
    // Acquire the lock, this is required as the observers cannot modified
    // when the observer list is being used
    iObserverLock.Wait();
    StartListening();
    // release the lock
    iObserverLock.Signal();
    iIsInitialized = ETrue;
}

/*
 * StartListening() is called to start listening to the database event
 * It has to be protected by the iObserverLock. It is not taken inside this function
 * for performance reasons.
 */
void CLandmarkDbEventHandler::StartListening()
{
    //qDebug() << "starting database event notification";
    
    // If there is atleast one observer and no request already pending
    // then request for notification
    if (iObserverList.Count() > 0 && !IsActive()) {
        iDatabase->NotifyDatabaseEvent(iEvent, iStatus);
        //qDebug() << "CLandmarkDbEventHandler::StartListening => SetActive";
        SetActive();
    }
}

/*
 * Default Destructor
 * This is not called on winscw emulator, as it is static object
 */
CLandmarkDbEventHandler::~CLandmarkDbEventHandler()
{
    //qDebug() << "CLandmarkDbEventHandler::~CLandmarkDbEventHandler - start";
    Cancel();
    iObserverLock.Close();
    iObserverList.Close();
    delete iDatabase;
    //qDebug() << "CLandmarkDbEventHandler::~CLandmarkDbEventHandler - finish";
}

/*
 * Called by ActiveScheduler when an event occurs on this active object,
 * i.e., when a landmark database event occurs
 */
void CLandmarkDbEventHandler::RunL()
{
    //qDebug() << "CLandmarkDbEventHandler::RunL() start";
    // If the event completed successfully
    if (iStatus.Int() == KErrNone) {
        // Acquire the lock, this is required as the observers cannot modified
        // when the observer list is being used
        iObserverLock.Wait();
        // Iterate through all the registered observers and call handleDatabaseEvent
        for (TInt i = 0; i < iObserverList.Count(); i++) {
            // Call the observer and indicate the event occurred

            //qDebug() << "Notifying " << i+1 << " observer";
            iObserverList[i]->handleDatabaseEvent(iEvent);

        }
        // Request for database notification again
        StartListening();

        // release the lock
        iObserverLock.Signal();
    }
    //qDebug() << "CLandmarkDbEventHandler::RunL() end";
}

void CLandmarkDbEventHandler::DoCancel()
{
    //qDebug() << "CLandmarkDbEventHandler::DoCancel, stopping database event notification";
    // Cancel landmark database notification request
    iDatabase->CancelNotifyDatabaseEvent();
    //qDebug() << "CLandmarkDbEventHandler::DoCancel - finish";
}

/*
 * AddObserver is used to add an observer, the observer will be notified upon
 * database event completion
 * The observer derives from MLandmarkDbEventObserver class
 */
TInt CLandmarkDbEventHandler::AddObsever(MLandmarkDbEventObserver* aObserverHandle)
{
    //qDebug() << "Adding Observer - start ";

    if (!iIsInitialized) {
        TRAPD(err,InitializeL());
        if (err) {
            return err;
        }
    }
    // Acquire the lock, this is required as the observers cannot be added,
    // when the observer list is being used
    iObserverLock.Wait();
    TInt ret = iObserverList.InsertInAddressOrder(aObserverHandle);
    // release the lock
    StartListening();
    iObserverLock.Signal();
    //qDebug() << "Adding Observer - end, ret = " << ret;
    return ret;
}

/*
 * AddObserver is used to remove an observer, the observer will be notified upon
 * database event completion
 * The observer derives from MLandmarkDbEventObserver class
 */
TInt CLandmarkDbEventHandler::RemoveObsever(MLandmarkDbEventObserver* aObserverHandle)
{
    //qDebug() << "Removing Observer - start";

    if (!iIsInitialized) {
        TRAPD(err,InitializeL());
        if (err) {
            return err;
        }
    }

    //qDebug() << "start the lock";
    
    // Acquire the lock, this is required as the observers cannot be removed,
    // when the observer list is being used
    iObserverLock.Wait();
    // Find the observer in the list
    TInt index = iObserverList.FindInAddressOrder(aObserverHandle);
    if (index != KErrNotFound) {
        // Remove the observer if found
        iObserverList.Remove(index);
        iObserverList.Compress();
        // this is done to reuse index for return value, note index does not contain error code
        // if successful, hece this is required
        index = KErrNone;
    }

    // release the lock
    //qDebug() << "release the lock";
    iObserverLock.Signal();

    if (iObserverList.Count() <= 0 && IsActive())
        Cancel();
    
    //qDebug() << "Removing Observer - end, index = " << index;

    return index;
}

