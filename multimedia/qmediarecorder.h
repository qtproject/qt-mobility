/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIARECORDER_H
#define QMEDIARECORDER_H

#include "qabstractmediaobject.h"
#include "qmediaserviceprovider.h"

#include <QtCore/qpair.h>


class QUrl;
class QAudioFormat;
class QSize;
class QMediaRecorderService;

class QMediaRecorderPrivate;
class Q_MEDIA_EXPORT QMediaRecorder : public QAbstractMediaObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(QAudioFormat audioFormat READ audioFormat WRITE setAudioFormat NOTIFY audioFormatChanged)
    Q_PROPERTY(QString audioCodec READ audioCodec WRITE setAudioCodec NOTIFY audioCodecChanged)
    Q_PROPERTY(int audioBitrate READ audioBitrate WRITE setAudioBitrate NOTIFY audioBitrateChanged)
    Q_PROPERTY(qreal audioQuality READ audioQuality WRITE setAudioQuality NOTIFY audioQualityChanged)
    Q_PROPERTY(QSize resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(QMediaRecorder::FrameRate frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged)
    Q_PROPERTY(QString videoCodec READ videoCodec WRITE setVideoCodec NOTIFY videoCodecChanged)
    Q_PROPERTY(int videoBitrate READ videoBitrate WRITE setVideoBitrate NOTIFY videoBitrateChanged)
    Q_PROPERTY(qreal videoQuality READ videoQuality WRITE setVideoQuality NOTIFY videoQualityChanged)

public:
    typedef QPair<int, int> FrameRate;

    enum State
    {
        StoppedState,
        RecordingState,
        PausedState
    };

    enum Error
    {
        NoError,
        ResourceError,
        FormatError
    };

    QMediaRecorder(QAbstractMediaObject *mediaObject);
    QMediaRecorder(QObject *parent = 0, QMediaRecorderService *service = 0);
    ~QMediaRecorder();

    bool isValid() const;

    QUrl sink() const;
    bool setSink(const QUrl &sink);

    State state() const;

    Error error() const;
    QString errorString() const;

    qint64 duration() const;

    QAbstractMediaService* service() const;

    QStringList supportedFormats() const;
    QString formatDescription(const QString &formatMimeType) const;
    QString format() const;

    QAudioFormat audioFormat() const;
    bool isAudioFormatSupported(const QAudioFormat &format) const;

    QStringList supportedAudioCodecs() const;
    QString audioCodecDescription(const QString &codecName) const;
    QString audioCodec() const;

    int audioBitrate() const;
    qreal audioQuality() const;

    QStringList supportedAudioEncodingOptions() const;
    QVariant audioEncodingOption(const QString &name) const;
    void setAudioEncodingOption(const QString &name, const QVariant &value);

    QSize resolution() const;
    QSize minimumResolution() const;
    QSize maximumResolution() const;
    QList<QSize> supportedResolutions() const;

    FrameRate frameRate() const;
    FrameRate minimumFrameRate();
    FrameRate maximumFrameRate();
    QList<FrameRate> supportedFrameRates() const;

    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;
    QString videoCodec() const;

    int videoBitrate() const;
    qreal videoQuality() const;

    QStringList supportedVideoEncodingOptions() const;
    QVariant videoEncodingOption(const QString &name) const;
    void setVideoEncodingOption(const QString &name, const QVariant &value);

public slots:
    void record();
    void pause();
    void stop();

    void setFormat(const QString &formatMimeType);

    bool setAudioFormat(const QAudioFormat &format);
    bool setAudioCodec(const QString &codecName);
    void setAudioBitrate(int bitrate);
    void setAudioQuality(qreal quality);

    void setResolution(const QSize &);
    void setFrameRate(const QMediaRecorder::FrameRate &rate);
    bool setVideoCodec(const QString &codecName);
    void setVideoBitrate(int bitrate);
    void setVideoQuality(qreal);

signals:
    void stateChanged(QMediaRecorder::State state);
    void durationChanged(qint64 duration);

    void formatChanged(const QString &formatMimeType);

    void audioFormatChanged(const QAudioFormat &format);
    void audioCodecChanged(const QString &codecName);
    void audioBitrateChanged(int bitrate);
    void audioQualityChanged(qreal quality);

    void resolutionChanged(const QSize &);
    void frameRateChanged(const QMediaRecorder::FrameRate &rate);
    void videoCodecChanged(const QString &codecName);
    void videoBitrateChanged(int bitrate);
    void videoQualityChanged(qreal);

    void error(QMediaRecorder::Error error);

private:
    Q_DISABLE_COPY(QMediaRecorder)
    Q_DECLARE_PRIVATE(QMediaRecorder)
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QMediaRecorder::State))
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &));
};

//Q_DECLARE_METATYPE(QMediaRecorder::FrameRate);

#endif  // QMEDIARECORDER_H
