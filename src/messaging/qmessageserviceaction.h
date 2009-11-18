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
#ifndef QMESSAGESERVICEACTION_H
#define QMESSAGESERVICEACTION_H
#include <QObject>
#include <qmessageglobal.h>
#include <qmessagestore.h>
#include <qmessage.h>

class QMessageServiceActionPrivate;

class Q_MESSAGING_EXPORT QMessageServiceAction : public QObject
{
    Q_OBJECT

    friend class QMessageServiceActionPrivate;

public:
    enum State {
        Pending = 0,
        InProgress,
        Successful,
        Failed
    };

    QMessageServiceAction(QObject *parent = 0);
    ~QMessageServiceAction();

    bool queryMessages(const QMessageFilter &filter = QMessageFilter(), const QMessageOrdering &ordering = QMessageOrdering(), uint limit = 0, uint offset = 0);
    bool queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options = 0, const QMessageOrdering &ordering = QMessageOrdering(), uint limit = 0, uint offset = 0);

    bool countMessages(const QMessageFilter &filter = QMessageFilter());

    bool send(QMessage &message);
    bool compose(const QMessage &message);
    bool retrieveHeader(const QMessageId &id);
    bool retrieveBody(const QMessageId &id);
    bool retrieve(const QMessageContentContainerId &id);
    bool show(const QMessageId &id);
    bool exportUpdates(const QMessageAccountId &id);

    State state() const;
    QMessageStore::ErrorCode lastError() const;

public slots:
    void cancelOperation();

signals:
    void stateChanged(QMessageServiceAction::State a);
    void messagesFound(const QMessageIdList &ids);
    void messagesCounted(int count);
    void progressChanged(uint value, uint total);

private:
    QMessageServiceActionPrivate *d_ptr;
};
#endif
