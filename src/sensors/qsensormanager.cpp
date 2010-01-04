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

QTM_BEGIN_NAMESPACE

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
    Register a sensor for \a type. The \a id must be unique. The \a func must return an instance
    of the appropriate backend class.
*/
void QSensorManager::registerBackend(const QString &type, const QSensorId &id, CreateBackendFunc func)
{
    if (!m_backendsByType.contains(type)) {
        qDebug() << "first backend of type" << type;
        (void)m_backendsByType[type];
    }
    m_backendsByType[type][id] = func;
    m_allBackends[id] = func;
}

/*!
    \internal
*/
void QSensorManager::registerRegisterFunc(RegisterBackendFunc func)
{
    m_staticRegistrations.append(func);
}

/*!
    Create a backend for \a id. Returns null if the ID is not valid.
*/
QSensorBackend *QSensorManager::createBackend(const QSensorId &id)
{
    if (!m_staticPluginsLoaded)
        loadStaticPlugins();

    if (!m_allBackends.contains(id)) {
        qWarning() << "Sensor backend for identifier" << id << "does not exist.";
        return 0;
    }

    CreateBackendFunc func = m_allBackends[id];
    return func();
}

QSensorId QSensorManager::firstSensorForType(const QString &type)
{
    if (!m_staticPluginsLoaded)
        loadStaticPlugins();

    const BackendList &list = m_backendsByType[type];
    BackendList::const_iterator iter = list.constBegin();
    if (iter != list.constEnd()) {
        qDebug() << "default for type" << type << "is" << iter.key();
        return iter.key();
    }
    qDebug() << "NO default for type" << type;
    return QSensorId();
}

QSensorManager::QSensorManager()
    : m_staticPluginsLoaded(false)
{
}

void QSensorManager::loadStaticPlugins()
{
    qDebug() << "initializing static plugins";
    m_staticPluginsLoaded = true;
    foreach (RegisterBackendFunc func, m_staticRegistrations)
        func();
}

QTM_END_NAMESPACE

