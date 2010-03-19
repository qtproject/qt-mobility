/****************************************************************************
 **
 ** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "orientationsym.h"

/**
 * set the id of the orientation sensor
 */
const char *COrientationSensorSym::id("sym.orientation");

/**
 * Factory function, this is used to create the orientation sensor object
 * @return COrientationSensorSym if successful, leaves on failure
 */
COrientationSensorSym* COrientationSensorSym::NewL(QSensor *sensor)
    {
    COrientationSensorSym* self = new (ELeave) COrientationSensorSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

/**
 * Destructor
 * Closes the backend resources
 */
COrientationSensorSym::~COrientationSensorSym()
    {
    // Release the backend resources
    Close();
    }

/**
 * Default constructor
 */
COrientationSensorSym::COrientationSensorSym(QSensor *sensor):CSensorBackendSym(sensor)
    {
    setReading<QOrientationReading>(&iReading);    
    iBackendData.iSensorType = KSensrvChannelTypeIdOrientationData;
    }

/*
 * RecvData is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle orientation sensor specific
 * reading data and provides conversion and utility code
 */ 
void COrientationSensorSym::RecvData(CSensrvChannel &aChannel)
    {
    TPckg<TSensrvOrientationData> orientationpkg( iData );
    TInt ret = aChannel.GetData( orientationpkg );
    if(KErrNone != ret)
        {
        // If there is no reading available, return without setting
        return;
        }
    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();    
    //Mapping device orientation enum values to Qt Orientation enum values
    switch (iData.iDeviceOrientation)
        {
        // Indicates Display facing down 
        case TSensrvOrientationData::EOrientationDisplayDown:
            {            
            iReading.setOrientation(QOrientationReading::TopDown);                
            }
            break;

        // Indicates Display facing up
        case TSensrvOrientationData::EOrientationDisplayUp:
            {
            iReading.setOrientation(QOrientationReading::TopUp);
            }
            break;

        // Indicates Display left up
        case TSensrvOrientationData::EOrientationDisplayLeftUp:
            {
            iReading.setOrientation(QOrientationReading::LeftUp);
            }
            break;

        // Indicates Display right up
        case TSensrvOrientationData::EOrientationDisplayRightUp:
            {
            iReading.setOrientation(QOrientationReading::RightUp);
            }
            break;

        // Indicates Display upwards(Top up)
        case TSensrvOrientationData::EOrientationDisplayUpwards:
            {
            iReading.setOrientation(QOrientationReading::FaceUp);
            }
            break;

        // Indicates Display downwards(Top down)
        case TSensrvOrientationData::EOrientationDisplayDownwards:
            {
            iReading.setOrientation(QOrientationReading::FaceDown);
            }
            break;

        // Undefined value
        default:
            {
            iReading.setOrientation(QOrientationReading::Undefined);
            }
        }
    
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Release the lock
    iBackendData.iReadingLock.Signal();
    }

/**
 * Second phase constructor
 * Initialize the backend resources
 */
void COrientationSensorSym::ConstructL()
    {
    InitializeL();
    }

