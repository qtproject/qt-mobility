/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessagecontentcontainer.h"
#include "qmessagecontentcontainer_p.h"
#include "winhelpers_p.h"
#include <QTextCodec>
#include <QDebug>

QTM_BEGIN_NAMESPACE

namespace WinHelpers {

QMessageContentContainer fromLocator(const WinHelpers::AttachmentLocator &loc)
{
    return QMessageContentContainerPrivate::from(loc.first, loc.second);
}

}

namespace {

QByteArray attachmentContent(const QMessageId &id, ULONG number)
{
    QByteArray result;

    QMessageManager::Error error(QMessageManager::NoError);
    MapiSessionPtr session(MapiSession::createSession(&error));
    if (session && session->isValid()) {
        result = session->attachmentData(&error, id, number);
    }

    return result;
}

QString attachmentTextContent(const QMessageId &id, ULONG number, const QByteArray &charset)
{
    QString result;

    QMessageManager::Error error(QMessageManager::NoError);
    MapiSessionPtr session(MapiSession::createSession(&error));
    if (session && session->isValid()) {
        QByteArray data = session->attachmentData(&error, id, number);

        if (error == QMessageManager::NoError) {
            // Convert attachment data to string form
            QTextCodec *codec;
            if (!charset.isEmpty()) {
                codec = QTextCodec::codecForName(charset);
            } else {
                codec = QTextCodec::codecForLocale();
            }

            if (codec) {
                result = codec->toUnicode(data);
            }
        }
    }
    return result;
}

}

QMessageContentContainer::QMessageContentContainer()
    : d_ptr(new QMessageContentContainerPrivate(this))
{
}

QMessageContentContainer::QMessageContentContainer(const QMessageContentContainer &other)
    : d_ptr(new QMessageContentContainerPrivate(this))
{
     this->operator=(other);
}

QMessageContentContainer& QMessageContentContainer::operator=(const QMessageContentContainer& other)
{
    if (&other != this) {
        *d_ptr = *other.d_ptr;
    }

    return *this;
}

QMessageContentContainer::~QMessageContentContainer()
{
    delete d_ptr;
}

QByteArray QMessageContentContainer::contentType() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensureBodyPresent(d_ptr->_message);
    }

    return d_ptr->_type;
}

QByteArray QMessageContentContainer::contentSubType() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensureBodyPresent(d_ptr->_message);
    }

    return d_ptr->_subType;
}

QByteArray QMessageContentContainer::contentCharset() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensureBodyPresent(d_ptr->_message);
    }

    return d_ptr->_charset;
}

QByteArray QMessageContentContainer::suggestedFileName() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensureBodyPresent(d_ptr->_message);
    }

    return d_ptr->_name;
}

bool QMessageContentContainer::isContentAvailable() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensureBodyPresent(d_ptr->_message);
        d_ptr->_message->d_ptr->ensureAttachmentsPresent(d_ptr->_message);
    }

    //if we are a container, then availabilty depends on the availability of subparts
    if(!contentIds().isEmpty())
    {
        bool available = true;
        foreach(const QMessageContentContainer& c, d_ptr->_attachments)
            available &= c.isContentAvailable();
        return available;
    }

    return d_ptr->_available;
}

int QMessageContentContainer::size() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensurePropertiesPresent(d_ptr->_message);
    }

    return d_ptr->_size;
}

QString QMessageContentContainer::textContent() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensureBodyPresent(d_ptr->_message);
    }

    if (d_ptr->_textContent.isEmpty() && d_ptr->_attachmentNumber != 0) {
        const_cast<QString&>(d_ptr->_textContent) = attachmentTextContent(d_ptr->_containingMessageId, d_ptr->_attachmentNumber, d_ptr->_charset);
    }
    if (!d_ptr->_textContent.isEmpty()) {
        return d_ptr->_textContent;
    }

    QTextCodec *codec = QTextCodec::codecForName(d_ptr->_charset.data());
    if (codec) {
        return codec->toUnicode(d_ptr->_content);
    } else {
        return QString::fromLatin1(d_ptr->_content);
    }
}

QByteArray QMessageContentContainer::content() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensureBodyPresent(d_ptr->_message);
    }

    if (d_ptr->_content.isEmpty() && d_ptr->_attachmentNumber != 0) {
        const_cast<QByteArray&>(d_ptr->_content) = attachmentContent(d_ptr->_containingMessageId, d_ptr->_attachmentNumber);
    }

    return d_ptr->_content;
}

void QMessageContentContainer::writeTextContent(QTextStream& out) const
{
    out << textContent();
}

void QMessageContentContainer::writeContent(QDataStream& out) const
{
    QByteArray data(content());
    out.writeRawData(data.constData(), data.length());
}

QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    QMessageContentContainerIdList ids;

    if (d_ptr->isMessage()) {
        foreach (const QMessageContentContainer &container, d_ptr->_attachments) {
            ids.append(container.d_ptr->_id);
        }
    }

    return ids;
}

QMessageContentContainer QMessageContentContainer::find(const QMessageContentContainerId &id) const
{
    if (d_ptr->isMessage()) {
        if (const QMessageContentContainer *container = d_ptr->attachment(id)) {
            return *container;
        }
    }

    return QMessageContentContainer();

}

bool QMessageContentContainer::contains(const QMessageContentContainerId &id) const
{
    if (d_ptr->isMessage()) {
        return (d_ptr->attachment(id) != 0);
    }

    return false;
}

QString QMessageContentContainer::headerFieldValue(const QByteArray &name) const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensurePropertiesPresent(d_ptr->_message);
    }

    QMultiMap<QByteArray, QString>::const_iterator it = d_ptr->_header.find(name);
    if (it != d_ptr->_header.end()) {
        return it.value();
    }

    return QString();
}

QStringList QMessageContentContainer::headerFieldValues(const QByteArray &name) const
{
    QStringList values;

    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensurePropertiesPresent(d_ptr->_message);
    }

    QMultiMap<QByteArray, QString>::const_iterator it = d_ptr->_header.find(name);
    while ((it != d_ptr->_header.end()) && (it.key() == name)) {
        values.append(it.value());
        ++it;
    }

    return values;
}

QList<QByteArray> QMessageContentContainer::headerFields() const
{
    if (d_ptr->isMessage()) {
        d_ptr->_message->d_ptr->ensurePropertiesPresent(d_ptr->_message);
    }

    return d_ptr->_header.keys();
}

void QMessageContentContainer::setDerivedMessage(QMessage *derived)
{
    d_ptr->setDerivedMessage(derived);
}

QTM_END_NAMESPACE
