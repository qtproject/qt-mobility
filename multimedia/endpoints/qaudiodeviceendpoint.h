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

#ifndef QAUDIODEVICEENDPOINT_H
#define QAUDIODEVICEENDPOINT_H

#include "qmediaendpointinterface.h"

class QIcon;
class QAudioDeviceEndpointPrivate;

class Q_MEDIA_EXPORT QAudioDeviceEndpoint : public QObject, public QMediaEndpointInterface
{
    Q_OBJECT
    Q_PROPERTY(DeviceDirection directionFilter READ directionFilter WRITE setDirectionFilter)
    Q_PROPERTY(Roles roleFilter READ roleFilter WRITE setRoleFilter)
    Q_PROPERTY(FormFactors formFactorFilter READ formFactorFilter WRITE setFormFactorFilter)
    Q_PROPERTY(int selectedDevice READ selectedDevice WRITE setSelectedDevice NOTIFY selectedDeviceChanged)
    Q_ENUMS(DeviceDirection Role FormFactor)
    Q_FLAGS(Roles FormFactors)
    Q_INTERFACES(QMediaEndpointInterface)
    Q_DECLARE_PRIVATE(QAudioDeviceEndpoint)
public:
    enum DeviceDirection
    {
        InputDevice = 0x01,
        OutputDevice = 0x02,
        InputOutputDevice = InputDevice | OutputDevice
    };

    enum Role
    {
        ConsoleRole = 0x01,
        MultimediaRole = 0x02,
        CommunicationsRole = 0x04,
        AllRoles = ~0

    };

    Q_DECLARE_FLAGS(Roles, Role);

    enum FormFactor
    {
        UnknownFormFactor = 0x0000,
        RemoteNetworkDevice = 0x0001,
        Speakers = 0x0002,
        LineLevel = 0x0004,
        Headphones = 0x0008,
        Microphone = 0x0010,
        Headset = 0x0020,
        Handset = 0x0040,
        UnknownDigitalPassthrough = 0x0080,
        SPDIF = 0x0100,
        HDMI = 0x0200,
        AllFormFactors = ~0
    };

    Q_DECLARE_FLAGS(FormFactors, FormFactor)

    QAudioDeviceEndpoint(QObject *parent = 0);
    ~QAudioDeviceEndpoint();

    Direction direction() const;

    DeviceDirection directionFilter() const;
    virtual void setDirectionFilter(DeviceDirection direction);

    Roles roleFilter() const;
    virtual void setRoleFilter(Roles roles);

    FormFactors formFactorFilter() const;
    virtual void setFormFactorFilter(FormFactors forms);

    virtual int deviceCount() const = 0;

    virtual int direction(int index) const = 0;
    virtual Roles roles(int index) const = 0;
    virtual FormFactor formFactor(int index) const = 0;

    virtual QString name(int index) const = 0;
    virtual QString description(int index) const = 0;
    virtual QIcon icon(int index) const = 0;

    virtual int defaultInputDevice(Role role) const = 0;
    virtual int defaultOutputDevice(Role role) const = 0;

    int selectedDevice() const;

public Q_SLOTS:
    virtual void setSelectedDevice(int index);

Q_SIGNALS:
    void selectedDeviceChanged(int index);
    void selectedDeviceChanged(const QString &deviceName);
    void devicesInserted(int index, int count);
    void devicesRemoved(int index, int count);
    void devicesChanged(int index, int count);
};

#define QAudioDeviceEndpoint_iid "com.nokia.Qt.QAudioDeviceEndpoint/1.0"

Q_MEDIA_DECLARE_ENDPOINT(QAudioDeviceEndpoint, QAudioDeviceEndpoint_iid)

#endif
