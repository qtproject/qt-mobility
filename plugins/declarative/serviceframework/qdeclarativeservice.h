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
***************************************************************************/

#ifndef QDECLARATIVESERVICE_H
#define QDECLARATIVESERVICE_H

#include <QtCore>
#include <qserviceinterfacedescriptor.h>
#include <qservicemanager.h>
#include <qdeclarative.h>
#include <qdeclarativelist.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceInterfaceDescriptor)

class QDeclarativeService : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName);
    Q_PROPERTY(QString serviceName READ serviceName);
    Q_PROPERTY(QString versionNumber READ versionNumber);
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged);
    Q_PROPERTY(QObject* serviceObject READ serviceObject NOTIFY serviceObjectChanged);

public:
    QDeclarativeService();
    ~QDeclarativeService();

    void setInterfaceDesc(const QServiceInterfaceDescriptor& desc);
    QServiceInterfaceDescriptor interfaceDesc() const;

    void setInterfaceName(const QString& interface);
    QString interfaceName() const;
    QString serviceName() const;
    QString versionNumber() const;

    bool isValid() const;
    QObject* serviceObject();

Q_SIGNALS:
    void validChanged();
    void serviceObjectChanged();

private:
    QServiceManager* serviceManager;

    QServiceInterfaceDescriptor m_descriptor;
    QObject* serviceInstance;
};

QML_DECLARE_TYPE(QDeclarativeService)

class QDeclarativeServiceList : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName);
    Q_PROPERTY(QString minVersion READ minVersion WRITE setMinVersion);
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeService> services READ services NOTIFY servicesChanged);

public:
    QDeclarativeServiceList();
    ~QDeclarativeServiceList();

    QDeclarativeListProperty<QDeclarativeService> services();
    
    void setInterfaceName(const QString& interface);
    QString interfaceName() const;

    void setMinVersion(const QString& interface);
    QString minVersion() const;
    
private:
    QServiceManager* serviceManager;
    QList<QDeclarativeService *> m_services;
    QString m_interface;
    QString m_version;

Q_SIGNALS:
    void servicesChanged(const QDeclarativeListProperty<QDeclarativeService>&);
};

QML_DECLARE_TYPE(QDeclarativeServiceList)

#endif

