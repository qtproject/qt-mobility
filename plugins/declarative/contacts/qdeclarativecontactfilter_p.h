/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

    Q_PROPERTY(FilterType type READ type NOTIFY typeChanged)

    Q_ENUMS(FilterType);
    Q_FLAGS(MatchFlags);
public:
    QDeclarativeContactFilter(QObject *parent=0)
        :QObject(parent)
    {
    }

    enum FilterType {
        InvalidFilter,
        ContactDetailFilter,
        ContactDetailRangeFilter,
        ChangeLogFilter,
        ActionFilter,
        RelationshipFilter,
        IntersectionFilter,
        UnionFilter,
        LocalIdFilter,
        DefaultFilter
    };

    FilterType type() const {
        switch (filter().type()) {
        case QContactFilter::InvalidFilter:
            return InvalidFilter;
        case QContactFilter::ContactDetailFilter:
            return ContactDetailFilter;
        case QContactFilter::ContactDetailRangeFilter:
            return ContactDetailRangeFilter;
        case QContactFilter::ChangeLogFilter:
            return ChangeLogFilter;
        case QContactFilter::ActionFilter:
            return ActionFilter;
        case QContactFilter::RelationshipFilter:
            return RelationshipFilter;
        case QContactFilter::IntersectionFilter:
            return IntersectionFilter;
        case QContactFilter::UnionFilter:
            return UnionFilter;
        case QContactFilter::LocalIdFilter:
            return LocalIdFilter;
        case QContactFilter::DefaultFilter:
            return DefaultFilter;
        }
        return InvalidFilter;
    }

    enum MatchFlag {
        MatchExactly = Qt::MatchExactly,    // 0
        MatchContains = Qt::MatchContains,  // 1
        MatchStartsWith = Qt::MatchStartsWith,  // 2
        MatchEndsWith = Qt::MatchEndsWith, // 3
        MatchFixedString = Qt::MatchFixedString, // 8
        MatchCaseSensitive = Qt::MatchCaseSensitive, // 16
        MatchPhoneNumber = 1024,
        MatchKeypadCollation = 2048
    };
    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    virtual QContactFilter filter() const
    {
        return QContactFilter();
    }

signals:
    void typeChanged();
    void valueChanged();
};


QML_DECLARE_TYPE(QDeclarativeContactFilter)


#endif
