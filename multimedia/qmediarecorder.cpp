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
#include "qaudioencodecontrol.h"
#include "qvideoencodecontrol.h"
#include "qmediaformatcontrol.h"

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qstringlist.h>

#include <QtMultimedia/QAudioFormat>


Q_MEDIA_EXPORT QAbstractMediaService *createMediaCaptureService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject(QAudioRecorderService_iid) : 0;

    if (object != 0) {
        QAbstractMediaService *service = qobject_cast<QAbstractMediaService*>(object);

        if (service != 0)
            return service;

        delete object;
    }

    return 0;
}

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaRecorderPrivate : public QAbstractMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaRecorder)

public:
    QMediaRecorderPrivate(QAbstractMediaService *service);

    QAbstractMediaService *service;
    QMediaRecorderControl *control;
    QMediaFormatControl *formatControl;
    QAudioEncodeControl *audioControl;
    QVideoEncodeControl *videoControl;

    QMediaRecorder::Error error;
    QString errorString;

    void _q_stateChanged(QMediaRecorder::State state);
    void _q_error(int error, const QString &errorString);
};

QMediaRecorderPrivate::QMediaRecorderPrivate(QAbstractMediaService *service)
    :QAbstractMediaObjectPrivate(),
     service(service),
     error(QMediaRecorder::NoError)
{
    if(service) {
        control = qobject_cast<QMediaRecorderControl *>(service->control(QMediaRecorderControl_iid));
        formatControl = qobject_cast<QMediaFormatControl *>(service->control(QMediaFormatControl_iid));
        audioControl = qobject_cast<QAudioEncodeControl *>(service->control(QAudioEncodeControl_iid));
        videoControl = qobject_cast<QVideoEncodeControl *>(service->control(QVideoEncodeControl_iid));
    } else {
        control = 0;
        formatControl = 0;
        audioControl = 0;
        videoControl = 0;
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
    Construct a media recorder object with \a parent.

    The QMediaRecorder will use a default recording service.
*/

QMediaRecorder::QMediaRecorder(QObject *parent):
    QAbstractMediaObject(*new QMediaRecorderPrivate(0), parent)
{
    Q_D(QMediaRecorder);

    d->service = createMediaCaptureService();
    d->control = qobject_cast<QMediaRecorderControl*>(d->service->control(QMediaRecorderControl_iid));

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(QMediaRecorder::State)), SLOT(_q_stateChanged(QMediaRecorder::State)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

/*!
    Construct a media recorder object with \a mediaObject and \a parent.
*/

QMediaRecorder::QMediaRecorder(QAbstractMediaObject *mediaObject, QObject *parent):
    QAbstractMediaObject(*new QMediaRecorderPrivate(mediaObject->service()), parent)
{
    Q_D(QMediaRecorder);

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(QMediaRecorder::State)), SLOT(_q_stateChanged(QMediaRecorder::State)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}

/*!
    Construct a media recorder object with \a service and \a parent.
*/

QMediaRecorder::QMediaRecorder(QAbstractMediaService *service, QObject *parent):
    QAbstractMediaObject(*new QMediaRecorderPrivate(service), parent)
{
    Q_D(QMediaRecorder);

    if (d->control) {
        connect(d->control, SIGNAL(stateChanged(QMediaRecorder::State)), SLOT(_q_stateChanged(QMediaRecorder::State)));
        connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));
    }
}


/*!
    Destruct the media recorder object.
*/

QMediaRecorder::~QMediaRecorder()
{
    stop();
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
*/
QStringList QMediaRecorder::supportedFormats() const
{
    return d_func()->formatControl ?
           d_func()->formatControl->supportedFormats() : QStringList();
}

/*!
*/
QString QMediaRecorder::formatDescription(const QString &formatMimeType) const
{
    return d_func()->formatControl ?
           d_func()->formatControl->formatDescription(formatMimeType) : QString();
}

/*!
*/
QString QMediaRecorder::format() const
{
    return d_func()->formatControl ?
           d_func()->formatControl->format() : QString();
}

/*!
*/
void QMediaRecorder::setFormat(const QString &formatMimeType)
{
    if (d_func()->formatControl)
        d_func()->formatControl->setFormat(formatMimeType);
}

/*!
*/
QAudioFormat QMediaRecorder::audioFormat() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->format() :  QAudioFormat();
}

/*!
*/
bool QMediaRecorder::isAudioFormatSupported(const QAudioFormat &format) const
{
    return d_func()->audioControl ?
           d_func()->audioControl->isFormatSupported(format) : false;
}

/*!
*/
bool QMediaRecorder::setAudioFormat(const QAudioFormat &format)
{
    return d_func()->audioControl ?
           d_func()->audioControl->setFormat(format) : false;
}

/*!
*/
QStringList QMediaRecorder::supportedAudioCodecs() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->supportedAudioCodecs() : QStringList();
}

