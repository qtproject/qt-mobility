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
#include "s60audioendpointselector.h"

#include <QtGui/QIcon>
#include <QtCore/QDebug>


S60AudioEndpointSelector::S60AudioEndpointSelector(QObject *session, QObject *parent)
    :QAudioEndpointSelector(parent)
{    
    m_session = qobject_cast<S60AudioCaptureSession*>(session); 
    update();    
    m_audioInput = defaultEndpoint();
}

S60AudioEndpointSelector::~S60AudioEndpointSelector()
{
}

QList<QString> S60AudioEndpointSelector::availableEndpoints() const
{
    return m_names;
}

QString S60AudioEndpointSelector::endpointDescription(const QString& name) const
{
    QString desc;

    for(int i = 0; i < m_names.count(); i++) {
        if (m_names.at(i).compare(name) == 0) {
            desc = m_descriptions.at(i);
            break;
        }
    }
    return desc;
}

QString S60AudioEndpointSelector::defaultEndpoint() const
{
    if (m_names.size() > 0)
        return m_names.at(0);
    return QString();
}

QString S60AudioEndpointSelector::activeEndpoint() const
{
    return m_audioInput;
}

void S60AudioEndpointSelector::setActiveEndpoint(const QString& name)
{
    if (m_audioInput.compare(name) != 0) {
        m_audioInput = name;    
        m_session->setCaptureDevice(name);
        emit activeEndpointChanged(name);
    }
}

void S60AudioEndpointSelector::update()
{
    m_names.clear();
    m_descriptions.clear();

    m_names.append(QString("MMF"));
    m_descriptions.append(QString("MMF"));
}
