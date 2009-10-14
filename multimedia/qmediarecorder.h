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

#include <multimedia/qmediaobject.h>
#include <multimedia/qmediaserviceprovider.h>

#include <QtCore/qpair.h>


class QUrl;
class QAudioFormat;
class QSize;
class QMediaRecorderService;

class QMediaRecorderPrivate;
class Q_MEDIA_EXPORT QMediaRecorder : public QMediaObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_ENUMS(Error)
    Q_ENUMS(EncodingQuality)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(QString audioCodec READ audioCodec WRITE setAudioCodec NOTIFY audioCodecChanged)
    Q_PROPERTY(int audioBitrate READ audioBitrate WRITE setAudioBitrate NOTIFY audioBitrateChanged)
    Q_PROPERTY(QMediaRecorder::EncodingQuality audioQuality READ audioQuality WRITE setAudioQuality NOTIFY audioQualityChanged)
    Q_PROPERTY(QSize resolution READ resolution WRITE setResolution NOTIFY resolutionChanged)
    Q_PROPERTY(QMediaRecorder::FrameRate frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged)
    Q_PROPERTY(QString videoCodec READ videoCodec WRITE setVideoCodec NOTIFY videoCodecChanged)
    Q_PROPERTY(int videoBitrate READ videoBitrate WRITE setVideoBitrate NOTIFY videoBitrateChanged)
    Q_PROPERTY(QMediaRecorder::EncodingQuality videoQuality READ videoQuality WRITE setVideoQuality NOTIFY videoQualityChanged)

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

    enum EncodingQuality
    {
        VeryLowQuality = 0,
        LowQuality,
        NormalQuality,
        HighQuality,
        VeryHighQuality
    };

    QMediaRecorder(QMediaObject *mediaObject, QObject *parent = 0);
    ~QMediaRecorder();

    QUrl sink() const;
    bool setSink(const QUrl &sink);

    State state() const;

    Error error() const;
    QString errorString() const;

    qint64 duration() const;

    QStringList supportedFormats() const;
    QString formatDescription(const QString &formatMimeType) const;
    QString format() const;


    QStringList supportedAudioCodecs() const;
    QString audioCodecDescription(const QString &codecName) const;
    QString audioCodec() const;

    int audioBitrate() const;
    EncodingQuality audioQuality() const;

    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;
    QString videoCodec() const;

    QSize resolution() const;
    QSize minimumResolution() const;
    QSize maximumResolution() const;
    QList<QSize> supportedResolutions() const;

    FrameRate frameRate() const;
    FrameRate minimumFrameRate();
    FrameRate maximumFrameRate();
    QList<FrameRate> supportedFrameRates() const;

    int videoBitrate() const;
    EncodingQuality videoQuality() const;

public slots:
    void record();
    void pause();
    void stop();

    void setFormat(const QString &formatMimeType);

    bool setAudioCodec(const QString &codecName);
    void setAudioBitrate(int bitrate);
    void setAudioQuality(EncodingQuality quality);

    void setResolution(const QSize &);
    void setFrameRate(const QMediaRecorder::FrameRate &rate);
    bool setVideoCodec(const QString &codecName);
    void setVideoBitrate(int bitrate);
    void setVideoQuality(EncodingQuality quality);

Q_SIGNALS:
    void stateChanged(QMediaRecorder::State state);
    void durationChanged(qint64 duration);

    void error(QMediaRecorder::Error error);

private:
    Q_DISABLE_COPY(QMediaRecorder)
    Q_DECLARE_PRIVATE(QMediaRecorder)
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QMediaRecorder::State))
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &));
};

//Q_DECLARE_METATYPE(QMediaRecorder::FrameRate);

#endif  // QMEDIARECORDER_H
