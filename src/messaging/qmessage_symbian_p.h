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
#ifndef QMESSAGE_P_H
#define QMESSAGE_P_H

#include "qmessage.h"
#include "qmessageaddress.h"


QTM_BEGIN_NAMESPACE

class QMessagePrivate
{
    Q_DECLARE_PUBLIC(QMessage)

public:
    QMessagePrivate(QMessage *message);
    ~QMessagePrivate();

    static QString senderName(const QMessage &message);
    static void setSenderName(const QMessage &message, const QString &senderName);
    static void setSize(const QMessage &message, int size);
    static void setStandardFolder(QMessage& message, QMessage::StandardFolder sf);
    static QMessagePrivate* implementation(const QMessage &message);
    static QMessageContentContainerPrivate* containerImplementation(const QMessage &message);

public: // Data
    QMessage *q_ptr;

    QMessageId _id;
    QMessage::Type _type;
    QString _body;
    int _size;
    QMessageAccountId _parentAccountId;
    QMessageFolderId _parentFolderId;
    QMessage::StandardFolder _standardFolder;
    QMessageAddress _from;
    QString _senderName;
    QMessageAddressList _toList;
    QMessageAddressList _ccList;
    QMessageAddressList _bccList;
    QMessage::StatusFlags _status;
    QMessage::Priority _priority;
    QString _subject;
    QDateTime _date;
    QDateTime _receivedDate;
    bool _modified;
    QMessageContentContainerId _bodyId;
};


QTM_END_NAMESPACE
#endif
