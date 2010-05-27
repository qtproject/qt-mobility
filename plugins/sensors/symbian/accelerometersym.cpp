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
#include "accelerometersym.h"
#include <sensrvgeneralproperties.h>

#define GRAVITATION_CONSTANT 9.812865328        //According to wikipedia link http://en.wikipedia.org/wiki/Standard_gravity

/**
 * set the id of the accelerometer sensor
 */
const char *CAccelerometerSensorSym::id("sym.accelerometer");

/**
 * Factory function, this is used to create the accelerometer object
 * @return CAccelerometerSensorSym if successful, leaves on failure
 */
CAccelerometerSensorSym* CAccelerometerSensorSym::NewL(QSensor *sensor)
    {
    CAccelerometerSensorSym* self = new (ELeave) CAccelerometerSensorSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

/**
 * Destructor
 * Closes the backend resources
 */
CAccelerometerSensorSym::~CAccelerometerSensorSym()
    {
    // Release the backend resources
    Close();
    }

/**
 * Default constructor
 */
CAccelerometerSensorSym::CAccelerometerSensorSym(QSensor *sensor):CSensorBackendSym(sensor),
        iScale(0),
        iUnit(0)
        {
        setReading<QAccelerometerReading>(&iReading);
        iBackendData.iSensorType = KSensrvChannelTypeIdAccelerometerXYZAxisData;    
        //Disable property listening
        SetListening(ETrue, EFalse);
        }

/*
 * RecvData is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle accelerometer sensor specific
 * reading data and provides conversion and utility code
 */
void CAccelerometerSensorSym::RecvData(CSensrvChannel &aChannel)
    {
    TPckg<TSensrvAccelerometerAxisData> accelerometerpkg( iData );
    TInt ret = aChannel.GetData( accelerometerpkg );
    if(KErrNone != ret)
        {
        // If there is no reading available, return without setting
        return;
        }
    TReal x = iData.iAxisX;
    TReal y = iData.iAxisY;
    TReal z = iData.iAxisZ;
    //Converting unit to m/s^2
    qoutputrangelist rangeList = sensor()->outputRanges();
    TReal maxValue = rangeList[sensor()->outputRange()].maximum;
    if(iScale && iUnit == ESensevChannelUnitAcceleration)
        {
        x = (x/iScale) * maxValue;
        y = (y/iScale) * maxValue;
        z = (z/iScale) * maxValue;
        }
    else if(iUnit == ESensrvChannelUnitGravityConstant)
        {
        //conversion is yet to done
        }
    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();
    // Set qt mobility accelerometer reading with data from sensor server
    iReading.setX(x);
    iReading.setY(y);
    iReading.setZ(z);
    // Set the timestamp
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Release the lock
    iBackendData.iReadingLock.Signal();
    }

/**
 * Second phase constructor
 * Initialize the backend resources
 */
void CAccelerometerSensorSym::ConstructL()
    {
    //Initialize the backend resources
    InitializeL(); 
    
    TSensrvProperty unitProperty;
    TRAPD(err, iBackendData.iSensorChannel->GetPropertyL(KSensrvPropIdChannelUnit, ESensrvSingleProperty, unitProperty));
    if(err == KErrNone)
        {
        unitProperty.GetValue(iUnit);
        }
    
    TSensrvProperty dataFormatProperty;
    iBackendData.iSensorChannel->GetPropertyL(KSensrvPropIdChannelDataFormat, ESensrvSingleProperty, dataFormatProperty);
    TInt dataFormat;
    dataFormatProperty.GetValue(dataFormat);
    if(dataFormat == ESensrvChannelDataFormatScaled)
        {
        TSensrvProperty scaleRangeProperty;
        TRAPD(err, iBackendData.iSensorChannel->GetPropertyL(KSensrvPropIdScaledRange, ESensrvSingleProperty, scaleRangeProperty));         //Slight confusion
        scaleRangeProperty.GetMaxValue(iScale);
        }
    }

