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
#include <qaudioencodercontrol.h>

#include <QtMultimedia/qaudioformat.h>

#include "audiorecorder.h"

AudioRecorder::AudioRecorder()
{
    audiosource = new QAudioCaptureSource;
    capture = new QMediaRecorder(audiosource);

    if (capture->supportedAudioCodecs().size() > 0) {
        QAudioEncoderSettings audioSettings;
        audioSettings.setQuality(QtMediaServices::LowQuality);
        audioSettings.setEncodingMode(QtMediaServices::ConstantQualityEncoding);
        audioSettings.setCodec(capture->supportedAudioCodecs().first());
        capture->setEncodingSettings(audioSettings,QVideoEncoderSettings(),
                capture->supportedContainers().first());
    }

    // set a default file
#ifdef Q_OS_SYMBIAN
    capture->setOutputLocation(recordPathAudio(QUrl()));    
#else
    capture->setOutputLocation(QUrl("test.raw"));
#endif

    QWidget *window = new QWidget;
    window->setMinimumSize(320,240);
    window->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    QGridLayout* layout = new QGridLayout;

    QLabel* deviceLabel = new QLabel;
    deviceLabel->setText(tr("Audio Device"));
    deviceBox = new QComboBox(this);
    deviceBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    deviceBox->setMinimumSize(200,10);

    QLabel* containerLabel = new QLabel;
    containerLabel->setText(tr("File Container"));
    containersBox = new QComboBox(this);
    containersBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    containersBox->setMinimumSize(200,10);

    QLabel* codecLabel = new QLabel;
    codecLabel->setText(tr("Audio Codec"));
    codecsBox = new QComboBox(this);
    codecsBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    codecsBox->setMinimumSize(200,10);    
    
    QLabel* sampleRateLabel = new QLabel;
    sampleRateLabel->setText(tr("Sample Rate"));
    sampleRateBox = new QComboBox(this);
    sampleRateBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    sampleRateBox->setMinimumSize(200,10);
    
    QLabel* channelLabel = new QLabel;
    channelLabel->setText(tr("Channel count"));
    channelBox = new QComboBox(this);
    channelBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    channelBox->setMinimumSize(200,10);
    
    QLabel* qualityLabel = new QLabel;
    qualityLabel->setText(tr("Audio Quality"));
    qualityBox = new QComboBox(this);
    qualityBox->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    qualityBox->setMinimumSize(200,10);

    QList<QString> inputs = audiosource->audioInputs();
    for(int i = 0; i < inputs.size(); i++)
        deviceBox->addItem(inputs.at(i));

    QStringList codecs = capture->supportedAudioCodecs();
    for(int i = 0; i < codecs.count(); i++)
        codecsBox->addItem(codecs.at(i));

    QStringList containers = capture->supportedContainers();
    for(int i = 0; i < containers.count(); i++)
        containersBox->addItem(containers.at(i));

    QList<int> samplerates = capture->supportedAudioSampleRates();    
    for(int i = 0; i < samplerates.count(); i++) {        
        QString rateString = QString("%1").arg(samplerates.at(i));
        sampleRateBox->addItem(rateString, QVariant(samplerates.at(i))); 
    }
    
    QList<int> channels;
    channels <<1<<2;     
    for(int i = 0; i < channels.count(); i++) {        
        QString channelString = QString("%1").arg(channels.at(i));
        channelBox->addItem(channelString, QVariant(channels.at(i))); 
    }
      
    qualityBox->addItem(tr("Low"));
    qualityBox->addItem(tr("Medium"));
    qualityBox->addItem(tr("High"));

    connect(capture, SIGNAL(durationChanged(qint64)), this, SLOT(updateProgress(qint64)));
    connect(capture, SIGNAL(stateChanged(QMediaRecorder::State)), this, SLOT(stateChanged(QMediaRecorder::State)));
    connect(capture, SIGNAL(error(QMediaRecorder::Error)), this, SLOT(errorChanged(QMediaRecorder::Error)));

    layout->addWidget(deviceLabel,0,0,Qt::AlignHCenter);
    connect(deviceBox,SIGNAL(activated(int)),SLOT(deviceChanged(int)));
    layout->addWidget(deviceBox,0,1,1,3,Qt::AlignLeft);

    layout->addWidget(containerLabel,1,0,Qt::AlignHCenter);
    connect(containersBox,SIGNAL(activated(int)),SLOT(containerChanged(int)));
    layout->addWidget(containersBox,1,1,1,3,Qt::AlignLeft);

    layout->addWidget(codecLabel,2,0,Qt::AlignHCenter);
    connect(codecsBox,SIGNAL(activated(int)),SLOT(codecChanged(int)));
    layout->addWidget(codecsBox,2,1,1,3,Qt::AlignLeft);   
    
    layout->addWidget(sampleRateLabel,3,0,Qt::AlignHCenter);
    connect(sampleRateBox,SIGNAL(activated(int)),SLOT(sampleRateChanged(int)));
    layout->addWidget(sampleRateBox,3,1,1,3,Qt::AlignLeft);
    
    layout->addWidget(channelLabel,4,0,Qt::AlignHCenter);
    connect(channelBox,SIGNAL(activated(int)),SLOT(channelCountChanged(int)));
    layout->addWidget(channelBox,4,1,1,3,Qt::AlignLeft);
    
    layout->addWidget(qualityLabel,5,0,Qt::AlignHCenter);
    connect(qualityBox,SIGNAL(activated(int)),SLOT(qualityChanged(int)));
    layout->addWidget(qualityBox,5,1,1,3,Qt::AlignLeft);

    fileButton = new QPushButton(this);
    fileButton->setText(tr("Output File"));    
    connect(fileButton,SIGNAL(clicked()),SLOT(selectOutputFile()));
    layout->addWidget(fileButton,6,0,Qt::AlignHCenter);
    
    pauseButton = new QPushButton(this);    
    pauseButton->setText(tr("Pause"));
    connect(pauseButton,SIGNAL(clicked()),SLOT(togglePause()));
    layout->addWidget(pauseButton,6,1,Qt::AlignHCenter);

    button = new QPushButton(this);
    button->setText(tr("Record"));
    connect(button,SIGNAL(clicked()),SLOT(toggleRecord()));
    layout->addWidget(button,6,2,Qt::AlignHCenter);

    statusLabel = new QLabel;
    statusLabel->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    statusLabel->setMinimumSize(130,10);
    statusLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusLabel->setLineWidth(1);
    layout->addWidget(statusLabel,7,0,Qt::AlignHCenter);
    
    QLabel* durationLabel = new QLabel;
    durationLabel->setText(tr("Duration"));
    layout->addWidget(durationLabel,7,1,Qt::AlignRight);

    recTime = new QLabel;
    layout->addWidget(recTime,7,2,Qt::AlignLeft);
    
    window->setLayout(layout);
    setCentralWidget(window);
    window->show();

    active = false;
    paused = false;
}

