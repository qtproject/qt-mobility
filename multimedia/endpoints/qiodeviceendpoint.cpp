/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qiodeviceendpoint.h"

#include <private/qobject_p.h>

class QIODeviceEndpointPrivate : public QObjectPrivate
{
public:
    QIODeviceEndpointPrivate()
        : device(0)
    {
    }

    QIODevice *device;
};

/*!
    \class QIODeviceEndpoint
    \preliminary
    \brief The QIODeviceEndpoint class provides a media endpoint that reads or writes a QIODevice.

    \sa QAbstractMediaService::setDataInput(), QAbstractMediaService::setDataOutput()
*/

/*!
    Contructs a new I/O device media end point with the given \a parent.
*/
QIODeviceEndpoint::QIODeviceEndpoint(QObject *parent)
    : QObject(*new QIODeviceEndpointPrivate, parent)
{
}

/*!
    Destroys an I/O device media end point.
*/
QIODeviceEndpoint::~QIODeviceEndpoint()
{
}

/*!
    \reimp
*/
QMediaEndpointInterface::Direction QIODeviceEndpoint::direction() const
{
    return InputOutput;
}

/*!
    \property QIODeviceEndpoint::device

    The I/O device written to or read from.
*/

/*!
    Returns the I/O device.
*/
QIODevice *QIODeviceEndpoint::device() const
{
    return d_func()->device;
}

/*!
    Sets the I/O \a device.
*/
void QIODeviceEndpoint::setDevice(QIODevice *device)
{
    d_func()->device = device;
}
