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
#include "qmfhelpers_p.h"

#include <QTextCodec>
#include <QDebug>

using namespace QmfHelpers;

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
        _container = convert(_message);
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
    }

    QMailMessageContentType contentType() const 
    {
        QMailMessageContentType ct;
        ct.setType(_type);
        ct.setSubType(_subType);

        if (!_name.isEmpty()) {
            ct.setName(_name);
        }
        if (!_charset.isEmpty()) {
            ct.setCharset(_charset);
        }

        return ct;
    }
    
    void applyPendingChanges() const
    {
        if (!_content.isEmpty()) {
            _container->setBody(QMailMessageBody::fromData(_content, contentType(), QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding));
        } else if (!_textContent.isEmpty()) {
            _container->setBody(QMailMessageBody::fromData(_textContent, contentType(), QMailMessageBody::Base64));
        } else if (!_filename.isEmpty()) {
            _container->setBody(QMailMessageBody::fromFile(_filename, contentType(), QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding));
        } else {
            if (contentType().type() == "multipart") {
                _container->setMultipartType(QMailMessagePartContainer::multipartTypeForName(_subType));
            }
        }
    }
};

namespace {

QByteArray charsetFor(const QString &text)
{
    QByteArray charset(QMessage::preferredCharsetFor(text));
    if (charset.isEmpty()) {
        charset = "UTF-8";
    }

    return charset;
}

struct PartLocator
{
    const QMailMessagePart::Location &_location;
    QMailMessagePart *_part;

    PartLocator(const QMailMessagePart::Location &location)
        : _location(location),
          _part(0)
    {
    }

    bool operator()(QMailMessagePart &part)
    {
        if (part.location() == _location) {
            _part = &part;
            return false;
        }

        return true;
    }
};

struct PartRemover
{
    const QMailMessagePart::Location &_location;

    PartRemover(const QMailMessagePart::Location &location)
        : _location(location)
    {
    }

    bool operator()(QMailMessagePart &part)
    {
        for (uint i = 0; i < part.partCount(); ++i) {
            if (part.partAt(i).location() == _location) {
                part.removePartAt(i);
                return false;
            }
        }

        return true;
    }
};

struct SizeAccumulator
{
    uint _size;

    SizeAccumulator() : _size(0) {}

    bool operator()(QMailMessagePart &part)
    {
        if (part.hasBody()) {
            _size += part.body().length();
        }

        return true;
    }
};

}

QMessageContentContainer::QMessageContentContainer()
    : d_ptr(new QMessageContentContainerPrivate)
{
}

QMessageContentContainer::QMessageContentContainer(const QMessageContentContainer &other)
    : d_ptr(new QMessageContentContainerPrivate)
{
    this->operator=(other);
}

const QMessageContentContainer& QMessageContentContainer::operator=(const QMessageContentContainer& other)
{
    if (&other != this) {
        if (other.d_ptr->_container == &other.d_ptr->_part) {
            d_ptr->_part = other.d_ptr->_part;
            d_ptr->_container = &d_ptr->_part;
        } else {
            d_ptr->_message = other.d_ptr->_message;
            d_ptr->_part = QMailMessagePart();
            d_ptr->_container = convert(d_ptr->_message);
        }
    }

    return *this;
}

QMessageContentContainer::~QMessageContentContainer()
{
    delete d_ptr;
}

QMessageContentContainerId QMessageContentContainer::containerId() const
{
    if (d_ptr->_container == &d_ptr->_part) {
        return convert(d_ptr->_part.location());
    } else {
        return QmfHelpers::bodyId(convert(d_ptr->_message)->id());
    }
}

