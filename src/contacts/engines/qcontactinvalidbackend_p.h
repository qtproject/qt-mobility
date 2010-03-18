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


#ifndef QCONTACTINVALIDBACKEND_P_H
#define QCONTACTINVALIDBACKEND_P_H

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

#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

#include <QMap>
#include <QString>

QTM_BEGIN_NAMESPACE

class QContactInvalidEngine : public QContactManagerEngine
{
public:
    QContactInvalidEngine();
    QString managerName() const;

    QMap<QString, QString> managerParameters() const {return QMap<QString, QString>();}
    int managerVersion() const {return 0;}

    QList<QContactLocalId> contactIds(const QContactFilter&, const QList<QContactSortOrder>&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
    }

    QList<QContact> contacts(const QContactFilter&, const QList<QContactSortOrder>&, const QContactFetchHint&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QList<QContact>();
    }

    QContact contact(const QContactLocalId&, const QContactFetchHint&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QContact();
    }

    bool saveContacts(QList<QContact>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    bool removeContacts(const QList<QContactLocalId>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    QContact conformingContact(const QContact&, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return QContact();
    }

    /* Synthesize the display label of a contact */
    virtual QString synthesizedDisplayLabel(const QContact&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QString();
    }

    /* "Self" contact id (MyCard) */
    virtual bool setSelfContactId(const QContactLocalId&, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    virtual QContactLocalId selfContactId(QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return 0;
    }

    /* Relationships between contacts */
    virtual QList<QContactRelationship> relationships(const QString&, const QContactId&, QContactRelationship::Role, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QList<QContactRelationship>();
    }

    virtual bool saveRelationships(QList<QContactRelationship>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    virtual bool removeRelationships(const QList<QContactRelationship>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    /* Validation for saving */
    virtual bool validateContact(const QContact&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    virtual bool validateDefinition(const QContactDetailDefinition&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QMap<QString, QContactDetailDefinition>();
    }

    virtual QContactDetailDefinition detailDefinition(const QString&, const QString&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QContactDetailDefinition();
    }

    virtual bool saveDetailDefinition(const QContactDetailDefinition&, const QString&, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    virtual bool removeDetailDefinition(const QString&, const QString&, QContactManager::Error* error)
    {
        *error =  QContactManager::NotSupportedError;
        return false;
    }

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QContactAbstractRequest*) {}
    virtual bool startRequest(QContactAbstractRequest*) {return false;}
    virtual bool cancelRequest(QContactAbstractRequest*) {return false;}
    virtual bool waitForRequestFinished(QContactAbstractRequest*, int) {return false;}

    /* Capabilities reporting */
    virtual bool hasFeature(QContactManager::ManagerFeature, const QString&) const
    {
        return false;
    }

    virtual QStringList supportedRelationshipTypes(const QString&) const
    {
        return QStringList();
    }

    virtual bool isFilterSupported(const QContactFilter&) const
    {
        return false;
    }

    virtual QList<QVariant::Type> supportedDataTypes() const
    {
        return QList<QVariant::Type>();
    }
    virtual QStringList supportedContactTypes() const
    {
        return QStringList();
    }
};

QTM_END_NAMESPACE

#endif

