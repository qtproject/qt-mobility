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

#ifndef QIODEVICEENDPOINT_H
#define QIODEVICEENDPOINT_H

#include "qmediaendpointinterface.h"

class QIODeviceEndpointInterface : public QMediaEndpointInterface
{
public:
    virtual ~QIODeviceEndpointInterface();

    Direction direction() const;
};

#define QIODeviceEndpointInterface_iid "com.nokia.Qt.QIODeviceEndpointInterface/1.0"

Q_DECLARE_INTERFACE(QIODeviceEndpointInterface, QIODeviceEndpointInterface_iid)

class QIODevice;

class QIODeviceEndpointPrivate;

class QIODeviceEndpoint : public QObject, public QIODeviceEndpointInterface
{
    Q_OBJECT
    Q_PROPERTY(QIODevice* device READ device WRITE setDevice)
    Q_INTERFACES(QIODeviceEndpointInterface)
    Q_DECLARE_PRIVATE(QIODeviceEndpoint)
public:
    QIODeviceEndpoint(QObject *parent = 0);
    ~QIODeviceEndpoint();

    QIODevice *device() const;
    virtual void setDevice(QIODevice *device);
};

#endif
