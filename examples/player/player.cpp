#include "player.h"

#include "qwmpmetadata.h"
#include "qwmpplayercontrol.h"
#include "qwmpplayerservice.h"

#include <QtGui>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , service(0)
    , slider(0)
{
    service = new QWmpPlayerService;    

    connect(service->control(), SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(service->control(), SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(service->metaData(), SIGNAL(changed()), this, SLOT(metaDataChanged()));

    QWidget *videoWidget = service->createWidget();

    if (videoWidget)
        service->setVideoOutput(videoWidget);

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

    QBoxLayout *layout = new QVBoxLayout;
    if (videoWidget)
        layout->addWidget(videoWidget);
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
        service->control()->setUrl(QLatin1String("file:///") + fileName);
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
    qDebug(qPrintable(service->metaData()->keys().join(QLatin1String("; "))));
    setWindowTitle(service->metaData()->value(QLatin1String("Title")).toString());
}
