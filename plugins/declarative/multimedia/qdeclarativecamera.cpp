/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativecamera_p.h"
#include "qdeclarativecamerapreviewprovider_p.h"

#include <qmediaplayercontrol.h>
#include <qmediaservice.h>
#include <qpaintervideosurface_p.h>
#include <qvideorenderercontrol.h>
#include <QtDeclarative/qdeclarativeinfo.h>

#include <QtCore/QTimer>
#include <QtGui/qevent.h>


QT_BEGIN_NAMESPACE

class FocusZoneItem : public QGraphicsItem {
public:
    FocusZoneItem(const QCameraFocusZone & zone, const QColor &color, QGraphicsItem *parent = 0)
        :QGraphicsItem(parent),m_zone(zone), m_color(color)
    {}

    virtual ~FocusZoneItem() {}
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0)
    {
        Q_UNUSED(widget);
        Q_UNUSED(option);

        painter->setPen(QPen(QBrush(m_color), 2.5));
        QRectF r = boundingRect();
        QPointF dw(r.width()/10, 0);
        QPointF dh(0, r.width()/10);

        painter->drawLine(r.topLeft(), r.topLeft()+dw);
        painter->drawLine(r.topLeft(), r.topLeft()+dh);

        painter->drawLine(r.topRight(), r.topRight()-dw);
        painter->drawLine(r.topRight(), r.topRight()+dh);

        painter->drawLine(r.bottomLeft(), r.bottomLeft()+dw);
        painter->drawLine(r.bottomLeft(), r.bottomLeft()-dh);

        painter->drawLine(r.bottomRight(), r.bottomRight()-dw);
        painter->drawLine(r.bottomRight(), r.bottomRight()-dh);
    }

    QRectF boundingRect() const {
        if (!parentItem())
            return QRectF();

        QRectF p = parentItem()->boundingRect();
        QRectF zone = m_zone.area();

        return QRectF(p.left() + zone.left()*p.width(),
                      p.top() + zone.top()*p.height(),
                      p.width()*zone.width(),
                      p.height()*zone.height());
    }


    QCameraFocusZone m_zone;
    QColor m_color;
};


void QDeclarativeCamera::_q_nativeSizeChanged(const QSizeF &size)
{
    setImplicitWidth(size.width());
    setImplicitHeight(size.height());
}

void QDeclarativeCamera::_q_error()
{
    emit error(Error(m_camera->error()), m_camera->errorString());
    emit errorChanged();
}

void QDeclarativeCamera::_q_imageCaptured(int id, const QImage &preview)
{
    m_capturedImagePreview = preview;
    QString previewId = QString("preview_%1").arg(id);
    QDeclarativeCameraPreviewProvider::registerPreview(previewId, preview);

    emit imageCaptured(QLatin1String("image://camera/")+previewId);
}

void QDeclarativeCamera::_q_imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    m_capturedImagePath = fileName;
    emit imageSaved(fileName);
}

void QDeclarativeCamera::_q_updateState(QCamera::State state)
{
    emit cameraStateChanged(QDeclarativeCamera::State(state));
}

void QDeclarativeCamera::_q_updateLockStatus(QCamera::LockType type,
                                             QCamera::LockStatus status,
                                             QCamera::LockChangeReason reason)
{
    if (type == QCamera::LockFocus) {
        if (status == QCamera::Unlocked && reason == QCamera::LockFailed) {
            //display failed focus points in red for 1 second
            m_focusFailedTime = QTime::currentTime();
            QTimer::singleShot(1000, this, SLOT(_q_updateFocusZones()));
        } else {
            m_focusFailedTime = QTime();
        }
        _q_updateFocusZones();
    }
}

void QDeclarativeCamera::_q_updateFocusZones()
{
    qDeleteAll(m_focusZones);
    m_focusZones.clear();

    foreach(const QCameraFocusZone &zone, m_camera->focus()->focusZones()) {
        QColor c;
        QCamera::LockStatus lockStatus = m_camera->lockStatus(QCamera::LockFocus);

        if (lockStatus == QCamera::Unlocked) {
            //display failed focus points in red for 1 second
            if (zone.status() == QCameraFocusZone::Selected &&
                    m_focusFailedTime.msecsTo(QTime::currentTime()) < 500) {
                c = Qt::red;
            }
        } else {
            switch (zone.status()) {
            case QCameraFocusZone::Focused:
                c = Qt::green;
                break;
            case QCameraFocusZone::Selected:
                c = lockStatus == QCamera::Searching ? Qt::yellow : Qt::black;
                break;
            default:
                c= QColor::Invalid;
                break;
            }
        }

        if (c.isValid())
            m_focusZones.append(new FocusZoneItem(zone, c, m_viewfinderItem));
    }
}

void QDeclarativeCamera::_q_updateImageSettings()
{
    if (m_imageSettingsChanged) {
        m_imageSettingsChanged = false;
        m_capture->setEncodingSettings(m_imageSettings);
    }
}

void QDeclarativeCamera::_q_applyPendingState()
{
    if (!m_isStateSet) {
        m_isStateSet = true;
        setCameraState(m_pendingState);
    }
}

void QDeclarativeCamera::_q_captureFailed(int id, QCameraImageCapture::Error error, const QString &message)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    emit captureFailed(message);
}


