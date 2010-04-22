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

#include "qdocumentgallery.h"

#include "qabstractgallery_p.h"

#include "qgallerycontainerrequest.h"
#include "qgallerycopyrequest.h"
#include "qgallerycountrequest.h"
#include "qgalleryfilterrequest.h"
#include "qgalleryitemrequest.h"
#include "qgallerymoverequest.h"
#include "qgalleryremoverequest.h"
#include "qgalleryurlrequest.h"

#include "qgalleryerrorresponse_p.h"
#include "qgallerytrackeraggregatelistresponse_p.h"
#include "qgallerytrackercountresponse_p.h"
#include "qgallerytrackerfilecopyresponse_p.h"
#include "qgallerytrackerfilelistresponse_p.h"
#include "qgallerytrackerfilemoveresponse_p.h"
#include "qgallerytrackerfileremoveresponse_p.h"
#include "qgallerytrackerschema_p.h"
#include "qgallerytrackerurlresponse_p.h"

#include <QtDBus/qdbusmetatype.h>

QTM_BEGIN_NAMESPACE

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate
{
public:
    QGalleryAbstractResponse *createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse *createUrlResponse(QGalleryUrlRequest *request);
    QGalleryAbstractResponse *createContainerResponse(QGalleryContainerRequest *request);
    QGalleryAbstractResponse *createFilterResponse(QGalleryFilterRequest *request);
    QGalleryAbstractResponse *createCountResponse(QGalleryCountRequest *request);
    QGalleryAbstractResponse *createCopyResponse(QGalleryCopyRequest *request);
    QGalleryAbstractResponse *createMoveResponse(QGalleryMoveRequest *request);
    QGalleryAbstractResponse *createRemoveResponse(QGalleryRemoveRequest *request);

private:
    QGalleryDBusInterfacePointer metaDataInterface();
    QGalleryDBusInterfacePointer searchInterface();

    QGalleryDBusInterfacePointer metaDataService;
    QGalleryDBusInterfacePointer searchService;
};

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

QGalleryAbstractResponse *QDocumentGalleryPrivate::createItemResponse(QGalleryItemRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.resolveItemType(request->itemId());

    QString query = schema.buildIdQuery(&result, request->itemId());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else {
        QGalleryAbstractResponse *response = 0;
        if (schema.isFileType()) {
            response = new QGalleryTrackerFileListResponse(
                    searchInterface(),
                    schema,
                    query,
                    request->propertyNames(),
                    QStringList(),
                    1);
            response->setCursorPosition(0);
        } else if (schema.isAggregateType()) {
            response = new QGalleryTrackerAggregateListResponse(
                    metaDataInterface(),
                    schema,
                    query,
                    request->propertyNames(),
                    QStringList(),
                    1);
            response->setCursorPosition(0);
        } else {
            result = QGalleryAbstractRequest::InvalidItemError;
        }
    }

    return new QGalleryErrorResponse(result);
}


