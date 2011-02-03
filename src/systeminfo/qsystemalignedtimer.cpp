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
#include "qsysteminfocommon_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QSystemAlignedTimer
  \ingroup systeminfo
  \inmodule QtSystemInfo
  \brief The QSystemAlignedTimer class provides a service for applications to synchronize their activity.

  QSystemAlignedTimer is a fuzzy timer that allows applications that must do periodic activity like
  after being in sleep mode a certain period, to synchronize their activities in the same window of time.

  For example send network "alive" messages at the same time (i.e. turn the wireless radio on at the same time).

  The service is not only for network-aware applications, it is for use by any applications
  that need to periodic wake-ups.

  The recommended use case is when app uses single-shot timer only: set mintime 0 for the first call
  'to jump to the train' and mintime > 0 after 1st wakeup.

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
    \value InternalError                  Internal error.

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
    : QObject(parent)
{
    d = new QSystemAlignedTimerPrivate(parent);
    connect(d, SIGNAL(timeout()), this, SIGNAL(timeout()));
    connect(d, SIGNAL(error(QSystemAlignedTimer::AlignedTimerError)), this, SIGNAL(error(QSystemAlignedTimer::AlignedTimerError)));
}

/*!
    Destructs the QSystemAlignedTimer
  */
QSystemAlignedTimer::~QSystemAlignedTimer()
{
    delete d, d = 0;
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
    d->start(minimumTime, maximumTime);
}

/*!
    Starts the alignedtimer.
*/
void QSystemAlignedTimer::start()
{
    d->start();
}

/*!
     This should be called when the application wakes up via other means than QSystemAlignedTimer timeout.

    Other applications that are in their wakeup window *may* be woken up. Single-shot timer is canceled,
    and reoccuring timer interval will restart.

    Symbian does not support this wokeUp call for reoccuring timers and will simply ignore it.
  */
void QSystemAlignedTimer::wokeUp()
{
    d->wokeUp();
}

/*!
  Stops this timer request.
  */
void QSystemAlignedTimer::stop()
{
    d->stop();
}


/*!
  Set the timeout minimum interval to \a seconds in seconds that must be waited before timeout
  signal is emitted.

   Time in seconds that MUST be waited before timeout.
   Value 0 means 'wake me up when someboy else is woken'.

   mintime value 0 should be used with special care, as it may cause too frequent wakeups.
   It is recommended that the first wait (if possible) uses minimum value of 0 to
   "jump to the train" and minimum value > 0 after 1st wakeup

 */
void QSystemAlignedTimer::setMinimumInterval(int seconds)
{
    d->setMinimumInterval(seconds);
}

/*!
  \property QSystemAlignedTimer::minimumInterval
  \brief The timers's minimumInterval.

  Returns this current timer minimum interval.
  */
int QSystemAlignedTimer::minimumInterval() const
{
    return d->minimumInterval();
}

/*!
  Set the timeout maximum interval to \a seconds the wait must end.

   Time in seconds when the wait MUST end. It is wise to have maxtime-mintime
   quite big so all users of this service get synced.
   For example if you preferred wait is 120 seconds, use minval 110 and maxval 130.

   max interval must be greater than min interval.

 */
void QSystemAlignedTimer::setMaximumInterval(int seconds)
{
    d->setMaximumInterval(seconds);
}

/*!
  \property QSystemAlignedTimer::maximumInterval
  \brief The timers's maximumInterval.

  Returns this current timer maximum interval.
  */
int QSystemAlignedTimer::maximumInterval() const
{
    return d->maximumInterval();
}

/*!
  Sets this timer to be a single shot \a singleShot is true, otherwise false.
  */
inline void QSystemAlignedTimer::setSingleShot(bool singleShot)
{
    d->setSingleShot(singleShot);
}

/*!

  This static function calls a slot after a given time interval.
  \a sec is the time interval in seconds.
  The receiver is the \a receiver object and the \a member is the slot.
  */
void QSystemAlignedTimer::singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member)
{
    QSystemAlignedTimerPrivate::singleShot(minimumTime, maximumTime, receiver, member);
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
    return d->isSingleShot();
}

/*!
  Returns the last AlignedTimerError.

  */
QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimer::lastError() const
{
    return d->lastError();
}

/*!
  Returns true if the timer is running; otherwise false.
  */
bool	QSystemAlignedTimer::isActive () const
{
    return d->isActive();
}

#include "moc_qsystemalignedtimer.cpp"

QTM_END_NAMESPACE