QUrl AudioRecorder::recordPathAudio(QUrl filePath)
{   
    if (!filePath.isEmpty())
        return filePath;  
    
    QDir outputDir(QDir::rootPath());
    
    int lastImage = 0;  
    int fileCount = 0;
    foreach(QString fileName, outputDir.entryList(QStringList() << "testclip_*")) {        
        int imgNumber = fileName.mid(5, fileName.size()-9).toInt();
        lastImage = qMax(lastImage, imgNumber);
        if (outputDir.exists(fileName))             
            fileCount+=1;        
    }    
    lastImage+=fileCount;    
    QUrl location(QDir::toNativeSeparators(outputDir.canonicalPath()+QString("/testclip_%1").arg(lastImage+1,4,10,QLatin1Char('0'))));
    return location;
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
    QString text = QString("%1").arg(currentTime/1000);
    recTime->setText(text);
}

void AudioRecorder::stateChanged(QMediaRecorder::State state)
{
    if (capture->error() != QMediaRecorder::NoError)
        return;

    switch(state) {
        case QMediaRecorder::RecordingState: {
            statusLabel->setText(tr("Recording"));
            button->setText(tr("Stop"));
            break;
        }
        case QMediaRecorder::PausedState: {
            statusLabel->setText(tr("Paused"));
            button->setText(tr("Record"));
            break;
        }
        default: {
            statusLabel->setText(tr("Stopped"));
            button->setText(tr("Record"));
        }
    }
}

void AudioRecorder::deviceChanged(int idx)
{
    audiosource->setAudioInput(deviceBox->itemText(idx));
}

