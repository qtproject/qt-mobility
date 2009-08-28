/* /****************************************************************************
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

#include <limits.h>

#include <QObject>
#include <QDateTime>
#include "qgeopositioninfosource_s60_p.h"
#include "qgeopositioninfosource.h"
#include "qmlbackendao_s60_p.h"

// constructor
CQGeoPositionInfoSourceS60::CQGeoPositionInfoSourceS60(QObject * parent) : QGeoPositionInfoSource(parent),
                currentModuleID(TUid::Null()),reqmoduleID(TUid::Null()),devStatusUpdateAO(NULL),
                reqUpdateAO(NULL),regUpdateAO(NULL),supportedMethods(PositioningMethod(0)),currentMethod(PositioningMethod(0)),
                ListSize(0),mStartUpdates(FALSE),ModuleFlags(0)
    {
    memset(list, 0 , MAX_SIZE * sizeof(CPosMethodInfo));
    }

// destructor
CQGeoPositionInfoSourceS60::~CQGeoPositionInfoSourceS60()
    {
   	if (reqUpdateAO) 
		delete reqUpdateAO;
	if (regUpdateAO) 
		delete regUpdateAO;
	if (devStatusUpdateAO) 
		delete devStatusUpdateAO; 

	
    }

// static function NewLC
CQGeoPositionInfoSourceS60* CQGeoPositionInfoSourceS60::NewLC(QObject * parent)
    {
    CQGeoPositionInfoSourceS60* self =
            new (ELeave) CQGeoPositionInfoSourceS60(parent);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// static function NewL
CQGeoPositionInfoSourceS60* CQGeoPositionInfoSourceS60::NewL(QObject * parent)
    {
    CQGeoPositionInfoSourceS60* self = CQGeoPositionInfoSourceS60::NewLC(parent);
    CleanupStack::Pop(); 
    
    //check if the second phase construction is successful
    if( !self->isValid())
        {
        delete self;
        self = NULL;
        }
    
    return self;
    }

// 2nd phase constructor
void CQGeoPositionInfoSourceS60::ConstructL()
    {
    TInt error = mPositionServer.Connect();
 
    if(error == KErrNone)
    {    
    CleanupClosePushL( mPositionServer );
    
    devStatusUpdateAO = CQMLBackendAO::NewL(this,REQ_DEV_STATUS);
    
    if( devStatusUpdateAO == NULL)
        {
        CleanupStack::Pop(1);
        
        return;
        }
    
    //update the list array with the available method initially
    UpdateDeviceStatus();
    
    //devStatusUpdateAO->NotifyDeviceStatus(mStatusEvent);

    CleanupStack::PushL(devStatusUpdateAO);
    
    if(currentModuleID != TUid::Null())
        regUpdateAO = CQMLBackendAO::NewL(this,REQ_REG_UPDATE,currentModuleID);
     
    CleanupStack::Pop(2);
    }
     
    }

QGeoPositionInfo CQGeoPositionInfoSourceS60::getLastknownPositionS60(TPositionModuleId moduleID) const
    {
    TRequestStatus status;
    TPosition pos;
    TInt error = KErrNone;
    RPositioner iLastKnownpositioner;
    QGeoPositionInfo posUpdate;
  
    
    error=iLastKnownpositioner.Open(*const_cast<RPositionServer*>(&mPositionServer),moduleID);
      //request for lastknown position update and wait for the request to complete
    if(error == KErrNone)
        {
        TPositionInfo posInfo;
        TReal val;
        
        iLastKnownpositioner.SetRequestor( CRequestor::ERequestorService ,
                  CRequestor::EFormatApplication, _L("QTmobility_Location"));

        
        iLastKnownpositioner.GetLastKnownPosition(posInfo,status);

        User::WaitForRequest(status);

        error = status.Int();
        
        iLastKnownpositioner.Close();
    
        if( (error == KErrNone) || (error == KPositionPartialUpdate))
            {
                QGeoCoordinate  coord;
                
                posInfo.GetPosition(pos);
                
                coord.setLatitude(pos.Latitude());
                coord.setLongitude(pos.Longitude());
                coord.setAltitude(pos.Altitude());

                //store the QGeoCoordinate values 
                posUpdate.setCoordinate(coord);

                TDateTime datetime = pos.Time().DateTime();
                QDateTime dt(QDate(datetime.Year(),datetime.Month(),datetime.Day()),
                        QTime(datetime.Hour(),datetime.Minute(),datetime.Second(),
                                datetime.MicroSecond()/1000));

                //store the time stamp
                posUpdate.setDateTime(dt);

                //store the horizontal accuracy
                posUpdate.setProperty(QGeoPositionInfo::HorizontalAccuracy,pos.HorizontalAccuracy());

                //store the vertical accuracy
                posUpdate.setProperty(QGeoPositionInfo::VerticalAccuracy,pos.VerticalAccuracy());
            }
        }
    return posUpdate;
    }

// lastknownPosition 
QGeoPositionInfo CQGeoPositionInfoSourceS60::lastKnownPosition ( bool fromSatellitePositioningMethodsOnly ) const
   {
   //update_available_types();
   TUint8 bits;
   TInt index = -1;
   TPositionModuleId modID;
   QGeoPositionInfo posUpdate;
 
      if ( fromSatellitePositioningMethodsOnly )
          
          {
              if( currentMethod == SatellitePositioningMethods)
                  
              {
              posUpdate = getLastknownPositionS60(currentModuleID);
              }
              
              else if( supportedMethods & SatellitePositioningMethods )    
              {
              bits = ModuleFlags;
              
              PositioningMethod method = SatellitePositioningMethods; 
              
              do
                  {
                      index = getIndexPositionModule(bits);       
                      
                      if( index < 0)
                          break;
                      
                      modID = list[index].mUid;
                       
                      posUpdate = getLastknownPositionS60(modID);
                  
                      if(posUpdate.isValid())
                          break;
                      
                      bits = bits & (0XFF ^ (1<<index));
                  }while(1);
           
              }
          }
      
      else
          {
          bits = ModuleFlags;
          do
              {
                  index = getIndexPositionModule(bits);       
                  
                  if( index < 0)
                      break;
                  
                  modID = list[index].mUid;
                  
                  posUpdate = getLastknownPositionS60(modID);
              
                  if(posUpdate.isValid())
                      break;
                  
                  bits = bits & (0XFF ^ (1<<index));
              }while(1);
          
             
          }
      
      return posUpdate;
   }
  
//
int CQGeoPositionInfoSourceS60::minimumUpdateInterval () const
    {
        if(currentModuleID == TUid::Null())
	        return 0;
	    
	    TInt i = checkmodule(currentModuleID);
	    if (i!= -1)
	        return list[i].mTimeToNextFix.Int64()/1000;             //divide by 1000, to convert microsecond to milisecond
	    return 0;
}



//private function : to derive the supported positioning methods 
void CQGeoPositionInfoSourceS60::update_available_types()
    {
    PositioningMethods types;

    for(TInt i = 0;i < ListSize ; i++)
        {
        //check only for the available module without any device errors
        if(list[i].mIsAvailable && 
                (list[i].mStatus != TPositionModuleStatus::EDeviceUnknown) && 
                (list[i].mStatus != TPositionModuleStatus::EDeviceError) )
            {
                types |= list[i].mPosMethod;

            }
        
        //if both SatellitePositioningMethods and NonSatellitePositioningMethods return
        if( types.testFlag(AllPositioningMethods)) 
                    break;
        }
    
    //update the supported methods based on types value, derived from the above steps
    supportedMethods = types;  
    }

//private function : to retrieve the index of the supplied module id from the list array
TInt CQGeoPositionInfoSourceS60::checkmodule(TPositionModuleId id)const
    {
    TInt i;
    for(i=0;i<ListSize;i++)
        if(list[i].mUid == id)
            return i;
    return -1;
    }

//private function : get the index of the list that supports the preferred method if 
//available,else returns the index of the default module
TInt CQGeoPositionInfoSourceS60::getIndexPositionModule(TUint8 aBits,PositioningMethods posMethods) const
    {
    TInt index,error;
    
    TPositionModuleId modID;
    
    //get the default module Id, whose index is returned if the preferred methods are not
    //available
    error = mPositionServer.GetDefaultModuleId(modID);
    
    if(error != KErrNone)
        modID = TUid::Null();
    
    //index = -1 : no methods available in the list that supports preferred methods
    index= -1;  
    
       
        for(TInt i = 0; i < ListSize ; i++)
            {
            //check the module properties to select the preferred method,search should not
            //not select an unavailable method,error device or index matching aLastIndex  
            if( list[i].mIsAvailable && posMethods.testFlag(list[i].mPosMethod)
                && (list[i].mStatus != TPositionModuleStatus::EDeviceUnknown)           
                && (list[i].mStatus != TPositionModuleStatus::EDeviceError)
                && ( ((aBits >> i) & 1) ))
                {
                return i;
                }
            
            //store the index of the default module
            if( (modID == list[i].mUid) && ((aBits >> i) & 1))
                index = i;
            }
        
    
     return index;
    }

//private function : to get the index of the positioning method with time to first fix 
//lesser than timeout
TInt CQGeoPositionInfoSourceS60::getMoreAccurateMethod(TInt aTimeout,TUint8 aBits)
    {
    TInt index = -1, temp = INT_MAX;
    PositioningMethods posMethods;
    TTimeIntervalMicroSeconds microSeconds;
    
    posMethods = preferredPositioningMethods();
    
    //convert the timeout --> micro seconds
    microSeconds = aTimeout * 1000;
    
    for(TInt i =0 ;i < ListSize ; i++)
        {
        if( list[i].mIsAvailable 
                       //&& posMethods.testFlag(list[i].mPosMethod)
                       && (list[i].mStatus != TPositionModuleStatus::EDeviceUnknown)           
                       && (list[i].mStatus != TPositionModuleStatus::EDeviceError)
                       && (((aBits >> i) & 1)) 
                       &&  (list[i].mTimeToFirstFix < microSeconds))
            {
                if( list[i].mhorizontalAccuracy < temp )
                    {
                    index = i;
                    temp = list[i].mhorizontalAccuracy;
                    }
            }
        }
    return index;
    }

//private function : to update the list array
void CQGeoPositionInfoSourceS60::update_status(TPositionModuleInfo aModInfo,TInt aStatus)
    {

    TInt i,error, index;
    TPositionModuleId id;
    PositioningMethod method;
    TBool available;
    TReal32 accuracy;
    TTimeIntervalMicroSeconds time_to_first_fix,time_to_next_fix;
    TPositionQuality quality;
    CQMLBackendAO *temp = NULL;

    //query for the following parameters
    id = aModInfo.ModuleId();

    //module with a satellite capability is assumed to be provided satellite based positioning
    //method
    method = (aModInfo.Capabilities() & TPositionModuleInfo::ECapabilitySatellite) ?
        SatellitePositioningMethods : NonSatellitePositioningMethods;

    //gets the device availability based on the user settings   
    available = aModInfo.IsAvailable();             

    //quality : holds the required metrics
    aModInfo.GetPositionQuality(quality);    

    //Accuracy 
    accuracy = quality.HorizontalAccuracy();        

    //time taken for the first fix
    time_to_first_fix = quality.TimeToFirstFix();  

    //time taken for the subsequent fix
    time_to_next_fix = quality.TimeToNextFix(); 


    if( (i = checkmodule(id)) == -1)
        {
        //store the correct method in use from the currentmoduleID retireved earlier
        if( id == currentModuleID)
		{
            currentMethod = method;
			QGeoPositionInfoSource::setPreferredPositioningMethods(currentMethod);
		}
        //update the properties of the module

        //TPositionModuleId of the module
        list[ListSize].mUid = id;

        //positioning method method type
        list[ListSize].mPosMethod = method;

        //status of the device
        list[ListSize].mStatus = aStatus;

        //availablility of the module
        list[ListSize].mIsAvailable = available;

        //horizontal accuracy of the module
        list[ListSize].mhorizontalAccuracy = accuracy;

        //time required to get the first fix
        list[ListSize].mTimeToFirstFix = time_to_first_fix;

        //time required for subsequent fix
        list[ListSize].mTimeToNextFix = time_to_next_fix;

        //count on the list array size
        ListSize++;
        }
    else
        {
        //module's status has changed
        if(list[i].mStatus != aStatus)
            list[i].mStatus = aStatus;

        //module's availability has changed
        if(list[i].mIsAvailable != available)
            list[i].mIsAvailable = available;

        //module's horizontal accuracy has changed
        if(list[i].mhorizontalAccuracy != accuracy)
            list[i].mhorizontalAccuracy = accuracy;

        //module's time to first fix has changed
        if(list[i].mTimeToFirstFix != time_to_first_fix)
            list[i].mTimeToFirstFix = time_to_first_fix;

        //module's time to subsequent fix has changed
        if(list[i].mTimeToNextFix != time_to_next_fix)
            list[i].mTimeToFirstFix = time_to_next_fix;

        //if the currentModuleID is NULL, try updating the reg update with the available 
        //positioning method
        if( currentModuleID == TUid::Null() && (available == TRUE) && 
                                                    (aStatus != TPositionModuleStatus::EDeviceUnknown) &&          
                                                        (aStatus !=TPositionModuleStatus::EDeviceError))
            {
                TInt interval;
            
                interval = QGeoPositionInfoSource::updateInterval();
                
                TRAPD(ret , temp = CQMLBackendAO::NewL(this,REQ_REG_UPDATE,
                                               list[i].mUid));
                
                if((ret == KErrNone) && (temp != NULL))
                    {    
                temp->setUpdateInterval(interval);
                
                regUpdateAO = temp;
              
                //to be uncommented when startUpdates are done
                
                if(mStartUpdates)
                    regUpdateAO->startUpdates();
                     

                    currentModuleID = list[i].mUid;

                    currentMethod = list[i].mPosMethod;

                    QGeoPositionInfoSource::setPreferredPositioningMethods(currentMethod);
                    }
            }
        
        //check if the status of the currently used modules for regular update or 
        //request update has changed
        if( ((id == currentModuleID)||(id == reqmoduleID)) && 
                ((aStatus== TPositionModuleStatus::EDeviceUnknown) ||          
                        (aStatus==TPositionModuleStatus::EDeviceError) || 
                        (available == FALSE)) )
            {
            //if the change happened for regular update
            if( id == currentModuleID)
                {   
                TInt interval;
                
                TUint8 bits;
                
                interval = QGeoPositionInfoSource::updateInterval();
                
                delete  regUpdateAO;

                error = -1;
                
                bits = ModuleFlags;
                
                do
                    {
                    //error = Get the index of the positioning module based on 
                    //the preferred method if available , else get the default 
                    //position module
                    index = getIndexPositionModule(bits);

                    if(index >= 0)
                        {
                        TRAPD(ret,temp = CQMLBackendAO::NewL(this,REQ_REG_UPDATE,
                                list[index].mUid));
                        
                        if((ret == KErrNone) && (temp != NULL))
                            break;
                        
                        bits = bits & (0XFF ^ (1<<index));
                        }
                    }while( index >= 0);


                if(temp != NULL)
                    {
                    //successful in creating the subsession for the required 
                    //method
                    regUpdateAO = temp;
                    
                    regUpdateAO->setUpdateInterval(interval);
                    
                    //to be uncommented when startUpdates are done
                     
                     if(mStartUpdates)
                         regUpdateAO->startUpdates();
                     
                    
                    currentModuleID = list[index].mUid;
                    currentMethod = list[index].mPosMethod;
					QGeoPositionInfoSource::setPreferredPositioningMethods(currentMethod);
                    }
                else
                    {
                    //no methods available,clean up the resources
                    regUpdateAO = NULL;
                    currentModuleID = TUid::Null();
                    currentMethod = PositioningMethod(0);
					QGeoPositionInfoSource::setPreferredPositioningMethods(currentMethod);
                    }

                }

            //check if device status of the request update module changed
            if(id == reqmoduleID)
                {
                delete reqUpdateAO;
                reqUpdateAO = NULL;
                reqmoduleID = TUid::Null();
                emit requestTimeout;
                }

            }

        }   //end else

    //update the supported source types based on the device status
    update_available_types();       
    }


// Notification methods from active object. Notifies device status changes
void CQGeoPositionInfoSourceS60::UpdateDeviceStatus(void)
    {
    TPositionModuleStatus moduleStatus;
    TPositionModuleInfo moduleInfo;
    TInt error;
    
    //ListSize = 0 : called updateDeviceStatus() first time to initialise the array
    if(ListSize == 0)
        {
        TUint modCount;
        
        //count on the modules currently supported by the device
        mPositionServer.GetNumModules(modCount);
        
        //module ID of the default module
        error = mPositionServer.GetDefaultModuleId(currentModuleID);
      
        if(error != KErrNone)
            currentModuleID = TUid::Null();
        
        for(TInt i = 0;i < modCount ; i++)
            {
            //get module information
            mPositionServer.GetModuleInfoByIndex(i,moduleInfo);        
            
            //get module status 
            mPositionServer.GetModuleStatus(moduleStatus,moduleInfo.ModuleId());
            
            //update the properties of the module in the list array
            update_status(moduleInfo,moduleStatus.DeviceStatus());
            
            ModuleFlags |= (1 << i);
            }
        }
    else
        {
            //UpdateDeviceStatus() called afetr registering for NotifyModuleStatusEvent
            
            //get the module id from the status event
            TPositionModuleId id = mStatusEvent.ModuleId();
            
            //get module information
            mPositionServer.GetModuleInfoById(id,moduleInfo);  
            
            //get current status of the module
            mStatusEvent.GetModuleStatus(moduleStatus);
            
            //update the properties of the module in the list array
            update_status(moduleInfo,moduleStatus.DeviceStatus());
            
            
        }
    
    //register for next NotifyModuleStatusEvent
    devStatusUpdateAO->NotifyDeviceStatus(mStatusEvent);
    
    }

// 
void CQGeoPositionInfoSourceS60::TPositionInfo2QGeoPositionInfo(
                            HPositionGenericInfo *mPosInfo, QGeoPositionInfo& posInfo)
    {
    TPosition pos;
    QGeoCoordinate  coord;   
    double val;

    mPosInfo->GetPosition(pos);


    coord.setLatitude(pos.Latitude());
    coord.setLongitude(pos.Longitude());
    coord.setAltitude(pos.Altitude());

    //store the QGeoCoordinate values 
    posInfo.setCoordinate(coord);
    
    TDateTime datetime = pos.Time().DateTime();
    QDateTime dt(QDate(datetime.Year(),datetime.Month(),datetime.Day()),
            QTime(datetime.Hour(),datetime.Minute(),datetime.Second(),
                    datetime.MicroSecond()/1000));

    //store the time stamp
    posInfo.setDateTime(dt);
    
    //store the horizontal accuracy
    posInfo.setProperty(QGeoPositionInfo::HorizontalAccuracy,pos.HorizontalAccuracy());

    //store the vertical accuracy
    posInfo.setProperty(QGeoPositionInfo::VerticalAccuracy,pos.VerticalAccuracy());

    //check for the horizontal speed
    if(mPosInfo->IsFieldAvailable(EPositionFieldHorizontalSpeed))
        {
        mPosInfo->GetValue(EPositionFieldHorizontalSpeed,val);
        posInfo.setProperty(QGeoPositionInfo::GroundSpeed,val);
        }

    //check for the vertcal speed
    if(mPosInfo->IsFieldAvailable(EPositionFieldVerticalSpeed))
        {
        mPosInfo->GetValue(EPositionFieldVerticalSpeed,val);
        posInfo.setProperty(QGeoPositionInfo::VerticalSpeed,val);
        }

    //check for the magnetic variation
    if(mPosInfo->IsFieldAvailable(EPositionFieldMagneticCourseError))
        {
        mPosInfo->GetValue(EPositionFieldMagneticCourseError,val);
        posInfo.setProperty(QGeoPositionInfo::MagneticVariation,val);
        }

    //check for the heading
    if(mPosInfo->IsFieldAvailable(EPositionFieldHeading))
        {
        mPosInfo->GetValue(EPositionFieldHeading,val);
        posInfo.setProperty(QGeoPositionInfo::Heading,val);
        } 
    }
// 
void CQGeoPositionInfoSourceS60::UpdatePosition(HPositionGenericInfo *mPosInfo, int error)
    {
    QGeoPositionInfo  posInfo;
   
    if(error == KErrNone)
        {
        //fill posUpdate 
        TPositionInfo2QGeoPositionInfo(mPosInfo,posInfo);
        
        //emit posUpdate 
        emit positionUpdated(posInfo);
        }
    
    else if(error == KErrTimedOut)
        {
        //request has timed out 
        emit requestTimeout();
        }
    
    else 
        {
        //posiitoning module is unable to return any position information
        emit positionUpdated(posInfo);
        }
    }

// Returns the PositionServer handle
RPositionServer& CQGeoPositionInfoSourceS60:: getPositionServer()
    {
    return mPositionServer;
    }


// for request update
void CQGeoPositionInfoSourceS60::requestUpdate ( int timeout)
    {
    TInt index = -1;
    TUint8 bits;
    
    CQMLBackendAO *temp;

    //return if already a request update is pending
    if(reqUpdateAO && reqUpdateAO->IsRequestPending())
        return;
	
	
    bits = ModuleFlags;
    
    do  {

    //index of the more accurate method in the array
    index = getMoreAccurateMethod(timeout,bits);

    //no positioning method method available : emit requestTimeout
    if(index < 0)
        {
        emit requestTimeout();
        break;
	}
    //if the selected module for request update is same as the previous one reuse the request 
    if(list[index].mUid == reqmoduleID)
        {
            reqUpdateAO->requestUpdate(timeout);
            return;
        }  

    TRAPD(ret,temp = CQMLBackendAO::NewL(this,REQ_ONCE_UPDATE,list[index].mUid));

    if((ret == KErrNone) && (temp != NULL))
        {
        //delete the old reqest update
        delete reqUpdateAO;

        //set the requestAO to the newly created AO
        reqUpdateAO = temp;

		//set the request module ID 
		reqmoduleID = list[index].mUid;

        //start the update
        reqUpdateAO->requestUpdate(timeout);

        return;
        }
      
    bits = bits & (0XFF ^ (1<<index));

    }while( index >=0 );

    //cleanup resources if the invalid requpdate is still stored
    if(reqUpdateAO)
            {
            delete reqUpdateAO;
            reqmoduleID = TUid::Null();
            }

    }

// starts the regular updates
void CQGeoPositionInfoSourceS60::startUpdates ()
    {
    if (receivers(SIGNAL(positionUpdated(QGeoPositionInfo))) > 0 && !mStartUpdates)
		regUpdateAO->startUpdates();
		 mStartUpdates = true;
    }

// stops the regular updates
void CQGeoPositionInfoSourceS60::stopUpdates ()
    {
    mStartUpdates = false;
    regUpdateAO->CancelUpdate();
    }
TPositionModuleId CQGeoPositionInfoSourceS60::getPreferredPoistionModuleId(QGeoPositionInfoSource::PositioningMethods methods)
{
    // walk through the list for available method
    for( int  k=0; k< ListSize; k++)
    {
    if((list[k].mPosMethod & methods)  && (list[k].mIsAvailable))
    {
        
        return list[k].mUid;
    }
    
    }

    return TUid::Null();

}

//Added by PM
void CQGeoPositionInfoSourceS60::setPreferredPositioningMethods ( PositioningMethods methods )
    {
    PositioningMethods preferredMethod(PositioningMethods(0));
    TInt index = -1;
    // the poistioning methods are not supported 
    // if the preferred positioning method is the current poistioning
           
    if( !(supportedMethods & methods ) || (methods == AllPositioningMethods) ||
         ( methods == currentMethod ))
            
    		return;
    
    //TPositionModuleId moduleId = getPreferredPoistionModuleId(methods);    
    
    //if(moduleId == TUid::Null())
    //return;
    CQMLBackendAO *temp = NULL;
    TUint8 bits;
    bits = ModuleFlags;
    do
    {
    //index = Get the index of the positioning module based on 
    //the preferred method if available , else get the default 
    //position module
    index = getIndexPositionModule(bits, methods);

    if(index == -1)
        return;
    
    if(index >= 0)
        {
        
        TRAPD(error, temp = CQMLBackendAO::NewL(this,REQ_REG_UPDATE,
                list[index].mUid));
        if(temp != NULL )
            break;
        
        bits = bits & (0XFF ^ (1<<index));
        }
    }while( index >= 0);
     
    //CQMLBackendAO *temp = CQMLBackendAO::NewL(this, REQ_REG_UPDATE, moduleId );
    if(temp != NULL)
        {
        // this is needed as the previous setupdateinterval value should not be lost
        int updateInterval = QGeoPositionInfoSource::updateInterval();
        if(regUpdateAO)
            delete regUpdateAO;
        regUpdateAO = temp;
        currentModuleID = list[index].mUid ;
        
        index = checkmodule(currentModuleID);
        
        currentMethod = list[index].mPosMethod ;
        
        
        if(updateInterval)
            {
                int value = regUpdateAO->setUpdateInterval(updateInterval);
                //as the poistioning module has changed
                //possibility of the minimumupdateinterval being changed
                 if(value != updateInterval)
                    QGeoPositionInfoSource::setUpdateInterval(value);
                
            }
			QGeoPositionInfoSource::setPreferredPositioningMethods(currentMethod);        
        }
    
    
    
                            
    // irrespective of whatever be the preferred methods just set 
    // the method that is set
    //QGeoPositionInfoSource::setPreferredPositioningMethods(currentMethod);
    
                
                            
    }


// 
//Added by PM
void CQGeoPositionInfoSourceS60::setUpdateInterval ( int msec )
    {
        if(regUpdateAO)
            {
            int interval = regUpdateAO->setUpdateInterval(msec);
            // as the above set value can be minimum value so
	        // assigning to the base class data member
            QGeoPositionInfoSource::setUpdateInterval(interval);
            }
    }

void CQGeoPositionInfoSourceS60::connectNotify(const char *signal)
{
	// start update if it already connected
	if (mStartUpdates && regUpdateAO && QLatin1String(signal) == SIGNAL(positionUpdated(QGeoPositionInfo)))
		regUpdateAO->startUpdates();
	
}

void CQGeoPositionInfoSourceS60::disconnectNotify(const char *signal)
{
	// Cancel updates if slot is disconnected for the positionUpdate() signal.
	
	if ( (regUpdateAO)&&(QLatin1String(signal) == SIGNAL(positionUpdated(QGeoPositionInfo))) && receivers(SIGNAL(positionUpdated(QGeoPositionInfo))) == 0  )
			regUpdateAO->CancelUpdate();

}

