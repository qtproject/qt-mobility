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

#include <QtGui>

#include <qaudiocapturesource.h>
#include <qmediarecorder.h>
#include <qmediaservice.h>

#include <QtMultimedia/qaudioformat.h>

#include "audiorecorder.h"

AudioRecorder::AudioRecorder()
{
    audiosource = new QAudioCaptureSource;
    capture = new QMediaRecorder(audiosource);

    // set a default file
#ifdef Q_OS_SYMBIAN
    capture->setOutputLocation(QUrl("c:\\data\\test.wav"));
#else
    capture->setOutputLocation(QUrl("test.raw"));
#endif

    QWidget *window = new QWidget;
    QGridLayout* layout = new QGridLayout;

    QLabel* deviceLabel = new QLabel;
    deviceLabel->setText("Devices");
    deviceBox = new QComboBox(this);
    deviceBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QLabel* codecLabel = new QLabel;
    codecLabel->setText("Codecs");
    codecsBox = new QComboBox(this);
    codecsBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QLabel* qualityLabel = new QLabel;
    qualityLabel->setText("Quality");
    qualityBox = new QComboBox(this);
    qualityBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QList<QString> inputs = audiosource->audioInputs();
    for(int i = 0; i < inputs.size(); i++)
        deviceBox->addItem(inputs.at(i));

    QStringList codecs = capture->supportedAudioCodecs();
    for(int i = 0; i < codecs.count(); i++)
        codecsBox->addItem(codecs.at(i));

    qualityBox->addItem("Low");
    qualityBox->addItem("Medium");
    qualityBox->addItem("High");

    connect(capture, SIGNAL(durationChanged(qint64)), this, SLOT(updateProgress(qint64)));
    connect(capture, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(stateChanged(QMediaRecorder::State)));

    layout->addWidget(deviceLabel,0,0,Qt::AlignHCenter);
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox,0,1,1,3,Qt::AlignLeft);

    layout->addWidget(codecLabel,1,0,Qt::AlignHCenter);
    connect(codecsBox,SIGNAL(activated(int)),SLOT(codecChanged(int)));
    layout->addWidget(codecsBox,1,1,Qt::AlignLeft);

    layout->addWidget(qualityLabel,2,0,Qt::AlignHCenter);
    connect(qualityBox,SIGNAL(activated(int)),SLOT(qualityChanged(int)));
    layout->addWidget(qualityBox,2,1,Qt::AlignLeft);

    fileButton = new QPushButton(this);
    fileButton->setText(tr("Output File"));
    connect(fileButton,SIGNAL(clicked()),SLOT(selectOutputFile()));
    layout->addWidget(fileButton,3,0,Qt::AlignHCenter);

    button = new QPushButton(this);
    button->setText(tr("Record"));
    connect(button,SIGNAL(clicked()),SLOT(toggleRecord()));
    layout->addWidget(button,3,1,Qt::AlignHCenter);

    recTime = new QLabel;
    recTime->setText("0 sec");
    layout->addWidget(recTime,4,0,Qt::AlignHCenter);

    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    active = false;
}

AudioRecorder::~AudioRecorder()
{
    delete capture;
    delete audiosource;
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
    audiosource->setAudioInput(deviceBox->itemText(idx));
}

void AudioRecorder::codecChanged(int idx)
{
    //Q_UNUSED(idx);
    QAudioEncoderSettings audioSettings = capture->audioSettings();
    audioSettings.setCodec(codecsBox->itemText(idx));
    //audioSettings.setQuality(QtMedia::HighQuality);

    capture->setEncodingSettings(audioSettings);
}

void AudioRecorder::qualityChanged(int idx)
{
    //Q_UNUSED(idx);
    
    QAudioEncoderSettings audioSettings = capture->audioSettings();

    if(qualityBox->itemText(idx).compare("Low") == 0) {
		// 8000Hz mono is 8kbps
		audioSettings.setBitRate(8);
		audioSettings.setSampleRate(8000);
		audioSettings.setChannelCount(1);
	} else if(qualityBox->itemText(idx).compare("Medium") == 0) {
		// 22050Hz mono is 44.1kbps
		audioSettings.setBitRate(44);
		audioSettings.setSampleRate(22050);
		audioSettings.setChannelCount(1);
	} else if(qualityBox->itemText(idx).compare("High") == 0) {
		// 44100Hz mono is 88.2kbps
		audioSettings.setBitRate(88);
		audioSettings.setSampleRate(44100);
		audioSettings.setChannelCount(1);
	}
    capture->setEncodingSettings(audioSettings);
}

void AudioRecorder::toggleRecord()
{
    if(!active) {
        recTime->setText("0 sec");
        currentTime = 0;
        capture->record();

        button->setText(tr("Stop"));
        active = true;
    } else {
        capture->stop();
        button->setText(tr("Record"));
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
        capture->setOutputLocation(QUrl(fileNames.first()));
}
