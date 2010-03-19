/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTSYMBIANSIMENGINE_H
#define CNTSYMBIANSIMENGINE_H

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
#include "qcontactmanagerengine.h"
#include "qcontactmanagerenginefactory.h"
#include <flogger.h>
#include <f32file.h>

//#define SYMBIANSIM_BACKEND_DEBUG

namespace {
#ifdef SYMBIANSIM_BACKEND_DEBUG
    void PbkPrintToLog( TRefByValue<const TDesC> aFormat, ... )
    {
        _LIT( KLogDir, "Sim" );
        _LIT( KLogName, "sim.log" );

        VA_LIST args;
        VA_START( args, aFormat );
        RFileLogger::WriteFormat(KLogDir, KLogName, EFileLoggingModeAppend, aFormat, args);
        VA_END( args );
    }
#else
    void PbkPrintToLog( TRefByValue<const TDesC> aFormat, ... ) { Q_UNUSED(aFormat); }
#endif    
}  // namespace

QTM_USE_NAMESPACE

#define CNT_SYMBIANSIM_MANAGER_NAME "symbiansim"

Q_DEFINE_LATIN1_CONSTANT(KSimSyncTarget, "SIM");
Q_DEFINE_LATIN1_CONSTANT(KParameterKeySimStoreName, "store");
Q_DEFINE_LATIN1_CONSTANT(KParameterValueSimStoreNameAdn, "ADN");
Q_DEFINE_LATIN1_CONSTANT(KParameterValueSimStoreNameSdn, "SDN");
Q_DEFINE_LATIN1_CONSTANT(KParameterValueSimStoreNameFdn, "FDN");

class CntSimStore;
class CntAbstractSimRequest;

class CntSymbianSimEngineData : public QSharedData
{
public:
    CntSymbianSimEngineData();
    ~CntSymbianSimEngineData();
    
    CntSimStore *m_simStore;
    QMap<QContactAbstractRequest *, CntAbstractSimRequest *> m_asyncRequests;    
};

class CntSymbianSimEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    CntSymbianSimEngine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
    CntSymbianSimEngine(const CntSymbianSimEngine &other);
    ~CntSymbianSimEngine();
    void deref();
    QString managerName() const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    QList<QContactLocalId> contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const;
    QList<QContact> contacts(const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error* error) const;
    QList<QContact> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error* error) const;
    QContact contact(const QContactLocalId& contactId, const QStringList& definitionRestrictions, QContactManager::Error* error) const;
   
    bool saveContact(QContact* contact, QContactManager::Error* error);
    bool saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);
    bool removeContact(const QContactLocalId& contactId, QContactManager::Error* error);
    bool removeContacts(QList<QContactLocalId>* contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error);

    /* Synthesize the display label of a contact */
    QString synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const;
    
    /* Definitions - Accessors and Mutators */
    QMap<QString, QContactDetailDefinition> detailDefinitions(const QString& contactType, QContactManager::Error* error) const;

    /* Asynchronous Request Support */
    void requestDestroyed(QContactAbstractRequest* req);
    bool startRequest(QContactAbstractRequest* req);
    bool cancelRequest(QContactAbstractRequest* req);
    bool waitForRequestFinished(QContactAbstractRequest* req, int msecs);    
    
    /* Capabilities reporting */
    bool hasFeature(QContactManager::ManagerFeature feature, const QString& contactType = QContactType::TypeContact) const;
    QStringList supportedContactTypes() const;

public:
    void updateDisplayLabel(QContact& contact) const;
    CntSimStore* simStore() { return d->m_simStore; }
    void setReadOnlyAccessConstraint(QContactDetail* detail) const;

private:
    bool executeRequest(QContactAbstractRequest *req, QContactManager::Error* qtError) const;

private:
    QExplicitlySharedDataPointer<CntSymbianSimEngineData> d;
};

class Q_DECL_EXPORT CntSymbianSimFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error* error);
        QString managerName() const;
};

#endif  //CNTSYMBIANSIMENGINE_H
