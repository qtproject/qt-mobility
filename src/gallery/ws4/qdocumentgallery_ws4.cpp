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

#include "qgalleryerrorresponse_p.h"
#include "qws4gallerycopyresponse_p.h"
#include "qws4gallerycountresponse_p.h"
#include "qws4galleryitemresponse_p.h"
#include "qws4gallerymoveresponse_p.h"
#include "qws4galleryquerybuilder_p.h"
#include "qws4galleryremoveresponse_p.h"

#include <QtCore/qstringbuilder.h>

#include <msdasc.h>
#include <searchapi.h>

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate
{
    Q_DECLARE_PUBLIC(QDocumentGallery)
public:
    QDocumentGalleryPrivate()
        : commandFactory(0)
    {}

    ~QDocumentGalleryPrivate()
    {
        if (commandFactory)
            commandFactory->Release();
    }

    IDBCreateCommand *commandFactory;

    void open();
    IRowsetScroll *execute(const QString &query, bool live, int *result);

    QGalleryAbstractResponse *createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse *createCountResponse(QGalleryCountRequest *request);
    QGalleryAbstractResponse *createCopyResponse(QGalleryCopyRequest *request);
    QGalleryAbstractResponse *createMoveResponse(QGalleryMoveRequest *request);
    QGalleryAbstractResponse *createRemoveResponse(QGalleryRemoveRequest *request);
};

void QDocumentGalleryPrivate::open()
{
    Q_ASSERT(commandFactory == 0);

    IDataInitialize *dataInitializer = 0;
    if (SUCCEEDED(CoCreateInstance(
            CLSID_MSDAINITIALIZE,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IDataInitialize,
            reinterpret_cast<void **>(&dataInitializer)))) {
        IDBInitialize *dataSource = 0;
        if (SUCCEEDED(dataInitializer->GetDataSource(
                0,
                CLSCTX_INPROC_SERVER,
                L"provider=Search.CollatorDSO.1",
                IID_IDBInitialize,
                reinterpret_cast<IUnknown **>(&dataSource)))) {
            if (SUCCEEDED(dataSource->Initialize())) {
                IDBCreateSession *sessionFactory = 0;
                if (SUCCEEDED(dataSource->QueryInterface(
                        IID_IDBCreateSession, reinterpret_cast<void **>(&sessionFactory)))) {
                    sessionFactory->CreateSession(
                            0, IID_IDBCreateCommand, reinterpret_cast<IUnknown **>(&commandFactory));
                    sessionFactory->Release();
                }
            }
            dataSource->Release();
        }
        dataInitializer->Release();
    }
}

IRowsetScroll *QDocumentGalleryPrivate::execute(const QString &query, bool live, int *result)
{
    *result = QGalleryItemRequest::InvalidFilter;

    IRowsetScroll *rowSet = 0;

    IUnknown *command = 0;
    if (SUCCEEDED(commandFactory->CreateCommand(0, IID_ICommand, &command))) {
        ICommandProperties *commandProperties = 0;
        if (SUCCEEDED(command->QueryInterface(
                IID_ICommandProperties, reinterpret_cast<void **>(&commandProperties)))) {
            DBPROP properties[2] = {};
            DBPROPSET propertySet = {};

            properties[0].dwPropertyID = DBPROP_ROWSET_ASYNCH;
            properties[0].dwOptions = DBPROPOPTIONS_OPTIONAL;
            properties[0].vValue.vt = VT_I4;
            properties[0].vValue.lVal = DBPROPVAL_ASYNCH_SEQUENTIALPOPULATION | DBPROPVAL_ASYNCH_RANDOMPOPULATION;

            propertySet.cProperties = 1;
            propertySet.rgProperties = properties;
            propertySet.guidPropertySet = DBPROPSET_ROWSET;

            commandProperties->SetProperties(1, &propertySet);
        }

        ICommandText *commandText = 0;
        if (SUCCEEDED(command->QueryInterface(
                IID_ICommandText, reinterpret_cast<void **>(&commandText)))) {
            if (SUCCEEDED(commandText->SetCommandText(
                    DBGUID_DEFAULT, reinterpret_cast<const wchar_t *>(query.unicode())))) {
                HRESULT hr = commandText->Execute(
                        0, IID_IRowsetScroll, 0, 0, reinterpret_cast<IUnknown **>(&rowSet));

                if (!SUCCEEDED(hr)) {
                    qWarning("Failed to execute command %x, %s", hr, qPrintable(query));
                }
                *result = 0;
            }
            commandText->Release();
        }
        command->Release();
    }
    return rowSet;
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createItemResponse(
        QGalleryItemRequest *request)
{
    QWS4GalleryQueryBuilder builder;

    builder.setFields(request->fields());
    builder.setSortFields(request->sortFields());
    builder.setItemType(request->itemType());
    builder.setFilter(request->filter());

    int result = builder.buildQuery();

    if (result == 0) {
        if (IRowsetScroll *rowSet = execute(builder.query(), request->isLive(), &result))
            return new QWS4GalleryItemResponse(rowSet, *request, builder.columns());
    }

    return new QGalleryErrorResponse(result);
}


QGalleryAbstractResponse *QDocumentGalleryPrivate::createCountResponse(
        QGalleryCountRequest *request)
{
    QWS4GalleryQueryBuilder builder;

    builder.setItemType(request->itemType());
    builder.setFilter(request->filter());

    int result = builder.buildQuery();

    if (result == 0) {
        if (IRowsetScroll *rowSet = execute(builder.query(), request->isLive(), &result))
            return new QWS4GalleryCountResponse(rowSet);
    }

    return new QGalleryErrorResponse(result);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createCopyResponse(QGalleryCopyRequest *request)
{
    return new QWS4GalleryCopyResponse(*request);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createMoveResponse(QGalleryMoveRequest *request)
{
    return new QWS4GalleryMoveResponse(*request);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createRemoveResponse(QGalleryRemoveRequest *request)
{
    return new QWS4GalleryRemoveResponse(*request);
}

QDocumentGallery::QDocumentGallery(QObject *parent)
    : QAbstractGallery(*new QDocumentGalleryPrivate, parent)
{
    Q_D(QDocumentGallery);

    d->open();

    if (!d->commandFactory) {
        qWarning("No command factory");
    }
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
    if (!d_func()->commandFactory)
        return new QGalleryErrorResponse(QGalleryAbstractRequest::ConnectionError);

    switch (request->type()) {
    case QGalleryAbstractRequest::Item:
        return d_func()->createItemResponse(static_cast<QGalleryItemRequest *>(request));
    case QGalleryAbstractRequest::Count:
        return d_func()->createCountResponse(static_cast<QGalleryCountRequest *>(request));
    case QGalleryAbstractRequest::Copy:
        return d_func()->createCopyResponse(static_cast<QGalleryCopyRequest *>(request));
    case QGalleryAbstractRequest::Move:
        return d_func()->createMoveResponse(static_cast<QGalleryMoveRequest *>(request));
    case QGalleryAbstractRequest::Remove:
        return d_func()->createRemoveResponse(static_cast<QGalleryRemoveRequest *>(request));
    default:
        return 0;
    }
}
