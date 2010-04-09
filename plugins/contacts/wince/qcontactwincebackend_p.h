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

#ifndef QCONTACTWINCEBACKEND_P_H
#define QCONTACTWINCEBACKEND_P_H

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
#include <QSharedData>
#include <windows.h>
#include <pimstore.h>

#include "qtcontactsglobal.h"
#include "qcontact.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"
#include "qcontactrequestworker_p.h"

QTM_USE_NAMESPACE

/*
 * Simple "smart" pointer for IUnknown management - takes ownership when assigning, calls release at dtor
 *
 * typename T is expected to be the base interface class, not a pointer to it
 *
 * (e.g. SCP<IUnknown> foo, not SCP<IUnknown*> foo)
 *
 * but all other usage acts like a pointer thereof.
 */
template <typename T> class SimpleComPointer {
public:
    SimpleComPointer()
        : mPtr(0)
    {
    }

    SimpleComPointer(T* assign)
        : mPtr(assign)
    {
    }

    ~SimpleComPointer()
    {
        if (mPtr)
            mPtr->Release();
    }

    // These are const incorrect.
    // cast operator, not operator*
    operator T * () const
    {
        return mPtr;
    }

    T* operator ->() const
    {
        return mPtr;
    }

    // Releases, since this is non const.
    T** operator&()
    {
        if (mPtr)
            mPtr->Release();
        mPtr = 0;
        return &mPtr;
    }

    SimpleComPointer<T>& operator=(T* other)
    {
        if (mPtr != other) {
            if (mPtr)
                mPtr->Release();
            mPtr = other;
        }
        return *this;
    }
    SimpleComPointer<T>& operator=(const SimpleComPointer<T>& other)
    {
        if (&other != this && other.mPtr != mPtr) {
            if (mPtr)
                mPtr->Release();
            mPtr = other.mPtr;
        }
        return *this;
    }

private:
    T* mPtr;
};

class ComInit
{
public:
    ComInit() {mHr = CoInitializeEx( NULL, 0);}
    ~ComInit() {if (SUCCEEDED(mHr)) CoUninitialize(); }
    HRESULT hr() const {return mHr;}
private:
    HRESULT mHr;
};

class ContactWinceFactory;
class QContactWinCEEngineData : public QSharedData
{
public:
    QContactWinCEEngineData()
        : QSharedData(),
        m_refCount(QAtomicInt(1)),
        m_phonemeta(PIMPR_INVALID_ID),
        m_emailmeta(PIMPR_INVALID_ID),
        m_factory(0)
    {
    }

    QContactWinCEEngineData(const QContactWinCEEngineData& other)
        : QSharedData(other),
        m_refCount(QAtomicInt(1)),
        m_phonemeta(PIMPR_INVALID_ID),
        m_emailmeta(PIMPR_INVALID_ID),
        m_factory(0),
        m_avatarmeta(PIMPR_INVALID_ID),
        m_avatartypemeta(PIMPR_INVALID_ID)
    {
    }

    ~QContactWinCEEngineData()
    {
    }
    QAtomicInt m_refCount;

    ComInit m_cominit;

    SimpleComPointer<IPOutlookApp2> m_app;
    SimpleComPointer<IFolder> m_folder;
    SimpleComPointer<IPOutlookItemCollection> m_collection;
    SimpleComPointer<IPOlItems2> m_items2;

    // The ID of our sekrit extra phone number and email metadata id
    PROPID m_phonemeta;
    PROPID m_emailmeta;
    PROPID m_avatarmeta;
    PROPID m_avatartypemeta;

    // List of ids (OIDs are equiv to unique ids, yay)
    QList<QContactLocalId> m_ids;
    QContactRequestWorker m_requestWorker;
    QString m_engineName;
    ContactWinceFactory* m_factory;
};

class QContactWinCEEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    QContactWinCEEngine(ContactWinceFactory* factory, const QString& engineName, const QMap<QString, QString>& parameters, QContactManager::Error* error);
    QContactWinCEEngine(const QContactWinCEEngine& other);
    ~QContactWinCEEngine();
    QContactWinCEEngine& operator=(const QContactWinCEEngine& other);

    /* URI reporting */
    virtual QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    virtual int managerVersion() const;

    /* Filtering */
    virtual QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;

    virtual QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    virtual QContact contact(const QContactLocalId& contactId, const QtMobility::QContactFetchHint& hint, QContactManager::Error* error) const;

    virtual bool saveContact(QContact* contact, QContactManager::Error* error);
    virtual bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);
    virtual bool saveRelationship(QContactRelationship* relationship, QContactManager::Error* error);
    virtual bool removeRelationship(const QContactRelationship& relationship, QContactManager::Error* error);

    virtual bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    virtual bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Return a pruned or modified contact which is valid and can be saved in the backend */
    virtual QContact compatibleContact(const QContact& original, QContactManager::Error* error)
    {
        return QContactManagerEngine::compatibleContact(original, error);
    }

    /* Synthesize the display label of a contact */
    virtual QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;


    /* "Self" contact id (MyCard) */
    virtual bool setSelfContactId(const QContactLocalId& contactId, QContactManager::Error* error);
    virtual QContactLocalId selfContactId(QContactManager::Error* error) const;

    /* Relationships between contacts */
    virtual QList<QContactRelationship> relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const;
    virtual bool saveRelationships(QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    virtual bool removeRelationships(const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Validation for saving */
    virtual QContact compatibleContact(const QContact&, QContactManager::Error* error) const {*error =  QContactManager::NotSupportedError;return QContact();}
    virtual bool validateContact(const QContact& contact, QContactManager::Error* error) const
    {
        return QContactManagerEngine::validateContact(contact, error);
    }
    virtual bool validateDefinition(const QContactDetailDefinition& def, QContactManager::Error* error) const
    {
        return QContactManagerEngine::validateDefinition(def, error);
    }

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;
    virtual QContactDetailDefinition detailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error) const
    {
        return QContactManagerEngine::detailDefinition(definitionId, contactType, error);
    }
    virtual bool saveDetailDefinition(const QContactDetailDefinition& def, const QString& contactType, QContactManager::Error* error);
    virtual bool removeDetailDefinition(const QString& definitionId, const QString& contactType, QContactManager::Error* error);

    /* Asynchronous Request Support */
    virtual void requestDestroyed(QContactAbstractRequest* req);
    virtual bool startRequest(QContactAbstractRequest* req);
    virtual bool cancelRequest(QContactAbstractRequest* req);
    virtual bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);

    /* Capabilities reporting */
    virtual bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const;
    virtual bool isRelationshipTypeSupported(const QString& relationshipType, const QString& contactType) const
    {
        Q_UNUSED(relationshipType);
        Q_UNUSED(contactType);
        return false;
    }

    virtual bool isFilterSupported(const QContactFilter& filter) const;
    virtual QList<QVariant::Type> supportedDataTypes() const;
    virtual QStringList supportedContactTypes() const
    {
        return QContactManagerEngine::supportedContactTypes();
    }

    /*helper functions*/
    PROPID metaAvatar() const;
    PROPID metaAvatarType() const;
    PROPID metaEmail() const;
    PROPID metaPhone() const;

private:
    QSharedDataPointer<QContactWinCEEngineData> d;

    QContact convertToQContact(IItem *contact) const;
    bool convertFromQContact(const QContact& contact, IItem* item, QContactManager::Error &error) const;
    void buildHashForContactDetailToPoomPropId() const;
    bool convertP2QContacts(const SimpleComPointer<IPOutlookItemCollection>& collection, QList<QContact>* contacts) const;
    QString convertFilterToQueryString(const QContactFilter& filter) const;
    QList<QContactLocalId> convertP2QIdList(const SimpleComPointer<IPOutlookItemCollection>& collection) const;

    friend class ContactWinceFactory;
};

class QMutex;

class ContactWinceFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
public:
    ContactWinceFactory();
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
        QString managerName() const;
        void resetEngine();
private:
    QMutex m_mutex;
    QContactWinCEEngine* m_engine;
};

#endif
