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

#include "qcameracontrol.h"
#include  "qabstractmediacontrol_p.h"

/*!
    \class QCameraControl
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

/*!
    Constructs a camera control object with \a parent.
*/

QCameraControl::QCameraControl(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
{
}

/*!
    Destruct the camera control object.
*/

QCameraControl::~QCameraControl()
{
}

/*!
    \fn QCameraControl::setDevice(const QString& device)

    Set the camera device to \a device.
*/


/*!
    Return the white balance mode being used.
*/

QCamera::WhiteBalanceMode QCameraControl::whiteBalanceMode() const
{
    return QCamera::WhiteBalanceAuto;
}

/*!
    Set the white balance mode to \a mode
*/

void QCameraControl::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    Q_UNUSED(mode);
}

/*!
    Return the white balance modes available.
*/

QCamera::WhiteBalanceModes QCameraControl::supportedWhiteBalanceModes() const
{
    return QCamera::WhiteBalanceAuto;
}

/*!
    Return the white balance.
*/

int QCameraControl::manualWhiteBalance() const
{
    return -1;
}

/*!
    Set the white balance to \a colorTemperature
*/

void QCameraControl::setManualWhiteBalance(int colorTemperature)
{
    Q_UNUSED(colorTemperature);
}


/*!
    \fn void QCameraControl::stateChanged(QCamera::State state)

    Signal emitted when \a state changes state.
*/


