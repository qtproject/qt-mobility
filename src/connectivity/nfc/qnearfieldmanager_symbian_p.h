/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QNEARFIELDMANAGER_SYMBIAN_P_H_
#define QNEARFIELDMANAGER_SYMBIAN_P_H_


#include "qnearfieldmanager_p.h"
#include "qnearfieldtarget.h"
#include "qndeffilter.h"

#include <QtCore/QObject>
#include <QtCore/QMetaMethod>
#include <QPointer>
#include <QList>
#include <qremoteserviceregister.h>

class CNearFieldManager;
class CNdefMessage;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class Proxy : public QObject
{
    Q_OBJECT
public:
    Proxy(QObject* parent = 0);

Q_SIGNALS:
    void handleMessage(const QNdefMessage& message);
};

class ContentHandlerInterface : public QObject
{
    Q_OBJECT
public:
    ContentHandlerInterface(QObject* parent = 0);

public slots:
    void handleMessage(const QByteArray& message);
};

class QNearFieldManagerPrivateImpl : public QNearFieldManagerPrivate
{
    Q_OBJECT

public:
    QNearFieldManagerPrivateImpl();
    ~QNearFieldManagerPrivateImpl();

    bool isAvailable() const;

    int registerNdefMessageHandler(QObject *object, const QMetaMethod &method);
    int registerNdefMessageHandler(const QNdefFilter &filter,
                                   QObject *object, const QMetaMethod &method);

    bool unregisterNdefMessageHandler(int id);

    bool startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes);
    void stopTargetDetection();

public://call back function by symbian backend implementation
    void targetFound(QNearFieldTarget* target);
    void targetDisconnected();

public://call back function by symbian backend implementation
    void invokeNdefMessageHandler(const QNdefMessage msg);


private slots:
    void _q_privateHandleMessageSlot(QNdefMessage msg);

private:
    struct Callback {
        QNdefFilter filter;
        QObject *object;
        QMetaMethod method;
    };

    int getFreeId();

    QList<Callback> m_registeredHandlers;
    QList<int> m_freeIds;

    CNearFieldManager* m_symbianbackend;

    QPointer<QNearFieldTarget> m_target;
    QList<QPointer<QNearFieldTarget> > m_targetList;
    //For content handler purpose;
    QObject *m_chobject;
    QMetaMethod m_chmethod;

    QRemoteServiceRegister* m_serviceRegister ;
};

QTM_END_NAMESPACE

QT_END_HEADER
#endif /* QNEARFIELDMANAGER_SYMBIAN_P_H_ */
