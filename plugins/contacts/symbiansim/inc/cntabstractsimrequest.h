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

#ifndef CNTABSTRACTSIMREQUEST_H_
#define CNTABSTRACTSIMREQUEST_H_

#include <QObject>
#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
class QContactAbstractRequest;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class CntSymbianSimEngine;
class CntSimStore;
class QTimer;

#ifdef SYMBIANSIM_BACKEND_USE_DELAY
const int KRequestDelay = 10; // in ms
#else
const int KRequestDelay = 0;
#endif
const int KMaxRetryCount = 10;

class CntAbstractSimRequest : public QObject
{
Q_OBJECT
public:
    CntAbstractSimRequest(CntSymbianSimEngine *engine, QContactAbstractRequest *req);
    
    bool start();

public Q_SLOTS:    
    virtual void run() = 0;
    virtual bool cancel();
        
protected:
    bool waitAndRetry();
    void singleShotTimer(int msec, QObject *receiver, const char *member);
    void cancelTimer();
    CntSymbianSimEngine *engine();
    CntSimStore *simStore();
    QContactAbstractRequest *req() { return m_request; }

    template<class T>
    T *req() { return static_cast<T*>(m_request); }
    
private:
    QContactAbstractRequest *m_request;
    QTimer *m_timer;
    int m_retryCount;
};

#endif // CNTABSTRACTSIMREQUEST_H_
