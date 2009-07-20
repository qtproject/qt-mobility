#ifndef QGSTREAMERCAPTUREPROPERTIES_H
#define QGSTREAMERCAPTUREPROPERTIES_H

#include "qaudiocapturepropertiescontrol.h"
class QGstreamerCaptureSession;

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>

class QGstreamerCaptureProperties : public QAudioCapturePropertiesControl
{
    Q_OBJECT
public:
    QGstreamerCaptureProperties(QGstreamerCaptureSession *session, QObject *parent);
    virtual ~QGstreamerCaptureProperties();

    QAudioFormat format() const;
    bool isFormatSupported(const QAudioFormat &format) const;
    bool setFormat(const QAudioFormat &format);

    QStringList supportedAudioCodecs() const;
    QString codecDescription(const QString &codecName);
    bool setAudioCodec(const QString &codecName);

    int bitrate() const;
    void setBitrate(int);

    qreal quality() const;
    void setQuality(qreal);

    QStringList supportedEncodingOptions();
    QVariant encodingOption(const QString &name);
    void setEncodingOption(const QString &name, const QVariant &value);

private:
    QGstreamerCaptureSession *m_session;
    QMap<QString, QVariant> m_options;
    QString m_codec;
    QStringList m_codecs;
    QMap<QString,QString> m_codecDescriptions;
    QMap<QString,QStringList> m_codecOptions;
};

#endif
