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

#ifndef MEEGOSENSORBASE_H
#define MEEGOSENSORBASE_H

#include <qsensorbackend.h>
#include <sensormanagerinterface.h>
#include <abstractsensor_i.h>

QTM_USE_NAMESPACE

        class meegosensorbase : public QSensorBackend
{
public:
    meegosensorbase(QSensor *sensor);
    virtual ~meegosensorbase();


protected:
    virtual bool doConnect()=0;
    virtual void start();
    virtual void stop();

    static const char* const ALWAYS_ON;
    static const char* const BUFFER_SIZE;
    static const char* const MAX_BUFFER_SIZE;
    static const char* const EFFICIENT_BUFFER_SIZE;
    static const float GRAVITY_EARTH;
    static const float GRAVITY_EARTH_THOUSANDTH;    //for speed
    static const int KErrNotFound;

    void setRanges(qreal correctionFactor=1);
    virtual const QString sensorName()=0;

    template<typename T>
    void initSensor(bool &initDone)
    {

        const QString name = sensorName();

        if (!initDone) {
            if (!m_remoteSensorManager->loadPlugin(name)){
                sensorError(KErrNotFound);
                return;
            }
            m_remoteSensorManager->registerSensorInterface<T>(name);
        }
        m_sensorInterface = T::controlInterface(name);
        if (!m_sensorInterface) {
            m_sensorInterface = const_cast<T*>(T::listenInterface(name));
        }
        if (!m_sensorInterface) {
            sensorError(KErrNotFound);
            return;
        }

        initDone = true;
        
        //metadata
        QList<DataRange> intervals = m_sensorInterface->getAvailableIntervals();

        for (int i=0, l=intervals.size(); i<l; i++){
            qreal intervalMax = ((DataRange)(intervals.at(i))).max;
            qreal intervalMin =((DataRange)(intervals.at(i))).min;

            if (intervalMin==0 && intervalMax==0){
                // 0 interval has different meanings in e.g. magge/acce
                // magge -> best-effort
                // acce -> lowest possible
                // in Qt API setting 0 means default
                continue;
            }

            qreal rateMin = intervalMax<1 ? 1 : 1/intervalMax * 1000;
            rateMin = rateMin<1 ? 1 : rateMin;

            intervalMin = intervalMin<1 ? 10: intervalMin;     // do not divide with 0
            qreal rateMax = 1/intervalMin * 1000;

            //            qreal rateMax = (intervalMin<1) ? rateMin : 1/intervalMin * 1000; // TODO: replace the two lines above with this one once sensord does provide interval>0
            addDataRate(rateMin, rateMax);
        }

        //bufferSizes
        //TODO: waiting next sensord version
//        IntegerRangeList sizes = m_sensorInterface->getAvailableBufferSizes();
//        int l = sizes.size();
//        if (l>0){
//            m_efficientBufferSize = (l==1) ? 1 : sizes.at(1).first;
//            m_maxBufferSize = sizes.at(l-1).second;
//        }
        m_maxBufferSize = 256;  // TODO: remove once the snippet above is taken into use
        sensor()->setProperty(MAX_BUFFER_SIZE, m_maxBufferSize);
        sensor()->setProperty(EFFICIENT_BUFFER_SIZE, m_efficientBufferSize);

        if (name=="alssensor") return;                // SensorFW returns lux values, plugin enumerated values
        if (name=="accelerometersensor") return;      // SensorFW returns milliGs, plugin m/s^2
        if (name=="magnetometersensor") return;       // SensorFW returns nanoTeslas, plugin Teslas
        if (name=="gyroscopesensor") return;          // SensorFW returns DSPs, plugin milliDSPs

        setDescription(m_sensorInterface->property("description").toString());

        if (name=="tapsensor") return;
        setRanges();
    };


    AbstractSensorChannelInterface* m_sensorInterface;
    int m_bufferSize;
    const int bufferSize();

private:
    static SensorManagerInterface* m_remoteSensorManager;
    int m_prevOutputRange;
    bool doConnectAfterCheck();
    int m_efficientBufferSize, m_maxBufferSize;

};

#endif
