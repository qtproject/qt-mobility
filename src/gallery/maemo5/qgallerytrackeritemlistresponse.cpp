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

#include "qgallerytrackeritemlistresponse_p.h"

#include "qgallerythumbnailloader_p.h"
#include "qgallerytrackermetadataedit_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>

QTM_BEGIN_NAMESPACE

QGalleryTrackerItemListResponse::QGalleryTrackerItemListResponse(
        const QGalleryDBusInterfacePointer &searchInterface,
        const QGalleryDBusInterfacePointer &metaDataInterface,
        const QGalleryTrackerSchema &schema,
        const QString &query,
        const QStringList &properties,
        const QStringList &sortProperties,
        int minimumPagedItems,
        QObject *parent)
    : QGalleryTrackerListResponse(schema, minimumPagedItems, parent)
    , m_searchInterface(searchInterface)
    , m_metaDataInterface(metaDataInterface)
    , m_service(schema.service())
    , m_query(query)
{
    for (QStringList::const_iterator property = properties.begin(), end = properties.end();
            property != end;
            ++property) {
        QString field = schema.field(*property);

        if (!field.isEmpty() && !m_fields.contains(field)) {
            m_fields.append(field);
            m_propertyNames.append(*property);
            m_propertyAttributes.append(schema.propertyAttributes(*property)
                    & (QGalleryProperty::CanRead | QGalleryProperty::CanWrite));
        }
    }

    m_sortDescending = !sortProperties.isEmpty()
            && sortProperties.first().startsWith(QLatin1Char('-'));

    for (QStringList::const_iterator property = sortProperties.begin(), end = sortProperties.end();
            property != end;
            ++property) {
        QString field = property->startsWith(QLatin1Char('+'))
                     || property->startsWith(QLatin1Char('-'))
                ? schema.field(property->mid(1))
                : schema.field(*property);

        if (!field.isEmpty())
            m_sortFields.append(field);
    }

    if (properties.contains(QLatin1String("thumbnailImage"))) {
        QGalleryThumbnailLoader *imageLoader = new QGalleryThumbnailLoader(this);

        setImageColumn(imageLoader, m_propertyNames.count() + 2);
        m_propertyNames.append(QLatin1String("thumbnailImage"));
        m_propertyAttributes.append(QGalleryProperty::CanRead);
    }
}

QGalleryTrackerItemListResponse::~QGalleryTrackerItemListResponse()
{
    for (QList<QGalleryTrackerMetaDataEdit *>::iterator it = m_edits.begin();
            it != m_edits.end();
            ++it) {
        (*it)->commit();
    }
}

QStringList QGalleryTrackerItemListResponse::propertyNames() const
{
    return m_propertyNames;
}

int QGalleryTrackerItemListResponse::propertyKey(const QString &name) const
{
    int key = m_propertyNames.indexOf(name);

    return key >= 0 ? key + 2 : -1;
}

QGalleryProperty::Attributes QGalleryTrackerItemListResponse::propertyAttributes(int key) const
{
    return m_propertyAttributes.value(key - 2);
}

QUrl QGalleryTrackerItemListResponse::url(int index) const
{
    QUrl url;

    QStringList row = QGalleryTrackerListResponse::row(index);

    if (!row.isEmpty()) {
        url.setScheme(QLatin1String("file"));
        url.setPath(row.at(0));
    }

    return url;
}

QString QGalleryTrackerItemListResponse::type(int index) const
{
    return QGalleryTrackerSchema::typeFromService(row(index).value(1));
}

QList<QGalleryResource> QGalleryTrackerItemListResponse::resources(int index) const
{
    QStringList row = QGalleryTrackerListResponse::row(index);

    if (!row.isEmpty()) {
        QMap<int, QVariant> properties;

        for (QList<int>::const_iterator it = m_resourceKeys.begin(), end = m_resourceKeys.end();
                it != end;
                ++it) {
            QString value = row.value(*it);

            if (!value.isEmpty()) {
                properties.insert(*it, value);
            }
        }

        return QList<QGalleryResource>() << QGalleryResource(QUrl(row.at(0)), properties);
    } else {
        return QList<QGalleryResource>();
    }
}

void QGalleryTrackerItemListResponse::setMetaData(int index, int key, const QVariant &value)
{
    key -= 2;

    if (key > 0 && key < m_fields.count()) {
        QGalleryTrackerMetaDataEdit *edit = 0;

        for (QList<QGalleryTrackerMetaDataEdit *>::iterator it = m_edits.begin();
                it != m_edits.end();
                ++it) {
            if ((*it)->index() == index) {
                edit = *it;
                break;
            }
        }

        if (!edit) {
            QStringList row = QGalleryTrackerListResponse::row(index);

            if (row.isEmpty())
                return;

            edit = new QGalleryTrackerMetaDataEdit(
                    m_metaDataInterface, row.value(0), row.value(1), this);
            edit->setIndex(index);

            connect(edit, SIGNAL(finished(QGalleryTrackerMetaDataEdit*)),
                    this, SLOT(editFinished(QGalleryTrackerMetaDataEdit*)));

            if (m_edits.isEmpty())
                QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));

            m_edits.append(edit);
        }

        edit->setValue(m_fields.at(key), value.toString());
    }
}

bool QGalleryTrackerItemListResponse::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) {
        for (QList<QGalleryTrackerMetaDataEdit *>::iterator it = m_edits.begin();
                it != m_edits.end();
                ++it) {
            (*it)->commit();
        }
        m_edits.clear();

        return true;
    } else {
        return QGalleryTrackerListResponse::event(event);
    }
}

void QGalleryTrackerItemListResponse::editFinished(QGalleryTrackerMetaDataEdit *edit)
{
    edit->deleteLater();

    QStringList updatedRow = row(edit->index());

    if (!updatedRow.isEmpty()) {
        QList<int> keys;

        QMap<QString, QString> values = edit->values();

        for (QMap<QString, QString>::const_iterator it = values.constBegin();
                it != values.constEnd();
                ++it) {
            int key = m_fields.indexOf(it.key());

            updatedRow[key + 2] = it.value();

            do {
                keys.append(key);
            } while ((key = m_fields.indexOf(it.key(), key + 1)) != -1);
        }

        if (!keys.isEmpty()) {
            replaceRow(edit->index(), updatedRow);

            emit metaDataChanged(edit->index(), 1, keys);
        }
    }
}

QDBusPendingCall QGalleryTrackerItemListResponse::queryRows(int offset, int limit)
{
        QList<QVariant> arguments = QList<QVariant>()
            << 0 //live query id.
            << m_service
            << m_fields
            << QString() // Search text
            << QStringList()
            << m_query
            << false // Search by service
            << m_sortFields
            << m_sortDescending
            << offset
            << limit;

    return m_searchInterface->asyncCallWithArgumentList(QLatin1String("Query"), arguments);
}

#include "moc_qgallerytrackeritemlistresponse_p.cpp"

QTM_END_NAMESPACE