/*!
    \qmlclass Camera QDeclarativeCamera
    \since Mobility 1.1
    \brief The Camera element allows you to add camera viewfinder to a scene.
    \ingroup qml-multimedia
    \inherits Item

    This element is part of the \bold{QtMultimediaKit 1.1} module.

    \qml
    import Qt 4.7
    import QtMultimediaKit 1.1

    Camera {
        focus : visible // to receive focus and capture key events when visible

        flashMode: Camera.FlashRedEyeReduction
        whiteBalanceMode: Camera.WhiteBalanceFlash
        exposureCompensation: -1.0

        onImageCaptured : {
            photoPreview.source = preview  // Show the preview in an Image element
        }

    }
    \endqml

    You can use the \c Camera element to capture images from a camera, and manipulate the capture and
    processing settings that get applied to the image.

    \note On Symbian, your process requires the \c UserEnvironment capability to use this element.
*/

/*!
    \class QDeclarativeCamera
    \brief The QDeclarativeCamera class provides a camera item that you can add to a QDeclarativeView.
    \since Mobility 1.1
*/

/*!
    Construct a declarative camera object using \a parent object.
 */
QDeclarativeCamera::QDeclarativeCamera(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_camera(0),
    m_viewfinderItem(0),
    m_imageSettingsChanged(false),
    m_pendingState(ActiveState),
    m_isStateSet(false),
    m_isValid(true)
{
#if defined(Q_OS_SYMBIAN)
    RProcess thisProcess;
    if (!thisProcess.HasCapability(ECapabilityUserEnvironment)) {
        qmlInfo(this) << "Camera Element requires UserEnvironment Capability to be successfully used on Symbian";
	m_isValid = false;
	return;
    }
#endif
    m_camera = new QCamera(this);
    m_viewfinderItem = new QGraphicsVideoItem(this);
    m_camera->setViewfinder(m_viewfinderItem);
    m_exposure = m_camera->exposure();
    m_focus = m_camera->focus();

    connect(m_camera, SIGNAL(error(QCamera::Error)),
            this, SLOT(_q_error()));

    connect(m_viewfinderItem, SIGNAL(nativeSizeChanged(QSizeF)),
            this, SLOT(_q_nativeSizeChanged(QSizeF)));

    connect(m_camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)), this, SIGNAL(lockStatusChanged()));
    connect(m_camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(_q_updateState(QCamera::State)));

    m_capture = new QCameraImageCapture(m_camera, this);

    connect(m_capture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(_q_imageCaptured(int, QImage)));
    connect(m_capture, SIGNAL(imageSaved(int,QString)), this, SLOT(_q_imageSaved(int, QString)));
    connect(m_capture, SIGNAL(error(int,QCameraImageCapture::Error,QString)),
            this, SLOT(_q_captureFailed(int,QCameraImageCapture::Error,QString)));

    connect(m_focus, SIGNAL(focusZonesChanged()), this, SLOT(_q_updateFocusZones()));
    connect(m_camera, SIGNAL(lockStatusChanged(QCamera::LockType,QCamera::LockStatus,QCamera::LockChangeReason)),
            this, SLOT(_q_updateLockStatus(QCamera::LockType,QCamera::LockStatus,QCamera::LockChangeReason)));

    connect(m_exposure, SIGNAL(isoSensitivityChanged(int)), this, SIGNAL(isoSensitivityChanged(int)));
    connect(m_exposure, SIGNAL(apertureChanged(qreal)), this, SIGNAL(apertureChanged(qreal)));
    connect(m_exposure, SIGNAL(shutterSpeedChanged(qreal)), this, SIGNAL(shutterSpeedChanged(qreal)));

    //connect(m_exposure, SIGNAL(exposureCompensationChanged(qreal)), this, SIGNAL(exposureCompensationChanged(qreal)));

    connect(m_focus, SIGNAL(opticalZoomChanged(qreal)), this, SIGNAL(opticalZoomChanged(qreal)));
    connect(m_focus, SIGNAL(digitalZoomChanged(qreal)), this, SIGNAL(digitalZoomChanged(qreal)));
    connect(m_focus, SIGNAL(maximumOpticalZoomChanged(qreal)), this, SIGNAL(maximumOpticalZoomChanged(qreal)));
    connect(m_focus, SIGNAL(maximumDigitalZoomChanged(qreal)), this, SIGNAL(maximumDigitalZoomChanged(qreal)));

    //delayed start to evoid stopping the cammera immediately if
    //stop() is called after constructor,
    //or to set the rest of camera settings before starting the camera
    QMetaObject::invokeMethod(this, "_q_applyPendingState", Qt::QueuedConnection);

}

/*! Destructor, clean up memory */
QDeclarativeCamera::~QDeclarativeCamera()
{
    if (m_isValid) {
        m_camera->unload();

        delete m_viewfinderItem;
        delete m_capture;
        delete m_camera;
    }
}

/*!
    Returns any camera error.
    \since Mobility 1.1
    \sa QDeclarativeCamera::Error
*/
QDeclarativeCamera::Error QDeclarativeCamera::error() const
{
    if (!m_isValid)
        return QDeclarativeCamera::CameraError;

    return QDeclarativeCamera::Error(m_camera->error());
}

/*!
    \qmlproperty string Camera::errorString
    \since Mobility 1.1

    A description of the current error, if any.
*/
/*!
    \property QDeclarativeCamera::errorString
    \since Mobility 1.1

    A description of the current error, if any.
*/
QString QDeclarativeCamera::errorString() const
{
    if (!m_isValid)
        return QString();

    return m_camera->errorString();
}

