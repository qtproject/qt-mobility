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

#include "stillsettings.h"
#ifdef Q_OS_SYMBIAN
#include "ui_stillsettings_s60.h"
#else
//#include "ui_stillsettings.h"
#endif

#include <QtGui/qcombobox.h>
#include <QtCore/qdebug.h>
#include <qimageencodercontrol.h>
#include <experimental/qstillimagecapture.h>
#include <qmediaencodersettings.h>
#include <qmediaservice.h>
#include <QFile>



StillSettings::StillSettings(QStillImageCapture *imageCapture, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StillSettingsUi),
    imageCapture(imageCapture)
{
    ui->setupUi(this);
     
    //image resolutions
    QList<QSize> supportedResolutions = imageCapture->supportedResolutions();
    foreach(const QSize &resolution, supportedResolutions) {
        ui->imageResolutionBox->addItem(QString("%1x%2").arg(resolution.width()).arg(resolution.height()),
                                        QVariant(resolution));
    }

    //image codecs
    foreach(const QString &codecName, imageCapture->supportedImageCodecs()) {
        QString description = imageCapture->imageCodecDescription(codecName);
        ui->imageCodecBox->addItem(codecName+": "+description, QVariant(codecName));
    }
    ui->imageQualitySlider->setRange(0, int(QtMedia::VeryHighQuality));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

StillSettings::~StillSettings()
{
    delete ui;
}

void StillSettings::changeEvent(QEvent *e)
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

QImageEncoderSettings StillSettings::imageSettings() const
{
    QImageEncoderSettings settings = imageCapture->encodingSettings();
    settings.setResolution(boxValue(ui->imageResolutionBox).toSize());
    settings.setCodec(boxValue(ui->imageCodecBox).toString());
    settings.setQuality(QtMedia::EncodingQuality(ui->imageQualitySlider->value()));
    return settings;
}

void StillSettings::setImageSettings(const QImageEncoderSettings &imageSettings)
{
    selectComboBoxItem(ui->imageResolutionBox, QVariant(imageSettings.resolution()));
    selectComboBoxItem(ui->imageCodecBox, QVariant(imageSettings.codec()));
    ui->imageQualitySlider->setValue(imageSettings.quality());
}

QVariant StillSettings::boxValue(const QComboBox *box) const
{
    int idx = box->currentIndex();
    if (idx == -1)
        return QVariant();

    return box->itemData(idx);
}

void StillSettings::selectComboBoxItem(QComboBox *box, const QVariant &value)
{
    for (int i=0; i<box->count(); i++) {
        if (box->itemData(i) == value) {
            box->setCurrentIndex(i);
            break;
        }
    }
}
