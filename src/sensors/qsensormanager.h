/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSENSORMANAGER_H
#define QSENSORMANAGER_H

#include "qsensor.h"
#include <QHash>

QTM_BEGIN_NAMESPACE

class QSensorBackend;
class QSensorBackendFactory;
class QSensorPluginInterface;

typedef QSensorPluginInterface *(*CreatePluginFunc)();

class Q_SENSORS_EXPORT QSensorManager
{
public:
    // Register a backend (call this from a plugin)
    static void registerBackend(const QByteArray &type, const QByteArray &identifier, QSensorBackendFactory *factory);

    // Create a backend (uses the type and identifier set in the sensor)
    static QSensorBackend *createBackend(QSensor *sensor);

    // For static plugins
    static void registerStaticPlugin(CreatePluginFunc func);
};

class Q_SENSORS_EXPORT QSensorBackendFactory
{
public:
    virtual QSensorBackend *createBackend(QSensor *sensor) = 0;
protected:
    ~QSensorBackendFactory() {}
};

#define REGISTER_STATIC_PLUGIN(pluginname)\
    static QSensorPluginInterface *create_static_plugin_ ## pluginname()\
    {\
        return new pluginname;\
    }\
    static bool side_effect_sensor_backend_ ## pluginname ()\
    {\
        QSensorManager::registerStaticPlugin(create_static_plugin_ ## pluginname);\
        return false;\
    }\
    /* This assignment calls the function above */\
    static bool dummy_sensor_backend_ ## pluginname = side_effect_sensor_backend_ ## pluginname();

QTM_END_NAMESPACE

#endif

