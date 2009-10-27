/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#ifndef QTRACKERCONTACTASYNCREQUEST_H_
#define QTRACKERCONTACTASYNCREQUEST_H_

#include <QObject>

#include <QtTracker/Tracker>
#include <QtTracker/QLive>
#include <qcontact.h>
class QContactAbstractRequest;
class QContactManagerEngine;

/*!
 * Current implementation only server get all contacts requests.
 * This class is a link between QtMobility async request and tracker signals
 */
class QTrackerContactAsyncRequest
{
public:
    QTrackerContactAsyncRequest(QContactAbstractRequest* req);
    virtual ~QTrackerContactAsyncRequest();

protected:
    QContactAbstractRequest* req;
};

/*!
 * Running QContactFetchRequest. Doing the async tracker query and when data is ready setting the
 * finished status of request. \sa QTrackerContactFetchRequest
 */
class QTrackerContactFetchRequest : public QObject, public QTrackerContactAsyncRequest
{
    Q_OBJECT
public:
    QTrackerContactFetchRequest(QContactAbstractRequest* req, QContactManagerEngine* parent);
public slots:
    void contactsReady();
    void phoneNumbersReady();
    void emailAddressesReady();
    void iMAcountsReady();

private:
    // fills received phone number from tracker to list of contacts to QContactPhoneMumber details
    void processQueryPhoneNumbers(SopranoLive::LiveNodes queryPhoneNumbers, QList<QContact>& contacts, bool affiliationNumbers);
    void processQueryEmailAddresses(SopranoLive::LiveNodes queryEmailAddresses, QList<QContact>& contacts, bool affiliationEmails);
    void processQueryIMAccounts(SopranoLive::LiveNodes queryIMAccounts, QList<QContact>& contacts, bool affiliationAccounts);
    void validateRequest();

protected:
    SopranoLive::LiveNodes query;

    QList<SopranoLive::LiveNodes> queryPhoneNumbersNodes; // 2 - one for affiliations and another one for PersonContact
    int queryPhoneNumbersNodesReady; // remove after figuring out how to check if data in LiveNodes is updated
    QList<SopranoLive::LiveNodes> queryEmailAddressNodes; // 2 - one for affiliations and another one for PersonContact
    int queryEmailAddressNodesReady; // remove after figuring out how to check if data in LiveNodes is updated
    QList<SopranoLive::LiveNodes> queryIMAccountNodes; // 2 - one for affiliations and another one for PersonContact
    int queryIMAccountNodesReady; // remove after figuring out how to check if data in LiveNodes is updated
};

/*!
 * Running QContactLocalIdFetchRequest. Doing the async tracker query and when data is ready setting the
 * finished status of request. \sa QTrackerContactIdFetchRequest
 */
class QTrackerContactIdFetchRequest : public QObject, public QTrackerContactAsyncRequest
{
    Q_OBJECT
public:
    QTrackerContactIdFetchRequest(QContactAbstractRequest* req, QContactManagerEngine* parent);
public slots:
    void modelUpdated();

protected:
    SopranoLive::LiveNodes query;
};



#endif /* QTRACKERCONTACTASYNCREQUEST_H_ */
