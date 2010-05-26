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
        const QGalleryDBusInterfacePointer &fileInterface,
        const QUrl &itemUrl,
        bool create,
        QObject *parent)
    : QGalleryAbstractResponse(parent)
    , m_watcher(0)
    , m_fileInterface(fileInterface)
    , m_itemUrl(itemUrl)
{
    Q_UNUSED(create);

    m_watcher = new QDBusPendingCallWatcher(
            m_fileInterface->asyncCall(QLatin1String("GetServiceType"), m_itemUrl.path()), this);

    if (m_watcher->isFinished()) {
        callFinished(m_watcher);
    } else {
        connect(m_watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(callFinished(QDBusPendingCallWatcher*)));
    }
}

QGalleryTrackerUrlResponse::~QGalleryTrackerUrlResponse()
{
}

int QGalleryTrackerUrlResponse::minimumPagedItems() const
{
    return 1;
}

QStringList QGalleryTrackerUrlResponse::propertyNames() const
{
    return QStringList();
}

int QGalleryTrackerUrlResponse::propertyKey(const QString &) const
{
    return -1;
}

QGalleryProperty::Attributes QGalleryTrackerUrlResponse::propertyAttributes(int) const
{
    return QGalleryProperty::CanRead;
}

int QGalleryTrackerUrlResponse::count() const
{
    return !m_itemId.isNull() ? 1 : 0;
}

QVariant QGalleryTrackerUrlResponse::id(int) const
{
    return m_itemId;
}

QUrl QGalleryTrackerUrlResponse::url(int) const
{
    return m_itemUrl;
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
    return ItemStatus();
}

QVariant QGalleryTrackerUrlResponse::metaData(int, int) const
{
    return QVariant();
}

void QGalleryTrackerUrlResponse::setMetaData(int, int, const QVariant &)
{
}

void QGalleryTrackerUrlResponse::cancel()
{
}

bool QGalleryTrackerUrlResponse::waitForFinished(int)
{
    if (m_watcher) {
        m_watcher->waitForFinished();

        callFinished(m_watcher);
    }
    return true;
}

void QGalleryTrackerUrlResponse::callFinished(QDBusPendingCallWatcher *watcher)
{
    Q_ASSERT(watcher == m_watcher);

    watcher->deleteLater();
    m_watcher = 0;

    if (watcher->isError()) {
        qWarning("DBUS error %s", qPrintable(watcher->error().message()));

        finish(QGalleryAbstractRequest::ConnectionError);
    } else {
        QGalleryTrackerSchema schema;
        schema.resolveTypeFromService(QDBusPendingReply<QString>(*watcher).value());

        if (schema.isItemType()) {
            m_itemId = schema.itemIdFromUri(m_itemUrl.path());
            m_itemType = schema.itemType();

            emit inserted(0, 1);

            finish(QGalleryAbstractRequest::Succeeded);
        } else {
            finish(QGalleryAbstractRequest::ItemTypeError);
        }
    }
}

#include "moc_qgallerytrackerurlresponse_p.cpp"

QTM_END_NAMESPACE
