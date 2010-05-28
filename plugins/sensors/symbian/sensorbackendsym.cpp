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
// Internal Includes
#include "sensorbackendsym.h"

#include <sensrvgeneralproperties.h>

// Constants
const TInt KDesiredReadingCount = 1;
const TInt KMaximumReadingCount = 1;
const TInt KDefaultBufferingPeriod = 0;
const TInt KAccuracyInvalid = -1;

///// Internal Functions

/*
 * FindSensorL is used to find if a specific sensor is available on the
 * device, if FindSensorL leaves then the sensor is not supported
 */
void CSensorBackendSym::FindSensorL()
    {
    // This method scans the device for the availability
    // of specified sensor using Sensor APIs
    CSensrvChannelFinder* finder = CSensrvChannelFinder::NewLC();
    RSensrvChannelInfoList channelList;
    TSensrvChannelInfo channelInfo;
    // Retrieve the list of channels available    
    finder->FindChannelsL( channelList, channelInfo );
    CleanupStack::PopAndDestroy(finder);
    // Iterate the channel list and look for required sensor
    for (TInt i = 0; i<channelList.Count(); i++)
        {
        // Iterate the available channels
        if (channelList[i].iChannelType == iBackendData.iSensorType)
            {
            // Required sensor found
            iBackendData.iChannelInfo = channelList[i];
            channelList.Close();
            return;
            }
        }
    // Required sensor not found, leave with error
    channelList.Close();
    iBackendData.iSensorAvailable = EFalse;
    User::Leave(KErrNotFound);
    }

/*
 * OpenSensorChannelL is used to open the channel for sensor as indicated by
 * iPrivateData.iChannelInfo
 */
void CSensorBackendSym::OpenSensorChannelL()
    {
    // Search if sensor is available on the phone
    FindSensorL();
    // Required sensor found
    if( !(iBackendData.iSensorChannel) )
        {
        // Create and Open the Sensor channel
        iBackendData.iSensorChannel = CSensrvChannel::NewL( iBackendData.iChannelInfo );      
        // Open the channel for  sensor
        iBackendData.iSensorChannel->OpenChannelL();        
        iBackendData.iReadingLock.CreateLocal();
        iBackendData.iSensorAvailable = ETrue;
        }
    else
        {
        User::Leave(KErrAlreadyExists);
        }
    }

/*
 * CloseSensorChannel is used to close the sensor channel and release the
 * resources
 */
void CSensorBackendSym::CloseSensorChannelL()
    {
    // If the sensor channel is not created nothing to delete
    if( iBackendData.iSensorChannel )
        {
        // Stop listening before closing the channel
        iBackendData.iSensorAvailable = EFalse;
        StopListeningL();
        // Close the channel
        User::LeaveIfError(iBackendData.iSensorChannel->CloseChannel());
        iBackendData.iReadingLock.Close();
        // If close is successful, destroy iSensorChannel
        delete iBackendData.iSensorChannel;
        iBackendData.iSensorChannel = NULL;
        }
    User::Leave( KErrNotFound );
    }

TInt CSensorBackendSym::SetProperty(TSensrvPropertyId aPropertyId, TSensrvPropertyType aPropertyType, TSensrvArrayIndex aArrayIndex, TReal aValue)
    {
    //Creating property object
    TSensrvProperty prop;   
    //Set property Id
    prop.SetPropertyId(aPropertyId);
    //Set Index of property
    prop.SetItemIndex(-1);
    //Set value depending on type of property
    if(aPropertyType == ESensrvRealProperty)
        {
        prop.SetValue(aValue);
        }
    else if(aPropertyType == ESensrvIntProperty)
        {
        prop.SetValue((TInt)aValue);
        }
    //Set array Index
    prop.SetArrayIndex(aArrayIndex);
    //Setting the property
    return iBackendData.iSensorChannel->SetProperty(prop);
    }

