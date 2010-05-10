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

#include "qlandmarkmanagerengine.h"
#include "qlandmark.h"
#include "qlandmarkcategory.h"
#include "qlandmarkid.h"
#include "qlandmarkcategoryid.h"

#include "qlandmarkrequests_p.h"

#include "qlandmarkabstractrequest.h"
#include "qlandmarkidfetchrequest.h"
#include "qlandmarkfetchrequest.h"
#include "qlandmarkremoverequest.h"
#include "qlandmarksaverequest.h"
#include "qlandmarkcategoryidfetchrequest.h"
#include "qlandmarkcategoryfetchrequest.h"
#include "qlandmarkcategoryremoverequest.h"
#include "qlandmarkcategorysaverequest.h"
#include "qlandmarkimportrequest.h"
#include "qlandmarkexportrequest.h"

#include "qlandmarknamesort.h"
#include "qlandmarkdistancesort.h"

#include "qlandmarkattributefilter.h"
#include "qlandmarkboxfilter.h"

#include "qgeocoordinate.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QLandmarkManagerEngine
    \brief The QLandmarkManagerEngine class provides the interface for all implementations
    of the landmark manager backend functionality.
    \ingroup landmarks-backend

    Instances of this class are usually provided by \l QLandmarkManagerEngineFactory, which is loaded
    from a plugin.
*/

/*!
    Constructs a landmark manager engine.
*/
QLandmarkManagerEngine::QLandmarkManagerEngine()
{
}

/*!
    Destructor.
*/
QLandmarkManagerEngine::~QLandmarkManagerEngine()
{
}

/*! Returns the manager name for this QLandmarkManagerEngine */
QString QLandmarkManagerEngine::managerName() const
{
    return QString(QLatin1String("base"));
}

/*!
  Returns the parameters with which this engine was constructed.  Note that
  the engine may have discarded unused or invalid parameters at the time of
  construction, and these will not be returned.
 */
QMap<QString, QString> QLandmarkManagerEngine::managerParameters() const
{
    return QMap<QString, QString>(); // default implementation requires no parameters.
}

/*!
  Returns the unique URI of this manager, which is built from the manager name and the parameters
  used to construct it.
 */
QString QLandmarkManagerEngine::managerUri() const
{
    return QLandmarkManager::buildUri(managerName(), managerParameters(), managerVersion());
}

/*!
  Returns the engine backend implementation version number
 */
int QLandmarkManagerEngine::managerVersion() const
{
    return 0;
}

/*!
    Returns a list of landmark ids of landmarks that match the given \a filter, sorted
    according to the given \a sortOrders.  Depending on the backend, this filtering operation
    may involve retrieving all the landmarks.  Any error which occurs will be saved in \a error
    and \a errorString.
 */
QList<QLandmarkId> QLandmarkManagerEngine::landmarkIds(const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders, QLandmarkManager::Error* error,
        QString *errorString) const
{
    return QList<QLandmarkId>();
}

/*!
    Returns a list of all category identifiers.  Any error which occurs will be saved in \a
    error and \a errorString.
*/
QList<QLandmarkCategoryId> QLandmarkManagerEngine::categoryIds(QLandmarkManager::Error *error,
        QString *errorString) const
{
    return QList<QLandmarkCategoryId>();
}

/*!
    Returns the landmark in the datastore identified by \a landmarkId.

    Any errors encountered are:stored in \a error and \a errorString.
*/
QLandmark QLandmarkManagerEngine::landmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
        QString *errorString) const
{
    return QLandmark();
}

/*!
    Returns a list of landmarks which match the given \a landmarkIds.

    The engine will populate \a errorMap (the map of indices of the
    \a landmarkIds list to the error which occurred when retrieving the landmark
    at that index) for every index for which the landmark could not be
    retrieved.

    Overall operation errors are stored in \a error and \a errorString.
*/
QList<QLandmark> QLandmarkManagerEngine::landmarks(const QList<QLandmarkId> &landmarkIds, QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
        QString *errorString) const
{
    return QList<QLandmark>();
}

/*!
    Returns a list of landmarks which match the given \a filter and are sorted according to the \a sortOrders.

    Overall operation errors are stored in \a error and \a errorString.
*/
QList<QLandmark> QLandmarkManagerEngine::landmarks(const QLandmarkFilter &filter, const QList<QLandmarkSortOrder> &sortOrders,
        QLandmarkManager::Error *error, QString *errorString) const
{
    return QList<QLandmark>();
}

