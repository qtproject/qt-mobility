/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <multimedia/qmediarecorder.h>

#include <multimedia/qmediarecordercontrol.h>
#include <multimedia/qmediaobject_p.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qmediaserviceprovider.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qvideoencodercontrol.h>
#include <multimedia/qmediaformatcontrol.h>


#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qmetaobject.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/QAudioFormat>
#else
#include <multimedia/qaudioformat.h>
#endif

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief The QMediaRecorder class is used for the recording of media content.

    The QMediaRecorder class is a high level media recording class.
    It's not intended to be used alone but for accessing the media
    recording functions of other media objects, like QRadioTuner,
    QCamera or QAudioCaptureSource.

    \code
    // Audio only recording
    audioSource = new QAudioCaptureSource;
    recorder = new QMediaRecorder(audioSource);

    recorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    recorder->record();
    \endcode

    \code
    // Audio/Video recording
    camera = new QCamera(deviceName);
    recorder = new QMediaRecorder(camera);

    recorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    recorder->record();
    \endcode

    \sa
*/

class QMediaRecorderPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QMediaRecorder)

public:
    QMediaRecorderPrivate();
    void initControls();

    QMediaRecorderControl *control;
    QMediaFormatControl *formatControl;
    QAudioEncoderControl *audioControl;
    QVideoEncoderControl *videoControl;

    QMediaRecorder::State state;
    QMediaRecorder::Error error;
    QString errorString;

    void _q_stateChanged(QMediaRecorder::State state);
    void _q_error(int error, const QString &errorString);
};

QMediaRecorderPrivate::QMediaRecorderPrivate():
     control(0),
     formatControl(0),
     audioControl(0),
     videoControl(0),
     state(QMediaRecorder::StoppedState),
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

#define ENUM_NAME(c,e,v) (c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(e)).valueToKey((v)))


void QMediaRecorderPrivate::_q_stateChanged(QMediaRecorder::State ps)
{
    Q_Q(QMediaRecorder);

    if (ps == QMediaRecorder::RecordingState)
        q->addPropertyWatch("duration");
    else
        q->removePropertyWatch("duration");

//    qDebug() << "Recorder state changed:" << ENUM_NAME(QMediaRecorder,"State",ps);
    if (state != ps) {
        emit q->stateChanged(ps);
    }

    state = ps;
}


void QMediaRecorderPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaRecorder);

    this->error = QMediaRecorder::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}


/*!
    Constructs a media recorder which records the media produced by \a mediaObject.

    The \a parent is passed to QMediaObject.
*/

QMediaRecorder::QMediaRecorder(QMediaObject *mediaObject, QObject *parent):
    QMediaObject(*new QMediaRecorderPrivate, parent, mediaObject->service())
{
    Q_D(QMediaRecorder);

    d->initControls();
}

/*!
    Destroys a media object.
*/

QMediaRecorder::~QMediaRecorder()
{
}

/*!
    \property QMediaRecorder::outputLocation
    \brief the destination location of media content.
*/

QUrl QMediaRecorder::outputLocation() const
{
    return d_func()->control ? d_func()->control->outputLocation() : QUrl();
}

/*!
    Sets the output \a location and returns true if this operation was successful.
    Setting the location can fail for example when the service supports
    only local file system locations while the network url was passed,
    or the service doesn't support media recording.
*/

bool QMediaRecorder::setOutputLocation(const QUrl &location)
{
    Q_D(QMediaRecorder);
    return d->control ? d->control->setOutputLocation(location) : false;
}

/*!
    Return the current media recorder state.

    \sa QMediaRecorder::State
*/

QMediaRecorder::State QMediaRecorder::state() const
{
    return d_func()->control ? QMediaRecorder::State(d_func()->control->state()) : StoppedState;
}

/*!
    Returns the current error state.

    \sa errorString()
*/

QMediaRecorder::Error QMediaRecorder::error() const
{
    return d_func()->error;
}

/*!
    Returns a string describing the current error state.

    \sa error()
*/

QString QMediaRecorder::errorString() const
{
    return d_func()->errorString;
}

/*!
    \property QMediaRecorder::duration

    \brief the recorded media duration in milliseconds.
*/

qint64 QMediaRecorder::duration() const
{
    return d_func()->control ? d_func()->control->duration() : 0;
}


/*!
    Returns a list of MIME types of supported container formats.
*/
QStringList QMediaRecorder::supportedFormats() const
{
    return d_func()->formatControl ?
           d_func()->formatControl->supportedFormats() : QStringList();
}

