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

#include "compasssym.h"

/**
 * set the id of the accelerometer sensor
 */
char const * const CCompassSym::id("sym.compass");

/**
 * Factory function, this is used to create the compass object
 * @return CCompassSym if successful, leaves on failure
 */
CCompassSym* CCompassSym::NewL(QSensor *sensor)
    {
    CCompassSym* self = new (ELeave) CCompassSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

/**
 * Destructor
 * Closes the backend resources
 */
CCompassSym::~CCompassSym()
    {
    delete iMagnetometer;
    Close();
    }

/**
 * Default constructor
 */
CCompassSym::CCompassSym(QSensor *sensor):CSensorBackendSym(sensor)
    {
    setReading<QCompassReading>(&iReading);
    iBackendData.iSensorType = KSensrvChannelTypeIdMagneticNorthData;
    }

/**
 * start() overrides CSensorBackendSym::start()
 * This is to allow starting magnetometer before stopping the compass 
 */
void CCompassSym::start()
    {
    // start the magnetometer
    iMagnetometer->start();
    // start the compass backend
    CSensorBackendSym::start();
    }

/**
 * stop() overrides CSensorBackendSym::stop()
 * This is to allow stopping magnetometer before stopping the compass 
 */
void CCompassSym::stop()
    {
    // stop the magnetometer
    iMagnetometer->stop();
    // start the compass backend
    CSensorBackendSym::stop();
    }

/*
 * RecvData is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle compass sensor specific
 * reading data and provides conversion and utility code
 */  
void CCompassSym::RecvData(CSensrvChannel &aChannel)
    {
    TPckg<TSensrvMagneticNorthData> proxpkg( iData );
    TInt ret = aChannel.GetData( proxpkg );
    if(KErrNone != ret)
        {
        // If there is no reading available, return without setting
        return;
        }
    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();    
    iReading.setAzimuth(iData.iAngleFromMagneticNorth);
    // Retrieve and set the calibration level
    iReading.setCalibrationLevel(iMagnetometer->GetCalibrationLevel());
    // Set the timestamp
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Release the lock
    iBackendData.iReadingLock.Signal();
    }

/**
 * Second phase constructor
 * Initialize the backend resources
 */
void CCompassSym::ConstructL()
    {
    // Initialize the backend
    InitializeL();
    // Create magnetometer, this is required as in sensor server,
    // calibration data is available only for magnetometer
    iMagnetometer = CMagnetometerSensorSym::NewL(NULL);
    // Listen only for property change on magnetometer as we are
    // interested only in calibration property
    iMagnetometer->SetListening(EFalse, ETrue);
    }