void AudioRecorder::containerChanged(int idx)
{
    QAudioEncoderSettings settings = capture->audioSettings();
    capture->setEncodingSettings(capture->audioSettings(), QVideoEncoderSettings(), containersBox->itemText(idx));
}

void AudioRecorder::codecChanged(int idx)
{
    updateSamplerates(idx);
    updateChannelCount(idx);
    updateQuality(idx);
    QAudioEncoderSettings settings = capture->audioSettings();
    settings.setCodec(codecsBox->itemText(idx));
    capture->setEncodingSettings(settings);
}

void AudioRecorder::sampleRateChanged(int idx)
{    
    QAudioEncoderSettings settings = capture->audioSettings();
    settings.setSampleRate((sampleRateBox->itemData(idx).toInt()));
    capture->setEncodingSettings(settings);
}

void AudioRecorder::channelCountChanged(int idx)
{    
    QAudioEncoderSettings settings = capture->audioSettings();
    settings.setChannelCount((channelBox->itemData(idx).toInt()));
    capture->setEncodingSettings(settings);
}

void AudioRecorder::qualityChanged(int idx)
{ 
    QAudioEncoderSettings settings = capture->audioSettings();    
    
    switch(idx) {
    case 0:
        settings.setQuality(QtMediaServices::LowQuality);
        break;
    case 1:
        settings.setQuality(QtMediaServices::NormalQuality);
        break;
    default:
        settings.setQuality(QtMediaServices::HighQuality);
    }
    capture->setEncodingSettings(settings);
}

void AudioRecorder::toggleRecord()
{
    if(!active) {
        if(!paused) {
            recTime->setText("0");
            currentTime = 0;
        }
#ifdef Q_OS_SYMBIAN    
    if (!paused)    
        capture->setOutputLocation(recordPathAudio(destination));
#endif
        capture->record();        
        active = true;
        paused = false;
    } else {
        capture->stop();        
        active = false;
    }
}

void AudioRecorder::togglePause()
{
    if(active && !paused) {       
        capture->pause();        
        active = false;
        paused = true;
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
#ifdef Q_OS_SYMBIAN
        destination = QUrl(fileNames.first());
#else    
        capture->setOutputLocation(QUrl(fileNames.first()));
        
#endif    
}

void AudioRecorder::errorChanged(QMediaRecorder::Error err)
{
    Q_UNUSED(err)

    statusLabel->setText(capture->errorString());
}

void AudioRecorder::updateSamplerates(int idx)
{
    QAudioEncoderSettings settings;
    settings.setCodec(codecsBox->itemText(idx));
    
    QList<int> supportedSampleRates = capture->supportedAudioSampleRates(settings);
    sampleRateBox->clear();    
    for(int i = 0; i < supportedSampleRates.count(); i++) {        
        QString rateString = QString("%1").arg(supportedSampleRates.at(i));
        sampleRateBox->addItem(rateString, QVariant(supportedSampleRates.at(i))); 
    }    
}

void AudioRecorder::updateChannelCount(int idx)
{    
    QAudioEncoderControl *audioEncoder = qobject_cast<QAudioEncoderControl*>(capture->service()->control(QAudioEncoderControl_iid));
    channelBox->clear();
    QStringList list = audioEncoder->supportedEncodingOptions(codecsBox->itemText(idx));
    QList<int> channels;
    if (list.contains("channels"))
        channels <<1<<2;
    else
        channels <<1;
    for(int i = 0; i < channels.count(); i++) {        
        QString channelString = QString("%1").arg(channels.at(i));
        channelBox->addItem(channelString, QVariant(channels.at(i))); 
    }        
}

void AudioRecorder::updateQuality(int idx)
{    
    QAudioEncoderControl *audioEncoder = qobject_cast<QAudioEncoderControl*>(capture->service()->control(QAudioEncoderControl_iid));
    qualityBox->clear();
    QStringList list = audioEncoder->supportedEncodingOptions(codecsBox->itemText(idx));
    QList<int> channels;
    if (list.contains("quality")) {
        qualityBox->addItem(tr("Low"));
        qualityBox->addItem(tr("Medium"));
        qualityBox->addItem(tr("High"));
    }else {
        qualityBox->addItem(tr("Low"));
    }           
}
