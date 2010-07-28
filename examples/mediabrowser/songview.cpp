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

#include "songview.h"

#include <QtGui>

#include <qdocumentgallery.h>
#include <qgalleryitemlistmodel.h>

SongView::SongView(QWidget *parent, Qt::WindowFlags flags)
    : GalleryView(parent, flags)
    , model(0)
{
    setType(QDocumentGallery::Audio);
    setFields(QStringList()
            << QDocumentGallery::trackNumber
            << QDocumentGallery::title
            << QDocumentGallery::duration
            << QDocumentGallery::artist
            << QDocumentGallery::albumArtist);
    setSortFields(QStringList()
            << QDocumentGallery::artist
            << QDocumentGallery::albumTitle
            << QDocumentGallery::trackNumber);

    model = new QGalleryItemListModel;
    model->addColumn(QDocumentGallery::trackNumber);
    model->addColumn(QDocumentGallery::title);
    model->addColumn(QDocumentGallery::duration);
    model->addColumn(QDocumentGallery::artist);
    model->addColumn(QDocumentGallery::albumArtist);

    model->setHeaderData(0, Qt::Horizontal, tr("Track"));
    model->setHeaderData(1, Qt::Horizontal, tr("Title"));
    model->setHeaderData(2, Qt::Horizontal, tr("Duration"));
    model->setHeaderData(3, Qt::Horizontal, tr("Artist"));
    model->setHeaderData(4, Qt::Horizontal, tr("Album Artist"));

    QTableView *view = new QTableView;
    view->setShowGrid(false);
    view->verticalHeader()->setVisible(false);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setModel(model);
    connect(view, SIGNAL(activated(QModelIndex)), this, SLOT(activated(QModelIndex)));

    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(view);

    setLayout(layout);
}

SongView::~SongView()
{
    delete model;
}

void SongView::mediaChanged(QGalleryItemList *media)
{
    model->setItemList(media);
}

void SongView::activated(const QModelIndex &)
{
}

