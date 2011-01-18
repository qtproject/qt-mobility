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


#include "qsystemalignedtimer.h"

QTM_BEGIN_NAMESPACE
Q_GLOBAL_STATIC(QSystemAlignedTimerPrivate, alignedTimerPrivate)

/*!
  \class QSystemAlignedTimer
  \ingroup systeminfo
  \inmodule QtSystemInfo
  \brief The QSystemAlignedTimer class provides a service for applications to synchronize their activity.

  QSystemAlignedTimer is a fuzzy timer that allows applications that must do periodic activity like
  after being in sleep mode a certain period, to syncronize their activities in the same window of time.

  For example send network "alive" messages at the same time (i.e. turn the wireless radio on at the same time).

  The service is not only for network-aware applications, it is for use by any applications
  that need to periodic wake-ups.

*/

/*!
   \fn QSystemBatteryInfo::QSystemAlignedTimer(QObject *parent)
   Constructs a QSystemAlignedTimer object with the given \a parent.
  */
QSystemAlignedTimer::QSystemAlignedTimer(QObject *parent)
    : QObject(parent),d(alignedTimerPrivate())
{
}

/*!
    Destructs the QSystemAlignedTimer
  */
QSystemAlignedTimer::~QSystemAlignedTimer()
{

}

/*!
  Starts the timer with the preferred interval of \a sec in seconds.

  This is not a guaranteed interval, and the timeout signal may be fired at any time,
  depending on other clients attached to this timer.

  In the case of 0, it means 'wake me up when someone else is woken up'.

  If you need a window of time in which your timer should fire, use QSystemAlignedTimer::setWindow

  \sa QSystemAlignedTimer::setWindow

  */
void QSystemAlignedTimer::start(int sec)
{
    Q_UNUSED(sec)
}

/*!
    Starts the alignedtimer.
*/
void QSystemAlignedTimer::start()
{

}

/*!
  Set the timeout window with \a timerWindow in seconds, in which the timer should fire.
  It is recommended to have the window quite big.

  For example if your preferred interval is 120 seconds, and you need the timer to
  fire within 20 seconds of that interval, use a timerWindow of 20.

*/
void QSystemAlignedTimer::setWindow(int timerWindow)
{
    currentTimerWindow = timerWindow;
}



/*!
  Called if the application woke up by itself.
  This method should be called if the application has woken up by some other
  method than via system alignedtimer heartbeat to prevent unnecessary wakeup signals.

  True if success, false if error
  */
bool QSystemAlignedTimer::wokeUp()
{
    return false;
}

/*!
  Stops this timer request.
  */
void QSystemAlignedTimer::stop()
{

}


/*!
  Set the timeout interval to \a minTime in seconds that must be waited before timeout
  signal is emitted, and \a maxTime in seconds when the wait must end.

  For example if you preferred wait is 120 seconds, use minTime of 110 and maxTime of 130.

  minTime value of 0 means 'wake me up when someone else is woken'.

  Default value for maxTime is 0. If maxTime is 0, then minTime will be a "preferred" interval.
 */
void QSystemAlignedTimer::setInterval(int minTime, int maxTime)
{
    if(minTime != 0 && maxTime == 0) {
        preferredInterval = minTime;
    }
}

/*!
  Returns the current preferred timer window.
  */
int QSystemAlignedTimer::timerWindow() const
{
    return currentTimerWindow;
}


/*!
  Returns this current timer interval.
  This will usually differ than what was set with \a setInterval.
  */
int QSystemAlignedTimer::interval() const
{
    return preferredInterval;
}

/*!
  */
inline void QSystemAlignedTimer::setSingleShot(bool singleShot)
{
Q_UNUSED(singleShot);
}

/*!
  */
void QSystemAlignedTimer::singleShot(int msec, QObject *receiver, const char *member)
{
    Q_UNUSED(msec);
    Q_UNUSED(receiver);
    Q_UNUSED(member);
}

/*!
  */
void QSystemAlignedTimer::setRunning(bool running)
{
    isTimerRunning = running;
}


/*!
  \internal
  */
void QSystemAlignedTimerPrivate::timerEvent(QTimerEvent *)
{

}


#include "moc_qsystemalignedtimer.cpp"

QTM_END_NAMESPACE
