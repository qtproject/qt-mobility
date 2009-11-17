#ifndef QTRACKERCONTACTSLIVE_H
#define QTRACKERCONTACTSLIVE_H

#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactphonenumber.h>
#include <qcontactname.h>
#include <qcontactnickname.h>

#include <QtTracker/Tracker>
#include <QtTracker/QLive>
#include <QtTracker/ontologies/nco.h>

using namespace SopranoLive;

namespace ContactContext {
    typedef enum Location {
        Unknown = 0,
        Home,
        Work
    };
}

/**
 * This class will abstact and hide how contact information is saved to Tracker
 * by using Live node.
 */
class QTrackerContactsLive
{
public:
    QTrackerContactsLive();

    /**
     * Set the QContact object that we are editing.
     *
     * \param qc The QContact object that is used for reading the data that
     *           will be stored in Tracker.
     */
    void setQContact(const QContact& qc);

    /**
     * Give the Live node object that is used for this transaction. This
     * object will be used internally by this object and will contain the data
     * that will be stored into Tracker.
     *
     * \param lc A Live node object representing a contact. See NCO ontology
     *           for details.
     */
    void setLiveContact(const Live<nco::PersonContact>& lc);

    /**
     * Return a service pointer that is used for this transaction. The Live
     * node in setLiveContact() is retrieved from this service.
     */
    RDFServicePtr service();

    /**
     * When all data is saved, we need to call this method to send the data to
     * tracker in one batch.
     */
    void commit();

    /**
     * Worker method that is doing the saving of the name properties. This method
     * will use the objects given by setQContact() and setLiveContact() to determine
     * what will be stored into Tracker.
     */
    void saveName();

private:
    /**
     * \deprecated. soon to be removed. \sa method implementation comment
     * Return a nco::Contact that is either a nco::Affiliation or
     * nco::PersonContact depending on the context (work or home) for the given contact detail.
     *
     * \param det The contact detail that we are currently interested in and which
     *            context we are examining.
     * \param ncoContact The nco::PersonContact that we want to store the contact
     *                    detail for.
     * \return Returns a nco::Affiliation is the QContactDetail context is
     *         work. Otherwise returns nco::PersonContact.
     */
    // TODO: We need to make these private once all savings are done using this class.
    Live<nco::Role> contactByContext(const QContactDetail&, const Live<nco::PersonContact>&);


    /**
     * \deprecated. soon to be removed. \sa method implementation comment
     *
     * This will look at the context of the QContactDetail and return the data if this is
     * home or work context.
     *
     * \param det The QContactDetail to investigate.
     * \return ContactContext::Home for home QContactDetail context.
     *         ContactContext::Work for work QContactDetail context.
     */
    // TODO: We need to make these private once all savings are done using this class.
    ContactContext::Location locationContext(const QContactDetail& det) const;

private:
    QContact editedContact_;
    Live<nco::PersonContact> liveContact_;

    RDFServicePtr service_;
    RDFTransactionPtr transaction_;
};

#endif // QTRACKERCONTACTSLIVE_H
