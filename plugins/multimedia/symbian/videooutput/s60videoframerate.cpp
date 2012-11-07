/**
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
