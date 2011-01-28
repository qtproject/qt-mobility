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
#ifdef QT_SIMULATOR
QSystemDeviceInfoPrivate *getSystemAlignedTimerPrivate() { return alignedTimerPrivate(); }
#endif

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
  \fn void QSystemAlignedTimer::timeout()

  This signal is emitted when the timer times out.
 */

/*!
    \enum QSystemAlignedTimer::AlignedTimerError
    This enum describes the last known AlignedTimerError

    \value NoError                        No error.
    \value AlignedTimerNotSupported       The aligned timer is not support on this platform
    \value InvalidArgument                Interval arguments are invalid.
    \value TimerFailed                    General timer failure.

  */

/*!
    \enum QSystemAlignedTimer::AlignedTimerError
    This enum describes the last known AlignedTimerError

    \value AlignedTimerNotSupported       The aligned timer is not support on this platform
    \value InvalidArgument                Interval arguments are invalid.
    \value TimerFailed                    General timer failure.

  */
/*!
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

  */
void QSystemAlignedTimer::start(int minimumTime, int maximumTime)
{
    Q_UNUSED(minimumTime)
    Q_UNUSED(maximumTime)
}

/*!
    Starts the alignedtimer.
*/
void QSystemAlignedTimer::start()
{

}


/*!
     This should be called when the application wakes up via other means than QSystemAlignedTimer timeout.

    Other applications that are in their wakeup window may be woken up. Single-shot timer is stopped,
    and reoccuring timer interval will get reset.
    Symbian does not support this wokeUp call for reoccuring timers and will simply ignore it.
  */
void QSystemAlignedTimer::wokeUp()
{
}

/*!
  Stops this timer request.
  */
void QSystemAlignedTimer::stop()
{

}


/*!
  Set the timeout minimum interval to \a seconds in seconds that must be waited before timeout
  signal is emitted.

   Time in seconds that MUST be waited before timeout.
   Value 0 means 'wake me up when someboy else is woken'.
   It  is recommended that the first wait (if possible) uses minimum value of 0 to
   "jump to the train"

 */
void QSystemAlignedTimer::setMinimumInterval(int seconds) const
{
    Q_UNUSED(seconds)
}

/*!
  \property QSystemAlignedTimer::minimumInterval
  \brief The timers's minimumInterval.

  Returns this current timer minimum interval.
  */
int QSystemAlignedTimer::minimumInterval() const
{
    return 0;
}

/*!
  Set the timeout maximum interval to \a seconds the wait must end.

   Time in seconds when the wait MUST end. It is wise to have maxtime-mintime
   quite big so all users of this service get synced.
   For example if you preferred wait is 120 seconds, use minval 110 and maxval 130.
   Default value for maxTime is 0. If maxTime is 0, then minTime will be a "preferred" interval.
 */
void QSystemAlignedTimer::setMaximumInterval(int seconds) const
{
    Q_UNUSED(seconds)

}

/*!
  \property QSystemAlignedTimer::maximumInterval
  \brief The timers's maximumInterval.

  Returns this current timer maximum interval.
  */
int QSystemAlignedTimer::maximumInterval() const
{
    return 0;
}

/*!
  Sets this timer to be a single shot \a singleShot is true, otherwise false.
  */
inline void QSystemAlignedTimer::setSingleShot(bool singleShot)
{
    Q_UNUSED(singleShot);
}

/*!

  This static function calls a slot after a given time interval.
  \a sec is the time interval in seconds.
  The receiver is the \a receiver object and the \a member is the slot.
  */
void QSystemAlignedTimer::singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member)
{
    Q_UNUSED(minimumTime);
    Q_UNUSED(maximumTime);
    Q_UNUSED(receiver);
    Q_UNUSED(member);
}
/*!
  \property QSystemAlignedTimer::singleShot
  \brief Whether the timer is single shot.
*/

/*!
  Returns true if this timer is a single shot, otherwise false.
  */
bool QSystemAlignedTimer::isSingleShot() const
{
    return d->single;
}

/*!
  Returns the last AlignedTimerError.

  */
QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimer::lastError() const
{
    return d->lastTimerError;
}

/*!
   Constructs a QSystemAlignedTimerPrivate object with the given \a parent.
  */
QSystemAlignedTimerPrivate::QSystemAlignedTimerPrivate(QObject *parent)
 : QObject(parent), id(0), isTimerRunning(0), single(0)
{
    lastTimerError = QSystemAlignedTimer::AlignedTimerNotSupported;
}

/*!
  \internal
  */
void QSystemAlignedTimerPrivate::timerEvent(QTimerEvent *)
{

}



#include "moc_qsystemalignedtimer.cpp"

QTM_END_NAMESPACE
