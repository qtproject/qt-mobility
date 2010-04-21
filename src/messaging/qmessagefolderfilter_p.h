/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef QMESSAGEFOLDERFILTERPRIVATE_H
#define QMESSAGEFOLDERFILTERPRIVATE_H
#include "qmessagefolderfilter.h"

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include <qvariant.h>
#include "qmessagefolder.h"
#endif
#ifdef Q_OS_WIN
#include "winhelpers_p.h"
#include "qmessagemanager.h"
#endif
#include <QSet>

QTM_BEGIN_NAMESPACE

class QMessageFolderFilterPrivate
{
    Q_DECLARE_PUBLIC(QMessageFolderFilter)
public:
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    enum Criterion { 
        None = 0, 
        IdEquality, 
        IdInclusion, 
        NameEquality, 
        NameInclusion, 
        PathEquality, 
        PathInclusion, 
        AccountEquality, 
        AccountInclusion, 
        ParentEquality, 
        ParentInclusion,
        AncestorInclusion,
        ParentAccountFilter,
        ParentFolderFilter,
        AncestorFolderFilter
    };

    enum Operator { 
        Identity = 0, 
        And, 
        Or, 
        Not, 
        Nand, 
        Nor, 
        OperatorEnd 
    };
#endif

    QMessageFolderFilterPrivate(QMessageFolderFilter *folderFilter);
    ~QMessageFolderFilterPrivate();
    QMessageFolderFilter *q_ptr;

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    typedef QList<QMessageFolderFilter> SortedMessageFolderFilterList;
    
    bool filter(const QMessageFolder &messageFolder) const;
    static bool filter(const QMessageFolder &messageFolder, const QMessageFolderFilterPrivate &filter);

    static void changeComparatorValuesToOpposite(QMessageFolderFilter& filter);
    static void changeANDsAndORsToOpposite(QMessageFolderFilter& filter);
    static void applyNot(QMessageFolderFilter& filter);
    static bool lessThan(const QMessageFolderFilter filter1, const QMessageFolderFilter filter2); 
    static QMessageFolderFilterPrivate* implementation(const QMessageFolderFilter &filter);

    QMessageDataComparator::MatchFlags _matchFlags;
    
    enum Field {None = 0, ParentAccountIdFilter, Id, ParentFolderId, AncestorFolderIds, ParentAccountId, Name, Path };
    enum Comparator {Equality = 0, Inclusion};

    bool _valid;
    bool _notFilter;
    
    QMessageFolderIdList _ids;
    QVariant _value;
    QMessageAccountFilter *_accountFilter;
    QMessageFolderFilterPrivate::Field _field;

    Comparator _comparatorType;
    int _comparatorValue;
    
    QList<SortedMessageFolderFilterList> _filterList;
#else
    QMessageFolderFilterPrivate &operator=(const QMessageFolderFilterPrivate &other);
    bool operator==(const QMessageFolderFilterPrivate &other) const;

    Operator _operator;
    Criterion _criterion;
    QSet<QMessageFolderId> _ids;
    QSet<QMessageAccountId> _accountIds;
    QString _value;
    QMessageDataComparator::EqualityComparator _equality;
    QMessageDataComparator::InclusionComparator _inclusion;
    QMessageDataComparator::MatchFlags _matchFlags;
    bool _valid;
    QList<QMessageFolderFilter*> _arguments; // for bool ops
    QMessageAccountFilter *_accountFilter;
    QMessageFolderFilter *_folderFilter;
#endif

#ifdef Q_OS_WIN
    static QMessageFolderFilter preprocess(QMessageManager::Error *error, MapiSessionPtr session, const QMessageFolderFilter &filter);
    static void preprocess(QMessageManager::Error *error, MapiSessionPtr session, QMessageFolderFilter *filter);
    static bool matchesFolder(const QMessageFolderFilter &filter, const MapiFolderPtr &folder);
    static bool QMessageFolderFilterPrivate::isNonMatching(const QMessageFolderFilter &filter);
#endif
};

QTM_END_NAMESPACE
#endif
