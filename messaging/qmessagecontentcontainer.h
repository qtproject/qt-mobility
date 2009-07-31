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
#ifndef QMESSAGECONTENTCONTAINER_H
#define QMESSAGECONTENTCONTAINER_H
#include <QString>
#include <QList>
#include <QDataStream>
#include <qmessageglobal.h>
#include <qmessagecontentcontainerid.h>
#include <qmessageid.h>

class QMessage;
class QMessageContentContainerPrivate;

class Q_MESSAGING_EXPORT QMessageContentContainer
{
    Q_DECLARE_PRIVATE(QMessageContentContainer)

public:
    QMessageContentContainer();
    QMessageContentContainer(const QMessageContentContainer &other);
    const QMessageContentContainer& operator=(const QMessageContentContainer &other);
    virtual ~QMessageContentContainer();

    virtual QMessageContentContainerId containerId() const;
    virtual QMessageId messageId() const;

    virtual QByteArray contentType() const;
    virtual QByteArray contentSubType() const;
    virtual QByteArray contentCharset() const;
    virtual QByteArray contentFileName() const;

    virtual bool contentAvailable() const;
    virtual uint size() const;

    virtual QString decodedTextContent() const;
    virtual QByteArray decodedContent() const;
    virtual QString decodedContentFileName() const;
    virtual void writeContentTo(QDataStream &out) const;

    virtual QMessageContentContainerIdList contentIds() const;

    virtual QMessageContentContainer container(const QMessageContentContainerId &id) const;
    virtual bool contains(const QMessageContentContainerId &id) const;

    virtual QString headerField(const QByteArray &name) const;
    virtual QList<QString> headerFieldValues(const QByteArray &name) const;
    virtual QList<QByteArray> headerFields() const;

    static void setPreferredCharsets(const QList<QByteArray> &charsetNames);
    static QList<QByteArray> preferredCharsets();

#ifdef QMESSAGING_OPTIONAL
    virtual void setContentType(const QByteArray &data);
    virtual void setContentSubType(const QByteArray &data);
    virtual void setContentCharset(const QByteArray &data);
    virtual void setContentFileName(const QByteArray &data);
    virtual void setContent(const QString &text);
    virtual void setContent(const QByteArray &data);
    virtual void setContentFromFile(const QString &fileName);
    virtual void readContentFrom(QDataStream &in);
    virtual QMessageContentContainerId appendContent(const QMessageContentContainer &content);
    virtual void replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer &content);
    virtual void clearContents();
    virtual void appendHeaderField(const QByteArray &name, const QString &value);
    virtual void setHeaderField(const QByteArray &name, const QString &value);
    virtual void appendHeaderField(const QByteArray &name, const QByteArray &value);
    virtual void setHeaderField(const QByteArray &name, const QByteArray &value);
    virtual bool containerDataModified() const;

protected:
    virtual QMessageContentContainerId prependContent(const QMessageContentContainer &content);
#endif

private:
    friend class QMessage;
    friend class QMessageStore;

    void setDerivedMessage(QMessage *derived);
    void applyPendingChanges() const;

    QMessageContentContainerPrivate *d_ptr;
};
#endif
