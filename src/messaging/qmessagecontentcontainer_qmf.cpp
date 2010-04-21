/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagecontentcontainer_p.h"

#include <QTextCodec>
#include <QDebug>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);

QTM_BEGIN_NAMESPACE

namespace {

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

struct SizeAccumulator
{
    int _size;

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

QMessageContentContainer& QMessageContentContainer::operator=(const QMessageContentContainer& other)
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

QByteArray QMessageContentContainer::contentType() const
{
    if (!d_ptr->_type.isEmpty()) {
        return d_ptr->_type;
    }
    return d_ptr->_container->contentType().type();
}

QByteArray QMessageContentContainer::contentSubType() const
{
    if (!d_ptr->_subType.isEmpty()) {
        return d_ptr->_subType;
    }
    return d_ptr->_container->contentType().subType();
}

QByteArray QMessageContentContainer::contentCharset() const
{
    if (!d_ptr->_charset.isEmpty()) {
        return d_ptr->_charset;
    }
    return d_ptr->_container->contentType().charset();
}

QByteArray QMessageContentContainer::suggestedFileName() const
{
    if (d_ptr->_container == &d_ptr->_part) {
        // Return the display name for the part
        return d_ptr->_part.displayName().toAscii();
    }

    return QByteArray();
}

bool QMessageContentContainer::isContentAvailable() const
{
    if (!d_ptr->_content.isEmpty()) {
        return true;
    }
    return (d_ptr->_container->partCount() > 0) || d_ptr->_container->contentAvailable();
}

int QMessageContentContainer::size() const
{
    if (d_ptr->_container->hasBody()) {
        return d_ptr->_container->body().length();
    }

    SizeAccumulator accumulator;
    d_ptr->_container->foreachPart<SizeAccumulator&>(accumulator);

    return accumulator._size;
}

QString QMessageContentContainer::textContent() const
{
    return d_ptr->_container->body().data();
}

QByteArray QMessageContentContainer::content() const
{
    return d_ptr->_container->body().data(QMailMessageBody::Decoded);
}

void QMessageContentContainer::writeTextContent(QTextStream& out) const
{
    if (d_ptr->_container->hasBody()) {
        d_ptr->_container->body().toStream(out);
    }
}

void QMessageContentContainer::writeContent(QDataStream& out) const
{
    if (d_ptr->_container->hasBody()) {
        d_ptr->_container->body().toStream(out, QMailMessageBody::Decoded);
    }
}

QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    QMessageContentContainerIdList ids;

    for (uint i = 0; i < d_ptr->_container->partCount(); ++i) {
        ids.append(convert(d_ptr->_container->partAt(i).location()));
    }

    return ids;
}

QMessageContentContainer QMessageContentContainer::find(const QMessageContentContainerId &id) const
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

QString QMessageContentContainer::headerFieldValue(const QByteArray &name) const
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

void QMessageContentContainer::setDerivedMessage(QMessage *derived)
{
    d_ptr->setDerivedMessage(derived);
}

QTM_END_NAMESPACE
