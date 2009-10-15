#include "settings.h"
#include "ui_settings.h"

#include <QtGui/qcombobox.h>
#include <QtCore/qdebug.h>
#include <multimedia/qmediarecorder.h>
#include <multimedia/qmediaservice.h>

#include <QVideoSurfaceFormat>

Settings::Settings(QMediaRecorder *mediaRecorder, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
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
    QList<QtMedia::FrameRate> supportedFrameRates = mediaRecorder->supportedFrameRates();
    QtMedia::FrameRate rate;
    foreach(rate, supportedFrameRates) {
        QString rateString = rate.second == 1 ? QString::number(rate.first) : QString("%1/%2").arg(rate.first).arg(rate.second);
        ui->videoFramerateBox->addItem(rateString, QVariant::fromValue<QtMedia::FrameRate>(rate));
    }

    //containers
    foreach(const QString &format, mediaRecorder->supportedFormats()) {
        ui->containerFormatBox->addItem(format+":"+mediaRecorder->formatDescription(format),
                                        QVariant(format));
    }
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
    settings.setFrameRate(boxValue(ui->videoFramerateBox).value<QtMedia::FrameRate>());

    return settings;
}

void Settings::setVideoSettings(const QVideoEncoderSettings &videoSettings)
{
    selectComboBoxItem(ui->videoCodecBox, QVariant(videoSettings.codec()));
    selectComboBoxItem(ui->videoResolutionBox, QVariant(videoSettings.resolution()));
    ui->videoQualitySlider->setValue(videoSettings.quality());

    //special case for frame rate
    for (int i=0; i<ui->videoFramerateBox->count(); i++) {
        QtMedia::FrameRate itemRate = ui->videoFramerateBox->itemData(i).value<QtMedia::FrameRate>();
        if ( itemRate == videoSettings.frameRate()) {
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
