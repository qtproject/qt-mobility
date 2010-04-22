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


#include "qgallerytrackerurlresponse_p.h"

#include "qgallerytrackermetadataedit_p.h"
#include "qgallerytrackerschema_p.h"
#include "qgalleryurlrequest.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qdatetime.h>
#include <QtDBus/qdbuspendingreply.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

QGalleryTrackerUrlResponse::QGalleryTrackerUrlResponse(
        const QGalleryDBusInterfacePointer &searchInterface,
        const QGalleryDBusInterfacePointer &metaDataInterface,
        QGalleryUrlRequest *request,
        QObject *parent)
    : QGalleryAbstractResponse(parent)
    , m_fileQueryWatcher(0)
    , m_typeQueryWatcher(0)
    , m_edit(0)
    , m_outstandingEdits(0)
    , m_searchInterface(searchInterface)
    , m_metaDataInterface(metaDataInterface)
    , m_propertyNames(request->propertyNames())
    , m_rowIndexes(m_propertyNames.count(), -1)
    , m_pendingRowIndexes(m_propertyNames.count(), -1)
    , m_url(request->itemUrl())
{
    QGalleryTrackerSchema schema;
    schema.setItemType(QLatin1String("File"));

    m_itemId = QLatin1String("file::") + m_url.path();

    int error = QGalleryAbstractRequest::NoResult;
    QString query = schema.buildIdQuery(&error, m_itemId);

    if (error != QGalleryAbstractRequest::NoResult) {
        finish(error);
    } else {
        m_fileQueryWatcher = execute(query, schema);

        if (m_fileQueryWatcher->isFinished()) {
            fileQueryFinished(m_fileQueryWatcher);
        } else {
            connect(m_fileQueryWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this, SLOT(fileQueryFinished(QDBusPendingCallWatcher*)));
        }
    }
}

QGalleryTrackerUrlResponse::~QGalleryTrackerUrlResponse()
{
    if (m_edit)
        m_edit->commit();
}

int QGalleryTrackerUrlResponse::minimumPagedItems() const
{
    return 1;
}

QStringList QGalleryTrackerUrlResponse::propertyNames() const
{
    return m_propertyNames;
}

int QGalleryTrackerUrlResponse::propertyKey(const QString &name) const
{
    return m_propertyNames.indexOf(name);
}

QGalleryProperty::Attributes QGalleryTrackerUrlResponse::propertyAttributes(int) const
{
    return QGalleryProperty::CanRead;
}

int QGalleryTrackerUrlResponse::count() const
{
    return !m_row.isEmpty() ? 1 : 0;
}

QString QGalleryTrackerUrlResponse::id(int) const
{
    return m_itemId;
}

QUrl QGalleryTrackerUrlResponse::url(int) const
{
    return m_url;
}

QString QGalleryTrackerUrlResponse::type(int) const
{
    return m_itemType;
}

QList<QGalleryResource> QGalleryTrackerUrlResponse::resources(int) const
{
    return QList<QGalleryResource>();
}

QGalleryItemList::ItemStatus QGalleryTrackerUrlResponse::status(int) const
{
    ItemStatus status;

    if (m_fileQueryWatcher || m_typeQueryWatcher)
        status |= Reading;

    if (m_outstandingEdits > 0)
        status |= Writing;

    return status;
}

QVariant QGalleryTrackerUrlResponse::metaData(int, int key) const
{
    int column = m_rowIndexes.value(key, -1);

    return column >= 0 ? m_row.value(column + 2) : QVariant();
}

void QGalleryTrackerUrlResponse::setMetaData(int index, int key, const QVariant &value)
{
    if (!m_row.isEmpty() && index == 0) {
        QString field = m_fields.value(key);

        if (!field.isNull()) {
            if (!m_edit) {
                m_edit = new QGalleryTrackerMetaDataEdit(
                        m_metaDataInterface, m_row.value(0), m_row.value(1), this);
                connect(m_edit, SIGNAL(finished(QGalleryTrackerMetaDataEdit*)),
                        this, SLOT(editFinished(QGalleryTrackerMetaDataEdit*)));

                QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));

                m_outstandingEdits += 1;
            }

            m_edit->setValue(field, value.toString());
        }
    }
}

void QGalleryTrackerUrlResponse::cancel()
{
    QGalleryAbstractResponse::cancel();
}

bool QGalleryTrackerUrlResponse::waitForFinished(int msecs)
{
    QTime timer;
    for (timer.start(); msecs >= 0; msecs -= timer.restart()) {
        if (m_typeQueryWatcher) {
            m_typeQueryWatcher->waitForFinished();

            typeQueryFinished(m_typeQueryWatcher);
        } else if (m_fileQueryWatcher) {
            m_fileQueryWatcher->waitForFinished();

            fileQueryFinished(m_fileQueryWatcher);
        } else {
            return true;
        }
    }
    return false;
}