/*!
*/
QString QMediaRecorder::codecDescription(const QString &codecName) const
{
    return d_func()->audioControl ?
           d_func()->audioControl->codecDescription(codecName) : QString();
}

/*!
*/
QString QMediaRecorder::audioCodec() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->audioCodec() : QString();
}

/*!
*/
bool QMediaRecorder::setAudioCodec(const QString &codecName)
{
    return d_func()->audioControl ?
           d_func()->audioControl->setAudioCodec(codecName) : false;
}

/*!
*/
int QMediaRecorder::audioBitrate() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->bitrate() : -1;
}

/*!
*/
void QMediaRecorder::setAudioBitrate(int bitrate)
{
    if (d_func()->audioControl)
        d_func()->audioControl->setBitrate(bitrate);
}

/*!
*/
qreal QMediaRecorder::audioQuality() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->quality() : -1.0;
}

/*!
*/
void QMediaRecorder::setAudioQuality(qreal quality)
{
    if (d_func()->audioControl)
        d_func()->audioControl->setQuality(quality);
}

/*!
*/
QStringList QMediaRecorder::supportedAudioEncodingOptions() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->supportedEncodingOptions() : QStringList();
}

/*!
*/
QVariant QMediaRecorder::audioEncodingOption(const QString &name) const
{
    return d_func()->audioControl ?
           d_func()->audioControl->encodingOption(name) : QVariant();
}

/*!
*/
void QMediaRecorder::setAudioEncodingOption(const QString &name, const QVariant &value)
{
    if (d_func()->audioControl)
        d_func()->audioControl->setEncodingOption(name, value);
}

/*!
*/
QSize QMediaRecorder::resolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->resolution() : QSize();
}

/*!
*/
QSize QMediaRecorder::minimumResolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->minimumResolution() : QSize();
}

/*!
*/
QSize QMediaRecorder::maximumResolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->maximumResolution() : QSize();
}

/*!
*/
QList<QSize> QMediaRecorder::supportedResolutions() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedResolutions() : QList<QSize>();
}

/*!
*/
void QMediaRecorder::setResolution(const QSize &resolution)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setResolution(resolution);
}

/*!
*/
QPair<int,int> QMediaRecorder::frameRate() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->frameRate() : qMakePair<int,int>(-1,-1);
}

/*!
*/
QPair<int,int> QMediaRecorder::minimumFrameRate()
{
    return d_func()->videoControl ?
           d_func()->videoControl->minimumFrameRate() : qMakePair<int,int>(-1,-1);
}

/*!
*/
QPair<int,int> QMediaRecorder::maximumFrameRate()
{
    return d_func()->videoControl ?
           d_func()->videoControl->maximumFrameRate() : qMakePair<int,int>(-1,-1);
}

/*!
*/
QList< QPair<int,int> > QMediaRecorder::supportedFrameRates() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedFrameRates() : QList< QPair<int,int> >();
}

/*!
*/
void QMediaRecorder::setFrameRate(const QPair<int,int> &rate)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setFrameRate(rate);
}

/*!
*/
QStringList QMediaRecorder::supportedVideoCodecs() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedVideoCodecs() : QStringList();
}

/*!
*/
QString QMediaRecorder::videoCodecDescription(const QString &codecName) const
{
    return d_func()->videoControl ?
           d_func()->videoControl->videoCodecDescription(codecName) : QString();
}

/*!
*/
QString QMediaRecorder::videoCodec() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->videoCodec() : QString();
}

/*!
*/
bool QMediaRecorder::setVideoCodec(const QString &codecName)
{
    return d_func()->videoControl ?
           d_func()->videoControl->setVideoCodec(codecName) : false;
}

/*!
*/
int QMediaRecorder::videoBitrate() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->bitrate() : -1;
}

/*!
*/
void QMediaRecorder::setVideoBitrate(int bitrate)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setBitrate(bitrate);
}

/*!
*/
qreal QMediaRecorder::videoQuality() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->quality() : -1.0;
}
/*!
*/
void QMediaRecorder::setVideoQuality(qreal quality)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setQuality(quality);
}
/*!
*/
QStringList QMediaRecorder::supportedVideoEncodingOptions() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedEncodingOptions() : QStringList();
}
/*!
*/
QVariant QMediaRecorder::videoEncodingOption(const QString &name) const
{
    return d_func()->videoControl ?
           d_func()->videoControl->encodingOption(name) : QVariant();
}

/*!
*/
void QMediaRecorder::setVideoEncodingOption(const QString &name, const QVariant &value)
{
    if (d_func()->videoControl)
        d_func()->videoControl->setEncodingOption(name, value);
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

#include "moc_qmediarecorder.cpp"
