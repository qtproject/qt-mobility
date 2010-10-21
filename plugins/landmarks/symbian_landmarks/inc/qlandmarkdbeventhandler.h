/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in 
 ** accordance with the Qt Commercial License Agreement provided with
 ** the Software or, alternatively, in accordance with the terms
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
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#ifndef QLANDMARK_DB_EVENT_HANDLER_H
#define QLANDMARK_DB_EVENT_HANDLER_H

// System Includes
#include <EPos_CPosLandmarkDatabase.h>
#include <e32base.h>

/*
 * Observers that require notification of database events should derive from MLandmarkDbEventObserver
 * and register with the Event Handler using CLandmarkDbEventHandler::AddObserver
 */
class MLandmarkDbEventObserver
{
public:
    virtual void handleDatabaseEvent(const TPosLmEvent& aEvent) = 0;
};

/**
 * CLandmarkDbEventHandler class is used to observer the landmark database
 * events. It is a self running active object
 * The initialization happens only once ensuring there is only one object of CLandmarkDbEventHandler
 * There 
 */
NONSHARABLE_CLASS( CLandmarkDbEventHandler ): public CActive
{
public:

    /*
     * Default Ctor
     */
    CLandmarkDbEventHandler();

    /*
     * Default dtor
     */
    ~CLandmarkDbEventHandler();

    /*
     * AddObserver is used to add an observer, the observer will be notified upon
     * database event completion
     * The observer derives from MLandmarkDbEventObserver class
     * The observer object destruction is to be done by the caller of AddObserver
     */
    TInt AddObsever(MLandmarkDbEventObserver* aObserverHandle);

    /*
     * RemoveObsever is used to remove an observer, the observer will be notified upon
     * database event completion
     * The observer derives from MLandmarkDbEventObserver class
     * The observer object destruction is to be done by the caller of AddObserver
     */
    TInt RemoveObsever(MLandmarkDbEventObserver* aObserverHandle);

private:

    /*
     * Called by ActiveScheduler when an event occurs on this active object,
     * i.e., when a landmark database event occurs
     */
    void RunL();

    /*
     * Used to cancel the notification request
     */
    void DoCancel();

    /*
     * This should be called before any use, it initializes all the required objects
     */
    void InitializeL();

    /*
     * StartListening() is called to start listening to the database event
     * It has to be protected by the iObserverLock. It is not taken inside this function
     * for performance reasons.
     */
    void StartListening();

private:
    /*
     * This is a handle to the database, it is used for Notification request
     */
    CPosLandmarkDatabase* iDatabase;

    /*
     * Holds the event that occurred in the landmark database
     */
    TPosLmEvent iEvent;

    /*
     * Holds the observers, the destruction of the observer is not done by this array
     * It is the responsibility of the caller of AddObserver
     */
    RPointerArray<MLandmarkDbEventObserver> iObserverList;

    /*
     * To synchronize the access to the observer list
     */
    RFastLock iObserverLock;

    /*
     * ETrue if Initialize is done
     */
    TBool iIsInitialized;

};

#endif // QLANDMARK_DB_EVENT_HANDLER_H