TInt CSensorBackendSym::SetMeasurementRange()
    {
    //Setting measurement range
    //Check if more than one output ranges are available
    if(sensor()->outputRanges().length() <= 1)
        {
        return KErrNone;
        }
    TSensrvProperty propertyType;
    //Getting the property to check the type
    TRAPD(err, iBackendData.iSensorChannel->GetPropertyL(KSensrvPropIdMeasureRange, ESensrvSingleProperty, propertyType));
    if(err != KErrNone)
        {
        return err;
        }
    //Find the type of property
    TSensrvPropertyType type = propertyType.PropertyType();
    //If type is integer
    if(type == ESensrvRealProperty)
        {    
        return SetProperty(KSensrvPropIdMeasureRange, ESensrvRealProperty, ESensrvArrayPropertyInfo, sensor()->outputRange());
        }
    //If type is real
    if(type == ESensrvIntProperty)
        {
        return SetProperty(KSensrvPropIdMeasureRange, ESensrvIntProperty, ESensrvArrayPropertyInfo, sensor()->outputRange());
        }       
    return KErrNone;
    }

TInt CSensorBackendSym::SetDataRate()
    {
    //Get available datarates
    qrangelist availableDataRates = sensor()->availableDataRates();
    if ( availableDataRates.count() != 0 )
        {
        //Check if discret values or range value is used
        if( availableDataRates[0].first == availableDataRates[0].second )
            {
            //In descrete ranges if only one available, no need to set that range
            if(availableDataRates.length() <= 1)
                {
                return KErrNone;
                }
            return SetProperty(KSensrvPropIdDataRate, ESensrvIntProperty,ESensrvArrayPropertyInfo,
                    availableDataRates.indexOf(qrange(sensor()->dataRate(),sensor()->dataRate())));
            }
        else
            {
            // Uses range value
            return SetProperty(KSensrvPropIdDataRate, ESensrvIntProperty, ESensrvSingleProperty, sensor()->dataRate());
            }
        }
    // No data rates available
    return KErrNone;
    }

void CSensorBackendSym::SetProperties()
    {
    if(sensor()->outputRange() != -1)
        {
        //Set measurement range
        TInt err = SetMeasurementRange();
        if(err != KErrNone)
            {
            sensorError(err);
            }
        }
    if(sensor()->dataRate() != -1)
        {
        //Set data rate
        TInt err = SetDataRate();
        if(err != KErrNone)
            {
            sensorError(err);
            }
        }
    }

/*
 * Used to start listening to the sensor
 */
void CSensorBackendSym::StartListeningL()
    {
    // Check if data listening is enabled
    if(iBackendData.iDataListening)
        {
        SetProperties();
        // Start listening to the sensor 
        // Before calling this api the channel should be found and opened
        iBackendData.iSensorChannel->StartDataListeningL( this,
                KDesiredReadingCount,
                KMaximumReadingCount,
                KDefaultBufferingPeriod );
        }
    // start property listening if required         //put it above
    if ( iBackendData.iPropertyListening )
        {
        iBackendData.iSensorChannel->SetPropertyListenerL(this);
        }
    }

/*
 * Used to stop listening to the sensor
 */
void CSensorBackendSym::StopListeningL()
    {
    if(iBackendData.iPropertyListening)
        {
        //Stop Property listening
        iBackendData.iSensorChannel->SetPropertyListenerL(NULL);
        }
    if(iBackendData.iDataListening)
        {
        // Stop listening to the sensor channel
        User::LeaveIfError(iBackendData.iSensorChannel->StopDataListening());
        }
    }

/*
 * Default C++ constructor
 */
CSensorBackendSym::CSensorBackendSym(QSensor *sensor):QSensorBackend(sensor)
    {
    // By default enabling Data listening     
    iBackendData.iDataListening = ETrue;
    // By default disabling Property listening
    iBackendData.iPropertyListening = EFalse;
    }

/*
 * Backend Destructor
 */
CSensorBackendSym::~CSensorBackendSym()
    {
    // No Implementation
    }

void CSensorBackendSym::GetDescription()
    {
    RSensrvPropertyList list;
    TRAPD(err, iBackendData.iSensorChannel->GetAllPropertiesL(KSensrvSensorDescription, list));
    if(err == KErrNone)
        {
        QString str;
        TBuf8<KSensrvPropertyTextBufferSize> desc;
        for(int i=0; i<list.Count(); i++)
            {            
            if(list[i].GetArrayIndex() == ESensrvArrayPropertyInfo)
                {                
                continue;
                }
            list[i].GetValue(desc);
            str.append((const char*)desc.PtrZ());
            }
        setDescription(str);
        }
    }

