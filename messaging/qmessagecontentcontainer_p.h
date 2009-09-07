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
#include "qmessagecontentcontainer.h"
#include "qmessage.h"

#include <QList>
#include <QMultiMap>

class QMessageContentContainerPrivate
{
    Q_DECLARE_PUBLIC(QMessageContentContainer)

public:
    QMessageContentContainerPrivate(QMessageContentContainer *contentContainer)
        : q_ptr(contentContainer),
          _message(0),
#ifdef Q_OS_WIN
          _attachmentNumber(0),
#endif
	  _available(false),
	  _size(0),
          _attachments(0)
    {
    }

    ~QMessageContentContainerPrivate()
    {
        delete _attachments;
        _attachments = 0;
    }

    QMessageContentContainerPrivate &operator=(const QMessageContentContainerPrivate &other)
    {
        if (&other != this) {
            _type = other._type;
            _subType = other._subType;
            _charset = other._charset;
            _name = other._name;
            _content = other._content;
            _textContent = other._textContent;
            _filename = other._filename;
            _messageId = other._messageId;
            _id = other._id;
            _available = other._available;
            _size = other._size;
            _header = other._header;
        }

        return *this;
    }

    QMessageContentContainer *q_ptr;
    QMessage *_message;

#ifdef Q_OS_WIN
    QMessageId _containingMessageId;
    ULONG _attachmentNumber;
#endif

    QByteArray _type;
    QByteArray _subType;
    QByteArray _charset;
    QByteArray _name;
    QByteArray _content;
    QString _textContent;
    QString _filename;
    QMessageId _messageId;
    QMessageContentContainerId _id;
    bool _available;
    uint _size;
    QList<QMessageContentContainer> *_attachments;
    QMultiMap<QByteArray, QString> _header;

    bool isMessage() const
    {
        return (_message != 0);
    }

    void setDerivedMessage(QMessage *derived)
    {
        _message = derived;
        if (_message) {
            _attachments = new QList<QMessageContentContainer>;
        }
    }

    void clearContents()
    {
        _type = QByteArray("text");
        _subType = QByteArray("plain");
        _charset = QByteArray();
        _name = QByteArray();
        _content = QByteArray();
        _textContent = QString();
        _filename = QString();
        _messageId = QMessageId();
        _id = QMessageContentContainerId();
        _available = false;
        _size = 0;
	_header.clear();

        if (_attachments) {
            _attachments->clear();
        }
    }

    QMessageContentContainer *attachment(const QMessageContentContainerId &id)
    {
        if (isMessage()) {
            if (id == _message->body()) {
                return _message;
            } else {
                foreach (const QMessageContentContainer &container, *_attachments) {
                    if (container.containerId() == id) {
                        return const_cast<QMessageContentContainer*>(&container);
                    }
                }
            }
        }

        return 0;
    }

    const QMessageContentContainer *attachment(const QMessageContentContainerId &id) const
    {
        if (isMessage()) {
            if (id == _message->body()) {
                return _message;
            } else {
                foreach (const QMessageContentContainer &container, *_attachments) {
                    if (container.containerId() == id) {
                        return &container;
                    }
                }
            }
        }

        return 0;
    }
};
