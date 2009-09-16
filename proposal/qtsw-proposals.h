/*
 * Asynchronous Action Functionality
 *
 * Currently, the QContactAction class is purely synchronous:
 * you retrieve it; you call perform action; you delete it.
 *
 * This is (obviously) not good enough; here is a proposal for
 * an asynchronous API to allow actions to be performed in a
 * way which provides clients with progress / status updates.
 *
 * It is similar to the way in which QContactAbstractRequest
 * works.  QContactAction::performAction(..) now starts the
 * asynchronous action.  Prior to calling performAction(),
 * the action will be in the Inactive state.  After calling
 * performAction(), the action will either be Active,
 * Finished or Autonomous (which is the same as Finished for
 * most intents and purposes; see inline comments).
 */

class QTCONTACTS_EXPORT QContactAction : public QObject
{
    Q_OBJECT

public:

    // .. all the other stuff needed for QContactAction...

    /* Initiate the asynchronous action on the given contact (and optionally detail) */
    virtual void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail()) = 0;

    /* The status of Autonomous is where an action which doesn't send any progress signals */
    enum Status {
        Inactive = 0,   // operation not yet started
        Active,         // operation started, not yet finished
        Finished,       // operation successfully completed
        Autonomous      // operation started, no further information available - name under discussion.
    };

signals:
    void progress(QContactAction::Status status, const QVariant& result);
};
