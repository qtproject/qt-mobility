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

#include "cntabstractsimrequest.h"
#include "cntsymbiansimengine.h"
#include "cntsimstore.h"
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

const int KRetryCount = 10;
const int KRetryTime = 100; // in ms

CntAbstractSimRequest::CntAbstractSimRequest(CntSymbianSimEngine *engine)
    :QObject(engine),
    m_timer(0),
    m_retryCount(0)
{
    
}

bool CntAbstractSimRequest::waitAndRetry()
{
    if (m_retryCount < KRetryCount)
    {
        singleShotTimer(KRetryTime, this, SLOT(retry()));
        m_retryCount++;
        qDebug() << "retrying request" << m_retryCount;
        return true;
    }
    return false;
}

void CntAbstractSimRequest::singleShotTimer(int msec, QObject *receiver, const char *member)
{
    // We could use QTimer::singleShot but there is no way to cancel it...
    delete m_timer;
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), receiver, member);
    m_timer->start(msec);
}

void CntAbstractSimRequest::cancelTimer()
{
    delete m_timer;
    m_timer = 0;
}

CntSymbianSimEngine *CntAbstractSimRequest::engine()
{
    return static_cast<CntSymbianSimEngine*>(parent());
}

CntSimStore *CntAbstractSimRequest::simStore()
{
    return engine()->simStore();
}
