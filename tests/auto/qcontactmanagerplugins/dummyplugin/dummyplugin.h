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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef DUMMYPLUGIN_H
#define DUMMYPLUGIN_H

#include "qcontactmanagerenginefactory.h"
#include "qcontactmanagerengine.h"
#include "qscopedpointer.h"
#include "qcontactmanager.h"

QTM_USE_NAMESPACE

class DummyEngine : public QContactManagerEngine
{
    Q_OBJECT
public:
    DummyEngine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
    DummyEngine(const DummyEngine& other);
    ~DummyEngine();
    DummyEngine& operator=(const DummyEngine& other);
    QContactManagerEngine* clone();
    void deref();
    QString managerName() const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contacts(QContactManager::Error* error) const;
    QContact contact(const QContactLocalId& contactId, QContactManager::Error* error) const;
    QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    bool saveContact(QContact* contact, bool batch, QContactManager::Error* error);
    bool removeContact(const QContactLocalId& contactId, bool batch, QContactManager::Error* error);

    /* Capabilities reporting */
    QStringList capabilities() const;
    QStringList fastFilterableDefinitions() const;
    QList<QVariant::Type> supportedDataTypes() const;

    QMap<QString, QString> managerParameters() const {return QMap<QString, QString>();}
    int managerVersion() const {return 0;}

    QList<QContactLocalId> contactIds(const QContactFilter&, const QList<QContactSortOrder>&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
    }

    QList<QContact> contacts(const QContactFilter&, const QList<QContactSortOrder>&, const QContactFetchHint&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QList<QContact>();
    }

    bool saveContacts(QList<QContact>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    bool removeContacts(const QList<QContactLocalId>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    QContact conformingContact(const QContact&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return QContact();
    }

    /* Synthesize the display label of a contact */
    virtual QString synthesizedDisplayLabel(const QContact&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QString();
    }

    /* "Self" contact id (MyCard) */
    virtual bool setSelfContactId(const QContactLocalId&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual QContactLocalId selfContactId(QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return 0;
    }

    /* Relationships between contacts */
    virtual QList<QContactRelationship> relationships(const QString&, const QContactId&, QContactRelationship::Role, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QList<QContactRelationship>();
    }

    virtual bool saveRelationships(QList<QContactRelationship>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual bool removeRelationships(const QList<QContactRelationship>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    /* Validation for saving */
    virtual QContact compatibleContact(const QContact&, QContactManager::Error* error) const
    {
        *error =  QContactManager::NotSupportedError;
        return QContact();
    }

    virtual bool validateContact(const QContact&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual bool validateDefinition(const QContactDetailDefinition&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QMap<QString, QContactDetailDefinition>();
    }

    virtual QContactDetailDefinition detailDefinition(const QString&, const QString&, QContactManager::Error* error) const
    {
        *error = QContactManager::NotSupportedError;
        return QContactDetailDefinition();
    }

    virtual bool saveDetailDefinition(const QContactDetailDefinition&, const QString&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
        return false;
    }

    virtual bool removeDetailDefinition(const QString&, const QString&, QContactManager::Error* error)
    {
        *error = QContactManager::NotSupportedError;
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

    virtual bool isRelationshipTypeSupported(const QString&, const QString&) const
    {
        return false;
    }

    virtual bool isFilterSupported(const QContactFilter&) const
    {
        return false;
    }

    virtual QStringList supportedContactTypes() const
    {
        return QStringList();
    }

};

class DummyEngineFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
        QString managerName() const;
};

#endif // DUMMYPLUGIN_H
