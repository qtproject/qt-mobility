/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ambientlightsensorsym.h"

/**
 * set the id of the ambient light sensor
 */
const char *CAmbientLightSensorSym::id("sym.ambientlight");

/**
 * Factory function, this is used to create the ambient light sensor object
 * @return CAmbientLightSensorSym if successful, leaves on failure
 */
CAmbientLightSensorSym* CAmbientLightSensorSym::NewL(QSensor *sensor)
    {
    CAmbientLightSensorSym* self = new (ELeave) CAmbientLightSensorSym(sensor);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

/**
 * Destructor
 * Closes the backend resources
 */
CAmbientLightSensorSym::~CAmbientLightSensorSym()
    {
    // Release the backend resources
    Close();
    }

/**
 * Default constructor
 */
CAmbientLightSensorSym::CAmbientLightSensorSym(QSensor *sensor):CSensorBackendSym(sensor)
    {
    setReading<QAmbientLightReading>(&iReading);    
    iBackendData.iSensorType = KSensrvChannelTypeIdAmbientLightData;
    }

/*
 * RecvData is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle ambient light sensor specific
 * reading data and provides conversion and utility code
 */  
void CAmbientLightSensorSym::RecvData(CSensrvChannel &aChannel)
    {
    TPckg<TSensrvAmbientLightData> lightpkg( iData );
    TInt ret = aChannel.GetData( lightpkg );
    if(KErrNone != ret)
        {
        // If there is no reading available, return without setting
        return;
        }
    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();
    switch (iData.iAmbientLight)
        {
        case TSensrvAmbientLightData::KAmbientLightVeryDark:
        case TSensrvAmbientLightData::KAmbientLightDark:
            {
            iReading.setLightLevel(QAmbientLightReading::Dark);               
            }
            break;

        case TSensrvAmbientLightData::KAmbientLightTwilight:
            {
            iReading.setLightLevel(QAmbientLightReading::Twilight);
            }
            break;

        case TSensrvAmbientLightData::KAmbientLightLight:
            {
            iReading.setLightLevel(QAmbientLightReading::Light);
            }
            break;

        case TSensrvAmbientLightData::KAmbientLightBright:
            {
            iReading.setLightLevel(QAmbientLightReading::Bright);
            }
            break;

        case TSensrvAmbientLightData::KAmbientLightSunny:
            {
            iReading.setLightLevel(QAmbientLightReading::Sunny);
            }
            break;

        default:
            {
            iReading.setLightLevel(QAmbientLightReading::Undefined);
            }
        }
    // Set the timestamp
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Release the lock
    iBackendData.iReadingLock.Signal();
    }

/**
 * Second phase constructor
 * Initialize the backend resources
 */
void CAmbientLightSensorSym::ConstructL()
    {
    //Initialize the backend resources
    InitializeL();
    }