/*!
    Returns the cateory in the datastore identified by \a categoryId.

    Any errors encountered are stored in \a error and \a errorString.
*/
QLandmarkCategory QLandmarkManagerEngine::category(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error, QString *errorString) const
{
    return QLandmarkCategory();
}

/*!
    Returns a list of categories identified by \a landmarkCategoryIds.

    Overall operation errors are stored in \a error and \a errorString.
*/
QList<QLandmarkCategory> QLandmarkManagerEngine::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
        QLandmarkManager::Error *error, QString *errorString) const
{
    return QList<QLandmarkCategory>();
}

/*!
    Adds the given \a landmark to the datastore if \a landmark has a
    default-constructed identifer, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a landmark is neither
    empty nor equal to the URI of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation will fail and and \a error will be set to
    \c QLandmarkManager::DoesNotExistError.

    Alternatively, the function will update the existing landmark in the
    datastore if \a landmark has a non-empty id and currently exists
    within the datastore.

    Returns false on failure or true on success.  On successful save
    of a landmark with an empty id, it will be assigned a valid
    id and have its manager URI set to the URI of this manager.

    This function is called by the landmarks framework in both the
    single landmark save and batch batch save, if the saveLandmarks
    function is not overridden.

    The engine must emit the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Any errors encountered during this operation should be stored in
    \a error and \a errorString.
*/
bool QLandmarkManagerEngine::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error, QString *errorString)
{
    return false;
}

/*!
    Adds the list of \a landmarks to the datastore.
    Returns true if the landmarks were saved successfully, otherwise returns
    false.

    The engine will populate \a errorMap (the map of indices of the
    \a landmarks list to the error which occurred when saving the landmark
    at that index) for every index for which the landmark could not be
    saved.

    The function will only set \a error to \c QLandmarkManager::NoError
    if all landmarks were saved successfully.

    For each newly saved landmark that was successful, the identifier
    of the landmark will be updated with a new value.  If a failure
    occurs when saving a new landmark, the identifier will be cleared
    (and become an invalid identifier).

    The engine emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Overall operation errors are stored in \a error and
    \a errorString.  \a error is set to QLandmarkManager::NoError,
    if all \a landmarks were successfully saved.

    \sa QLandmarkManagerEngine::saveLandmark()
*/
bool QLandmarkManagerEngine::saveLandmarks(QList<QLandmark> * landmarks, QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error, QString *errorString)
{
    return false;
}

/*!
    Remove the landmark identified by \a landmarkId from the datastore.

    Returns true if the landmark was removed successfully, otherwise
    returnse false.

    The engine emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Any errors encountered during this operation should be stored to
    \a error and \a errorString.
*/
bool QLandmarkManagerEngine::removeLandmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString)
{
    return false;
}

/*!
    Removes every landmark whose identifier is contained in the list
    of \a landmarkIds.  Returns true if all landmarks were removed
    successfully, otherwise false.

    The engine populates \a errorMap (the map of indices of the
    \a landmarkIds list to the error which occurred when saving the landmark
    at that index) for every index for which the landmark could not be
    removed.

    The engine also emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Overall operation errors are stored in \a error and
    \a errorString.  \a error is set to QLandmarkManager::NoError, if
    all landmarks were successfully removed.

    \sa QLandmarkManagerEngine::removeLandmark()
*/
bool QLandmarkManagerEngine::removeLandmarks(const QList<QLandmarkId> &landmarkIds, QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error, QString *errorString)
{
    return false;
}

