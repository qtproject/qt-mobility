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

#include "s60_QGeoPositionInfoSourceS60.h"
#include "s60_QMLBackendAO.h"

//The name of the requestor //Added by PM
_LIT(KRequestor,"QTMobility Location");

// constructor
CQMLBackendAO::CQMLBackendAO() :
    CActive(EPriorityStandard), // Standard priority
    mPosInfo(NULL),mRequester(NULL),mRequestType(RequestType(0)) 
    {
    }

//
//
//
CQMLBackendAO* CQMLBackendAO::NewLC(CQGeoPositionInfoSourceS60 *requester, 
                            RequestType  requestType, TPositionModuleId  modID)
    {
    CQMLBackendAO* self = new (ELeave) CQMLBackendAO();
    CleanupStack::PushL(self);
    self->ConstructL(requester, requestType, modID);
    return self;
    }

//
//
//
CQMLBackendAO* CQMLBackendAO::NewL(CQGeoPositionInfoSourceS60 *requester, 
                            RequestType  requestType, TPositionModuleId  modID)
    {
    CQMLBackendAO* self = CQMLBackendAO::NewLC(requester, requestType, modID);
    CleanupStack::Pop(); // self;
    return self;
    }

//
//
//
TInt CQMLBackendAO::ConstructL(CQGeoPositionInfoSourceS60 *requester, RequestType  requestType, 
                                                                      TPositionModuleId  modId)
    {
    TInt error = KErrNone;
    
    mRequester = requester;
    
    mRequestType = requestType;
    
    
    
    if( (mRequestType == REQ_REG_UPDATE) || (mRequestType == REQ_ONCE_UPDATE) )
        {
		RPositionServer &PosServer = mRequester->getPositionServer();
        error  =  mPositioner.Open(PosServer,modId);

        if( error != KErrNone  )
            return error;

        CleanupClosePushL(mPositioner);

        error = mPositioner.SetRequestor( CRequestor::ERequestorService ,
                CRequestor::EFormatApplication, _L("QTmobility_Location"));

        CleanupStack::Pop(1);

        if( error != KErrNone  )
            return error;
          
        }
       
    CActiveScheduler::Add(this); // Add to scheduler
    
    return error;
    }

// 
CQMLBackendAO::~CQMLBackendAO()
    {
    Cancel();
    
    if( (mRequestType == REQ_ONCE_UPDATE) || (mRequestType == REQ_REG_UPDATE) )
        {
        //close the subsession
        mPositioner.Close();
        
        if( mPosInfo)
            {
            mPosInfo->ClearRequestedFields();

            mPosInfo->ClearPositionData();
            //delete the HPositionGenericInfo
            delete mPosInfo;
        }
        }
    else if(mRequestType == REQ_DEV_STATUS)
        {
        //done only by the position server Active Object
        RPositionServer &posServer = mRequester->getPositionServer();
        posServer.Close();
        }
    }

//
void CQMLBackendAO::DoCancel()
    {
    CancelAll();
    }


//
void CQMLBackendAO::RunL()
    {
     switch(mRequestType)
	{
	case REQ_DEV_STATUS :
				handleDeviceNotification(iStatus.Int());
				break;
	case REQ_REG_UPDATE :
	case REQ_ONCE_UPDATE:
				handlePosUpdateNotification(iStatus.Int());
				break;
	default 	    :
                break;
	}	
    }

//
TInt CQMLBackendAO::RunError(TInt aError)
    {
    return aError;
    }


// checks any pending request in activeobject 
bool CQMLBackendAO::IsRequestPending()
    {
        if(IsActive())
            return true;
        else
            return false;
    }



// Async call to get notifications about device status.
void CQMLBackendAO::NotifyDeviceStatus(TPositionModuleStatusEventBase &aStatusEvent)
    {
    RPositionServer& PosServ = mRequester->getPositionServer();
    
    //register for device status events
    TPositionModuleStatusEventBase::TModuleEvent RequestedEvents(
                                   TPositionModuleStatusEventBase::EEventDeviceStatus);
    
    aStatusEvent.SetRequestedEvents(RequestedEvents);
    
    PosServ.NotifyModuleStatusEvent(aStatusEvent,iStatus);
    
    SetActive();
        
    }

void CQMLBackendAO::CancelAll()
    {
    TInt error;
 
        if(mRequestType == REQ_DEV_STATUS)
            {
            RPositionServer &PosServer = mRequester->getPositionServer();
            error = PosServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
            }
        else if( (mRequestType == REQ_ONCE_UPDATE) || (mRequestType == REQ_REG_UPDATE) )
            {
            mPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
            }
    
    }

