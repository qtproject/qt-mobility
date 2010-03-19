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
    
    void deref();
    QString managerName() const;
    QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;

    /* "Self" contact id (MyCard) */
    // bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error); /* Not supported */
    QContactLocalId selfContactId(QContactManager::Error* errors) const;

    /* Filtering */
    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;

    /* Contacts - Accessors and Mutators */
    QContact contact(const QContactLocalId& contactId, const QStringList& definitionRestrictions, QContactManager::Error* error) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error* error ) const;
    QList<QContact> contacts( const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error* error ) const;
    
    bool saveContact(QContact* contact, QContactManager::Error* error);
    
    //QList<QContactManager::Error> removeContacts(QList<QContactLocalId>* contactIds, QContactManager::Error* error);
    bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);
    
    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;
    //bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error* error);
    //bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error);
    
    /* Version Reporting */
    int implementationVersion() const { return MAEMO5_ENGINE_VERSION; };
    
    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    QStringList supportedRelationshipTypes(const QString& contactType) const;
    bool isFilterSupported(const QContactFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;


    // XXX TODO: FIXME - include some trampoline functions to access protected static members of QCME.
    static QList<QContactLocalId> sortContactsTrampoline(const QList<QContact>& unsorted, const QList<QContactSortOrder>& sortOrders)
    {
        return QContactManagerEngine::sortContacts(unsorted, sortOrders);
    }

  private:
    QSharedDataPointer<QContactMaemo5EngineData> d;
};

#endif

