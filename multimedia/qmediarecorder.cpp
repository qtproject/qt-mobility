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

#include "qmediarecorder.h"

#include "qmediarecordercontrol.h"
#include "qabstractmediaobject_p.h"
#include "qaudiorecorderservice.h"
#include "qmediaserviceprovider.h"
#include "qaudioencodercontrol.h"
#include "qvideoencodercontrol.h"
#include "qmediaformatcontrol.h"
#include "qmediarecorderservice.h"

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qstringlist.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/QAudioFormat>
#else
#include "qaudioformat.h"
#endif

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

QMediaRecorderService *createMediaRecorderService()
{
    QMediaServiceProvider *provider = defaultServiceProvider("mediarecorder");

    QObject *object = provider ? provider->createObject(QAudioRecorderService_iid) : 0;

    if (object != 0) {
        QMediaRecorderService *service = qobject_cast<QMediaRecorderService*>(object);

        if (service != 0)
            return service;

        delete object;
    }

    return 0;
}


class QMediaRecorderPrivate : public QAbstractMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaRecorder)

public:
    QMediaRecorderPrivate();
    void initControls();

    QAbstractMediaService *service;
    QMediaRecorderControl *control;
    QMediaFormatControl *formatControl;
    QAudioEncoderControl *audioControl;
    QVideoEncoderControl *videoControl;

    bool ownService;

    QMediaRecorder::Error error;
    QString errorString;

    void _q_stateChanged(QMediaRecorder::State state);
    void _q_error(int error, const QString &errorString);
};

QMediaRecorderPrivate::QMediaRecorderPrivate():
     service(0),
     control(0),
     formatControl(0),
     audioControl(0),
     videoControl(0),
     ownService(false),
     error(QMediaRecorder::NoError)
{
}

void QMediaRecorderPrivate::initControls()
{
    Q_Q(QMediaRecorder);

    if (!service)
        return;

    control = qobject_cast<QMediaRecorderControl*>(service->control(QMediaRecorderControl_iid));
    formatControl = qobject_cast<QMediaFormatControl *>(service->control(QMediaFormatControl_iid));
    audioControl = qobject_cast<QAudioEncoderControl *>(service->control(QAudioEncoderControl_iid));
    videoControl = qobject_cast<QVideoEncoderControl *>(service->control(QVideoEncoderControl_iid));

    if (control) {
        q->connect(control, SIGNAL(stateChanged(QMediaRecorder::State)),
                q, SLOT(_q_stateChanged(QMediaRecorder::State)));

        q->connect(control, SIGNAL(error(int,QString)),
                q, SLOT(_q_error(int,QString)));
    }
}

void QMediaRecorderPrivate::_q_stateChanged(QMediaRecorder::State ps)
{
    Q_Q(QMediaRecorder);

    if (ps == QMediaRecorder::RecordingState)
        q->addPropertyWatch("duration");
    else
        q->removePropertyWatch("duration");

    emit q->stateChanged(ps);
}


void QMediaRecorderPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaRecorder);

    this->error = QMediaRecorder::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}


/*!
    Construct a media recorder object with \a mediaObject and \a parent.
*/

QMediaRecorder::QMediaRecorder(QAbstractMediaObject *mediaObject):
    QAbstractMediaObject(*new QMediaRecorderPrivate, mediaObject)
{
    Q_D(QMediaRecorder);

    d->service = mediaObject->service();
    Q_ASSERT(d->service != 0);
    d->ownService = false;

    d->initControls();
}

/*!
    Construct a media recorder object with \a service and \a parent.
*/

QMediaRecorder::QMediaRecorder(QObject *parent, QMediaRecorderService *service):
    QAbstractMediaObject(*new QMediaRecorderPrivate, parent)
{
    Q_D(QMediaRecorder);

    if (service) {
        d->service = service;
        d->ownService = false;
    } else {
        d->service = createMediaRecorderService();
        d->ownService = true;
    }

    Q_ASSERT(d->service != 0);

    d->initControls();
}


/*!
    Destruct the media recorder object.
*/

QMediaRecorder::~QMediaRecorder()
{
    Q_D(QMediaRecorder);

    if (d->ownService)
        delete d->service;
}

