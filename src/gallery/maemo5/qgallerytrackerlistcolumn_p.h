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

#include <QtCore/qshareddata.h>

QTM_BEGIN_NAMESPACE

class QGalleryTrackerValueColumn
{
public:
    virtual ~QGalleryTrackerValueColumn() {}

    virtual QVariant toVariant(const QString &string) const = 0;
    virtual QString toString(const QVariant &variant) const { return variant.toString(); }
};

class QGalleryTrackerCompositeColumn
{
public:
    virtual ~QGalleryTrackerCompositeColumn() {}

    virtual QVariant value(QVector<QVariant>::const_iterator row) const = 0;
};

class QGalleryTrackerAliasColumn
{
public:
    QGalleryTrackerAliasColumn(int aliasedKey) : m_aliasedKey(aliasedKey) {}
    virtual ~QGalleryTrackerAliasColumn() {}

    int aliasedKey() const { return m_aliasedKey; }

private:
    const int m_aliasedKey;
};

class QGalleryTrackerImageData : public QSharedData
{
public:
    QGalleryTrackerImageData(uint id) : id(id) {}

    uint id;
    QVariant image;
};

class QGalleryTrackerImage
{
public:
    QGalleryTrackerImage() {}
    QGalleryTrackerImage(QGalleryTrackerImageData *data) : d(data) {}
    QGalleryTrackerImage(const QGalleryTrackerImage &image) : d(image.d) {}
    ~QGalleryTrackerImage() {}

    QGalleryTrackerImage &operator =(const QGalleryTrackerImage &image);

    uint id() const { return d->id; }
    QVariant image() const { return d->image; }

    QGalleryTrackerImageData *data() { return d.data(); }

private:
    QExplicitlySharedDataPointer<QGalleryTrackerImageData> d;
};

class QGalleryTrackerImageColumn : public QObject
{
    Q_OBJECT
public:
    QGalleryTrackerImageColumn(QObject *parent = 0) : QObject(parent) {}

    virtual QVector<QGalleryTrackerImage> loadImages(
            QVector<QVariant>::const_iterator begin,
            QVector<QVariant>::const_iterator end,
            int tableWidth,
            uint abortId = 0) = 0;

    virtual void abort(uint id) = 0;

Q_SIGNALS:
    void imagesLoaded(const QList<uint> &imageIds);
};

class QGalleryTrackerStringColumn : public QGalleryTrackerValueColumn
{
public:
    virtual ~QGalleryTrackerStringColumn() {}

    QVariant toVariant(const QString &string) const;
};

class QGalleryTrackerIntegerColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
};

class QGalleryTrackerDoubleColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
};

class QGalleryTrackerDateTimeColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
    QString toString(const QVariant &variant) const;
};

class QGalleryTrackerStaticColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerStaticColumn(const QVariant &value) : m_value(value) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const QVariant m_value;
};

class QGalleryTrackerPrefixColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerPrefixColumn(int column, const QString &prefix)
        : m_column(column), m_prefix(prefix) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_column;
    const QString m_prefix;
};

class QGalleryTrackerServicePrefixColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerServicePrefixColumn(int uriColumn, int serviceColumn)
        : m_uriColumn(uriColumn) , m_serviceColumn(serviceColumn) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_uriColumn;
    const int m_serviceColumn;
};

class QGalleryTrackerServiceTypeColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerServiceTypeColumn(int serviceColumn) : m_serviceColumn(serviceColumn) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_serviceColumn;
};

class QGalleryTrackerCompositeIdColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerCompositeIdColumn(const QVector<int> columns, const QString &prefix)
        : m_columns(columns), m_prefix(prefix) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const QVector<int> m_columns;
    const QString m_prefix;
};

class QGalleryTrackerFileUrlColumn : public QGalleryTrackerCompositeColumn
{
public:
    QGalleryTrackerFileUrlColumn(int column) : m_column(column) {}

    QVariant value(QVector<QVariant>::const_iterator row) const;

private:
    const int m_column;
};

QTM_END_NAMESPACE

#endif
