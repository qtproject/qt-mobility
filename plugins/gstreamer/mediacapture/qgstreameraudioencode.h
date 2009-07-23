#ifndef QGSTREAMERAUDIOENCODE_H
#define QGSTREAMERAUDIOENCODE_H

#include "qaudioencodecontrol.h"
class QGstreamerCaptureSession;

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

#include <gst/gst.h>

class QGstreamerAudioEncode : public QAudioEncodeControl
{
    Q_OBJECT
public:
    QGstreamerAudioEncode(QObject *parent);
    virtual ~QGstreamerAudioEncode();

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

    void applyOptions();
    GstElement *encoder();

private:
    GstBin *m_encoderBin;
    GstElement *m_encoderElement;
    GstElement *m_muxerElement;
    GstElement *m_capsfilter;
    GstElement *m_identity2;

    QMap<QString, QVariant> m_options;
    QString m_codec;
    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QStringList> m_codecOptions;
    QMap<QString,QString> m_muxers;
};

#endif
