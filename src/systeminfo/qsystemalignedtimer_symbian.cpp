/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsystemalignedtimer_symbian_p.h"
#include "trace.h"


QTM_BEGIN_NAMESPACE

QSystemAlignedTimerPrivate::QSystemAlignedTimerPrivate(QObject *parent) :
    QObject(parent),
    m_lastError(QSystemAlignedTimer::NoError),
    m_minimumInterval(0),
    m_maximumInterval(0),
    m_running(false),
    m_singleShot(false), m_singleShotReceiver(0),m_singleShotMember(0),m_heartbeattimer(0)
{
    TRACES(qDebug() << "QSystemAlignedTimerPrivate--Constructor Start");
    m_heartbeattimer = CHeartbeatTimer::NewL();
    m_heartbeattimer->addObserver(this);
    TRACES(qDebug() << "QSystemAlignedTimerPrivate--Constructor End");
}

QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate()
{
 TRACES(qDebug() << "QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate-Start");

 if (m_heartbeattimer ) {
    m_heartbeattimer->removeObserver(this);
    delete m_heartbeattimer;
    m_heartbeattimer = 0;
   }

 TRACES(qDebug() << "QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate-End");
}

void QSystemAlignedTimerPrivate::wokeUp()
{
 TRACES(qDebug() << "QSystemAlignedTimerPrivate::wokeUp-Start");

    if (m_singleShot) {
     //if singleshot stop the timer
        TRACES(qDebug() << "QSystemAlignedTimerPrivate::wokeUp-Timer is single shot");
        stop();
        return;
    }

    //If not singleshot resetting the timer
    m_heartbeattimer->ResetTimer();
}

int QSystemAlignedTimerPrivate::minimumInterval() const
{
    return m_minimumInterval;
}

void QSystemAlignedTimerPrivate::setMinimumInterval(int seconds)
{
    m_minimumInterval = seconds;
}

int QSystemAlignedTimerPrivate::maximumInterval() const
{
    return m_maximumInterval;
}

void QSystemAlignedTimerPrivate::setMaximumInterval(int seconds)
{
    m_maximumInterval = seconds;
}

void QSystemAlignedTimerPrivate::setSingleShot(bool singleShot)
{
    m_singleShot = singleShot;
    m_heartbeattimer->setsingleShot(singleShot);
}

bool QSystemAlignedTimerPrivate::isSingleShot() const
{
    return m_singleShot;
}

void QSystemAlignedTimerPrivate::singleShot(int minimumTime, int maximumTime, QObject *receiver, const char *member)
{
   TRACES(qDebug() << "QSystemAlignedTimerPrivate::singleShot--Static");
   QSystemAlignedTimerPrivate *alignedTimer = NULL;
   alignedTimer = new QSystemAlignedTimerPrivate();

   if (alignedTimer){
        TRACES(qDebug() << "QSystemAlignedTimerPrivate::singleShot--Issuing request,Wait for SingleShot() callback");
        alignedTimer->m_singleShotReceiver = receiver;
        alignedTimer->m_singleShotMember = member;
        alignedTimer->start(minimumTime, maximumTime);
       }
}

QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimerPrivate::lastError() const
{
    return m_lastError;
}

// public slots

void QSystemAlignedTimerPrivate::start(int minimumTime, int maximumTime)
{
    m_minimumInterval = minimumTime;
    m_maximumInterval = maximumTime;

    start();
}

void QSystemAlignedTimerPrivate::start()
{
    if (m_running) {
        TRACES(qDebug() << "QSystemAlignedTimerPrivate::start()-Already Started, return");
        return;
    }

    if ( m_minimumInterval < 0 || m_maximumInterval < 0 ) {
      m_lastError = QSystemAlignedTimer::InvalidArgument;
      emit error(m_lastError);
      return;
     }

    //Arguments are valid, Go ahead to start the timer
    TTimeIntervalMicroSeconds window   = (m_maximumInterval - m_minimumInterval)* 1000 * 1000;
    TTimeIntervalMicroSeconds interval = m_maximumInterval * 1000 * 1000;
    TRACES(qDebug() << "QSystemAlignedTimerPrivate::start()-Issuing request");
    TRACES(qDebug() << "winodow:"<< window.Int64());
    TRACES(qDebug() << "Interval:"<< interval.Int64());
    m_heartbeattimer->StartTimer( window, interval );

    m_running = true;
    m_lastError = QSystemAlignedTimer::NoError;
}

void QSystemAlignedTimerPrivate::stop()
{
    if (!m_running) {
        return;
    }
    TRACES(qDebug() << "QSystemAlignedTimerPrivate::stop()-Issuing stop request");
    //Do heartbeattimer here
    m_heartbeattimer->StopTimer();

    m_running = false;
    m_lastError = QSystemAlignedTimer::NoError;
}

void QSystemAlignedTimerPrivate::NotifyheartbeatReceived()
{
  TRACES(qDebug() << "QSystemAlignedTimerPrivate::NotiftyheartbeatReceived()");
   //Check if there are any single shot members.
   //Issuing or (Not to issue) of timer is taken care by CHeartbeatTimer
     if (m_singleShotMember != NULL && m_singleShotMember !=NULL )
      {
       TRACES(qDebug() << "Single shot members exist");
       QMetaObject::invokeMethod(m_singleShotReceiver, m_singleShotMember);
       this->deleteLater();
      }
      else
      {
       emit timeout();
      }
}

bool QSystemAlignedTimerPrivate::isActive () const
{
    return m_running;
}

#include "moc_qsystemalignedtimer_symbian_p.cpp"

QTM_END_NAMESPACE
