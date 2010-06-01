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

#include "qgeoplacesmanager.h"
#include "qgeoroutingmanager.h"
#include "qgeomappingmanager.h"

#include "qgeoplacesmanagerengine_nokia_p.h"
#include "qgeoroutingmanagerengine_nokia_p.h"
#include "qgeomappingmanagerengine_nokia_p.h"

#include <QtPlugin>
#include <QNetworkProxy>

QGeoServiceProviderPluginNokia::QGeoServiceProviderPluginNokia() {}

QGeoServiceProviderPluginNokia::~QGeoServiceProviderPluginNokia() {}

QString QGeoServiceProviderPluginNokia::providerName() const
{
    return "nokia";
}

QGeoPlacesManager* QGeoServiceProviderPluginNokia::createPlacesManager(const QMap<QString, QString> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    QGeoPlacesManagerEngineNokia *engine = new QGeoPlacesManagerEngineNokia(parameters, error, errorString);
    return new QGeoPlacesManager(engine);
}

QGeoMappingManager* QGeoServiceProviderPluginNokia::createMappingManager(const QMap<QString, QString> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString)const
{
    QGeoMappingManagerEngineNokia *engine = new QGeoMappingManagerEngineNokia(parameters, error, errorString);
    return new QGeoMappingManager(engine);
}

QGeoRoutingManager* QGeoServiceProviderPluginNokia::createRoutingManager(const QMap<QString, QString> &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    QGeoRoutingManagerEngineNokia *engine = new QGeoRoutingManagerEngineNokia(parameters, error, errorString);
    return new QGeoRoutingManager(engine);
}

Q_EXPORT_PLUGIN2(qtgeoservices_nokia, QGeoServiceProviderPluginNokia)
