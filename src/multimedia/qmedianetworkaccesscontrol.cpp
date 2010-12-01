/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmedianetworkaccesscontrol.h"

QT_BEGIN_NAMESPACE

/*!
    \class QMediaNetworkAccessControl
    \preliminary
    \brief The QMediaNetworkAccessControl is used to allow the setting of the Network Access Point for media related activities.
    \ingroup multimedia

    The functionality provided by this control allows the
    setting of a Network Access Point.

    This control can be used to set a network access for various
    network related activities. the exact nature in dependant on the underlying
    usage by the supported QMediaObject

    \sa QMediaServices::requestControl()
*/

QMediaNetworkAccessControl::QMediaNetworkAccessControl(QObject *parent) :
    QMediaControl(parent)
{
}

/*!
    Destroys a network access control.
*/
QMediaNetworkAccessControl::~QMediaNetworkAccessControl()
{
}

/*!
    \fn void QMediaNetworkAccessControl::setConfigurations(const QList<QNetworkConfiguration> &configurations);

    \a configurations contains a list of network configurations to be used for network access.

    It is assumed the list is sorted in highest to lowest preference order.
    By calling this function all previous configurations will be invalidated.

    Note: All Network Configurations should be in QNetworkConfiguration::Discovered state to be usable
    Any configurations not in the Discovered state are ignored since they should be immediatly available for
    use.
*/

/*
    \fn QNetworkConfiguration QMediaNetworkAccessControl::currentConfiguration() const

    Returns the current active configuration in use.
    Can potentially return the backend default configuration.
    However this is not guaranteed.
    A default constructed QNetworkConfoguration is returned if
    no user supplied configurations are used.
*/


/*!
    \fn QNetworkConfiguration::configurationChanged(const QNetworkConfiguration & configuration)
    This signal is emited when the current active configuration changes to \a configuration.
*/



#include "moc_qmedianetworkaccesscontrol.cpp"
QT_END_NAMESPACE
