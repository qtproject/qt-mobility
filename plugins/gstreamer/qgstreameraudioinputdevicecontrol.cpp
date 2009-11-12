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

#include "qgstreameraudioinputdevicecontrol.h"

#include <QtGui/QIcon>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#ifdef HAVE_ALSA
#include <alsa/asoundlib.h>
#endif

QGstreamerAudioInputDeviceControl::QGstreamerAudioInputDeviceControl(QObject *parent)
    :QAudioDeviceControl(parent), m_selectedDevice(0)
{
    update();
}

QGstreamerAudioInputDeviceControl::~QGstreamerAudioInputDeviceControl()
{
}

int QGstreamerAudioInputDeviceControl::deviceCount() const
{
    return m_names.size();
}

QString QGstreamerAudioInputDeviceControl::name(int index) const
{
    return m_names[index];
}

QString QGstreamerAudioInputDeviceControl::description(int index) const
{
    return m_descriptions[index];
}

QIcon QGstreamerAudioInputDeviceControl::icon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int QGstreamerAudioInputDeviceControl::defaultDevice() const
{
    return 0;
}

int QGstreamerAudioInputDeviceControl::selectedDevice() const
{
    return m_selectedDevice;
}


void QGstreamerAudioInputDeviceControl::setSelectedDevice(int index)
{
    if (index != m_selectedDevice) {
        m_selectedDevice = index;
        emit selectedDeviceChanged(index);
        emit selectedDeviceChanged(name(index));
    }
}


void QGstreamerAudioInputDeviceControl::update()
{
    m_names.clear();
    m_descriptions.clear();
    updateAlsaDevices();
    updateOssDevices();
}

void QGstreamerAudioInputDeviceControl::updateAlsaDevices()
{
#ifdef HAVE_ALSA
    void **hints, **n;
    if (snd_device_name_hint(-1, "pcm", &hints) < 0) {
        qWarning()<<"no alsa devices available";
        return;
    }
    n = hints;

    while (*n != NULL) {
        char *name = snd_device_name_get_hint(*n, "NAME");
        char *descr = snd_device_name_get_hint(*n, "DESC");
        char *io = snd_device_name_get_hint(*n, "IOID");

        if ((name != NULL) && (descr != NULL)) {
            if ( io == NULL || qstrcmp(io,"Input") == 0 ) {
                m_names.append(QLatin1String("alsa:")+QString::fromUtf8(name));
                m_descriptions.append(QString::fromUtf8(descr));
            }
        }

        if (name != NULL)
            free(name);
        if (descr != NULL)
            free(descr);
        if (io != NULL)
            free(io);
        n++;
    }
    snd_device_name_free_hint(hints);
#endif
}

void QGstreamerAudioInputDeviceControl::updateOssDevices()
{
    QDir devDir("/dev");
    devDir.setFilter(QDir::System);
#ifndef QT_QWS_N810
    QFileInfoList entries = devDir.entryInfoList(QStringList() << "dsp*");
    foreach(const QFileInfo& entryInfo, entries) {
        m_names.append(QLatin1String("oss:")+entryInfo.filePath());
        m_descriptions.append(QString("OSS device %1").arg(entryInfo.fileName()));
    }
#else
    m_names.append("dsppcm");
    m_descriptions.append("PCM audio input");
#endif
}

