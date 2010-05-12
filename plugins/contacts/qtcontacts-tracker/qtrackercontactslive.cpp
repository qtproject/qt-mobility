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

#include "qtrackercontactslive.h"

QTrackerContactsLive::QTrackerContactsLive()
{
    transaction_ = ::tracker()->initiateTransaction();
    service_ = this->service();
}

void QTrackerContactsLive::setLiveContact(const Live<nco::PersonContact> &lc) {
    liveContact_ = lc;
}

// TODO: Maybe we need to retrieve the original contact object from Tracker here
//       to be able to determine what has changed in the original contact object.
void QTrackerContactsLive::setQContact(const QContact &qc) {
    editedContact_ = qc;
}

RDFServicePtr QTrackerContactsLive::service() {
    if(service_) {
        return service_;
    } else {
        if(transaction_) {
            // if transaction was obtained, grab the service from inside it and use it
            service_ = transaction_->service();
        } else {
            // otherwise, use tracker directly, with no transactions.
            service_ = ::tracker();
        }
        return service_;
    }
}

// TODO: Handle internally checks if the fields needs to be updated or not so
//       that we only save modified data to Tracker.
void QTrackerContactsLive::saveName() {

    QContactName name = editedContact_.detail<QContactName>();
    QContactNickname nickname = editedContact_.detail<QContactNickname>();

    if(!name.isEmpty()) {
        liveContact_->setNameHonorificPrefix(name.prefix());
        liveContact_->setNameGiven(name.firstName());
        liveContact_->setNameAdditional(name.middleName());
        liveContact_->setNameFamily(name.lastName());
    }

    if(!nickname.isEmpty()) {
        liveContact_->setNickname(nickname.nickname());
    }
}

void QTrackerContactsLive::commit() {
    transaction_->commit();
}


