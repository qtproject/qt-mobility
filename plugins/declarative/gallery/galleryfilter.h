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

#ifndef GALLERYFILTER_H
#define GALLERYFILTER_H

#include <qmobilityglobal.h>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QGalleryFilter;

class GalleryFilterBase : public QObject
{
    Q_OBJECT
public:
    explicit GalleryFilterBase(QObject *parent = 0)
        : QObject(parent)
    {
    }

    virtual QGalleryFilter filter() const = 0;
};

class GalleryFilterUnionItem : public GalleryFilterBase
{
    Q_OBJECT
public:
    explicit GalleryFilterUnionItem(QObject *parent = 0)
        : GalleryFilterBase(parent)
    {
    }
};

class GalleryFilterIntersectionItem : public GalleryFilterUnionItem
{
    Q_OBJECT
public:
    explicit GalleryFilterIntersectionItem(QObject *parent = 0)
        : GalleryFilterUnionItem(parent)
    {
    }
};

class GalleryFilter : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(MatchFlags matchFlags READ matchFlags WRITE setMatchFlags)
    Q_FLAGS(MatchFlags)
public:
    enum MatchFlag
    {
        MatchExactly        = Qt::MatchExactly,
        MatchContains       = Qt::MatchContains,
        MatchStartsWith     = Qt::MatchStartsWith,
        MatchEndsWith       = Qt::MatchEndsWith,
        MatchRegExp         = Qt::MatchRegExp,
        MatchWildcard       = Qt::MatchWildcard,
        MatchFixedString    = Qt::MatchFixedString,
        MatchCaseSensitive  = Qt::MatchCaseSensitive,
    };

    Q_DECLARE_FLAGS(MatchFlags, MatchFlag)

    explicit GalleryFilter(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
        , m_matchFlags(Qt::MatchExactly)
    {
    }

    QString propertyName() const { return m_propertyName; }
    void setPropertyName(const QString &name) { m_propertyName = name; }

    QVariant value() const { return m_value; }
    void setValue(const QVariant &value) { m_value = value; }

    MatchFlags matchFlags() const;
    void setMatchFlags(MatchFlags flags) { m_matchFlags = flags; }

    QGalleryFilter filter() const;

private:
    QString m_propertyName;
    QVariant m_value;
    MatchFlags m_matchFlags;
};

class GalleryFilterLessThan : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit GalleryFilterLessThan(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
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

class GalleryFilterLessThanEquals : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit GalleryFilterLessThanEquals(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
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

class GalleryFilterGreaterThan : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit GalleryFilterGreaterThan(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
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

class GalleryFilterGreaterThanEquals : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
public:
    explicit GalleryFilterGreaterThanEquals(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
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

class GalleryFilterExclusiveRange : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant minimum READ minimumValue WRITE setMinimumValue)
    Q_PROPERTY(QVariant maximum READ maximumValue WRITE setMaximumValue)
public:
    explicit GalleryFilterExclusiveRange(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
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

class GalleryFilterInclusiveRange : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName)
    Q_PROPERTY(QVariant minimum READ minimumValue WRITE setMinimumValue)
    Q_PROPERTY(QVariant maximum READ maximumValue WRITE setMaximumValue)
public:
    explicit GalleryFilterInclusiveRange(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
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

class GalleryFilterUnion : public GalleryFilterIntersectionItem
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<GalleryFilterUnionItem> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")
public:
    explicit GalleryFilterUnion(QObject *parent = 0)
        : GalleryFilterIntersectionItem(parent)
    {
    }

    QDeclarativeListProperty<GalleryFilterUnionItem> filters() {
        return QDeclarativeListProperty<GalleryFilterUnionItem>(this, m_filters); }

    QGalleryFilter filter() const;

private:
    QList<GalleryFilterUnionItem *> m_filters;
};

class GalleryFilterIntersection : public GalleryFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<GalleryFilterIntersectionItem> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")
public:
    explicit GalleryFilterIntersection(QObject *parent = 0)
        : GalleryFilterBase(parent)
    {
    }

    QDeclarativeListProperty<GalleryFilterIntersectionItem> filters() {
        return QDeclarativeListProperty<GalleryFilterIntersectionItem>(this, m_filters); }

    QGalleryFilter filter() const;

private:
    QList<GalleryFilterIntersectionItem *> m_filters;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterBase))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterUnionItem))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterIntersectionItem))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterLessThan))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterLessThanEquals))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterGreaterThan))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterGreaterThanEquals))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterExclusiveRange))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterInclusiveRange))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterUnion))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(GalleryFilterIntersection))

#endif // DECLARATIVEGALLERYFILTER_H
