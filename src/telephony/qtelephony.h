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

#ifndef QTELEPHONY_H
#define QTELEPHONY_H

#include "qmobilityglobal.h"
#include "qcallinfo.h"

#include <QObject>
#include <QList>
#include <QString>
#include "../contacts/qtcontactsglobal.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QTelephonyCallListPrivate;
class QCallInfo;
class QTelephonyCallList : public QObject
{
    Q_OBJECT
public:
    QTelephonyCallList(QObject *parent = 0);
    virtual ~QTelephonyCallList();
Q_SIGNALS:
    void callstatusChanged(const QCallInfo::CallStatus status);
    void callsChanged();
public:
    Q_PROPERTY(QList<QCallInfo*> calls READ calls)
    QList<QCallInfo*> calls() const;
    Q_INVOKABLE QList<QCallInfo*> calls(const QCallInfo::CallType& calltype) const;
    Q_INVOKABLE QList<QCallInfo*> calls(const QCallInfo::CallStatus& callstatus) const;
    Q_INVOKABLE QList<QCallInfo*> calls(const QCallInfo::CallType& calltype, const QCallInfo::CallStatus& callStatus) const;
    //can't do Q_PROPERTY because of QCallInfo::CallStatus created parse error in moc
    Q_INVOKABLE QCallInfo::CallStatus currentCallStatus();
    Q_PROPERTY(QCallInfo* currentCall READ currentCall)
    QCallInfo* currentCall();
private:
    QTelephonyCallListPrivate *d;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif /*QTELEPHONY_H*/

// End of file

