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

#include "slideshow.h"

#include <qmediaservice.h>
#include <qmediaplaylist.h>
#include <qvideowidget.h>

#include <QtGui>

SlideShow::SlideShow(QWidget *parent)
    : QWidget(parent)
    , imageViewer(0)
    , playlist(0)
    , statusLabel(0)
    , countdownLabel(0)
    , playButton(0)
    , stopButton(0)
    , viewerLayout(0)
{
    imageViewer = new QMediaImageViewer(this);

    connect(imageViewer, SIGNAL(stateChanged(QMediaImageViewer::State)),
            this, SLOT(stateChanged(QMediaImageViewer::State)));
    connect(imageViewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            this, SLOT(statusChanged(QMediaImageViewer::MediaStatus)));
    connect(imageViewer, SIGNAL(elapsedTimeChanged(int)), this, SLOT(elapsedTimeChanged(int)));

    playlist = new QMediaPlaylist;
    playlist->setMediaObject(imageViewer);

    connect(playlist, SIGNAL(loaded()), this, SLOT(playlistLoaded()));
    connect(playlist, SIGNAL(loadFailed()), this, SLOT(playlistLoadFailed()));

    QVideoWidget *videoWidget = new QVideoWidget;
    videoWidget->setMediaObject(imageViewer);

    statusLabel = new QLabel(tr("%1 Images").arg(0));
    statusLabel->setAlignment(Qt::AlignCenter);

    viewerLayout = new QStackedLayout;
    viewerLayout->setStackingMode(QStackedLayout::StackAll);
    viewerLayout->addWidget(videoWidget);
    viewerLayout->addWidget(statusLabel);

    QMenu *openMenu = new QMenu(this);
    openMenu->addAction(tr("Directory..."), this, SLOT(openDirectory()));
    openMenu->addAction(tr("Playlist..."), this, SLOT(openPlaylist()));

    QToolButton *openButton = new QToolButton;
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    openButton->setMenu(openMenu);
    openButton->setPopupMode(QToolButton::InstantPopup);

    playButton = new QToolButton;
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->setEnabled(false);

    connect(playButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(this, SIGNAL(enableButtons(bool)), playButton, SLOT(setEnabled(bool)));

    stopButton = new QToolButton;
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setEnabled(false);

    connect(stopButton, SIGNAL(clicked()), imageViewer, SLOT(stop()));

    QAbstractButton *nextButton = new QToolButton;
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    nextButton->setEnabled(false);

    connect(nextButton, SIGNAL(clicked()), playlist, SLOT(next()));
    connect(this, SIGNAL(enableButtons(bool)), nextButton, SLOT(setEnabled(bool)));

    QAbstractButton *previousButton = new QToolButton;
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    previousButton->setEnabled(false);

    connect(previousButton, SIGNAL(clicked()), playlist, SLOT(previous()));
    connect(this, SIGNAL(enableButtons(bool)), previousButton, SLOT(setEnabled(bool)));

    countdownLabel = new QLabel;

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(previousButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(nextButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(countdownLabel);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(viewerLayout);
    layout->addLayout(controlLayout);

    setLayout(layout);

}

void SlideShow::openPlaylist()
{
    QString path = QFileDialog::getOpenFileName(this);

    if (!path.isEmpty()) {
        playlist->clear();
        playlist->load(QUrl::fromLocalFile(path));
    }
}

void SlideShow::openDirectory()
{
    QString path = QFileDialog::getExistingDirectory(this);

    if (!path.isEmpty()) {
        playlist->clear();

        QDir dir(path);

        foreach (const QString &fileName, dir.entryList(QDir::Files))
            playlist->addMedia(QUrl::fromLocalFile(dir.absoluteFilePath(fileName)));

        statusChanged(imageViewer->mediaStatus());

        emit enableButtons(playlist->mediaCount() > 0);
    }
}

void SlideShow::play()
{
    switch (imageViewer->state()) {
    case QMediaImageViewer::StoppedState:
    case QMediaImageViewer::PausedState:
        imageViewer->play();
        break;
    case QMediaImageViewer::PlayingState:
        imageViewer->pause();
        break;
    }
}

void SlideShow::stateChanged(QMediaImageViewer::State state)
{
    switch (state) {
    case QMediaImageViewer::StoppedState:
        stopButton->setEnabled(false);
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMediaImageViewer::PlayingState:
        stopButton->setEnabled(true);
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaImageViewer::PausedState:
        stopButton->setEnabled(true);
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void SlideShow::statusChanged(QMediaImageViewer::MediaStatus status)
{
    switch (status) {
    case QMediaImageViewer::NoMedia:
        statusLabel->setText(tr("%1 Images").arg(playlist->mediaCount()));
        viewerLayout->setCurrentIndex(1);
        break;
    case QMediaImageViewer::LoadingMedia:
        statusLabel->setText(tr("Image %1 of %2\nLoading...")
                .arg(playlist->currentIndex())
                .arg(playlist->mediaCount()));
        viewerLayout->setCurrentIndex(1);
        break;
    case QMediaImageViewer::LoadedMedia:
        statusLabel->setText(tr("Image %1 of %2")
                .arg(playlist->currentIndex())
                .arg(playlist->mediaCount()));
        viewerLayout->setCurrentIndex(0);
        break;
    case QMediaImageViewer::InvalidMedia:
        statusLabel->setText(tr("Image %1 of %2\nInvalid")
                .arg(playlist->currentIndex())
                .arg(playlist->mediaCount()));
        viewerLayout->setCurrentIndex(1);
        break;
    default:
        break;
    }
}

void SlideShow::playlistLoaded()
{
    statusChanged(imageViewer->mediaStatus());

    emit enableButtons(playlist->mediaCount() > 0);
}

void SlideShow::playlistLoadFailed()
{
    statusLabel->setText(playlist->errorString());
    viewerLayout->setCurrentIndex(1);

    emit enableButtons(false);
}

void SlideShow::elapsedTimeChanged(int time)
{
    const int remaining = (imageViewer->timeout() - time) / 1000;

    countdownLabel->setText(tr("%1:%2")
            .arg(remaining / 60, 2, 10, QLatin1Char('0'))
            .arg(remaining % 60, 2, 10, QLatin1Char('0')));
}
