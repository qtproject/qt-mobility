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

#ifndef MAEMO6SENSORBASE_H
#define MAEMO6SENSORBASE_H

#include <qsensorbackend.h>
#include <sensormanagerinterface.h>
#include <abstractsensor_i.h>

QTM_USE_NAMESPACE

        class maemo6sensorbase : public QSensorBackend
{
public:
    maemo6sensorbase(QSensor *sensor);
    virtual ~maemo6sensorbase();


protected:
    virtual void start();
    virtual void stop();
    AbstractSensorChannelInterface* m_sensorInterface;

    static const float GRAVITY_EARTH;
    static const float GRAVITY_EARTH_THOUSANDTH;    //for speed

    template<typename T>
    void initSensor(QString sensorName, bool &initDone)
    {

        if (!initDone) {
            m_remoteSensorManager->loadPlugin(sensorName);
            m_remoteSensorManager->registerSensorInterface<T>(sensorName);
        }
        m_sensorInterface = T::controlInterface(sensorName);
        if (!m_sensorInterface) {
            m_sensorInterface = const_cast<T*>(T::listenInterface(sensorName));
        }

        initDone = true;

        if (sensorName=="alssensor") return; // SensorFW returns lux values, plugin enumerated values


        //metadata
        int l = m_sensorInterface->getAvailableIntervals().size();
        for (int i=0; i<l; i++){
            qreal intervalMax = ((DataRange)(m_sensorInterface->getAvailableIntervals().at(i))).max;
            qreal rateMin = intervalMax<1 ? 1 : 1/intervalMax * 1000;
            rateMin = rateMin<1 ? 1 : rateMin;
            qreal intervalMin =((DataRange)(m_sensorInterface->getAvailableIntervals().at(i))).min;
            intervalMin = intervalMin<1 ? 10: intervalMin;     // do not divide with 0
            qreal rateMax = 1/intervalMin * 1000;
            //            qreal rateMax = (intervalMin<1) ? rateMin : 1/intervalMin * 1000; // TODO: replace the two lines above with this one once sensord does provide interval>0
            addDataRate(rateMin, rateMax);
        }

        l = m_sensorInterface->getAvailableDataRanges().size();

        for (int i=0; i<l; i++){
            qreal rangeMin = ((DataRange)(m_sensorInterface->getAvailableDataRanges().at(i))).min;
            qreal rangeMax =((DataRange)(m_sensorInterface->getAvailableDataRanges().at(i))).max;
            qreal resolution = ((DataRange)(m_sensorInterface->getAvailableDataRanges().at(i))).min;
            addOutputRange(rangeMin, rangeMax, resolution);
        }
        setDescription(m_sensorInterface->property("description").toString());
    };

private:
    static SensorManagerInterface* m_remoteSensorManager;

};

#endif