void CSensorBackendSym::GetDataRate()
    {
    RSensrvPropertyList list;
    TRAPD(err, iBackendData.iSensorChannel->GetAllPropertiesL(KSensrvPropIdDataRate, list));
    if(err == KErrNone)
        {    
        //if list has only one item then it is range of values and not descrete values, agreed with DS team
        if(list.Count() == 1)               
            {
            TInt min, max, value;
            list[0].GetMinValue(min);
            list[0].GetMaxValue(max);
            //Set datarate as range
            addDataRate(min, max);
            list[0].GetValue(value);
            }
        //if list has more than one item, data rate will be having descrete values, agreed with DS team
        else                                
            {
            TInt datarate, index;
            for(int i=0; i<list.Count(); i++)
                {  
                if(list[i].GetArrayIndex() == ESensrvArrayPropertyInfo)
                    {
                    //If array index is ESensrvArrayPropertyInfo, getting the value to get current datarate
                    list[i].GetValue(index);
                    list[index].GetValue(datarate);
                    continue;
                    }     
                list[i].GetValue(datarate);
                addDataRate(datarate, datarate);
                }
            }
        }
    }

void CSensorBackendSym::GetMeasurementrangeAndAccuracy()
    {
    /*
    In QT Mobility measurement range and accuracy are coupled together to form the output range
    where as, in Symbian accuracy and measurement range are independent properties.
    To solve the QT requirement, the mapping used is as follows
    1. If symbian provides only one accuracy, use this with all the measurement ranges
    2. If there are n accuracies and n measurement ranges, map linearly (n:n)
    3. If there are n accuracies and n+x measurement ranges, then the mapping will be 
         n:n for each n measurement ranges and accuracies
         KAccuracyInvalid : for each n+x measurement ranges
    */
    TReal accuracy = 0;
    RSensrvPropertyList accuracyList;
    RSensrvPropertyList list;
    TInt err;
    TRAP(err, iBackendData.iSensorChannel->GetAllPropertiesL(KSensrvPropIdChannelAccuracy, accuracyList));
    if(err == KErrNone)
        {            
        if(accuracyList.Count() == 1)
            {
            accuracyList[0].GetValue(accuracy);          
            }
        else
            {
            accuracy = KAccuracyInvalid; 
            }
        }
        
    //measurement minimum & maximum
    list.Reset();
    TRAP(err, iBackendData.iSensorChannel->GetAllPropertiesL(KSensrvPropIdMeasureRange, list));
    if(err == KErrNone)
        {            
        for(int i=0; i<list.Count(); i++)
            {
            if(list[i].GetArrayIndex() == ESensrvArrayPropertyInfo)
                {
                continue;
                }
            if(list[i].PropertyType() == ESensrvIntProperty )
                {
                TInt min, max;
                list[i].GetMinValue(min);
                list[i].GetMaxValue(max);
                if(accuracy != KAccuracyInvalid)
                    {
                    addOutputRange(min, max, accuracy);
                    }
                else
                    {
                    if(accuracyList.Count() > i)
                        {
                        accuracyList[i].GetValue(accuracy);
                        addOutputRange(min, max, accuracy);
                        }
                    else
                        {
                        addOutputRange(min, max, KAccuracyInvalid);
                        }
                    }
                }
            else if(list[i].PropertyType() == ESensrvRealProperty  )
                {
                TReal min, max;
                list[i].GetMinValue(min);
                list[i].GetMaxValue(max);
                if(accuracy != KAccuracyInvalid)
                    {
                    addOutputRange(min, max, accuracy);
                    }
                else
                    {
                    if(accuracyList.Count() > i)
                        {
                        accuracyList[i].GetValue(accuracy);
                        addOutputRange(min, max, accuracy);
                        }
                    else
                        {
                        addOutputRange(min, max, KAccuracyInvalid);
                        }
                    }
                }
            }
        }
    }

void CSensorBackendSym::GetPropertiesL()
    {
    //description
    GetDescription();
    
    //data rate
    GetDataRate();
    
    //accuracy and measurement ranges
    GetMeasurementrangeAndAccuracy();
    }



