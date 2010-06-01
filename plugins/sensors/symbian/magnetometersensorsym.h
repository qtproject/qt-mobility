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

#ifndef MAGNETOMETERSENSORSYM_H
#define MAGNETOMETERSENSORSYM_H

// QT Mobility Sensor API headers
#include <qsensorbackend.h>
#include <qmagnetometer.h>

// Internal Headers
#include "sensorbackendsym.h"

// Sensor client headers
// Magnetometer Sensor specific header
#include <sensrvmagnetometersensor.h>

QTM_USE_NAMESPACE

class CMagnetometerSensorSym: public CSensorBackendSym
    {
public:
    /**
     * Factory function, this is used to create the magnetometer sensor object
     * @return CMagnetometerSensorSym if successful, leaves on failure
     */
    static CMagnetometerSensorSym* NewL(QSensor *sensor);
    
    /**
     * Destructor
     * Closes the backend resources
     */
    ~CMagnetometerSensorSym();
    
    /*
     * Used to retrieve the current calibration level
     * iCalibrationLevel is automatically updated whenever there is a change
     * in calibration level
     */
    qreal GetCalibrationLevel();
    
    /**
     * start is overridden to allow retrieving initial calibration property before
     * and to set the required value type flags
     */
    void start();
    
private:
    /**
     * Default constructor
     */
    CMagnetometerSensorSym(QSensor *sensor);
    
    /*
     * RecvData is used to retrieve the sensor reading from sensor server
     * It is implemented here to handle magnetometer sensor specific
     * reading data and provides conversion and utility code
     */ 
    void RecvData(CSensrvChannel &aChannel);
    
    /**
     * HandlePropertyChange is called from backend, to indicate a change in property
     */
    void HandlePropertyChange(CSensrvChannel &aChannel, const TSensrvProperty &aChangedProperty);
    
    /**
     * Second phase constructor
     * Initialize the backend resources
     */
    void ConstructL();    
    
public:
    /**
     * Holds the id of the magnetometer
     */
    static char const * const id;
    
private:     
    QMagnetometerReading iReading;
    TSensrvMagnetometerAxisData iData;
    qreal iCalibrationLevel;
    bool iReturnGeoValues;
    };

#endif //MAGNETOMETERSENSORSYM_H
