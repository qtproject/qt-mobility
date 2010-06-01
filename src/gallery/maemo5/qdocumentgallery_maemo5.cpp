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

#include "qdocumentgallery.h"

#include "qabstractgallery_p.h"

#include "qgallerycontainerrequest.h"
#include "qgallerycountrequest.h"
#include "qgalleryfilterrequest.h"
#include "qgalleryitemrequest.h"
#include "qgalleryremoverequest.h"
#include "qgalleryurlrequest.h"

#include "qgalleryerrorresponse_p.h"
#include "qgallerytrackeraggregateresponse_p.h"
#include "qgallerytrackercountresponse_p.h"
#include "qgallerytrackeritemresponse_p.h"
#include "qgallerytrackerremoveresponse_p.h"
#include "qgallerytrackerschema_p.h"
#include "qgallerytrackerurlresponse_p.h"

#include <QtCore/qmetaobject.h>
#include <QtDBus/qdbusmetatype.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QDocumentGalleryRefreshManager : public QObject
{
    Q_OBJECT
Q_SIGNALS:
    void refresh(int updateId);

public Q_SLOTS:
    void itemEdited(const QString &service);
};

void QDocumentGalleryRefreshManager::itemEdited(const QString &service)
{
    emit refresh(QGalleryTrackerSchema::serviceUpdateId(service));
}

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate, public QGalleryDBusInterfaceFactory
{
public:
    QGalleryAbstractResponse *createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse *createUrlResponse(QGalleryUrlRequest *request);
    QGalleryAbstractResponse *createContainerResponse(QGalleryContainerRequest *request);
    QGalleryAbstractResponse *createFilterResponse(QGalleryFilterRequest *request);
    QGalleryAbstractResponse *createCountResponse(QGalleryCountRequest *request);
    QGalleryAbstractResponse *createRemoveResponse(QGalleryRemoveRequest *request);

private:
    QGalleryDBusInterfacePointer daemonInterface();
    QGalleryDBusInterfacePointer metaDataInterface();
    QGalleryDBusInterfacePointer searchInterface();
    QGalleryDBusInterfacePointer fileInterface();
    QGalleryDBusInterfacePointer thumbnailInterface();

    void _q_statisticsReady(const QVector<QStringList> &statistics);
    void _q_statisticsChanged(const QVector<QStringList> &statistics);

    QGalleryDBusInterfacePointer daemonService;
    QGalleryDBusInterfacePointer metaDataService;
    QGalleryDBusInterfacePointer searchService;
    QGalleryDBusInterfacePointer fileService;
    QGalleryDBusInterfacePointer thumbnailService;
    QDocumentGalleryRefreshManager refreshManager;
};

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::daemonInterface()
{
    if (!daemonService) {
        daemonService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker"),
                QLatin1String("org.freedesktop.Tracker"));
    }
    return daemonService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::metaDataInterface()
{
    if (!metaDataService) {
        metaDataService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker/Metadata"),
                QLatin1String("org.freedesktop.Tracker.Metadata"));
    }
    return metaDataService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::searchInterface()
{
    if (!searchService) {
        searchService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker/Search"),
                QLatin1String("org.freedesktop.Tracker.Search"));
    }
    return searchService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::fileInterface()
{
    if (!fileService) {
        fileService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker/Files"),
                QLatin1String("org.freedesktop.Tracker.Files"));
    }
    return fileService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::thumbnailInterface()
{
    if (!thumbnailService) {
        thumbnailService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.thumbnailer"),
                QLatin1String("/org/freedesktop/thumbnailer/Generic"),
                QLatin1String("org.freedesktop.thumbnailer.Generic"));
    }
    return thumbnailService;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createItemResponse(QGalleryItemRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.resolveTypeFromItemId(request->itemId().toString());

    QString query = schema.buildIdQuery(&result, request->itemId().toString());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else {
        QGalleryAbstractResponse *response = 0;
        if (schema.isItemType()) {
            schema.setPropertyNames(request->propertyNames());
            schema.resolveColumns();

            if (request->isLive()) {
                QObject::connect(
                        daemonInterface().data(), SIGNAL(IndexFinished(double)),
                        response, SLOT(refresh()));
                QObject::connect(
                        &refreshManager, SIGNAL(refresh(int)), response, SLOT(refresh(int)));
                QObject::connect(
                        response, SIGNAL(itemEdited(QString)),
                        &refreshManager, SLOT(itemEdited(QString)));
            }

            response = new QGalleryTrackerItemResponse(this, schema, query, 0, 1);

            return response;
        } else if (schema.isAggregateType()) {
            schema.setPropertyNames(request->propertyNames());
            schema.resolveColumns();

            response = new QGalleryTrackerAggregateResponse(this, schema, query, 0, 1);

            if (request->isLive()) {
                QObject::connect(
                        daemonInterface().data(), SIGNAL(IndexFinished(double)),
                        response, SLOT(refresh()));
                QObject::connect(
                        &refreshManager, SIGNAL(refresh(int)), response, SLOT(refresh(int)));
                QObject::connect(
                        response, SIGNAL(itemEdited(QString)),
                        &refreshManager, SLOT(itemEdited(QString)));
            }

            return response;
        } else {
            result = QGalleryAbstractRequest::InvalidItemError;
        }
    }

    return new QGalleryErrorResponse(result);
}