/*!
    Adds the given \a category to the datastore if \a category has a
    default-constructed identifier, or an identifier with the manager
    URI set to the URI of this manager and an empty id.

    If the manager URI of the identifier of the \a category is neither
    empty nor equal to the URI  of this manager, or the id member of the
    identifier is not empty, but does not exist in the manager,
    the operation should fail and \a error should be set to
    \c QLandmarkManager::DoesNotExistError.

    Alternatively, the function should update the existing category in the
    datastore if \a category has a non-empty id and currently exists
    within the datastore.

    Returns false on failure or true on success.  On successful save
    of a category with an invalid id, it should be assigned a valid
    id and have its manager URI set to the URI of this manager.

    The engine returns the appropriate signals to inform clients of changes
    to the datastore resulting from this operation.

    Overall operations errors should be stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::saveCategory(QLandmarkCategory* category,
        QLandmarkManager::Error *error, QString *errorString)
{
    return false;
}

/*!
    Removes the category identified by \a categoryId from the datastore.
    The categoryId is cleared(and becomes invalid) on successful
    removal.  An unsuccessful removal should leave the identifer alone.

    Returns true if the category was removed successfully, otherwise
    returnse false.

    The engine emits the appropriate signals to inform clients of changes
    to the datastore resulting from this operation

    Overall operational errors are stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::removeCategory(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error, QString *errorString)
{
    return false;
}

/*!
    Reads landmarks from the given \a device and saves them.  The data from the \a device
    is expected to adhere to the provided \a format.

    Returns true if all landmarks could be imported, otherwise returns false.
    It may be possible that only a subset of landmarks are imported.

    Overall operational errors are stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::importLandmarks(QIODevice *device, const QByteArray &format,
        QLandmarkManager::Error *error, QString *errorString)
{
    return false;
}

/*!
    Writes landmarks to the given \a device.  The landmarks will be written
    according to the specified \a format.  If  \a landmarkIds is empty, then
    all landmarks will be exported, otherwise only those landmarks that
    match \a landmarkIds will be exported.

    Returns true if all specified landmarks were successfully exported,
    otherwise returns false.  It may be possible that only a subset
    of landmarks are exported.

    Overall operation errors are stored in \a error and
    \a errorString.
*/
bool QLandmarkManagerEngine::exportLandmarks(QIODevice *device, const QByteArray &format, QList<QLandmarkId> landmarkIds,
        QLandmarkManager::Error *error, QString *errorString) const
{
    return false;
}

/*!
    \fn bool QLandmarkManagerEngine::isFilterSupported(QLandmarkFilter::FilterType filterType) const

    Returns a whether the supplied \a filterType can be implemented
    natively by this engine. If not, the functionality will be emulated.
*/

/*!
    \fn bool QLandmarkManagerEngine::isReadOnly() const

    Returns true if the manager engine is exclusively read only.  Meaning
    landmarks and categories cannot be added, modified or removed
*/

/*!
    \fn bool QLandmarkManagerEngine::isReadOnly(const QLandmarkId &landmarkId) const

    Returns true if the landmark identified by \a landmarkId
    considered read-only by the manager engine.

    If the \a landmarkId does not refer to an existing landmark,
    it is considered writable unless the manager engine. is exclusively read-only.

 */

 /*!
    \fn bool QLandmarkManagerEngine::isReadOnly(const QLandmarkCategoryId &categoryId) const

    Returns true if the category identified by \a categoryId is
    considered read-only by the manager engine.

    If \a categoryId does not refer to an existing category,
    it is considered writable unless the manager engine is exclusively read-only.
*/

/*!
    Notifies the manager engine that the givan \a request has been destroyed.
*/
void QLandmarkManagerEngine::requestDestroyed(QLandmarkAbstractRequest* request)
{
    Q_UNUSED(request);
}

/*!
    Asks the manager engine to begin the given \a request
    which is currently in a re(startable) state.

    Returns true if the request was started successfully,
    else returns false.

    \sa QLandmarkAbstractRequest::start()
*/
bool QLandmarkManagerEngine::startRequest(QLandmarkAbstractRequest* request)
{
    Q_UNUSED(request);
    return false;
}

/*!
    Asks the manager engine to cancel the given \a request which was
    previously started and is currently in a cancellable state.
    Returns true if cancellation of the request was started successfully,
    otherwise returns false.

     \sa startRequest(), QLandmarkAbstractRequest::cancel()
 */
bool QLandmarkManagerEngine::cancelRequest(QLandmarkAbstractRequest* request)
{
    Q_UNUSED(request);
    return false;
}

/*!
    Blocks until the manager engine has completed the given \a request
    which was previously started, or until \a msecs milliseconds have passed.
    Returns true if the request was completed, and false if the request was not in the
    \c QLandmarkAbstractRequest::Active state or no progress could be reported.

    \sa startRequest()
 */
bool QLandmarkManagerEngine::waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs)
{
    Q_UNUSED(request);
    Q_UNUSED(msecs);
    return false;
}

/*!
  \fn QLandmarkManagerEngine::dataChanged()

  This signal is emitted some time after changes occur to the data managed by this
  engine, and the engine is unable to precisely determine which changes occurred, or if the
  engine considers the changes to be radical enough to require clients to reload all data.

  If this signal is emitted, no other signals may be emitted for the associated changes.

  As it is possible that other processes (or other devices) may have caused the
  changes, the timing can not be determined.

  \sa landmarksAdded(), landmarksChanged(), landmarksRemoved(), categoriesAdded(),
  categoriesChanged(), categoriesRemoved()
 */