/*!
    Returns true if a valid recording device is available.
*/

bool QMediaRecorder::isValid() const
{
    return d_func()->control != 0;
}

/*!
    Returns the media service being used.
*/

QAbstractMediaService* QMediaRecorder::service() const
{
    return d_func()->service;
}

/*!
    Returns the sink being used.
*/

QUrl QMediaRecorder::sink() const
{
    return d_func()->control ? d_func()->control->sink() : QUrl();
}

/*!
    Returns true if set of sink being used to \a sink is successful.
*/

bool QMediaRecorder::setSink(const QUrl &sink)
{
    Q_D(QMediaRecorder);
    return d->control ? d->control->setSink(sink) : false;
}

/*!
    Return the current state.
*/

QMediaRecorder::State QMediaRecorder::state() const
{
    return d_func()->control ? QMediaRecorder::State(d_func()->control->state()) : StoppedState;
}

/*!
    Return the current error state.
*/

QMediaRecorder::Error QMediaRecorder::error() const
{
    return d_func()->error;
}

/*!
    Return the current error string.
*/

QString QMediaRecorder::errorString() const
{
    return d_func()->errorString;
}

/*!
    \property QMediaRecorder::duration
    \brief Recorded media duration in milliseconds.
*/

/*!
    Return the current duration in milliseconds.
*/

qint64 QMediaRecorder::duration() const
{
    return d_func()->control ? d_func()->control->duration() : 0;
}


/*!
  Returns the list of supported container formats.
*/
QStringList QMediaRecorder::supportedFormats() const
{
    return d_func()->formatControl ?
           d_func()->formatControl->supportedFormats() : QStringList();
}

/*!
  Returns the description of container \a format.
*/
QString QMediaRecorder::formatDescription(const QString &format) const
{
    return d_func()->formatControl ?
           d_func()->formatControl->formatDescription(format) : QString();
}

/*!
  \property QMediaRecorder::format
  \brief Conteiner format mime type.
*/

QString QMediaRecorder::format() const
{
    return d_func()->formatControl ?
           d_func()->formatControl->format() : QString();
}

void QMediaRecorder::setFormat(const QString &formatMimeType)
{
    if (d_func()->formatControl)
        d_func()->formatControl->setFormat(formatMimeType);
}

/*!
  \property QMediaRecorder::audioFormat
  \brief Audio format.
*/


QAudioFormat QMediaRecorder::audioFormat() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->format() :  QAudioFormat();
}

/*!
  Returns true if audio \a format is supported.
*/
bool QMediaRecorder::isAudioFormatSupported(const QAudioFormat &format) const
{
    return d_func()->audioControl ?
           d_func()->audioControl->isFormatSupported(format) : false;
}

bool QMediaRecorder::setAudioFormat(const QAudioFormat &format)
{
    return d_func()->audioControl ?
           d_func()->audioControl->setFormat(format) : false;
}

/*!
  Returns the list of supported audio codecs.
*/
QStringList QMediaRecorder::supportedAudioCodecs() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->supportedAudioCodecs() : QStringList();
}

/*!
  Returns description of audio \a codec.
*/
QString QMediaRecorder::audioCodecDescription(const QString &codec) const
{
    return d_func()->audioControl ?
           d_func()->audioControl->codecDescription(codec) : QString();
}

/*!
  \property QMediaRecorder::audioCodec
  \brief Audio codec name.
*/

QString QMediaRecorder::audioCodec() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->audioCodec() : QString();
}

bool QMediaRecorder::setAudioCodec(const QString &codecName)
{
    return d_func()->audioControl ?
           d_func()->audioControl->setAudioCodec(codecName) : false;
}

/*!
  \property QMediaRecorder::audioBitrate
  \brief Bitrate of encoded audio stream.
*/

int QMediaRecorder::audioBitrate() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->bitrate() : -1;
}

void QMediaRecorder::setAudioBitrate(int bitrate)
{
    if (d_func()->audioControl)
        d_func()->audioControl->setBitrate(bitrate);
}

