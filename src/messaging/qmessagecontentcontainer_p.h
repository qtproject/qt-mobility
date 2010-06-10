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
#ifndef QMESSAGECONTENTCONTAINER_P_H
#define QMESSAGECONTENTCONTAINER_P_H

#include "qmessageglobal.h"
#include "qmessagecontentcontainer.h"
#include "qmessage.h"

#ifdef USE_QMF_IMPLEMENTATION
#include "qmfhelpers_p.h"
#else
#include "qmessage_p.h"
#if defined(Q_OS_WIN)
#include "winhelpers_p.h"
#include "qmessagecontentcontainerid_p.h"
#endif
#endif

#include <QSharedData>
#include <QList>
#include <QMultiMap>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

QTM_BEGIN_NAMESPACE

#ifdef USE_QMF_IMPLEMENTATION
class QMessageContentContainerPrivate
{
public:
    mutable QMessage *_message;
    mutable QMailMessagePart _part;
    mutable QMailMessagePartContainer *_container;

    QByteArray _type;
    QByteArray _subType;
    QByteArray _charset;
    QByteArray _name;
    QByteArray _content;
    QString _textContent;
    QString _filename;

    QMessageContentContainerPrivate()
        : _message(0),
          _container(&_part)
    {
    }

    void setDerivedMessage(QMessage *derived)
    {
        _message = derived;
        _part = QMailMessagePart();
        _container = QmfHelpers::convert(_message);
    }
};

#else
class QMessageContentContainerPrivate
{
    Q_DECLARE_PUBLIC(QMessageContentContainer)

public:
    QMessageContentContainerPrivate(QMessageContentContainer *contentContainer)
        :
        q_ptr(contentContainer),
         _message(0),
#ifdef Q_OS_WIN
         _attachmentNumber(0),
#endif
         _available(false),
         _size(0)
    {
    }

    QMessageContentContainerPrivate(const QMessageContentContainerPrivate& other)
        :
        q_ptr(other.q_ptr),
        _message(other._message),
#ifdef Q_OS_WIN
        _attachmentNumber(other._attachmentNumber),
        _containingMessageId(other._containingMessageId),
#endif
        _attachments(other._attachments),
        _available(other._available),
        _size(other._size),
        _type(other._type),
        _subType(other._subType),
        _charset(other._charset),
        _name(other._name),
        _content(other._content),
        _textContent(other._textContent),
        _filename(other._filename),
        _messageId(other._messageId),
        _id(other._id),
        _header(other._header)
    {
    }

    QMessageContentContainerPrivate &operator=(const QMessageContentContainerPrivate &other)
    {
        q_ptr = other.q_ptr;
        _message = other._message;
#ifdef Q_OS_WIN
        _attachmentNumber = other._attachmentNumber;
        _containingMessageId = other._containingMessageId;
#endif
        _available = other._available;
        _size = other._size;
        _attachments = other._attachments;
        _type = other._type;
        _subType = other._subType;
        _charset = other._charset;
        _name = other._name;
        _content = other._content;
        _textContent = other._textContent;
        _filename = other._filename;
        _messageId = other._messageId;
        _id = other._id;
        _header = other._header;

        return *this;
    }

#ifdef Q_OS_WIN
    static QMessageContentContainer from(const QMessageId &id, ULONG number)
    {
        QMessageContentContainer result;
        result.d_ptr->_containingMessageId = id;
        result.d_ptr->_attachmentNumber = number;
        result.d_ptr->_available = true;
        return result;
    }
#endif

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
    int _size;
    QList<QMessageContentContainer> _attachments;
    QMultiMap<QByteArray, QString>  _header;

    bool isMessage() const
    {
        return (_message != 0);
    }

    void setDerivedMessage(QMessage *derived)
    {
        _message = derived;
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
        _available = false;
        _size = 0;
        _header.clear();
        _attachments.clear();
    }

