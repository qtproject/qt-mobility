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
#ifndef QMESSAGECONTENTCONTAINER_H
#define QMESSAGECONTENTCONTAINER_H
#include <qmessageglobal.h>
#include <qmessagecontentcontainerid.h>
#include <qmessageid.h>

#include <QDataStream>
#include <QList>
#include <QString>
#include <QStringList>
#include <QTextStream>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

QTM_BEGIN_NAMESPACE

class QMessage;
class QMessageContentContainerPrivate;

class Q_MESSAGING_EXPORT QMessageContentContainer
{
    friend class QMessageContentContainerPrivate;

public:
    QMessageContentContainer();
    QMessageContentContainer(const QMessageContentContainer &other);
    virtual ~QMessageContentContainer();

    QMessageContentContainer& operator=(const QMessageContentContainer &other);

    QByteArray contentType() const;
    QByteArray contentSubType() const;
    QByteArray contentCharset() const;

    QByteArray suggestedFileName() const;

    bool isContentAvailable() const;
    int size() const;

    QString textContent() const;
    QByteArray content() const;

    void writeTextContent(QTextStream &out) const;
    void writeContent(QDataStream &out) const;

    QMessageContentContainerIdList contentIds() const;

    QMessageContentContainer find(const QMessageContentContainerId &id) const;
    bool contains(const QMessageContentContainerId &id) const;

    QString headerFieldValue(const QByteArray &name) const;
    QStringList headerFieldValues(const QByteArray &name) const;
    QList<QByteArray> headerFields() const;

private:
    friend class QMessage;
    friend class QMessagePrivate;
#ifdef Q_OS_WIN
    friend class MapiSession;
#endif

    void setDerivedMessage(QMessage *derived);

    QMessageContentContainerPrivate* d_ptr;
};

QTM_END_NAMESPACE
#endif
