/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessage.h"
#include "qmessageaddress.h"

#include <QMap>

class QMessagePrivate
{
    Q_DECLARE_PUBLIC(QMessage)

public:
    QMessagePrivate(QMessage *message)
        :q_ptr(message),
         _size(0),
         _type(QMessage::NoType),
         _status(0),
         _priority(QMessage::NormalPriority),
#ifdef QMESSAGING_OPTIONAL
         _originatorPort(0),
         _destinationPort(0),
#endif
         _modified(false)
    {
    }

    QMessage *q_ptr;

    QMessageId _id;
    uint _size;
    QMessageAccountId _parentAccountId;
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderId _parentFolderId;
#endif
    QMessage::Type _type;
    QMessageAddress _from;
    QString _senderName;
    QMessageAddressList _to;
    QMessageAddressList _cc;
    QMessageAddressList _bcc;
    QMessage::StatusFlags _status;
    QMessage::Priority _priority;
    QString _subject;
    QDateTime _date;
    QDateTime _receivedDate;
#ifdef QMESSAGING_OPTIONAL
    uint _originatorPort;
    uint _destinationPort;
    QMap<QString, QString> _customFields;
#endif
    bool _modified;
#if defined(Q_OS_WIN)
    static QMessage from(const QMessageId &id);
    static QString senderName(const QMessage &message);
    static void setSenderName(const QMessage &message, const QString &senderName);
    static void setSize(const QMessage &message, uint size);
#endif
};