/*!
  \property QMediaRecorder::audioQuality
  \brief Quality of encoded audio stream.

  The quality value is normalized in range from 0 to 100,
  with 0 means the lowest quality the audio codec supports,
  100 - the best quality the codec supports.

  The default quality value is 50 and usually means
  some reasonable quality settings.

  Depending on codec, the quality property may affect
  different parameters, like bitrate or presets.
*/
qreal QMediaRecorder::audioQuality() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->quality() : -1.0;
}

void QMediaRecorder::setAudioQuality(qreal quality)
{
    if (d_func()->audioControl)
        d_func()->audioControl->setQuality(quality);
}

/*!
  Returns the list of codec specific audio encoding options.
*/
QStringList QMediaRecorder::supportedAudioEncodingOptions() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->supportedEncodingOptions() : QStringList();
}

/*!
  Returns value of audio encoding \a option.
*/
QVariant QMediaRecorder::audioEncodingOption(const QString &option) const
{
    return d_func()->audioControl ?
           d_func()->audioControl->encodingOption(option) : QVariant();
}

/*!
  Set the codec specific \a option to \a value.
*/
void QMediaRecorder::setAudioEncodingOption(const QString &option, const QVariant &value)
{
    if (d_func()->audioControl)
        d_func()->audioControl->setEncodingOption(option, value);
}

/*!
  \property QMediaRecorder::resolution
  \brief Resolution of encoded video stream.

  \sa minimumResolution(), maximumResolution(), supportedResolutions()
*/

QSize QMediaRecorder::resolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->resolution() : QSize();
}

void QMediaRecorder::setResolution(const QSize &resolution)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setResolution(resolution);
}

/*!
  Returns the smallest resolution, video encoder supports.

  \sa resolution
*/
QSize QMediaRecorder::minimumResolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->minimumResolution() : QSize();
}

/*!
  Returns the largest resolution, video encoder supports.

  \sa resolution
*/
QSize QMediaRecorder::maximumResolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->maximumResolution() : QSize();
}

/*!
  Returns the list of resolutions if the video encoder supports only the limited set
  of video frame sizes, otherwise returns an empty list.

  \sa resolution(), minimumResolution(), maximumResolution()
*/
QList<QSize> QMediaRecorder::supportedResolutions() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedResolutions() : QList<QSize>();
}


/*!
  \property QMediaRecorder::frameRate
  \brief Frame rate of video stream.
*/

QMediaRecorder::FrameRate QMediaRecorder::frameRate() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->frameRate() : qMakePair<int,int>(-1,-1);
}

void QMediaRecorder::setFrameRate(const QMediaRecorder::FrameRate &rate)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setFrameRate(rate);
}

/*!
  Returns the smallest frame rate, video encoder supports.

  \sa frameRate
*/
QMediaRecorder::FrameRate QMediaRecorder::minimumFrameRate()
{
    return d_func()->videoControl ?
           d_func()->videoControl->minimumFrameRate() : qMakePair<int,int>(-1,-1);
}

/*!
  Returns the largest frame rate, video encoder supports.

  \sa frameRate
*/
QMediaRecorder::FrameRate QMediaRecorder::maximumFrameRate()
{
    return d_func()->videoControl ?
           d_func()->videoControl->maximumFrameRate() : qMakePair<int,int>(-1,-1);
}

/*!
  Returns the list of frame rates if the video encoder supports only the limited set
  of video frame rates, otherwise returns an empty list.

  \sa frameRate
*/
QList< QMediaRecorder::FrameRate > QMediaRecorder::supportedFrameRates() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedFrameRates() : QList<QMediaRecorder::FrameRate>();
}


/*!
  Returns the list of supported video codecs.
*/
QStringList QMediaRecorder::supportedVideoCodecs() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedVideoCodecs() : QStringList();
}

/*!
  Returns description of video \a codec.
*/
QString QMediaRecorder::videoCodecDescription(const QString &codec) const
{
    return d_func()->videoControl ?
           d_func()->videoControl->videoCodecDescription(codec) : QString();
}

/*!
  \property QMediaRecorder::videoCodec
  \brief Video codec name.
*/
QString QMediaRecorder::videoCodec() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->videoCodec() : QString();
}

