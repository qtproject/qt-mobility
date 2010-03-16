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

#ifndef QS60SENSORAPICOMMON_H
#define QS60SENSORAPICOMMON_H

// mobility
#include <qsensorbackend.h>

// symbian
#include <rrsensorapi.h>

QTM_USE_NAMESPACE

class QS60SensorApiCommon : public QSensorBackend,  public MRRSensorDataListener
{
public:
    QS60SensorApiCommon(QSensor *sensor);
    virtual ~QS60SensorApiCommon();

    // from QSensorBackend
    virtual void start();
    virtual void stop();
    void poll();

    // from MRRSensorDataListener
    virtual void HandleDataEventL(TRRSensorInfo aSensor, TRRSensorEvent aEvent);

    
protected:
    void findAndCreateNativeSensorL();
    virtual int nativeSensorId()= 0;

private:
    CRRSensorApi* m_nativeSensor;
};

#endif //QS60SENSORAPICOMMON_H
