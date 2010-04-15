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

#include <ImmVibe.h>


QTM_BEGIN_NAMESPACE

static inline void ensureLibraryInitialized()
{
    if (VIBE_FAILED(ImmVibeInitialize(VIBE_CURRENT_VERSION_NUMBER))) {
        //that should be done once
        //error management
        qWarning("the Immersion library could not be initialized");
    }

}

QString QFeedbackDevice::name() const
{
    char szDeviceName[VIBE_MAX_DEVICE_NAME_LENGTH] = { 0 };
    if (!ImmVibeGetDeviceCapabilityString(m_id,VIBE_DEVCAPTYPE_DEVICE_NAME,
                        VIBE_MAX_CAPABILITY_STRING_LENGTH, szDeviceName))
        return QString();

    return QString::fromLocal8Bit(szDeviceName);
}

QFeedbackDevice::State QFeedbackDevice::state() const
{
    QFeedbackDevice::State ret = Unknown;
    VibeInt32 s = 0;
    if (m_id >= 0 && ImmVibeGetDeviceState(m_id, &s)) {
        if (s == VIBE_DEVICESTATE_ATTACHED)
            ret = QFeedbackDevice::Ready;
        else if (s == VIBE_DEVICESTATE_BUSY)
            ret = QFeedbackDevice:: Busy;
    }

    return ret;

}

int QFeedbackDevice::simultaneousEffect() const
{
    VibeInt32 ret = 0;
    ImmVibeGetDeviceCapabilityInt32(m_id, VIBE_DEVCAPTYPE_NUM_EFFECT_SLOTS, &ret);
    return ret;
}

QFeedbackDevice QFeedbackDevice::defaultDevice(Type /*t*/)
{
    ensureLibraryInitialized();

    //TODO: we don't take the type into consideration here
    QFeedbackDevice ret;
    const int nbDev = ImmVibeGetDeviceCount();
    //the device 0 is the default one (-1 would indicate an error)
    ret.m_id = nbDev > 0 ? 0 : -1;
    return ret;
}

QList<QFeedbackDevice> QFeedbackDevice::devices()
{
    QList<QFeedbackDevice> ret;
    ensureLibraryInitialized();

    const int nbDev = ImmVibeGetDeviceCount();
    for (int i = 0; i < nbDev; ++i) {
        QFeedbackDevice dev;
        dev.m_id = 0;
        ret << dev;
    }

    return ret;
}


QTM_END_NAMESPACE
