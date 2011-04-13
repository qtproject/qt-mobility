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

#ifndef HEARTBEATTIMER_S60_H
#define HEARTBEATTIMER_S60_H

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>
#include <QList>
#include "trace.h"
#include <flextimer.h>
#include <QObject>

class MHeartBeatObserver
{
public:
    virtual void NotifyheartbeatReceived() = 0;
};

class CHeartbeatTimer : public CFlexTimer
{
public:
    static CHeartbeatTimer* NewL();
    ~CHeartbeatTimer();

    void addObserver(MHeartBeatObserver *observer);
    void removeObserver(MHeartBeatObserver *observer);
    void StartTimer( TTimeIntervalMicroSeconds aWindow, TTimeIntervalMicroSeconds aInterval);
    void setsingleShot(bool asingleShot);
    void StopTimer();
    void ResetTimer();

protected:  //from CActive
    virtual void RunL();

private:
    void ConstructL();
    CHeartbeatTimer(TInt aPriority);

private:
    bool m_singleShot;
    TTimeIntervalMicroSeconds m_interval;
    QList<MHeartBeatObserver *> m_observers;
    /*The callback function which is called at the completion of flextimer server requests*/
    //TCallBack m_callback;
};


#endif //HEARTBEATTIMER_S60_H
