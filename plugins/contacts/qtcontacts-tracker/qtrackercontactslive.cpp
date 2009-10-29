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
        if(!name.prefix().isEmpty()) {
            liveContact_->setNameHonorificPrefix(name.prefix());
        }

        if(!name.first().isEmpty()) {
            liveContact_->setNameGiven(name.first());
        }

        if(!name.middle().isEmpty()) {
            liveContact_->setNameAdditional(name.middle());
        }

        if(!name.last().isEmpty()) {
            liveContact_->setNameFamily(name.last());
        }
    }

    if(!nickname.isEmpty()) {
        liveContact_->setNickname(nickname.nickname());
    }
}

void QTrackerContactsLive::commit() {
    transaction_->commit();
}

// TODO this code is not used anymore: Left it here as Johan probably have plans to refactor it
// It contains blocking call and if it is used inside transaction introduces a bug:
// node doesnt exist so it tries to create it 2 times and add to contact to nco:hasAffiliation
// which is a single field value - so contact cannot be saved as transaction is broken
Live<nco::Role> QTrackerContactsLive::contactByContext(const QContactDetail& det, const Live<nco::PersonContact>& ncoContact) {
    if (locationContext(det) == ContactContext::Work) {
        return ncoContact->getHasAffiliation();
    } else {   // Assume home context.
        // Tracker will return the same contact as we are editing - we want to add "home" properties to it.
        return ncoContact;
    }
}

ContactContext::Location QTrackerContactsLive::locationContext(const QContactDetail& det) const {
    if (det.contexts().contains(QContactDetail::ContextHome)) {
        return ContactContext::Home;
    } else if(det.contexts().contains(QContactDetail::ContextWork)) {
        return ContactContext::Work;
    }
    return ContactContext::Unknown;
}


