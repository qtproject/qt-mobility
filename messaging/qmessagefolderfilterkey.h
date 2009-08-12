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
#ifndef QMESSAGEFOLDERFILTERKEY_H
#define QMESSAGEFOLDERFILTERKEY_H
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include <qmessageglobal.h>
#include <qmessagedatacomparator.h>
#include <qmessageid.h>
#include <qmessageaccountid.h>
#include <qmessagefolderid.h>
#include <qmessageaccountfilterkey.h>

class QMessageFolderFilterKeyPrivate;

class Q_MESSAGING_EXPORT QMessageFolderFilterKey
{
    Q_DECLARE_PRIVATE(QMessageFolderFilterKey)

public:
    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options() const;

    QMessageFolderFilterKey();
    QMessageFolderFilterKey(const QMessageFolderFilterKey &other);
    virtual ~QMessageFolderFilterKey();
    bool isEmpty() const;

    QMessageFolderFilterKey operator~() const;
    QMessageFolderFilterKey operator&(const QMessageFolderFilterKey &other) const;
    QMessageFolderFilterKey operator|(const QMessageFolderFilterKey &other) const;
    const QMessageFolderFilterKey& operator&=(const QMessageFolderFilterKey &other);
    const QMessageFolderFilterKey& operator|=(const QMessageFolderFilterKey &other);

    bool operator==(const QMessageFolderFilterKey &other) const;
    const QMessageFolderFilterKey& operator=(const QMessageFolderFilterKey &other);

    static QMessageFolderFilterKey id(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey id(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey id(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey displayName(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilterKey displayName(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey path(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilterKey path(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilterKey parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilterKey parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilterKey ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QMessageFolderFilterKeyPrivate *d_ptr;
};
#endif
#endif
