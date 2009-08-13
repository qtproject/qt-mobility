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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagecontentcontainer.h"

#include <QList>
#include <QMultiMap>

class QMessageContentContainerPrivate
{
    Q_DECLARE_PUBLIC(QMessageContentContainer)

public:
    QMessageContentContainerPrivate(QMessageContentContainer *contentContainer)
        : q_ptr(contentContainer),
	  _available(false),
	  _size(0),
          _attachments(0)
    {
    }

    QMessageContentContainer *q_ptr;

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
        return (_attachments != 0);
    }

    void setDerivedMessage(QMessage *derived)
    {
        _attachments = new QList<QMessageContentContainer>;

        Q_UNUSED(derived)
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
        if (_attachments != 0) {
            foreach (const QMessageContentContainer &container, *_attachments) {
                if (container.containerId() == id) {
                    return const_cast<QMessageContentContainer*>(&container);
                }
            }
        }

        return 0;
    }

    const QMessageContentContainer *attachment(const QMessageContentContainerId &id) const
    {
        if (_attachments != 0) {
            foreach (const QMessageContentContainer &container, *_attachments) {
                if (container.containerId() == id) {
                    return &container;
                }
            }
        }

        return 0;
    }
};
