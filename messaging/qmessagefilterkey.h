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
#ifndef QMESSAGEFILTERKEY_H
#define QMESSAGEFILTERKEY_H
#include <qmessageglobal.h>
#include <qmessagedatacomparator.h>
#include <qmessageid.h>
#include <qmessageaccountid.h>
#include <qmessagefolderid.h>
#include <qmessageaccountfilterkey.h>
#include <qmessagefolderfilterkey.h>
#include <qmessage.h>

class QMessageFilterKeyPrivate;

class Q_MESSAGING_EXPORT QMessageFilterKey
{
    Q_DECLARE_PRIVATE(QMessageFilterKey)

public:
    QMessageFilterKey();
    QMessageFilterKey(const QMessageFilterKey &other);
    virtual ~QMessageFilterKey();

    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options() const;

    bool isEmpty() const;
    bool isSupported() const;

    QMessageFilterKey operator~() const;
    QMessageFilterKey operator&(const QMessageFilterKey &other) const;
    QMessageFilterKey operator|(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator&=(const QMessageFilterKey &other);
    const QMessageFilterKey& operator|=(const QMessageFilterKey &other);

    bool operator==(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator=(const QMessageFilterKey &other);

    static QMessageFilterKey byId(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey byId(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey byId(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey byType(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey byType(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp);

    static QMessageFilterKey bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey byRecipients(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey byRecipients(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey bySubject(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey bySubject(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey byTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey byTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey byStatus(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey byStatus(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey byPriority(QMessage::Priority priority, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);

    static QMessageFilterKey bySize(int value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey bySize(int value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey byCustomField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey byCustomField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey byParentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

#ifdef QMESSAGING_OPTIONAL_FOLDER
    static QMessageFilterKey byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey byParentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey byAncestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
#endif

private:
    QMessageFilterKeyPrivate *d_ptr;
};
#endif
