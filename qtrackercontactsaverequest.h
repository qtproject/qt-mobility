/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#ifndef QTRACKERCONTACTSAVEREQUEST_H_
#define QTRACKERCONTACTSAVEREQUEST_H_

#include <QObject>
#include <qtrackercontactasyncrequest.h>
#include <QPair>
#include <QList>
#include <QtTracker/QLive>
#include <QtTracker/ontologies/nco.h>
#include <qtcontacts.h>

class QContactAbstractRequest;
class QContactManagerEngine;

class QTrackerContactSaveRequest: public QObject, public QTrackerContactAsyncRequest
{
    Q_OBJECT
public:
    QTrackerContactSaveRequest(QContactAbstractRequest* req, QContactManagerEngine* parent);
    virtual ~QTrackerContactSaveRequest();

private:
    /* worker methods*/
    void saveContact(QContact &contact);
    void computeProgress();
    void addAffiliation(SopranoLive::RDFServicePtr service, QUniqueId contactId);
    void saveContactDetails(SopranoLive::RDFServicePtr service,SopranoLive::Live<SopranoLive::nco::PersonContact>& ncoContact,const QContact &contact);
    void saveAddresses(SopranoLive::RDFServicePtr service, SopranoLive::RDFVariable &var, const QList<QContactDetail> &details );
    void saveEmails(SopranoLive::RDFServicePtr service, SopranoLive::RDFVariable &var, const QList<QContactDetail> &details );
    void saveUrls(SopranoLive::RDFServicePtr service, SopranoLive::RDFVariable &var, const QList<QContactDetail> &details );
    void savePhoneNumbers(SopranoLive::RDFServicePtr service, SopranoLive::RDFVariable &var, const QList<QContactDetail> &details );
    void deletePhoneNumbers(SopranoLive::RDFServicePtr service, const SopranoLive::RDFVariable& rdfContactIn);
    void addTag(SopranoLive::RDFServicePtr service, SopranoLive::RDFVariable &var, const QString &tag);
    void createTagIfItDoesntExistAlready(SopranoLive::RDFServicePtr service, const QString &tag);

private:
    /*holding the data about status of async operation*/
    QList<QContact> contactsFinished;
    QList<QContactManager::Error> errorsOfContactsFinished;

    /* extracted utilities */
    static QStringList detailsDefinitionsInContact(const QContact &c);
    static bool contactHasWorkRelatedDetails(const QContact &c);

};

#endif /* QTRACKERCONTACTSAVEREQUEST_H_ */
