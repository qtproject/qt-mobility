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
    return command + "{<" + subject + "> " + predicate + " \'"  + object + "\'}";
}

static QString _qt_createUpdateStatement(
        const QString &subject,
        const QMap<QString, QString> &m_values,
        const QMap<QString, QString> &m_oldValues)
{
    QString statement;
    for ( QMap<QString,QString>::const_iterator newIterator = m_values.constBegin(), oldIterator = m_oldValues.constBegin();
            newIterator != m_values.constEnd();
            ++newIterator, ++oldIterator
          ){
        // Delete old value (if it exists ) and insert new value ( if it exists )
        if ( !oldIterator.value().isEmpty() )
            statement += _qt_createSubUpdateStatement("DELETE", subject, newIterator.key(), oldIterator.value() );
        if ( !newIterator.value().isEmpty() )
            statement += _qt_createSubUpdateStatement("INSERT", subject, newIterator.key(), newIterator.value() );
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
                _qt_createUpdateStatement( m_service, m_values, m_oldValues )
                );
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