/*!
   \fn QLandmarkManagerEngine::landmarksAdded(const QList<QLandmarkId> &landmarkIds)

   This signal is emitted some time after a set of landmarks has been added
   (and where the \l dataChanged() signal was not emitted for those
   changes).  As it is possible that other processes(or other devices) may
   have added the landmarks, the exact timing cannot be determined.

   There may be one or more landmark identifiers in the \a landmarkIds list.

   \sa dataChanged()
*/

/*!
    \fn QLandmarkManagerEngine::landmarksChanged(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted some time after a set of landmarks have been modified
    (and where the \l dataChanged() signal was not emitted for those changes).  As it is
    possible that other processes(or other devices) may have modified the landmarks,
    the timing cannot be determined.

    There may be one ore more landmark identifiers in the \a landmarkIds list.

    \sa dataChanged()
*/


/*!
    \fn QLandmarkManagerEngine::landmarksRemoved(const QList<QLandmarkId> &landmarkIds)

    This signal is emitted some time after a set of landmarks have been removed
    (and where the \l dataChanged() signal was not emitted for those changes).  As it is
    possible that other processes(or other devices) may have removed the landmarks,
    the timing cannot be determined.

    There may be one ore more landmark identifiers in the \a landmarkIds list.

    \sa dataChanged()
*/

/*!
   \fn QLandmarkManagerEngine::categoriesAdded(const QList<QLandmarkCategoryId> &categoryIds)

   This signal is emitted some time after a set of categories has been added
   (and where the \l dataChanged() signal was not emitted for those
   changes).  As it is possible that other processes(or other devices) may
   have added the landmarks, the exact timing cannot be determined.

   There may be one or more category identifiers in the \a categoryIds list.

   \sa dataChanged()
*/

/*!
    \fn QLandmarkManagerEngine::categoriesChanged(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted some time after a set of categories have been modified
    (and where the \l dataChanged() signal was not emitted for those changes).  As it is
    possible that other processes(or other devices) may have modified the categories,
    the timing cannot be determined.

    There may be one ore more category identifiers in the \a categoryIds list.

    \sa dataChanged()
*/

/*!
    \fn QLandmarkManagerEngine::categoriesRemoved(const QList<QLandmarkCategoryId> &categoryIds)

    This signal is emitted some time after a set of categories have been removed
    (and where the \l dataChanged() signal was not emitted for those changes).  As it is
    possible that other processes(or other devices) may have removed the categories,
    the timing cannot be determined.

    There may be one ore more category identifiers in the \a categoryIds list.

    \sa dataChanged()
*/

/*!
    Updates the given asynchronous request \a req by setting the new \a state
    of the request.  It then causes the stateChanged() signal to be emitted
    by the request.

*/
void QLandmarkManagerEngine::updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state)
{
    if (!req)
        return;
    if (req->d_ptr->state != state) {
        req->d_ptr->state = state;
        emit req->stateChanged(state);
    }

}

/*!
    Updates the given QLandmarkIdFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req, const QList<QLandmarkId>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    if (!req)
        return;
    QLandmarkIdFetchRequestPrivate * rd = static_cast<QLandmarkIdFetchRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = error;
    rd->landmarkIds = result;
    bool emitState = rd->state != newState;
    rd->state =newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    if (!req)
        return;
    QLandmarkFetchRequestPrivate * rd = static_cast<QLandmarkFetchRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = error;
    rd->landmarks = result;
    bool emitState = rd->state != newState;
    rd->state =newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkRemoveRequest \a req with the operation \a error and
    \a errorString and map of input index to individual errors, \a errorMap.  In addition,
    the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error, const QString &errorString,
        const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkRemoveRequestPrivate* rd = static_cast<QLandmarkRemoveRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    rd->errorMap = errorMap;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkSaveRequest \a req with the latest \a result, operation \a error
    and \a errorString, and map of input index to individual errors, \a errorMap.
    In addition, the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result, QLandmarkManager::Error error,
        const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkSaveRequestPrivate* rd = static_cast<QLandmarkSaveRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    rd->errorMap = errorMap;
    rd->landmarks = result;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkCategoryIdFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest* req, const QList<QLandmarkCategoryId>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)

{
    QLandmarkCategoryIdFetchRequestPrivate* rd = static_cast<QLandmarkCategoryIdFetchRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    rd->categoryIds = result;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkCategoryFetchRequest \a req with the latest \a result,
    and operation \a error and \a errorString.  In addition, the state of the request
    will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the
    request progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest* req, const QList<QLandmarkCategory>& result,
        QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)

{
    QLandmarkCategoryFetchRequestPrivate* rd = static_cast<QLandmarkCategoryFetchRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    rd->categories = result;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkCategoryRemoveRequest \a req with the operation \a error and
    \a errorString and map of input index to individual errors, \a errorMap.  In addition,
    the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest* req, QLandmarkManager::Error error, const QString &errorString,
        const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkRemoveRequestPrivate* rd = static_cast<QLandmarkRemoveRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    rd->errorMap = errorMap;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkSaveCategoryRequest \a req with the latest \a result, operation error \a error
    and \a errorString, and map of input index to individual errors, \a errorMap.
    In addition, the state of the request will be changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
 */