QMessageId QMessageContentContainer::messageId() const
{
    if (d_ptr->_container == &d_ptr->_part) {
        return convert(d_ptr->_part.location().containingMessageId());
    } else {
        return convert(convert(d_ptr->_message)->id());
    }
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
    if (!d_ptr->_type.isEmpty()) {
        return d_ptr->_type;
    }
    return d_ptr->_container->contentType().type();
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentSubType(const QByteArray &data)
{
    d_ptr->_subType = data;
}
#endif

QByteArray QMessageContentContainer::contentSubType() const
{
    if (!d_ptr->_subType.isEmpty()) {
        return d_ptr->_subType;
    }
    return d_ptr->_container->contentType().subType();
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentCharset(const QByteArray &data)
{
    d_ptr->_charset = data;
}
#endif

QByteArray QMessageContentContainer::contentCharset() const
{
    if (!d_ptr->_charset.isEmpty()) {
        return d_ptr->_charset;
    }
    return d_ptr->_container->contentType().charset();
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentFileName(const QByteArray &data)
{
    d_ptr->_name = data;
}
#endif

QByteArray QMessageContentContainer::contentFileName() const
{
    if (!d_ptr->_name.isEmpty()) {
        return d_ptr->_name;
    }
    return d_ptr->_container->contentType().name();
}

bool QMessageContentContainer::isContentAvailable() const
{
    if (!d_ptr->_content.isEmpty()) {
        return true;
    }
    return (d_ptr->_container->partCount() > 0) || d_ptr->_container->contentAvailable();
}

uint QMessageContentContainer::size() const
{
    applyPendingChanges();

    SizeAccumulator accumulator;
    d_ptr->_container->foreachPart<SizeAccumulator&>(accumulator);

    return accumulator._size;
}

QString QMessageContentContainer::textContent() const
{
    applyPendingChanges();
    return d_ptr->_container->body().data();
}

QByteArray QMessageContentContainer::content() const
{
    applyPendingChanges();
    return d_ptr->_container->body().data(QMailMessageBody::Decoded);
}

void QMessageContentContainer::writeTextContentTo(QTextStream& out) const
{
    applyPendingChanges();
    if (d_ptr->_container->hasBody()) {
        d_ptr->_container->body().toStream(out);
    }
}

void QMessageContentContainer::writeContentTo(QDataStream& out) const
{
    applyPendingChanges();
    if (d_ptr->_container->hasBody()) {
        d_ptr->_container->body().toStream(out, QMailMessageBody::Decoded);
    }
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
    d_ptr->_charset = charsetFor(text);

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
    if (contentType() != QByteArray("multipart")) {
        clearContents();

        setContentType(QByteArray("multipart"));
        setContentSubType(QByteArray("mixed"));
    }

    content.applyPendingChanges();
    d_ptr->_container->appendPart(content.d_ptr->_part);

    return convert(d_ptr->_container->partAt(d_ptr->_container->partCount() - 1).location());
}

void QMessageContentContainer::replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer &content)
{
    QMailMessagePart::Location location(convert(id));

    if (location.isValid(false)) {
        PartLocator locator(location);
        d_ptr->_container->foreachPart<PartLocator&>(locator);

        if (locator._part) {
            content.applyPendingChanges();
            *locator._part = content.d_ptr->_part;
        }
    } else {
        if (location.containingMessageId() == convert(d_ptr->_message)->id()) {
            // Replace the body with the replacement content
            content.applyPendingChanges();

            QMailMessageContentType ct;
            ct.setType(content.contentType());
            ct.setSubType(content.contentSubType());
            ct.setCharset(content.contentCharset());

            convert(d_ptr->_message)->setBody(QMailMessageBody::fromData(content.content(), ct, QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding));
            d_ptr->_container = convert(d_ptr->_message);
        }
    }
}
#endif

QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    QMessageContentContainerIdList ids;

    for (uint i = 0; i < d_ptr->_container->partCount(); ++i) {
        ids.append(convert(d_ptr->_container->partAt(i).location()));
    }

    return ids;
}

QMessageContentContainer QMessageContentContainer::container(const QMessageContentContainerId &id) const
{
    QMessageContentContainer container;

    QMailMessagePart::Location location(convert(id));

    if (location.isValid(false)) {
        PartLocator locator(location);
        d_ptr->_container->foreachPart<PartLocator&>(locator);

        if (locator._part) {
            container.d_ptr->_part = *locator._part;
            container.d_ptr->_container = &container.d_ptr->_part;
        }
    } else {
        if (location.containingMessageId() == convert(d_ptr->_message)->id()) {
            // Create a part containing the body content of this message
            QMailMessage *message(convert(d_ptr->_message));
            QMailMessageContentDisposition cd(QMailMessageContentDisposition::Inline);
            container.d_ptr->_part = QMailMessagePart::fromData(message->body().data(), cd, message->contentType(), QMailMessageBody::Base64);
            container.d_ptr->_container = &container.d_ptr->_part;
        }
    }

    return container;
}

bool QMessageContentContainer::contains(const QMessageContentContainerId &id) const
{
    QMailMessagePart::Location location(convert(id));

    if (location.isValid(false)) {
        PartLocator locator(location);
        d_ptr->_container->foreachPart<PartLocator&>(locator);
        return (locator._part != 0);
    } else {
        return (location.containingMessageId() == convert(d_ptr->_message)->id());
    }
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QString &value)
{
    QByteArray charset = charsetFor(value);
    if (QTextCodec* codec = QTextCodec::codecForName(charset)) {
        appendHeaderField(name, codec->fromUnicode(value));
    }
}

void QMessageContentContainer::setHeaderField(const QByteArray &name, const QString &value)
{
    QByteArray charset = charsetFor(value);
    if (QTextCodec* codec = QTextCodec::codecForName(charset)) {
        setHeaderField(name, codec->fromUnicode(value));
    }
}
#endif

QString QMessageContentContainer::headerField(const QByteArray &name) const
{
    return d_ptr->_container->headerFieldText(name);
}

QStringList QMessageContentContainer::headerFieldValues(const QByteArray &name) const
{
    return d_ptr->_container->headerFieldsText(name);
}

QList<QByteArray> QMessageContentContainer::headerFields() const
{
    QList<QByteArray> fields;
    foreach (const QMailMessageHeaderField &field, d_ptr->_container->headerFields()) {
        fields.append(field.id());
    }

    return fields;
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QByteArray &value)
{
    d_ptr->_container->appendHeaderField(name, value);
}

void QMessageContentContainer::setHeaderField(const QByteArray &name, const QByteArray &value)
{
    d_ptr->_container->setHeaderField(name, value);
}
#endif

#ifdef QMESSAGING_OPTIONAL
bool QMessageContentContainer::containerDataModified() const
{
    return true;
}

QMessageContentContainerId QMessageContentContainer::prependContent(const QMessageContentContainer & content)
{
    if (contentType() != QByteArray("multipart")) {
        clearContents();

        setContentType(QByteArray("multipart"));
        setContentSubType(QByteArray("mixed"));
    }

    content.applyPendingChanges();
    d_ptr->_container->prependPart(content.d_ptr->_part);

    return convert(d_ptr->_container->partAt(0).location());
}

void QMessageContentContainer::setDerivedMessage(QMessage *derived)
{
    d_ptr->setDerivedMessage(derived);
}

void QMessageContentContainer::applyPendingChanges() const
{
    d_ptr->applyPendingChanges();
}
#endif

void QMessageContentContainer::removeContent(const QMessageContentContainerId &id)
{
    QMailMessagePart::Location location(convert(id));

    if (location.isValid(false)) {
        // See if this part is a direct descendant
        for (uint i = 0; i < d_ptr->_container->partCount(); ++i) {
            if (d_ptr->_container->partAt(i).location() == location) {
                d_ptr->_container->removePartAt(i);
                return;
            }
        }

        // Otherwise, try to find it down the descendant tree
        PartRemover remover(location);
        d_ptr->_container->foreachPart<PartRemover&>(remover);
    }
}

