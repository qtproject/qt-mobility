/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEGALLERYFILTER_H
#define QDECLARATIVEGALLERYFILTER_H

#include <qmobilityglobal.h>
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
};

class QDeclarativeGalleryFilterUnionItem : public QDeclarativeGalleryFilterBase
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryFilterUnionItem(QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
    {
    }
};

class QDeclarativeGalleryFilterIntersectionItem : public QDeclarativeGalleryFilterUnionItem
{
    Q_OBJECT
public:
    explicit QDeclarativeGalleryFilterIntersectionItem(QObject *parent = 0)
        : QDeclarativeGalleryFilterUnionItem(parent)
    {
    }
};

class QDeclarativeGalleryFilter : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(MatchFlags matchFlags READ matchFlags WRITE setMatchFlags)
    Q_FLAGS(MatchFlags)
public:
    enum MatchFlag
    {
        MatchExactly    = Qt::MatchExactly,
        Contains        = Qt::MatchContains,
        StartsWith      = Qt::MatchStartsWith,
        EndsWith        = Qt::MatchEndsWith,
        RegExp          = Qt::MatchRegExp,
        Wildcard        = Qt::MatchWildcard,
        FixedString     = Qt::MatchFixedString,
        CaseSensitive   = Qt::MatchCaseSensitive,
    };

    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    explicit QDeclarativeGalleryFilter(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
        , m_matchFlags(Qt::MatchExactly)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant value() const { return m_value; }
    void setValue(const QVariant &value) { m_value = value; }

    MatchFlags matchFlags() const { return m_matchFlags; }
    void setMatchFlags(MatchFlags flags) { m_matchFlags = flags; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_value;
    MatchFlags m_matchFlags;
};

class QDeclarativeGalleryFilterLessThan : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit QDeclarativeGalleryFilterLessThan(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant value() const { return m_value; }
    void setValue(const QVariant &value) { m_value = value; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_value;
};

class QDeclarativeGalleryFilterLessThanEquals : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit QDeclarativeGalleryFilterLessThanEquals(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant value() const { return m_value; }
    void setValue(const QVariant &value) { m_value = value; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_value;
};

class QDeclarativeGalleryFilterGreaterThan : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit QDeclarativeGalleryFilterGreaterThan(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant value() const { return m_value; }
    void setValue(const QVariant &value) { m_value = value; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_value;
};

class QDeclarativeGalleryFilterGreaterThanEquals : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit QDeclarativeGalleryFilterGreaterThanEquals(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant value() const { return m_value; }
    void setValue(const QVariant &value) { m_value = value; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_value;
};

class QDeclarativeGalleryFilterExclusiveRange : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant minimum READ minimumValue WRITE setMinimumValue)
    Q_PROPERTY(QVariant maximum READ maximumValue WRITE setMaximumValue)
public:
    explicit QDeclarativeGalleryFilterExclusiveRange(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant minimumValue() const { return m_minimumValue; }
    void setMinimumValue(const QVariant &minimum) { m_minimumValue = minimum; }

    QVariant maximumValue() const { return m_maximumValue; }
    void setMaximumValue(const QVariant &maximum) { m_maximumValue = maximum; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_minimumValue;
    QVariant m_maximumValue;
};

class QDeclarativeGalleryFilterInclusiveRange : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant minimum READ minimumValue WRITE setMinimumValue)
    Q_PROPERTY(QVariant maximum READ maximumValue WRITE setMaximumValue)
public:
    explicit QDeclarativeGalleryFilterInclusiveRange(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant minimumValue() const { return m_minimumValue; }
    void setMinimumValue(const QVariant &minimum) { m_minimumValue = minimum; }

    QVariant maximumValue() const { return m_maximumValue; }
    void setMaximumValue(const QVariant &maximum) { m_maximumValue = maximum; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_minimumValue;
    QVariant m_maximumValue;
};

class QDeclarativeGalleryFilterUnion : public QDeclarativeGalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGalleryFilterUnionItem> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")
public:
    explicit QDeclarativeGalleryFilterUnion(QObject *parent = 0)
        : QDeclarativeGalleryFilterIntersectionItem(parent)
    {
    }

    QDeclarativeListProperty<QDeclarativeGalleryFilterUnionItem> filters() {
        return QDeclarativeListProperty<QDeclarativeGalleryFilterUnionItem>(this, m_filters); }

    QGalleryFilter filter() const;

private:
    QList<QDeclarativeGalleryFilterUnionItem *> m_filters;
};

class QDeclarativeGalleryFilterIntersection : public QDeclarativeGalleryFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGalleryFilterIntersectionItem> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")
public:
    explicit QDeclarativeGalleryFilterIntersection(QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
    {
    }

    QDeclarativeListProperty<QDeclarativeGalleryFilterIntersectionItem> filters() {
        return QDeclarativeListProperty<QDeclarativeGalleryFilterIntersectionItem>(this, m_filters); }

    QGalleryFilter filter() const;

private:
    QList<QDeclarativeGalleryFilterIntersectionItem *> m_filters;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterBase))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterUnionItem))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterIntersectionItem))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterLessThan))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterLessThanEquals))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterGreaterThan))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterGreaterThanEquals))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterExclusiveRange))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterInclusiveRange))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterUnion))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterIntersection))

#endif
