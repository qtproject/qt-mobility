/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLBACKENDAO_S60_P_H
#define QMLBACKENDAO_S60_P_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <lbs.h>
#include <lbscommon.h>

#include "qgeopositioninfosource.h"
//#include "QGeoPositionInfoSourceS60.h"

class CQGeoPositionInfoSourceS60 ;

// Types of request 
enum RequestType{ 
    REQ_REG_UPDATE = 1,  // Regular updates
    REQ_ONCE_UPDATE,     // request update
    REQ_DEV_STATUS       // device updates
    };

// 
class CQMLBackendAO : public CActive
    {
public:
    // Cancel and destroy
    ~CQMLBackendAO();

    // Two-phased constructor.
    static CQMLBackendAO* NewL(CQGeoPositionInfoSourceS60 *requester, 
                                RequestType  requestType, TPositionModuleId  modId = TUid::Uid(0));

    // Two-phased constructor.
    static CQMLBackendAO* NewLC(CQGeoPositionInfoSourceS60 *requester, 
            RequestType  requestType, TPositionModuleId  modID);
    
    // checks any pending request in activeobject 
    bool IsRequestPending(); 
    
    // Async call to get notifications about device status.
    void NotifyDeviceStatus(TPositionModuleStatusEventBase &aStatusEvent) ;
    
    // requesting for position update once
    void requestUpdate(TInt aTimeout);
    
    // cancels an outstanding update request
    void CancelUpdate();
    
    // Sets an interval in millisecs for regular notifications 
    int setUpdateInterval ( int msec );
    
    void startUpdates();
     

public:
    // New functions
    // Function for making the initial request
    void StartL(TTimeIntervalMicroSeconds32 aDelay);

private:
    // C++ constructor
    CQMLBackendAO();

    // Second-phase constructor
    TInt CQMLBackendAO::ConstructL(CQGeoPositionInfoSourceS60 *requester, RequestType  requestType, 
                                                                          TPositionModuleId  modId);
    
    // Device Notifications are handled
    bool handleDeviceNotification (int error);

    // regular position notifications are handled
    bool handlePosUpdateNotification (int error);

    void CancelAll();
    
    bool InitializePosInfo();
    
private:
    // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    int RunError(int aError);

private:
    enum TQMLBackendAOState
        {
        EUninitialized, // Uninitialized
        EInitialized, // Initalized
        EError
        // Error condition
        };
    
private:
    
    // Request is a device or a regular
    CQGeoPositionInfoSourceS60 *mRequester;

    // Request type once, regular, device
    RequestType  mRequestType;
    
    // Positioning Information
    RPositioner mPositioner;
    
    HPositionGenericInfo *mPosInfo;

    };

#endif // QMLBACKENDAO_S60_P_H
