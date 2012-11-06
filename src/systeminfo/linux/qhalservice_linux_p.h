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

#ifndef QHALSERVICE_H
#define QHALSERVICE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#if !defined(QT_NO_HAL)

#include <qmobilityglobal.h>

#include <QtCore/qvariant.h>
#include <QtDBus/QtDBus>

QTM_BEGIN_NAMESPACE

class QHalInterfacePrivate;
class QHalInterface : public QObject
{
    Q_OBJECT

public:
    QHalInterface(QObject *parent = 0);
    ~QHalInterface();

    bool isValid();
    QStringList findDeviceByCapability(const QString &cap);
    QStringList getAllDevices();
    
private:
    QHalInterfacePrivate *d;
};

class QHalDeviceInterfacePrivate;
class QHalDeviceInterface : public QObject
{
    Q_OBJECT

public:
    QHalDeviceInterface(const QString &devicePathName, QObject *parent = 0);
    ~QHalDeviceInterface();

    bool isValid();
    bool getPropertyBool(const QString &prop);
    QString getPropertyString(const QString &prop);
    qint32 getPropertyInt(const QString &prop);

    bool setConnections();

Q_SIGNALS:
    void propertyModified(int,  QVariantList);

private:
    QHalDeviceInterfacePrivate *d;
};

class QHalDeviceLaptopPanelInterfacePrivate;
class QHalDeviceLaptopPanelInterface : public QObject
{
    Q_OBJECT

public:
    QHalDeviceLaptopPanelInterface(const QString &devicePathName, QObject *parent = 0);
    ~QHalDeviceLaptopPanelInterface();

    quint32 getBrightness();

private:
    QHalDeviceLaptopPanelInterfacePrivate *d;
};

QTM_END_NAMESPACE

#endif // QT_NO_HAL

#endif // QHALSERVICE_H
