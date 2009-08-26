#ifndef QGSTREAMERVIDEOENCODE_H
#define QGSTREAMERVIDEOENCODE_H

#include "qvideoencodecontrol.h"
class QGstreamerCaptureSession;

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <gst/gst.h>

class QGstreamerVideoEncode : public QVideoEncodeControl
{
    Q_OBJECT
public:
    QGstreamerVideoEncode(QObject *parent);
    virtual ~QGstreamerVideoEncode();

    QSize resolution() const;
    QSize minimumResolution() const;
    QSize maximumResolution() const;
    QList<QSize> supportedResolutions() const;
    void setResolution(const QSize &);

    QPair<int,int> frameRate() const;
    QPair<int,int> minumumFrameRate() const;
    QPair<int,int> maximumFrameRate() const;
    QList< QPair<int,int> > supportedFrameRates() const;
    void setFrameRate(const QPair<int,int> &rate);

    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;

    QString videoCodec() const;
    bool setVideoCodec(const QString &codecName);
    QString audioCodec() const;

    int bitrate() const;
    void setBitrate(int);

    qreal quality() const;
    void setQuality(qreal);

    QStringList supportedEncodingOptions() const;
    QVariant encodingOption(const QString &name) const;
    void setEncodingOption(const QString &name, const QVariant &value);

    GstElement *createEncoder();

private:
    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QByteArray> m_elementNames;
    QMap<QString,QStringList> m_codecOptions;

    QString m_codec;
    QMap<QString, QVariant> m_options;
    QSize m_resolution;
    QPair<int,int> m_frameRate;
};

#endif
