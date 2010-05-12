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
#include "proximitysensorsym.h"

/**
 * set the id of the proximity sensor
 */
const char *CProximitySensorSym::id("sym.proximity");

/**
 * Factory function, this is used to create the proximity sensor object
 * @return CProximitySensorSym if successful, leaves on failure
 */
CProximitySensorSym* CProximitySensorSym::NewL(QSensor *sensor)
    {
    CProximitySensorSym* self = new (ELeave) CProximitySensorSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

/**
 * Destructor
 * Closes the backend resources
 */
CProximitySensorSym::~CProximitySensorSym()
    {
    // Release the backend resources
    Close();
    }

/**
 * Default constructor
 */
CProximitySensorSym::CProximitySensorSym(QSensor *sensor):CSensorBackendSym(sensor)
        {
        setReading<QProximityReading>(&iReading);    
        iBackendData.iSensorType = KSensrvChannelTypeIdProximityMonitor;
        }

/*
 * RecvData is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle proximity sensor specific
 * reading data and provides conversion and utility code
 */ 
void CProximitySensorSym::RecvData(CSensrvChannel &aChannel)
    {
    TPckg<TSensrvProximityData> proxpkg( iData );
    TInt ret = aChannel.GetData( proxpkg );
    if(KErrNone != ret)
        {
        // If there is no reading available, return without setting
        return;
        }
    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();    
    iReading.setClose(iData.iProximityState == TSensrvProximityData::EProximityDiscernible);
    // Set the timestamp
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Release the lock
    iBackendData.iReadingLock.Signal();
    }

/**
 * Second phase constructor
 * Initialize the backend resources
 */
void CProximitySensorSym::ConstructL()
    {
    //Initialize the backend resources
    InitializeL();
    }

