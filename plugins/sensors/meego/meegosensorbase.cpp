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

#include "meegosensorbase.h"


SensorManagerInterface* meegosensorbase::m_remoteSensorManager = 0;

//According to wikipedia link http://en.wikipedia.org/wiki/Standard_gravity
//const float meegosensorbase::GRAVITY_EARTH = 9.812865328;
const float meegosensorbase::GRAVITY_EARTH_THOUSANDTH = 0.009812865328;
const int meegosensorbase::KErrNotFound=-1;
const int meegosensorbase::KErrInUse=-14;
const char* const meegosensorbase::ALWAYS_ON = "alwaysOn";
const char* const meegosensorbase::BUFFER_SIZE = "bufferSize";
const char* const meegosensorbase::MAX_BUFFER_SIZE = "maxBufferSize";
const char* const meegosensorbase::EFFICIENT_BUFFER_SIZE = "efficientBufferSize";
QStringList meegosensorbase::m_bufferingSensors = QStringList()<<"meego.accelerometer"<<"meego.magnetometer"<<"meego.gyroscope"<<"meego.rotationsensor";

meegosensorbase::meegosensorbase(QSensor *sensor)
    : QSensorBackend(sensor), m_sensorInterface(0), m_bufferSize(-1), m_prevOutputRange(0), m_efficientBufferSize(1), m_maxBufferSize(1)
{
    if (!m_remoteSensorManager)
        m_remoteSensorManager = &SensorManagerInterface::instance();    
    
}

meegosensorbase::~meegosensorbase()
{
    if (m_sensorInterface) {
        stop();
        QObject::disconnect(m_sensorInterface);
        delete m_sensorInterface, m_sensorInterface = 0;
    }
}

void meegosensorbase::start()
{
    if (m_sensorInterface) {
        // dataRate
        QString type = sensor()->type();
        if (type!="QTapSensor" && type!="QProximitySensor"){
            int dataRate = sensor()->dataRate();
            int interval = dataRate>0 ? 1000 / dataRate : 0;
            // for testing maximum speed
            //interval = 1;
            //dataRate = 1000;
            qDebug() << "Setting data rate" << dataRate << "Hz (interval" << interval << "ms) for" << sensor()->identifier();
            m_sensorInterface->setInterval(interval);
        }

        // outputRange
        int currentRange = sensor()->outputRange();
        int l = sensor()->outputRanges().size();
        if (l>1){
            if (currentRange != m_prevOutputRange){
#ifdef Q_WS_MAEMO_6
                bool isOk = m_sensorInterface->setDataRangeIndex(currentRange); //NOTE THAT THE CHANGE MIGHT NOT SUCCEED, FIRST COME FIRST SERVED
                if (!isOk) sensorError(KErrInUse);
                else m_prevOutputRange = currentRange;
#else
                // TODO: remove when sensord integrated, in MeeGo env there is a delay
                qoutputrange range = sensor()->outputRanges().at(currentRange);
                qreal correction = 1/correctionFactor();
                DataRange range1(range.minimum*correction, range.maximum*correction, range.accuracy*correction);
                m_sensorInterface->requestDataRange(range1);
                m_prevOutputRange = currentRange;
#endif
            }
        }
        
        // always on
        QVariant alwaysOn = sensor()->property(ALWAYS_ON);
        alwaysOn.isValid()?
                m_sensorInterface->setStandbyOverride(alwaysOn.toBool()):
                m_sensorInterface->setStandbyOverride(false);
        
        // connects after buffering checks
        doConnectAfterCheck();
        
        int returnCode = m_sensorInterface->start().error().type();
        if (returnCode==0) return;
        qWarning()<<"m_sensorInterface did not start, error code:"<<returnCode;
    }
    sensorStopped();
}

void meegosensorbase::stop()
{
    if (m_sensorInterface) m_sensorInterface->stop();
}

void meegosensorbase::setRanges(qreal correctionFactor){
    if (!m_sensorInterface) return;
    
    QList<DataRange> ranges = m_sensorInterface->getAvailableDataRanges();
    
    for (int i=0, l=ranges.size(); i<l; i++){
        DataRange range = ranges.at(i);
        qreal rangeMin = range.min * correctionFactor;
        qreal rangeMax = range.max * correctionFactor;
        qreal resolution = range.resolution * correctionFactor;
        addOutputRange(rangeMin, rangeMax, resolution);
    }
}


bool meegosensorbase::doConnectAfterCheck(){
    if (!m_sensorInterface) return false;
    
    // buffer size
    int size = bufferSize();
    if (size == m_bufferSize) return true;
    
    
    if (m_bufferingSensors.contains(sensor()->identifier()))
        m_sensorInterface->setBufferSize(size);
    else size = 1;

    // if multiple->single or single->multiple or if uninitialized
    if ((m_bufferSize>1 && size==1) || (m_bufferSize==1 && size>1) || m_bufferSize==-1){
        m_bufferSize = size;
        disconnect(this);
        if (!doConnect()){
            qWarning() << "Unable to connect "<< sensorName();
            return false;
        }
        return true;
    }
    m_bufferSize = size;
    return true;
}


int meegosensorbase::bufferSize() const{
    QVariant bufferVariant = sensor()->property(BUFFER_SIZE);
    int bufferSize = bufferVariant.isValid()?bufferVariant.toInt():1;
    if (bufferSize==1) return 1;
    
    // otherwise check validity
    if (bufferSize<1){
        qWarning()<<"bufferSize cannot be "<<bufferSize<<", must be a positive number";
        return m_bufferSize>0?m_bufferSize:1;
    }
    if (bufferSize>m_maxBufferSize){
        qWarning()<<"bufferSize cannot be "<<bufferSize<<", MAX value is "<<m_maxBufferSize;
        return m_bufferSize>0?m_bufferSize:1;
    }
    return bufferSize;
}

qreal meegosensorbase::correctionFactor() const{return 1;}

