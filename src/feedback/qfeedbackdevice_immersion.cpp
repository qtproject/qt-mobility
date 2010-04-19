/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qfeedbackdevice.h"
#include "qfeedbackeffect_p.h"
#include <QtCore/QString>
#include <QtCore/QMutex>

#include <ImmVibe.h>


QTM_BEGIN_NAMESPACE

static int g_defaultDevices[2] = {VIBE_INVALID_DEVICE_HANDLE_VALUE, VIBE_INVALID_DEVICE_HANDLE_VALUE};

class DeviceHandler
{
public:
    DeviceHandler()
    {
        if (VIBE_FAILED(ImmVibeInitialize(VIBE_CURRENT_VERSION_NUMBER))) {
            //that should be done once
            //error management
            qWarning("the Immersion library could not be initialized");
        }

        //looking for the default devices
        const int nbDev = ImmVibeGetDeviceCount();
        for (int i = 0; i < nbDev; ++i) {
            VibeInt32 type = 0;
            ImmVibeGetDeviceCapabilityInt32(i, VIBE_DEVCAPTYPE_ACTUATOR_TYPE, &type);
            if (type == VIBE_DEVACTUATORTYPE_PIEZO_WAVE || type == VIBE_DEVACTUATORTYPE_PIEZO) {
                if (VIBE_IS_INVALID_DEVICE_HANDLE(g_defaultDevices[QFeedbackDevice::Touch]))
                    g_defaultDevices[QFeedbackDevice::Touch] = i;
            } else if (VIBE_IS_INVALID_DEVICE_HANDLE(g_defaultDevices[QFeedbackDevice::Vibra])) {
                g_defaultDevices[QFeedbackDevice::Vibra] = i;
            }
        }
    }

    ~DeviceHandler()
    {
        //cleanup the devices when terminating
        for (int i = 0 ; i < handles.size(); ++i)
            ImmVibeCloseDevice(handles.at(i));

        ImmVibeTerminate();
    }

    VibeInt32 handleForDevice(const QFeedbackDevice &device)
    {
        //we avoid locking too much (it will only lock if the device is not yet open
        if (handles.size() <= device.id()) {
            QMutexLocker locker(&mutex);
            while (handles.size() <= device.id())
                handles.append(VIBE_INVALID_DEVICE_HANDLE_VALUE);
        }

        if (VIBE_IS_INVALID_DEVICE_HANDLE(handles.at(device.id()))) {
            QMutexLocker locker(&mutex);
            if (VIBE_IS_INVALID_DEVICE_HANDLE(handles.at(device.id())))
                ImmVibeOpenDevice(device.id(), &handles[device.id()] );
        }
        return handles.at(device.id());
    }

private:
    QVector<VibeInt32> handles;
    QMutex mutex;
};

Q_GLOBAL_STATIC(DeviceHandler, qDeviceHandler);


VibeInt32 qHandleForDevice(const QFeedbackDevice &device)
{
    return qDeviceHandler()->handleForDevice(device);
}

QString QFeedbackDevice::name() const
{
    char szDeviceName[VIBE_MAX_DEVICE_NAME_LENGTH] = { 0 };
    if (VIBE_FAILED(ImmVibeGetDeviceCapabilityString(m_id,VIBE_DEVCAPTYPE_DEVICE_NAME,
                        VIBE_MAX_CAPABILITY_STRING_LENGTH, szDeviceName)))
        return QString();

    return QString::fromLocal8Bit(szDeviceName);
}

QFeedbackDevice::State QFeedbackDevice::state() const
{
    QFeedbackDevice::State ret = Unknown;
    VibeInt32 s = 0;
    if (m_id >= 0 && VIBE_SUCCEEDED(ImmVibeGetDeviceState(m_id, &s))) {
        if (s == VIBE_DEVICESTATE_ATTACHED)
            ret = QFeedbackDevice::Ready;
        else if (s == VIBE_DEVICESTATE_BUSY)
            ret = QFeedbackDevice:: Busy;
    }

    return ret;

}


bool QFeedbackDevice::isEnabled() const
{
    VibeBool ret = false;
    ImmVibeGetDevicePropertyBool(qHandleForDevice(*this), VIBE_DEVPROPTYPE_DISABLE_EFFECTS, &ret);
    return ret;
}

void QFeedbackDevice::setEnabled(bool enabled)
{
    ImmVibeSetDevicePropertyBool(qHandleForDevice(*this), VIBE_DEVPROPTYPE_DISABLE_EFFECTS, enabled);
}


QFeedbackDevice QFeedbackDevice::defaultDevice(Type t)
{
    qDeviceHandler(); //ensures initialization

    QFeedbackDevice ret;
    ret.m_id = g_defaultDevices[t];
    return ret;
}

QList<QFeedbackDevice> QFeedbackDevice::devices()
{
    qDeviceHandler();  //ensures initialization

    QList<QFeedbackDevice> ret;
    const int nbDev = ImmVibeGetDeviceCount();
    for (int i = 0; i < nbDev; ++i) {
        QFeedbackDevice dev;
        dev.m_id = i;
        ret << dev;
    }

    return ret;
}

QTM_END_NAMESPACE
