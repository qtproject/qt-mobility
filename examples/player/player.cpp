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

#include "player.h"

#include "playercontrols.h"
#include "playlistmodel.h"
#include "videowidget.h"

#include <qmediaservice.h>
#include <qmediaplaylist.h>

#include <QtGui>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , videoWidget(0)
    , coverLabel(0)
    , slider(0)
#ifdef Q_OS_SYMBIAN    
    , mediaKeysObserver(0)
    , playlistDialog(0)
#else
    , colorDialog(0)
#endif    
{
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    playlist->setMediaObject(player);

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    videoWidget = new VideoWidget;
    videoWidget->setMediaObject(player);

    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(playlist);

    playlistView = new QListView;
    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));

    connect(playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, player->duration() / 1000);

    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));

#ifdef Q_OS_SYMBIAN
#else
    QPushButton *openButton = new QPushButton(tr("Open"), this);

    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

#endif
    PlayerControls *controls = new PlayerControls(this);
    controls->setState(player->state());
    controls->setVolume(player->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, SIGNAL(play()), player, SLOT(play()));
    connect(controls, SIGNAL(pause()), player, SLOT(pause()));
    connect(controls, SIGNAL(stop()), player, SLOT(stop()));
    connect(controls, SIGNAL(next()), playlist, SLOT(next()));
    connect(controls, SIGNAL(previous()), playlist, SLOT(previous()));
    connect(controls, SIGNAL(changeVolume(int)), player, SLOT(setVolume(int)));
    connect(controls, SIGNAL(changeMuting(bool)), player, SLOT(setMuted(bool)));
    connect(controls, SIGNAL(changeRate(qreal)), player, SLOT(setPlaybackRate(qreal)));

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
            controls, SLOT(setState(QMediaPlayer::State)));
    connect(player, SIGNAL(volumeChanged(int)), controls, SLOT(setVolume(int)));
    connect(player, SIGNAL(mutedChanged(bool)), controls, SLOT(setMuted(bool)));

#ifdef Q_OS_SYMBIAN
#else
    QPushButton *fullScreenButton = new QPushButton(tr("FullScreen"), this);
    fullScreenButton->setCheckable(true);

    if (videoWidget != 0) {
        connect(fullScreenButton, SIGNAL(clicked(bool)), videoWidget, SLOT(setFullScreen(bool)));
        connect(videoWidget, SIGNAL(fullScreenChanged(bool)),
                fullScreenButton, SLOT(setChecked(bool)));
    } else {
        fullScreenButton->setEnabled(false);
    }

    QPushButton *colorButton = new QPushButton(tr("Color Options..."));
    if (videoWidget)
        connect(colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    else
        colorButton->setEnabled(false);

#endif
    
#ifdef Q_OS_SYMBIAN
    // Set some sensible default volume.
    player->setVolume(50);
    
    QLabel *label = new QLabel(tr("Playlist"), this);
    QVBoxLayout *playlistDialogLayout = new QVBoxLayout;
    playlistDialogLayout->addWidget(label);
    playlistDialogLayout->addWidget(playlistView);
    playlistDialog = new QDialog(this);
    playlistDialog->setWindowTitle(tr("Playlist"));
    playlistDialog->setLayout(playlistDialogLayout);
    playlistDialog->setContextMenuPolicy(Qt::NoContextMenu);
    
    QAction *close = new QAction(tr("Close"), this);
    close->setSoftKeyRole(QAction::NegativeSoftKey);
    playlistDialog->addAction(close);
    
    mediaKeysObserver = new MediaKeysObserver(this);
    
    slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    slider->setMinimumSize(1, 1);
    
    connect(controls, SIGNAL(open()), this, SLOT(open()));
    connect(controls, SIGNAL(fullScreen(bool)), this, SLOT(handleFullScreen(bool)));
    connect(videoWidget, SIGNAL(fullScreenChanged(bool)), this, SLOT(handleFullScreen(bool)));
    connect(controls, SIGNAL(openPlayList()), this, SLOT(showPlayList()));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(handleStateChange(QMediaPlayer::State)));
    connect(mediaKeysObserver, SIGNAL(mediaKeyPressed(MediaKeysObserver::MediaKeys)), this, SLOT(handleMediaKeyEvent(MediaKeysObserver::MediaKeys)));
    connect(close, SIGNAL(triggered()), playlistDialog, SLOT(reject()));
    
    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoWidget, 7);
    layout->addWidget(slider, 1);
    layout->addWidget(controls, 2);
#else
    QBoxLayout *displayLayout = new QHBoxLayout;
    if (videoWidget)
        displayLayout->addWidget(videoWidget, 2);
    else
        displayLayout->addWidget(coverLabel, 2);
    displayLayout->addWidget(playlistView);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);
    controlLayout->addWidget(fullScreenButton);
    controlLayout->addWidget(colorButton);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    layout->addWidget(slider);
    layout->addLayout(controlLayout);