/*!
    \qmlproperty enumeration Camera::cameraState
    \since Mobility 1.1

    The current state of the camera object.

    \table
    \header \o Value \o Description
    \row \o UnloadedState
         \o The initial camera state, with camera not loaded,
           the camera capabilities except of supported capture modes
           are unknown.
           While the supported settings are unknown in this state,
           it's allowed to set the camera capture settings like codec,
           resolution, or frame rate.

    \row \o LoadedState
         \o The camera is loaded and ready to be configured.

           In the Idle state it's allowed to query camera capabilities,
           set capture resolution, codecs, etc.

           The viewfinder is not active in the loaded state.

    \row \o ActiveState
          \o In the active state as soon as camera is started
           the viewfinder displays video frames and the
           camera is ready for capture.
    \endtable
*/
/*!
    \property QDeclarativeCamera::cameraState
    \since Mobility 1.1

    The current state of the camera object.

    \table
    \header \o Value \o Description
    \row \o UnloadedState
         \o The initial camera state, with camera not loaded,
           the camera capabilities except of supported capture modes
           are unknown.
           While the supported settings are unknown in this state,
           it's allowed to set the camera capture settings like codec,
           resolution, or frame rate.

    \row \o LoadedState
         \o The camera is loaded and ready to be configured.

           In the Idle state it's allowed to query camera capabilities,
           set capture resolution, codecs, etc.

           The viewfinder is not active in the loaded state.

    \row \o ActiveState
          \o In the active state as soon as camera is started
           the viewfinder displays video frames and the
           camera is ready for capture.
    \endtable
*/
/*!
    \enum QDeclarativeCamera::State
    \value UnloadedState
            The initial camera state, with camera not loaded,
            the camera capabilities except of supported capture modes
            are unknown.
            While the supported settings are unknown in this state,
            it's allowed to set the camera capture settings like codec,
            resolution, or frame rate.

    \value LoadedState
            The camera is loaded and ready to be configured.
            In the Idle state it's allowed to query camera capabilities,
            set capture resolution, codecs, etc.
            The viewfinder is not active in the loaded state.

    \value ActiveState
            In the active state as soon as camera is started
            the viewfinder displays video frames and the
            camera is ready for capture.


    The default camera state is ActiveState.
*/

QDeclarativeCamera::State QDeclarativeCamera::cameraState() const
{
    if (!m_isValid)
        return QDeclarativeCamera::UnloadedState;

    return m_isStateSet ? QDeclarativeCamera::State(m_camera->state()) : m_pendingState;
}

void QDeclarativeCamera::setCameraState(QDeclarativeCamera::State state)
{
    if (!m_isValid)
        return;

    if (!m_isStateSet) {
        m_pendingState = state;
        return;
    }

    switch (state) {
    case QDeclarativeCamera::ActiveState:
        m_camera->start();
        break;
    case QDeclarativeCamera::UnloadedState:
        m_camera->unload();
        break;
    case QDeclarativeCamera::LoadedState:
        m_camera->load();
        break;
    }
}

/*!
    \qmlmethod Camera::start()
    \fn QDeclarativeCamera::start()
    \since Mobility 1.1

    Starts the camera.
*/
void QDeclarativeCamera::start()
{
    if (m_isValid)
        m_camera->start();
}

/*!
    \qmlmethod Camera::stop()
    \fn QDeclarativeCamera::stop()
    \since Mobility 1.1

    Stops the camera.
*/
void QDeclarativeCamera::stop()
{
    if (m_isValid)
        m_camera->stop();
}


/*!
    \qmlproperty enumeration Camera::lockStatus
    \since Mobility 1.1

    The overall status for all the requested camera locks.

    \table
    \header \o Value \o Description
    \row \o Unlocked
        \o The application is not interested in camera settings value.
        The camera may keep this parameter without changes, this is common with camera focus,
        or adjust exposure and white balance constantly to keep the viewfinder image nice.

    \row \o Searching
        \o The application has requested the camera focus, exposure or white balance lock with
        searchAndLock(). This state indicates the camera is focusing or calculating exposure and white balance.

    \row \o Locked
        \o The camera focus, exposure or white balance is locked.
        The camera is ready to capture, application may check the exposure parameters.

        The locked state usually means the requested parameter stays the same,
        except in the cases when the parameter is requested to be constantly updated.
        For example in continuous focusing mode, the focus is considered locked as long
        and the object is in focus, even while the actual focusing distance may be constantly changing.
    \endtable
*/
/*!
    \property QDeclarativeCamera::lockStatus
    \since Mobility 1.1

    The overall status for all the requested camera locks.

    \table
    \header \o Value \o Description
    \row \o Unlocked
        \o The application is not interested in camera settings value.
        The camera may keep this parameter without changes, this is common with camera focus,
        or adjust exposure and white balance constantly to keep the viewfinder image nice.

    \row \o Searching
        \o The application has requested the camera focus, exposure or white balance lock with
        searchAndLock(). This state indicates the camera is focusing or calculating exposure and white balance.

    \row \o Locked
        \o The camera focus, exposure or white balance is locked.
        The camera is ready to capture, application may check the exposure parameters.

        The locked state usually means the requested parameter stays the same,
        except in the cases when the parameter is requested to be constantly updated.
        For example in continuous focusing mode, the focus is considered locked as long
        and the object is in focus, even while the actual focusing distance may be constantly changing.
    \endtable
*/
/*!
    \enum QDeclarativeCamera::LockStatus
    \value Unlocked
        The application is not interested in camera settings value.
        The camera may keep this parameter without changes, this is common with camera focus,
        or adjust exposure and white balance constantly to keep the viewfinder image nice.

    \value Searching
        The application has requested the camera focus, exposure or white balance lock with
        searchAndLock(). This state indicates the camera is focusing or calculating exposure and white balance.

    \value Locked
        The camera focus, exposure or white balance is locked.
        The camera is ready to capture, application may check the exposure parameters.

        The locked state usually means the requested parameter stays the same,
        except in the cases when the parameter is requested to be constantly updated.
        For example in continuous focusing mode, the focus is considered locked as long
        and the object is in focus, even while the actual focusing distance may be constantly changing.
*/
QDeclarativeCamera::LockStatus QDeclarativeCamera::lockStatus() const
{
    if (!m_isValid)
        return QDeclarativeCamera::Unlocked;

    return QDeclarativeCamera::LockStatus(m_camera->lockStatus());
}

