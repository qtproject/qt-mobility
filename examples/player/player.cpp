#include "player.h"

#include "playlistmodel.h"

#include <qmediaplayer.h>
#include <qmediaplaylist.h>
#include <qmediametadata.h>

#include <QtGui>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , slider(0)
{
    player = new QMediaPlayer;
    metaData = new QMediaMetadata(player);

    connect(player, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(metaData, SIGNAL(metaDataChanged()), this, SLOT(metaDataChanged()));

    QWidget *videoWidget = 0; /*service->createWidget();

    if (videoWidget)
        service->setVideoOutput(videoWidget);
    */
    PlaylistModel *playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(player->mediaPlaylist());

    QTableView *playlistView = new QTableView;
    playlistView->setModel(playlistModel);

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 0);

    QPushButton *openButton = new QPushButton(tr("Open"));
    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    QPushButton *playButton = new QPushButton(tr("Play"));
    connect(playButton, SIGNAL(clicked()), player, SLOT(play()));

    QPushButton *pauseButton = new QPushButton(tr("Pause"));
    connect(pauseButton, SIGNAL(clicked()), player, SLOT(pause()));

    QPushButton *stopButton = new QPushButton(tr("Stop"));
    connect(stopButton, SIGNAL(clicked()), player, SLOT(stop()));

    QLabel *volumeLabel = new QLabel(tr("Volume"));

    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(player->volume());
    connect(volumeSlider, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));

    QPushButton *muteButton = new QPushButton(tr("Mute"));
    muteButton->setCheckable(true);
    muteButton->setChecked(player->isMuted());
    connect(muteButton, SIGNAL(clicked(bool)), player, SLOT(setMuted(bool)));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(volumeLabel);
    controlLayout->addWidget(volumeSlider);
    controlLayout->addWidget(muteButton);

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
    delete player;
}

void Player::open()
{
    QString fileName = QFileDialog::getOpenFileName();

    if (!fileName.isNull()) {
        player->mediaPlaylist()->append(QMediaSource("", QLatin1String("file:///") + fileName));
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
    setWindowTitle(metaData->valueFor(QLatin1String("Title")).toString());
}
