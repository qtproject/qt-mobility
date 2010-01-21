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

#include <qsensormanager.h>
#include <QDebug>
#include "qsensorpluginloader_p.h"
#include <qsensorplugin.h>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QSensorPluginLoader, pluginLoader, (QSensorPluginInterface_iid, QLatin1String("/sensors")))

/*!
    \class QSensorManager
    \ingroup sensors_backend

    \preliminary
    \brief The QSensorManager class returns the sensors on a device.

    A given device will have a variety of sensors. The sensors are
    categorized by type. The QSensor::Type enum defined the types that
    the API supports.
*/

/*!
    Returns the sensor manager.
*/
QSensorManager *QSensorManager::instance()
{
    // FIXME should be using Q_GLOBAL_STATIC or something
    static QSensorManager *instance = 0;
    if (!instance)
        instance = new QSensorManager;
    return instance;
}

/*!
    Register a sensor for \a type. The \a identifier must be unique. The \a func must return an instance
    of the appropriate backend class.
*/
void QSensorManager::registerBackend(const QByteArray &type, const QByteArray &identifier, CreateBackendFunc func)
{
    if (!m_backendsByType.contains(type)) {
        qDebug() << "first backend of type" << type << "is" << identifier;
        (void)m_backendsByType[type];
        m_defaultIdentifierForType[type] = identifier;
    }
    m_backendsByType[type][identifier] = func;
    m_allBackends[identifier] = func;
}

/*!
    \internal
*/
void QSensorManager::registerRegisterFunc(RegisterBackendFunc func)
{
    m_staticRegistrations.append(func);
}

/*!
    Create a backend matching \a identifier for \a sensor. Returns null if the identifier is not valid.
*/
QSensorBackend *QSensorManager::createBackend(const QByteArray &identifier, QSensor *sensor)
{
    if (!m_pluginsLoaded)
        loadPlugins();

    if (!m_allBackends.contains(identifier)) {
        qWarning() << "Sensor backend for identifier" << identifier << "does not exist.";
        return 0;
    }

    CreateBackendFunc func = m_allBackends[identifier];
    return func(sensor);
}

/*!
    Returns the first registered sensor identifier for \a type.
*/
QByteArray QSensorManager::firstSensorForType(const QByteArray &type)
{
    if (!m_pluginsLoaded)
        loadPlugins();

    if (m_defaultIdentifierForType.contains(type)) {
        qDebug() << "default for type" << type << "is" << m_defaultIdentifierForType[type];
        return m_defaultIdentifierForType[type];
    }
    qDebug() << "NO default for type" << type;
    return QByteArray();
}

QSensorManager::QSensorManager()
    : m_pluginsLoaded(false)
{
}

void QSensorManager::loadPlugins()
{
    qDebug() << "initializing static plugins";
    m_pluginsLoaded = true;

    foreach (RegisterBackendFunc func, m_staticRegistrations)
        func();

    qDebug() << "initializing plugins";

    foreach (QSensorPluginInterface *plugin, pluginLoader()->plugins()) {
        plugin->registerSensors();
    }
}

/*!
    \macro CREATE_FUNC(classname)
    \relates QSensorManager

    Defines a static function that creates an instance of \a classname.
    \sa {Creating a sensor plugin}
*/

/*!
    \macro REGISTER_STATEMENT(classname, type, identifier)
    \relates QSensorManager

    Registers a sensor plugin using a creation function as defined by CREATE_FUNC().

    Designed to register an instance of \a classname with sensor \a type and unique \a identifier.
    \sa CREATE_FUNC(), {Creating a sensor plugin}
*/

/*!
    \macro REGISTER_LOCAL_SENSOR(classname, type, identifier)
    \relates QSensorManager

    Registers a local sensor plugin.

    Designed to register an instance of \a classname with sensor \a type and unique \a identifier.

    Note that this macro relies on static initialization so it may not be appropriate
    for use in a library.
    \sa {Creating a sensor plugin}
*/

QTM_END_NAMESPACE

