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

class QContactAbstractRequest;
class QContactManagerEngine;

/*!
 * Current implementation only server get all contacts requests.
 * TODO refactor for other requests
 * This class is a link between QtMobility async request and tracker signals
 */
class QTrackerContactAsyncRequest : public QObject
{
    Q_OBJECT
public:
    QTrackerContactAsyncRequest(QContactAbstractRequest* req, QContactManagerEngine* parent);
    virtual ~QTrackerContactAsyncRequest();
public slots:
    void modelUpdated();
    void contactsReady();

protected:
    QContactAbstractRequest* req;
    SopranoLive::LiveNodes query;
};

#endif /* QTRACKERCONTACTASYNCREQUEST_H_ */
