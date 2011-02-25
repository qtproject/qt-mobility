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

#include "qgallerytrackerlistcolumn_p.h"

#include "qgallerytrackerschema_p.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QTM_BEGIN_NAMESPACE

QVariant QGalleryTrackerStringColumn::toVariant(const QString &string) const
{
    return QVariant(string);
}

QVariant QGalleryTrackerStringListColumn::toVariant(const QString &string) const
{
    return string.split(m_separatorChar, QString::SkipEmptyParts);
}

QString QGalleryTrackerStringListColumn::toString(const QVariant &variant) const
{
    return variant.type() == QVariant::StringList
        ? variant.toStringList().join(m_separatorString)
        : variant.toString();
}

QVariant QGalleryTrackerIntegerColumn::toVariant(const QString &string) const
{
    bool ok;

    int number = string.toInt(&ok);

    return ok ? QVariant(number) : QVariant();
}

QVariant QGalleryTrackerDoubleColumn::toVariant(const QString &string) const
{
    bool ok;

    double number = string.toDouble(&ok);

    return ok ? QVariant(number) : QVariant();
}

QVariant QGalleryTrackerDateTimeColumn::toVariant(const QString &string) const
{
    QDateTime dateTime = QDateTime::fromString(string, Qt::ISODate);

    return dateTime.isValid() ? QVariant(dateTime) : QVariant();
}

QString QGalleryTrackerDateTimeColumn::toString(const QVariant &variant) const
{
    return variant.toDateTime().toString(Qt::ISODate);
}

QVariant QGalleryTrackerStaticColumn::value(QVector<QVariant>::const_iterator) const
{
    return m_value;
}

QVariant QGalleryTrackerPrefixColumn::value(QVector<QVariant>::const_iterator row) const
{
    return m_prefix + (row + m_column)->toString();
}

QVariant QGalleryTrackerCompositeIdColumn::value(QVector<QVariant>::const_iterator row) const
{
    QString fragment = (row + m_columns.at(0))->toString();
    fragment.replace(QLatin1String("/"), QLatin1String("//"));

    return m_prefix + fragment + QLatin1Char('/') + (row + m_columns.at(1))->toString();
}

QVariant QGalleryTrackerFileUrlColumn::value(QVector<QVariant>::const_iterator row) const
{
    return QUrl::fromLocalFile((row + m_column)->toString());
}

QGalleryTrackerCompositeColumn *QGalleryTrackerFileUrlColumn::create(const QVector<int> &)
{
    return new QGalleryTrackerFileUrlColumn(0);
}

QVariant QGalleryTrackerFilePathColumn::value(QVector<QVariant>::const_iterator row) const
{
    return *row;
}

QGalleryTrackerCompositeColumn *QGalleryTrackerFilePathColumn::create(const QVector<int> &)
{
    return new QGalleryTrackerFilePathColumn;
}

#include "moc_qgallerytrackerlistcolumn_p.cpp"

QTM_END_NAMESPACE
