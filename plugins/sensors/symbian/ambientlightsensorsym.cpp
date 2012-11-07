/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ambientlightsensorsym.h"

/**
 * set the id of the ambient light sensor
 */
char const * const CAmbientLightSensorSym::id("sym.ambientlight");

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
 * DataReceived is used to retrieve the sensor reading from sensor server
 * It is implemented here to handle ambient light sensor specific
 * reading data and provides conversion and utility code
 */
void CAmbientLightSensorSym::DataReceived(CSensrvChannel &aChannel, TInt aCount, TInt /*aDataLost*/)
    {
    ProcessData(aChannel, aCount, iData);
    }

void CAmbientLightSensorSym::ProcessReading()
    {
    // Get a lock on the reading data
    iBackendData.iReadingLock.Wait();

    // Reason why switch/case was changed to separate if clauses is that
    // we do not need to use new enums that were added to platform code,
    // so this code should work also in case that those new enums are not
    // defined in some platform where this same QtMobility code is used.
    if (iData.iAmbientLight < TSensrvAmbientLightData::KAmbientLightTwilight) {
        // KAmbientLightVeryDark, KAmbientLightDark
        iReading.setLightLevel(QAmbientLightReading::Dark);	
    } else if (iData.iAmbientLight < TSensrvAmbientLightData::KAmbientLightLight) {
        // KAmbientLightTwilight
        iReading.setLightLevel(QAmbientLightReading::Twilight);
    } else if (iData.iAmbientLight < TSensrvAmbientLightData::KAmbientLightBright) {
        // KAmbientLightLight
        iReading.setLightLevel(QAmbientLightReading::Light);
    } else if (iData.iAmbientLight < TSensrvAmbientLightData::KAmbientLightSunny) {
        // KAmbientLightBright
        iReading.setLightLevel(QAmbientLightReading::Bright);    	  	
    } else {
        // KAmbientLightCloudy , KAmbientLightCloudySunny, KAmbientLightSunny
        iReading.setLightLevel(QAmbientLightReading::Sunny);	
    }
    
    // Set the timestamp
    iReading.setTimestamp(iData.iTimeStamp.Int64());
    // Release the lock
    iBackendData.iReadingLock.Signal();
    // Notify that a reading is available
    newReadingAvailable();
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

