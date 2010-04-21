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

#include "qvideooutputcontrol.h"

QT_BEGIN_NAMESPACE

/*!
    \class QVideoOutputControl
    \preliminary

    \brief The QVideoOutputControl class provides a means of selecting the
    active video output control.

    \ingroup multimedia-serv

    There are multiple controls which a QMediaService may use to output
    video ony one of which may be active at one time, QVideoOutputControl
    is the means by which this active control is selected.

    The possible output controls are QVideoRendererControl,
    QVideoWindowControl, and QVideoWidgetControl.

    The interface name of QVideoOutputControl is \c com.nokia.Qt.QVideoOutputControl/1.0 as
    defined in QVideoOutputControl_iid.

    \sa QMediaService::control(), QVideoWidget, QVideoRendererControl,
    QVideoWindowControl, QVideoWidgetControl
*/

/*!
    \macro QVideoOutputControl_iid

    \c com.nokia.Qt.QVideoOutputControl/1.0

    Defines the interface name of the QVideoOutputControl class.

    \relates QVideoOutputControl
*/

/*!
    \enum QVideoOutputControl::Output

    Identifies the possible render targets of a video output.

    \value NoOutput Video is not rendered.
    \value WindowOutput Video is rendered to the target of a QVideoWindowControl.
    \value RendererOutput Video is rendered to the target of a QVideoRendererControl.
    \value WidgetOutput Video is rendered to a QWidget provided by QVideoWidgetControl.
    \value UserOutput Start value for user defined video targets.
    \value MaxUserOutput End value for user defined video targets.
*/

/*!
    Constructs a new video output control with the given \a parent.
*/
QVideoOutputControl::QVideoOutputControl(QObject *parent)
    : QMediaControl(parent)
{
}

/*!
    Destroys a video output control.
*/
QVideoOutputControl::~QVideoOutputControl()
{
}

/*!
    \fn QList<QVideoOutputControl::Output> QVideoOutputControl::availableOutputs() const

    Returns a list of available video output targets.
*/

/*!
    \fn QVideoOutputControl::output() const

    Returns the current video output target.
*/

/*!
    \fn QVideoOutputControl::setOutput(Output output)

    Sets the current video \a output target.
*/

/*!
    \fn QVideoOutputControl::availableOutputsChanged(const QList<QVideoOutputControl::Output>  &outputs)

    Signals that available set of video \a outputs has changed.
*/

#include "moc_qvideooutputcontrol.cpp"
QT_END_NAMESPACE

