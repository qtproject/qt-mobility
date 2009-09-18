#ifndef S60VIDEOENCODE_H
#define S60VIDEOENCODE_H

#include "qvideoencodercontrol.h"
class S60CameraSession;

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

class S60VideoEncoder : public QVideoEncoderControl
{
    Q_OBJECT
public:
    S60VideoEncoder(QObject *parent);
    virtual ~S60VideoEncoder();

    QSize resolution() const;
    QSize minimumResolution() const;
    QSize maximumResolution() const;
    void setResolution(const QSize &);

    QMediaRecorder::FrameRate frameRate() const;
    QMediaRecorder::FrameRate minimumFrameRate() const;
    QMediaRecorder::FrameRate maximumFrameRate() const;
    QList< QMediaRecorder::FrameRate > supportedFrameRates() const;
    void setFrameRate(const QMediaRecorder::FrameRate &rate);

    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;

    QString videoCodec() const;
    bool setVideoCodec(const QString &codecName);
    QString audioCodec() const;

    int bitrate() const;
    void setBitrate(int);

    int quality() const;
    void setQuality(int);

    QStringList supportedEncodingOptions();
    QVariant encodingOption(const QString &name);
    void setEncodingOption(const QString &name, const QVariant &value);

private:
    S60CameraSession* m_session;

    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QStringList> m_codecOptions;

    QString m_codec;
    QMap<QString, QVariant> m_options;
    QSize m_resolution;
    QMediaRecorder::FrameRate m_frameRate;
};

#endif
