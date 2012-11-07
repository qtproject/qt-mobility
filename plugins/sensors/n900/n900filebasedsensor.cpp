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

#include "n900filebasedsensor.h"
#include <time.h>
#include <QTimer>

n900filebasedsensor::n900filebasedsensor(QSensor *sensor)
    : QSensorBackend(sensor)
    , m_timerid(0)
{
}

n900filebasedsensor::~n900filebasedsensor()
{
}

void n900filebasedsensor::start()
{
    if (m_timerid)
        return;

    int dataRate = sensor()->dataRate();
    if (dataRate == 0) {
        if (sensor()->availableDataRates().count())
            // Use the faster part of the first available rate by default
            dataRate = sensor()->availableDataRates().first().second;
        else
            dataRate = 1;
    }

    int interval = 1000 / dataRate;

    if (interval)
        m_timerid = startTimer(interval);

    // Don't poll now because Symbian/MeeGo can't do anything till we hit the event loop
    // Emulate this behaviour for consistency
    QTimer::singleShot(0, this, SLOT(pollnow()));
}

void n900filebasedsensor::stop()
{
    if (m_timerid) {
        killTimer(m_timerid);
        m_timerid = 0;
    }
}

void n900filebasedsensor::timerEvent(QTimerEvent * /*event*/)
{
    poll();
}

quint64 n900filebasedsensor::getTimestamp()
{
    struct timespec tv;
    int ok;

    ok = clock_gettime(CLOCK_MONOTONIC, &tv);
    Q_ASSERT(ok == 0);

    quint64 result = (tv.tv_sec * 1000000ULL) + (tv.tv_nsec * 0.001); // scale to microseconds
    return result;
}

void n900filebasedsensor::pollnow()
{
    poll();
}

