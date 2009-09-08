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
#include "qmessagecontentcontainer_p.h"
#include "winhelpers_p.h"

#include <QTextCodec>

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

    QMessageStore::ErrorCode error(QMessageStore::NoError);
    MapiSessionPtr session(MapiSession::createSession(&error, false));
    if (session && session->isValid()) {
        result = session->attachmentData(&error, id, number);
    }

    return result;
}

QString attachmentTextContent(const QMessageId &id, ULONG number, const QByteArray &charset)
{
    QString result;

    QMessageStore::ErrorCode error(QMessageStore::NoError);
    MapiSessionPtr session(MapiSession::createSession(&error, false));
    if (session && session->isValid()) {
        QByteArray data = session->attachmentData(&error, id, number);

        if (error == QMessageStore::NoError) {
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

QMessageContentContainerId QMessageContentContainer::containerId() const
{
    return d_ptr->_id;
}

QMessageId QMessageContentContainer::messageId() const
{
    return d_ptr->_messageId;
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentType(const QByteArray &data)
{
    clearContents();
    d_ptr->_type = data;
}
#endif

QByteArray QMessageContentContainer::contentType() const
{
    return d_ptr->_type;
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentSubType(const QByteArray &data)
{
    d_ptr->_subType = data;
}
#endif

QByteArray QMessageContentContainer::contentSubType() const
{
    return d_ptr->_subType;
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentCharset(const QByteArray &data)
{
    d_ptr->_charset = data;
}
#endif

QByteArray QMessageContentContainer::contentCharset() const
{
    return d_ptr->_charset;
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentFileName(const QByteArray &data)
{
    d_ptr->_name = data;
}
#endif

QByteArray QMessageContentContainer::contentFileName() const
{
    return d_ptr->_name;
}

bool QMessageContentContainer::isContentAvailable() const
{
    return d_ptr->_available;
}

uint QMessageContentContainer::size() const
{
    return d_ptr->_size;
}

QString QMessageContentContainer::textContent() const
{
    if (d_ptr->_textContent.isEmpty() && d_ptr->_attachmentNumber != 0) {
        d_ptr->_textContent = attachmentTextContent(d_ptr->_containingMessageId, d_ptr->_attachmentNumber, d_ptr->_charset);
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
    if (d_ptr->_content.isEmpty() && d_ptr->_attachmentNumber != 0) {
        d_ptr->_content = attachmentContent(d_ptr->_containingMessageId, d_ptr->_attachmentNumber);
    }

    return d_ptr->_content;
}

void QMessageContentContainer::writeTextContentTo(QTextStream& out) const
{
    out << textContent();
}

void QMessageContentContainer::writeContentTo(QDataStream& out) const
{
    QByteArray data(content());
    out.writeRawData(data.constData(), data.length());
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::clearContents()
{
    d_ptr->clearContents();
}

void QMessageContentContainer::setContent(const QString &text)
{
    d_ptr->_type = "text";
    d_ptr->_subType = "plain";
    d_ptr->_textContent = text;
    //d_ptr->_charset = charsetFor(text);
    d_ptr->_available = true;

    if (!d_ptr->_content.isEmpty()) {
        d_ptr->_content = QByteArray();
    }
    if (!d_ptr->_filename.isEmpty()) {
        d_ptr->_filename = QString();
    }
}

void QMessageContentContainer::setContent(const QByteArray &data)
{
    d_ptr->_content = data;
    d_ptr->_available = true;

    if (!d_ptr->_textContent.isEmpty()) {
        d_ptr->_textContent = QString();
    }
    if (!d_ptr->_filename.isEmpty()) {
        d_ptr->_filename = QString();
    }
}

void QMessageContentContainer::setContentFromFile(const QString &fileName)
{
    d_ptr->_filename = fileName;
    d_ptr->_available = true;

    if (!d_ptr->_content.isEmpty()) {
        d_ptr->_content = QByteArray();
    }
    if (!d_ptr->_textContent.isEmpty()) {
        d_ptr->_textContent = QString();
    }
}

void QMessageContentContainer::readContentFrom(QDataStream &in)
{
    QByteArray content;
    while (!in.atEnd()) {
        char buffer[1024];
        int len = in.readRawData(buffer, 1024);
        content.append(QByteArray(buffer, len));
    }

    setContent(content);
}

QMessageContentContainerId QMessageContentContainer::appendContent(const QMessageContentContainer & content)
{
    if (d_ptr->isMessage()) {
        d_ptr->_attachments->append(content);

        QMessageContentContainer &container(d_ptr->_attachments->last());
        container.d_ptr->_id = QMessageContentContainerId(QString::number(d_ptr->_attachments->count()));
        return container.d_ptr->_id;
    }

    return QMessageContentContainerId();
}

void QMessageContentContainer::replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer & content)
{
    if (d_ptr->isMessage()) {
        if (QMessageContentContainer *container = d_ptr->attachment(id)) {
            *container = content;
            return;
        }
    }
}
#endif

QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    QMessageContentContainerIdList ids;

    if (d_ptr->isMessage()) {
        foreach (const QMessageContentContainer &container, *d_ptr->_attachments) {
            ids.append(container.containerId());
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

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QString &value)
{
    d_ptr->_header.insert(name, value);
}

void QMessageContentContainer::setHeaderField(const QByteArray &name, const QString &value)
{
    d_ptr->_header.replace(name, value);
}
#endif

QString QMessageContentContainer::headerFieldValue(const QByteArray &name) const
{
    QMultiMap<QByteArray, QString>::const_iterator it = d_ptr->_header.find(name);
    if (it != d_ptr->_header.end()) {
        return it.value();
    }

    return QString();
}

QStringList QMessageContentContainer::headerFieldValues(const QByteArray &name) const
{
    QStringList values;

    QMultiMap<QByteArray, QString>::const_iterator it = d_ptr->_header.find(name);
    while ((it != d_ptr->_header.end()) && (it.key() == name)) {
        values.append(it.value());
        ++it;
    }

    return values;
}

QList<QByteArray> QMessageContentContainer::headerFields() const
{
    return d_ptr->_header.keys();
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QByteArray &value)
{
    appendHeaderField(name, QString(value));
}

void QMessageContentContainer::setHeaderField(const QByteArray &name, const QByteArray &value)
{
    setHeaderField(name, QString(value));
}
#endif

#ifdef QMESSAGING_OPTIONAL
bool QMessageContentContainer::containerDataModified() const
{
    return false; // stub
}

QMessageContentContainerId QMessageContentContainer::prependContent(const QMessageContentContainer & content)
{
    if (d_ptr->isMessage()) {
        d_ptr->_attachments->prepend(content);

        int n = 1;
        foreach (const QMessageContentContainer &container, *d_ptr->_attachments) {
            container.d_ptr->_id = QMessageContentContainerId(QString::number(n));
        }

        return QMessageContentContainerId(QString::number(1));
    }

    return QMessageContentContainerId();
}

void QMessageContentContainer::setDerivedMessage(QMessage *derived)
{
    d_ptr->setDerivedMessage(derived);
}

void QMessageContentContainer::applyPendingChanges() const
{
}
#endif

void QMessageContentContainer::removeContent(const QMessageContentContainerId &id)
{
    if (d_ptr->isMessage()) {
        for (int i = 0; i < d_ptr->_attachments->count(); ++i) {
            if (d_ptr->_attachments->at(i).containerId() == id) {
                d_ptr->_attachments->removeAt(i);

                // Adjust the identifiers for any trailing attachments
                for (int j = i; j < d_ptr->_attachments->count(); ++j) {
                    d_ptr->_attachments->at(j).d_ptr->_id = QMessageContentContainerId(QString::number(j + 1));
                }
                return;
            }
        }
    }
}