QGalleryAbstractResponse *QDocumentGalleryPrivate::createUrlResponse(
        QGalleryUrlRequest *request)
{
    return new QGalleryTrackerUrlResponse(searchInterface(), metaDataInterface(), request);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createContainerResponse(
        QGalleryContainerRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.setItemType(request->itemType());

    QString query = schema.buildContainerQuery(&result, request->containerId());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else {
        QGalleryAbstractResponse *response = 0;
        if (schema.isFileType()) {
            response = new QGalleryTrackerFileListResponse(
                    searchInterface(),
                    schema,
                    query,
                    request->propertyNames(),
                    request->sortPropertyNames(),
                    request->minimumPagedItems());
            response->setCursorPosition(request->initialCursorPosition());

            return response;
        } else if (schema.isAggregateType()) {
            response = new QGalleryTrackerAggregateListResponse(
                    metaDataInterface(),
                    schema,
                    query,
                    request->propertyNames(),
                    request->sortPropertyNames(),
                    request->minimumPagedItems());
            response->setCursorPosition(request->initialCursorPosition());

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

    QString query = schema.buildFilterQuery(&result, request->containerId(), request->filter());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else {
        if (schema.isFileType()) {
            QGalleryAbstractResponse *response = new QGalleryTrackerFileListResponse(
                    searchInterface(),
                    schema,
                    query,
                    request->propertyNames(),
                    request->sortPropertyNames(),
                    request->minimumPagedItems());
            response->setCursorPosition(request->initialCursorPosition());

            return response;
        } else if (schema.isAggregateType()) {
            QGalleryAbstractResponse *response = new QGalleryTrackerAggregateListResponse(
                    metaDataInterface(),
                    schema,
                    query,
                    request->propertyNames(),
                    request->sortPropertyNames(),
                    request->minimumPagedItems());
            response->setCursorPosition(request->initialCursorPosition());

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

    QString query = schema.buildFilterQuery(&result, request->containerId(), request->filter());

    if (result != QGalleryAbstractRequest::Succeeded) {
        qWarning("Invalid Query %d, %s", result, qPrintable(query));
    } else if (schema.isFileType() || schema.isAggregateType()) {
        return new QGalleryTrackerCountResponse(metaDataInterface(), schema, query);
    }

    return new QGalleryErrorResponse(result);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createCopyResponse(QGalleryCopyRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.setItemType(QDocumentGallery::File);

    QStringList fileNames = schema.urisFromItemIds(&result, request->itemIds());

    if (fileNames.isEmpty()) {
        if (result == QGalleryAbstractRequest::Succeeded)
            result = QGalleryAbstractRequest::InvalidItemError;
    } else {
        QString destinationPath = schema.uriFromItemId(&result, request->destinationId());

        if (destinationPath.isEmpty()) {
            result = QGalleryAbstractRequest::InvalidDestinationError;
        } else {
            return new QGalleryTrackerFileCopyResponse(
                    metaDataInterface(),
                    schema,
                    request->propertyNames(),
                    fileNames,
                    destinationPath);
        }
    }

    return new QGalleryErrorResponse(result);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createMoveResponse(QGalleryMoveRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.setItemType(QDocumentGallery::File);

    QStringList fileNames = schema.urisFromItemIds(&result, request->itemIds());

    if (fileNames.isEmpty()) {
        if (result == QGalleryAbstractRequest::Succeeded)
            result = QGalleryAbstractRequest::InvalidItemError;
    } else {
        QString destinationPath = schema.uriFromItemId(&result, request->destinationId());

        if (destinationPath.isEmpty()) {
            result = QGalleryAbstractRequest::InvalidDestinationError;
        } else {
            return new QGalleryTrackerFileMoveResponse(
                    metaDataInterface(),
                    schema,
                    request->propertyNames(),
                    fileNames,
                    destinationPath);
        }
    }

    return new QGalleryErrorResponse(result);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createRemoveResponse(
        QGalleryRemoveRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QGalleryTrackerSchema schema;
    schema.setItemType(QDocumentGallery::File);

    QStringList fileNames = schema.urisFromItemIds(&result, request->itemIds());

    if (fileNames.isEmpty()) {
        if (result == QGalleryAbstractRequest::Succeeded)
            result = QGalleryAbstractRequest::InvalidItemError;
    } else {
        return new QGalleryTrackerFileRemoveResponse(
                metaDataInterface(),
                schema,
                request->propertyNames(),
                fileNames);
    }

    return new QGalleryErrorResponse(result);
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
    case QGalleryAbstractRequest::Copy:
    case QGalleryAbstractRequest::Move:
    case QGalleryAbstractRequest::Remove:
        return true;
    default:
        return false;
    }
}

QStringList QDocumentGallery::itemTypes() const
{
    return QStringList();
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
    case QGalleryAbstractRequest::Copy:
        return d->createCopyResponse(static_cast<QGalleryCopyRequest *>(request));
    case QGalleryAbstractRequest::Move:
        return d->createMoveResponse(static_cast<QGalleryMoveRequest *>(request));
    case QGalleryAbstractRequest::Remove:
        return d->createRemoveResponse(static_cast<QGalleryRemoveRequest *>(request));
    default:
        return 0;
    }
}

#include "moc_qdocumentgallery.cpp"

QTM_END_NAMESPACE

