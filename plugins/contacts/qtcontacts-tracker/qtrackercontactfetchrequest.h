/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QTRACKERCONTACTFETCHREQUEST_H_
#define QTRACKERCONTACTFETCHREQUEST_H_

#include "qtrackercontactasyncrequest.h"

#include <qmobilityglobal.h>
#include <qcontactonlineaccount.h>
#include <qcontactmanager.h>

#include <QtTracker/Tracker>
#include <QtTracker/QLive>

QTM_BEGIN_NAMESPACE
class QContactAbstractRequest;
class QContactManagerEngine;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

namespace IMAccount {
    enum IMResultColumn {
        URI = 0,
        ContactPresence,
        ContactMessage,
        ContactNickname,
        ContactDisplayname,
        ContactIMId,
        ContactAvatar
    };
};

namespace IMContact {
    enum IMResultColumn {
        URI,
        ContactId,
        ContactIMId,
        ContactPresence,
        ContactMessage,
        ContactNickname,
        AccountType,
        Capabilities,
        ServiceProvider
    };
};

/*!
 * Running QContactFetchRequest. Doing the async tracker query and when data is ready setting the
 * finished status of request. \sa QTrackerContactFetchRequest
 */
class QTrackerContactFetchRequest : public QObject, public QTrackerContactAsyncRequest
{
    Q_OBJECT
//    Q_ENUMS(IMResultColumn)
public:
    QTrackerContactFetchRequest(QContactAbstractRequest* req, QContactManagerEngine* parent);
public slots:
    void contactsReady();
    void phoneNumbersReady();
    void emailAddressesReady();
    void imAcountsReady();

protected slots:
    virtual void run();
    virtual void emitFinished(QContactManager::Error error = QContactManager::NoError);

protected:
    QContactManager::Error applyFilterToContact(SopranoLive::RDFVariable &variable, const QContactFilter &filter);
    QContactManager::Error applyDetailRangeFilterToContact(SopranoLive::RDFVariable &variable, const QContactFilter &filter);

    // contacts query
    SopranoLive::LiveNodes query;

    QList<SopranoLive::LiveNodes> queryPhoneNumbersNodes; // 2 - one for affiliations and another one for PersonContact
    int queryPhoneNumbersNodesPending;
    QList<SopranoLive::LiveNodes> queryEmailAddressNodes; // 2 - one for affiliations and another one for PersonContact
    int queryEmailAddressNodesPending;
    SopranoLive::LiveNodes queryIMAccountNodes;
    int queryIMAccountNodesPending;

    // result of the request - multiple queries updating it
    QList<QContact> result;

private:
    bool isMeContact(const QContactFilter &filter);
    // fills received phone number from tracker to list of contacts to QContactPhoneMumber details
    // all the following methods update \sa result
    void processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers, bool affiliationNumbers);
    void processQueryEmailAddresses(SopranoLive::LiveNodes queryEmailAddresses, bool affiliationEmails);
    void processQueryIMContacts(SopranoLive::LiveNodes queryIMContacts);
    void validateRequest();
    void readFromQueryRowToContact(QContact &contact, int queryRow);
    void addContactToResultSet(QContact &contact);
    QContactOnlineAccount getOnlineAccountFromIMQuery(SopranoLive::LiveNodes imAccountQuery, int queryRow);
    QContactOnlineAccount getIMAccountFromIMQuery(SopranoLive::LiveNodes imAccountQuery, int queryRow) ;
    QContactOnlineAccount getIMContactFromIMQuery(SopranoLive::LiveNodes imAccountQuery, int queryRow);

    // access existing contacts in result list, contactid to index in \sa result lookup
    QHash<quint32, int> id2ContactLookup;
};

#endif /* QTRACKERCONTACTFETCHREQUEST_H_ */
