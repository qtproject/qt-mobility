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
#include <qcameraimagecapture.h>
#include <qcameraimagecapturecontrol.h>
#include <qmediaencodersettings.h>

#include <qimageencodercontrol.h>
#include <qmediaobject_p.h>
#include <qmediaservice.h>
#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCameraImageCapture
    \inmodule QtMultimediaKit
    \ingroup multimedia

    \preliminary
    \brief The QCameraImageCapture class is used for the recording of media content.

    The QCameraImageCapture class is a high level images recording class.
    It's not intended to be used alone but for accessing the media
    recording functions of other media objects, like QCamera.

    \code
        camera = new QCamera;
        viewFinder = new QCameraViewfinder;
        camera->setViewfinder(viewFinder);
        viewFinder->show();

        imageCapture = new QCameraImageCapture(camera);

        camera->setCaptureMode(QCamera::CaptureStillImage);
        camera->start();

        //on half pressed shutter button
        camera->searchAndLock();

        //on shutter button pressed
        imageCapture->capture();

        //on shutter button released
        camera->unlock();
    \endcode

    \sa QCamera
*/

namespace
{
class MediaRecorderRegisterMetaTypes
{
public:
    MediaRecorderRegisterMetaTypes()
    {
        qRegisterMetaType<QCameraImageCapture::Error>("QCameraImageCapture::Error");
    }
} _registerRecorderMetaTypes;
}


class QCameraImageCapturePrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QCameraImageCapture)
public:
    QCameraImageCapturePrivate();

    QMediaObject *mediaObject;

    QCameraImageCaptureControl *control;
    QImageEncoderControl *encoderControl;

    QCameraImageCapture::Error error;
    QString errorString;

    void _q_error(int error, const QString &errorString);
    void _q_readyChanged(bool);

    void unsetError() { error = QCameraImageCapture::NoError; errorString.clear(); }

    QCameraImageCapture *q_ptr;
};

QCameraImageCapturePrivate::QCameraImageCapturePrivate():
     mediaObject(0),
     control(0),
     encoderControl(0),
     error(QCameraImageCapture::NoError)
{
}

void QCameraImageCapturePrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QCameraImageCapture);

    this->error = QCameraImageCapture::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}

void QCameraImageCapturePrivate::_q_readyChanged(bool ready)
{
    Q_Q(QCameraImageCapture);
    emit q->readyForCaptureChanged(ready);
}


/*!
    Constructs a media recorder which records the media produced by \a mediaObject.

    The \a parent is passed to QMediaObject.
*/

QCameraImageCapture::QCameraImageCapture(QMediaObject *mediaObject, QObject *parent):
    QObject(parent), d_ptr(new QCameraImageCapturePrivate)
{
    Q_D(QCameraImageCapture);

    d->q_ptr = this;

    if (mediaObject)
        mediaObject->bind(this);
}

/*!
    Destroys images capture object.
*/

QCameraImageCapture::~QCameraImageCapture()
{
    Q_D(QCameraImageCapture);

    if (d->mediaObject)
        d->mediaObject->unbind(this);
}

/*!
  \reimp
*/
QMediaObject *QCameraImageCapture::mediaObject() const
{
    return d_func()->mediaObject;
}

/*!
  \reimp
*/
bool QCameraImageCapture::setMediaObject(QMediaObject *mediaObject)
{
    Q_D(QCameraImageCapture);

    if (d->mediaObject) {
        if (d->control) {
            disconnect(d->control, SIGNAL(imageExposed(int)),
                       this, SIGNAL(imageExposed(int)));
            disconnect(d->control, SIGNAL(imageCaptured(int,QImage)),
                       this, SIGNAL(imageCaptured(int,QImage)));
            disconnect(d->control, SIGNAL(imageSaved(int,QString)),
                       this, SIGNAL(imageSaved(int,QString)));
            disconnect(d->control, SIGNAL(readyForCaptureChanged(bool)),
                       this, SLOT(_q_readyChanged(bool)));
            disconnect(d->control, SIGNAL(error(int,QString)),
                       this, SLOT(_q_error(int,QString)));

            QMediaService *service = d->mediaObject->service();
            service->releaseControl(d->control);
            if (d->encoderControl)
                service->releaseControl(d->encoderControl);
        }
    }

    d->mediaObject = mediaObject;

    if (d->mediaObject) {
        QMediaService *service = mediaObject->service();
        if (service) {
            d->control = qobject_cast<QCameraImageCaptureControl*>(service->requestControl(QCameraImageCaptureControl_iid));
            d->encoderControl = qobject_cast<QImageEncoderControl *>(service->requestControl(QImageEncoderControl_iid));

            if (d->control) {
                connect(d->control, SIGNAL(imageExposed(int)),
                        this, SIGNAL(imageExposed(int)));
                connect(d->control, SIGNAL(imageCaptured(int,QImage)),
                        this, SIGNAL(imageCaptured(int,QImage)));
                connect(d->control, SIGNAL(imageSaved(int, QString)),
                        this, SIGNAL(imageSaved(int, QString)));
                connect(d->control, SIGNAL(readyForCaptureChanged(bool)),
                        this, SLOT(_q_readyChanged(bool)));
                connect(d->control, SIGNAL(error(int,QString)),
                        this, SLOT(_q_error(int,QString)));

                return true;
            }
        }
    }

    // without QCameraImageCaptureControl discard the media object
    d->mediaObject = 0;
    d->control = 0;
    d->encoderControl = 0;

    return false;
}

