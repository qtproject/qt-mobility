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

#include "radio.h"

#include <qabstractmediaservice.h>
#include <qradioplayercontrol.h>

#include <QtGui>

Radio::Radio()
{
    player = new QRadioPlayer;
    connect(player,SIGNAL(frequencyChanged(int)),this,SLOT(freqChanged(int)));
    connect(player,SIGNAL(signalStrengthChanged(int)),this,SLOT(signalChanged(int)));

    if(player->isSupportedBand(QRadioPlayer::FM))
        player->setBand(QRadioPlayer::FM);
    else {
        qWarning()<<"Currently only works for FM";
        exit(0);
    }

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* buttonBar = new QHBoxLayout;
    QHBoxLayout* topBar = new QHBoxLayout;

    layout->addLayout(topBar);

    freq = new QLabel;
    freq->setText(QString("%1 kHz").arg(player->frequency()/1000));
    topBar->addWidget(freq);

    signal = new QLabel;
    signal->setText(tr("No Signal"));
    topBar->addWidget(signal);

    volumeSlider = new QSlider(Qt::Vertical,this);
    volumeSlider->setRange(0,100);
    qWarning()<<player->volume();
    volumeSlider->setValue(player->volume());
    connect(volumeSlider,SIGNAL(valueChanged(int)),this,SLOT(updateVolume(int)));
    topBar->addWidget(volumeSlider);

    layout->addLayout(buttonBar);

    searchLeft = new QPushButton;
    searchLeft->setText(tr("scan Down"));
    connect(searchLeft,SIGNAL(clicked()),SLOT(searchDown()));
    buttonBar->addWidget(searchLeft);

    left = new QPushButton;
    left->setText(tr("Freq Down"));
    connect(left,SIGNAL(clicked()),SLOT(freqDown()));
    buttonBar->addWidget(left);

    right = new QPushButton;
    connect(right,SIGNAL(clicked()),SLOT(freqUp()));
    right->setText(tr("Freq Up"));
    buttonBar->addWidget(right);

    searchRight = new QPushButton;
    searchRight->setText(tr("scan Up"));
    connect(searchRight,SIGNAL(clicked()),SLOT(searchUp()));
    buttonBar->addWidget(searchRight);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();
}

Radio::~Radio()
{
}

void Radio::freqUp()
{
    int f = player->frequency();
    f = f + 5000;
    player->setFrequency(f);
}

void Radio::freqDown()
{
    int f = player->frequency();
    f = f - 5000;
    player->setFrequency(f);
}

void Radio::searchUp()
{
    player->searchForward();
}

void Radio::searchDown()
{
    player->searchBackward();
}

void Radio::freqChanged(int f)
{
    freq->setText(QString("%1 kHz").arg(player->frequency()/1000));
}

void Radio::signalChanged(int s)
{
    if(player->signalStrength() > 25)
        signal->setText(tr("Got Signal"));
    else
        signal->setText(tr("No Signal"));
}

void Radio::updateVolume(int v)
{
    player->setVolume(v);
}

