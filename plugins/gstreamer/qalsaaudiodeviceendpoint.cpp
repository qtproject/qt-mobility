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

#include "qalsaaudiodeviceendpoint.h"

#include <QtGui/QIcon>
#include <QtCore/QDebug>

#include <alsa/asoundlib.h>

QAlsaAudioDeviceEndpoint::QAlsaAudioDeviceEndpoint(QObject *parent)
    :QAudioDeviceEndpoint(parent)
{
    update();
}

QAlsaAudioDeviceEndpoint::~QAlsaAudioDeviceEndpoint()
{
}

void QAlsaAudioDeviceEndpoint::setDirectionFilter(QAudioDeviceEndpoint::DeviceDirection direction)
{
    if (direction != directionFilter()) {
        QAudioDeviceEndpoint::setDirectionFilter(direction);
        update();
    }
}

void QAlsaAudioDeviceEndpoint::setRoleFilter(QAudioDeviceEndpoint::Roles roles)
{
    if (roles != roleFilter()) {
        QAudioDeviceEndpoint::setRoleFilter(roles);
        update();
    }
}

void QAlsaAudioDeviceEndpoint::setFormFactorFilter(QAudioDeviceEndpoint::FormFactors forms)
{
    if (forms != formFactorFilter()) {
        QAudioDeviceEndpoint::setFormFactorFilter(forms);
        update();
    }
}

int QAlsaAudioDeviceEndpoint::deviceCount() const
{
    return m_names.count();
}

int QAlsaAudioDeviceEndpoint::direction(int index) const
{
    return m_directions[index];
}

QAudioDeviceEndpoint::Roles QAlsaAudioDeviceEndpoint::roles(int index) const
{
    return m_roles[index];
}

QAudioDeviceEndpoint::FormFactor QAlsaAudioDeviceEndpoint::formFactor(int index) const
{
    return m_formFactors[index];
}

QString QAlsaAudioDeviceEndpoint::name(int index) const
{
    return m_names[index];
}

QString QAlsaAudioDeviceEndpoint::description(int index) const
{
    return m_descriptions[index];
}

QIcon QAlsaAudioDeviceEndpoint::icon(int index) const
{
    Q_UNUSED(index);
    return QIcon();
}

int QAlsaAudioDeviceEndpoint::defaultInputDevice(QAudioDeviceEndpoint::Role role) const
{
    Q_UNUSED(role);
    return 0;
}

int QAlsaAudioDeviceEndpoint::defaultOutputDevice(QAudioDeviceEndpoint::Role role) const
{
    Q_UNUSED(role);
    return 0;
}

void QAlsaAudioDeviceEndpoint::update()
{
    m_names.clear();
    m_descriptions.clear();
    m_directions.clear();
    m_roles.clear();
    m_formFactors.clear();

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
            DeviceDirection direction = OutputDevice;

            if (io == NULL)
                direction = InputOutputDevice;
            else if (qstrcmp(io,"Input"))
                direction = InputDevice;
            else
                direction = OutputDevice;

            if ( direction == InputOutputDevice || directionFilter() == direction ) {
                m_names.append(QString::fromUtf8(name));
                m_descriptions.append(QString::fromUtf8(descr));
                m_directions.append(direction);
                m_roles.append(AllRoles);
                m_formFactors.append(UnknownFormFactor);
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
}