/*!
    Returns true if the images capture service ready to use.
*/
bool QCameraImageCapture::isAvailable() const
{
    if (d_func()->control != NULL)
        return true;
    else
        return false;
}

/*!
    Returns the availability error code.
*/
QtMultimediaKit::AvailabilityError QCameraImageCapture::availabilityError() const
{
    if (d_func()->control != NULL)
        return QtMultimediaKit::NoError;
    else
        return QtMultimediaKit::ServiceMissingError;
}

/*!
    Returns the current error state.

    \sa errorString()
*/

QCameraImageCapture::Error QCameraImageCapture::error() const
{
    return d_func()->error;
}

/*!
    Returns a string describing the current error state.

    \sa error()
*/

QString QCameraImageCapture::errorString() const
{
    return d_func()->errorString;
}


/*!
    Returns a list of supported image codecs.
*/
QStringList QCameraImageCapture::supportedImageCodecs() const
{
    return d_func()->encoderControl ?
           d_func()->encoderControl->supportedImageCodecs() : QStringList();
}

/*!
    Returns a description of an image \a codec.
*/
QString QCameraImageCapture::imageCodecDescription(const QString &codec) const
{
    return d_func()->encoderControl ?
           d_func()->encoderControl->imageCodecDescription(codec) : QString();
}

/*!
    Returns a list of resolutions images can be encoded at.

    If non null image \a settings parameter is passed,
    the returned list is reduced to resolution supported with partial settings like image codec or quality applied.

    If the encoder supports arbitrary resolutions within the supported range,
    *\a continuous is set to true, otherwise *\a continuous is set to false.

    \sa QImageEncoderSettings::resolution()
*/
QList<QSize> QCameraImageCapture::supportedResolutions(const QImageEncoderSettings &settings, bool *continuous) const
{
    if (continuous)
        *continuous = false;

    return d_func()->encoderControl ?
           d_func()->encoderControl->supportedResolutions(settings, continuous) : QList<QSize>();
}

/*!
    Returns the image encoder settings being used.

    \sa setEncodingSettings()
*/

QImageEncoderSettings QCameraImageCapture::encodingSettings() const
{
    return d_func()->encoderControl ?
           d_func()->encoderControl->imageSettings() : QImageEncoderSettings();
}

/*!
    Sets the image encodeing \a settings.

    If some parameters are not specified, or null settings are passed,
    the encoder choose the default encoding parameters.

    \sa encodingSettings()
*/

void QCameraImageCapture::setEncodingSettings(const QImageEncoderSettings &settings)
{
    Q_D(QCameraImageCapture);

    if (d->encoderControl)
        d->encoderControl->setImageSettings(settings);
}

/*!
  \property QCameraImageCapture::readyForCapture
   Indicates the service is ready to capture a an image immediately.
*/

bool QCameraImageCapture::isReadyForCapture() const
{
    if (d_func()->control)
        return d_func()->control->isReadyForCapture();
    else
        return false;
}

/*!
    \fn QCameraImageCapture::readyForCaptureChanged(bool ready)

    Signals that a camera's \a ready for capture state has changed.
*/


/*!
    Capture the image and save it to \a file.
    This operation is asynchronous in majority of cases,
    followed by signals QCameraImageCapture::imageCaptured(), QCameraImageCapture::imageSaved()
    or QCameraImageCapture::error().

    If an empty \a file is passed, the camera backend choses
    the default location and naming scheme for photos on the system,
    if only file name without full path is specified, the image will be saved to
    the default directory, with a full path reported with imageCaptured() and imageSaved() signals.

    QCameraImageCapture::capture returns the capture Id parameter, used with
    imageExposed(), imageCaptured() and imageSaved() signals.
*/
int QCameraImageCapture::capture(const QString &file)
{
    Q_D(QCameraImageCapture);

    d->unsetError();

    if (d->control) {
        return d->control->capture(file);
    } else {
        d->error = NotSupportedFeatureError;
        d->errorString = tr("Device does not support images capture.");

        emit error(d->error);
    }

    return -1;
}

/*!
    Cancel incomplete capture requests.
    Already captured and queused for proicessing images may be discarded.
*/
void QCameraImageCapture::cancelCapture()
{
    Q_D(QCameraImageCapture);

    d->unsetError();

    if (d->control) {
        d->control->cancelCapture();
    } else {
        d->error = NotSupportedFeatureError;
        d->errorString = tr("Device does not support images capture.");

        emit error(d->error);
    }
}


/*!
    \enum QCameraImageCapture::Error

    \value NoError         No Errors.
    \value NotReadyError   The service is not ready for capture yet.
    \value ResourceError   Device is not ready or not available.
    \value NotSupportedFeatureError Device does not support stillimages capture.
    \value FormatError     Current format is not supported.
    \value OutOfSpaceError No space left on device.
*/

/*!
    \fn QCameraImageCapture::error(QCameraImageCapture::Error error)

    Signals that an \a error has occurred.
*/


/*!
    \fn QCameraImageCapture::imageExposed(int id)

    Signal emited when the frame with request \a id was exposed.
*/

/*!
    \fn QCameraImageCapture::imageCaptured(int id, const QImage &preview);

    Signal emited when the frame with request \a id was captured, but not processed and saved yet.
    Frame \a preview can be displayed to user.
*/

/*!
    \fn QCameraImageCapture::imageSaved(int id, const QString &fileName)

    Signal emited when the frame with request \a id was saved to \a fileName.
*/


#include "moc_qcameraimagecapture.cpp"
QT_END_NAMESPACE

