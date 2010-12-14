/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qdeclarativecamera_p.h"
#include "qdeclarativecamerapreviewprovider_p.h"

#include <qmediaplayercontrol.h>
#include <qmediaservice.h>
#include <qpaintervideosurface_p.h>
#include <qvideorenderercontrol.h>

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

void QDeclarativeCamera::_q_error(int errorCode, const QString &errorString)
{
    emit error(Error(errorCode), errorString);
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
    \qmlclass Video QDeclarativeCamera
    \since 4.7
    \brief The Camera element allows you to add camera viewfinder to a scene.
    \inherits Item

    This element is part of the \bold{Qt.camera 4.7} module.

    \qml
    import Qt 4.7
    import Qt.camera 4.7

    Camera {
        focus: true
    }
    \endqml

*/

/*!
    \internal
    \class QDeclarativeCamera
    \brief The QDeclarativeCamera class provides a camera item that you can add to a QDeclarativeView.
*/

QDeclarativeCamera::QDeclarativeCamera(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    m_camera(0),
    m_viewfinderItem(0),
    m_imageSettingsChanged(false),
    m_pendingState(ActiveState),
    m_isStateSet(false)
{
    m_camera = new QCamera(this);
    m_viewfinderItem = new QGraphicsVideoItem(this);
    m_camera->setViewfinder(m_viewfinderItem);
    m_exposure = m_camera->exposure();
    m_focus = m_camera->focus();

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

QDeclarativeCamera::~QDeclarativeCamera()
{
    m_camera->unload();

    delete m_viewfinderItem;
    delete m_capture;
    delete m_camera;
}


QDeclarativeCamera::Error QDeclarativeCamera::error() const
{
    return QDeclarativeCamera::Error(m_camera->error());
}

QString QDeclarativeCamera::errorString() const
{
    return m_camera->errorString();
}

/*!
    \property QDeclarativeCamera::cameraState
    \brief The current state of the camera object.

    The default camera state is ActiveState.
*/

QDeclarativeCamera::State QDeclarativeCamera::cameraState() const
{
    return m_isStateSet ? QDeclarativeCamera::State(m_camera->state()) : m_pendingState;
}

/*!
  Start the camera.
*/
void QDeclarativeCamera::start()
{
    m_camera->start();
}

/*!
  Stop the camera.
*/
void QDeclarativeCamera::stop()
{
    m_camera->stop();
}

void QDeclarativeCamera::setCameraState(QDeclarativeCamera::State state)
{
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
    \property QDeclarativeCamera::lockStatus
    \brief The overall status for all the requested camera locks.
*/

QDeclarativeCamera::LockStatus QDeclarativeCamera::lockStatus() const
{
    return QDeclarativeCamera::LockStatus(m_camera->lockStatus());
}

/*!
  Start focusing, exposure and white balance calculation.
  If the camera has keyboard focus,
  searchAndLock() is called automatically on camera focus button pressed.
*/
void QDeclarativeCamera::searchAndLock()
{
    m_camera->searchAndLock();
}

/*!
  Unlock focus.

  If the camera has keyboard focus,
  unlock() is called automatically on camera focus button released.
 */
void QDeclarativeCamera::unlock()
{
    m_camera->unlock();
}

/*!
  Start image capture.
*/
void QDeclarativeCamera::captureImage()
{
    m_capture->capture();
}

QImage QDeclarativeCamera::capturedImagePreview() const
{
    return m_capturedImagePreview;
}

QString QDeclarativeCamera::capturedImagePath() const
{
    return m_capturedImagePath;
}

void QDeclarativeCamera::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}

void QDeclarativeCamera::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_viewfinderItem->setSize(newGeometry.size());
    _q_updateFocusZones();

    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void QDeclarativeCamera::keyPressEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_CameraFocus:
        m_camera->searchAndLock();
        break;
    case Qt::Key_Camera:
        if (m_camera->captureMode() == QCamera::CaptureStillImage)
            captureImage();
        //else
        //    m_recorder->record();
        break;
    default:
        QDeclarativeItem::keyPressEvent(event);
    }
}

void QDeclarativeCamera::keyReleaseEvent(QKeyEvent * event)
{
    switch (event->key()) {
    case Qt::Key_CameraFocus:
        m_camera->unlock();
        break;
    case Qt::Key_Camera:
        //if (m_camera->captureMode() == QCamera::CaptureVideo)
        //    m_recorder->stop();
        break;
    default:
        QDeclarativeItem::keyReleaseEvent(event);
    }
}

QDeclarativeCamera::ExposureMode QDeclarativeCamera::exposureMode() const
{
    return ExposureMode(m_exposure->exposureMode());
}

int QDeclarativeCamera::flashMode() const
{
    return m_exposure->flashMode();
}

void QDeclarativeCamera::setFlashMode(int mode)
{
    if (m_exposure->flashMode() != mode) {
        m_exposure->setFlashMode(QCameraExposure::FlashModes(mode));
        emit flashModeChanged(mode);
    }
}

qreal QDeclarativeCamera::exposureCompensation() const
{
    return m_exposure->exposureCompensation();
}

void QDeclarativeCamera::setExposureCompensation(qreal ev)
{
    m_exposure->setExposureCompensation(ev);
}

int QDeclarativeCamera::isoSensitivity() const
{
    return m_exposure->isoSensitivity();
}

qreal QDeclarativeCamera::shutterSpeed() const
{
    return m_exposure->shutterSpeed();
}

qreal QDeclarativeCamera::aperture() const
{
    return m_exposure->aperture();
}

void QDeclarativeCamera::setExposureMode(QDeclarativeCamera::ExposureMode mode)
{
    if (exposureMode() != mode) {
        m_exposure->setExposureMode(QCameraExposure::ExposureMode(mode));
        emit exposureModeChanged(exposureMode());
    }
}

void QDeclarativeCamera::setManualIsoSensitivity(int iso)
{
    m_exposure->setManualIsoSensitivity(iso);
}

QSize QDeclarativeCamera::captureResolution() const
{
    return m_imageSettings.resolution();
}

void QDeclarativeCamera::setCaptureResolution(const QSize &resolution)
{
    if (m_imageSettings.resolution() != resolution) {
        m_imageSettings.setResolution(resolution);

        if (!m_imageSettingsChanged) {
            m_imageSettingsChanged = true;
            QMetaObject::invokeMethod(this, "_q_updateImageSettings", Qt::QueuedConnection);
        }

        emit captureResolutionChanged(resolution);
    }
}

qreal QDeclarativeCamera::maximumOpticalZoom() const
{
    return m_focus->maximumOpticalZoom();
}

qreal QDeclarativeCamera::maximumDigitalZoom() const
{
    return m_focus->maximumDigitalZoom();
}

qreal QDeclarativeCamera::opticalZoom() const
{
    return m_focus->opticalZoom();
}

qreal QDeclarativeCamera::digitalZoom() const
{
    return m_focus->digitalZoom();
}

void QDeclarativeCamera::setOpticalZoom(qreal value)
{
    m_focus->zoomTo(value, digitalZoom());
}

void QDeclarativeCamera::setDigitalZoom(qreal value)
{
    m_focus->zoomTo(opticalZoom(), value);
}

QDeclarativeCamera::WhiteBalanceMode QDeclarativeCamera::whiteBalanceMode() const
{
    return WhiteBalanceMode(m_camera->imageProcessing()->whiteBalanceMode());
}

int QDeclarativeCamera::manualWhiteBalance() const
{
    return m_camera->imageProcessing()->manualWhiteBalance();
}

void QDeclarativeCamera::setWhiteBalanceMode(QDeclarativeCamera::WhiteBalanceMode mode) const
{
    if (whiteBalanceMode() != mode) {
        m_camera->imageProcessing()->setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode(mode));
        emit whiteBalanceModeChanged(whiteBalanceMode());
    }
}

void QDeclarativeCamera::setManualWhiteBalance(int colorTemp) const
{
    if (manualWhiteBalance() != colorTemp) {
        m_camera->imageProcessing()->setManualWhiteBalance(colorTemp);
        emit manualWhiteBalanceChanged(manualWhiteBalance());
    }
}

/*!
  \enum QDeclarativeCamera::LockStatus
    \value Unlocked
        The application is not interested in camera settings value.
        The camera may keep this parameter without changes, this is common with camera focus,
        or adjust exposure and white balance constantly to keep the viewfinder image nice.

    \value Searching
        The application has requested the camera focus, exposure or white balance lock with
        QCamera::searchAndLock(). This state indicates the camera is focusing or calculating exposure and white balance.

    \value Locked
        The camera focus, exposure or white balance is locked.
        The camera is ready to capture, application may check the exposure parameters.

        The locked state usually means the requested parameter stays the same,
        except of the cases when the parameter is requested to be constantly updated.
        For example in continuous focusing mode, the focus is considered locked as long
        and the object is in focus, even while the actual focusing distance may be constantly changing.
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
    \value FlashManual          Flash power is manualy set.
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

QT_END_NAMESPACE

#include "moc_qdeclarativecamera_p.cpp"
