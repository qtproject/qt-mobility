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

#include "qgeoserviceprovider.h"
#include "qgeoserviceprovider_p.h"
#include "qgeoserviceproviderplugin.h"

#include "qgeoplacesmanager.h"
#include "qgeomappingmanager.h"
#include "qgeoroutingmanager.h"

#include <QList>
#include <QString>

#include <QPluginLoader>
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>
#include <QObject>
#include <QProcess>
#include <QEventLoop>

#include "qmobilitypluginsearch.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoServiceProvider
    \brief The QGeoServiceProvider aggregates access to services which provide
    geographical information.
    \ingroup maps

    The Maps and Navigation API allows people to access various kinds of
    geographical information, including functionality to perform geocoding,
    routing and the display of maps.  The QGeoServiceProvider aggregates the
    access to a set of these services that are provided by a single vendor.

    It is possible to mix and match service providers for the various domains,
    so that a geocoding manager from one service provider can be used with
    a geographic routing manager from another service provider.

    This is not recommended unless the client is able to verify that the
    data provided by the different services are compatible, as differences
    in the underlying data sets could cause serious incongruences between
    the services.

    Subclasses of QGeoServiceProvider guarantee that the different services
    that they provide are interoperable.

    At this point only the Nokia Servics plugin is pacakged with Qt Mobility,
    which is accessible using the provider name "nokia".
*/

/*!
    Returns a list of names of the available service providers, for use with
    QGeoServiceProvider::createServiceProvider().
*/
QStringList QGeoServiceProvider::availableServiceProviders()
{
    return QGeoServiceProviderPrivate::plugins().keys();
}

/*!
    Constract a QGeoServiceProvider whose backend has the name \a providerName
    and is constructed with the provided \a parameters.

    TODO if the name is bad use the platform default?

    If no plugin was able to be loaded then error() and errorString() will
    provide details about why this is the case.
*/
QGeoServiceProvider::QGeoServiceProvider(const QString &providerName, const QMap<QString, QString> &parameters)
    : d_ptr(new QGeoServiceProviderPrivate())
{
    d_ptr->loadPlugin(providerName, parameters);
}

/*!
    Destroys the service provider object.
*/
QGeoServiceProvider::~QGeoServiceProvider()
{
    if (d_ptr->plugin)
        delete d_ptr->plugin;

    delete d_ptr;
}

/*!
    Returns the QGeoPlacesManager that is responsible for geocoding and 
    string based landmark search operations made available by the service 
    provider.

    This function will return 0 if the service provider does not provide
    any geocoding services.
*/
QGeoPlacesManager* QGeoServiceProvider::placesManager() const
{
    if (!d_ptr->plugin)
        return 0;

    return d_ptr->plugin->placesManager();
}

/*!
    Returns the QGeoMappingManager that is responsible for the mapping
    operations made available by the service provider.

    This function will return 0 if the service provider does not provide
    any mapping services.
*/
QGeoMappingManager* QGeoServiceProvider::mappingManager() const
{
    if (!d_ptr->plugin)
        return 0;

    return d_ptr->plugin->mappingManager();
}

/*!
    Returns the QGeoRoutingManager that is responsible for the geographic
    routing operations made available by the service provider.

    This function will return 0 if the service provider does not provide
    any geographic routing services.
*/
QGeoRoutingManager* QGeoServiceProvider::routingManager() const
{
    if (!d_ptr->plugin)
        return 0;

    return d_ptr->plugin->routingManager();
}

/*!
*/
QGeoServiceProvider::Error QGeoServiceProvider::error() const
{
    return d_ptr->error;
}

/*!
*/
QString QGeoServiceProvider::errorString() const
{
    return d_ptr->errorString;
}

/*******************************************************************************
*******************************************************************************/

QGeoServiceProviderPrivate::QGeoServiceProviderPrivate()
    : plugin(0),
    error(QGeoServiceProvider::NoError) {}

void QGeoServiceProviderPrivate::loadPlugin(const QString &providerName, const QMap<QString,QString> &parameters)
{
    if (!QGeoServiceProviderPrivate::plugins().keys().contains(providerName)) {
        error = QGeoServiceProvider::NotSupportedError;
        errorString = QString("The geoservices provider %1 is not supported.").arg(providerName);
        plugin = 0;
        return;
    }

    error = QGeoServiceProvider::NoError;
    errorString = "";

    QList<QGeoServiceProviderPlugin*> candidates = QGeoServiceProviderPrivate::plugins().values(providerName);

    for (int i = 0; i < candidates.size(); ++i)
    {
        QGeoServiceProviderPlugin* p = candidates[i];
        if (p) {
            if (p->initialize(parameters, &error, &errorString)) {
                error = QGeoServiceProvider::NoError;
                errorString = "";
                plugin = p;
                break;
            }
        }
    }
}

QHash<QString, QGeoServiceProviderPlugin*> QGeoServiceProviderPrivate::plugins(bool reload)
{
    static QHash<QString, QGeoServiceProviderPlugin*> plugins;
    static bool alreadyDiscovered = false;

    if (reload  == true)
        alreadyDiscovered = false;

    if (!alreadyDiscovered) {
        loadStaticPlugins(&plugins);
        loadDynamicPlugins(&plugins);
        alreadyDiscovered = true;
    }
    return plugins;
}

void QGeoServiceProviderPrivate::loadDynamicPlugins (QHash<QString, QGeoServiceProviderPlugin*> *plugins)
{
    QStringList paths;
    paths << mobilityPlugins(QLatin1String("geoservices"));

    QPluginLoader qpl;
    for (int i=0;i < paths.count(); ++i) {
        qpl.setFileName(paths.at(i));

        QGeoServiceProviderPlugin *p = qobject_cast<QGeoServiceProviderPlugin*>(qpl.instance());
        if (p) {
            QString name = p->providerName();

#if !defined QT_NO_DEBUG
            const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
            if (showDebug)
                qDebug() << "Dynamic: found a service provider plugin with name" << name;
#endif
            plugins->insertMulti(name,p);
        }
    }
}

void QGeoServiceProviderPrivate::loadStaticPlugins (QHash<QString, QGeoServiceProviderPlugin*> *plugins)
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    QObjectList staticPlugins = QPluginLoader::staticInstances();
    for(int i=0; i < staticPlugins.count(); ++i) {
        QGeoServiceProviderPlugin *p = qobject_cast<QGeoServiceProviderPlugin*>(staticPlugins.at(i));
        if (p){
            QString name = p->providerName();

#if !defined QT_NO_DEBUG
            const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
            if (showDebug)
                qDebug() << "Static: found a service provider plugin with name" << name;
#endif
            if(!name.isEmpty()) {
                plugins->insertMulti(name, p);
            }
        }

    }
}

QTM_END_NAMESPACE