/*!
    \qmlmethod Camera::searchAndLock()
    \fn QDeclarativeCamera::searchAndLock()
    \since Mobility 1.1

    Start focusing, exposure and white balance calculation.
    If the camera has keyboard focus, searchAndLock() is called
    automatically when the camera focus button is pressed.
*/
void QDeclarativeCamera::searchAndLock()
{
    if (m_isValid)
        m_camera->searchAndLock();
}

/*!
    \qmlmethod Camera::unlock()
    \fn QDeclarativeCamera::unlock()
    \since Mobility 1.1

    Unlock focus.

    If the camera has keyboard focus, unlock() is called automatically
    when the camera focus button is released.
 */
void QDeclarativeCamera::unlock()
{
    if (m_isValid)
        m_camera->unlock();
}

/*!
    \qmlmethod Camera::captureImage()
    \fn QDeclarativeCamera::captureImage()
    \since Mobility 1.1

    Start image capture.  The \l {imageCaptured()}{onImageCaptured()}
    and \l {imageSaved()}{onImageSaved()} signals will be emitted when
    the capture is complete.
*/
void QDeclarativeCamera::captureImage()
{
    if (m_isValid)
        m_capture->capture();
}

// XXX this doesn't seem to be used
/*!
    \fn QDeclarativeCamera::capturedImagePreview() const
    \since Mobility 1.1
    */
QImage QDeclarativeCamera::capturedImagePreview() const
{
    return m_capturedImagePreview;
}

/*!
    \qmlproperty string Camera::capturedImagePath
    \since Mobility 1.1

    The path to the captured image.
*/
/*!
    \property QDeclarativeCamera::capturedImagePath
    \since Mobility 1.1

    The path to the captured image.
*/
QString QDeclarativeCamera::capturedImagePath() const
{
    return m_capturedImagePath;
}

/*!
    Paint method.
    Takes a \a painter object, a graphics \a style option and a \a widget
    \since Mobility 1.1
*/
void QDeclarativeCamera::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *widget)
{
}

/*!
    Change viewfinder size to \a newGeometry and returning the \a oldGeometry
    \since Mobility 1.1
*/
void QDeclarativeCamera::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_viewfinderItem->setSize(newGeometry.size());
    _q_updateFocusZones();

    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

/*!
    \fn void QDeclarativeCamera::keyPressEvent(QKeyEvent * event)
    Handler for keypress events. The \a event is compared to a small set of events.
    \since Mobility 1.1
*/
void QDeclarativeCamera::keyPressEvent(QKeyEvent * event)
{
    if (!m_isValid || event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        m_camera->searchAndLock();
        event->accept();
        break;
    case Qt::Key_Camera:
        if (m_camera->captureMode() == QCamera::CaptureStillImage)
            captureImage();
        //else
        //    m_recorder->record();
        event->accept();
        break;
    default:
        QDeclarativeItem::keyPressEvent(event);
    }
}

/*!
    Handle the release of a key in \a event and take action if needed.
    \since Mobility 1.1
*/
void QDeclarativeCamera::keyReleaseEvent(QKeyEvent * event)
{
    if (!m_isValid || event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        m_camera->unlock();
        event->accept();
        break;
    case Qt::Key_Camera:
        //if (m_camera->captureMode() == QCamera::CaptureVideo)
        //    m_recorder->stop();
        event->accept();
        break;
    default:
        QDeclarativeItem::keyReleaseEvent(event);
    }
}


