/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

class QDeclarativeContactCompoundFilter : public QDeclarativeContactFilter
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeContactFilter> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    explicit QDeclarativeContactCompoundFilter(QObject* parent = 0) : QDeclarativeContactFilter(parent){}
    virtual ~QDeclarativeContactCompoundFilter() {}
    // 'READ' accessor for the filters, basically this is also a 'WRITE' accessor
    // as per QDeclarativeListProperty's design.
    QDeclarativeListProperty<QDeclarativeContactFilter> filters();

    static void filters_append(QDeclarativeListProperty<QDeclarativeContactFilter>* prop, QDeclarativeContactFilter* filter);
    static int filters_count(QDeclarativeListProperty<QDeclarativeContactFilter>* prop);
    static QDeclarativeContactFilter* filters_at(QDeclarativeListProperty<QDeclarativeContactFilter>* prop, int index);
    static void filters_clear(QDeclarativeListProperty<QDeclarativeContactFilter>* prop);

protected:
    QList<QDeclarativeContactFilter*> m_filters;
};

QML_DECLARE_TYPE(QDeclarativeContactFilter)
QML_DECLARE_TYPE(QDeclarativeContactCompoundFilter)


#endif
