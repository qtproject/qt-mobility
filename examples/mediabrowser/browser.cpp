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
    : QMainWindow(parent, flags)
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
    connect(artistView, SIGNAL(showAlbums(QVariant,QString)),
            this, SLOT(showAlbums(QVariant,QString)));
    connect(artistView, SIGNAL(showSongs(QVariant,QString)),
            this, SLOT(showSongs(QVariant,QString)));

    albumArtistView = new ArtistView(QDocumentGallery::AlbumArtist);
    albumArtistView->setGallery(gallery);
    connect(albumArtistView, SIGNAL(showAlbums(QVariant,QString)),
            this, SLOT(showAlbums(QVariant,QString)));
    connect(albumArtistView, SIGNAL(showSongs(QVariant,QString)),
            this, SLOT(showSongs(QVariant,QString)));

    albumView = new AlbumView;
    albumView->setGallery(gallery);
    connect(albumView, SIGNAL(showSongs(QVariant,QString)),
            this, SLOT(showSongs(QVariant,QString)));

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

    menuBar()->addAction(tr("Artists"), this, SLOT(showArtists()));
    menuBar()->addAction(tr("Album Artists"), this, SLOT(showAlbumArtists()));
    menuBar()->addAction(tr("Albums"), this, SLOT(showAlbums()));
    menuBar()->addAction(tr("Songs"), this, SLOT(showSongs()));
    menuBar()->addAction(tr("Photos"), this, SLOT(showPhotos()));

    setCentralWidget(stack);
    showArtists();

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
}

Browser::~Browser()
{
}

void Browser::showArtists()
{
    showView(artistView, tr("Artists"));
}

void Browser::showArtists(const QVariant &containerId, const QString &title)
{
#ifdef Q_WS_MAEMO_5
    ArtistView *artistView = new ArtistView(QDocumentGallery::Artist);
    artistView->setGallery(gallery);
    connect(artistView, SIGNAL(showAlbums(QVariant,QString)),
            this, SLOT(showAlbums(QVariant,QString)));
    connect(artistView, SIGNAL(showSongs(QVariant,QString)),
            this, SLOT(showSongs(QVariant,QString)));
#endif

    showView(artistView, containerId, title);
}

void Browser::showAlbumArtists()
{
    showView(albumArtistView, tr("Album Artists"));
}

void Browser::showAlbumArtists(const QVariant &containerId, const QString &title)
{
#ifdef Q_WS_MAEMO_5
    ArtistView *albumArtistView = new ArtistView(QDocumentGallery::AlbumArtist);
    albumArtistView->setGallery(gallery);
    connect(albumArtistView, SIGNAL(showAlbums(QVariant,QString)),
            this, SLOT(showAlbums(QVariant,QString)));
    connect(albumArtistView, SIGNAL(showSongs(QVariant,QString)),
            this, SLOT(showSongs(QVariant,QString)));
#endif

    showView(albumArtistView, containerId, title);
}

void Browser::showAlbums()
{
    showView(albumView, tr("Albums"));
}

void Browser::showAlbums(const QVariant &containerId, const QString &title)
{
#ifdef Q_WS_MAEMO_5
    AlbumView *albumView = new AlbumView;
    albumView->setGallery(gallery);
    connect(albumView, SIGNAL(showSongs(QVariant,QString)),
            this, SLOT(showSongs(QVariant,QString)));
#endif

    showView(albumView, containerId, title);
}

void Browser::showSongs()
{
    showView(songView, tr("Songs"));
}

void Browser::showSongs(const QVariant &containerId, const QString &title)
{
#ifdef Q_WS_MAEMO_5
    SongView *songView = new SongView;
    songView->setGallery(gallery);
#endif

    showView(songView, containerId, title);
}

void Browser::showPhotos()
{
    showView(photoView, tr("Photos"));
}

void Browser::showPhotos(const QVariant &containerId, const QString &title)
{
#ifdef Q_WS_MAEMO_5
    PhotoView *photoView = new PhotoView;
    photoView->setGallery(gallery);
#endif

    showView(photoView, containerId, title);
}

void Browser::showView(GalleryView *view, const QString &title)
{
    view->showChildren(QVariant());

    stack->setCurrentWidget(view);

    setWindowTitle(title);
}

void Browser::showView(GalleryView *view, const QVariant &containerId, const QString &title)
{
    view->showChildren(containerId);

#ifdef Q_WS_MAEMO_5
    QWidget *parent = qobject_cast<QWidget *>(sender());
    if (parent)
        view->setParent(parent->window(), Qt::Window);
    view->setAttribute(Qt::WA_Maemo5StackedWindow);
    view->setAttribute(Qt::WA_DeleteOnClose);
    view->setWindowTitle(title);
    view->show();
#else
    stack->setCurrentWidget(view);

    setWindowTitle(title);
#endif
}