/*!
    \qmlproperty enumeration Camera::flashMode
    \since Mobility 1.1

    \table
    \header \o Value \o Description
    \row \o FlashOff             \o Flash is Off.
    \row \o FlashOn              \o Flash is On.
    \row \o FlashAuto            \o Automatic flash.
    \row \o FlashRedEyeReduction \o Red eye reduction flash.
    \row \o FlashFill            \o Use flash to fillin shadows.
    \row \o FlashTorch           \o Constant light source, useful for focusing and video capture.
    \row \o FlashSlowSyncFrontCurtain
                                \o Use the flash in conjunction with a slow shutter speed.
                                This mode allows better exposure of distant objects and/or motion blur effect.
    \row \o FlashSlowSyncRearCurtain
                                \o The similar mode to FlashSlowSyncFrontCurtain but flash is fired at the end of exposure.
    \row \o FlashManual          \o Flash power is manually set.
    \endtable

*/
/*!
    \property QDeclarativeCamera::flashMode
    \since Mobility 1.1

    \table
    \header \o Value \o Description
    \row \o FlashOff             \o Flash is Off.
    \row \o FlashOn              \o Flash is On.
    \row \o FlashAuto            \o Automatic flash.
    \row \o FlashRedEyeReduction \o Red eye reduction flash.
    \row \o FlashFill            \o Use flash to fillin shadows.
    \row \o FlashTorch           \o Constant light source, useful for focusing and video capture.
    \row \o FlashSlowSyncFrontCurtain
                                \o Use the flash in conjunction with a slow shutter speed.
                                This mode allows better exposure of distant objects and/or motion blur effect.
    \row \o FlashSlowSyncRearCurtain
                                \o The similar mode to FlashSlowSyncFrontCurtain but flash is fired at the end of exposure.
    \row \o FlashManual          \o Flash power is manually set.
    \endtable

*/
/*!
    \enum QDeclarativeCamera::FlashMode
    \value FlashOff             Flash is Off.
    \value FlashOn              Flash is On.
    \value FlashAuto            Automatic flash.
    \value FlashRedEyeReduction Red eye reduction flash.
    \value FlashFill            Use flash to fillin shadows.
    \value FlashTorch           Constant light source, useful for focusing and video capture.
    \value FlashSlowSyncFrontCurtain
                                Use the flash in conjunction with a slow shutter speed.
                                This mode allows better exposure of distant objects and/or motion blur effect.
    \value FlashSlowSyncRearCurtain
                                The similar mode to FlashSlowSyncFrontCurtain but flash is fired at the end of exposure.
    \value FlashManual          Flash power is manually set.

*/
int QDeclarativeCamera::flashMode() const
{
    if (!m_isValid)
        return 0;

    return m_exposure->flashMode();
}

void QDeclarativeCamera::setFlashMode(int mode)
{
    if (m_isValid && m_exposure->flashMode() != mode) {
        m_exposure->setFlashMode(QCameraExposure::FlashModes(mode));
        emit flashModeChanged(mode);
    }
}

/*!
    \qmlproperty real Camera::exposureCompensation
    \since Mobility 1.1

    Adjustment for the automatically calculated exposure.  The value is
    in EV units.
 */
/*!
    \property QDeclarativeCamera::exposureCompensation
    \since Mobility 1.1

    Adjustment for the automatically calculated exposure.  The value is
    in EV units.
 */
qreal QDeclarativeCamera::exposureCompensation() const
{
    if (!m_isValid)
        return 0.0;

    return m_exposure->exposureCompensation();
}

void QDeclarativeCamera::setExposureCompensation(qreal ev)
{
    if (m_isValid)
        m_exposure->setExposureCompensation(ev);
}

/*!
    \qmlproperty real Camera::isoSensitivity
    \since Mobility 1.1

    The sensor's ISO sensitivity.
 */
/*!
    \property QDeclarativeCamera::iso
    \since Mobility 1.1

    The sensor's ISO sensitivity.
 */
int QDeclarativeCamera::isoSensitivity() const
{
    if (!m_isValid)
        return 0;

    return m_exposure->isoSensitivity();
}

void QDeclarativeCamera::setManualIsoSensitivity(int iso)
{
    if (!m_isValid)
        return;

    m_exposure->setManualIsoSensitivity(iso);
}

/*!
    \qmlproperty real Camera::shutterSpeed
    \since Mobility 1.1

    The camera's shutter speed, in seconds.
*/
/*!
    \property QDeclarativeCamera::shutterSpeed
    \since Mobility 1.1

    The camera's shutter speed, in seconds.
*/
qreal QDeclarativeCamera::shutterSpeed() const
{
    if (!m_isValid)
        return 0.0;

    return m_exposure->shutterSpeed();
}

/*!
    \qmlproperty real Camera::aperture
    \since Mobility 1.1

    The lens aperture as an F number (the ratio of the focal length to effective aperture diameter).
*/
/*!
    \property QDeclarativeCamera::aperture
    \since Mobility 1.1

    The lens aperture as an F number (the ratio of the focal length to effective aperture diameter).
*/
qreal QDeclarativeCamera::aperture() const
{
    if (!m_isValid)
        return 0.0;

    return m_exposure->aperture();
}

