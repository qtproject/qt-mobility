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

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif

QTM_USE_NAMESPACE

#define CNT_SYMBIANSIM_MANAGER_NAME "symbiansim"

class CntSymbianSimEngine : public QContactManagerEngine
{
    Q_OBJECT

public:
    CntSymbianSimEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
    ~CntSymbianSimEngine();
    void deref();
    QString managerName() const;

    /* Contacts - Accessors and Mutators */
    QList<QContactLocalId> contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QList<QContactLocalId> contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const;
    QContact contact(const QContactLocalId& contactId, QContactManager::Error& error) const;
    bool saveContact(QContact* contact, QContactManager::Error& error);
    bool removeContact(const QContactLocalId& contactId, QContactManager::Error& error);

    /* Synthesize the display label of a contact */
    QString synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& error) const;

private:
    QContact fetchContactL(const QContactLocalId &localId) const;
    QList<QContact> fetchContactsL() const;
    void saveContactL(QContact* contact) const;
    void transformError(TInt symbianError, QContactManager::Error& qtError) const;
    QList<QContact> decodeSimContactsL(TDes8& rawData) const;
    QContact encodeSimContactL(const QContact* contact, TDes8& rawData) const;

private:
    RTelServer m_etelServer;
    RMobilePhone m_etelPhone;
    RMobilePhoneBookStore m_etelStore;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg m_etelInfoPckg;

    QString m_managerUri;
};

class Q_DECL_EXPORT CntSymbianSimFactory : public QObject, public QContactManagerEngineFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactManagerEngineFactory)
    public:
        QContactManagerEngine* engine(const QMap<QString, QString>& parameters, QContactManager::Error& error);
        QString managerName() const;
};

#endif  //CNTSYMBIANSIMENGINE_H
