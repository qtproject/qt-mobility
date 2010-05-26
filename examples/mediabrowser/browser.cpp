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

#include "browser.h"

#include "albumview.h"
#include "artistview.h"
#include "photoview.h"
#include "songview.h"

#include "qdocumentgallery.h"

#include <QtGui>

Browser::Browser(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
    , gallery(0)
    , stack(0)
    , artistView(0)
    , albumArtistView(0)
    , albumView(0)
    , songView(0)
    , photoView(0)
{
    gallery = new QDocumentGallery;

    artistView = new ArtistView(QDocumentGallery::Artist);
    artistView->setGallery(gallery);
    connect(artistView, SIGNAL(showAlbums(QVariant)), this, SLOT(showAlbums(QVariant)));
    connect(artistView, SIGNAL(showSongs(QVariant)), this, SLOT(showSongs(QVariant)));

    albumArtistView = new ArtistView(QDocumentGallery::AlbumArtist);
    albumArtistView->setGallery(gallery);
    connect(albumArtistView, SIGNAL(showAlbums(QVariant)), this, SLOT(showAlbums(QVariant)));
    connect(albumArtistView, SIGNAL(showSongs(QVariant)), this, SLOT(showSongs(QVariant)));

    albumView = new AlbumView;
    albumView->setGallery(gallery);
    connect(albumView, SIGNAL(showSongs(QVariant)), this, SLOT(showSongs(QVariant)));

    songView = new SongView;
    songView->setGallery(gallery);

    photoView = new PhotoView;
    photoView->setGallery(gallery);

    stack = new QStackedWidget;
    stack->addWidget(artistView);
    stack->addWidget(albumArtistView);
    stack->addWidget(albumView);
    stack->addWidget(songView);
    stack->addWidget(photoView);

    QPushButton *artistButton = new QPushButton(tr("Artists"));
    connect(artistButton, SIGNAL(clicked()), this, SLOT(showArtists()));

    QPushButton *albumArtistButton = new QPushButton(tr("Album Artists"));
    connect(albumArtistButton, SIGNAL(clicked()), this, SLOT(showAlbumArtists()));

    QPushButton *albumButton = new QPushButton(tr("Albums"));
    connect(albumButton, SIGNAL(clicked()), this, SLOT(showAlbums()));

    QPushButton *songButton = new QPushButton(tr("Songs"));
    connect(songButton, SIGNAL(clicked()), this, SLOT(showSongs()));

    QPushButton *photoButton = new QPushButton(tr("Photos"));
    connect(photoButton, SIGNAL(clicked()), this, SLOT(showPhotos()));

    QBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(artistButton);
    hLayout->addWidget(albumArtistButton);
    hLayout->addWidget(albumButton);
    hLayout->addWidget(songButton);
    hLayout->addWidget(photoButton);

    QBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setSpacing(0);
    vLayout->addWidget(stack);
    vLayout->addLayout(hLayout);

    setLayout(vLayout);
}

Browser::~Browser()
{
}

void Browser::showArtists(const QVariant &containerId)
{
    artistView->showChildren(containerId);

    stack->setCurrentWidget(artistView);
}

void Browser::showAlbumArtists(const QVariant &containerId)
{
    albumArtistView->showChildren(containerId);

    stack->setCurrentWidget(albumArtistView);
}

void Browser::showAlbums(const QVariant &containerId)
{
    albumView->showChildren(containerId);

    stack->setCurrentWidget(albumView);
}

void Browser::showSongs(const QVariant &containerId)
{
    songView->showChildren(containerId);

    stack->setCurrentWidget(songView);
}

void Browser::showPhotos(const QVariant &containerId)
{
    photoView->showChildren(containerId);

    stack->setCurrentWidget(photoView);
}