/*!
    \qmlproperty enumeration Camera::exposureMode
    \since Mobility 1.1

    \table
    \header \o Value \o Description
    \row \o ExposureManual        \o Manual mode.
    \row \o ExposureAuto          \o Automatic mode.
    \row \o ExposureNight         \o Night mode.
    \row \o ExposureBacklight     \o Backlight exposure mode.
    \row \o ExposureSpotlight     \o Spotlight exposure mode.
    \row \o ExposureSports        \o Spots exposure mode.
    \row \o ExposureSnow          \o Snow exposure mode.
    \row \o ExposureBeach         \o Beach exposure mode.
    \row \o ExposureLargeAperture \o Use larger aperture with small depth of field.
    \row \o ExposureSmallAperture \o Use smaller aperture.
    \row \o ExposurePortrait      \o Portrait exposure mode.
    \row \o ExposureModeVendor    \o The base value for device specific exposure modes.
    \endtable

*/
/*!
    \enum QDeclarativeCamera::ExposureMode
    \value ExposureManual        Manual mode.
    \value ExposureAuto          Automatic mode.
    \value ExposureNight         Night mode.
    \value ExposureBacklight     Backlight exposure mode.
    \value ExposureSpotlight     Spotlight exposure mode.
    \value ExposureSports        Spots exposure mode.
    \value ExposureSnow          Snow exposure mode.
    \value ExposureBeach         Beach exposure mode.
    \value ExposureLargeAperture Use larger aperture with small depth of field.
    \value ExposureSmallAperture Use smaller aperture.
    \value ExposurePortrait      Portrait exposure mode.
    \value ExposureModeVendor    The base value for device specific exposure modes.

*/
/*!
    \property QDeclarativeCamera::exposureMode
    \since Mobility 1.1

    Camera exposure modes.
*/
QDeclarativeCamera::ExposureMode QDeclarativeCamera::exposureMode() const
{
    if (!m_isValid)
        return QDeclarativeCamera::ExposureAuto;

    return ExposureMode(m_exposure->exposureMode());
}

void QDeclarativeCamera::setExposureMode(QDeclarativeCamera::ExposureMode mode)
{
    if (!m_isValid)
        return;

    if (exposureMode() != mode) {
        m_exposure->setExposureMode(QCameraExposure::ExposureMode(mode));
        emit exposureModeChanged(exposureMode());
    }
}

/*!
    \qmlproperty size Camera::captureResolution
    \since Mobility 1.1

    The resolution to capture the image at.  If empty, the system will pick
    a good size.
*/
/*!
    \property QDeclarativeCamera::captureResolution
    \since Mobility 1.1

    The resolution to capture the image at.  If empty, the system will pick
    a good size.
*/
QSize QDeclarativeCamera::captureResolution() const
{
    if (!m_isValid)
        return QSize();

    return m_imageSettings.resolution();
}

void QDeclarativeCamera::setCaptureResolution(const QSize &resolution)
{
    if (m_isValid && m_imageSettings.resolution() != resolution) {
        m_imageSettings.setResolution(resolution);

        if (!m_imageSettingsChanged) {
            m_imageSettingsChanged = true;
            QMetaObject::invokeMethod(this, "_q_updateImageSettings", Qt::QueuedConnection);
        }

        emit captureResolutionChanged(resolution);
    }
}

/*!
    \qmlproperty real Camera::maximumOpticalZoom
    \since Mobility 1.1

    The maximum optical zoom factor, or 1.0 if optical zoom is not supported.
*/
/*!
    \property QDeclarativeCamera::maximumOpticalZoom
    \since Mobility 1.1

    The maximum optical zoom factor, or 1.0 if optical zoom is not supported.
*/
qreal QDeclarativeCamera::maximumOpticalZoom() const
{
    if (!m_isValid)
        return 0.0;

    return m_focus->maximumOpticalZoom();
}

/*!
    \qmlproperty real Camera::maximumDigitalZoom
    \since Mobility 1.1

    The maximum digital zoom factor, or 1.0 if digital zoom is not supported.
*/
/*!
    \property  QDeclarativeCamera::maximumDigitalZoom
    \since Mobility 1.1

    The maximum digital zoom factor, or 1.0 if digital zoom is not supported.
*/
qreal QDeclarativeCamera::maximumDigitalZoom() const
{
    if (!m_isValid)
        return 0.0;

    return m_focus->maximumDigitalZoom();
}

/*!
    \qmlproperty real Camera::opticalZoom
    \since Mobility 1.1

    The current optical zoom factor.
*/
/*!
    \property QDeclarativeCamera::opticalZoom
    \since Mobility 1.1

    The current optical zoom factor.
*/
qreal QDeclarativeCamera::opticalZoom() const
{
    if (!m_isValid)
        return 0.0;

    return m_focus->opticalZoom();
}

void QDeclarativeCamera::setOpticalZoom(qreal value)
{
    if (m_isValid)
        m_focus->zoomTo(value, digitalZoom());
}

/*!
    \qmlproperty real Camera::digitalZoom
    \since Mobility 1.1

    The current digital zoom factor.
*/
/*!
    \property   QDeclarativeCamera::digitalZoom
    \since Mobility 1.1

    The current digital zoom factor.
*/
qreal QDeclarativeCamera::digitalZoom() const
{
    if (!m_isValid)
        return 0.0;

    return m_focus->digitalZoom();
}

void QDeclarativeCamera::setDigitalZoom(qreal value)
{
    if (m_isValid)
        m_focus->zoomTo(opticalZoom(), value);
}

