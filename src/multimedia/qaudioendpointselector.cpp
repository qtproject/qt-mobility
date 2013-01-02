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

#include "qaudioendpointselector.h"

QT_BEGIN_NAMESPACE

/*!
    \class QAudioEndpointSelector

    \brief The QAudioEndpointSelector class provides an audio endpoint selector media control.
    \inmodule QtMultimediaKit
    \ingroup multimedia
    \since 1.0

    The QAudioEndpointSelector class provides descriptions of the audio
    endpoints available on a system and allows one to be selected as the audio
    of a media service.

    The interface name of QAudioEndpointSelector is \c com.nokia.Qt.QAudioEndpointSelector/1.0 as
    defined in QAudioEndpointSelector_iid.

    \sa QMediaService::requestControl()
*/

/*!
    \macro QAudioEndpointSelector_iid

    \c com.nokia.Qt.QAudioEndpointSelector/1.0

    Defines the interface name of the QAudioEndpointSelector class.

    \relates QAudioEndpointSelector
*/

/*!
    Constructs a new audio endpoint selector with the given \a parent.
*/
QAudioEndpointSelector::QAudioEndpointSelector(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
    Destroys an audio endpoint selector.
*/
QAudioEndpointSelector::~QAudioEndpointSelector()
{
}

/*!
    \fn QList<QString> QAudioEndpointSelector::availableEndpoints() const

    Returns a list of available audio endpoints.
    \since 1.0
*/

/*!
    \fn QString QAudioEndpointSelector::endpointDescription(const QString& name) const

    Returns the description of the endpoint \a name.
    \since 1.0
*/

/*!
    \fn QString QAudioEndpointSelector::defaultEndpoint() const

    Returns the name of the default audio endpoint.
    \since 1.0
*/

/*!
    \fn QString QAudioEndpointSelector::activeEndpoint() const

    Returns the name of the currently selected audio endpoint.
    \since 1.0
*/

/*!
    \fn QAudioEndpointSelector::setActiveEndpoint(const QString& name)

    Set the audio endpoint to \a name.
    \since 1.0
*/

/*!
    \fn QAudioEndpointSelector::activeEndpointChanged(const QString& name)

    Signals that the audio endpoint has changed to \a name.
    \since 1.0
*/

/*!
    \fn QAudioEndpointSelector::availableEndpointsChanged()

    Signals that list of available endpoints has changed.
    \since 1.0
*/

#include "moc_qaudioendpointselector.cpp"
QT_END_NAMESPACE

