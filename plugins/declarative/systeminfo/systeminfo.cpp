/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include "qdeclarativenetworkinfo_p.h"
#include "qdeclarativescreensaver_p.h"
#include "qdeclarativedeviceinfo_p.h"
#include "qdeclarativegeneralinfo_p.h"

#include "qsysteminfo.h"
QT_BEGIN_NAMESPACE

QTM_USE_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeGeneralInfo));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QSystemNetworkInfo));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QSystemDisplayInfo));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeDeviceInfo));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeScreenSaver));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QSystemStorageInfo));
QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeNetworkInfo));

class QSystemInfoDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtMobility.systeminfo"));

        qmlRegisterType<QDeclarativeGeneralInfo>(uri, 1, 1, "GeneralInfo");
        qmlRegisterType<QSystemDisplayInfo>(uri, 1, 1, "DisplayInfo");
        qmlRegisterType<QDeclarativeDeviceInfo>(uri, 1, 1, "DeviceInfo");
        qmlRegisterType<QDeclarativeNetworkInfo>(uri,1, 1, "NetworkInfo");
        qmlRegisterType<QDeclarativeScreenSaver>(uri, 1, 1, "ScreenSaver");
        qmlRegisterType<QSystemStorageInfo>(uri, 1, 1, "StorageInfo");
    }
};


QT_END_NAMESPACE

#include "systeminfo.moc"

Q_EXPORT_PLUGIN2(qsysteminfodeclarativemodule, QT_PREPEND_NAMESPACE(QSystemInfoDeclarativeModule));