#endif    

    setLayout(layout);

    metaDataChanged();

    QStringList fileNames = qApp->arguments();
    fileNames.removeAt(0);
    foreach (QString const &fileName, fileNames) {
        if (fileName.startsWith(QLatin1String("http://")))
            playlist->addMedia(QUrl(fileName));
        else if (QFileInfo(fileName).exists())
            playlist->addMedia(QUrl::fromLocalFile(fileName));
    }
}

Player::~Player()
{
#ifdef Q_OS_SYMBIAN
    delete playlistDialog;
#else    
    delete playlist;
#endif    
    delete player;
}

void Player::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames();
    foreach (QString const &fileName, fileNames)
        playlist->addMedia(QUrl::fromLocalFile(fileName));
}

void Player::durationChanged(qint64 duration)
{
    slider->setMaximum(duration / 1000);
}

void Player::positionChanged(qint64 progress)
{
    if (!slider->isSliderDown()) {
        slider->setValue(progress / 1000);
    }
}

void Player::metaDataChanged()
{
    //qDebug() << "update metadata" << player->metaData(QtMedia::Title).toString();
    if (player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(player->metaData(QtMedia::AlbumArtist).toString())
                .arg(player->metaData(QtMedia::Title).toString()));

        if (coverLabel) {
            QUrl url = player->metaData(QtMedia::CoverArtUrlLarge).value<QUrl>();

            coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
}

void Player::jump(const QModelIndex &index)
{
#ifdef Q_OS_SYMBIAN
    if (playlistDialog->isVisible())
        playlistDialog->accept();
#endif
    if (index.isValid()) {
        playlist->setCurrentIndex(index.row());
        player->play();
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
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
#ifndef QT_NO_CURSOR
        setCursor(QCursor(Qt::BusyCursor));
#endif
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
#ifndef QT_NO_CURSOR
        setCursor(QCursor(Qt::BusyCursor));
#endif
        break;
    case QMediaPlayer::EndOfMedia:
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        setStatusInfo(QString());
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        setStatusInfo(player->errorString());
        break;
    }
}

void Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
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

void Player::displayErrorMessage()
{
    setStatusInfo(player->errorString());
}

#ifdef Q_OS_SYMBIAN
#else
void Player::showColorDialog()
{
    if (!colorDialog) {
        QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
        brightnessSlider->setRange(-100, 100);
        brightnessSlider->setValue(videoWidget->brightness());
        connect(brightnessSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setBrightness(int)));
        connect(videoWidget, SIGNAL(brightnessChanged(int)), brightnessSlider, SLOT(setValue(int)));

        QSlider *contrastSlider = new QSlider(Qt::Horizontal);
        contrastSlider->setRange(-100, 100);
        contrastSlider->setValue(videoWidget->contrast());
        connect(contrastSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setContrast(int)));
        connect(videoWidget, SIGNAL(contrastChanged(int)), contrastSlider, SLOT(setValue(int)));

        QSlider *hueSlider = new QSlider(Qt::Horizontal);
        hueSlider->setRange(-100, 100);
        hueSlider->setValue(videoWidget->hue());
        connect(hueSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setHue(int)));
        connect(videoWidget, SIGNAL(hueChanged(int)), hueSlider, SLOT(setValue(int)));

        QSlider *saturationSlider = new QSlider(Qt::Horizontal);
        saturationSlider->setRange(-100, 100);
        saturationSlider->setValue(videoWidget->saturation());
        connect(saturationSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setSaturation(int)));
        connect(videoWidget, SIGNAL(saturationChanged(int)), saturationSlider, SLOT(setValue(int)));

        QFormLayout *layout = new QFormLayout;
        layout->addRow(tr("Brightness"), brightnessSlider);
        layout->addRow(tr("Contrast"), contrastSlider);
        layout->addRow(tr("Hue"), hueSlider);
        layout->addRow(tr("Saturation"), saturationSlider);

        colorDialog = new QDialog(this);
        colorDialog->setWindowTitle(tr("Color Options"));
        colorDialog->setLayout(layout);
    }
    colorDialog->show();
}
#endif
#ifdef Q_OS_SYMBIAN
void Player::handleFullScreen(bool isFullscreen)
{
    if(isFullscreen) {
        showFullScreen();
        if(player->state()==QMediaPlayer::PlayingState || 
           player->state()==QMediaPlayer::PausedState)
            videoWidget->setFullScreen(true);
        else
            videoWidget->setFullScreen(false);
        
    } else
        showMaximized();
}

void Player::handleStateChange(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PausedState)
        return;
    
    handleFullScreen(isFullScreen());
}

void Player::handleMediaKeyEvent(MediaKeysObserver::MediaKeys key)
{
    switch (key) {
        case MediaKeysObserver::EVolIncKey: 
            player->setVolume(player->volume() + 10);
            break;
        case MediaKeysObserver::EVolDecKey:
            player->setVolume(player->volume() - 10);
            break;
        default:
        break;
    }
}
void Player::showPlayList()
{
    if (!playlistDialog)
        return;
    
    playlistDialog->exec();
}
#endif
