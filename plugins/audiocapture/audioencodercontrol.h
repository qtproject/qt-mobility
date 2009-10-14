#ifndef AUDIOENCODERCONTROL_H
#define AUDIOENCODERCONTROL_H

#include <multimedia/qaudioencodercontrol.h>

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <QtMultimedia/qaudioformat.h>

class AudioCaptureSession;

class AudioEncoderControl : public QAudioEncoderControl
{
    Q_OBJECT
public:
    AudioEncoderControl(QObject *parent);
    virtual ~AudioEncoderControl();

    QStringList supportedAudioCodecs() const;
    QString audioCodec() const;
    bool setAudioCodec(const QString &codecName);

    QString codecDescription(const QString &codecName) const;

    int bitrate() const;
    void setBitrate(int);

    QtMedia::EncodingQuality quality() const;
    void setQuality(QtMedia::EncodingQuality);

    QStringList supportedEncodingOptions(const QString &codec) const;
    QVariant encodingOption(const QString &codec, const QString &name) const;
    void setEncodingOption(const QString &codec, const QString &name, const QVariant &value);

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
