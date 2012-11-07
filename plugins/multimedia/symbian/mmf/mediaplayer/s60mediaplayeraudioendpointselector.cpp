/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60mediaplayercontrol.h"
#include "s60mediaplayersession.h"
#include "s60mediaplayeraudioendpointselector.h"
#include "s60mmtrace.h"

S60MediaPlayerAudioEndpointSelector::S60MediaPlayerAudioEndpointSelector(QObject *control, QObject *parent)
    : QAudioEndpointSelector(parent)
    , m_control(0)
{
    TRACE("S60MediaPlayerAudioEndpointSelector::S60MediaPlayerAudioEndpointSelector" << qtThisPtr());
    m_control = qobject_cast<S60MediaPlayerControl*>(control);
    m_audioEndpointNames.append("Default");
    m_audioEndpointNames.append("All");
    m_audioEndpointNames.append("None");
    m_audioEndpointNames.append("Earphone");
    m_audioEndpointNames.append("Speaker");
}

S60MediaPlayerAudioEndpointSelector::~S60MediaPlayerAudioEndpointSelector()
{
    TRACE("S60MediaPlayerAudioEndpointSelector::~S60MediaPlayerAudioEndpointSelector" << qtThisPtr());
}


QList<QString> S60MediaPlayerAudioEndpointSelector::availableEndpoints() const
{
    return m_audioEndpointNames;
}

QString S60MediaPlayerAudioEndpointSelector::endpointDescription(const QString& name) const
{
    if (name == QString("Default")) // ENoPreference
        return QString("Used to indicate that the playing audio can be routed to"
                       "any speaker. This is the default value for audio.");
    else if (name == QString("All")) // EAll
        return QString("Used to indicate that the playing audio should be routed to all speakers.");
    else if (name == QString("None")) // ENoOutput
        return QString("Used to indicate that the playing audio should not be routed to any output.");
    else if (name == QString("Earphone")) // EPrivate
        return QString("Used to indicate that the playing audio should be routed to"
                       "the default private speaker. A private speaker is one that can only"
                       "be heard by one person.");
    else if (name == QString("Speaker")) // EPublic
        return QString("Used to indicate that the playing audio should be routed to"
                       "the default public speaker. A public speaker is one that can "
                       "be heard by multiple people.");
    return QString();
}

QString S60MediaPlayerAudioEndpointSelector::activeEndpoint() const
{
    if (m_control->session())
        return m_control->session()->activeEndpoint();
    else
        return m_control->mediaControlSettings().audioEndpoint();
}

QString S60MediaPlayerAudioEndpointSelector::defaultEndpoint() const
{
    if (m_control->session())
        return m_control->session()->defaultEndpoint();
    else
        return m_control->mediaControlSettings().audioEndpoint();
}

void S60MediaPlayerAudioEndpointSelector::setActiveEndpoint(const QString &name)
{
    QString oldEndpoint = m_control->mediaControlSettings().audioEndpoint();

    if (name != oldEndpoint &&
           (
              name == QString("Default")
           || name == QString("All")
           || name == QString("None")
           || name == QString("Earphone")
           || name == QString("Speaker")
           )
        ) {
        TRACE("S60MediaPlayerAudioEndpointSelector::setActiveEndpoint" << qtThisPtr()
              << "name" << name);
        if (m_control->session()) {
            m_control->session()->setActiveEndpoint(name);
            emit activeEndpointChanged(name);
        }
        m_control->setAudioEndpoint(name);
    }
}
