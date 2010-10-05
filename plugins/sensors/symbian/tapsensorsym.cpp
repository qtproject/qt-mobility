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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "tapsensorsym.h"
/**
 * set the id of the Tap sensor
 */
char const * const CTapSensorSym::id("sym.tap");

/**
 * Factory function, this is used to create the tap sensor object
 * @return CTapSensorSym if successful, leaves on failure
 */
CTapSensorSym* CTapSensorSym::NewL(QSensor *sensor)
    {
    CTapSensorSym* self = new (ELeave) CTapSensorSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

/**
 * Destructor
 * Closes the backend resources
 */
CTapSensorSym::~CTapSensorSym()
    {
    Close();
    }

/**
 * Default constructor
 */
CTapSensorSym::CTapSensorSym(QSensor *sensor):CSensorBackendSym(sensor)
    {
    setReading<QTapReading>(&iReading);    
    iBackendData.iSensorType = KSensrvChannelTypeIdAccelerometerDoubleTappingData;
    }
/*
 * RecvData is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle tap sensor specific
 * reading data and provides conversion and utility code
 */ 
void CTapSensorSym::RecvData(CSensrvChannel &aChannel)
    {
    TPckg<TSensrvTappingData> tappkg( iData );
    TInt ret = aChannel.GetData( tappkg );
    if(KErrNone != ret)
        {
		// If there is no reading available, return without setting
        return;
        }
	// Get a lock on the reading data
    iBackendData.iReadingLock.Wait();
	//Mapping device tap sensor enum values to Qt tap sensor enum values
    switch (iData.iDirection)
        {
			// Indicates a tap on positive X axis
        case KSensrvAccelerometerDirectionXplus:
            {
            iReading.setTapDirection(QTapReading::X_Pos);
            }
            break;
			// Indicates a tap on negative X axis
        case KSensrvAccelerometerDirectionXminus:
            {
            iReading.setTapDirection(QTapReading::X_Neg);
            }
            break;
			// Indicates a tap on positive Y axis
        case KSensrvAccelerometerDirectionYplus:
            {
            iReading.setTapDirection(QTapReading::Y_Pos);
            }
            break;
			// Indicates a tap on negative Y axis
        case KSensrvAccelerometerDirectionYminus:
            {
            iReading.setTapDirection(QTapReading::Y_Neg);
            }
            break;    
			// Indicates a tap on positive Z axis
        case KSensrvAccelerometerDirectionZplus:
            {
            iReading.setTapDirection(QTapReading::Z_Pos);
            }
            break;
			// Indicates a tap on negative Z axis
        case KSensrvAccelerometerDirectionZminus:
            {
            iReading.setTapDirection(QTapReading::Z_Neg);
            }
            break;    
        default:
            {
				// Indicates a tap on X axis
            if(iData.iDirection==KSensrvAccelerometerDirectionXplus|KSensrvAccelerometerDirectionXminus)
                {
                iReading.setTapDirection(QTapReading::X);
                }
				// Indicates a tap on Y axis
            else if(iData.iDirection==KSensrvAccelerometerDirectionYplus|KSensrvAccelerometerDirectionYminus)
                {
                iReading.setTapDirection(QTapReading::Y);
                }
				// Indicates a tap on Z axis
            else if(iData.iDirection==KSensrvAccelerometerDirectionZplus|KSensrvAccelerometerDirectionZminus)
                {
                iReading.setTapDirection(QTapReading::Z);
                }
				 // Undefined value
            else
                {
                iReading.setTapDirection(QTapReading::Undefined);
                }
            }            
        }
	//Set the type of tap to be double tap
    iReading.setDoubleTap(true);
    iReading.setTimestamp(iData.iTimeStamp.Int64());  
	    // Release the lock
    iBackendData.iReadingLock.Signal();
    }
/**
 * Second phase constructor
 * Initialize the backend resources
 */
void CTapSensorSym::ConstructL()
    {
    InitializeL();
    }

