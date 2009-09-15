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

#include "qimageprocessingcontrol.h"
#include  "qabstractmediacontrol_p.h"

/*!
    \class QImageProcessingControl
    \ingroup multimedia

    \preliminary
    \brief The abstract class for controling image processing parameters,
    like white balance, contrast, saturation, sharpening and denoising.
    
    \sa QAbstractMediaService, QCamera
*/

/*!
    Constructs an image processing control object with \a parent.
*/

QImageProcessingControl::QImageProcessingControl(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
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

