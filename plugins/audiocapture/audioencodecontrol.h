#ifndef AUDIOENCODECONTROL_H
#define AUDIOENCODECONTROL_H

#include "qaudioencodecontrol.h"
class AudioCaptureSession;

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <QtMultimedia/qaudioformat.h>

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
    QAudioFormat m_format;
    QMap<QString, QVariant> m_options;
    QString m_codec;
    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QStringList> m_codecOptions;
};

#endif
