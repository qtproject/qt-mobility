/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeExtensionPlugin>

#include <qdeclarativendefrecord.h>

#include "qdeclarativebluetoothdiscoverymodel_p.h"
#include "qdeclarativebluetoothimageprovider_p.h"
#include "qdeclarativebluetoothservice_p.h"
#include "qdeclarativebluetoothsocket_p.h"
#include "qdeclarativenearfieldsocket_p.h"
#include "qdeclarativenearfield_p.h"
#include "qdeclarativendeffilter_p.h"
#include "qdeclarativendeftextrecord_p.h"
#include "qdeclarativendefurirecord_p.h"
#include "qdeclarativendefmimerecord_p.h"

QT_USE_NAMESPACE

QTM_USE_NAMESPACE

class QConnectivityQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.connectivity"));

        qmlRegisterType<QDeclarativeBluetoothDiscoveryModel>(uri, 1, 2, "BluetoothDiscoveryModel");
        qmlRegisterType<QDeclarativeBluetoothService>(uri, 1, 2, "BluetoothService");
        qmlRegisterType<QDeclarativeBluetoothSocket>(uri, 1, 2, "BluetoothSocket");

        qmlRegisterType<QDeclarativeNearFieldSocket>(uri, 1, 2, "NearFieldSocket");

        qmlRegisterType<QDeclarativeNearField>(uri, 1, 2, "NearField");
        qmlRegisterType<QDeclarativeNdefFilter>(uri, 1, 2, "NdefFilter");
        qmlRegisterType<QDeclarativeNdefRecord>(uri, 1, 2, "NdefRecord");
        qmlRegisterType<QDeclarativeNdefTextRecord>(uri, 1, 2, "NdefTextRecord");
        qmlRegisterType<QDeclarativeNdefUriRecord>(uri, 1, 2, "NdefUriRecord");
        qmlRegisterType<QDeclarativeNdefMimeRecord>(uri, 1, 2, "NdefMimeRecord");
    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri) {
        Q_UNUSED(uri);
        engine->addImageProvider("bluetoothicons", new BluetoothThumbnailImageProvider);
    }
};

#include "plugin.moc"

Q_EXPORT_PLUGIN2(qconnectivityqmlplugin, QConnectivityQmlPlugin);