/*!
    \enum QDeclarativeCamera::WhiteBalanceMode
    \value WhiteBalanceManual       Manual white balance. In this mode the manual white balance property value is used.
    \value WhiteBalanceAuto         Auto white balance mode.
    \value WhiteBalanceSunlight     Sunlight white balance mode.
    \value WhiteBalanceCloudy       Cloudy white balance mode.
    \value WhiteBalanceShade        Shade white balance mode.
    \value WhiteBalanceTungsten     Tungsten white balance mode.
    \value WhiteBalanceFluorescent  Fluorescent white balance mode.
    \value WhiteBalanceIncandescent Incandescent white balance mode.
    \value WhiteBalanceFlash        Flash white balance mode.
    \value WhiteBalanceSunset       Sunset white balance mode.
    \value WhiteBalanceVendor       Vendor defined white balance mode.
*/
/*!
    \qmlproperty enumeration Camera::whiteBalanceMode
    \since Mobility 1.1

    \table
    \header \o Value \o Description
    \row \o WhiteBalanceManual       \o Manual white balance. In this mode the manual white balance property value is used.
    \row \o WhiteBalanceAuto         \o Auto white balance mode.
    \row \o WhiteBalanceSunlight     \o Sunlight white balance mode.
    \row \o WhiteBalanceCloudy       \o Cloudy white balance mode.
    \row \o WhiteBalanceShade        \o Shade white balance mode.
    \row \o WhiteBalanceTungsten     \o Tungsten white balance mode.
    \row \o WhiteBalanceFluorescent  \o Fluorescent white balance mode.
    \row \o WhiteBalanceIncandescent \o Incandescent white balance mode.
    \row \o WhiteBalanceFlash        \o Flash white balance mode.
    \row \o WhiteBalanceSunset       \o Sunset white balance mode.
    \row \o WhiteBalanceVendor       \o Vendor defined white balance mode.
    \endtable

    \sa manualWhiteBalance
*/
/*!
    \property QDeclarativeCamera::whiteBalanceMode
    \since Mobility 1.1

    \sa WhiteBalanceMode
*/
QDeclarativeCamera::WhiteBalanceMode QDeclarativeCamera::whiteBalanceMode() const
{
    if (!m_isValid)
        return QDeclarativeCamera::WhiteBalanceAuto;

    return WhiteBalanceMode(m_camera->imageProcessing()->whiteBalanceMode());
}

void QDeclarativeCamera::setWhiteBalanceMode(QDeclarativeCamera::WhiteBalanceMode mode) const
{
    if (m_isValid && whiteBalanceMode() != mode) {
        m_camera->imageProcessing()->setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode(mode));
        emit whiteBalanceModeChanged(whiteBalanceMode());
    }
}

/*!
    \qmlproperty int Camera::manualWhiteBalance
    \since Mobility 1.1

    The color temperature used when in manual white balance mode (WhiteBalanceManual).

    \sa whiteBalanceMode
*/
/*!
    \property QDeclarativeCamera::manualWhiteBalance
    \since Mobility 1.1

    The color temperature used when in manual white balance mode (WhiteBalanceManual).

    \sa whiteBalanceMode
*/
int QDeclarativeCamera::manualWhiteBalance() const
{
    if (!m_isValid)
        return 0;

    return m_camera->imageProcessing()->manualWhiteBalance();
}

void QDeclarativeCamera::setManualWhiteBalance(int colorTemp) const
{
    if (m_isValid && manualWhiteBalance() != colorTemp) {
        m_camera->imageProcessing()->setManualWhiteBalance(colorTemp);
        emit manualWhiteBalanceChanged(manualWhiteBalance());
    }
}

/*!
    \fn void QDeclarativeCamera::error(QDeclarativeCamera::Error , const QString &)
    \since Mobility 1.1
    This handler is called when an error occurs.  The enumeration value \a error is one of the
    values defined below, and a descriptive string value is available in \a errorString.

    \table
    \header \o Value \o Description
    \row \o NoError \o No errors have occurred.
    \row \o CameraError \o An error has occurred.
    \row \o InvalidRequestError \o System resource doesn't support requested functionality.
    \row \o ServiceMissingError \o No camera service available.
    \row \o NotSupportedFeatureError \o The feature is not supported.
    \endtable

    \sa QDeclarativeCamera::Error
*/
/*!
    \qmlsignal Camera::onError(error, errorString)
    \since Mobility 1.1


    This handler is called when an error occurs.  The enumeration value \a error is one of the
    values defined below, and a descriptive string value is available in \a errorString.

    \table
    \header \o Value \o Description
    \row \o NoError \o No errors have occurred.
    \row \o CameraError \o An error has occurred.
    \row \o InvalidRequestError \o System resource doesn't support requested functionality.
    \row \o ServiceMissingError \o No camera service available.
    \row \o NotSupportedFeatureError \o The feature is not supported.
    \endtable
*/
/*!
    \qmlsignal Camera::onError(error, errorString)
    \since Mobility 1.1


    This handler is called when an error occurs.  The enumeration value \a error is one of the
    values defined below, and a descriptive string value is available in \a errorString.
*/
/*!
    \enum QDeclarativeCamera::Error
    \value NoError                  No errors have occurred.
    \value CameraError              An error has occurred.
    \value InvalidRequestError      System resource doesn't support requested functionality.
    \value ServiceMissingError      No camera service available.
    \value NotSupportedFeatureError The feature is not supported.
*/


/*!
    \qmlsignal Camera::onCaptureFailed(message)
    \since Mobility 1.1

    This handler is called when an error occurs during capture.  A descriptive message is available in \a message.
*/
/*!
    \fn QDeclarativeCamera::captureFailed(const QString &message)
    \since Mobility 1.1

    This handler is called when an error occurs during capture.  A descriptive message is available in \a message.
*/

