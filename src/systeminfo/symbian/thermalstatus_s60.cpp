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

#include "thermalstatus_s60.h"


CThermalStatus::CThermalStatus() : CActive(EPriorityStandard),
    m_currentthermalstatus(ENormal),m_previousthermalstatus(ENormal)
{
    TRACES(qDebug() << "CThermalStatus::CThermalStatus::Constructor-Start");
    CActiveScheduler::Add(this);

    TInt err = m_thermalstateProperty.Attach(KTMPropertyCategory, ETMPropertyName);
    if (err == KErrNone)
     {
        m_thermalstateProperty.Get(m_currentthermalData);
        if (m_currentthermalData.Length() > 0)
         {
          //Initialize the values
          m_previousthermalstatus = m_currentthermalData[0]; //First byte
          m_currentthermalstatus  = m_previousthermalstatus;
          TRACES(qDebug() << "Initial thermal values:" << m_currentthermalstatus);
         }
        startMonitoring();
     }
     else
     {
        TRACES(qDebug() << "CThermalStatus::CThermalStatus::Constructor-Property Attach Error" << err);
        m_currentthermalstatus  = KThermalerror;
        m_previousthermalstatus = KThermalerror;
     }
    TRACES(qDebug() << "CThermalStatus::CThermalStatus::Constructor-End");
}

CThermalStatus::~CThermalStatus()
{
  TRACES(qDebug() << "CThermalStatus::~CThermalStatus()";)
}

void CThermalStatus::addObserver(MThermalStatusObserver *observer)
{
  m_observers.append(observer);
}

void CThermalStatus::removeObserver(MThermalStatusObserver *observer)
{
  m_observers.removeOne(observer);
  if (m_observers.isEmpty())
   {
    stopMonitoring();
   }
}

void CThermalStatus::DoCancel()
{
  m_thermalstateProperty.Cancel();
}

void CThermalStatus::RunL()
{
  TRACES(qDebug() << "CThermalStatus::RunL()-Start";)
  if (iStatus.Int() != KErrNone) { TRACES(qDebug() << "CThermalStatus::RunL() Error:" << iStatus.Int()); return; }

  TInt err = m_thermalstateProperty.Get(m_currentthermalData);
  if ( err == KErrNone && m_currentthermalData.Length() > 0 )
    {
     m_currentthermalstatus = m_currentthermalData[0]; //First byte

     TRACES(qDebug() << "Previous thermal value:" << m_previousthermalstatus;)
     TRACES(qDebug() << "Current  thermal value :" << m_currentthermalstatus;)

     if ( m_currentthermalstatus != m_previousthermalstatus )
      {
        foreach (MThermalStatusObserver *observer, m_observers)
         observer->NotiftythermalStateChanged(m_currentthermalstatus);
      }
    }

  startMonitoring();

  TRACES(qDebug() << "CThermalStatus::RunL()-End";)
}

void CThermalStatus::startMonitoring()
{
 TRACES(qDebug() << "CThermalStatus::startMonitoring-Start";)
 if (!IsActive())
    {
    m_thermalstateProperty.Subscribe(iStatus);
    SetActive();
    }
 TRACES(qDebug() << "CThermalStatus::startMonitoring-End";)
}

void CThermalStatus::stopMonitoring()
 {
  TRACES(qDebug() << "CThermalStatus::stopMonitoring()-Start";)
  if (IsActive())
   {
    Cancel();
    m_thermalstateProperty.Close();
   }
  TRACES(qDebug() << "CThermalStatus::stopMonitoring()-End";)
 }

TUint8 CThermalStatus::getThermalStatus()
{
 TRACES(qDebug() << "CThermalStatus::getThermalStatus" << m_currentthermalstatus;)
 return m_currentthermalstatus;
}
