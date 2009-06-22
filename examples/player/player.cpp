#include "player.h"

#include "qwmpplayercontrol.h"
#include "qwmpplayerservice.h"

#include <QtGui>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , service(0)
{
    service = new QWmpPlayerService;

    QPushButton *openButton = new QPushButton(tr("Open"));
    QPushButton *playButton = new QPushButton(tr("Play"));
    QPushButton *pauseButton = new QPushButton(tr("Pause"));
    QPushButton *stopButton = new QPushButton(tr("Stop"));

    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
    connect(playButton, SIGNAL(clicked()), service->control(), SLOT(play()));
    connect(pauseButton, SIGNAL(clicked()), service->control(), SLOT(pause()));
    connect(stopButton, SIGNAL(clicked()), service->control(), SLOT(stop()));

    QBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(openButton);
    layout->addWidget(playButton);
    layout->addWidget(pauseButton);
    layout->addWidget(stopButton);
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
