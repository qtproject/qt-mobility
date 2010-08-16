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

#include <qcameraimageprocessing.h>

#include <qmediaobject_p.h>
#include <qcameracontrol.h>
#include <qcameraexposurecontrol.h>
#include <qcamerafocuscontrol.h>
#include <qmediarecordercontrol.h>
#include <qcameraimageprocessingcontrol.h>
#include <qcameraimagecapturecontrol.h>
#include <qvideodevicecontrol.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCameraImageProcessing

    \preliminary
    \brief The QCameraImageProcessing class provides interface for
    focus and zoom related camera settings.

    \inmodule QtMultimediaKit
    \ingroup camera

The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.
*/


class QCameraImageProcessingPrivate : public QMediaObjectPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QCameraImageProcessing)
public:
    void initControls();

    QCameraImageProcessing *q_ptr;

    QCamera *camera;
    QCameraImageProcessingControl *imageControl;
};


void QCameraImageProcessingPrivate::initControls()
{
    imageControl = 0;

    QMediaService *service = camera->service();
    if (service)
        imageControl = qobject_cast<QCameraImageProcessingControl *>(service->requestControl(QCameraImageProcessingControl_iid));
}

/*!
    Construct a QCameraImageProcessing for \a camera.
*/

QCameraImageProcessing::QCameraImageProcessing(QCamera *camera):
    QObject(camera), d_ptr(new QCameraImageProcessingPrivate)
{
    Q_D(QCameraImageProcessing);
    d->camera = camera;
    d->q_ptr = this;
    d->initControls();
}


/*!
    Destroys the camera focus object.
*/

QCameraImageProcessing::~QCameraImageProcessing()
{
}


/*!
    Returns true if image processing related settings are supported by this camera.
*/
bool QCameraImageProcessing::isAvailable() const
{
    return d_func()->imageControl != 0;
}


/*!
    Returns the white balance mode being used.
*/

QCameraImageProcessing::WhiteBalanceMode QCameraImageProcessing::whiteBalanceMode() const
{
    return d_func()->imageControl ? d_func()->imageControl->whiteBalanceMode() : QCameraImageProcessing::WhiteBalanceAuto;
}

/*!
    Sets the white balance to \a mode.
*/

void QCameraImageProcessing::setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode)
{
    if (d_func()->imageControl)
        d_func()->imageControl->setWhiteBalanceMode(mode);
}

/*!
    Returns true if the white balance \a mode is supported.
*/

bool QCameraImageProcessing::isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const
{
    return d_func()->imageControl ? d_func()->imageControl->isWhiteBalanceModeSupported(mode) : false;
}

/*!
    Returns the current color temperature if the
    manual white balance is active, otherwise the
    return value is undefined.
*/

int QCameraImageProcessing::manualWhiteBalance() const
{
    return d_func()->imageControl ? d_func()->imageControl->manualWhiteBalance() : -1;
}

/*!
    Sets manual white balance to \a colorTemperature
*/

void QCameraImageProcessing::setManualWhiteBalance(int colorTemperature)
{
    if (d_func()->imageControl)
        d_func()->imageControl->setManualWhiteBalance(colorTemperature);
}

/*!
    Return the contrast.
*/
int QCameraImageProcessing::contrast() const
{
    QVariant value;

    if (d_func()->imageControl)
        value = d_func()->imageControl->processingParameter(QCameraImageProcessingControl::Contrast);

    return value.toInt();
}

/*!
    Set the contrast to \a value.

    Valid contrast values range between -100 and 100, the default is 0.
*/
void QCameraImageProcessing::setContrast(int value)
{
    if (d_func()->imageControl)
        d_func()->imageControl->setProcessingParameter(QCameraImageProcessingControl::Contrast,
                                                       QVariant(value));
}

/*!
    Returns the saturation value.
*/
int QCameraImageProcessing::saturation() const
{
    QVariant value;

    if (d_func()->imageControl)
        value = d_func()->imageControl->processingParameter(QCameraImageProcessingControl::Saturation);

    return value.toInt();
}

/*!
    Sets the saturation value to \a value.

    Valid saturation values range between -100 and 100, the default is 0.
*/

void QCameraImageProcessing::setSaturation(int value)
{
    if (d_func()->imageControl)
        d_func()->imageControl->setProcessingParameter(QCameraImageProcessingControl::Saturation,
                                                       QVariant(value));
}

/*!
    Identifies if sharpening is supported.

    Returns true if sharpening is supported; and false if it is not.
*/
bool QCameraImageProcessing::isSharpeningSupported() const
{
    if (d_func()->imageControl)
        return d_func()->imageControl->isProcessingParameterSupported(QCameraImageProcessingControl::Sharpening);
    else
        return false;
}

/*!
    Returns the sharpening level.
*/
int QCameraImageProcessing::sharpeningLevel() const
{
    QVariant value;

    if (d_func()->imageControl)
        value = d_func()->imageControl->processingParameter(QCameraImageProcessingControl::Sharpening);

    if (value.isNull())
        return -1;
    else
        return value.toInt();
}

/*!
    Sets the sharpening \a level.

    Valid sharpening level values range between -1 for default sharpening level,
    0 for sharpening disabled and 100 for maximum sharpening applied.
*/

void QCameraImageProcessing::setSharpeningLevel(int level)
{
    Q_D(QCameraImageProcessing);
    if (d->imageControl)
        d->imageControl->setProcessingParameter(QCameraImageProcessingControl::Sharpening,
                                                level == -1 ? QVariant() : QVariant(level));
}

/*!
    Returns true if denoising is supported.
*/
bool QCameraImageProcessing::isDenoisingSupported() const
{
    if (d_func()->imageControl)
        return d_func()->imageControl->isProcessingParameterSupported(QCameraImageProcessingControl::Denoising);
    else
        return false;
}

/*!
    Returns the denoising level.
*/
int QCameraImageProcessing::denoisingLevel() const
{
    QVariant value;

    if (d_func()->imageControl)
        value = d_func()->imageControl->processingParameter(QCameraImageProcessingControl::Denoising);

    if (value.isNull())
        return -1;
    else
        return value.toInt();
}

/*!
    Sets the denoising \a level.

    Valid denoising level values range between -1 for default denoising level,
    0 for denoising disabled and 100 for maximum denoising applied.
*/
void QCameraImageProcessing::setDenoisingLevel(int level)
{
    Q_D(QCameraImageProcessing);
    if (d->imageControl)
        d->imageControl->setProcessingParameter(QCameraImageProcessingControl::Denoising,
                                                level == -1 ? QVariant() : QVariant(level));
}


/*!
    \enum QCameraImageProcessing::WhiteBalanceMode

    \value WhiteBalanceManual       Manual white balance. In this mode the white balance should be set with
                                    setManualWhiteBalance()
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

#include "moc_qcameraimageprocessing.cpp"
QT_END_NAMESPACE
