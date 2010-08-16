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

#include <QDebug>

#include <qcamera.h>
#include <qcamerafocus.h>

#include <qmediaobject_p.h>
#include <qcameracontrol.h>
#include <qcameraexposurecontrol.h>
#include <qcamerafocuscontrol.h>
#include <qmediarecordercontrol.h>
#include <qcameraimagecapturecontrol.h>
#include <qvideodevicecontrol.h>

QT_BEGIN_NAMESPACE

class QCameraFocusZoneData : public QSharedData
{
public:
    QCameraFocusZoneData():
        status(QCameraFocusZone::Invalid)
    {

    }

    QCameraFocusZoneData(const QRectF &_area, QCameraFocusZone::FocusZoneStatus _status):
        area(_area),
        status(_status)
    {

    }


    QCameraFocusZoneData(const QCameraFocusZoneData &other):
        QSharedData(other),
        area(other.area),
        status(other.status)
    {
    }

    QCameraFocusZoneData& operator=(const QCameraFocusZoneData &other)
    {
        area = other.area;
        status = other.status;
        return *this;
    }

    QRectF area;
    QCameraFocusZone::FocusZoneStatus status;
};

QCameraFocusZone::QCameraFocusZone()
    :d(new QCameraFocusZoneData)
{

}

QCameraFocusZone::QCameraFocusZone(const QRectF &area, QCameraFocusZone::FocusZoneStatus status)
    :d(new QCameraFocusZoneData(area, status))
{
}

QCameraFocusZone::QCameraFocusZone(const QCameraFocusZone &other)
    :d(other.d)
{

}

QCameraFocusZone::~QCameraFocusZone()
{

}

QCameraFocusZone& QCameraFocusZone::operator=(const QCameraFocusZone &other)
{
    d = other.d;
    return *this;
}

bool QCameraFocusZone::operator==(const QCameraFocusZone &other) const
{
    return d == other.d ||
           (d->area == other.d->area && d->status == other.d->status);
}

bool QCameraFocusZone::operator!=(const QCameraFocusZone &other) const
{
    return !(*this == other);
}

bool QCameraFocusZone::isValid() const
{
    return d->status != Invalid && !d->area.isValid();
}

QRectF QCameraFocusZone::area() const
{
    return d->area;
}

QCameraFocusZone::FocusZoneStatus QCameraFocusZone::status() const
{
    return d->status;
}

void QCameraFocusZone::setStatus(QCameraFocusZone::FocusZoneStatus status)
{
    d->status = status;
}


/*!
    \class QCameraFocus

    \preliminary
    \brief The QCameraFocus class provides interface for
    focus and zoom related camera settings.

    \inmodule QtMultimediaKit
    \ingroup camera

The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.
*/


class QCameraFocusPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QCameraFocus)
public:
    void initControls();

    QCameraFocus *q_ptr;

    QCamera *camera;
    QCameraFocusControl *focusControl;    
};


void QCameraFocusPrivate::initControls()
{
    Q_Q(QCameraFocus);

    focusControl = 0;

    QMediaService *service = camera->service();
    if (service)
        focusControl = qobject_cast<QCameraFocusControl *>(service->requestControl(QCameraFocusControl_iid));

    if (focusControl) {
        q->connect(focusControl, SIGNAL(opticalZoomChanged(qreal)), q, SIGNAL(opticalZoomChanged(qreal)));
        q->connect(focusControl, SIGNAL(digitalZoomChanged(qreal)), q, SIGNAL(digitalZoomChanged(qreal)));
    }
}

/*!
    Construct a QCameraFocus for \a camera.
*/

QCameraFocus::QCameraFocus(QCamera *camera):
    QObject(camera), d_ptr(new QCameraFocusPrivate)
{
    Q_D(QCameraFocus);
    d->camera = camera;
    d->q_ptr = this;
    d->initControls();
}


/*!
    Destroys the camera focus object.
*/

QCameraFocus::~QCameraFocus()
{
}

/*!
    Returns true if focus related settings are supported by this camera.
*/
bool QCameraFocus::isAvailable() const
{
    return d_func()->focusControl != 0;
}

/*!
    Returns the focus mode being used.
*/

QCameraFocus::FocusMode QCameraFocus::focusMode() const
{
    return d_func()->focusControl ? d_func()->focusControl->focusMode() : QCameraFocus::AutoFocus;
}

/*!
    Set the focus mode to \a mode
*/

void QCameraFocus::setFocusMode(QCameraFocus::FocusMode mode)
{
    if (d_func()->focusControl)
        d_func()->focusControl->setFocusMode(mode);
}

/*!
    Returns true if the focus \a mode is supported by camera.
*/

bool QCameraFocus::isFocusModeSupported(QCameraFocus::FocusMode mode) const
{
    return d_func()->focusControl ? d_func()->focusControl->isFocusModeSupported(mode) : false;
}

