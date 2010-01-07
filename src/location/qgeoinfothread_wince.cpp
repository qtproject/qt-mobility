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

#include <QMetaType>

#include "qgeoinfothread_wince_p.h"

Q_DECLARE_METATYPE(GPS_POSITION);

QTM_BEGIN_NAMESPACE

// ========== QGeoInfoValidator ==========

QGeoInfoValidator::QGeoInfoValidator() {}

QGeoInfoValidator::~QGeoInfoValidator() {}

// ========== QGeoInfoThreadWinCE ==========


// This QGeoInfoThreadWinCE instance takes ownership of the validator, and must delete it before
// it is destructed.
QGeoInfoThreadWinCE::QGeoInfoThreadWinCE(QGeoInfoValidator *validator, bool timeoutsForPeriodicUpdates, QObject *parent)
        : QThread(parent),
        validator(validator),
        timeoutsForPeriodicUpdates(timeoutsForPeriodicUpdates),
        requestScheduled(false),
        requestInterval(0),
        updatesScheduled(false),
        updatesInterval(0),
        hasLastPosition(false),
        invalidDataReceived(false),
        updateTimeoutTriggered(false)
{
    qRegisterMetaType<GPS_POSITION>();
    m_newDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_gpsStateChange = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_wakeUpEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

QGeoInfoThreadWinCE::~QGeoInfoThreadWinCE()
{
    // Let/make the thread finish...

    mutex.lock();

    updatesScheduled = false;
    requestScheduled = false;
    wakeUp();

    mutex.unlock();

    wait();

    // ...then clean up.

    delete validator;

    CloseHandle(m_newDataEvent);
    CloseHandle(m_gpsStateChange);
    CloseHandle(m_wakeUpEvent);
}

// TODO - just need to add the WinCE line from QTime::currentTime to QDateTime::currentDateTime
// and we can scrap this method
QDateTime QGeoInfoThreadWinCE::currentDateTime()
{
    QDate d = QDate::currentDate();
    QTime t = QTime::currentTime();

    // just in case we past midnight in between the last two calls
    if (d != QDate::currentDate()) {
        d = QDate::currentDate();
        t = QTime::currentTime();
    }

    return QDateTime(d, t);
}

int QGeoInfoThreadWinCE::msecsTo(QDateTime from, QDateTime to)
{
    int MSECS_PER_DAY = 86400000;
    return (from.date().daysTo(to.date()) * MSECS_PER_DAY) + from.time().msecsTo(to.time());
}

void QGeoInfoThreadWinCE::requestUpdate(int timeout)
{
    QMutexLocker locker(&mutex);

    if (!requestScheduled) {
        requestScheduled = true;

        if (timeout == 0)
            timeout = DefaultRequestTimeout;

        requestInterval = timeout;
        requestNextTime = currentDateTime().addMSecs(requestInterval);

        // See comments above run() to see why we're doing things like this
        if (!isRunning())
            start();
        else
            wakeUp();
    }
}

void QGeoInfoThreadWinCE::startUpdates()
{
    if (!updatesScheduled) {
        QMutexLocker locker(&mutex);

        updatesScheduled = true;
        updateTimeoutTriggered = false;
        hasLastPosition = false;

        if (updatesInterval != 0)
            updatesNextTime = currentDateTime().addMSecs(updatesInterval);

        // See comments above run() to see why we're doing things like this
        if (!isRunning())
            start();
        else
            wakeUp();
    }
}

void QGeoInfoThreadWinCE::stopUpdates()
{
    if (updatesScheduled) {
        QMutexLocker locker(&mutex);
        updatesScheduled = false;
        wakeUp();
    }
}

void QGeoInfoThreadWinCE::setUpdateInterval(int interval)
{
    QMutexLocker locker(&mutex);

    if (interval == updatesInterval)
        return;

    int oldInterval = updatesInterval;
    updatesInterval = interval;

    if (updatesScheduled) {
        QDateTime now = currentDateTime();

        // The periodic update interval has been changed and updates are still ocurring.

        if (oldInterval != 0) {
            if (updatesInterval != 0) {
                // If we are changing fixed intervals we update the scheduled time for the next
                // periodic update, making sure that it is scheduled in the future.
                updatesNextTime = updatesNextTime.addMSecs(updatesInterval - oldInterval);
                while (msecsTo(now, updatesNextTime) < 0)
                    updatesNextTime = updatesNextTime.addMSecs(updatesInterval);
            } else {
                // If we now want to emit updates as the data arrives we invalidate the scheduled
                // time for the next update, just to be on the safe side.
                updatesNextTime = now;
            }
        } else {
            // If we were previously emitting updates as the data arrived we set the scheduled time
            // for the next periodic update.
            updatesNextTime = now.addMSecs(updatesInterval);
        }

        wakeUp();
    }
}

void QGeoInfoThreadWinCE::wakeUp()
{
    SetEvent(m_wakeUpEvent);
}

// We try to keep the GPS turned off as much as we can to preserve battery life.
// When run() is called we turn on the GPS device and we leave it on
// until the request is satisfied or periodic updates are stopped.
// The methods requestUpdate() and startUpdates() will call start() if required.
void QGeoInfoThreadWinCE::run()
{
    gpsReachedOnState = false;
    m_gps = GPSOpenDevice(m_newDataEvent, m_gpsStateChange, NULL, 0);

    const int handleCount = 3;
    HANDLE handles[handleCount] = { m_newDataEvent, m_gpsStateChange, m_wakeUpEvent };

    while (true) {
        QMutexLocker locker(&mutex);

        if (!updatesScheduled && !requestScheduled)
            break;

        // If the periodic update is 0 then updates are returned as available.
        // If this is not the case then the next timeout will be set for whichever of
        // the request and periodic updates that is due next.

        // We cap the amount of time we spend waiting for updates.
        DWORD timeout = MaximumMainLoopWaitTime;

        QDateTime now = currentDateTime();

        if (requestScheduled) {
            if (!updatesScheduled || (updatesInterval == 0)
                    || (msecsTo(requestNextTime, updatesNextTime) >= 0)) {
                timeout = msecsTo(now, requestNextTime);
            } else {
                timeout = msecsTo(now, updatesNextTime);
            }
        } else {
            // updatesScheduled has to be true or we wouldn't still be in the larger while loop.
            if (updatesInterval != 0)
                timeout = msecsTo(now, updatesNextTime);
        }

        if (timeout > MaximumMainLoopWaitTime)
            timeout = MaximumMainLoopWaitTime;

        locker.unlock();
        DWORD dwRet = WaitForMultipleObjects(handleCount, handles, FALSE, timeout);
        locker.relock();

        // The GPS data has been updated.
        if (dwRet == WAIT_OBJECT_0) {
            // The other options are:
            // dwRet == WAIT_OBJECT_0 + 1
            //   => The GPS state has been updated.
            // dwRet == WAIT_OBJECT_0 + 2
            //   => We called QGeoInfoThreadWinCE::wakeUp().
            // dwRet == WAIT_TIMEOUT
            //   => WaitForMultipleObjects() timed out.

            GPS_POSITION posn;
            posn.dwVersion = GPS_VERSION_1;
            posn.dwSize = sizeof(posn);

            dwRet = GPSGetPosition(m_gps, &posn, timeout, 0);

            if (dwRet == ERROR_SUCCESS) {
                if (!validator->valid(posn)) {
                    invalidDataReceived = true;
                } else {
                    m_lastPosition = posn;
                    hasLastPosition = true;
                    updateTimeoutTriggered = false;

                    // A request and a periodic update could both be satisfied at once.
                    // We use this flag to prevent a double update.
                    bool emitDataUpdated = false;

                    // If a request is in process we emit the dataUpdated signal.
                    if (requestScheduled) {
                        emitDataUpdated = true;
                        requestScheduled = false;
                    }

                    // If we are updating as data becomes available or if the update period has elapsed
                    // we emit the dataUpdated signal.
                    if (updatesScheduled) {
                        QDateTime now = currentDateTime();
                        if (updatesInterval == 0) {
                            emitDataUpdated = true;
                        } else if (msecsTo(now, updatesNextTime) < 0) {
                            while (msecsTo(now, updatesNextTime) < 0)
                                updatesNextTime = updatesNextTime.addMSecs(updatesInterval);
                            emitDataUpdated = true;
                        }
                    }

                    if (emitDataUpdated) {
                        hasLastPosition = false;
                        emit dataUpdated(m_lastPosition);                    
                    }
                }
            }
        }
        //} else {
        if (dwRet != WAIT_OBJECT_0 || invalidDataReceived) {
            invalidDataReceived = false;

            // Third party apps may have the ability to turn off the gps hardware independently of
            // the Microsoft GPS API.
            // This checks for an unexpected power down and turns the hardware back on.

            // The GPS state has been updated.
            if (dwRet == WAIT_OBJECT_0 + 1) {
                GPS_DEVICE device;
                device.dwVersion = GPS_VERSION_1;
                device.dwSize = sizeof(device);

                dwRet = GPSGetDeviceState(&device);

                if (device.dwDeviceState == SERVICE_STATE_ON) {
                    gpsReachedOnState = true;
                } else if ((device.dwDeviceState == SERVICE_STATE_OFF) && gpsReachedOnState) {
                    // We do not want to mess with devices that are slow starting up, so we only
                    // turn on devices that have previously reached the "On" state.
                    gpsReachedOnState = false;
                    m_gps = GPSOpenDevice(m_newDataEvent, m_gpsStateChange, NULL, 0);
                }
            }

            // We reach this point if the gps state has changed, if the wake up event has been
            // triggered, if we received data we were not interested in from the GPS, 
            // or if a timeout occurred while waiting for gps data.
            //
            // In all of these cases we should check for request and periodic update timeouts.

            QDateTime now = currentDateTime();

            // Check for request timeouts.
            if (requestScheduled && msecsTo(now, requestNextTime) < 0) {
                requestScheduled = false;
                emit updateTimeout();
            }

            // Check to see if a periodic update is due.
            if (updatesScheduled && updatesInterval != 0 && msecsTo(now, updatesNextTime) < 0) {
                while (msecsTo(now, updatesNextTime) < 0)
                    updatesNextTime = updatesNextTime.addMSecs(updatesInterval);
                if (hasLastPosition) {
                    hasLastPosition = false;
                    emit dataUpdated(m_lastPosition);                    
                } else {
                    if (timeoutsForPeriodicUpdates && !updateTimeoutTriggered) {
                        updateTimeoutTriggered = true;
                        emit updateTimeout();                    
                    }
                }
            }
        }
    }

    GPSCloseDevice(m_gps);
}

#include "moc_qgeoinfothread_wince_p.cpp"
QTM_END_NAMESPACE
