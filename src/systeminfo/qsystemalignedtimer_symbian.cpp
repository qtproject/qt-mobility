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
    TRACES(qDebug() << "QSystemAlignedTimerPrivate--Constructor End");
}

QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate()
{
  TRACES(qDebug() << "QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate-Start");
  TRACES(qDebug() << "QSystemAlignedTimerPrivate::~QSystemAlignedTimerPrivate-End");
}

void QSystemAlignedTimerPrivate::wokeUp()
{
 TRACES(qDebug() << "QSystemAlignedTimerPrivate::wokeUp-Start");
 if ( !m_heartbeattimer ) return;

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
    TRACES(qDebug() << "SetMaxInterval:" << m_maximumInterval);
}

void QSystemAlignedTimerPrivate::setSingleShot(bool singleShot)
{
    m_singleShot = singleShot;
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
        alignedTimer->setSingleShot(true);
        alignedTimer->start(minimumTime, maximumTime);
       }
}

QSystemAlignedTimer::AlignedTimerError QSystemAlignedTimerPrivate::lastError() const
{
 TRACES(qDebug() << "QSystemAlignedTimerPrivate::lastError:" << m_lastError);
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
 TRACES(qDebug() << "QSystemAlignedTimerPrivate::start+");
    if (m_running) {
        TRACES(qDebug() << "QSystemAlignedTimerPrivate::start()-Already Started, return");
        return;
    }

    TRACES(qDebug() << "minimum interval:" << m_minimumInterval);
    TRACES(qDebug() << "minimum interval:" << m_maximumInterval);

    if ( m_minimumInterval < 0 || m_maximumInterval < 0 ) {
      TRACES(qDebug() << "m_minimumInterval < 0 || m_maximumInterval < 0 , return");
      m_lastError = QSystemAlignedTimer::InvalidArgument;
      emit error(m_lastError);
      return;
     }

    if ( !m_heartbeattimer ) {
     m_heartbeattimer = CHeartbeatTimer::NewL();
     m_heartbeattimer->setsingleShot(m_singleShot);
     m_heartbeattimer->addObserver(this);
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
 TRACES(qDebug() << "QSystemAlignedTimerPrivate::start-");
}

void QSystemAlignedTimerPrivate::stop()
{
    if (!m_running) {
        return;
    }

    if ( m_heartbeattimer ) {
       TRACES(qDebug() << "QSystemAlignedTimerPrivate::stop()-Issuing stop request");
       //Do heartbeattimer here
       m_heartbeattimer->StopTimer();
       m_heartbeattimer->removeObserver(this);
       delete m_heartbeattimer;
       m_heartbeattimer = 0;
    }

    m_running = false;
    m_lastError = QSystemAlignedTimer::NoError;
}

void QSystemAlignedTimerPrivate::NotifyheartbeatReceived()
{
  TRACES(qDebug() << "QSystemAlignedTimerPrivate::NotiftyheartbeatReceived()");
   //Check if there are any single shot members.
   //Issuing or (Not to issue) of timer is taken care by CHeartbeatTimer
   if (m_singleShot) m_running = false;
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
 TRACES(qDebug() << "QSystemAlignedTimerPrivate::isActive+");
 TRACES(qDebug() << "State :" << m_running);
    return m_running;
}

#include "moc_qsystemalignedtimer_symbian_p.cpp"

QTM_END_NAMESPACE
