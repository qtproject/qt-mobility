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
};

class QDeclarativeGalleryFilter : public QDeclarativeGalleryFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QString property READ propertyName WRITE setPropertyName NOTIFY propertyNameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(Comparator comparator READ comparator WRITE setComparator NOTIFY comparatorChanged)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)
    Q_PROPERTY(bool inverted READ isInverted WRITE setInverted NOTIFY invertedChanged)
    Q_ENUMS(Comparator comparator)
public:
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

    explicit QDeclarativeGalleryFilter(QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
    {
    }

    QString propertyName() const { return m_filter.propertyName(); }
    void setPropertyName(const QString &name) {
        m_filter.setPropertyName(name); emit propertyNameChanged(); }

    QVariant value() const { return m_filter.value(); }
    void setValue(const QVariant &value) { m_filter.setValue(value); emit valueChanged(); }

    Comparator comparator() const { return Comparator(m_filter.comparator()); }
    void setComparator(Comparator comparator) {
        m_filter.setComparator(QGalleryFilter::Comparator(comparator)); emit comparatorChanged(); }

    Qt::CaseSensitivity caseSensitivity() const { return m_filter.caseSensitivity(); }
    void setCaseSensitivity(Qt::CaseSensitivity sensitivity) {
        m_filter.setCaseSensitivity(sensitivity); emit caseSensitivityChanged(); }

    bool isInverted() const { return m_filter.isInverted(); }
    void setInverted(bool inverted) { m_filter.setInverted(inverted); emit invertedChanged(); }

    QGalleryFilter filter() const;

Q_SIGNALS:
    void propertyNameChanged();
    void valueChanged();
    void comparatorChanged();
    void caseSensitivityChanged();
    void invertedChanged();

private:
    QGalleryMetaDataFilter m_filter;
};

class QDeclarativeGalleryFilterUnion : public QDeclarativeGalleryFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGalleryFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")
public:
    explicit QDeclarativeGalleryFilterUnion(QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
    {
    }

    QDeclarativeListProperty<QDeclarativeGalleryFilterBase> filters() {
        return QDeclarativeListProperty<QDeclarativeGalleryFilterBase>(this, m_filters); }

    QGalleryFilter filter() const;

private:
    QList<QDeclarativeGalleryFilterBase *> m_filters;
};

class QDeclarativeGalleryFilterIntersection : public QDeclarativeGalleryFilterBase
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGalleryFilterBase> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")
public:
    explicit QDeclarativeGalleryFilterIntersection(QObject *parent = 0)
        : QDeclarativeGalleryFilterBase(parent)
    {
    }

    QDeclarativeListProperty<QDeclarativeGalleryFilterBase> filters() {
        return QDeclarativeListProperty<QDeclarativeGalleryFilterBase>(this, m_filters); }

    QGalleryFilter filter() const;

private:
    QList<QDeclarativeGalleryFilterBase *> m_filters;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterBase))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilter))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterUnion))
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryFilterIntersection))

#endif
