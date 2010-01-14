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

#ifndef QSENSORMANAGER_H
#define QSENSORMANAGER_H

#include <qsensor.h>
#include <QHash>

QTM_BEGIN_NAMESPACE

class QSensorBackend;
typedef QSensorBackend *(*CreateBackendFunc)();
typedef void(*RegisterBackendFunc)();
typedef QHash<QByteArray,CreateBackendFunc> BackendList;
typedef QHash<QString, BackendList> BackendTypeList;

class Q_SENSORS_EXPORT QSensorManager
{
public:
    // Get the singleton instance
    static QSensorManager *instance();

    void registerBackend(const QString &type, const QByteArray &identifier, CreateBackendFunc func);
    void registerRegisterFunc(RegisterBackendFunc func);
    QSensorBackend *createBackend(const QByteArray &identifier);
    QByteArray firstSensorForType(const QString &type);
private:
    QSensorManager();
    void loadPlugins();
    BackendList m_allBackends;
    BackendTypeList m_backendsByType;
    bool m_pluginsLoaded;
    QList<RegisterBackendFunc> m_staticRegistrations;
};

#define CREATE_FUNC(classname)\
    /* This generated function creates a new instance of the backend */\
    static QSensorBackend *create_sensor_backend_ ## classname ()\
    {\
        return new classname();\
    }

#define REGISTER_STATEMENT(classname, type, identifier)\
    QSensorManager::instance()->registerBackend(type, identifier, create_sensor_backend_ ## classname )\

#define REGISTER_FUNC(classname, type, identifier)\
    /* This function registers the backend */\
    static void register_sensor_backend_ ## classname ()\
    {\
        REGISTER_STATEMENT(classname, type, identifier);\
    }

#define REGISTER_LOCAL_SENSOR(classname, type, identifier)\
    CREATE_FUNC(classname)\
    REGISTER_FUNC(clasaname, type, identifier)\
    /* This function schedules the above for running */\
    static bool side_effect_sensor_backend_ ## classname ()\
    {\
        QSensorManager::instance()->registerRegisterFunc(register_sensor_backend_ ## classname );\
        return false;\
    }\
    /* This assignment calls the function above */\
    static bool dummy_sensor_backend_ ## classname = side_effect_sensor_backend_ ## classname();

QTM_END_NAMESPACE

#endif