bool QGalleryTrackerUrlResponse::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest) {
        if (m_edit) {
            m_edit->commit();
            m_edit = 0;
        }

        return true;
    } else {
        return QGalleryAbstractResponse::event(event);
    }
}

void QGalleryTrackerUrlResponse::editFinished(QGalleryTrackerMetaDataEdit *edit)
{
    edit->deleteLater();

    m_outstandingEdits -= 1;

    if (!m_row.isEmpty()) {
        QList<int> keys;

        QMap<QString, QString> values = edit->values();

        for (QMap<QString, QString>::const_iterator it = values.constBegin();
                it != values.constEnd();
                ++it) {
            int key = m_fields.indexOf(it.key());

            m_row[m_rowIndexes.at(key) + 2] = it.value();

            do {
                keys.append(key);
            } while ((key = m_fields.indexOf(it.key(), key + 1)) != -1);
        }

        if (keys.isEmpty())
            emit metaDataChanged(0, 1, keys);
    }
}

void QGalleryTrackerUrlResponse::fileQueryFinished(QDBusPendingCallWatcher *watcher)
{
    Q_ASSERT(m_fileQueryWatcher = watcher);

    m_fileQueryWatcher->deleteLater();
    m_fileQueryWatcher = 0;

    if (watcher->isError()) {
        qWarning("DBUS error %s", qPrintable(watcher->error().message()));

        finish(QGalleryAbstractRequest::ConnectionError);
    } else {
        QVector<QStringList> rows = QDBusPendingReply<QVector<QStringList> >(*watcher).value();

        if (rows.isEmpty()) {
            finish(QGalleryAbstractRequest::InvalidItemError);
        } else {
            m_row = rows.first();
            m_rowIndexes = m_pendingRowIndexes;

            m_itemType = QGalleryTrackerSchema::typeFromService(m_row.value(1));

            if (m_itemType.isEmpty() || m_itemType == QLatin1String("File")) {
                emit inserted(0, 1);

                finish(QGalleryAbstractRequest::Succeeded);
            } else {
                QGalleryTrackerSchema schema;
                schema.setItemType(m_itemType);

                m_itemId = schema.idFunc()(m_row);

                int error = QGalleryAbstractRequest::NoResult;
                QString query = schema.buildIdQuery(&error, m_itemId);

                emit inserted(0, 1);

                if (error != QGalleryAbstractRequest::NoResult) {
                    finish(error);
                } else {
                    m_typeQueryWatcher = execute(query, schema);

                    if (m_typeQueryWatcher->isFinished()) {
                        typeQueryFinished(m_fileQueryWatcher);
                    } else {
                        connect(m_typeQueryWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                                this, SLOT(typeQueryFinished(QDBusPendingCallWatcher*)));
                    }
                }
            }
        }
    }
}

void QGalleryTrackerUrlResponse::typeQueryFinished(QDBusPendingCallWatcher *watcher)
{
    Q_ASSERT(m_typeQueryWatcher = watcher);

    m_typeQueryWatcher->deleteLater();
    m_typeQueryWatcher = 0;

    if (watcher->isError()) {
        qWarning("DBUS error %s", qPrintable(watcher->error().message()));

        finish(QGalleryAbstractRequest::ConnectionError);
    } else {
        QVector<QStringList> rows = QDBusPendingReply<QVector<QStringList> >(*watcher).value();

        if (!rows.isEmpty()) {
            m_row = rows.first();
            m_rowIndexes = m_pendingRowIndexes;

            emit metaDataChanged(0, 1);
        }
        finish(QGalleryAbstractRequest::Succeeded);
    }
}

QDBusPendingCallWatcher *QGalleryTrackerUrlResponse::execute(
        const QString &query, const QGalleryTrackerSchema &schema)
{
    m_fields.clear();

    QStringList fields;

    for (int i = 0; i < m_propertyNames.count(); ++i) {
        QString field = schema.field(m_propertyNames.at(i));

        m_fields.append(field);

        if (!field.isNull()) {
            m_pendingRowIndexes[i] = fields.count();
            fields.append(field);
        } else {
            m_pendingRowIndexes[i] = -1;
        }
    }

    QList<QVariant> arguments = QList<QVariant>()
            << 0 //live query id.
            << schema.service()
            << fields
            << QString() // Search text
            << QStringList()
            << query
            << false // Search by service
            << QStringList()
            << false
            << 0
            << 1;

    return new QDBusPendingCallWatcher(
            m_searchInterface->asyncCallWithArgumentList(QLatin1String("Query"), arguments), this);
}

#include "moc_qgallerytrackerurlresponse_p.cpp"

QTM_END_NAMESPACE
