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

QMessageContentContainer::QMessageContentContainer()
{
}

QMessageContentContainer::QMessageContentContainer(const QMessageContentContainer &other)
{
    Q_UNUSED(other)
}

const QMessageContentContainer& QMessageContentContainer::operator=(const QMessageContentContainer& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

QMessageContentContainer::~QMessageContentContainer()
{
}

QMessageContentContainerId QMessageContentContainer::containerId() const
{
    return QMessageContentContainerId();
}

QMessageId QMessageContentContainer::messageId() const
{
    return QMessageId(); // stub
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentType(const QByteArray &data)
{
    Q_UNUSED(data)
}
#endif

QByteArray QMessageContentContainer::contentType() const
{
    return QByteArray(); // stub
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentSubType(const QByteArray &data)
{
    Q_UNUSED(data);
}
#endif

QByteArray QMessageContentContainer::contentSubType() const
{
    return QByteArray(); // stub
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentCharset(const QByteArray &data)
{
    Q_UNUSED(data)
}
#endif

QByteArray QMessageContentContainer::contentCharset() const
{
    return QByteArray(); // stub
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::setContentFileName(const QByteArray &data)
{
    Q_UNUSED(data)
}
#endif

QByteArray QMessageContentContainer::contentFileName() const
{
    return QByteArray(); // stub
}

bool QMessageContentContainer::contentAvailable() const
{
    return false;
}

void QMessageContentContainer::setIndicativeSize(uint size)
{
    Q_UNUSED(size)
}

uint QMessageContentContainer::indicativeSize() const
{
    return 0;
}

QString QMessageContentContainer::decodedTextContent() const
{
    return QString::null;
}

QByteArray QMessageContentContainer::decodedContent() const
{
    return QByteArray(); // stub
}

QString QMessageContentContainer::decodedContentFileName() const
{
    return QString(); // stub
}

void QMessageContentContainer::writeContentTo(QDataStream& out) const
{
    Q_UNUSED(out)
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::clearContents()
{
}

void QMessageContentContainer::setContent(const QString &text)
{
    Q_UNUSED(text)
}

void QMessageContentContainer::setContent(const QByteArray &data)
{
    Q_UNUSED(data)
}

void QMessageContentContainer::setContentFromFile(const QString &fileName)
{
    Q_UNUSED(fileName)
}

void QMessageContentContainer::readContentFrom(QDataStream &in)
{
    Q_UNUSED(in)
}

QMessageContentContainerId QMessageContentContainer::appendContent(const QMessageContentContainer & content)
{
    Q_UNUSED(content)
    return QMessageContentContainerId(); // stub
}

void QMessageContentContainer::replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer & content)
{
    Q_UNUSED(id)
    Q_UNUSED(content)
}
#endif

QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    return QMessageContentContainerIdList(); // stub
}

const QMessageContentContainer QMessageContentContainer::container(const QMessageContentContainerId id) const
{
    Q_UNUSED(id)
    return QMessageContentContainer(); // stub
}

QMessageContentContainer QMessageContentContainer::container(const QMessageContentContainerId id)
{
    Q_UNUSED(id)
    return QMessageContentContainer(); // stub
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QString &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

void QMessageContentContainer::setHeaderField(const QByteArray &name, const QString &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}
#endif

QString QMessageContentContainer::headerField(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QString(); // stub
}

QList<QString> QMessageContentContainer::headerFieldValues(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QList<QString>(); // stub
}

QList<QByteArray> QMessageContentContainer::headerFields() const
{
    return QList<QByteArray>(); // stub
}

#ifdef QMESSAGING_OPTIONAL
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QByteArray &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

void QMessageContentContainer::setHeaderField(const QByteArray &name, const QByteArray &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}
#endif

void QMessageContentContainer::setPreferredCharsets(const QList<QByteArray> &charsetNames)
{
    Q_UNUSED(charsetNames);
}

QList<QByteArray> QMessageContentContainer::preferredCharsets()
{
    return QList<QByteArray>(); // stub;
}

#ifdef QMESSAGING_OPTIONAL
bool QMessageContentContainer::containerDataModified() const
{
    return false; // stub
}

void QMessageContentContainer::setContainerDataModified(bool modified)
{
    Q_UNUSED(modified);
}

QMessageContentContainerId QMessageContentContainer::prependContent(const QMessageContentContainer & content)
{
    Q_UNUSED(content);
    return QMessageContentContainerId(); // stub
}
#endif
