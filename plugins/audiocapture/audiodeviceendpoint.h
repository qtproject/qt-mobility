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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef AUDIODEVICEENDPOINT_H
#define AUDIODEVICEENDPOINT_H

#include "qaudiodeviceendpoint.h"
#include <QStringList>

class AudioDeviceEndpoint : public QAudioDeviceEndpoint
{
Q_OBJECT
public:
    AudioDeviceEndpoint(QObject *parent);
    virtual ~AudioDeviceEndpoint();

    void setDirectionFilter(DeviceDirection direction);
    void setRoleFilter(Roles roles);
    void setFormFactorFilter(FormFactors forms);

    int deviceCount() const;

    int direction(int index) const;
    Roles roles(int index) const;
    FormFactor formFactor(int index) const;

    QString name(int index) const;
    QString description(int index) const;
    QIcon icon(int index) const;

    int defaultInputDevice(Role role) const;
    int defaultOutputDevice(Role role) const;

private:
    void update();

    QStringList m_names;
    QStringList m_descriptions;
    QList<int> m_directions;
    QList<Roles> m_roles;
    QList<FormFactor> m_formFactors;
};

#endif // AUDIODEVICEENDPOINT_H
