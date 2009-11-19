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

#include <experimental/qimageprocessingcontrol.h>
#include  "qmediacontrol_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QImageProcessingControl
    \ingroup multimedia
    \ingroup multimedia-serv

    \preliminary
    \brief The QImageProcessingControl class provides an abstract class
    for controling image processing parameters, like white balance,
    contrast, saturation, sharpening and denoising.
    
    The interface name of QImageProcessingControl is \c com.nokia.Qt.QImageProcessingControl/1.0 as
    defined in QImageProcessingControl_iid.

    \sa QMediaService::control(), QCamera
*/

/*!
    \macro QImageProcessingControl_iid

    \c com.nokia.Qt.QImageProcessingControl/1.0

    Defines the interface name of the QImageProcessingControl class.

    \relates QImageProcessingControl
*/

/*!
    Constructs an image processing control object with \a parent.
*/

QImageProcessingControl::QImageProcessingControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the image processing control object.
*/

QImageProcessingControl::~QImageProcessingControl()
{
}


/*!
    \fn QImageProcessingControl::whiteBalanceMode() const
    Return the white balance mode being used.
*/

/*!
    \fn QImageProcessingControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
    Set the white balance mode to \a mode
*/

/*!
    \fn QImageProcessingControl::supportedWhiteBalanceModes() const
    Return the list of supported white balance modes.
*/

/*!
    \fn QImageProcessingControl::manualWhiteBalance() const
    Return the manual white balance, in K.
*/

/*!
    \fn QImageProcessingControl::setManualWhiteBalance(int colorTemperature)
    Set the white balance to \a colorTemperature
*/

/*!
    \fn QImageProcessingControl::contrast() const
    Return the contrast.
*/

/*!
    \fn QImageProcessingControl::setContrast(qreal value)
    Set the contrast to \a value.
*/

/*!
    \fn QImageProcessingControl::isDenoisingSupported() const
    Returns true if denoising is supported.
*/

/*!
    \fn QImageProcessingControl::saturation() const
    Returns the saturation value.
*/

/*!
    \fn QImageProcessingControl::setSaturation(qreal value)
    Sets the saturation value to \a value.
*/

/*!
    \fn QImageProcessingControl::denoisingLevel() const
    Returns the denoising level.
*/

/*!
    \fn QImageProcessingControl::setDenoisingLevel(qreal level)

    Sets the denoising \a level.
*/

/*!
    \fn QImageProcessingControl::isSharpeningSupported() const

    Identifies if sharpening is supported.

    Returns true if sharpening is supported; and false if it is not.
*/

/*!
    \fn QImageProcessingControl::setSharpeningLevel(qreal level)
    Sets the sharpening \a level.
*/

/*!
    \fn QImageProcessingControl::sharpeningLevel() const
    Returns the sharpening level.
*/

#include "moc_qimageprocessingcontrol.cpp"
QTM_END_NAMESPACE

