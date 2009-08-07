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

#include "qaudiorendererendpoint.h"

#include <private/qobject_p.h>

class QAudioRendererEndpointPrivate : public QObjectPrivate
{
public:
    QAudioRendererEndpointPrivate()
        : frequency(0)
        , channels(1)
        , sampleSize(8)
        , device(0)
    {
    }

    int frequency;
    int channels;
    int sampleSize;
    QIODevice *device;
};

/*!
    \class QAudioRendererEndpoint
    \preliminary
    \brief The QAudioRendererEndpoint class provides a media end point that renders audio to a
    QIODevice

    The interface name of QAudioRendererEndpoint is \c com.nokia.Qt.QAudioRendererEndpoint/1.0 as
    defined in QAudioRendererEndpoint_iid.

    \sa QAbstractMediaService::setAudioOutput()
*/

/*!
    \macro QAudioRendererEndpoint_iid

    \c com.nokia.Qt.QAudioRendererEndpoint/1.0

    Defines the interface name of QAudioRendererEndpoint.

    \relates QAudioRendererEndpoint
*/

/*!
    Constructs a new audio renderer end point with the given \a parent.
*/
QAudioRendererEndpoint::QAudioRendererEndpoint(QObject *parent)
    : QObject(*new QAudioRendererEndpointPrivate, parent)
{
}

/*!
    Destroys an audio renderer end point.
*/
QAudioRendererEndpoint::~QAudioRendererEndpoint()
{
}

/*!
    \reimp
*/
QMediaEndpointInterface::Direction QAudioRendererEndpoint::direction() const
{
    return Output;
}

/*!
    Returns the I/O device audio is rendered to.
*/
QIODevice *QAudioRendererEndpoint::device() const
{
    return d_func()->device;
}

/*!
    Sets the I/O \a device audio is rendered to.
*/
void QAudioRendererEndpoint::setDevice(QIODevice *device)
{
    d_func()->device = device;
}
