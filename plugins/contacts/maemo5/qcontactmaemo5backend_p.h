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


#ifndef QCONTACTMAEMO5BACKEND_P_H
#define QCONTACTMAEMO5BACKEND_P_H

#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

#include <QMap>
#include <QString>
#include <QList>

#include <QDebug>

#include "qtcontacts.h"
#include "qcontactabook_p.h"
#include "qcontactchangeset.h"

#define MAEMO5_ENGINE_VERSION 0.1

QTM_USE_NAMESPACE

class Q_DECL_EXPORT ContactMaemo5Factory : public QObject, public QContactManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
  public:
    QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error*);
    QString managerName() const;
};

class QContactMaemo5EngineData : public QSharedData
{
public:
    QContactMaemo5EngineData()
        : QSharedData(),
        m_refCount(QAtomicInt(1)),
        m_abook(new QContactABook)
    {  
    }

    QContactMaemo5EngineData(const QContactMaemo5EngineData& other)
        : QSharedData(other),
        m_refCount(QAtomicInt(1)),
        m_abook(other.m_abook)
    {
    }

    ~QContactMaemo5EngineData()
    {
      delete m_abook;
    }

    QAtomicInt m_refCount;
    QContactABook *m_abook;
};

class QContactMaemo5Engine : public QContactManagerEngine
{
  Q_OBJECT
  public:
    QContactMaemo5Engine();
    QContactMaemo5Engine& operator=(const QContactMaemo5Engine& other);

    QString managerName() const;
    QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;
    bool validateContact(const QContact&, QContactManager::Error* error) const;
    bool validateDefinition(const QContactDetailDefinition&, QContactManager::Error* error) const;

    /* "Self" contact id (MyCard) */
    // bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error); /* Not supported */
    QContactLocalId selfContactId(QContactManager::Error* errors) const;

    /* Filtering */
    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;

    /* Contacts - Accessors and Mutators */
    QContact contact(const QContactLocalId& contactId, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error ) const;
    bool saveContact(QContact* contact, QContactManager::Error* error);
    bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);
    bool saveContacts(QList<QContact>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error); // implemented in terms of the singular saveContact
    bool removeContacts(const QList<QContactLocalId>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error); // implemented in terms of the singular removeContact
    
    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;
    QContactDetailDefinition detailDefinition(const QString& definitionName, const QString& contactType, QContactManager::Error* error) const; // implemented in terms of the plural detailDefinitions
    //bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error* error);
    //bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error);
    
    /* Version Reporting */
    int implementationVersion() const { return MAEMO5_ENGINE_VERSION; };
    
    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    bool isFilterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;




    // XXX TODO: FIXME - these are pure virtual and so MUST be implemented by the backend.  Stubs here.
    QMap<QString, QString> managerParameters() const {return QMap<QString,QString>();}
    int managerVersion() const {return 1;}
    bool setSelfContactId(const QContactLocalId&, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    QList<QContactRelationship> relationships(const QString&, const QContactId&, QContactRelationship::Role, QContactManager::Error* error) const {*error = QContactManager::NotSupportedError; return QList<QContactRelationship>();}
    bool saveRelationships(QList<QContactRelationship>*, QMap<int, QContactManager::Error>*, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    bool removeRelationships(const QList<QContactRelationship>&, QMap<int, QContactManager::Error>*, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    bool saveDetailDefinition(const QContactDetailDefinition&, const QString&, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    bool removeDetailDefinition(const QString&, const QString&, QContactManager::Error* error) {*error = QContactManager::NotSupportedError; return false;}
    void requestDestroyed(QContactAbstractRequest*) {}
    bool startRequest(QContactAbstractRequest*) {return false;}
    bool cancelRequest(QContactAbstractRequest*) {return false;}
    bool waitForRequestFinished(QContactAbstractRequest*, int) {return false;}
    bool isRelationshipTypeSupported(const QString&, const QString&) const {return false;}
    QStringList supportedContactTypes() const {return (QStringList() << QContactType::TypeContact);}

  private:
    QSharedDataPointer<QContactMaemo5EngineData> d;
};

#endif

