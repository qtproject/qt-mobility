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

#include "albumview.h"

#include "albumdelegate.h"
#include "gallerymodel.h"

#include <QtGui>

#include <qdocumentgallery.h>
#include <qgalleryitemlist.h>

AlbumView::AlbumView(QWidget *parent)
    : GalleryView(parent)
{
    setType(QDocumentGallery::Album);
    setFields(QStringList()
            << QDocumentGallery::title
            << QDocumentGallery::artist
            << QDocumentGallery::trackCount
            << QDocumentGallery::duration
            << QDocumentGallery::rating
            << QDocumentGallery::thumbnail);
    setSortFields(QStringList()
            << QDocumentGallery::title);

    model = new GalleryModel;
    model->setDisplayFieldForColumn(0, QDocumentGallery::title);
    model->setDecorationFieldForColumn(0, QDocumentGallery::thumbnail);
    model->setUserRoleFields(QVector<QString>() << QDocumentGallery::artist);

    QListView *view = new QListView;
    view->setIconSize(QSize(124, 124));
    view->setFlow(QListView::LeftToRight);
    view->setViewMode(QListView::IconMode);
    view->setSpacing(4);
    view->setUniformItemSizes(true);
    view->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
    view->setModel(model);
    view->setItemDelegate(new AlbumDelegate(this));
    connect(view->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    connect(view, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));


    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(view);

    setLayout(layout);
}

AlbumView::~AlbumView()
{
}

void AlbumView::mediaChanged()
{
    model->setList(media());
}

void AlbumView::activated(const QModelIndex &index)
{
    emit showSongs(media()->id(index.row()));
}

