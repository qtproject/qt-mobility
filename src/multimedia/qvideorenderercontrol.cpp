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

#include "qvideorenderercontrol.h"

#include "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QVideoRendererControl
    \preliminary

    \brief The QVideoRendererControl class provides a control for rendering
    to a video surface.

    \ingroup multimedia-serv

    Using the surface() property of QVideoRendererControl a QAbstractVideoSurface
    may be set as the video render target of a QMediaService.

    \code
    QVideoRendererControl *rendererControl = mediaService->control<QVideoRendererControl *>();
    rendererControl->setSurface(myVideoSurface);
    \endcode

    QVideoRendererControl is one of number of possible video output controls,
    in order to receive video it must be made the active video output
    control by setting the output property of QVideoOutputControl to
    \l {QVideoOutputControl::RendererOutput}{RendererOutput}. Consequently any
    QMediaService that implements QVideoRendererControl must also implement
    QVideoOutputControl.

    \code
    QVideoOutputControl *outputControl = mediaService->control<QVideoOutputControl *>();
    outputControl->setOutput(QVideoOutputControl::RendererOutput);
    \endcode

    The interface name of QVideoRendererControl is \c com.nokia.Qt.QVideoRendererControl/1.0 as
    defined in QVideoRendererControl_iid.

    \sa QMediaService::control(), QVideoOutputControl, QVideoWidget
*/

/*!
    \macro QVideoRendererControl_iid

    \c com.nokia.Qt.QVideoRendererControl/1.0

    Defines the interface name of the QVideoRendererControl class.

    \relates QVideoRendererControl
*/

/*!
    Constructs a new video renderer media end point with the given \a parent.
*/
QVideoRendererControl::QVideoRendererControl(QObject *parent)
    : QMediaControl(parent)
{
}

/*!
    Destroys a video renderer media end point.
*/
QVideoRendererControl::~QVideoRendererControl()
{
}

/*!
    \fn QVideoRendererControl::surface() const

    Returns the surface a video producer renders to.
*/

/*!
    \fn QVideoRendererControl::setSurface(QAbstractVideoSurface *surface)

    Sets the \a surface a video producer renders to.
*/

#include "moc_qvideorenderercontrol.cpp"
QT_END_NAMESPACE

