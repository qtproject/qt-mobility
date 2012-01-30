/**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60videoframerate.h"
#include <QtCore/QDebug>
#include <QtCore/QTimer>

static const int HistoryLength = 5;
static const int SamplingInterval = 100;
static const int TraceInterval = 2000;

S60VideoFrameRate::S60VideoFrameRate(QObject *parent)
    : QObject(parent)
    , m_sampleTimer(new QTimer(this))
    , m_traceTimer(new QTimer(this))
    , m_elapsedTimer(new QElapsedTimer)
    , m_count(0)
    , m_history(HistoryLength, 0.0)
    , m_historyIndex(0)
    , m_historyCount(0)
    , m_frequency(0.0)
    , m_frequencySum(0.0)
{
    connect(m_sampleTimer, SIGNAL(timeout()), this, SLOT(sample()));
    m_sampleTimer->start(SamplingInterval);
    connect(m_traceTimer, SIGNAL(timeout()), this, SLOT(trace()));
    m_traceTimer->start(TraceInterval);
    m_elapsedTimer->start();
}

S60VideoFrameRate::~S60VideoFrameRate()
{

}

qreal S60VideoFrameRate::frequency() const
{
    return m_frequency;
}

void S60VideoFrameRate::notify()
{
    ++m_count;
}

void S60VideoFrameRate::sample()
{
    const int ms = m_elapsedTimer->restart();
    const qreal freq = qreal(m_count * 1000) / ms;
    m_frequencySum -= m_history[m_historyIndex];
    m_frequencySum += freq;
    m_history[m_historyIndex] = freq;
    m_historyIndex = (m_historyIndex + 1) % m_history.count();
    if (m_historyCount < m_history.count())
        ++m_historyCount;
    m_frequency = m_frequencySum / m_historyCount;
    emit frequencyChanged(m_frequency);
    m_count = 0;
}

void S60VideoFrameRate::trace()
{
    qDebug() << "S60VideoFrameRate" << m_frequency << "fps";
}