    void setContentType(const QByteArray &type, const QByteArray &subType, const QByteArray &charset)
    {
        clearContents();

        _type = type;
        _subType = subType;
        _charset = charset;
    }

    void setContent(const QString &content, const QByteArray &type, const QByteArray &subType, const QByteArray &charset)
    {
        setContentType(type, subType, charset);

        _textContent = content;
        _size = content.length();
#ifdef Q_OS_WIN
        // Approximate size in bytes, not chars
        _size *= sizeof(TCHAR);
#endif
        _available = true;
    }

    void setContent(const QByteArray &content, const QByteArray &type, const QByteArray &subType, const QByteArray &charset)
    {
        setContentType(type, subType, charset);

        _content = content;
        _size = content.length();
        _available = true;
    }

    void setHeaderField(const QByteArray &name, const QByteArray &value)
    {
        _header.remove(name);
        _header.insert(name, value);
    }

    QMessageContentContainer *attachment(const QMessageContentContainerId &id)
    {
        if (isMessage()) {
#ifdef Q_OS_WIN
            _message->d_ptr->ensureAttachmentsPresent(_message);
#endif

            if (id == bodyContentId()) {
                return _message;
            } else {
                foreach (const QMessageContentContainer &container, _attachments) {
                    if (container.d_ptr->_id == id) {
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
#ifdef Q_OS_WIN
            _message->d_ptr->ensureAttachmentsPresent(_message);
#endif

            if (id == bodyContentId()) {
                return _message;
            } else {
                foreach (const QMessageContentContainer &container, _attachments) {
                    if (container.d_ptr->_id == id) {
                        return &container;
                    }
                }
            }
        }

        return 0;
    }

    bool createAttachment(const QString& attachmentPath)
    {
        //set the attachment data

        if (!QFile::exists(attachmentPath)) {
            qWarning() << "Could not create attachment. File " << attachmentPath << " does not exist";
            return false;
        }

        QFile attachmentFile(attachmentPath);
        if (!attachmentFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open attachment " << attachmentPath;
            return false;
        }

        _content = attachmentFile.readAll();
        _available = true;

        attachmentFile.close();

        //set the suggested file name for the attachment

        QFileInfo fi(attachmentPath);
        _name = fi.fileName().toLatin1();
        _size = fi.size();

        //set the attachment filepath

        _filename = attachmentPath;

        //set the mime-type

        QByteArray mimeType;

        QString extension(fi.suffix());
#ifdef Q_OS_WIN
        mimeType = WinHelpers::contentTypeFromExtension(extension);
#else
        // TODO
#endif
        int index = mimeType.indexOf("/");
        if (index != -1) {
            _type = mimeType.left(index).trimmed();
            _subType = mimeType.mid(index + 1).trimmed();
        }

        return true;
    }

    QMessageContentContainerId appendContent(QMessageContentContainer& container)
    {
#ifdef OS_WIN
        if (!isMessage()) {
            qWarning() << "Unable to add child QMessageContentContainer. MAPI only supports single level nesting of containers.";
            return;
        }
#endif
        container.d_ptr->_id = QMessageContentContainerId(QString::number(_attachments.count()+1));
        _attachments.append(container);
        return container.d_ptr->_id;
    }

    QMessageContentContainerId prependContent(QMessageContentContainer& container)
    {
#ifdef OS_WIN
        if (!isMessage()) {
            qWarning() << "Unable to add child QMessageContentContainer. MAPI only supports single level nesting of containers.";
            return;
        }
#endif
        _attachments.prepend(container);
        for (int i = 0; i < _attachments.count(); ++i) {
            _attachments[i].d_ptr->_id = QMessageContentContainerId(QString::number(i+1));
        }
        return _attachments[0].d_ptr->_id;
    }

    QMessageContentContainerId bodyContentId() const
    {
        return QMessageContentContainerId(QString::number(0));
    }

    static QString attachmentFilename(const QMessageContentContainer& container)
    {
        return container.d_ptr->_filename;
    }
};
#endif

QTM_END_NAMESPACE

#endif
