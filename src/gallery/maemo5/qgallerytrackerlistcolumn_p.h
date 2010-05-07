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

#ifndef QGALLERYTRACKERLISTCOLUMN_P_H
#define QGALLERYTRACKERLISTCOLUMN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qgalleryabstractresponse.h>

QTM_BEGIN_NAMESPACE

class QGalleryTrackerListColumn
{
public:
    QGalleryTrackerListColumn(const QString &name, QGalleryProperty::Attributes attributes)
        : m_name(name), m_attributes(attributes) {}
    virtual ~QGalleryTrackerListColumn() {}

    QString name() const { return m_name; }
    QGalleryProperty::Attributes attributes() const { return m_attributes; }

private:
    const QString m_name;
    const QGalleryProperty::Attributes m_attributes;
};

class QGalleryTrackerValueColumn : public QGalleryTrackerListColumn
{
public:
    QGalleryTrackerValueColumn(
            const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerListColumn(name, attributes) {}


    virtual QVariant toVariant(const QString &string) const = 0;
    virtual QString toString(const QVariant &variant) const { return variant.toString(); }

    virtual int compare(const QVariant &value1, const QVariant &value2) const = 0;
};

class QGalleryTrackerCompositeColumn : public QGalleryTrackerListColumn
{
public:
    QGalleryTrackerCompositeColumn(const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerListColumn(name, attributes) {}

    virtual QVariant value(QVector<QVariant>::const_iterator row) const = 0;
};

class QGalleryTrackerAliasColumn : public QGalleryTrackerListColumn
{
public:
    QGalleryTrackerAliasColumn(
            const QString &name, QGalleryProperty::Attributes attributes, int aliasedKey)
        : QGalleryTrackerListColumn(name, attributes), m_aliasedKey(aliasedKey) {}

    int aliasedKey() const { return m_aliasedKey; }

private:
    const int m_aliasedKey;
};

class QGalleryTrackerImageColumn : public QGalleryTrackerListColumn
{
public:
    QGalleryTrackerImageColumn(const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerListColumn(name, attributes) {}
    virtual ~QGalleryTrackerImageColumn() {}

    virtual QString imagePath(QVector<QVariant>::const_iterator row) const = 0;
};

class QGalleryTrackerStringColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerStringColumn(
            const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerValueColumn(name, attributes) {}

    QVariant toVariant(const QString &string) const;

    int compare(const QVariant &value1, const QVariant &value2) const;
};

class QGalleryTrackerIntegerColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerIntegerColumn(
            const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerValueColumn(name, attributes) {}

    QVariant toVariant(const QString &string) const;

    int compare(const QVariant &value1, const QVariant &value2) const;
};

class QGalleryTrackerFloatColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerFloatColumn(
            const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerValueColumn(name, attributes) {}

    QVariant toVariant(const QString &string) const;

    int compare(const QVariant &value1, const QVariant &value2) const;
};

class QGalleryTrackerDoubleColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerDoubleColumn(
            const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerValueColumn(name, attributes) {}

    QVariant toVariant(const QString &string) const;

    int compare(const QVariant &value1, const QVariant &value2) const;
};

class QGalleryTrackerDateTimeColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerDateTimeColumn(
            const QString &name, QGalleryProperty::Attributes attributes)
        : QGalleryTrackerValueColumn(name, attributes) {}

    QVariant toVariant(const QString &string) const;
    QString toString(const QVariant &variant) const;

    int compare(const QVariant &value1, const QVariant &value2) const;
};

class QGalleryTrackerStaticColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerStaticColumn(
            const QString &name, QGalleryProperty::Attributes attributes, const QVariant &value)
        : QGalleryTrackerCompositeColumn(name, attributes), m_value(value) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const QVariant m_value;
};

class QGalleryTrackerPrefixColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerPrefixColumn(
            const QString &name,
            QGalleryProperty::Attributes attributes,
            int column,
            const QString &prefix)
        : QGalleryTrackerCompositeColumn(name, attributes), m_column(column), m_prefix(prefix) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_column;
    const QString m_prefix;
};

class QGalleryTrackerServicePrefixColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerServicePrefixColumn(
            const QString &name,
            QGalleryProperty::Attributes attributes,
            int uriColumn,
            int serviceColumn)
        : QGalleryTrackerCompositeColumn(name, attributes)
        , m_uriColumn(uriColumn)
        , m_serviceColumn(serviceColumn) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_uriColumn;
    const int m_serviceColumn;
};

class QGalleryTrackerServiceTypeColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerServiceTypeColumn(
            const QString &name, QGalleryProperty::Attributes attributes, int serviceColumn)
        : QGalleryTrackerCompositeColumn(name, attributes)
        , m_serviceColumn(serviceColumn) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_serviceColumn;
};

class QGalleryTrackerCompositeIdColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerCompositeIdColumn(
            const QString &name,
            QGalleryProperty::Attributes attributes,
            int column1,
            int column2,
            const QString &prefix)
        : QGalleryTrackerCompositeColumn(name, attributes)
        , m_column1(column1)
        , m_column2(column2)
        , m_prefix(prefix) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_column1;
    const int m_column2;
    const QString m_prefix;
};

class QGalleryTrackerFileUrlColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerFileUrlColumn(
            const QString &name, QGalleryProperty::Attributes attributes, int column)
        : QGalleryTrackerCompositeColumn(name, attributes), m_column(column) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_column;
};

QTM_END_NAMESPACE

#endif
