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

#include "s60audiocapturesession.h"
#include "s60audiodevicecontrol.h"

#include <QtGui/QIcon>
#include <QtCore/QDebug>

//#include <multimedia/qaudiodeviceinfo.h>


S60AudioDeviceControl::S60AudioDeviceControl(QObject *parent)
    :QAudioDeviceControl(parent)
{
    m_session = qobject_cast<S60AudioCaptureSession*>(parent);

    update();
}

S60AudioDeviceControl::~S60AudioDeviceControl()
{
}

int S60AudioDeviceControl::deviceCount() const
{
    return 1;
}

QString S60AudioDeviceControl::name(int index) const
{
    return QString();
}

QString S60AudioDeviceControl::description(int index) const
{
    return QString();
}

QIcon S60AudioDeviceControl::icon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int S60AudioDeviceControl::defaultDevice() const
{
    return 0;
}

int S60AudioDeviceControl::selectedDevice() const
{
    return 0;
}

void S60AudioDeviceControl::setSelectedDevice(int index)
{
    m_session->setCaptureDevice(QString("MMF"));    //TODO: What this should be in S60 case? There are no special devices as Linux or Windows.
}

void S60AudioDeviceControl::update()
{
    m_names.clear();
    m_descriptions.clear();

    m_names.append(QString("MMF"));
    m_descriptions.append(QString("MMF"));
}
