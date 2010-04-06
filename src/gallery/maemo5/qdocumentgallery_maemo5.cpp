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
#include "qgallerytrackerfilelistresponse_p.h"
#include "qgallerytrackerschema_p.h"

#include <QtDBus/qdbusmetatype.h>

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate
{
public:

    QGalleryAbstractResponse *createItemResponse(QGalleryItemRequest *request);
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
    default:
        return 0;
    }
}
