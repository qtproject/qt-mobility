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
    connect(artistView, SIGNAL(showAlbums(QString)), this, SLOT(showAlbums(QString)));
    connect(artistView, SIGNAL(showSongs(QString)), this, SLOT(showSongs(QString)));

    albumArtistView = new ArtistView(QDocumentGallery::AlbumArtist);
    albumArtistView->setGallery(gallery);
    connect(albumArtistView, SIGNAL(showAlbums(QString)), this, SLOT(showAlbums(QString)));
    connect(albumArtistView, SIGNAL(showSongs(QString)), this, SLOT(showSongs(QString)));

    albumView = new AlbumView;
    albumView->setGallery(gallery);
    connect(albumView, SIGNAL(showSongs(QString)), this, SLOT(showSongs(QString)));

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

void Browser::showArtists(const QString &containerId)
{
    artistView->showChildren(containerId);

    stack->setCurrentWidget(artistView);
}

void Browser::showAlbumArtists(const QString &containerId)
{
    albumArtistView->showChildren(containerId);

    stack->setCurrentWidget(albumArtistView);
}

void Browser::showAlbums(const QString &containerId)
{
    albumView->showChildren(containerId);

    stack->setCurrentWidget(albumView);
}

void Browser::showSongs(const QString &containerId)
{
    songView->showChildren(containerId);

    stack->setCurrentWidget(songView);
}

void Browser::showPhotos(const QString &containerId)
{
    photoView->showChildren(containerId);

    stack->setCurrentWidget(photoView);
}

