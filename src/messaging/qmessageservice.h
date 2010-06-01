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
****************************************************************************/
#ifndef QMESSAGESERVICEACTION_H
#define QMESSAGESERVICEACTION_H
#include <QObject>
#include <qmessageglobal.h>
#include <qmessagemanager.h>
#include <qmessage.h>


QTM_BEGIN_NAMESPACE

class QMessageServicePrivate;

class Q_MESSAGING_EXPORT QMessageService : public QObject
{
    Q_OBJECT

    friend class QMessageServicePrivate;

public:
    enum State {
        InactiveState = 0,
        ActiveState,
        CanceledState,
        FinishedState
    };

    QMessageService(QObject *parent = 0);
    ~QMessageService();

    bool queryMessages(const QMessageFilter &filter = QMessageFilter(), const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0);
    bool queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags = 0, const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0);

    bool countMessages(const QMessageFilter &filter = QMessageFilter());

    bool send(QMessage &message);
    bool compose(const QMessage &message);
    bool retrieveHeader(const QMessageId &id);
    bool retrieveBody(const QMessageId &id);
    bool retrieve(const QMessageId &messageId, const QMessageContentContainerId &id);
    bool show(const QMessageId &id);
    bool exportUpdates(const QMessageAccountId &id);

    State state() const;
    QMessageManager::Error error() const;

public slots:
    void cancel();

signals:
    void stateChanged(QMessageService::State newState);
    void messagesFound(const QMessageIdList &ids);
    void messagesCounted(int count);
    void progressChanged(uint value, uint total);

private:
    QMessageServicePrivate *d_ptr;
};

QTM_END_NAMESPACE
#endif
