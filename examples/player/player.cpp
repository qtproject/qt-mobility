/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "player.h"

#include "playercontrols.h"
#include "playlistmodel.h"

#include <qabstractmediaservice.h>
#include <qmediaplaylist.h>

#include <QtGui>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , videoWidget(0)
    , coverLabel(0)
    , slider(0)
    , colorDialog(0)
{
    player = new QMediaPlayer;
    playlist = new QMediaPlaylist(player);

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(playlist, SIGNAL(playlistPositionChanged(int)), SLOT(playlistPositionChanged(int)));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));

    videoWidget = new QVideoWidget(player);

    connect(videoWidget, SIGNAL(displayModeChanged(QVideoWidget::DisplayMode)),
            this, SLOT(displayModeChanged(QVideoWidget::DisplayMode)));

    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(playlist);

    playlistView = new QTableView;
    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(playlist->currentPosition(), 0));

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
    connect(controls, SIGNAL(next()), playlist, SLOT(advance()));
    connect(controls, SIGNAL(previous()), playlist, SLOT(back()));
    connect(controls, SIGNAL(changeVolume(int)), player, SLOT(setVolume(int)));
    connect(controls, SIGNAL(changeMuting(bool)), player, SLOT(setMuted(bool)));
    connect(controls, SIGNAL(changeRate(float)), player, SLOT(setPlaybackRate(float)));

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
            controls, SLOT(setState(QMediaPlayer::State)));
    connect(player, SIGNAL(volumeChanged(int)), controls, SLOT(setVolume(int)));
    connect(player, SIGNAL(mutingChanged(bool)), controls, SLOT(setMuted(bool)));

    QPushButton *fullscreenButton = new QPushButton(tr("Fullscreen"));
    fullscreenButton->setCheckable(true);

    connect(fullscreenButton, SIGNAL(clicked(bool)), this, SLOT(setFullscreen(bool)));
    connect(this, SIGNAL(fullscreenChanged(bool)), fullscreenButton, SLOT(setChecked(bool)));

    fullscreenButton->setEnabled(videoWidget != 0);

    QPushButton *colorButton = new QPushButton(tr("Color Options..."));
    if (videoWidget)
        connect(colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    else
        colorButton->setEnabled(false);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);
    controlLayout->addWidget(fullscreenButton);
    controlLayout->addWidget(colorButton);

    QBoxLayout *layout = new QVBoxLayout;
    if (videoWidget) {
        QSplitter *splitter = new QSplitter(Qt::Vertical);

        splitter->addWidget(videoWidget);
        splitter->addWidget(playlistView);

        layout->addWidget(splitter);
    } else {
        layout->addWidget(coverLabel, 0, Qt::AlignCenter);
        layout->addWidget(playlistView);
    }
    layout->addWidget(slider);
    layout->addLayout(controlLayout);

    setLayout(layout);

    metaDataChanged();
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
        playlist->appendItem(QMediaSource(QUrl(QLatin1String("file://") + fileName)));
#else
        playlist->appendItem(QMediaSource(QUrl(QLatin1String("file:///") + fileName)));
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

void Player::metaDataChanged()
{
    //qDebug() << "update metadata" << player->metaData(QAbstractMediaObject::Title).toString();
    if (player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(player->metaData(QAbstractMediaObject::AlbumArtist).toString())
                .arg(player->metaData(QAbstractMediaObject::Title).toString()));

        if (coverLabel) {
            QUrl uri = player->metaData(QAbstractMediaObject::CoverArtUriLarge).value<QUrl>();

            coverLabel->setPixmap(!uri.isEmpty()
                    ? QPixmap(uri.toString())
                    : QPixmap());
        }
    }
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        playlist->setCurrentPosition(index.row());
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

void Player::setFullscreen(bool fullscreen)
{
    videoWidget->setDisplayMode(
            fullscreen ? QVideoWidget::FullscreenDisplay : QVideoWidget::WindowedDisplay);
}

void Player::displayModeChanged(QVideoWidget::DisplayMode mode)
{
    emit fullscreenChanged(mode == QVideoWidget::FullscreenDisplay);
}

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