/*
 * InitializeL is used to create and init the sensor server objects
 */ 
void CSensorBackendSym::InitializeL()
    {
    // Initialize Symbian Sensor Framework Objects
    OpenSensorChannelL();
    if(sensor())
        {
        GetPropertiesL();        
        }
    }

/*
 * Close is used to release all the sensor server objects
 * May change when error handling is supported by mobility apis
 */
TInt CSensorBackendSym::Close()
    {
    // Close Symbian Sensor Framework objects
    TRAPD(err,CloseSensorChannelL());
    return err;
    }


///// Derived From QSensorBackend

/**
 *  start is used to start listening to the sensor
 */
void CSensorBackendSym::start()
    {
    // Start listening to sensor, after this call DataRecieved will be called
    // when data is available
    TRAPD(err,StartListeningL())
    if( err != KErrNone )
        {
        sensorStopped();
        sensorError(err);        
        }
    }

/*
 * stop is used to stop listening to the sensor
 */
void CSensorBackendSym::stop()
    {
    // Stop listening to sensor, after this call DataRecieved wont be called
    TRAPD(err,StopListeningL())
    if ( err != KErrNone )
        {
        sensorError(err);
        }
    }

//Derived From MSensrvDataListener

/*
 * DataReceived is called by the Sensor Server when ever data is available in the
 * sensor buffer
 */
void CSensorBackendSym::DataReceived(CSensrvChannel &aChannel, TInt /*aCount*/, TInt /*aDataLost*/)
    {
    // Retrieve the data from sensor buffer
    RecvData(aChannel);
    // Notify that a reading is available
    newReadingAvailable();
    }

/**
 * DataError is called to indicate an error, fatal errors are inrecoverable
 */
void CSensorBackendSym::DataError(CSensrvChannel& /*aChannel*/, TSensrvErrorSeverity aError)
    {
    // If error is fatal stop the sensor
    if( aError == ESensrvErrorSeverityFatal )
        {
        sensorStopped();
        }
    sensorError(KErrGeneral);
    }

/*
 * GetDataListenerInterfaceL is used to get a pointer to the sensor server backend
 * It is not required for QT Mobility Sensors API
 */
void CSensorBackendSym::CSensorBackendSym::GetDataListenerInterfaceL (TUid /*aInterfaceUid*/, TAny*&/*aInterface*/)
    {
    // No implementation required
    }

// From MSensrvProeprtyListener        

/**
 * Notification about the changed value of a property.
 */
void  CSensorBackendSym::PropertyChanged(CSensrvChannel &aChannel, const TSensrvProperty &aChangedProperty)
    {
    TRAP_IGNORE(HandlePropertyChange(aChannel,aChangedProperty))
    }
   
/**
 *  Property listening failed.
 */
void  CSensorBackendSym::PropertyError(CSensrvChannel &/*aChannel*/, TSensrvErrorSeverity aError)
    {
    if( aError == ESensrvErrorSeverityFatal )
        {
        sensorStopped();
        }
    sensorError(KErrGeneral);
    }
  
/**
 * Set a listener for the indication, if the setting of the property succeeded.
 */ 
void  CSensorBackendSym::SetPropertySuccessIndicationChanged(TSetPropertySuccessIndicator /*aIndication*/)
    {
    // No implementation required
    }
   
/*
 * Returns a pointer to a specified interface extension - to allow future extension of this class without breaking binary compatibility.
 */
void  CSensorBackendSym::GetPropertyListenerInterfaceL (TUid /*aInterfaceUid*/, TAny *&/*aInterface*/)
    {
    // No implementation required
    }

void CSensorBackendSym::SetListening(TBool aDataListening, TBool aPropertyListening)
    {
    iBackendData.iDataListening = aDataListening;
    iBackendData.iPropertyListening = aPropertyListening;
    }

/**
 * Deriving class implements this if it requires property change notification
 */
void CSensorBackendSym::HandlePropertyChange(CSensrvChannel &/*aChannel*/, const TSensrvProperty &/*aChangedProperty*/)
    {
    // No implementation required in this class
    }