void QLandmarkManagerEngine::updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory>& result, QLandmarkManager::Error error,
        const QString &errorString, const QMap<int, QLandmarkManager::Error>& errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkCategorySaveRequestPrivate* rd = static_cast<QLandmarkCategorySaveRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    rd->categories = result;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkImportRequest \a req with the operation \a error and \a errorString.
    In addition the state of the request is changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify the clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
*/
void QLandmarkManagerEngine::updateLandmarkImportRequest(QLandmarkImportRequest *req, QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    QLandmarkImportRequestPrivate* rd = static_cast<QLandmarkImportRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
    Updates the given QLandmarkExportRequest \a req with the operation \a error and \a errorString.
    In addition the state of the request is changed to \a newState.

    It then causes the request to emit its resultsAvailable() signal to notify the clients of the request
    progress.

    If the new request state is different from the previous state, the stateChanged() signal will
    also be emitted from the request.
*/
void QLandmarkManagerEngine::updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
        QLandmarkAbstractRequest::State newState)
{
    QLandmarkExportRequestPrivate* rd = static_cast<QLandmarkExportRequestPrivate*>(req->d_ptr);
    rd->error = error;
    rd->errorString = errorString;
    bool emitState = rd->state != newState;
    rd->state = newState;
    emit req->resultsAvailable();
    if (emitState)
        emit req->stateChanged(newState);
}

/*!
  Compares two landmarks (\a a and \a b) using the given list of \a sortOrders.  Returns a negative number if \a a should appear
  before \a b according to the sort order, a positive number if \a a should appear after \a b according to the sort order,
  and zero if the two are unable to be sorted.
 */
int QLandmarkManagerEngine::compareLandmark(const QLandmark& a, const QLandmark& b, const QList<QLandmarkSortOrder>& sortOrders)
{
    int comparison = 0;
    for(int i=0; i < sortOrders.count(); ++i) {

        switch (sortOrders.at(i).type())
        {
            case (QLandmarkSortOrder::NameSort):
            {
                const QLandmarkNameSort nameSort = sortOrders.at(i);
                comparison = compareName(a, b, nameSort);
                break;
            }
            case (QLandmarkSortOrder::DistanceSort):
            {
                const QLandmarkDistanceSort distanceSort = sortOrders.at(i);
                comparison = compareName(a, b, distanceSort);
                break;
            }
            default:
                comparison =0;
        }

        if (comparison != 0)
            break;
    }
    return comparison;
}


/*!
  Compares two landmarks (\a a and \a b) by name.

  Returns a negative number if \a a should appear before \a b according to the \a nameSort,
  a positive number if \a a should appear after \a b according to the \a nameSort,
  and zero if the two are unable to be sorted.

  Assuming an ascending order sort, an integer less than, equal to, or greater than zero
  is returned if \a a is less than, equal to or greater than \a b.

  \a nameSort specifies whether an ascending or descending order is used and whether
  the sort is case sensitive or not.
 */
int QLandmarkManagerEngine::compareName(const QLandmark &a, const QLandmark &b, const QLandmarkNameSort &nameSort)
{
    int result = QString::compare(a.name(), b.name(), nameSort.caseSensitivity());

    if (nameSort.direction() == Qt::DescendingOrder)
        result *= -1;

    return result;
}

