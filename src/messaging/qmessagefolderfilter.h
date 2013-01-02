/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMESSAGEFOLDERFILTER_H
#define QMESSAGEFOLDERFILTER_H
#include <qmessageglobal.h>
#include <qmessagedatacomparator.h>
#include <qmessageid.h>
#include <qmessageaccountid.h>
#include <qmessagefolderid.h>
#include <qmessageaccountfilter.h>

QTM_BEGIN_NAMESPACE

class QMessageFolderFilterPrivate;

class Q_MESSAGING_EXPORT QMessageFolderFilter
{
    friend class QMessageFolderFilterPrivate;

public:
    QMessageFolderFilter();
    QMessageFolderFilter(const QMessageFolderFilter &other);
    virtual ~QMessageFolderFilter();

    QMessageFolderFilter& operator=(const QMessageFolderFilter &other);

    void setMatchFlags(QMessageDataComparator::MatchFlags matchFlags);
    QMessageDataComparator::MatchFlags matchFlags() const;

    bool isEmpty() const;
    bool isSupported() const;

    QMessageFolderFilter operator~() const;
    QMessageFolderFilter operator&(const QMessageFolderFilter &other) const;
    QMessageFolderFilter operator|(const QMessageFolderFilter &other) const;
    const QMessageFolderFilter& operator&=(const QMessageFolderFilter &other);
    const QMessageFolderFilter& operator|=(const QMessageFolderFilter &other);

    bool operator==(const QMessageFolderFilter &other) const;
    bool operator!=(const QMessageFolderFilter &other) const;

    static QMessageFolderFilter byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilter byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilter byId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilter byName(const QString &pattern, QMessageDataComparator::LikeComparator cmp);
    static QMessageFolderFilter byName(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilter byName(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilter byPath(const QString &pattern, QMessageDataComparator::LikeComparator cmp);
    static QMessageFolderFilter byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilter byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilter byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilter byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilter byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageFolderFilter byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageFolderFilter byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageFolderFilter byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QMessageFolderFilterPrivate *d_ptr;
};

QTM_END_NAMESPACE
#endif
