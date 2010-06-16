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
***************************************************************************/

#ifndef QDECLARATIVETELEPHONY_H
#define QDECLARATIVETELEPHONY_H

#include <QtCore>
#include <qdeclarative.h>
#include <qdeclarativelist.h>
#include <qtelephonycalllist.h>

QT_USE_NAMESPACE
QTM_USE_NAMESPACE

class QTelephonyCallInfoWrapper : public QObject {
    Q_OBJECT
    Q_ENUMS(CallType CallStatus)
    Q_PROPERTY(QString callIdentifier READ callIdentifier)
    Q_PROPERTY(QList<quint32> contacts READ contacts)
    Q_PROPERTY(CallType type READ type)
    Q_PROPERTY(CallStatus status READ status)

public:
    enum CallType {
        Unknown = QTelephonyCallInfo::Unknown,
        Any = QTelephonyCallInfo::Any,
        Voip = QTelephonyCallInfo::Voip,
        Voice = QTelephonyCallInfo::Voice,
        Video = QTelephonyCallInfo::Video
    };

    enum CallStatus {
        Undefined = QTelephonyCallInfo::Undefined,
        NoCall = QTelephonyCallInfo::NoCall,
        Ringing = QTelephonyCallInfo::Ringing,
        InProgress = QTelephonyCallInfo::InProgress,
        OnHold = QTelephonyCallInfo::OnHold,
        Dropped = QTelephonyCallInfo::Dropped
    };

    QTelephonyCallInfoWrapper();
    QTelephonyCallInfoWrapper(const QTelephonyCallInfo& other);
    QTelephonyCallInfoWrapper(const QTelephonyCallInfoWrapper& other);
    virtual ~QTelephonyCallInfoWrapper();

protected:
    QString callIdentifier() const;
    QList<quint32> contacts() const;

    CallType type() const;
    CallStatus status() const;
private:
    QTelephonyCallInfo* d;
};
QML_DECLARE_TYPE(QTelephonyCallInfoWrapper)

class QTelephonyCallListWrapper : public QObject {
    Q_OBJECT

public:
    QTelephonyCallListWrapper();
    virtual~QTelephonyCallListWrapper();

Q_SIGNALS:
    void activeCallStatusChanged(const QTelephonyCallInfoWrapper& call);
    void activeCallRemoved(const QTelephonyCallInfoWrapper& call);
    void activeCallAdded(const QTelephonyCallInfoWrapper& call);
public:
    QList<QTelephonyCallInfoWrapper> activeCalls(const QTelephonyCallInfoWrapper::CallType& calltype) const;

private slots:
    void activeCallStatusChanged(const QTelephonyCallInfo& call);
    void activeCallRemoved(const QTelephonyCallInfo& call);
    void activeCallAdded(const QTelephonyCallInfo& call);

private:
    QTelephonyCallList* d;
};

QML_DECLARE_TYPE(QTelephonyCallListWrapper)

#endif //QDECLARATIVETELEPHONY_H

