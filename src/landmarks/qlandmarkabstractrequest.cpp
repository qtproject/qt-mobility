#include "qlandmarkabstractrequest.h"

QTM_USE_NAMESPACE
/*!
    \class QLandmarkAbstractRequest
    \brief The QLandmarkAbstractRequest class provides a mechanism for
    asynchronous requests to be made.

    \ingroup landmarks

    It allows a client to asynchronously request some functionality
    from a QLandmarkManager.
*/

/*!
    \enum QLandmarkAbstractRequest::RequestType
    Defines the possible types of asynchronous requests.
    \value  InvalidRequest An invalid request
    \value  LandmarkIdFetchRequest A request to fetch a list of landmark
            identifiers.
    \value  CategoryIdFetchRequest A request to fetch a list of catgories
    \value  LandmarkSaveRequest A request to save a list of landmarks.
    \value  LandmarkRemoveRequest A request to remove a list of landmarks.
    \value  CategorySaveRequest A request to save a list of categories.
    \value  CategoryRemoveRequest A request to remove a list of categories.
*/

/*!
    \enum QLandmarkAbstractRequest::State
    Defines the possible states of asynchronous requests.
    \value InactiveState Operation not yet started
    \value ActiveState Operation started, not yet finished
    \value CanceledState Operation is finished due to cancellation
    \value FinishedState Operation successfully completed
*/

/*!
    Constructs a new, invalid asynchronous request.
*/
QLandmarkAbstractRequest::QLandmarkAbstractRequest()
{
}

/*!
    \internal
*/
QLandmarkAbstractRequest::QLandmarkAbstractRequest(QLandmarkAbstractRequestPrivate *dd)
{
}

/*!
    Destroys the asynchronous request
*/
QLandmarkAbstractRequest::~QLandmarkAbstractRequest()
{
}

/*!
    Returns the type of this asynchronous request.
*/
QLandmarkAbstractRequest::RequestType QLandmarkAbstractRequest::type() const
{
}

/*!
    Returns the state of the request
*/
QLandmarkAbstractRequest::State QLandmarkAbstractRequest::state()
{
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Inactive state;
    otherwise, returns false
    \sa state()
*/
bool QLandmarkAbstractRequest::isInactive() const
{
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Active state;
    otherwise, returns false
    \sa state()
*/
bool QLandmarkAbstractRequest::isActive() const
{
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Finished state;
    otherwise, returns false
    \sa state()
*/
bool QLandmarkAbstractRequest::isFinished() const
{
}

/*!
    Returns true if the request is in the \c QLandmarkAbstractRequest::Cancelled state;
    otherwise, returns false
    \sa state()
*/
bool QLandmarkAbstractRequest::isCancelled() const
{
}

/*!
    Returns the error of the most recent asynchronous operation
*/
QLandmarkManager::Error QLandmarkAbstractRequest::error() const
{
}

/*!
    Returns a human readable string of the most recent
    asynchronous operation.
*/
QString QLandmarkAbstractRequest::errorString() const
{
}

/*!
    Returns a pointer to the landmark manager which
    this request operates on.
*/
QLandmarkManager *QLandmarkAbstractRequest::manager() const
{
}

/*!
    Sets the \a manager which this request operates on.
*/
void QLandmarkAbstractRequest::setManager(QLandmarkManager *manager)
{
}

/*!
    Attempts to start the request.

    Returns true if the request was started, otherwise false.
*/
bool QLandmarkAbstractRequest::start()
{
}

/*!
    Attempts to cancel the request.

    Returns true if the request was cancelled, otherwise false.
*/
bool QLandmarkAbstractRequest::cancel()
{
}

/*!
    Blocks until the request has been completed or until \a msecs milliseconds
    has elapsed.

    Returns true if the request was cancelled or completed successfully
    within the given period, otherwise returns false.
*/
bool QLandmarkAbstractRequest::waitForFinished(int msecs)
{
}

/*!
  \fn void QLandmarkAbstractRequest::resultsAvailable()
  This signal is emitted when new results are available.  Results
  can include the operation error which may be accessed via error(),
  or derived-class specific results which are accessible through
  the derived class API.

  \sa error()
*/

/*!
    \fn void QLandmarkAbstractRequest::stateChanged(QLandmarkAbstractRequest::State newState)
    This signal is emitted when the state of the request is changed.  The new state of
    the request will be contained in \a newState.
*/

#include "moc_qlandmarkabstractrequest.cpp"





