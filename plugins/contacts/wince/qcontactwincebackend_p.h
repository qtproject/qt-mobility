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
        m_avatarImageMeta(PIMPR_INVALID_ID),
        m_avatarVideoMeta(PIMPR_INVALID_ID)
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
    PROPID m_avatarImageMeta;
    PROPID m_avatarVideoMeta;

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
    virtual int managerVersion() const;

    /* Filtering */
    virtual QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;

    virtual QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const;
    virtual QContact contact(const QContactLocalId& contactId, const QtMobility::QContactFetchHint& hint, QContactManager::Error* error) const;

    virtual bool saveContact(QContact* contact, QContactManager::Error* error);
    virtual bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);

    virtual bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    virtual bool removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Synthesize the display label of a contact */
    virtual QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;

    /* Definitions - Accessors and Mutators */
    virtual QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;

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
    PROPID metaAvatarImage() const;
    PROPID metaAvatarVideo() const;
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
