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

#include "qgallery.h"
#include "qabstractgallery_p.h"

#include "qgallerytrackeraggregatelistresponse_p.h"
#include "qgallerytrackercountresponse_p.h"
#include "qgallerytrackerfilecopyresponse_p.h"
#include "qgallerytrackerfilelistresponse_p.h"
#include "qgallerytrackerfilemoveresponse_p.h"
#include "qgallerytrackerfileremoveresponse_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtDBus/qdbusmetatype.h>

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate
{
public:

    QGalleryAbstractResponse *createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse *createCountResponse(QGalleryCountRequest *request);
    QGalleryAbstractResponse *createCopyResponse(QGalleryCopyRequest *request);
    QGalleryAbstractResponse *createMoveResponse(QGalleryMoveRequest *request);
    QGalleryAbstractResponse *createRemoveResponse(QGalleryRemoveRequest *request);
};

QGalleryAbstractResponse *QDocumentGalleryPrivate::createItemResponse(QGalleryItemRequest *request)
{
    int error = 0;

    QGalleryTrackerSchema schema;
    schema.setItemType(request->itemType());

    QString query = schema.buildQuery(&error, request->filter());

    if (error != 0) {
        qWarning("Invalid Query %d, %s", error, qPrintable(query));
    } else {
        QGalleryAbstractResponse *response = 0;
        if (schema.isFileType()) {
            response = new QGalleryTrackerFileListResponse(
                    QDBusConnection::sessionBus(),
                    schema,
                    query,
                    request->fields(),
                    request->sortFields(),
                    request->minimumPagedItems());
        } else if (schema.isAggregateType()) {
            response = new QGalleryTrackerAggregateListResponse(
                    QDBusConnection::sessionBus(),
                    schema,
                    query,
                    request->fields(),
                    request->sortFields(),
                    request->minimumPagedItems());
        }

        if (response) {
            response->setCursorPosition(request->initialCursorPosition());

            return response;
        }
    }

    return 0;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createCountResponse(
        QGalleryCountRequest *request)
{
    int error = 0;

    QGalleryTrackerSchema schema;
    schema.setItemType(request->itemType());

    QString query = schema.buildQuery(&error, request->filter());

    if (error != 0) {
        qWarning("Invalid Query %d, %s", error, qPrintable(query));
    } else if (schema.isFileType() || schema.isAggregateType()) {
        return new QGalleryTrackerCountResponse(QDBusConnection::sessionBus(), schema, query);
    }

    return 0;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createCopyResponse(QGalleryCopyRequest *request)
{
    int error = 0;

    QGalleryTrackerSchema schema;
    schema.setItemType(QDocumentGallery::File);

    QStringList fileNames = schema.urisFromItemIds(&error, request->itemIds());

    if (fileNames.isEmpty()) {
        if (error == 0)
            error = 1;
    } else {
        QString destinationPath = schema.uriFromItemId(&error, request->destinationId());

        if (destinationPath.isEmpty()) {
            if (error == 0)
                error = 1;
        } else {
            return new QGalleryTrackerFileCopyResponse(
                    QDBusConnection::sessionBus(),
                    schema,
                    request->fields(),
                    fileNames,
                    destinationPath);
        }
    }

    return 0;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createMoveResponse(QGalleryMoveRequest *request)
{
    int error = 0;

    QGalleryTrackerSchema schema;
    schema.setItemType(QDocumentGallery::File);

    QStringList fileNames = schema.urisFromItemIds(&error, request->itemIds());

    if (fileNames.isEmpty()) {
        if (error == 0)
            error = 1;
    } else {
        QString destinationPath = schema.uriFromItemId(&error, request->destinationId());

        if (destinationPath.isEmpty()) {
            if (error == 0)
                error = 1;
        } else {
            return new QGalleryTrackerFileMoveResponse(
                    QDBusConnection::sessionBus(),
                    schema,
                    request->fields(),
                    fileNames,
                    destinationPath);
        }
    }

    return 0;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createRemoveResponse(
        QGalleryRemoveRequest *request)
{
    int error = 0;

    QGalleryTrackerSchema schema;
    schema.setItemType(QDocumentGallery::File);

    QStringList fileNames = schema.urisFromItemIds(&error, request->itemIds());

    if (fileNames.isEmpty()) {
        if (error == 0)
            error = 1;
    } else {
        return new QGalleryTrackerFileRemoveResponse(
                QDBusConnection::sessionBus(),
                schema,
                request->fields(),
                fileNames);
    }

    return 0;
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
