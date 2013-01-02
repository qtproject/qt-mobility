/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "qxaaudioendpointselector.h"
#include "qxarecordsession.h"
#include "qxacommon.h"

QXAAudioEndpointSelector::QXAAudioEndpointSelector(QXARecordSession *session, QObject *parent)
:QAudioEndpointSelector(parent), m_session(session)
{
    connect(m_session, SIGNAL(availableAudioInputsChanged()),
        this, SLOT(availableAudioInputsChanged()));
    connect(m_session, SIGNAL(activeEndpointChanged(QString)),
        this, SIGNAL(activeEndpointChanged(QString)));
}

QXAAudioEndpointSelector::~QXAAudioEndpointSelector()
{
    QT_TRACE_FUNCTION_ENTRY_EXIT;
}

QList<QString> QXAAudioEndpointSelector::availableEndpoints() const
{
    if (m_session)
        return m_session->availableEndpoints();
    return QList<QString>();
}

QString QXAAudioEndpointSelector::endpointDescription(const QString &name) const
{
    if (m_session)
        return m_session->endpointDescription(name);
    return QString();
}

QString QXAAudioEndpointSelector::defaultEndpoint() const
{
    if (m_session)
        return m_session->defaultEndpoint();
    return QString();
}

QString QXAAudioEndpointSelector::activeEndpoint() const
{
    if (m_session)
        return m_session->activeEndpoint();
    return QString();
}

void QXAAudioEndpointSelector::setActiveEndpoint(const QString &name)
{
    if (m_session)
        m_session->setActiveEndpoint(name);
}

void QXAAudioEndpointSelector::availableAudioInputsChanged()
    {
    emit availableEndpointsChanged();
    }

