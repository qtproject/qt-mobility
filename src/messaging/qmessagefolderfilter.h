/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

    static QMessageFolderFilter byName(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageFolderFilter byName(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

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
