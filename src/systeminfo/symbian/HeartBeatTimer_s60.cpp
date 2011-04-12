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
      foreach (MHeartBeatObserver *observer, m_observers)
       {
        observer->NotifyheartbeatReceived();
       }
      if (m_singleShot == false) CFlexTimer::After(m_interval); //Refresh timer if not singleshot
    }

   if (iStatus == KErrCancel ) {
    TRACES(qDebug() << "CHeartbeatTimer::RunL->TimerCancelled");
    //Take some action ???
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
  TRACES(qDebug() << "CHeartbeatTimer::StopTimer-End");
 }
