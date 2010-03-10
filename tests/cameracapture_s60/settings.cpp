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

#include "settings.h"
#ifdef Q_OS_SYMBIAN
#include "ui_settings_s60.h"
#else
#include "ui_settings.h"
#endif

#include <QtGui/qcombobox.h>
#include <QtCore/qdebug.h>
#include <qmediarecorder.h>
#include <qmediaservice.h>
#include <QFile>



Settings::Settings(QMediaRecorder *mediaRecorder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsUi),
    mediaRecorder(mediaRecorder)
{
    ui->setupUi(this);
     
    //audio codecs
    foreach(const QString &codecName, mediaRecorder->supportedAudioCodecs()) {
        QString description = mediaRecorder->audioCodecDescription(codecName);
        ui->audioCodecBox->addItem(codecName+": "+description, QVariant(codecName));
    }
    //sample rate:
    foreach(int sampleRate, mediaRecorder->supportedAudioSampleRates()) {
        ui->audioSampleRateBox->addItem(QString::number(sampleRate), QVariant(sampleRate));
    }
    ui->audioQualitySlider->setRange(0, int(QtMedia::VeryHighQuality));

    //video codecs
    foreach(const QString &codecName, mediaRecorder->supportedVideoCodecs()) {
        QString description = mediaRecorder->videoCodecDescription(codecName);
        ui->videoCodecBox->addItem(codecName+": "+description, QVariant(codecName));
    }
    ui->videoQualitySlider->setRange(0, int(QtMedia::VeryHighQuality));


    ui->videoResolutionBox->addItem(tr("Default"));
    QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions();
    foreach(const QSize &resolution, supportedResolutions) {
        ui->videoResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
                                        QVariant(resolution));
    }
    ui->videoFramerateBox->addItem(tr("Default"));
    QList<qreal> supportedFrameRates = mediaRecorder->supportedFrameRates();
    qreal rate;
    foreach(rate, supportedFrameRates) {
        QString rateString = QString("%1").arg(rate, 0, 'f', 2);
        ui->videoFramerateBox->addItem(rateString, QVariant(rate));
    }
    //containers
    foreach(const QString &format, mediaRecorder->supportedContainers()) {
        ui->containerFormatBox->addItem(format+":"+mediaRecorder->containerDescription(format),
                                        QVariant(format));
    }    
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(ui->videoCodecBox, SIGNAL(activated(const QString)), this, SLOT(updateResolutions()));
    connect(ui->videoResolutionBox, SIGNAL(activated(const QString)), this, SLOT(updateFramerates()));
}

Settings::~Settings()
{
    delete ui;
}

void Settings::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QAudioEncoderSettings Settings::audioSettings() const
{
    QAudioEncoderSettings settings = mediaRecorder->audioSettings();
    settings.setCodec(boxValue(ui->audioCodecBox).toString());
    settings.setQuality(QtMedia::EncodingQuality(ui->audioQualitySlider->value()));
    settings.setSampleRate(boxValue(ui->audioSampleRateBox).toInt());
    return settings;
}

void Settings::setAudioSettings(const QAudioEncoderSettings &audioSettings)
{
    selectComboBoxItem(ui->audioCodecBox, QVariant(audioSettings.codec()));
    selectComboBoxItem(ui->audioSampleRateBox, QVariant(audioSettings.sampleRate()));
    ui->audioQualitySlider->setValue(audioSettings.quality());
}

QVideoEncoderSettings Settings::videoSettings() const
{        
    QVideoEncoderSettings settings = mediaRecorder->videoSettings();    
    settings.setCodec(boxValue(ui->videoCodecBox).toString());
    settings.setQuality(QtMedia::EncodingQuality(ui->videoQualitySlider->value()));
    settings.setResolution(boxValue(ui->videoResolutionBox).toSize());
    settings.setFrameRate(boxValue(ui->videoFramerateBox).value<qreal>());
    return settings;    
}

void Settings::setVideoSettings(const QVideoEncoderSettings &videoSettings)
{   
    selectComboBoxItem(ui->videoCodecBox, QVariant(videoSettings.codec()));
    selectComboBoxItem(ui->videoResolutionBox, QVariant(videoSettings.resolution()));
    ui->videoQualitySlider->setValue(videoSettings.quality());
    //special case for frame rate
    for (int i=0; i<ui->videoFramerateBox->count(); i++) {
        qreal itemRate = ui->videoFramerateBox->itemData(i).value<qreal>();
        if (qFuzzyCompare(itemRate, videoSettings.frameRate())) {
            ui->videoFramerateBox->setCurrentIndex(i);
            break;
        }
    }
}

QString Settings::format() const
{
    return boxValue(ui->containerFormatBox).toString();
}

void Settings::setFormat(const QString &format)
{
    selectComboBoxItem(ui->containerFormatBox, QVariant(format));
}

QVariant Settings::boxValue(const QComboBox *box) const
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void Settings::selectComboBoxItem(QComboBox *box, const QVariant &value)
{
    for (int i=0; i<box->count(); i++) {
        if (box->itemData(i) == value) {
            box->setCurrentIndex(i);
            break;
        }
    }
}

void Settings::updateResolutions()
{
    QVideoEncoderSettings settings = mediaRecorder->videoSettings();
    settings.setCodec(boxValue(ui->videoCodecBox).toString());

    QList<QSize> supportedResolutions = mediaRecorder->supportedResolutions(settings);
    ui->videoResolutionBox->clear();
    foreach(const QSize &resolution, supportedResolutions) {
        ui->videoResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
                                        QVariant(resolution));
    }
}

void Settings::updateFramerates()
{
    QVideoEncoderSettings settings = mediaRecorder->videoSettings();
    settings.setCodec(boxValue(ui->videoCodecBox).toString());
    settings.setResolution(boxValue(ui->videoResolutionBox).toSize());

    QList<qreal> supportedFrameRates = mediaRecorder->supportedFrameRates(settings);
    ui->videoFramerateBox->clear();
    qreal rate;
    foreach(rate, supportedFrameRates) {
        QString rateString = QString("%1").arg(rate, 0, 'f', 2);
        ui->videoFramerateBox->addItem(rateString, QVariant(rate));
    }
}
