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

#ifndef QTELEPHONYCALLLIST_H
#define QTELEPHONYCALLLIST_H

#include "qmobilityglobal.h"
#include "qtelephonycallinfo.h"

#include <QObject>
#include <QList>
#include <QString>
#include <QVariant>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QTelephonyCallListPrivate;
class QTelephonyCallInfo;
class QTelephonyCallInfoProperty;
class Q_TELEPHONY_EXPORT QTelephonyCallList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QTelephonyCallInfoProperty* currentCall READ currentCallProperty)
    Q_PROPERTY(QList<QTelephonyCallInfoProperty* > calls READ callsProperty)
public:
    QTelephonyCallList(QObject *parent = 0);
    virtual ~QTelephonyCallList();
Q_SIGNALS:
    void callstatusChanged(const QTelephonyCallInfo::CallStatus& status);
    void callsChanged();
public:
    QTelephonyCallInfo* currentCall() const;
    QList<QTelephonyCallInfo* > calls() const;
    QList<QTelephonyCallInfo* > calls(const QTelephonyCallInfo::CallType& calltype) const;
    QList<QTelephonyCallInfo* > calls(const QTelephonyCallInfo::CallStatus& callstatus) const;
    QList<QTelephonyCallInfo* > calls(const QTelephonyCallInfo::CallType& calltype, const QTelephonyCallInfo::CallStatus& callStatus) const;
protected:
    QTelephonyCallInfoProperty* currentCallProperty() const;
    QList<QTelephonyCallInfoProperty* > callsProperty() const;
private:
    QTelephonyCallListPrivate *d;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif /*QTELEPHONYCALLLIST_H*/

// End of file

