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

#include "gypsymock.h"

#include <QTimerEvent>
#include <QDebug>
#include <QMetaObject>

GypsyMock::GypsyMock() :
    m_satelliteSource(0),
    m_satellitesChangedCallback(0),
    m_disableUpdates(false)
{
    TRACE0;
    m_regular.satellites = 0;
    m_currentJournal = ":/data/gypsymock_basic_sat.journal";
}

GypsyMock::~GypsyMock()
{
    TRACE0;
    if (m_regular.satellites) {
        // fixme, needs to be analyzed why freeing here causes crash
        //gypsy_satellite_free_satellite_array(m_regular.satellites);
    }
}

void GypsyMock::disableUpdates(bool value)
{
    TRACE0;
    m_disableUpdates = value;
}

void GypsyMock::start()
{
    TRACE0;
    Q_ASSERT(QFile::exists(m_currentJournal));
    stop(); // just in case
    // If file is already open, continue reading it.
    if (!m_currentJournalFile.isOpen()) {
        m_currentJournalFile.setFileName(m_currentJournal);
        if (!m_currentJournalFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "Journal opening failed:" << m_currentJournalFile.fileName();
            Q_ASSERT(false);
        }
    }
    UPDATE_TYPE update = readNextEntry();
    if (update == REGULAR_UPDATE) {
        m_regular.timerId = startTimer(m_regular.timeout);
    } else {
        Q_ASSERT(false); // journal ran out
    }
}

void GypsyMock::stop()
{
    TRACE0;
    killTimer(m_regular.timerId);
    m_regular.timerId = 0;
    if (m_currentJournalFile.isOpen())
        m_currentJournalFile.close();
    m_satellitesChangedCallback = 0;
}

void GypsyMock::timerEvent(QTimerEvent *event)
{
    TRACE0;
    if (!m_satellitesChangedCallback || !m_satelliteSource) {
        TRACE1("No registered callbacks or objects, update dropped.");
        return;
    }
    if (event->timerId() == m_regular.timerId) {
        if (!m_disableUpdates) {
            (*m_satellitesChangedCallback)((GypsySatellite*)1,  // dummy
                                           m_regular.satellites,
                                           m_satelliteSource);
        }
        if (m_regular.repeats > 0) {
            m_regular.repeats--;
            return;
        }
        killTimer(m_regular.timerId);
    } else {
        TRACE2("timer astray, killing and ignoring:", event->timerId());
        killTimer(event->timerId()); // Something has gone astray, kill and ignore
        return;
    }
    // no more repeats
    UPDATE_TYPE update = readNextEntry();
    if (update == REGULAR_UPDATE) {
        TRACE2("Starting timer id: ", m_regular.timerId);
        m_regular.timerId = startTimer(m_regular.timeout);
    } else {
        Q_ASSERT(false); // journal ran out
    }
}

GypsyMock::UPDATE_TYPE GypsyMock::readNextEntry()
{
    TRACE0;
    QString line;
    QTextStream stream;
    do {
        line = m_currentJournalFile.readLine();
        //line = line.trimmed();
        if (line.at(0) == '#' || line.at(0) == '\n') {
            continue;
        }
        else if (line == "START_REGULAR_SAT_UPDATE\n") {
            line = m_currentJournalFile.readLine();
            stream.setString(&line);
            // initialize datastructs (release any previous data)
            if (m_regular.satellites)
                gypsy_satellite_free_satellite_array(m_regular.satellites);
            m_regular.satellites = g_ptr_array_new();
            parseRegularUpdate(stream);
            continue;
        }
        else if (line == "SATELLITE\n") {
            // read single satellite data
            line = m_currentJournalFile.readLine();
            stream.setString(&line);
            parseSatelliteData(stream);
            continue; // read next satellite or end of line
        }
        else if (line == "END_REGULAR_SAT_UPDATE\n") {
            return REGULAR_UPDATE; // read data for one update round
        }
        else {
            qCritical("Invalid input file for gypsy mocking.");
            Q_ASSERT(false);
        }
    } while (line.length() > 0);
    TRACE1("Invalid update.");
    return INVALID_UPDATE;
}

void GypsyMock::parseRegularUpdate(QTextStream& stream)
{
   TRACE0;
   stream >> m_regular.repeats >> m_regular.timeout;
   //qDebug() << "parsed values: " << m_regular.repeats << m_regular.timeout;
}

void GypsyMock::parseSatelliteData(QTextStream& stream)
{
    TRACE0;
    // create new satellite struct
    GypsySatelliteDetails* details = new GypsySatelliteDetails;
    int in_use;
    stream >> details->elevation >> details->azimuth >> details->satellite_id >> details->snr >> in_use;
    details->in_use = (bool)in_use; // stream doesn't support bools
    // qDebug() << "parsed values: " << details->elevation << details->azimuth << details->satellite_id << details->snr << details->in_use;
    // add it to the struct
    g_ptr_array_add(m_regular.satellites, (gpointer)details);
}

void GypsyMock::disconnectSignal(int callback, int satelliteSource)
{
    TRACE0;
    if ((gpointer)callback == (gpointer)m_satellitesChangedCallback &&
            (m_satelliteSource == (QObject*)satelliteSource)) {
        m_satellitesChangedCallback = 0;
    } else {
        TRACE1("Pretty bad, wrong callback was requested to disconnect. Fix.");
    }
}

void GypsyMock::connectSignal(int signal, int callback, int satelliteSource)
{
    TRACE0;
    if (!QString::compare(QString::fromAscii((char*)signal), "satellites-changed")) {
        m_satellitesChangedCallback = (SatellitesChangedCallback)callback;
        m_satelliteSource = (QObject*)satelliteSource;
    } else {
        TRACE2("Invalid callback signal:", signal);
    }
}
