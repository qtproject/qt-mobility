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

#include "qmessageserviceaction.h"


QTM_BEGIN_NAMESPACE

class QMessageServiceActionPrivate : public QObject
{
	Q_OBJECT
    Q_DECLARE_PUBLIC(QMessageServiceAction)

public:
    QMessageServiceActionPrivate(QMessageServiceAction* parent);
    ~QMessageServiceActionPrivate();

	bool sendSMS(QMessage &message);
	bool sendMMS(QMessage &message);
	bool sendEmail(QMessage &message);
	bool show(const QMessageId& id);
	bool compose(const QMessage &message);
	bool retrieve(const QMessageId &messageId, const QMessageContentContainerId &id);
	bool retrieveBody(const QMessageId& id);
	bool retrieveHeader(const QMessageId& id);
	
    bool queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const;
    bool queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const;
    
    bool countMessages(const QMessageFilter &filter);
	
signals:
	void stateChanged(QMessageServiceAction::State);
	void messagesFound(const QMessageIdList&);
    void messagesCounted(int count);
	void progressChanged(uint, uint);

private:	
    QMessageServiceAction *q_ptr;
    QMessageServiceAction::State _state;
    QMessageManager::ErrorCode _lastError;
    int _actionId;
    bool _active;
    
    friend class CMTMEngine;
};



QTM_END_NAMESPACE
