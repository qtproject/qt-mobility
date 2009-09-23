#ifndef AUDIOENCODECONTROL_H
#define AUDIOENCODECONTROL_H

#include "qaudioencodercontrol.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <QtMultimedia/qaudioformat.h>

class AudioCaptureSession;

class AudioEncodeControl : public QAudioEncoderControl
{
    Q_OBJECT
public:
    AudioEncodeControl(QObject *parent);
    virtual ~AudioEncodeControl();

    QStringList supportedAudioCodecs() const;
    QString audioCodec() const;
    bool setAudioCodec(const QString &codecName);

    QString codecDescription(const QString &codecName) const;

    int bitrate() const;
    void setBitrate(int);

    int quality() const;
    void setQuality(int);

    QStringList supportedEncodingOptions() const;
    QVariant encodingOption(const QString &name) const;
    void setEncodingOption(const QString &name, const QVariant &value);

    int frequency() const;
    void setFrequency(int frequency);
    QList<int> supportedFrequencies() const;
    QPair<int,int> supportedFrequencyRange() const;

    int channels() const;
    void setChannels(int channels);
    QList<int> supportedChannelCounts() const;

    int sampleSize() const;
    void setSampleSize(int sampleSize);
    QList<int> supportedSampleSizes() const;

private:
    AudioCaptureSession* m_session;
};

#endif
