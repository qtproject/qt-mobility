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

#include "qgalleryitemrequest.h"
#include "qgalleryqueryrequest.h"
#include "qgalleryremoverequest.h"
#include "qgallerytyperequest.h"

#include "qgallerytrackerchangenotifier_p.h"
#include "qgallerytrackeritemresponse_p.h"
#include "qgallerytrackerremoveresponse_p.h"
#include "qgallerytrackerschema_p.h"
#include "qgallerytrackertyperesultset_p.h"

#include <QtCore/qmetaobject.h>
#include <QtDBus/qdbusmetatype.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate, public QGalleryDBusInterfaceFactory
{
public:
    QGalleryAbstractResponse *createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse *createTypeResponse(QGalleryTypeRequest *request);
    QGalleryAbstractResponse *createFilterResponse(QGalleryQueryRequest *request);
    QGalleryAbstractResponse *createRemoveResponse(QGalleryRemoveRequest *request);

private:
    QGalleryDBusInterfacePointer daemonInterface();
    QGalleryDBusInterfacePointer metaDataInterface();
    QGalleryDBusInterfacePointer searchInterface();
    QGalleryDBusInterfacePointer fileInterface();
    QGalleryDBusInterfacePointer thumbnailInterface();
    QGalleryTrackerChangeNotifier *changeNotifier();

    QGalleryAbstractResponse *createItemListResponse(
            const QGalleryTrackerItemListArguments &arguments,
            int offset,
            int limit,
            bool isItemType,
            bool isLive);

    QGalleryDBusInterfacePointer daemonService;
    QGalleryDBusInterfacePointer metaDataService;
    QGalleryDBusInterfacePointer searchService;
    QGalleryDBusInterfacePointer fileService;
    QGalleryDBusInterfacePointer thumbnailService;
    QScopedPointer<QGalleryTrackerChangeNotifier> notifier;
};

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::daemonInterface()
{
    if (!daemonService) {
        daemonService = new QGalleryTrackerDaemonDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker"),
                "org.freedesktop.Tracker");
    }
    return daemonService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::metaDataInterface()
{
    if (!metaDataService) {
        metaDataService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker/Metadata"),
                "org.freedesktop.Tracker.Metadata");
    }
    return metaDataService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::searchInterface()
{
    if (!searchService) {
        searchService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker/Search"),
                "org.freedesktop.Tracker.Search");
    }
    return searchService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::fileInterface()
{
    if (!fileService) {
        fileService = new QGalleryDBusInterface(
                QLatin1String("org.freedesktop.Tracker"),
                QLatin1String("/org/freedesktop/Tracker/Files"),
                "org.freedesktop.Tracker.Files");
    }
    return fileService;
}

QGalleryDBusInterfacePointer QDocumentGalleryPrivate::thumbnailInterface()
{
    if (!thumbnailService) {

        thumbnailService = new QGalleryThumbnailerDBusInterface(
#ifdef Q_WS_MAEMO_5
                QLatin1String("org.freedesktop.thumbnailer"),
                QLatin1String("/org/freedesktop/thumbnailer/Generic"),
                "org.freedesktop.thumbnailer.Generic");
#else
                QLatin1String("org.freedesktop.thumbnails.Thumbnailer1"),
                QLatin1String("/org/freedesktop/thumbnails/Thumbnailer1"),
                "org.freedesktop.thumbnails.Thumbnailer1");
#endif
    }
    return thumbnailService;
}


