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
//Symbian
#include <e32std.h>
#include <rrsensorapi.h>

// Local
#include "qs60sensorapiaccelerometer.h"

// Constants
const int KAccelerometerSensorUID = 0x10273024;

const char *QS60SensorApiAccelerometer::id("s60sensorapi.accelerometer");

QS60SensorApiAccelerometer::QS60SensorApiAccelerometer(QSensor *sensor)
    : QSensorBackend(sensor)
    , m_nativeSensor(NULL)
    , m_sampleFactor(0.0f)
{
    TRAPD(err, findAndCreateNativeSensorL());
    if(err != KErrNone) {
        sensorStopped();
        sensorError(err);
    }
    
    setReading<QAccelerometerReading>(&m_reading);
    
    // http://www.st.com/stonline/products/literature/ds/12726/lis302dl.pdf
    // That 3D accelerometer inside N95 , N93i or N82 is from STMicroelectronics (type LIS302DL).
    // http://wiki.forum.nokia.com/index.php/Nokia_Sensor_APIs.
    // Sensor is set to 100Hz 2G mode and no public interface to switch it to 8G
    
    // 2G - mode
    addDataRate(100, 100);
    sensor->setDataRate(100);
    addOutputRange(-22.418, 22.418, 0.17651);
    setDescription(QLatin1String("lis302dl"));
    
    //Synbian interface gives values between -680 - 680
    m_sampleFactor =  this->sensor()->outputRanges()[0].maximum / 680.0f;
}

QS60SensorApiAccelerometer::~QS60SensorApiAccelerometer()
{
    stop();
    delete m_nativeSensor;
    m_nativeSensor = NULL;
}

void QS60SensorApiAccelerometer::start()
{
    if(!m_nativeSensor)
        return;
    
    m_nativeSensor->AddDataListener(this);    
}

void QS60SensorApiAccelerometer::stop()
{
    if(!m_nativeSensor)
        return;
    
    m_nativeSensor->RemoveDataListener();
}

void QS60SensorApiAccelerometer::poll()
{
    //empty implementation
}


void QS60SensorApiAccelerometer::HandleDataEventL(TRRSensorInfo aSensor, TRRSensorEvent aEvent)
{
    if (aSensor.iSensorId != KAccelerometerSensorUID) 
        return; 
    
    TTime time;
    time.UniversalTime();
    m_reading.setTimestamp(time.Int64());
    m_reading.setX((qreal)aEvent.iSensorData2 * m_sampleFactor);
    m_reading.setY((qreal)aEvent.iSensorData1 * -m_sampleFactor);
    m_reading.setZ((qreal)aEvent.iSensorData3 * -m_sampleFactor);
    newReadingAvailable();
}

void QS60SensorApiAccelerometer::findAndCreateNativeSensorL()
{
    if(m_nativeSensor)
        return;
    
    RArray<TRRSensorInfo> sensorList;
    CRRSensorApi::FindSensorsL(sensorList);
    CleanupClosePushL(sensorList);

    TInt index = 0;
    do {
        if (sensorList[index].iSensorId == KAccelerometerSensorUID)
            m_nativeSensor = CRRSensorApi::NewL(sensorList[index]);
    } while(!m_nativeSensor && ++index < sensorList.Count());
        
    if (!m_nativeSensor)
        User::Leave(KErrHardwareNotAvailable);
    
    CleanupStack::PopAndDestroy(&sensorList);
}
