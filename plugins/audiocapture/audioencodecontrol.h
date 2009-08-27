#ifndef AUDIOENCODECONTROL_H
#define AUDIOENCODECONTROL_H

#include "qaudioencodecontrol.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <QtMultimedia/qaudioformat.h>

class AudioCaptureSession;

class AudioEncodeControl : public QAudioEncodeControl
{
    Q_OBJECT
public:
    AudioEncodeControl(QObject *parent);
    virtual ~AudioEncodeControl();

    QAudioFormat format() const;
    bool isFormatSupported(const QAudioFormat &format) const;
    bool setFormat(const QAudioFormat &format);

    QStringList supportedAudioCodecs() const;
    QString codecDescription(const QString &codecName);
    bool setAudioCodec(const QString &codecName);
    QString audioCodec() const;

    int bitrate() const;
    void setBitrate(int);

    qreal quality() const;
    void setQuality(qreal);

    QStringList supportedEncodingOptions();
    QVariant encodingOption(const QString &name);
    void setEncodingOption(const QString &name, const QVariant &value);

private:
    AudioCaptureSession* m_session;
};

#endif
