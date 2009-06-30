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

#include "qwidgetmediaoutput_p.h"

/*!
    \class QWidgetMediaOutputInterface
    \preliminary
    \internal
    \brief The QWidgetMediaOutputInterface class provides an interface for widget media outputs.
*/

/*!
*/
QWidgetMediaOutputInterface::~QWidgetMediaOutputInterface()
{
}

/*!
    \class QWidgetMediaOutput
    \preliminary
    \brief The QWidgetMediaOutput class provides a QWidget media output.

    \note QWidgetMediaOutput must be created by a media object and cannot be instantiated
    directly.
*/

/*!
    Constructs a new media output widget with the given \a parent.
*/
QWidgetMediaOutput::QWidgetMediaOutput(QWidget *parent)
    : QWidget(*new QWidgetMediaOutputPrivate, parent, 0)
{
}

/*!
    \internal
*/
QWidgetMediaOutput::QWidgetMediaOutput(QWidgetMediaOutputPrivate &dd, QWidget *parent)
    : QWidget(dd, parent, 0)
{
}

/*!
    Destroys a media output widget.
*/
QWidgetMediaOutput::~QWidgetMediaOutput()
{
}

/*!
    Identifies if a media output widget is in full screen mode.

    Returns true if the widget is full screen, and false otherwise.
*/
bool QWidgetMediaOutput::isFullscreen() const
{
    return d_func()->fullscreen;
}

/*!
    Sets the \a fullscreen mode of a media output widget.
*/
void QWidgetMediaOutput::setFullscreen(bool fullscreen)
{
    d_func()->fullscreen = fullscreen;
}