bool QMediaRecorder::setVideoCodec(const QString &codecName)
{
    return d_func()->videoControl ?
           d_func()->videoControl->setVideoCodec(codecName) : false;
}

/*!
  \property QMediaRecorder::videoBitrate
  \brief Video bitrate.
*/
int QMediaRecorder::videoBitrate() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->bitrate() : -1;
}

void QMediaRecorder::setVideoBitrate(int bitrate)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setBitrate(bitrate);
}

/*!
  \property QMediaRecorder::videoQuality
  \brief Video quality value.

  Quality of encoded video stream.

  The quality value is normalized in range from 0 to 100,
  with 0 means the lowest quality the audio codec supports,
  100 - the best quality the codec supports.

  The default quality value is 50 and usually means
  some reasonable quality settings.

  Depending on codec, the quality property may affect
  different parameters, like bitrate or presets.
*/
qreal QMediaRecorder::videoQuality() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->quality() : -1.0;
}

void QMediaRecorder::setVideoQuality(qreal quality)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setQuality(quality);
}

/*!
  Returns the list of codec specific video encoding options.
*/
QStringList QMediaRecorder::supportedVideoEncodingOptions() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedEncodingOptions() : QStringList();
}
/*!
  Returns value of audio encoding \a option.
*/
QVariant QMediaRecorder::videoEncodingOption(const QString &option) const
{
    return d_func()->videoControl ?
           d_func()->videoControl->encodingOption(option) : QVariant();
}

/*!
  Set the codec specific \a option to \a value.
*/
void QMediaRecorder::setVideoEncodingOption(const QString &option, const QVariant &value)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setEncodingOption(option, value);
}


/*!
    Start recording.
*/

void QMediaRecorder::record()
{
    Q_D(QMediaRecorder);

    // reset error
    d->error = NoError;
    d->errorString = QString();

    if (d->control)
        d->control->record();
}

/*!
    Pause recording.
*/

void QMediaRecorder::pause()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->pause();
}

/*!
    Stop recording.
*/

void QMediaRecorder::stop()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->stop();
}

/*!
    \enum QMediaRecorder::State

    \value StoppedState    The recorder is not active.
    \value RecordingState  The recorder is currently active and producing data.
    \value PausedState     The recorder is paused.
*/

/*!
    \enum QMediaRecorder::Error

    \value NoError         No Errors.
    \value ResourceError   Device is not ready or not available.
    \value FormatError     Current format is not supported.
*/

/*!
    \fn QMediaRecorder::stateChanged(State state)

    Signal emitted when \a state changed.
*/

/*!
    \fn QMediaRecorder::durationChanged(qint64 duration)

    Signal emitted when \a duration changed.
*/

/*!
    \fn QMediaRecorder::error(QMediaRecorder::Error error)

    Signal emitted when \a error changed.
*/

/*!
    \fn QMediaRecorder::errorStringChanged(const QString &error)

    Signal emitted when \a error changed.
*/

/*!
    \fn formatChanged(const QString &format);

    Signal emitted when container \a format changed.
*/

/*!
    \fn audioFormatChanged(const QAudioFormat &format);

    Signal emitted when audio \a format changed.
*/

/*!
    \fn audioCodecChanged(const QString &codecName);

    Signal emitted when audio \a codec changed.
*/

/*!
    \fn audioBitrateChanged(int bitrate);

    Signal emitted when audio \a bitrate changed.
*/

/*!
    \fn audioQualityChanged(qreal quality);

    Signal emitted when audio \a quality changed.
*/

/*!
    \fn resolutionChanged(const QSize &resolution);

    Signal emitted when video \a resolution changed.
*/

/*!
    \fn frameRateChanged(const QMediaRecorder::FrameRate &rate);

    Signal emitted when frame \a rate changed.
*/

/*!
    \fn videoCodecChanged(const QString &codec);

    Signal emitted when video \a codec changed.
*/

/*!
    \fn videoBitrateChanged(int bitrate);

    Signal emitted when video \a bitrate changed.
*/

/*!
    \fn videoQualityChanged(qreal quality);

    Signal emitted when video \a quality value is changed.
*/



#include "moc_qmediarecorder.cpp"
