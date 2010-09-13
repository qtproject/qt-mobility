/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

class QDeclarativeTelephonyCallInfo : public QObject {
    Q_OBJECT
    Q_ENUMS(CallStatus Direction)
    Q_FLAGS(CallType)
    Q_PROPERTY(QString remotePartyIdentifier READ remotePartyIdentifier)
    Q_PROPERTY(unsigned int type READ type)
    Q_PROPERTY(CallStatus status READ status)

public:

    CALLTYPEENUM
    CALLSTATUSENUM
    CALLDIRECTIONENUM

    QDeclarativeTelephonyCallInfo();
    QDeclarativeTelephonyCallInfo(const QTelephonyCallInfo& other);
    QDeclarativeTelephonyCallInfo(const QDeclarativeTelephonyCallInfo& other);
    virtual ~QDeclarativeTelephonyCallInfo();
    QDeclarativeTelephonyCallInfo& operator=(const QDeclarativeTelephonyCallInfo& other);

    QString remotePartyIdentifier() const;
    unsigned int type() const;
    CallStatus status() const;
    QString subType() const;
    QVariant value(const QString& param) const;
    Direction direction() const;

private:
    QTelephonyCallInfo* d;
};
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeTelephonyCallInfo))

class QDeclarativeTelephonyCallList : public QObject {
    Q_OBJECT
    Q_PROPERTY(int activeCallCount READ activeCallCount)

public:
    QDeclarativeTelephonyCallList();
    virtual~QDeclarativeTelephonyCallList();

Q_SIGNALS:
    void activeCallStatusChanged(QDeclarativeTelephonyCallInfo* call);
    void activeCallRemoved(QDeclarativeTelephonyCallInfo* call);
    void activeCallAdded(QDeclarativeTelephonyCallInfo* call);
    void hasActiveCalls(bool value);

protected:
    int activeCallCount() const;
    Q_INVOKABLE QList< QDeclarativeTelephonyCallInfo* > activeCalls(const QDeclarativeTelephonyCallInfo::CallType& calltype);

private slots:
    void onCallStatusChanged(const QTelephonyCallInfo& call);
    void onCallRemoved(const QTelephonyCallInfo& call);
    void onCallAdded(const QTelephonyCallInfo& call);
    void onActiveCalls(bool value);

private:
    QTelephonyCallList* d;
    QList<QDeclarativeTelephonyCallInfo> tmpcallinfolist;
};

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeTelephonyCallList))

#endif //QDECLARATIVETELEPHONY_H

