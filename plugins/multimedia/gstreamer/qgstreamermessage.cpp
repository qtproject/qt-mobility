/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <gst/gst.h>

#include "qgstreamermessage.h"


static int wuchi = qRegisterMetaType<QGstreamerMessage>();


/*!
    \class gstreamer::QGstreamerMessage
    \internal
*/

QGstreamerMessage::QGstreamerMessage():
    m_message(0)
{
}

QGstreamerMessage::QGstreamerMessage(GstMessage* message):
    m_message(message)
{
    gst_message_ref(m_message);
}

QGstreamerMessage::QGstreamerMessage(QGstreamerMessage const& m):
    m_message(m.m_message)
{
    gst_message_ref(m_message);
}


QGstreamerMessage::~QGstreamerMessage()
{
    if (m_message != 0)
        gst_message_unref(m_message);
}

GstMessage* QGstreamerMessage::rawMessage() const
{
    return m_message;
}

QGstreamerMessage& QGstreamerMessage::operator=(QGstreamerMessage const& rhs)
{
    if (m_message != 0)
        gst_message_unref(m_message);

    if ((m_message = rhs.m_message) != 0)
        gst_message_ref(m_message);

    return *this;
}
