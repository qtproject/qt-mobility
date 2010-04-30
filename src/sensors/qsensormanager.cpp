/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsensormanager.h"
#include <QDebug>
#include "qsensorpluginloader_p.h"
#include "qsensorplugin.h"
#include <QSettings>
#include "sensorlog_p.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QSensorPluginLoader, pluginLoader, (QSensorPluginInterface_iid, QLatin1String("/sensors")))

typedef QHash<QByteArray,QSensorBackendFactory*> FactoryForIdentifierMap;
typedef QHash<QByteArray,FactoryForIdentifierMap> BackendIdentifiersForTypeMap;

class QSensorManagerPrivate
{
public:
    QSensorManagerPrivate()
        : pluginsLoaded(false)
    {
    }

    bool pluginsLoaded;

    QList<CreatePluginFunc> staticRegistrations;

    // Holds a mapping from type to available identifiers (and from there to the factory)
    BackendIdentifiersForTypeMap backendsByType;

    // Holds the first identifier for each type
    QHash<QByteArray, QByteArray> firstIdentifierForType;
};

Q_GLOBAL_STATIC(QSensorManagerPrivate, sensorManagerPrivate)

static void loadPlugins()
{
    QSensorManagerPrivate *d = sensorManagerPrivate();
    d->pluginsLoaded = true;

    SENSORLOG() << "initializing static plugins";
    Q_FOREACH (CreatePluginFunc func, d->staticRegistrations) {
        QSensorPluginInterface *plugin = func();
        plugin->registerSensors();
    }

    SENSORLOG() << "initializing plugins";
    Q_FOREACH (QSensorPluginInterface *plugin, pluginLoader()->plugins()) {
        plugin->registerSensors();
    }
}

// =====================================================================

/*!
    \class QSensorManager
    \ingroup sensors_backend

    \preliminary
    \brief The QSensorManager class handles registration and creation of sensor backends.

    Sensor plugins register backends using the registerBackend() function.

    When QSensor::connectToBackend() is called, the createBackend() function will be called.
*/

/*!
    Register a sensor for \a type. The \a identifier must be unique.

    The \a factory will be asked to create instances of the backend.
*/
void QSensorManager::registerBackend(const QByteArray &type, const QByteArray &identifier, QSensorBackendFactory *factory)
{
    QSensorManagerPrivate *d = sensorManagerPrivate();
    if (!d->backendsByType.contains(type)) {
        (void)d->backendsByType[type];
        d->firstIdentifierForType[type] = identifier;
    }
    SENSORLOG() << "registering backend for type" << type << "identifier" << identifier;// << "factory" << QString().sprintf("0x%08x", (unsigned int)factory);
    FactoryForIdentifierMap &factoryByIdentifier = d->backendsByType[type];
    factoryByIdentifier[identifier] = factory;
}

/*!
    \internal
*/
void QSensorManager::registerStaticPlugin(CreatePluginFunc func)
{
    QSensorManagerPrivate *d = sensorManagerPrivate();
    d->staticRegistrations.append(func);
}

