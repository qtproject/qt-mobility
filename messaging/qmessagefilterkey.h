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
    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options() const;

    QMessageFilterKey();
    QMessageFilterKey(const QMessageFilterKey &other);
    virtual ~QMessageFilterKey();
    bool isEmpty() const;
    bool isSupported() const;

    QMessageFilterKey operator~() const;
    QMessageFilterKey operator&(const QMessageFilterKey &other) const;
    QMessageFilterKey operator|(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator&=(const QMessageFilterKey &other);
    const QMessageFilterKey& operator|=(const QMessageFilterKey &other);

    bool operator==(const QMessageFilterKey &other) const;
    const QMessageFilterKey& operator=(const QMessageFilterKey &other);

    static QMessageFilterKey id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp);

    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey sender(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey subject(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey priority(QMessage::Priority priority, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);

    static QMessageFilterKey size(int value, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey size(int value, QMessageDataComparator::RelationComparator cmp);

    static QMessageFilterKey customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFilterKey customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFilterKey parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    static QMessageFilterKey parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFilterKey parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFilterKey ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
#endif

private:
    QMessageFilterKeyPrivate *d_ptr;
};
#endif
