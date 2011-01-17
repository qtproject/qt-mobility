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

#include "qnearfieldtarget_meego_p.h"

QTM_BEGIN_NAMESPACE

void PendingCallWatcher::addSendCommand(const QDBusPendingReply<QByteArray> &reply,
                                        const QNearFieldTarget::RequestId &id)
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(sendCommandFinished(QDBusPendingCallWatcher*)));

    m_pendingCommands.insert(watcher, id);
}

void PendingCallWatcher::addReadNdefMessages(const QDBusPendingReply<QList<QByteArray> > &reply)
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(readNdefMessagesFinished(QDBusPendingCallWatcher*)));

    m_pendingNdefReads.append(watcher);
}

void PendingCallWatcher::addWriteNdefMessages(const QDBusPendingReply<> &reply)
{
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(writeNdefMessages(QDBusPendingCallWatcher*)));

    m_pendingNdefWrites.append(watcher);
}

void PendingCallWatcher::sendCommandFinished(QDBusPendingCallWatcher *watcher)
{
    QNearFieldTarget::RequestId id = m_pendingCommands.take(watcher);

    if (!id.isValid())
        return;

    QDBusPendingReply<QByteArray> reply = *watcher;
    if (reply.isError()) {
        QMetaObject::invokeMethod(parent(), "error",
                                  Q_ARG(QNearFieldTarget::Error, QNearFieldTarget::NoError),
                                  Q_ARG(QNearFieldTarget::RequestId, id));
    } else {
        const QByteArray data = reply.argumentAt<0>();
        QMetaObject::invokeMethod(parent(), "handleResponse",
                                  Q_ARG(QNearFieldTarget::RequestId, id),
                                  Q_ARG(QByteArray, data));
    }

    watcher->deleteLater();
}

void PendingCallWatcher::readNdefMessagesFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QList<QByteArray> > reply = *watcher;
    if (reply.isError()) {
        QMetaObject::invokeMethod(parent(), "error",
                                  Q_ARG(QNearFieldTarget::Error, QNearFieldTarget::NdefReadError),
                                  Q_ARG(QNearFieldTarget::RequestId, QNearFieldTarget::RequestId()));
    } else {
        const QList<QByteArray> data = reply.argumentAt<0>();
        foreach (const QByteArray &m, data) {
            qDebug() << Q_FUNC_INFO << m.toHex();
            const QNdefMessage message = QNdefMessage::fromByteArray(m);

            qDebug() << "record count:" << message.count();
            foreach (const QNdefRecord &record, message)
                qDebug() << record.typeNameFormat() << record.type() << record.payload().toHex();

            QMetaObject::invokeMethod(parent(), "ndefMessageRead", Q_ARG(QNdefMessage, message));
        }
    }

    m_pendingNdefReads.removeOne(watcher);
    watcher->deleteLater();
}

void PendingCallWatcher::writeNdefMessages(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError()) {
        QMetaObject::invokeMethod(parent(), "error",
                                  Q_ARG(QNearFieldTarget::Error, QNearFieldTarget::NdefWriteError),
                                  Q_ARG(QNearFieldTarget::RequestId, QNearFieldTarget::RequestId()));
    } else {
        QMetaObject::invokeMethod(parent(), "ndefMessagesWritten");
    }

    m_pendingNdefWrites.removeOne(watcher);
    watcher->deleteLater();
}

int TagType1::memorySize() const
{
    return m_tag->size();
}

#include <moc_qnearfieldtarget_meego_p.cpp>

QTM_END_NAMESPACE