/*!
    Create a backend for \a sensor. Returns null if no suitable backend exists.
*/
QSensorBackend *QSensorManager::createBackend(QSensor *sensor)
{
    Q_ASSERT(sensor);

    QSensorManagerPrivate *d = sensorManagerPrivate();
    if (!d->pluginsLoaded)
        loadPlugins();

    SENSORLOG() << "QSensorManager::createBackend" << "type" << sensor->type() << "identifier" << sensor->identifier();

    if (!d->backendsByType.contains(sensor->type())) {
        SENSORLOG() << "no backends of type" << sensor->type() << "have been registered.";
        return 0;
    }

    const FactoryForIdentifierMap &factoryByIdentifier = d->backendsByType[sensor->type()];
    QSensorBackendFactory *factory;
    QSensorBackend *backend;

    if (sensor->identifier().isEmpty()) {
        QByteArray defaultIdentifier = QSensor::defaultSensorForType(sensor->type());
        SENSORLOG() << "Trying the default" << defaultIdentifier;
        // No identifier set, try the default
        factory = factoryByIdentifier[defaultIdentifier];
        //SENSORLOG() << "factory" << QString().sprintf("0x%08x", (unsigned int)factory);
        sensor->setIdentifier(defaultIdentifier); // the factory requires this
        backend = factory->createBackend(sensor);
        if (backend) goto gotbackend; // Got it!

        // The default failed to instantiate so try any other registered sensors for this type
        Q_FOREACH (const QByteArray &identifier, factoryByIdentifier.keys()) {
            SENSORLOG() << "Trying" << identifier;
            if (identifier == defaultIdentifier) continue; // Don't do the default one again
            factory = factoryByIdentifier[identifier];
            //SENSORLOG() << "factory" << QString().sprintf("0x%08x", (unsigned int)factory);
            sensor->setIdentifier(identifier); // the factory requires this
            backend = factory->createBackend(sensor);
            if (backend) goto gotbackend; // Got it!
        }
        SENSORLOG() << "FAILED";
        sensor->setIdentifier(QByteArray()); // clear the identifier
    } else {
        if (!factoryByIdentifier.contains(sensor->identifier())) {
            SENSORLOG() << "no backend with identifier" << sensor->identifier() << "for type" << sensor->type();
            return 0;
        }

        // We were given an explicit identifier so don't substitute other backends if it fails to instantiate
        factory = factoryByIdentifier[sensor->identifier()];
        //SENSORLOG() << "factory" << QString().sprintf("0x%08x", (unsigned int)factory);
        backend = factory->createBackend(sensor);
        if (backend) goto gotbackend; // Got it!
    }

    SENSORLOG() << "no suitable backend found for requested identifier" << sensor->identifier() << "and type" << sensor->type();
    return 0;

gotbackend:
    if (sensor->availableDataRates().count() == 0) {
        qWarning() << sensor->identifier() << "backend does not support any data rates. It cannot be used.";
    }
    if (sensor->dataRate() == 0) {
        qWarning() << sensor->identifier() << "backend did not supply default data rate.";
    }
    return backend;
}

// =====================================================================

/*!
    Returns a list of all sensor types.
*/
QList<QByteArray> QSensor::sensorTypes()
{
    QSensorManagerPrivate *d = sensorManagerPrivate();
    if (!d->pluginsLoaded)
        loadPlugins();

    return d->backendsByType.keys();
}

/*!
    Returns a list of ids for each of the sensors for \a type.
    If there are no sensors of that type available the list will be empty.
*/
QList<QByteArray> QSensor::sensorsForType(const QByteArray &type)
{
    QSensorManagerPrivate *d = sensorManagerPrivate();
    if (!d->pluginsLoaded)
        loadPlugins();

    // no sensors of that type exist
    if (!d->backendsByType.contains(type))
        return QList<QByteArray>();

    return d->backendsByType[type].keys();
}

/*!
    Returns the default sensor identifier for \a type.
    This is set in a config file and can be overridden if required.
    If no default is available the system will return the first registered
    sensor for \a type.
*/
QByteArray QSensor::defaultSensorForType(const QByteArray &type)
{
    QSensorManagerPrivate *d = sensorManagerPrivate();
    if (!d->pluginsLoaded)
        loadPlugins();

    // no sensors of that type exist
    if (!d->backendsByType.contains(type))
        return QByteArray();

    QSettings settings(QLatin1String("Nokia"), QLatin1String("Sensors"));
    QVariant value = settings.value(QString(QLatin1String("Default/%1")).arg(QString::fromLatin1(type)));
    if (!value.isNull()) {
        QByteArray defaultIdentifier = value.toByteArray();
        if (d->backendsByType[type].contains(defaultIdentifier)) // Don't return a value that we can't use!
            return defaultIdentifier;
    }

    // This is our fallback
    return d->firstIdentifierForType[type];
}

// =====================================================================

/*!
    \class QSensorBackendFactory
    \ingroup sensors_backend

    \preliminary
    \brief The QSensorBackendFactory class instantiates instances of
           QSensorBackend.

    This interface must be implemented in order to register a sensor backend.

    \sa {Creating a sensor plugin}
*/

/*!
    \fn QSensorBackendFactory::~QSensorBackendFactory()
    \internal
*/

/*!
    \fn QSensorBackendFactory::createBackend(QSensor *sensor)

    Instantiate a backend. If the factory handles multiple identifiers
    it should check with the \a sensor to see which one is requested.

    If the factory cannot create a backend it should return 0.
*/

/*!
    \macro REGISTER_STATIC_PLUGIN(pluginname)
    \relates QSensorManager

    Registers a static plugin, \a pluginname.

    Note that this macro relies on static initialization so it may not be appropriate
    for use in a library and may not work on all platforms.

    \sa {Creating a sensor plugin}
*/

QTM_END_NAMESPACE

