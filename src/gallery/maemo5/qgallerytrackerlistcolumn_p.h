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

class Q_AUTOTEST_EXPORT QGalleryTrackerValueColumn
{
public:
    virtual ~QGalleryTrackerValueColumn() {}

    virtual QVariant toVariant(const QString &string) const = 0;
    virtual QString toString(const QVariant &variant) const { return variant.toString(); }
};

class Q_AUTOTEST_EXPORT QGalleryTrackerCompositeColumn
{
public:
    virtual ~QGalleryTrackerCompositeColumn() {}

    virtual QVariant value(QVector<QVariant>::const_iterator row) const = 0;
};

class Q_AUTOTEST_EXPORT QGalleryTrackerImageColumn : public QObject
{
    Q_OBJECT
public:
    QGalleryTrackerImageColumn(QObject *parent = 0) : QObject(parent), m_offset(0) {}

    void moveOffset(int offset) { m_offset = offset; }

    QVariant image(int index) const { return m_images.at(index); }

    virtual void insertImages(
            int index, int count, QVector<QVariant>::const_iterator begin, int tableWidth) = 0;

    virtual void removeImages(int index, int count) = 0;

Q_SIGNALS:
    void imagesChanged(int index, int count, const QList<int> &keys);

protected:
    int m_offset;
    QVector<QVariant> m_images;
};

class QGalleryTrackerStringColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const;
};

class QGalleryTrackerStringListColumn : public QGalleryTrackerValueColumn
{
public:
    QGalleryTrackerStringListColumn()
        : m_separatorChar(QLatin1Char('|')), m_separatorString(QLatin1String("|")) {}
    QVariant toVariant(const QString &string) const;
    QString toString(const QVariant &variant) const;

private:
    const QChar m_separatorChar;
    const QString m_separatorString;
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
