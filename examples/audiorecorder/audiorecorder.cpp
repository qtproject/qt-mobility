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

#include <qmediarecorder.h>
#include <qabstractmediaservice.h>

#include <qaudiosource.h>

#include <QtGui>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/qaudioformat.h>
#else
#include <qaudioformat.h>
#endif

#include "audiorecorder.h"

AudioRecorder::AudioRecorder()
{
    audiosource = new QAudioSource;
    capture = new QMediaRecorder(audiosource);

    // set a default file
    capture->setSink(QUrl("test.raw"));
    // setup default freq,channel combinations available
    formats << qMakePair<int,int>(8000,1);
    formats << qMakePair<int,int>(11025,1);
    formats << qMakePair<int,int>(22050,1);
    formats << qMakePair<int,int>(44100,1);

    QWidget *window = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;

    deviceBox = new QComboBox(this);
    paramsBox = new QComboBox(this);

    if(capture->service()) {

        QList<QString> audioInputs;
        audioInputs = capture->service()->supportedEndpoints(QAbstractMediaService::AudioInput);
        if(audioInputs.size() > 0) {
            qWarning()<<"FOUND audioInputs: "<<audioInputs;
            capture->service()->setActiveEndpoint(QAbstractMediaService::AudioInput,audioInputs.first());
            for(int i = 0; i < audioInputs.size(); ++i) {
                deviceBox->addItem(audioInputs.at(i));
            }
        } else {
            qWarning()<<"no audio input available";
        }
        for(int i = 0; i < formats.size(); ++i) {
            paramsBox->addItem(QString("f=%1,ch=%2").arg(formats.at(i).first).arg(formats.at(i).second));
        }
        connect(capture, SIGNAL(durationChanged(qint64)), this, SLOT(updateProgress(qint64)));
        connect(capture, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(stateChanged(QMediaRecorder::State)));
    }

    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox);

    connect(paramsBox,SIGNAL(activated(int)),SLOT(paramsChanged(int)));
    layout->addWidget(paramsBox);

    fileButton = new QPushButton(this);
    fileButton->setText(tr("Select Output File"));
    connect(fileButton,SIGNAL(clicked()),SLOT(selectOutputFile()));
    layout->addWidget(fileButton);

    button = new QPushButton(this);
    button->setText(tr("Click to start recording"));
    connect(button,SIGNAL(clicked()),SLOT(togglePlay()));
    layout->addWidget(button);

    recTime = new QLabel;
    recTime->setText("0 sec");
    layout->addWidget(recTime);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    active = false;
}

AudioRecorder::~AudioRecorder()
{
}

void AudioRecorder::updateProgress(qint64 pos)
{
    currentTime = pos;
    if(currentTime == 0) currentTime = 1;
    QString text = QString("%1 secs").arg(currentTime/1000);
    recTime->setText(text);
}

void AudioRecorder::stateChanged(QMediaRecorder::State state)
{
    qWarning()<<"stateChanged() "<<state;
}

void AudioRecorder::deviceChanged(int idx)
{
    capture->service()->setActiveEndpoint(QAbstractMediaService::AudioInput,deviceBox->itemText(idx));
}

void AudioRecorder::paramsChanged(int idx)
{
    QStringList parts;
    QStringList values;

    parts = paramsBox->itemText(idx).split(",");
    if(parts.size() != 2) return;

    QAudioFormat fmt = audiosource->format();
    // freq
    values = parts.at(0).split("=");
    fmt.setFrequency(values.at(1).toInt());
    // channels
    values = parts.at(1).split("=");
    fmt.setChannels(values.at(1).toInt());
}

void AudioRecorder::togglePlay()
{
    if(!active) {
        recTime->setText("0 sec");
        currentTime = 0;
        capture->record();

        button->setText(tr("Click to stop"));
        active = true;
    } else {
        capture->stop();
        button->setText(tr("Click to start recording"));
        active = false;
    }
}

void AudioRecorder::selectOutputFile()
{
    QStringList fileNames;

    QFileDialog dialog(this);

    dialog.setFileMode(QFileDialog::AnyFile);
    if (dialog.exec())
        fileNames = dialog.selectedFiles();

    if(fileNames.size() > 0)
        capture->setSink(QUrl(fileNames.first()));
}
