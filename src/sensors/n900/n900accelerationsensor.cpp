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

#include <qaccelerationsensor.h>
#include <QDebug>

QTM_BEGIN_NAMESPACE

class n900accelerationsensor : public QAccelerationBackend
{
public:
    n900accelerationsensor()
        : m_interval(0)
        , m_timerid(0)
        , m_filename("/sys/class/i2c-adapter/i2c-3/3-001d/coord")
    {
    }

    QSensor::UpdatePolicies supportedPolicies() const
    {
        return (QSensor::OccasionalUpdates |
                QSensor::InfrequentUpdates |
                QSensor::FrequentUpdates |
                QSensor::TimedUpdates |
                QSensor::PolledUpdates);
    }

    void setUpdatePolicy(QSensor::UpdatePolicy policy, int interval)
    {
        rememberUpdatePolicy(policy, interval);

        if (m_timerid)
            return;

        switch (policy) {
        case QSensor::OccasionalUpdates:
        case QSensor::InfrequentUpdates:
        case QSensor::FrequentUpdates:
            m_interval = suggestedInterval(policy);
            break;
        case QSensor::TimedUpdates:
            m_interval = interval;
            break;
        case QSensor::PolledUpdates:
            m_interval = 0;
            break;
        default:
            break;
        }
    }

    bool start()
    {
        if (m_interval)
            m_timerid = startTimer(m_interval);
        return true;
    }

    void stop()
    {
        if (m_timerid) {
            killTimer(m_timerid);
            m_timerid = -1;
        }
    }

    void timerEvent(QTimerEvent * /*event*/)
    {
        poll();
    }

    void poll()
    {
        qWarning() << "poll";
        QTime timestamp = QTime::currentTime();
        FILE *fd = fopen(m_filename, "r");
        if (!fd) return;
        int x, y, z;
	int rs = fscanf(fd, "%i %i %i", &x, &y, &z);
        fclose(fd);
        if (rs != 3) return;

        m_lastReading = QAccelerationReading(timestamp, x, y, z);
        if (m_interval)
            m_sensor->newReadingAvailable();
    }

    QAccelerationReading currentReading()
    {
        if (m_interval == 0)
            poll();
        return m_lastReading;
    }

private:
    int m_interval;
    int m_timerid;
    const char *m_filename;
    QAccelerationReading m_lastReading;
};

REGISTER_SENSOR(n900accelerationsensor,
        QAccelerationSensor::typeId,
        QByteArray("n900.acceleration"))

QTM_END_NAMESPACE