QGalleryAbstractResponse *QDocumentGalleryPrivate::createUrlResponse(
        QGalleryUrlRequest *request)
{
    QGalleryTrackerUrlResponse *response = new QGalleryTrackerUrlResponse(
            fileInterface(), request->itemUrl(), request->create());

    QObject::connect(
            daemonInterface().data(), SIGNAL(IndexFinished(double)),
            response, SLOT(indexingFinished()));

    return response;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createContainerResponse(
        QGalleryContainerRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.setItemType(request->itemType());

    QString query = schema.buildContainerQuery(&result, request->containerId().toString());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else {
        QGalleryAbstractResponse *response = 0;
        if (schema.isItemType()) {
            schema.setPropertyNames(request->propertyNames());
            schema.setSortPropertyNames(request->sortPropertyNames());
            schema.resolveColumns();

            response = new QGalleryTrackerItemResponse(
                    this,
                    schema,
                    query,
                    request->initialCursorPosition(),
                    request->minimumPagedItems());

            if (request->isLive()) {
                QObject::connect(
                        daemonInterface().data(), SIGNAL(IndexFinished(double)),
                        response, SLOT(refresh()));
                QObject::connect(
                        &refreshManager, SIGNAL(refresh(int)), response, SLOT(refresh(int)));
                QObject::connect(
                        response, SIGNAL(itemEdited(QString)),
                        &refreshManager, SLOT(itemEdited(QString)));
            }

            return response;
        } else if (schema.isAggregateType()) {
            schema.setPropertyNames(request->propertyNames());
            schema.setSortPropertyNames(request->sortPropertyNames());
            schema.resolveColumns();

            response = new QGalleryTrackerAggregateResponse(
                    this,
                    schema,
                    query,
                    request->initialCursorPosition(),
                    request->minimumPagedItems());

            if (request->isLive()) {
                QObject::connect(
                        daemonInterface().data(), SIGNAL(IndexFinished(double)),
                        response, SLOT(refresh()));
                QObject::connect(
                        &refreshManager, SIGNAL(refresh(int)), response, SLOT(refresh(int)));
                QObject::connect(
                        response, SIGNAL(itemEdited(QString)),
                        &refreshManager, SLOT(itemEdited(QString)));
            }

            return response;
        } else {
            result = QGalleryAbstractRequest::ItemTypeError;
        }
    }

    return new QGalleryErrorResponse(result);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createFilterResponse(
        QGalleryFilterRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.setItemType(request->itemType());

    QString query = schema.buildFilterQuery(
            &result, request->containerId().toString(), request->filter());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else {
        if (schema.isItemType()) {
            schema.setPropertyNames(request->propertyNames());
            schema.setSortPropertyNames(request->sortPropertyNames());
            schema.resolveColumns();

            QGalleryAbstractResponse *response = new QGalleryTrackerItemResponse(
                    this,
                    schema,
                    query,
                    request->initialCursorPosition(),
                    request->minimumPagedItems());

            if (request->isLive()) {
                QObject::connect(
                        daemonInterface().data(), SIGNAL(IndexFinished(double)),
                        response, SLOT(refresh()));
                QObject::connect(
                        &refreshManager, SIGNAL(refresh(int)), response, SLOT(refresh(int)));
                QObject::connect(
                        response, SIGNAL(itemEdited(QString)),
                        &refreshManager, SLOT(itemEdited(QString)));
            }

            return response;
        } else if (schema.isAggregateType()) {
            schema.setPropertyNames(request->propertyNames());
            schema.setSortPropertyNames(request->sortPropertyNames());
            schema.resolveColumns();

            QGalleryAbstractResponse *response = new QGalleryTrackerAggregateResponse(
                    this,
                    schema,
                    query,
                    request->initialCursorPosition(),
                    request->minimumPagedItems());
            response->setCursorPosition(request->initialCursorPosition());

            if (request->isLive()) {
                QObject::connect(
                        daemonInterface().data(), SIGNAL(IndexFinished(double)),
                        response, SLOT(refresh()));
                QObject::connect(
                        &refreshManager, SIGNAL(refresh(int)), response, SLOT(refresh(int)));
                QObject::connect(
                        response, SIGNAL(itemEdited(QString)),
                        &refreshManager, SLOT(itemEdited(QString)));
            }

            return response;
        } else {
            result = QGalleryAbstractRequest::ItemTypeError;
        }
    }

    return new QGalleryErrorResponse(result);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createCountResponse(
        QGalleryCountRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.setItemType(request->itemType());

    QString query = schema.buildFilterQuery(
            &result, request->containerId().toString(), request->filter());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else if (schema.isItemType() || schema.isAggregateType()) {
        return new QGalleryTrackerCountResponse(metaDataInterface(), schema, query);
    }

    return new QGalleryErrorResponse(result);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createRemoveResponse(
        QGalleryRemoveRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QString fileName = QGalleryTrackerSchema::uriFromItemId(&result, request->itemId());

    if (fileName.isNull()) {
        if (result == QGalleryAbstractRequest::Succeeded)
            result = QGalleryAbstractRequest::InvalidItemError;

        return new QGalleryErrorResponse(result);
    } else {
        return new QGalleryTrackerRemoveResponse(fileInterface(), fileName);
    }
}

QDocumentGallery::QDocumentGallery(QObject *parent)
    : QAbstractGallery(*new QDocumentGalleryPrivate, parent)
{
    qDBusRegisterMetaType<QVector<QStringList> >();
}

QDocumentGallery::~QDocumentGallery()
{
}

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::Type type) const
{
    switch (type) {
    case QGalleryAbstractRequest::Item:
    case QGalleryAbstractRequest::Url:
    case QGalleryAbstractRequest::Container:
    case QGalleryAbstractRequest::Filter:
    case QGalleryAbstractRequest::Count:
    case QGalleryAbstractRequest::Remove:
        return true;
    default:
        return false;
    }
}

QStringList QDocumentGallery::itemTypePropertyNames(const QString &itemType) const
{
    QGalleryTrackerSchema schema;
    schema.setItemType(itemType);

    return schema.supportedPropertyNames();
}

QGalleryProperty::Attributes QDocumentGallery::propertyAttributes(
        const QString &propertyName, const QString &itemType) const
{
    QGalleryTrackerSchema schema;
    schema.setItemType(itemType);

    return schema.propertyAttributes(propertyName);
}

QGalleryAbstractResponse *QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{
    Q_D(QDocumentGallery);

    switch (request->type()) {
    case QGalleryAbstractRequest::Item:
        return d->createItemResponse(static_cast<QGalleryItemRequest *>(request));
    case QGalleryAbstractRequest::Url:
        return d->createUrlResponse(static_cast<QGalleryUrlRequest *>(request));
    case QGalleryAbstractRequest::Container:
        return d->createContainerResponse(static_cast<QGalleryContainerRequest *>(request));
    case QGalleryAbstractRequest::Filter:
        return d->createFilterResponse(static_cast<QGalleryFilterRequest *>(request));
    case QGalleryAbstractRequest::Count:
        return d->createCountResponse(static_cast<QGalleryCountRequest *>(request));
    case QGalleryAbstractRequest::Remove:
        return d->createRemoveResponse(static_cast<QGalleryRemoveRequest *>(request));
    default:
        return 0;
    }
}

#include "moc_qdocumentgallery.cpp"
#include "qdocumentgallery_maemo5.moc"

QTM_END_NAMESPACE

