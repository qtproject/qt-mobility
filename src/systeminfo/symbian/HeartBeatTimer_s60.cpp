/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "HeartBeatTimer_s60.h"


CHeartbeatTimer* CHeartbeatTimer::NewL()
 {
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::NewL-Start");
  CHeartbeatTimer* self = new (ELeave) CHeartbeatTimer( CActive::EPriorityStandard );
  CleanupStack::PushL(self);
  self->ConstructL();
  CleanupStack::Pop(self);
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::NewL-End");
  return self;
 }

void CHeartbeatTimer::ConstructL()
 {
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::CreateL-Start");
  CFlexTimer::ConstructL();
  CActiveScheduler::Add(this);
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::CreateL-End");
 }

CHeartbeatTimer::CHeartbeatTimer(TInt aPriority):CFlexTimer(aPriority),
                m_singleShot(false),m_interval(0)

 {
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer");
 }


CHeartbeatTimer::~CHeartbeatTimer()
 {
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::~CHeartbeatTimer-Start");
  TRACES(qDebug() << "CFlextTimer Cancel");
  CFlexTimer::Cancel();
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::~CHeartbeatTimer-End");
 }

void CHeartbeatTimer::addObserver(MHeartBeatObserver *observer)
 {
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::addObserver");
  m_observers.append(observer);
 }

void CHeartbeatTimer::removeObserver(MHeartBeatObserver *observer)
 {
  TRACES(qDebug() << "CHeartbeatTimer::CHeartbeatTimer::removeObserver");
  m_observers.removeOne(observer);
 }

void CHeartbeatTimer::RunL()
 {
   TRACES(qDebug() << "CHeartbeatTimer::RunL()-Start");

   if (iStatus != KErrNone) { TRACES(qDebug() << "CHeartbeatTimer::RunL() Error:" << iStatus.Int());}

   if (iStatus == KErrNone)
    {
      TRACES(qDebug() << "CHeartbeatTimer::RunL()->TimerExpired");
      TRACES(qDebug() << "CHeartbeatTimer::Notify observers Start");

      foreach (MHeartBeatObserver *observer, m_observers)
       {
        TRACES(qDebug() << "observer...:" << observer);
        observer->NotifyheartbeatReceived();
       }

      TRACES(qDebug() << "CHeartbeatTimer::Notify observers End");
      if (m_singleShot == false) {
       TRACES(qDebug() << "Timer is non-single shot");
       CFlexTimer::After(m_interval); //Refresh timer if not singleshot
       }
    }


   TRACES(qDebug() << "CHeartbeatTimer::RunL()-End");
}

void CHeartbeatTimer::StartTimer( TTimeIntervalMicroSeconds aWindow, TTimeIntervalMicroSeconds aInterval)
 {
  TRACES(qDebug() << "CHeartbeatTimer::StartTimer-Start");
  m_interval = aInterval;
  if (IsActive()) {
   TRACES(qDebug() << "CHeartbeatTimer::Timer is already running");
   return;
  }

  //Issue request
  TInt configerr = KErrNone;
  configerr = CFlexTimer::Configure(aWindow);
  if (configerr != KErrNone) TRACES(qDebug() << "CHeartbeatTimer::Configerror:"<<configerr);
  TRACES(qDebug() << "Configure done");
  CFlexTimer::After(aInterval);
  TRACES(qDebug() << "Issued after call");
  TRACES(qDebug() << "iStatus:" << iStatus.Int());
  TRACES(qDebug() << "CHeartbeatTimer::StartTimer-End");
 }

 void CHeartbeatTimer::setsingleShot(bool asingleShot)
 {
  m_singleShot = asingleShot;
 }

void CHeartbeatTimer::StopTimer()
 {
  TRACES(qDebug() << "CHeartbeatTimer::StopTimer-Start");
  if (!IsActive()) {
   TRACES(qDebug() << "CHeartbeatTimer::Timer is currently inactive");
   return;
  }
  TRACES(qDebug() << "iStatus:" << iStatus.Int());
  CFlexTimer::Cancel();
  TRACES(qDebug() << "CHeartbeatTimer::StopTimer-End");
 }

void CHeartbeatTimer::ResetTimer()
 {
  TRACES(qDebug() << "CHeartbeatTimer::ResetTimer-Start");
  if (!IsActive()) {
   TRACES(qDebug() << "CHeartbeatTimer::Timer is currently inactive");
   return;
  }
  TRACES(qDebug() << "iStatus:" << iStatus.Int());
  CFlexTimer::Cancel();
  CFlexTimer::After(m_interval);
  TRACES(qDebug() << "CHeartbeatTimer::ResetTimer-End");
 }
