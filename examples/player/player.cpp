/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "player.h"

#include "playercontrols.h"
#include "playlistmodel.h"

#include <qmediaplaylist.h>
#include <qmediametadata.h>
#include <qabstractmediaservice.h>

#include "qmediawidgetendpoint.h"

#include <QtGui>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , slider(0)
{
    player = new QMediaPlayer;
    metaData = new QMediaMetadata(player);

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(playlistPositionChanged(int)), SLOT(playlistPositionChanged(int)));
    connect(metaData, SIGNAL(metadataChanged()), SLOT(metadataChanged()));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(player, SIGNAL(bufferingChanged(bool)), this, SLOT(bufferingChanged(bool)));
    connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));

    QWidget *videoWidget = player->service()->createEndpoint<QMediaWidgetEndpoint *>();

    if (videoWidget) {
        qDebug() << "service supports video widgets, nice";
        player->service()->setVideoOutput(videoWidget);
    }

    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(player->mediaPlaylist());

    playlistView = new QTableView;
    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(player->playlistPosition(), 0));

    connect(playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, player->duration() / 1000);

    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));

    QPushButton *openButton = new QPushButton(tr("Open"));

    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    PlayerControls *controls = new PlayerControls;
    controls->setState(player->state());
    controls->setVolume(player->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, SIGNAL(play()), player, SLOT(play()));
    connect(controls, SIGNAL(pause()), player, SLOT(pause()));
    connect(controls, SIGNAL(stop()), player, SLOT(stop()));
    connect(controls, SIGNAL(next()), player, SLOT(advance()));
    connect(controls, SIGNAL(previous()), player, SLOT(back()));
    connect(controls, SIGNAL(changeVolume(int)), player, SLOT(setVolume(int)));
    connect(controls, SIGNAL(changeMuting(bool)), player, SLOT(setMuted(bool)));

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
            controls, SLOT(setState(QMediaPlayer::State)));
    connect(player, SIGNAL(volumeChanged(int)), controls, SLOT(setVolume(int)));
    connect(player, SIGNAL(mutingChanged(bool)), controls, SLOT(setMuted(bool)));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
    if (videoWidget) {
        QSplitter *splitter = new QSplitter(Qt::Vertical);

        splitter->addWidget(videoWidget);
        splitter->addWidget(playlistView);

        /*
        connect(player, SIGNAL(videoAvailabilityChanged(bool)), videoWidget, SLOT(setVisible(bool)));
        videoWidget->setMinimumSize(64,64);
        videoWidget->setVisible(false);*/

        layout->addWidget(splitter);
    } else {
        layout->addWidget(playlistView);
    }
    layout->addWidget(slider);
    layout->addLayout(controlLayout);

    setLayout(layout);

    metadataChanged();
}

Player::~Player()
{
    delete player;
}

void Player::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames();

    foreach (QString const &fileName, fileNames) {
#ifndef Q_OS_WIN
        player->mediaPlaylist()->appendItem(
                QMediaResource(QUrl(QLatin1String("file://") + fileName)));
#else
        player->mediaPlaylist()->appendItem(
                QMediaResource(QUrl(QLatin1String("file:///") + fileName)));
#endif
    }
}

void Player::durationChanged(qint64 duration)
{
    slider->setMaximum(duration / 1000);
}

void Player::positionChanged(qint64 progress)
{
    slider->setValue(progress / 1000);
}

void Player::metadataChanged()
{
    qDebug() << "update metadata" << metaData->metadata(QLatin1String("title")).toString();
    if (metaData->metadataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(metaData->metadata(QLatin1String("Artist")).toString())
                .arg(metaData->metadata(QLatin1String("Title")).toString()));
    }
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        player->setPlaylistPosition(index.row());
    }
}

void Player::playlistPositionChanged(int currentItem)
{
    playlistView->setCurrentIndex(playlistModel->index(currentItem, 0));
}

void Player::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::PrimedMedia:
        unsetCursor();
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setCursor(QCursor(Qt::BusyCursor));
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setCursor(QCursor(Qt::BusyCursor));
        break;
    case QMediaPlayer::EndOfMedia:
        unsetCursor();
        setStatusInfo(QString());
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        unsetCursor();
        setStatusInfo(player->errorString());
        break;
    }
}

void Player::bufferingChanged(bool buffering)
{
    if (buffering)
        statusChanged(player->mediaStatus());
}

void Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%%").arg(progress));
}

void Player::setTrackInfo(const QString &info)
{
    trackInfo = info;

    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);

}

void Player::setStatusInfo(const QString &info)
{
    statusInfo = info;

    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
}