/*!
  Compares two landmarks (\a a and \a b) by distance from a point given by \a distanceSort.
  Returns a negative number if \a a should appear before \a b according to the sort order,
  a positive number if \a a should appear after \a b according to the sort order,
  and zero if the two are unable to be sorted.

  Assuming an ascending order sort, a negative number is returned if \a is closer
  and a positive number if \b is considered closer.  0 is returned if both are the same
  distance away.

  A invalid coordinate is considered to be an infinite distance away.
 */
int QLandmarkManagerEngine::compareDistance(const QLandmark &a, const QLandmark &b, const QLandmarkDistanceSort &distanceSort)
{
    int result = 0;

    if (!distanceSort.coordinate().isValid())
        return result;

    if (a.coordinate().isValid()) {
        if (b.coordinate().isValid()) {
            double da = distanceSort.coordinate().distanceTo(a.coordinate());
            double db = distanceSort.coordinate().distanceTo(b.coordinate());

            if (da == db) { //TODO: Fix need to find a proper way to compare doubles
                result = 0;
            } else if (da < db) {
                result = -1;
            } else if (da > db) {
                result = 1;
            }
        } else {
            result = -1;
        }
    } else {
        if (b.coordinate().isValid()) {
            result = 1;
        } else {
            result = 0;
        }
    }

    if (distanceSort.direction() == Qt::DescendingOrder)
        result *= -1;

    return result;
}

/*!
  Performs insertion sort \a landmark into the \a sorted list, according to the provided \a sortOrders list.
  The first QLandmarkSortOrder in the list has the highest priority; if the \a landmark is deemed equal to another
  in the \a sorted list, the second QLandmarkSortOrder in the list is used (and so on until either the landmark is inserted
  or there are no more sort order objects in the list).
 */
void QLandmarkManagerEngine::addSorted(QList<QLandmark>* sorted, const QLandmark& landmark, const QList<QLandmarkSortOrder>& sortOrders)
{
}

/*!
  Returns true if the supplied \a landmark matches the supplied \a filter.
 */
bool QLandmarkManagerEngine::testFilter(const QLandmarkFilter& filter, const QLandmark& landmark)
{
    switch(filter.type()) {
        case QLandmarkFilter::DefaultFilter:
            return true;

        case QLandmarkFilter::AttributeFilter:
        {
            const QLandmarkAttributeFilter attribFilter(filter);
            QStringList filterKeys = attribFilter.attributeKeys();
            QStringList landmarkKeys = attribFilter.attributeKeys();

            foreach(const QString filterKey, filterKeys)
            {
                if (landmarkKeys.contains(filterKey)) {
                    if (!attribFilter.attribute(filterKey).isValid())
                        continue;

                    if (attribFilter.attribute(filterKey) == landmark.attribute(filterKey))
                        continue;
                    else
                        return false;
                } else {
                    return false;
                }
            }
            return true;
        }
        case QLandmarkFilter::BoxFilter:
        {
            const QLandmarkBoxFilter boxFilter(filter);
            if (!boxFilter.topLeftCoordinate().isValid())
                return false;

            if (!boxFilter.bottomRightCoordinate().isValid())
                return false;

            double tly = boxFilter.topLeftCoordinate().latitude();
            double bry = boxFilter.bottomRightCoordinate().latitude();
            double tlx = boxFilter.topLeftCoordinate().longitude();
            double brx = boxFilter.bottomRightCoordinate().longitude();

            bool latWrap = (tly < bry);
            bool longWrap = (tlx > brx);

            if (latWrap)
                return false;

            //check if landmark is outside the box's latitudes
            if ( landmark.coordinate().latitude() < bry && landmark.coordinate().latitude() > tly)
                return false;

            if (longWrap) {
                if (landmark.coordinate().longitude() >= brx || landmark.coordinate().longitude() <= tlx)
                    return false;
            } else {
                if (landmark.coordinate().longitude() < tlx || landmark.coordinate().longitude() > brx)
                    return false;
            }

            //landmark must be within the bounds to reach here.
            return true;
        }
    }
    return false;
}

/*! Sorts the given list of \a landmarks according to the provided \a sortOrders */
QList<QLandmarkId> QLandmarkManagerEngine::sortLandmarks(const QList<QLandmark>& landmarks, const QList<QLandmarkSortOrder>& sortOrders)
{
    return QList<QLandmarkId>();
}

#include "moc_qlandmarkmanagerengine.cpp"

QTM_END_NAMESPACE
