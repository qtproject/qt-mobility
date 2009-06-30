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

#include "qmediaplaylistmodel.h"
#include "qmediaplaylist.h"

#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayerservice.h"

#include <QtGui>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , service(0)
    , slider(0)
{
    service = new QGstreamerPlayerService(this);

    connect(service->control(), SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(service->control(), SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));    
    connect(service->control(), SIGNAL(playlistPositionChanged(int)), this, SLOT(setCurrentItem(int)));

    QWidget *videoWidget = service->createVideoWidget();

    if (videoWidget)
        service->setVideoOutput(videoWidget);

    playlistModel = new QMediaPlaylistModel(service->playlist(),this);

    playlistView = new QListView;
    playlistView->setModel(playlistModel);

    connect(playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 0);

    QPushButton *openButton = new QPushButton(tr("Open"));
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    QPushButton *playButton = new QPushButton(tr("Play"));
    connect(playButton, SIGNAL(clicked()), service->control(), SLOT(play()));

    QPushButton *pauseButton = new QPushButton(tr("Pause"));
    connect(pauseButton, SIGNAL(clicked()), service->control(), SLOT(pause()));

    QPushButton *stopButton = new QPushButton(tr("Stop"));
    connect(stopButton, SIGNAL(clicked()), service->control(), SLOT(stop()));

    QPushButton *advanceButton = new QPushButton(tr("Advance"));
    connect(advanceButton, SIGNAL(clicked()), service->control(), SLOT(advance()));

    QPushButton *backButton = new QPushButton(tr("Back"));
    connect(backButton, SIGNAL(clicked()), service->control(), SLOT(back()));

    QLabel *volumeLabel = new QLabel(tr("Volume"));

    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(service->control()->volume());
    connect(volumeSlider, SIGNAL(valueChanged(int)), service->control(), SLOT(setVolume(int)));

    QPushButton *muteButton = new QPushButton(tr("Mute"));
    muteButton->setCheckable(true);
    muteButton->setChecked(service->control()->isMuted());
    connect(muteButton, SIGNAL(clicked(bool)), service->control(), SLOT(setMuted(bool)));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(volumeLabel);
    controlLayout->addWidget(volumeSlider);
    controlLayout->addWidget(muteButton);
    controlLayout->addWidget(backButton);
    controlLayout->addWidget(advanceButton);

    QBoxLayout *layout = new QVBoxLayout;
    if (videoWidget) {
        QSplitter *splitter = new QSplitter(Qt::Vertical);

        splitter->addWidget(videoWidget);
        splitter->addWidget(playlistView);

        layout->addWidget(splitter);
    } else {
        layout->addWidget(playlistView);
    }
    layout->addWidget(slider);
    layout->addLayout(controlLayout);

    setLayout(layout);
}

Player::~Player()
{
    delete service;
}

void Player::open()
{
    QString fileName = QFileDialog::getOpenFileName();

    if (!fileName.isNull()) {
        service->playlist()->append(QMediaSource("", QUrl(QLatin1String("file:///") + fileName)));        
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

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        service->control()->jump(index.row());
    }
}

void Player::setCurrentItem(int currentItem)
{
    playlistView->setCurrentIndex(playlistModel->index(currentItem));
}
