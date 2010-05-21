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

#include <hwrmvibra.h>
#include <touchfeedback.h>

QTM_BEGIN_NAMESPACE

static bool g_vibraActive = true;

QString QFeedbackDevice::name() const
{
    switch(m_id)
    {
    case Vibra:
        return QLatin1String("Vibra");
    case Touch:
        return QLatin1String("Touch");
    }
    return QString();
}

QFeedbackDevice::State QFeedbackDevice::state() const
{
    QFeedbackDevice::State ret = Unknown;
    switch(m_id)
    {
    case Vibra:
        {
            //TODO we should not allocate the vibra here
            CHWRMVibra *vibra = CHWRMVibra::NewL();
            switch (vibra->VibraStatus())
            {
            case CHWRMVibra::EVibraStatusStopped:
                ret = Ready;
                break;
            case CHWRMVibra::EVibraStatusOn:
                ret = Busy;
                break;
            default:
                break;
            }
            delete vibra;
        }
        break;
    case Touch:
        //there is no way of getting the state of the device!
        break;
    default:
        break;
    }
    return ret;
}

QFeedbackDevice::Capabilities QFeedbackDevice::supportedCapabilities() const
{
    return 0;
}

QFeedbackDevice::Type QFeedbackDevice::Type() const
{
    return QFeedbackDevice::Type(m_id);
}


bool QFeedbackDevice::isEnabled() const
{
    switch(m_id)
    {
    case Vibra:
        return g_vibraActive;
    case Touch:
        return MTouchFeedback::Instance()->FeedbackEnabledForThisApp();
    default:
        return false;
    }
}

void QFeedbackDevice::setEnabled(bool enabled)
{
    switch(m_id)
    {
    case Vibra:
        g_vibraActive = enabled;
        break;
    case Touch:
        MTouchFeedback::Instance()->SetFeedbackEnabledForThisApp(enabled);
        break;
    default:
        break;
    }
}

QFeedbackDevice QFeedbackDevice::defaultDevice(Type t)
{
    QFeedbackDevice ret;
    ret.m_id =  t;
    return ret;
}

QList<QFeedbackDevice> QFeedbackDevice::devices()
{
    QList<QFeedbackDevice> ret;
    QFeedbackDevice fb;
    fb.m_id = Vibra;
    ret << fb;
    if (MTouchFeedback::Instance()->TouchFeedbackSupported()) {
        fb.m_id = Touch;
        ret << fb;
    }
    return ret;
}


QTM_END_NAMESPACE
