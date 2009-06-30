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
