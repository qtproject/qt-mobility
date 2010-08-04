/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "galleryview.h"

#include <qgallerycountrequest.h>
#include <qgalleryitemlist.h>
#include <qgalleryqueryrequest.h>

GalleryView::GalleryView(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , request(new QGalleryQueryRequest(this))
    , countRequest(new QGalleryCountRequest(this))
{
    request->setLive(true);

    connect(request, SIGNAL(itemsChanged(QGalleryItemList*)),
            this, SLOT(mediaChanged(QGalleryItemList*)));
    connect(countRequest, SIGNAL(countChanged()), this, SLOT(requestCountChanged()));
}

GalleryView::~GalleryView()
{
}

QAbstractGallery *GalleryView::gallery() const
{
    return request->gallery();
}

void GalleryView::setGallery(QAbstractGallery *gallery)
{
    request->setGallery(gallery);
    countRequest->setGallery(gallery);
}

void GalleryView::showChildren(const QVariant &containerId)
{
    request->setScopeItemId(containerId);
    request->execute();

    countRequest->setScopeItemId(containerId);
    countRequest->execute();
}

void GalleryView::setType(const QString &type)
{
    request->setItemType(type);

    countRequest->setItemType(type);
}

void GalleryView::setFields(const QStringList &fields)
{
    request->setPropertyNames(fields);
}

void GalleryView::setSortFields(const QStringList &fields)
{
    request->setSortPropertyNames(fields);
}

QGalleryItemList *GalleryView::media() const
{
    return request->items();
}

void GalleryView::requestCountChanged()
{
    emit countChanged(countRequest->count());
}