/*!
    Returns a description of a container format \a mimeType.
*/
QString QMediaRecorder::formatDescription(const QString &mimeType) const
{
    return d_func()->formatControl ?
           d_func()->formatControl->formatDescription(mimeType) : QString();
}

/*!
    Returns the MIME type of the selected container format.
*/

QString QMediaRecorder::format() const
{
    return d_func()->formatControl ?
           d_func()->formatControl->format() : QString();
}

/*!
    Returns a list of supported audio codecs.
*/
QStringList QMediaRecorder::supportedAudioCodecs() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->supportedAudioCodecs() : QStringList();
}

/*!
    Returns a description of an audio \a codec.
*/
QString QMediaRecorder::audioCodecDescription(const QString &codec) const
{
    return d_func()->audioControl ?
           d_func()->audioControl->codecDescription(codec) : QString();
}

/*!
    Returns a list of supported audio sample rates.
*/

QList<int> QMediaRecorder::supportedAudioSampleRates() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->supportedSampleRates() : QList<int>();
}

/*!
    Return the minimum resolution video can be encoded at.

    \sa QVideoEncoderSettings::resolution(), maximumResolution()
*/
QSize QMediaRecorder::minimumResolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->minimumResolution() : QSize();
}

/*!
    Returns the maximum resolution video can be encoded at.

    \sa QVideoEncoderSettings::resolution(), minimumResolution()
*/
QSize QMediaRecorder::maximumResolution() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->maximumResolution() : QSize();
}

/*!
    Returns a list of resolutions video can be encoded at.  An empty list is returned if the video
    encoder supports arbitrary resolutions within the minimum and maximum range.

    \sa QVideoEncoderSettings::resolution(), minimumResolution(), maximumResolution()
*/
QList<QSize> QMediaRecorder::supportedResolutions() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedResolutions() : QList<QSize>();
}

/*!
    Returns the minimum frame rate video can encoded at.

    \sa QVideoEncoderSettings::frameRate(), maximumFrameRate()
*/
qreal QMediaRecorder::minimumFrameRate()
{
    return d_func()->videoControl ?
           d_func()->videoControl->minimumFrameRate() : 0.0;
}

/*!
    Returns the maximum frame rate video can be encoded at.

    \sa QVideoEncoderSettings::frameRate(), minimumFrameRate()
*/
qreal QMediaRecorder::maximumFrameRate()
{
    return d_func()->videoControl ?
           d_func()->videoControl->maximumFrameRate() : 0.0;
}

/*!
    Returns a list of frame rates video can be encoded at. An empty list is returned if the encoder
    supports arbitrary frame rates within the minimum and maximum range.

    \sa QVideoEncoderSettings::frameRate(), minimumFrameRate(), maximumFrameRate()
*/
QList<qreal> QMediaRecorder::supportedFrameRates() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedFrameRates() : QList<qreal>();
}


/*!
    Returns a list of supported video codecs.
*/
QStringList QMediaRecorder::supportedVideoCodecs() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->supportedVideoCodecs() : QStringList();
}

/*!
    Returns a description of a video \a codec.
*/
QString QMediaRecorder::videoCodecDescription(const QString &codec) const
{
    return d_func()->videoControl ?
           d_func()->videoControl->videoCodecDescription(codec) : QString();
}

/*!
    Returns the audio encoder settings being used.
*/

QAudioEncoderSettings QMediaRecorder::audioSettings() const
{
    return d_func()->audioControl ?
           d_func()->audioControl->audioSettings() : QAudioEncoderSettings();
}

/*!
    Returns the video encoder settings being used.
*/

QVideoEncoderSettings QMediaRecorder::videoSettings() const
{
    return d_func()->videoControl ?
           d_func()->videoControl->videoSettings() : QVideoEncoderSettings();
}

/*!
    Sets the \a audio and \a video encoder settings and container \a format MIME type.
*/

void QMediaRecorder::setEncodingSettings(const QAudioEncoderSettings &audio,
                                         const QVideoEncoderSettings &video,
                                         const QString &format)
{
    Q_D(QMediaRecorder);

    if (d->audioControl)
        d->audioControl->setAudioSettings(audio);

    if (d->videoControl)
        d->videoControl->setVideoSettings(video);

    if (d->formatControl)
        d->formatControl->setFormat(format);

    if (d->control)
        d->control->applySettings();
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

    Signals that a media recorder's \a state has changed.
*/

/*!
    \fn QMediaRecorder::durationChanged(qint64 duration)

    Signals that the \a duration of the recorded media has changed.
*/

/*!
    \fn QMediaRecorder::error(QMediaRecorder::Error error)

    Signals that an \a error has occurred.
*/


#include "moc_qmediarecorder.cpp"
