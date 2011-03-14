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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGALLERYFILTER_H
#define QDECLARATIVEGALLERYFILTER_H

#include <qgalleryfilter.h>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QGalleryFilter;

class QDeclarativeGalleryFilterBase : public QObject
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryFilterBase(QObject *parent = 0)
        : QObject(parent)
    {
    }

    virtual QGalleryFilter filter() const = 0;

Q_SIGNALS:
    void filterChanged();
};

enum Comparator
{
    Equals,
    LessThan,
    GreaterThan,
    LessThanEquals,
    GreaterThanEquals,
    Contains,
    StartsWith,
    EndsWith,
    Wildcard,
    RegExp
};

class QDeclarativeGalleryValueFilter : public QDeclarativeGalleryFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName NOTIFY propertyNameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool negated READ isNegated WRITE setNegated NOTIFY negatedChanged)
public:
    QString propertyName() const { return m_filter.propertyName(); }
    void setPropertyName(const QString &name);

    QVariant value() const { return m_filter.value(); }
    void setValue(const QVariant &value);

    bool isNegated() const { return m_filter.isNegated(); }
    void setNegated(bool negated);

    QGalleryFilter filter() const;

Q_SIGNALS:
    void propertyNameChanged();
    void valueChanged();
    void negatedChanged();

protected:
    explicit QDeclarativeGalleryValueFilter(
            QGalleryFilter::Comparator comparator, QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
    {
        m_filter.setComparator(comparator);
    }

    QGalleryMetaDataFilter m_filter;
};

class QDeclarativeGalleryStringFilter : public QDeclarativeGalleryFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName NOTIFY propertyNameChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool negated READ isNegated WRITE setNegated NOTIFY negatedChanged)
public:
    QString propertyName() const { return m_filter.propertyName(); }
    void setPropertyName(const QString &name);

    QString value() const { return m_filter.value().toString(); }
    void setValue(const QString &value);

    bool isNegated() const { return m_filter.isNegated(); }
    void setNegated(bool negated);

    QGalleryFilter filter() const;

Q_SIGNALS:
    void propertyNameChanged();
    void valueChanged();
    void negatedChanged();

protected:
    explicit QDeclarativeGalleryStringFilter(
            QGalleryFilter::Comparator comparator, QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
    {
        m_filter.setComparator(comparator);
    }

    QGalleryMetaDataFilter m_filter;
};

class QDeclarativeGalleryEqualsFilter : public QDeclarativeGalleryValueFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryEqualsFilter(QObject *parent = 0)
        : QDeclarativeGalleryValueFilter(QGalleryFilter::Equals, parent)
    {
    }

    QGalleryFilter filter() const;
};

class QDeclarativeGalleryLessThanFilter : public QDeclarativeGalleryValueFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryLessThanFilter(QObject *parent = 0)
        : QDeclarativeGalleryValueFilter(QGalleryFilter::LessThan, parent)
    {
    }
};

class QDeclarativeGalleryLessThanEqualsFilter : public QDeclarativeGalleryValueFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryLessThanEqualsFilter(QObject *parent = 0)
        : QDeclarativeGalleryValueFilter(QGalleryFilter::LessThanEquals, parent)
    {
    }
};

class QDeclarativeGalleryGreaterThanFilter : public QDeclarativeGalleryValueFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryGreaterThanFilter(QObject *parent = 0)
        : QDeclarativeGalleryValueFilter(QGalleryFilter::GreaterThan, parent)
    {
    }
};

class QDeclarativeGalleryGreaterThanEqualsFilter : public QDeclarativeGalleryValueFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryGreaterThanEqualsFilter(QObject *parent = 0)
        : QDeclarativeGalleryValueFilter(QGalleryFilter::GreaterThanEquals, parent)
    {
    }
};

class QDeclarativeGalleryContainsFilter : public QDeclarativeGalleryStringFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryContainsFilter(QObject *parent = 0)
        : QDeclarativeGalleryStringFilter(QGalleryFilter::Contains, parent)
    {
    }
};

class QDeclarativeGalleryStartsWithFilter : public QDeclarativeGalleryStringFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryStartsWithFilter(QObject *parent = 0)
        : QDeclarativeGalleryStringFilter(QGalleryFilter::StartsWith, parent)
    {
    }
};


class QDeclarativeGalleryEndsWithFilter : public QDeclarativeGalleryStringFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryEndsWithFilter(QObject *parent = 0)
        : QDeclarativeGalleryStringFilter(QGalleryFilter::EndsWith, parent)
    {
    }
};

class QDeclarativeGalleryWildcardFilter : public QDeclarativeGalleryStringFilter
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryWildcardFilter(QObject *parent = 0)
        : QDeclarativeGalleryStringFilter(QGalleryFilter::Wildcard, parent)
    {
    }
};

class QDeclarativeGalleryFilterGroup
    : public QDeclarativeGalleryFilterBase
    , public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGalleryFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")
public:
    explicit QDeclarativeGalleryFilterGroup(QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
        , m_complete(false)
    {
    }

    void classBegin();
    void componentComplete();

    QDeclarativeListProperty<QDeclarativeGalleryFilterBase> filters();

protected:
    QList<QDeclarativeGalleryFilterBase *> m_filters;

private:
    bool m_complete;

    static void append(
            QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters,
            QDeclarativeGalleryFilterBase *filter);
    static int count(QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters);
    static QDeclarativeGalleryFilterBase *at(
            QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters, int index);
    static void clear(QDeclarativeListProperty<QDeclarativeGalleryFilterBase> *filters);
};

class QDeclarativeGalleryFilterUnion : public QDeclarativeGalleryFilterGroup
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryFilterUnion(QObject *parent = 0)
        : QDeclarativeGalleryFilterGroup(parent)
    {
    }

    QGalleryFilter filter() const;
};

class QDeclarativeGalleryFilterIntersection : public QDeclarativeGalleryFilterGroup
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryFilterIntersection(QObject *parent = 0)
        : QDeclarativeGalleryFilterGroup(parent)
    {
    }

    QGalleryFilter filter() const;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterBase))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryEqualsFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryLessThanFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryLessThanEqualsFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryGreaterThanFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryGreaterThanEqualsFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryContainsFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryStartsWithFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryEndsWithFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryWildcardFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterUnion))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterIntersection))

#endif
