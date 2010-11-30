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


/*!
  \class QSystemAlignedTimer
  \ingroup systeminfo
  \inmodule QtSystemInfo
  \brief The QSystemAlignedTimer class provides a service for applications to synchronize their activity.

  Applications that must do periodic activity - after being in sleep mode a certain period - do
  that at the same time. For example send network "alive" messages at the same time (i.e. turn the
  wireless radio on at the same time).

  The service is not only for network-aware applications, it is for use by any applications
  that need to periodic wake-ups.

*/

/*!
   \fn QSystemBatteryInfo::QSystemAlignedTimer(QObject *parent)
   Constructs a QSystemAlignedTimer object with the given \a parent.
  */
QSystemAlignedTimer::QSystemAlignedTimer(QObject *parent) :
    QObject(parent),d(parent)
{
}

/*!

*/

QTime QSystemAlignedTimer::requestTimer(QTime minInterval, QTime maxInterval)
{

}

/*!

*/
QTime QSystemAlignedTimer::requestTimer(ushort optimalInterval, ushort timerWindow)
{

}


/*!\internal
This function is called when the client sconnects to signals.

\sa connectNotify()
*/

void QSystemAlignedTimer::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            hearbeat())))) {
        connect(d,SIGNAL(hearbeat()),this,SIGNAL(hearbeat()),Qt::UniqueConnection);

    }
}
/*!
 Called if the application woke up by itself.
 This method should be called if the application  has woken up by some other
 method than via system heartbeat to prevent unnecessary wakeup signals.

True if success, false if error
  */
bool QSystemAlignedTimer::wokeUp()
{
    return false;
}


/*!
 \internal
 This function is called when the client disconnects to signals.

 \sa disconnectNotify()
*/

void QSystemAlignedTimer::disconnectNotify(const char *signal)
{
    Q_UNUSED(signal)
}


QSystemAlignedTimer::WaitMode QSystemAlignedTimer::waitMode()
{

}

bool QSystemAlignedTimer::setWaitMode(QSystemAlignedTimer::WaitMode)
{

}


QSystemAlignedTimer::SignalNeed QSystemAlignedTimer::signalNeeded()
{

}

bool QSystemAlignedTimer::setSignalNeeded(QSystemAlignedTimer::SignalNeed)
{

}


void QSystemAlignedTimerPrivate::timerEvent(QTimerEvent *)
{

}
