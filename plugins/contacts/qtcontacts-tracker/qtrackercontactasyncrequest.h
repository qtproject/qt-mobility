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


#ifndef QTRACKERCONTACTASYNCREQUEST_H_
#define QTRACKERCONTACTASYNCREQUEST_H_

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
