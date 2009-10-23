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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderfilter.h"
#include "winhelpers_p.h"

#include <QSet>

class QMessageFolderFilterPrivate
{
    Q_DECLARE_PUBLIC(QMessageFolderFilter)
public:
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
        AncestorInclusion
    };

    QMessageFolderFilterPrivate(QMessageFolderFilter *folderFilter)
        : q_ptr(folderFilter),
          _criterion(None),
          _equality(QMessageDataComparator::Equal),
          _inclusion(QMessageDataComparator::Includes),
          _options(0)
    {
    }

    QMessageFolderFilter *q_ptr;

    QMessageFolderFilterPrivate &operator=(const QMessageFolderFilterPrivate &other)
    {
        _criterion = other._criterion;
        _ids = other._ids;
        _accountIds = other._accountIds;
        _value = other._value;
        _equality = other._equality;
        _inclusion = other._inclusion;
        _options = other._options;

        return *this;
    }

    bool operator==(const QMessageFolderFilterPrivate &other) const
    {
        return ((_criterion == other._criterion) &&
                (_ids == other._ids) &&
                (_accountIds == other._accountIds) &&
                (_value == other._value) &&
                (_equality == other._equality) &&
                (_inclusion == other._inclusion) &&
                (_options == other._options));
    }

    Criterion _criterion;
    QSet<QMessageFolderId> _ids;
    QSet<QMessageAccountId> _accountIds;
    QString _value;
    QMessageDataComparator::EqualityComparator _equality;
    QMessageDataComparator::InclusionComparator _inclusion;
    QMessageDataComparator::Options _options;

#ifdef Q_OS_WIN
    static bool matchesStore(const QMessageFolderFilter &filter, const MapiStorePtr &store);
    static bool matchesFolder(const QMessageFolderFilter &filter, const MapiFolderPtr &folder);
#endif
};
#endif
