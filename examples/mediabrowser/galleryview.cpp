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

#include "galleryview.h"

#include <qgallerycountrequest.h>
#include <qgalleryfilterrequest.h>
#include <qgalleryitemlist.h>

GalleryView::GalleryView(QWidget *parent)
    : QWidget(parent)
    , request(new QGalleryFilterRequest(this))
    , countRequest(new QGalleryCountRequest(this))
{
    connect(request, SIGNAL(itemsChanged()), this, SLOT(mediaChanged()));
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
    request->setContainerId(containerId);
    request->execute();

    countRequest->setContainerId(containerId);
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

void GalleryView::sliderMoved(int value)
{
    if (QGalleryItemList *list = request->items())
        list->setCursorPosition(value - list->minimumPagedItems() / 3);
}

void GalleryView::requestCountChanged()
{
    emit countChanged(countRequest->count());
}
