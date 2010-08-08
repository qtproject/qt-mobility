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
#include "databasemanagerserver.h"
#include "clientservercommon.h"
#include "databasemanagersession.h"
#include <QCoreApplication>
//#include <QThread>

QTM_BEGIN_NAMESPACE

static TInt Timeout(TAny *aObject);

const TInt CDatabaseManagerServer::timeoutInterval = 30000000; // 30 seconds

TInt Timeout(TAny *aObject)
  {
    ((CDatabaseManagerServer *)aObject)->Shutdown();
    return 1;
  }

CDatabaseManagerServer::CDatabaseManagerServer()
    : CServer2(EPriorityNormal, ESharableSessions)
    , iSessionCount(0)
    {
    iPeriodic = CPeriodic::NewL(0);
    iPeriodic->Start(timeoutInterval, timeoutInterval, TCallBack(Timeout, this));
    }

CSession2* CDatabaseManagerServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
    {
        if (!User::QueryVersionSupported(TVersion(KServerMajorVersionNumber, 
                KServerMinorVersionNumber, KServerBuildVersionNumber), aVersion))
            {
            User::Leave(KErrNotSupported);
            }
        
        return CDatabaseManagerServerSession::NewL(*const_cast<CDatabaseManagerServer*>(this));
    }

void CDatabaseManagerServer::PanicServer(TDatabaseManagerSerververPanic aPanic)
    {
    _LIT(KTxtServerPanic,"Database manager server panic");
    User::Panic(KTxtServerPanic, aPanic);
    }

void CDatabaseManagerServer::IncreaseSessions()
    {
    iSessionCount++;
    iPeriodic->Cancel();
    }

void CDatabaseManagerServer::DecreaseSessions()
    {
    iSessionCount--;
    if (iSessionCount <= 0)
        {
        iPeriodic->Start(timeoutInterval, timeoutInterval, TCallBack(Timeout, this));        
        }
    }

void CDatabaseManagerServer::Shutdown()
  {  
  QCoreApplication::exit(0);
  }

QTM_END_NAMESPACE
// End of File
