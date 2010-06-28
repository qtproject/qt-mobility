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

#ifndef QCONTACTMULTIACTION_P_H
#define QCONTACTMULTIACTION_P_H

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

#include "qcontactaction.h"
#include "qcontactactionfactory.h"

#include "qserviceinterfacedescriptor.h"
#include "qserviceplugininterface.h"
#include "qservicecontext.h"
#include "qabstractsecuritysession.h"

#include <QSharedData>
#include <QString>
#include <QVariantMap>

QTM_USE_NAMESPACE

/*
   This action plugin is capable of producing two actions which each have the
   same action name, service name, interface name and implementation (minor) version,
   but internally use a different implementation.  This difference is reported via the
   meta data function of the factory (which is exposed to clients via the descriptor
   which provides a "front end" to the factory).

   Example use case:
   Company "Example VoIP Solutions" wants to provide a "Call" action with different implementations.
       -> it provides a SINGLE plugin which provides two actions, both of which are:
           - ServiceName = "Example VoIP Solution"
           - InterfaceName = "com.nokia.qt.mobility.contact.action"
           - Major Version = "1"
           - Minor Version = "1"
           - ActionName = "Call" (this is a custom property in the service interface xml)
       -> BUT one of the actions has the custom property:
           - Provider = "sip"
       -> where the other action has the custom property:
           - Provider = "example proprietary protocol"
       -> the custom properties are available to clients via the QContactActionDescriptor::metaData()
          function.
 */

class QContactMultiActionPlugin : public QObject, public QServicePluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QServicePluginInterface)

public:
    QObject* createInstance(const QServiceInterfaceDescriptor& descriptor,
                            QServiceContext* context,
                            QAbstractSecuritySession* session);
};

class QContactMultiActionFactory : public QContactActionFactory
{
    Q_OBJECT

public:
    QContactMultiActionFactory();
    ~QContactMultiActionFactory();

    QList<QContactActionDescriptor> actionDescriptors() const;
    QContactAction* create(const QContactActionDescriptor& which) const;

    QSet<QContactActionTarget> supportedTargets(const QContact& contact, const QContactActionDescriptor& which) const;
    QContactFilter contactFilter(const QContactActionDescriptor& which) const;
    QVariant metaData(const QString& key, const QList<QContactActionTarget>& targets, const QVariantMap& parameters, const QContactActionDescriptor& which) const;

    bool supportsContact(const QContact& contact, const QContactActionDescriptor& which) const;

private:
    QContactActionDescriptor m_actionOneDescriptor;
    QContactActionDescriptor m_actionTwoDescriptor;
};

class QContactActionOne : public QContactAction
{
    Q_OBJECT

public:
    QContactActionOne();
    ~QContactActionOne();

    bool invokeAction(const QContactActionTarget& target, const QVariantMap& params = QVariantMap());
    bool invokeAction(const QList<QContactActionTarget>& targets, const QVariantMap& params = QVariantMap());
    QVariantMap results() const;
    State state() const;

private slots:
    void performAction();
};

class QContactActionTwo : public QContactAction
{
    Q_OBJECT

public:
    QContactActionTwo();
    ~QContactActionTwo();

    bool invokeAction(const QContactActionTarget& target, const QVariantMap& params = QVariantMap());
    bool invokeAction(const QList<QContactActionTarget>& targets, const QVariantMap& params = QVariantMap());
    QVariantMap results() const;
    State state() const;

private slots:
    void performAction();
};

#endif