QGalleryTrackerChangeNotifier *QDocumentGalleryPrivate::changeNotifier()
{
    if (!notifier)
        notifier.reset(new QGalleryTrackerChangeNotifier(daemonInterface()));

    return notifier.data();
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createItemResponse(QGalleryItemRequest *request)
{
    QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromItemId(request->itemId().toString());

    QGalleryTrackerItemListArguments arguments;

    int result = schema.prepareIdResponse(
            &arguments, this, request->itemId().toString(), request->propertyNames());

    if (result != QGalleryAbstractRequest::Succeeded) {
        return new QGalleryAbstractResponse(result);
    } else {
        return createItemListResponse(arguments, 0, 1, schema.isItemType(), request->isLive());
    }
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createTypeResponse(QGalleryTypeRequest *request)
{
    QGalleryTrackerSchema schema(request->itemType());

    QGalleryTrackerTypeResultSetArguments arguments;

    int result = schema.prepareTypeResponse(
            &arguments,
            this,
            request->scope(),
            request->rootItem().toString(),
            request->filter());

    if (result != QGalleryAbstractRequest::Succeeded) {
        return new QGalleryAbstractResponse(result);
    } else {
        QGalleryTrackerTypeResultSet *response = new QGalleryTrackerTypeResultSet(arguments);

        if (request->isLive()) {
            QObject::connect(
                    changeNotifier(), SIGNAL(itemsChanged(int)),
                    response, SLOT(refresh(int)));
        }

        return response;
    }
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createItemListResponse(
        const QGalleryTrackerItemListArguments &arguments,
        int offset,
        int limit,
        bool isItemType,
        bool isLive)
{
    QGalleryTrackerItemList *response = 0;

    if (isItemType) {
        response = new QGalleryTrackerItemResponse(
                arguments, metaDataInterface(), isLive, offset, limit);
    } else {
        response = new QGalleryTrackerItemList(arguments, isLive, offset, limit);
    }

    if (isLive) {
        QObject::connect(
                changeNotifier(), SIGNAL(itemsChanged(int)), response, SLOT(refresh(int)));
    }
    QObject::connect(
            response, SIGNAL(itemEdited(QString)), changeNotifier(), SLOT(itemsEdited(QString)));

    return response;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createFilterResponse(
        QGalleryQueryRequest *request)
{
    QGalleryTrackerSchema schema(request->rootType());

    QGalleryTrackerItemListArguments arguments;

    int result = schema.prepareFilterResponse(
            &arguments,
            this,
            request->scope(),
            request->rootItem().toString(),
            request->filter(),
            request->propertyNames(),
            request->sortPropertyNames());

    if (result != QGalleryAbstractRequest::Succeeded) {
        return new QGalleryAbstractResponse(result);
    } else {
        return createItemListResponse(
                arguments,
                request->offset(),
                request->limit(),
                schema.isItemType(),
                request->isLive());
    }
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createRemoveResponse(
        QGalleryRemoveRequest *request)
{
    int result = QGalleryAbstractRequest::Succeeded;

    QString fileName = QGalleryTrackerSchema::uriFromItemId(&result, request->itemId());

    if (fileName.isNull()) {
        if (result == QGalleryAbstractRequest::Succeeded)
            result = QGalleryAbstractRequest::InvalidItemError;

        return new QGalleryAbstractResponse(result);
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

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::RequestType type) const
{
    switch (type) {
    case QGalleryAbstractRequest::QueryRequest:
    case QGalleryAbstractRequest::ItemRequest:
    case QGalleryAbstractRequest::TypeRequest:
    case QGalleryAbstractRequest::RemoveRequest:
        return true;
    default:
        return false;
    }
}

QStringList QDocumentGallery::itemTypePropertyNames(const QString &itemType) const
{
    return QGalleryTrackerSchema(itemType).supportedPropertyNames();
}

QGalleryProperty::Attributes QDocumentGallery::propertyAttributes(
        const QString &propertyName, const QString &itemType) const
{
    return QGalleryTrackerSchema(itemType).propertyAttributes(propertyName);
}

QGalleryAbstractResponse *QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{
    Q_D(QDocumentGallery);

    switch (request->type()) {
    case QGalleryAbstractRequest::QueryRequest:
        return d->createFilterResponse(static_cast<QGalleryQueryRequest *>(request));
    case QGalleryAbstractRequest::ItemRequest:
        return d->createItemResponse(static_cast<QGalleryItemRequest *>(request));
    case QGalleryAbstractRequest::TypeRequest:
        return d->createTypeResponse(static_cast<QGalleryTypeRequest *>(request));
    case QGalleryAbstractRequest::RemoveRequest:
        return d->createRemoveResponse(static_cast<QGalleryRemoveRequest *>(request));
    default:
        return 0;
    }
}

#include "moc_qdocumentgallery.cpp"

QTM_END_NAMESPACE

