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
#ifndef QMESSAGECONTENTCONTAINER_H
#define QMESSAGECONTENTCONTAINER_H
#include <QString>
#include <QList>
#include <QDataStream>
#include <qmessageglobal.h>
#include <qmessagecontentcontainerid.h>
#include <qmessageid.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class QMessage;
class QMessageContentContainerPrivate;

class Q_MESSAGING_EXPORT QMessageContentContainer
{
    Q_DECLARE_PRIVATE(QMessageContentContainer)

public:
    QMessageContentContainer();
    QMessageContentContainer(const QMessageContentContainer &other);
    virtual ~QMessageContentContainer();

    const QMessageContentContainer& operator=(const QMessageContentContainer &other);

    QMessageContentContainerId containerId() const;
    QMessageId messageId() const;

    QByteArray contentType() const;
    QByteArray contentSubType() const;
    QByteArray contentCharset() const;
    QByteArray contentFileName() const;

    bool isContentAvailable() const;
    uint size() const;

    QString decodedTextContent() const;
    QByteArray decodedContent() const;
    QString decodedContentFileName() const;
    void writeContentTo(QDataStream &out) const;

    QMessageContentContainerIdList contentIds() const;

    QMessageContentContainer container(const QMessageContentContainerId &id) const;
    bool contains(const QMessageContentContainerId &id) const;

    QString headerField(const QByteArray &name) const;
    QStringList headerFieldValues(const QByteArray &name) const;
    QList<QByteArray> headerFields() const;

    static void setPreferredCharsets(const QList<QByteArray> &charsetNames);
    static QList<QByteArray> preferredCharsets();

#ifdef Q_OS_WIN
    static QMessageContentContainer from(const QMessageId &id, ULONG number);
#endif

#ifdef QMESSAGING_OPTIONAL
    void setContentType(const QByteArray &data);
    void setContentSubType(const QByteArray &data);
    void setContentCharset(const QByteArray &data);
    void setContentFileName(const QByteArray &data);
    void setContent(const QString &text);
    void setContent(const QByteArray &data);
    void setContentFromFile(const QString &fileName);
    void readContentFrom(QDataStream &in);
    QMessageContentContainerId appendContent(const QMessageContentContainer &content);
    void replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer &content);
    void clearContents();
    void appendHeaderField(const QByteArray &name, const QString &value);
    void setHeaderField(const QByteArray &name, const QString &value);
    void appendHeaderField(const QByteArray &name, const QByteArray &value);
    void setHeaderField(const QByteArray &name, const QByteArray &value);
    bool containerDataModified() const;

protected:
    QMessageContentContainerId prependContent(const QMessageContentContainer &content);
#endif

protected:
    void removeContent(const QMessageContentContainerId &id);

private:
    friend class QMessage;
    friend class QMessagePrivate;

    void setDerivedMessage(QMessage *derived);
    void applyPendingChanges() const;

    QMessageContentContainerPrivate *d_ptr;
};
#endif
