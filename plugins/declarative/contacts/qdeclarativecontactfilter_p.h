/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#ifndef QDECLARATIVECONTACTFILTER_P_H
#define QDECLARATIVECONTACTFILTER_P_H
#include <qdeclarative.h>
#include <QDeclarativeExtensionPlugin>
#include "qcontactfilter.h"


QTM_USE_NAMESPACE

class QDeclarativeContactFilter : public QObject
{
    Q_OBJECT
    Q_ENUMS(FilterType)
    Q_FLAGS(MatchFlags)
public:
    QDeclarativeContactFilter(QObject *parent=0)
        :QObject(parent)
    {
        //for grouped filter: intersect /union filters
        if (parent && qobject_cast<QDeclarativeContactFilter*>(parent)) {
            connect(this, SIGNAL(filterChanged()), parent, SIGNAL(filterChanged()));
        }
    }

    enum FilterType {
        InvalidFilter = QContactFilter::InvalidFilter,
        DetailFilter = QContactFilter::ContactDetailFilter,
        DetailRangeFilter = QContactFilter::ContactDetailRangeFilter,
        ChangeLogFilter = QContactFilter::ChangeLogFilter,
        ActionFilter = QContactFilter::ActionFilter,
        RelationshipFilter = QContactFilter::RelationshipFilter,
        IntersectionFilter = QContactFilter::IntersectionFilter,
        UnionFilter = QContactFilter::UnionFilter,
        IdFilter = QContactFilter::LocalIdFilter,
        DefaultFilter = QContactFilter::DefaultFilter
    };

    FilterType type() const {
        return static_cast<FilterType>(filter().type());
    }

    enum MatchFlag {
        MatchExactly = QContactFilter::MatchExactly,
        MatchContains = QContactFilter::MatchContains,
        MatchStartsWith = QContactFilter::MatchStartsWith,
        MatchEndsWith = QContactFilter::MatchEndsWith,
        MatchFixedString = QContactFilter::MatchFixedString,
        MatchCaseSensitive = QContactFilter::MatchCaseSensitive,
        MatchPhoneNumber = QContactFilter::MatchPhoneNumber,
        MatchKeypadCollation = QContactFilter::MatchKeypadCollation
    };
    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    virtual QContactFilter filter() const
    {
        return QContactFilter();
    }

signals:
    void filterChanged();
};


QML_DECLARE_TYPE(QDeclarativeContactFilter)


#endif
