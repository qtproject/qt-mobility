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

#include "photoview.h"

#include "photodelegate.h"
#include "gallerymodel.h"

#include <QtGui>

#include <qdocumentgallery.h>
#include <qgalleryitemlist.h>

PhotoView::PhotoView(QWidget *parent)
    : GalleryView(parent)
{
    setType(QDocumentGallery::Image);
    setFields(QStringList()
            << QDocumentGallery::fileName
            << QDocumentGallery::thumbnailImage);
    setSortFields(QStringList()
            << QDocumentGallery::title);

    model = new GalleryModel;
    model->setDisplayFieldForColumn(0, QDocumentGallery::fileName);
    model->setDecorationFieldForColumn(0, QDocumentGallery::thumbnailImage);

    QListView *view = new QListView;
    view->setIconSize(QSize(124, 124));
    view->setFlow(QListView::TopToBottom);
    view->setViewMode(QListView::IconMode);
    view->setSpacing(4);
    view->setUniformItemSizes(true);
    view->setWrapping(true);
    view->setModel(model);
    view->setItemDelegate(new PhotoDelegate(this));
    connect(view, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));


    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(view);

    setLayout(layout);
}

PhotoView::~PhotoView()
{
}

void PhotoView::mediaChanged()
{
    model->setList(media());
}

void PhotoView::activated(const QModelIndex &index)
{
}

