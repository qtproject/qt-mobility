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

#include "qgeoserviceproviderplugin_nokia_p.h"

#include "qgeocodingmanager_nokia_p.h"
#include "qgeoroutingmanager_nokia_p.h"

#include <QtPlugin>

QGeoServiceProviderPluginNokia::QGeoServiceProviderPluginNokia()
    : m_geocodingManager(0),
    m_mappingManager(0),
    m_routingManager(0),
    m_landmarkManager(0) {}

QGeoServiceProviderPluginNokia::~QGeoServiceProviderPluginNokia()
{
    if (m_geocodingManager)
        delete m_geocodingManager;

    if (m_mappingManager)
        delete m_mappingManager;

    if (m_routingManager)
        delete m_routingManager;

    if (m_landmarkManager)
        delete m_landmarkManager;
}

QString QGeoServiceProviderPluginNokia::providerName() const
{
    return "nokia";
}

bool QGeoServiceProviderPluginNokia::initialize(const QMap<QString, QString> &parameters,
                                                QGeoServiceProvider::Error *error,
                                                QString *errorString)
{
    if (m_geocodingManager)
        delete m_geocodingManager;

    m_geocodingManager = new QGeoCodingManagerNokia();

    if (m_routingManager)
        delete m_routingManager;

    m_routingManager = new QGeoRoutingManagerNokia();


    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";

    return true;
}

QGeoCodingManager* QGeoServiceProviderPluginNokia::geocodingManager() const
{
    return m_geocodingManager;
}

QGeoMappingManager* QGeoServiceProviderPluginNokia::mappingManager() const
{
    return m_mappingManager;
}

QGeoRoutingManager* QGeoServiceProviderPluginNokia::routingManager() const
{
    return m_routingManager;
}

QLandmarkManager* QGeoServiceProviderPluginNokia::landmarkManager() const
{
    return m_landmarkManager;
}

Q_EXPORT_PLUGIN2(qtgeoservices_nokia, QGeoServiceProviderPluginNokia)
