/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
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

#include "qgallerytrackermetadataedit_p.h"

#include <QtDBus/qdbuspendingcall.h>

QTM_BEGIN_NAMESPACE

QGalleryTrackerMetaDataEdit::QGalleryTrackerMetaDataEdit(
        const QGalleryDBusInterfacePointer &metaDataInterface,
        const QString &uri,
        const QString &service,
        QObject *parent)
    : QObject(parent)
    , m_index(-1)
    , m_metaDataInterface(metaDataInterface)
    , m_uri(uri)
    , m_service(service)
{
}

QGalleryTrackerMetaDataEdit::~QGalleryTrackerMetaDataEdit()
{
}

static QString _qt_createSubUpdateStatement(
        const QString &command,
        const QString &subject,
        const QString &predicate,
        const QString &object)
{
    return command
            + QLatin1String("{<")
            + subject + QLatin1String("> ")
            + predicate
            + QLatin1String(" \'")
            + object
            + QLatin1String("\'}");
}

static QString _qt_createUpdateStatement(
        const QString &subject,
        const QMap<QString, QString> &m_values,
        const QMap<QString, QString> &m_oldValues)
{
    QString statement;
    for (QMap<QString,QString>::const_iterator newIterator = m_values.constBegin(), oldIterator = m_oldValues.constBegin();
            newIterator != m_values.constEnd();
            ++newIterator, ++oldIterator
          ){
        // Delete old value (if it exists ) and insert new value ( if it exists )
        if (!oldIterator.value().isEmpty()) {
            statement += _qt_createSubUpdateStatement(
                    QLatin1String("DELETE"), subject, newIterator.key(), oldIterator.value());
        }
        if (!newIterator.value().isEmpty()) {
            statement += _qt_createSubUpdateStatement(
                    QLatin1String("INSERT"), subject, newIterator.key(), newIterator.value());
        }
    }
    return statement;
}

void QGalleryTrackerMetaDataEdit::commit()
{
    if (m_values.isEmpty()) {
        emit finished(this);
    } else {
        m_metaDataInterface->call(
                QLatin1String("SparqlUpdate"),
                _qt_createUpdateStatement(m_service, m_values, m_oldValues));
        emit finished(this);
    }
}

void QGalleryTrackerMetaDataEdit::itemsInserted(int index, int count)
{
    if (index < m_index)
        m_index += count;
}

void QGalleryTrackerMetaDataEdit::itemsRemoved(int index, int count)
{
    if (index + count < m_index)
        m_index -= count;
    else if (index < m_index)
        m_index = -1;
}

#include "moc_qgallerytrackermetadataedit_p.cpp"

QTM_END_NAMESPACE
