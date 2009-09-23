#ifndef V4LVIDEOENCODE_H
#define V4LVIDEOENCODE_H

#include "qvideoencodecontrol.h"
class V4LCameraSession;

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

class V4LVideoEncode : public QVideoEncodeControl
{
    Q_OBJECT
public:
    V4LVideoEncode(QObject *parent);
    virtual ~V4LVideoEncode();

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

    qreal quality() const;
    void setQuality(qreal);

    QStringList supportedEncodingOptions();
    QVariant encodingOption(const QString &name);
    void setEncodingOption(const QString &name, const QVariant &value);

private:
    V4LCameraSession* m_session;

    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QStringList> m_codecOptions;

    QString m_codec;
    QMap<QString, QVariant> m_options;
    QSize m_resolution;
    QMediaRecorder::FrameRate m_frameRate;
};

#endif