//Initialize the posInfo with appropriate fields 
bool CQMLBackendAO::InitializePosInfo()
    {
    if( !mPosInfo )
        {
        mPosInfo = HPositionGenericInfo::New();
        
        if(mPosInfo == NULL)
            return FALSE;
        }
    else
        {
        mPosInfo->ClearRequestedFields();
        
        mPosInfo->ClearPositionData();
        }
    RPositionServer posServer;
    TPositionModuleInfo moduleInfo;
    TInt error = KErrNone;
    
    //get the posiiton server
    posServer = mRequester->getPositionServer();
    
    //retrieve the module id used by the posiitoner
    if( mRequestType == REQ_REG_UPDATE )
        error = posServer.GetModuleInfoById(mRequester->getCurrentPositonModuleID(), moduleInfo);
    else if( mRequestType == REQ_ONCE_UPDATE )
        error = posServer.GetModuleInfoById(mRequester->getRequestUpdateModuleID(), moduleInfo);
    
    if (error == KErrNone) {
        
        //get capabilities of the module
        TPositionModuleInfo::TCapabilities caps = moduleInfo.Capabilities();
      
        if (caps & TPositionModuleInfo::ECapabilitySatellite) {
                mPosInfo->SetRequestedField(EPositionFieldSatelliteNumInView);
                mPosInfo->SetRequestedField(EPositionFieldSatelliteNumUsed);
        }
        
        if (caps & TPositionModuleInfo::ECapabilitySpeed) {
                mPosInfo->SetRequestedField(EPositionFieldHorizontalSpeed);
                mPosInfo->SetRequestedField(EPositionFieldVerticalSpeed);
        }
        if (caps & TPositionModuleInfo::ECapabilityCompass) {
                mPosInfo->SetRequestedField(EPositionFieldMagneticCourseError);
                mPosInfo->SetRequestedField(EPositionFieldHeading);
        }
        return TRUE;
    }
    return FALSE;
    }

//requestUpdate : request for position update once
void CQMLBackendAO::requestUpdate(TInt aTimeout)
    {
    TPositionUpdateOptions  aPosOption;
    
    TInt ret = FALSE;
    
    TInt error = mPositioner.GetUpdateOptions(aPosOption);
    
    aPosOption.SetUpdateInterval(TTimeIntervalMicroSeconds(0));
    
    aPosOption.SetUpdateTimeOut(TTimeIntervalMicroSeconds(aTimeout * 1000));
    
    mPositioner.SetUpdateOptions(aPosOption);
    
    //setUpdateInterval(aTimeout);
    startUpdates();
    }


//    
void CQMLBackendAO::CancelUpdate()
    {
    Cancel();
    
    }

    
//
bool CQMLBackendAO::handleDeviceNotification (int error)
    {
    switch(error)
        {
        //NotifyPositionModulestatusEvent successfully completed
        case KErrNone :
            
        //module not found    
        case KErrNotFound :
            mRequester->UpdateDeviceStatus();
            break;
            
        //request has been successfully cancelled    
        case KErrCancel :
            break;
            
        //unrecoverabe errors    
        default : 
            break;
        }
    }


//
bool CQMLBackendAO::handlePosUpdateNotification (int error)
    {
   
       switch(error)
        {
        //NotifyPositionUpdate successfully completed 
        case KErrNone :
            
        //Posiiton Module is unable to return any position information    
        case KPositionQualityLoss :
        
        //requested information could not be retrieved within the maximum peroid    
        case KErrTimedOut:
            
            HPositionGenericInfo *positionInfo = NULL;

            positionInfo = HPositionGenericInfo::New();

            if(positionInfo == NULL)
                return;  
            
            //copy the buffer contents into a new HPositionGenericInfo buffer,to be used 
            //for creating QGeoPositionInfo object later
            memcpy(positionInfo , mPosInfo , mPosInfo->BufferSize());

            //if regUpdateAO, request for the next update 
            if(mRequestType == REQ_REG_UPDATE)
                {
                InitializePosInfo();
                mPositioner.NotifyPositionUpdate( *mPosInfo , iStatus);
                SetActive();
                }

            //KErrTimedOut should not be emited for regular update
            if( (error!= KErrTimedOut) || (mRequestType != REQ_REG_UPDATE))
                mRequester->UpdatePosition(positionInfo,error);

            delete positionInfo;
                
            break;
        
        //request was successfully cancelled    
        case KErrCancel :
            break;
        
        default :
            break;

        }
    }

//////////////////////////////////////////////////////////////
// Sets the interval for getting the regular notification   //
// the time interval set is in milli seconds                      //
//////////////////////////////////////////////////////////////
int CQMLBackendAO::setUpdateInterval ( int msec )
    {
    int minimumUpdateInterval = 0;
    TInt64 mUpdateInterval = 0 ;
    
    if(mRequester)
    minimumUpdateInterval = mRequester->minimumUpdateInterval();
    
    // if the current requesttype is  regular updates
    // then set the updateinterval otherwise ignore
    //if(mRequestType != REQ_REG_UPDATE)
    //    return;
    
    TPositionUpdateOptions  aPosOption;
    
    TInt error = mPositioner.GetUpdateOptions(aPosOption);
    
    // TTimeIntervalMicroSeconds is converted seconds
    TInt currentUpdateInterval  = aPosOption.UpdateInterval().Int64() / 1000;

    // If msec is not 0 and is less than the value returned by minimumUpdateInterval(), 
    // the interval will be set to the minimum interval.
    if( msec != 0 && msec <= minimumUpdateInterval)
        {
            mUpdateInterval = minimumUpdateInterval;       
        }
        
    // if the same value is being set then just ignore it.
    if( currentUpdateInterval == msec )
        {   
            if( msec != mUpdateInterval )
                mUpdateInterval = msec;
            return mUpdateInterval;
        }
  
    // if Zero or above minimum value will be set.
    if(msec == 0 || msec > minimumUpdateInterval )
    {     
        mUpdateInterval = msec;     
    }

    // will set Either zero, minimum or +ve value
    // seconds converted to TTimeIntervalMicroSeconds
    aPosOption.SetUpdateInterval(TTimeIntervalMicroSeconds (mUpdateInterval * 1000));
    error = mPositioner.SetUpdateOptions(aPosOption);
    
    return mUpdateInterval;
    }    

void CQMLBackendAO::startUpdates()
    {
    InitializePosInfo();
    mPositioner.NotifyPositionUpdate( *mPosInfo , iStatus);
    if(!IsActive())
    SetActive();
    }
