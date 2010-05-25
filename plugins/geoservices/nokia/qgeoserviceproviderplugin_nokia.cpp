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

#include "qgeoplacesmanager_nokia_p.h"
#include "qgeoroutingmanager_nokia_p.h"
#include "qgeomappingmanager_nokia_p.h"

#include <QtPlugin>
#include <QNetworkProxy>

QGeoServiceProviderPluginNokia::QGeoServiceProviderPluginNokia() {}

QGeoServiceProviderPluginNokia::~QGeoServiceProviderPluginNokia() {}

QString QGeoServiceProviderPluginNokia::providerName() const
{
    return "nokia";
}

/*
bool QGeoServiceProviderPluginNokia::initialize(const QMap<QString, QString> &parameters,
                                                QGeoServiceProvider::Error *error,
                                                QString *errorString)
{
    QList<QString> keys = parameters.keys();
    
    if (m_placesManager)
        delete m_placesManager;

    QGeoPlacesManagerNokia* placesManager = new QGeoPlacesManagerNokia();
    if(keys.contains("places.proxy")) {
        QString proxy = parameters.value("places.proxy");
        if(proxy.isEmpty())
            placesManager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
        else
            placesManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,proxy,8080));
    }
    if(keys.contains("places.host")) {
        QString host = parameters.value("places.host");
        if(!host.isEmpty())
            placesManager->setHost(host);
    }
    m_placesManager = placesManager;

    if (m_routingManager)
        delete m_routingManager;

    QGeoRoutingManagerNokia* routingManager = new QGeoRoutingManagerNokia();
    if(keys.contains("places.proxy")) {
        QString proxy = parameters.value("routing.proxy");
        if(proxy.isEmpty())
            routingManager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
        else
            routingManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,proxy,8080));
    }
    if(keys.contains("places.host")) {
        QString host = parameters.value("routing.host");
        if(!host.isEmpty())
            routingManager->setHost(host);
    }
    
    m_routingManager = routingManager;

    if (m_mappingManager)
        delete m_mappingManager;

    QGeoMappingManagerNokia* mappingManager = new QGeoMappingManagerNokia();
    if(keys.contains("mapping.proxy")) {
        QString proxy = parameters.value("mapping.proxy");
        if(proxy.isEmpty())
            mappingManager->setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
        else
            mappingManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,proxy,8080));
    }
    if(keys.contains("mapping.host")) {
        QString host = parameters.value("mapping.host");
        if(!host.isEmpty())
            mappingManager->setHost(host);
    }
    
    m_mappingManager = mappingManager;

    //TODO: don't get these two assignments
    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";

    return true;
}
*/

QGeoPlacesManager* QGeoServiceProviderPluginNokia::createPlacesManager(const QMap<QString, QString> &parameters,
                                                                       QGeoServiceProvider::Error *error,
                                                                       QString *errorString) const
{
    return new QGeoPlacesManagerNokia(parameters, error, errorString);
}

QGeoMappingManager* QGeoServiceProviderPluginNokia::createMappingManager(const QMap<QString, QString> &parameters,
                                                                         QGeoServiceProvider::Error *error,
                                                                         QString *errorString)const
{
    return new QGeoMappingManagerNokia(parameters, error, errorString);
}

QGeoRoutingManager* QGeoServiceProviderPluginNokia::createRoutingManager(const QMap<QString, QString> &parameters,
                                                                         QGeoServiceProvider::Error *error,
                                                                         QString *errorString) const
{
    return new QGeoRoutingManagerNokia(parameters, error, errorString);
}

Q_EXPORT_PLUGIN2(qtgeoservices_nokia, QGeoServiceProviderPluginNokia)
