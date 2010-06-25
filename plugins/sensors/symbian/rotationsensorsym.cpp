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

// Internal Headers
#include "rotationsensorsym.h"

#include <sensrvgeneralproperties.h>

/**
 * set the id of the proximity sensor
 */
char const * const CRotationSensorSym::id("sym.rotation");

const TInt KMinimumRange = -180;
const TInt KMaximumRange = 180;

/**
 * Factory function, this is used to create the rotation sensor object
 * @return CRotationSensorSym if successful, leaves on failure
 */
CRotationSensorSym* CRotationSensorSym::NewL(QSensor *sensor)
    {
    CRotationSensorSym* self = new (ELeave) CRotationSensorSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/**
 * Destructor
 * Closes the backend resources
 */
CRotationSensorSym::~CRotationSensorSym()
    {
    // Release the backend resources
    Close();
    }

/**
 * Default constructor
 */
CRotationSensorSym::CRotationSensorSym(QSensor *sensor):CSensorBackendSym(sensor)
        {
        setReading<QRotationReading>(&iReading);    
        iBackendData.iSensorType = KSensrvChannelTypeIdRotationData;
        }

/*
 * RecvData is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle rotation sensor specific
 * reading data and provides conversion and utility code
 */ 
void CRotationSensorSym::RecvData(CSensrvChannel &aChannel)
    {
    TPckg<TSensrvRotationData> rotationpkg( iData );
    TInt ret = aChannel.GetData( rotationpkg );
    if(KErrNone != ret)
        {
        // If there is no reading available, return without setting
        return;
        }
    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();
    // To Do verify with ds and ramsay
    
    // For x axis symbian provides reading from 0 to 359 range
    // This logic maps value to Qt range -90 to 90
    if(iData.iDeviceRotationAboutXAxis >= 0 && iData.iDeviceRotationAboutXAxis <= 180)
        {
        iReading.setX(90 - iData.iDeviceRotationAboutXAxis);
        }
    else if(iData.iDeviceRotationAboutXAxis > 180 && iData.iDeviceRotationAboutXAxis <= 270)
        {
        iReading.setX(iData.iDeviceRotationAboutXAxis - 270);
        }
    else if(iData.iDeviceRotationAboutXAxis > 270 && iData.iDeviceRotationAboutXAxis < 360)
        {
        iReading.setX(iData.iDeviceRotationAboutXAxis - 270);
        }
    
    // For y axis symbian provides reading from 0 to 359 range
    // This logic maps value to Qt range -180 to 180
    if(iData.iDeviceRotationAboutYAxis >= 0 && iData.iDeviceRotationAboutYAxis <= 180)
        {
        iReading.setY(iData.iDeviceRotationAboutYAxis);
        }
    else if(iData.iDeviceRotationAboutYAxis > 180 && iData.iDeviceRotationAboutYAxis < 360)
        {
        iReading.setY(iData.iDeviceRotationAboutYAxis - 360);
        }
    
    if(iData.iDeviceRotationAboutZAxis == TSensrvRotationData::KSensrvRotationUndefined)
        {
        sensor()->setProperty("hasZ", QVariant(FALSE));
        }
    else
        {
        sensor()->setProperty("hasZ", QVariant(TRUE));
        // For z axis symbian provides reading from 0 to 359 range
        // This logic maps value to Qt range -180 to 180
        if(iData.iDeviceRotationAboutZAxis >= 0 && iData.iDeviceRotationAboutZAxis <= 180)
            {
            iReading.setZ(iData.iDeviceRotationAboutZAxis);
            }
        else if(iData.iDeviceRotationAboutZAxis > 180 && iData.iDeviceRotationAboutZAxis < 360)
            {
            iReading.setZ(iData.iDeviceRotationAboutZAxis - 360);
            }
        }
    // Set the timestamp
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Release the lock
    iBackendData.iReadingLock.Signal();
    }

/**
 * Overriding this method in rotation sensor to hard code value of 
 * mesurement range from -180 to 180 as Qt wants
 * Symbian provides measurement range from 0 to 359
 */
void CRotationSensorSym::GetMeasurementrangeAndAccuracy()
    {
    TReal accuracy = 0;
    TSensrvProperty accuracyProperty;
    TRAPD(err, iBackendData.iSensorChannel->GetPropertyL(KSensrvPropIdChannelAccuracy, ESensrvSingleProperty, accuracyProperty));
    if(err == KErrNone)
        {
        accuracyProperty.GetValue(accuracy);
        }
    // Hard coding values -180 and 180 as Qt expects y and z axis
    // values range from -180 to 180.
    addOutputRange(KMinimumRange, KMaximumRange, accuracy);
    }


/**
 * Second phase constructor
 * Initialize the backend resources
 */
void CRotationSensorSym::ConstructL()
    {
    //Initialize the backend resources
    InitializeL();    
    }

