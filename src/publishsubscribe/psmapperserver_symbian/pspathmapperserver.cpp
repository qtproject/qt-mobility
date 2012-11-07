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
#include "pspathmapperserver_p.h"
#include "clientservercommon.h"
#include "pspathmappersession_p.h"
#include <QCoreApplication>
#include <QDebug>

QTM_BEGIN_NAMESPACE

static TInt Timeout(TAny *aObject);

const TInt CPSPathMapperServer::timeoutInterval = 30000000; // 30 seconds

TInt Timeout(TAny *aObject)
{
    ((CPSPathMapperServer *) aObject)->Shutdown();
    return 1;
}

CPSPathMapperServer::CPSPathMapperServer() :
    CServer2(EPriorityNormal, ESharableSessions), iSessionCount(0)
{
    iPeriodic = CPeriodic::NewL(0);
    iPeriodic->Start(timeoutInterval, timeoutInterval, TCallBack(Timeout, this));
}

CSession2 *CPSPathMapperServer::NewSessionL(const TVersion &aVersion, const RMessage2 & /*aMessage*/) const
{
    if (!User::QueryVersionSupported(TVersion(KServerMajorVersionNumber, KServerMinorVersionNumber,
        KServerBuildVersionNumber), aVersion)) {
        User::Leave( KErrNotSupported);
    }

    return new (ELeave) CPSPathMapperServerSession(iPathMapper,
        *const_cast<CPSPathMapperServer*> (this));
}

void CPSPathMapperServer::PanicServer(TPSPathMapperServerPanic aPanic)
{
    _LIT(KTxtServerPanic, "Path mapper server panic");
    User::Panic(KTxtServerPanic, aPanic);
}

void CPSPathMapperServer::IncreaseSessions()
{
    iSessionCount++;
    iPeriodic->Cancel();
}

void CPSPathMapperServer::DecreaseSessions()
{
    if (iSessionCount > 0)
        iSessionCount--;
    if (iSessionCount == 0) {
        iPeriodic->Start(timeoutInterval, timeoutInterval, TCallBack(Timeout, this));
    }
}

void CPSPathMapperServer::Shutdown()
{
    QCoreApplication::exit(0);
}

QTM_END_NAMESPACE

// End of File
