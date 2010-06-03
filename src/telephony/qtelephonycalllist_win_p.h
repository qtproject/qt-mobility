/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QTELEPHONY_WIN_H
#define QTELEPHONY_WIN_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include <QObject>
#include "qtelephonycalllist.h"
#include "qtelephonycallinfo.h"
#include "qmobilityglobal.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QTelephonyCallListPrivate : public QObject
{
    Q_OBJECT
Q_SIGNALS:
    void callstatusChanged(const QTelephonyCallInfo::CallStatus);
    void callsChanged();
public:
    QTelephonyCallListPrivate(QObject *parent = 0);
    virtual ~QTelephonyCallListPrivate();
    QTelephonyCallInfo* currentCall() { return 0; }
    QTelephonyCallInfo::CallStatus currentCallStatus() { return QTelephonyCallInfo::UnknownStatus; }
    QList<QTelephonyCallInfo*> calls() { return calllist; }
private:
    QList<QTelephonyCallInfo*> calllist;
};

class QTelephonyCallInfoPrivate : public QObject
{
    Q_OBJECT
public:
    QTelephonyCallInfoPrivate(QObject *parent = 0);
    virtual ~QTelephonyCallInfoPrivate();
    QString callIdentifier();
    QList<quint32> contacts();
    QTelephonyCallInfo::CallType type() { return QTelephonyCallInfo::UnknownType; }
    QTelephonyCallInfo::CallStatus status() { return QTelephonyCallInfo::UnknownStatus; }

public: //Declaration of properties (just an example)
    Q_PROPERTY(int contactBufferSize READ contactBufferSize)
    int contactBufferSize() const { return 124; };
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif

// End of file

