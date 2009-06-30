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

#include "qvideorenderermediaoutput.h"

#include <private/qobject_p.h>

#ifndef QT_NO_VIDEOSURFACE

/*!
    \class QVideoRendererMediaOutputInterface
    \preliminary
    \internal
    \brief The QVideoRendererMediaOutputInterface class provides an interface for video renderer
    media outputs.
*/

/*!
    Destroys a video renderer media output.
*/
QVideoRendererMediaOutputInterface::~QVideoRendererMediaOutputInterface()
{
}

class QVideoRendererMediaOutputPrivate : public QObjectPrivate
{
public:
    QVideoRendererMediaOutputPrivate()
        : surface(0)
    {
    }

    QAbstractVideoSurface *surface;
};

/*!
    \class QVideoRendererMediaOutput
    \preliminary
    \brief The QVideoRendererMediaOutput class provides a media output that renders to a video
    surface.

    \note QVideoRendererMediaOutput must be created by a media object and cannot be instantiated
    directly.
*/

/*!
    Constructs a new video renderer media output.
*/
QVideoRendererMediaOutput::QVideoRendererMediaOutput(QObject *parent)
    : QObject(*new QVideoRendererMediaOutputPrivate, parent)
{
}

/*!
    Destroys a video renderer media output.
*/
QVideoRendererMediaOutput::~QVideoRendererMediaOutput()
{
}

/*!
    Returns the video surface a renderer renders to.
*/
QAbstractVideoSurface *QVideoRendererMediaOutput::surface() const
{
    return d_func()->surface;
}

/*!
    Sets the video \a surface a renderer renders to.
*/
void QVideoRendererMediaOutput::setSurface(QAbstractVideoSurface *surface)
{
    d_func()->surface = surface;
}

#endif
