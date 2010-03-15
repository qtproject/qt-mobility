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
#include <experimental/qstillimagecapture.h>
#include <experimental/qimagecapturecontrol.h>
#include <qmediaencodersettings.h>

#include <qimageencodercontrol.h>
#include <qmediaobject_p.h>
#include <qmediaservice.h>
#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qmetaobject.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QStillImageCapture
    \ingroup multimedia

    \preliminary
    \brief The QStillImageCapture class is used for the recording of media content.

    The QStillImageCapture class is a high level images recording class.
    It's not intended to be used alone but for accessing the media
    recording functions of other media objects, like QCamera.

    \code
        camera = new QCamera;
        viewFinder = new QVideoWidget(camera);
        viewFinder->show();

        imageCapture = new QStillImageCapture(camera);

        camera->start();

        imageCapture->capture(fileName);
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
        qRegisterMetaType<QStillImageCapture::Error>("QStillImageCapture::Error");
    }
} _registerRecorderMetaTypes;
}


class QStillImageCapturePrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QStillImageCapture)

public:
    QStillImageCapturePrivate();
    void initControls();

    QImageCaptureControl *control;
    QImageEncoderControl *encoderControl;
    QStillImageCapture::Error error;
    QString errorString;

    void _q_error(int error, const QString &errorString);

    void unsetError() { error = QStillImageCapture::NoError; errorString.clear(); }
};

QStillImageCapturePrivate::QStillImageCapturePrivate():
     control(0),
     error(QStillImageCapture::NoError)
{
}

void QStillImageCapturePrivate::initControls()
{
    Q_Q(QStillImageCapture);

    if (!service)
        return;

    control = qobject_cast<QImageCaptureControl*>(service->control(QImageCaptureControl_iid));
    encoderControl = qobject_cast<QImageEncoderControl *>(service->control(QImageEncoderControl_iid));

    if (control) {
        q->connect(control, SIGNAL(imageCaptured(QString,QImage)),
                   q, SIGNAL(imageCaptured(QString,QImage)));
        q->connect(control, SIGNAL(imageSaved(QString)),
                   q, SIGNAL(imageSaved(QString)));
        q->connect(control, SIGNAL(readyForCaptureChanged(bool)),
                   q, SIGNAL(readyForCaptureChanged(bool)));
        q->connect(control, SIGNAL(error(int,QString)),
                   q, SLOT(_q_error(int,QString)));
    }

}

void QStillImageCapturePrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QStillImageCapture);

    this->error = QStillImageCapture::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}


/*!
    Constructs a media recorder which records the media produced by \a mediaObject.

    The \a parent is passed to QMediaObject.
*/

QStillImageCapture::QStillImageCapture(QMediaObject *mediaObject, QObject *parent):
    QMediaObject(*new QStillImageCapturePrivate, parent, mediaObject->service())
{
    Q_D(QStillImageCapture);

    d->initControls();
}

/*!
    Destroys images capture object.
*/

QStillImageCapture::~QStillImageCapture()
{
}

/*!
    Returns true if the images capture service ready to use.
*/
bool QStillImageCapture::isAvailable() const
{
    if (d_func()->control != NULL)
        return true;
    else
        return false;
}

/*!
    Returns the availability error code.
*/
QtMedia::AvailabilityError QStillImageCapture::availabilityError() const
{
    if (d_func()->control != NULL)
        return QtMedia::NoError;
    else
        return QtMedia::ServiceMissingError;
}

/*!
    Returns the current error state.

    \sa errorString()
*/

QStillImageCapture::Error QStillImageCapture::error() const
{
    return d_func()->error;
}

/*!
    Returns a string describing the current error state.

    \sa error()
*/

QString QStillImageCapture::errorString() const
{
    return d_func()->errorString;
}


/*!
    Returns a list of supported image codecs.
*/
QStringList QStillImageCapture::supportedImageCodecs() const
{
    return d_func()->encoderControl ?
           d_func()->encoderControl->supportedImageCodecs() : QStringList();
}

/*!
    Returns a description of an image \a codec.
*/
QString QStillImageCapture::imageCodecDescription(const QString &codec) const
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
QList<QSize> QStillImageCapture::supportedResolutions(const QImageEncoderSettings &settings, bool *continuous) const
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

QImageEncoderSettings QStillImageCapture::encodingSettings() const
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

void QStillImageCapture::setEncodingSettings(const QImageEncoderSettings &settings)
{
    Q_D(QStillImageCapture);

    if (d->encoderControl)
        d->encoderControl->setImageSettings(settings);
}

/*!
  \property QStillImageCapture::readyForCapture
   Indicates the service is ready to capture an image immediately.
*/

bool QStillImageCapture::isReadyForCapture() const
{
    return d_func()->control ? d_func()->control->isReadyForCapture() : false;
}

/*!
    \fn QStillImageCapture::readyForCaptureChanged(bool ready)

    Signals that a camera's \a ready for capture state has changed.
*/

/*!
    Capture the image and save it to \a file.
    This operation is asynchronous in majority of cases,
    followed by signals QStillImageCapture::imageCaptured(), QStillImageCapture::imageSaved()
    or QStillImageCapture::error()
*/
void QStillImageCapture::capture(const QString &file)
{
    Q_D(QStillImageCapture);

    d->unsetError();

    if (d->control) {
        d->control->capture(file);
    } else {
        d->error = NotSupportedFeatureError;
        d->errorString = tr("Device does not support images capture.");

        emit error(d->error);
    }
}

/*!
    Cancel pending capture requests.
*/
void QStillImageCapture::cancelCapture()
{
    Q_D(QStillImageCapture);

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
    \enum QStillImageCapture::Error

    \value NoError         No Errors.
    \value NotReadyError   The service is not ready for capture yet.
    \value ResourceError   Device is not ready or not available.
    \value NotSupportedFeatureError Device does not support stillimages capture.
    \value FormatError     Current format is not supported.
*/

/*!
    \fn QStillImageCapture::error(QStillImageCapture::Error error)

    Signals that an \a error has occurred.
*/


#include "moc_qstillimagecapture.cpp"
QTM_END_NAMESPACE

