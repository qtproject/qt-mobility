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
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativelist.h>
#include <qtelephony.h>
#include <qtelephonycalllist.h>

QT_USE_NAMESPACE
QTM_USE_NAMESPACE

class QTelephonyCallInfoWrapper : public QObject {
    Q_OBJECT
    Q_ENUMS(CallType CallStatus Direction)
    Q_PROPERTY(QString remotePartyIdentifier READ remotePartyIdentifier)
    Q_PROPERTY(CallType type READ type)
    Q_PROPERTY(CallStatus status READ status)

public:

    enum CallType {
        Any = QTelephony::Any,
        Text = QTelephony::Text,
        Data = QTelephony::Data,
        Video = QTelephony::Video,
        Voice = QTelephony::Voice,
        Other = QTelephony::Other,
    };

    enum CallStatus {
        Idle = QTelephony::Idle,
        Dialing = QTelephony::Dialing,
        Alerting = QTelephony::Alerting,
        Connected = QTelephony::Connected,
        Disconnecting = QTelephony::Disconnecting,
        Incoming = QTelephony::Incoming,
        OnHold = QTelephony::OnHold
    };

    enum Direction {
        Received = QTelephony::Received,
        Dialed = QTelephony::Dialed
    };

    QTelephonyCallInfoWrapper();
    QTelephonyCallInfoWrapper(const QTelephonyCallInfo& other);
    QTelephonyCallInfoWrapper(const QTelephonyCallInfoWrapper& other);
    virtual ~QTelephonyCallInfoWrapper();
    QTelephonyCallInfoWrapper& operator=(const QTelephonyCallInfoWrapper& other);


    QString remotePartyIdentifier() const;
    CallType type() const;
    CallStatus status() const;
    QString subType() const;
    QVariant value(const QString& param) const;
    Direction direction() const;

private:
    QTelephonyCallInfo* d;
};

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QTelephonyCallInfoWrapper));

class QTelephonyCallListWrapper : public QObject {
    Q_OBJECT

public:
    QTelephonyCallListWrapper();
    virtual~QTelephonyCallListWrapper();

Q_SIGNALS:
    void evActiveCallStatusChanged(QTelephonyCallInfoWrapper* call);
    void evActiveCallRemoved(QTelephonyCallInfoWrapper* call);
    void evActiveCallAdded(QTelephonyCallInfoWrapper* call);
public:
    QList<QTelephonyCallInfoWrapper> activeCalls(const QTelephonyCallInfoWrapper::CallType& calltype) const;

private slots:
    void onCallStatusChanged(const QTelephonyCallInfo& call);
    void onCallRemoved(const QTelephonyCallInfo& call);
    void onCallAdded(const QTelephonyCallInfo& call);

private:
    QTelephonyCallList* d;
};

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QTelephonyCallListWrapper))

#endif //QDECLARATIVETELEPHONY_H

