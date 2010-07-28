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

QTM_BEGIN_NAMESPACE

QMessageContentContainer::QMessageContentContainer()
{
}

QMessageContentContainer::QMessageContentContainer(const QMessageContentContainer &other)
{
    Q_UNUSED(other)
}

QMessageContentContainer& QMessageContentContainer::operator=(const QMessageContentContainer& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

QMessageContentContainer::~QMessageContentContainer()
{
}

QByteArray QMessageContentContainer::contentType() const
{
    return QByteArray(); // stub
}

QByteArray QMessageContentContainer::contentSubType() const
{
    return QByteArray(); // stub
}

QByteArray QMessageContentContainer::contentCharset() const
{
    return QByteArray(); // stub
}

QByteArray QMessageContentContainer::suggestedFileName() const
{
    return QByteArray(); // stub
}

bool QMessageContentContainer::isContentAvailable() const
{
    return false;
}

int QMessageContentContainer::size() const
{
    return 0;
}

QString QMessageContentContainer::textContent() const
{
    return QString();
}

QByteArray QMessageContentContainer::content() const
{
    return QByteArray(); // stub
}

void QMessageContentContainer::writeTextContent(QTextStream& out) const
{
    Q_UNUSED(out)
}

void QMessageContentContainer::writeContent(QDataStream& out) const
{
    Q_UNUSED(out)
}

QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    return QMessageContentContainerIdList(); // stub
}

QMessageContentContainer QMessageContentContainer::find(const QMessageContentContainerId &id) const
{
    Q_UNUSED(id)
    return QMessageContentContainer(); // stub
}

bool QMessageContentContainer::contains(const QMessageContentContainerId &id) const
{
    Q_UNUSED(id)
    return false; // stub
}

QString QMessageContentContainer::headerFieldValue(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QString(); // stub
}

QStringList QMessageContentContainer::headerFieldValues(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QStringList(); // stub
}

QList<QByteArray> QMessageContentContainer::headerFields() const
{
    return QList<QByteArray>(); // stub
}

void QMessageContentContainer::setDerivedMessage(QMessage *derived)
{
    Q_UNUSED(derived)
}

QTM_END_NAMESPACE
