/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
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

QTM_BEGIN_NAMESPACE

class QDeclarativeService : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName NOTIFY interfaceNameChanged)
    Q_PROPERTY(QString serviceName READ serviceName NOTIFY serviceNameChanged)
    Q_PROPERTY(int majorVersion READ majorVersion NOTIFY majorVersionChanged)
    Q_PROPERTY(int minorVersion READ minorVersion NOTIFY minorVersionChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(QObject* serviceObject READ serviceObject NOTIFY serviceObjectChanged)

public:
    QDeclarativeService();
    ~QDeclarativeService();

    void setInterfaceDesc(const QServiceInterfaceDescriptor& desc);
    QServiceInterfaceDescriptor interfaceDesc() const;

    void setInterfaceName(const QString& interface);
    QString interfaceName() const;
    QString serviceName() const;
    int majorVersion() const;
    int minorVersion() const;

    bool isValid() const;
    QObject* serviceObject();

Q_SIGNALS:
    void validChanged();
    void serviceObjectChanged();
    void interfaceNameChanged();
    void serviceNameChanged();
    void majorVersionChanged();
    void minorVersionChanged();

private:
    QObject* serviceInstance;
    QServiceManager* serviceManager;
    QServiceInterfaceDescriptor m_descriptor;
};


class QDeclarativeServiceList : public QObject, public QDeclarativeParserStatus {
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)
    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName NOTIFY interfaceNameChanged)
    Q_PROPERTY(int majorVersion READ majorVersion WRITE setMajorVersion NOTIFY majorVersionChanged)
    Q_PROPERTY(int minorVersion READ minorVersion WRITE setMinorVersion NOTIFY minorVersionChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeService> services READ services NOTIFY resultsChanged)

    Q_PROPERTY(MatchRule versionMatch READ versionMatch WRITE setVersionMatch NOTIFY versionMatchChanged)
    Q_ENUMS(MatchRule)

public:
    enum MatchRule {
        Minimum = 0,
        Exact
    };

    QDeclarativeServiceList();
    ~QDeclarativeServiceList();

    QDeclarativeListProperty<QDeclarativeService> services();
    
    void setServiceName(const QString& service);
    QString serviceName() const;
    
    void setInterfaceName(const QString& interface);
    QString interfaceName() const;

    void setMinorVersion(int minor);
    int minorVersion() const;
    
    void setMajorVersion(int major);
    int majorVersion() const;
    
    void setVersionMatch(QDeclarativeServiceList::MatchRule match);
    QDeclarativeServiceList::MatchRule versionMatch() const;

    void listUpdated();

    //Derived from QDeclarativeParserStatus
    virtual void classBegin();
    virtual void componentComplete();
    
private:
    QList<QDeclarativeService *> m_services;
    QServiceManager* serviceManager;
    QString m_service;
    QString m_interface;
    int m_major;
    int m_minor;
    QDeclarativeServiceList::MatchRule m_match;
    bool m_componentComplete;

    void updateFilterResults();

    static void s_append(QDeclarativeListProperty<QDeclarativeService> *prop, QDeclarativeService *service);
    static int s_count(QDeclarativeListProperty<QDeclarativeService> *prop);
    static QDeclarativeService* s_at(QDeclarativeListProperty<QDeclarativeService> *prop, int index);
    static void s_clear(QDeclarativeListProperty<QDeclarativeService> *prop);

Q_SIGNALS:
    void resultsChanged();
    void servicesChanged(const QDeclarativeListProperty<QDeclarativeService>&);
    void serviceNameChanged();
    void interfaceNameChanged();
    void minorVersionChanged();
    void majorVersionChanged();
    void versionMatchChanged();

};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeService));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeServiceList));

#endif