/*!
  \property QCameraFocus::focusPointMode
  The camera focus point selection mode.
*/

QCameraFocus::FocusPointMode QCameraFocus::focusPointMode() const
{
    return d_func()->focusControl ?
            d_func()->focusControl->focusPointMode() :
            QCameraFocus::FocusPointAuto;
}

void QCameraFocus::setFocusPointMode(QCameraFocus::FocusPointMode mode)
{
    if (d_func()->focusControl)
        d_func()->focusControl->setFocusPointMode(mode);
    else
        qWarning("Focus points mode selection is not supported");
}

/*!
  Returns true if focus point \a mode is supported.
 */
bool QCameraFocus::isFocusPointModeSupported(QCameraFocus::FocusPointMode mode) const
{
    return d_func()->focusControl ?
            d_func()->focusControl->isFocusPointModeSupported(mode) :
            false;

}

/*!
  \property QCameraFocus::customFocusPoint

  Position of custom focus point, in relative frame coordinates:
  QPointF(0,0) points to the left top frame point, QPointF(0.5,0.5) points to the frame center.

  Custom focus point is used only in FocusPointCustom focus mode.
 */

QPointF QCameraFocus::customFocusPoint() const
{
    return d_func()->focusControl ?
            d_func()->focusControl->customFocusPoint() :
            QPointF(0.5,0.5);
}

void QCameraFocus::setCustomFocusPoint(const QPointF &point)
{
    if (d_func()->focusControl)
        d_func()->focusControl->setCustomFocusPoint(point);
    else
        qWarning("Focus points selection is not supported");

}

/*!
  \property QCameraFocus::focusZones

  Returns the list of active focus zones.

  If QCamera::FocusPointAuto or QCamera::FocusPointFaceDetection focus mode is selected
  this method returns the list of zones the camera is actually focused on.

  The coordinates system is the same as for custom focus points:
  QPointF(0,0) points to the left top frame point, QPointF(0.5,0.5) points to the frame center.
 */
QCameraFocusZoneList QCameraFocus::focusZones() const
{
    return d_func()->focusControl ?
            d_func()->focusControl->focusZones() :
            QCameraFocusZoneList();
}

/*!
    Returns the maximum optical zoom
*/

qreal QCameraFocus::maximumOpticalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->maximumOpticalZoom() : 1.0;
}

/*!
    Returns the maximum digital zoom
*/

qreal QCameraFocus::maximumDigitalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->maximumDigitalZoom() : 1.0;
}

/*!
    Returns the current optical zoom value.

    \sa QCameraFocus::digitalZoom()
*/

qreal QCameraFocus::opticalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->opticalZoom() : 1.0;
}

/*!
    Returns the current digital zoom value.

    \sa QCameraFocus::opticalZoom()
*/
qreal QCameraFocus::digitalZoom() const
{
    return d_func()->focusControl ? d_func()->focusControl->digitalZoom() : 1.0;
}


/*!
    Set the camera \a optical and \a digital zoom values.
*/
void QCameraFocus::zoomTo(qreal optical, qreal digital)
{
    if (d_func()->focusControl)
        d_func()->focusControl->zoomTo(optical, digital);
    else
        qWarning("The camera doesn't support zooming.");
}

/*!
    \enum QCameraFocus::FocusMode

    \value ManualFocus          Manual or fixed focus mode.
    \value AutoFocus            One-shot auto focus mode.
    \value ContinuousFocus      Continuous auto focus mode.
    \value InfinityFocus        Focus strictly to infinity.
    \value HyperfocalFocus      Focus to hyperfocal distance, with with the maximum depth of field achieved.
                                All objects at distances from half of this
                                distance out to infinity will be acceptably sharp.
    \value MacroFocus           One shot auto focus to objects close to camera.
*/

/*!
    \enum QCameraFocus::FocusPointMode

    \value FocusPointAuto       Automaticaly select one or multiple focus points.
    \value FocusPointCenter     Focus to the frame center.
    \value FocusPointFaceDetection Focus on faces in the frame.
    \value FocusPointCustom     Focus to the custom point, defined by QCameraFocus::customFocusPoint property.
*/

/*!
    \fn void QCameraFocus::opticalZoomChanged(qreal value)

    Signal emitted when optical zoom value changes to new \a value.
*/

/*!
    \fn void QCameraFocus::digitalZoomChanged(qreal value)

    Signal emitted when digital zoom value changes to new \a value.
*/


/*!
  \fn QCameraFocus::focusZonesChanged()

  Signal is emited when the set of zones, camera focused on is changed.

  Usually the zones list is changed when the camera is focused.
*/


#include "moc_qcamerafocus.cpp"
QT_END_NAMESPACE