/*!
    \qmlsignal Camera::onImageCaptured(preview)
    \since Mobility 1.1

    This handler is called when an image has been captured but not yet saved to the filesystem.  The \a preview
    parameter can be used as the URL supplied to an Image element.

    \sa onImageSaved
*/
/*!
    \fn QDeclarativeCamera::imageCaptured(const QString &preview)
    \since Mobility 1.1

    This handler is called when an image has been captured but not yet saved to the filesystem.  The \a preview
    parameter can be used as the URL supplied to an Image element.

    \sa imageSaved()
*/

/*!
    \qmlsignal Camera::onImageSaved(path)
    \since Mobility 1.1

    This handler is called after the image has been written to the filesystem.  The \a path is a local file path, not a URL.

    \sa onImageCaptured
*/
/*!
    \fn QDeclarativeCamera::imageSaved(const QString &path)
    \since Mobility 1.1

    This handler is called after the image has been written to the filesystem.  The \a path is a local file path, not a URL.

    \sa imageCaptured()
*/


/*!
    \fn void QDeclarativeCamera::lockStatusChanged()
    \since Mobility 1.1
*/
/*!
    \qmlsignal Camera::lockStatusChanged()
    \since Mobility 1.1
*/

/*!
    \fn void QDeclarativeCamera::stateChanged(QDeclarativeCamera::State)
    \since Mobility 1.1

    \qmlsignal Camera::stateChanged(Camera::State)
*/

/*!
    \fn void QDeclarativeCamera::imageCaptured(const QString &)
    \since Mobility 1.1

    \qmlsignal Camera::imageCaptured(string)
*/

/*!
    \fn void QDeclarativeCamera::imageSaved(const QString &)
    \since Mobility 1.1

    \qmlsignal Camera::imageSaved(string)
*/


/*!
    \fn void QDeclarativeCamera::errorChanged()
    \since Mobility 1.1

*/
/*!
    \qmlsignal Camera::errorChanged()
    \since Mobility 1.1
*/

/*!
    \fn void QDeclarativeCamera::isoSensitivityChanged(int isoValue)
    \since Mobility 1.1

    The iso value has changed to \a isoValue.
*/
/*!
    \qmlsignal Camera::isoSensitivityChanged(int)
    \since Mobility 1.1
*/

/*!
    \fn void QDeclarativeCamera::apertureChanged(qreal aperture)
    Signals the change to a new \a aperture value.
    \since Mobility 1.1

*/
/*!
    \qmlsignal Camera::apertureChanged(real)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::shutterSpeedChanged(qreal speed)
    \since Mobility 1.1

    The shutter speed has been changed to \a speed.

*/
/*!
    \qmlsignal Camera::shutterSpeedChanged(real)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::exposureCompensationChanged(qreal expComp)
    \since Mobility 1.1

    Compensation has changed to the new value \a expComp.
*/
/*!
    \qmlsignal Camera::exposureCompensationChanged(real)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::opticalZoomChanged(qreal zoom)
    \since Mobility 1.1

    Optical zoom changed to \a zoom.
*/
/*!
    \qmlsignal Camera::opticalZoomChanged(real)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::digitalZoomChanged(qreal)
    \since Mobility 1.1

*/
/*!
    \qmlsignal Camera::digitalZoomChanged(real)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::maximumOpticalZoomChanged(qreal)
    \since Mobility 1.1

    \qmlsignal Camera::maximumOpticalZoomChanged(real)
*/

/*!
    \fn void QDeclarativeCamera::maximumDigitalZoomChanged(qreal zoom)
    The maximum digital zoom is now \a zoom.
    \since Mobility 1.1

*/
/*!
    \qmlsignal Camera::maximumDigitalZoomChanged(real)
    \since Mobility 1.1

*/


/*!
    \fn void QDeclarativeCamera::exposureModeChanged(QDeclarativeCamera::ExposureMode mode)
    \since Mobility 1.1

    Signals that the exposure mode is now \a mode.
*/
/*!
    \qmlsignal Camera::exposureModeChanged(Camera::ExposureMode)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::flashModeChanged(int)
    \since Mobility 1.1

*/
/*!
    \qmlsignal Camera::flashModeChanged(int mode)
    \since Mobility 1.1

    The flash mode is now \a mode.
    \sa QDeclarativeCamera::FlashMode
*/

/*!
    \fn void QDeclarativeCamera::whiteBalanceModeChanged(QDeclarativeCamera::WhiteBalanceMode) const
    \since Mobility 1.1

*/
/*!
    \qmlsignal Camera::whiteBalanceModeChanged(Camera::WhiteBalanceMode)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::manualWhiteBalanceChanged(int newWhiteBal) const
    Indicates that the white balance has been manually changed to \a newWhiteBal.
    \since Mobility 1.1

*/
/*!
    \qmlsignal Camera::manualWhiteBalanceChanged(int)
    \since Mobility 1.1

*/

/*!
    \fn void QDeclarativeCamera::captureResolutionChanged(const QSize &resolution)
    \since Mobility 1.1

    Signal that the resolution has changed to \a resolution.
 */
/*!

    \qmlsignal Camera::captureResolutionChanged(Item)
    \since Mobility 1.1

*/

/*!
    \fn QDeclarativeCamera::cameraStateChanged(QDeclarativeCamera::State state)
    \since Mobility 1.1

    The camera state has changed to value \a state.

*/
/*!
    \qmlsignal Camera::cameraStateChanged(Camera::State)
    \since Mobility 1.1

*/


QT_END_NAMESPACE

#include "moc_qdeclarativecamera_p.cpp"
