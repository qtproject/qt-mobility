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

#include "qlandmarkmanagerengine_symbian.h"
#include "qlandmarkmanagerengine_symbian_p.h"
#include "qlandmarkutility.h"

#include <QDebug>

/*!
 Constructs a landmark manager engine.
 */
QLandmarkManagerEngineSymbian::QLandmarkManagerEngineSymbian(const QString &filename)
{
    d_ptr = new LandmarkManagerEngineSymbianPrivate(*this, filename);
}

/*!
 Destructor.
 */
QLandmarkManagerEngineSymbian::~QLandmarkManagerEngineSymbian()
{
    delete d_ptr;
    d_ptr = NULL;
}

/*! Returns the manager name for this QLandmarkManagerEngineSymbian */
QString QLandmarkManagerEngineSymbian::managerName() const
{
    return d_ptr->managerName();
}

/*!
 Returns the parameters with which this engine was constructed.  Note that
 the engine may have discarded unused or invalid parameters at the time of
 construction, and these will not be returned.
 */
QMap<QString, QString> QLandmarkManagerEngineSymbian::managerParameters() const
{
    return d_ptr->managerParameters();
}

/*!
 Returns the unique URI of this manager, which is built from the manager name and the parameters
 used to construct it.
 */
QString QLandmarkManagerEngineSymbian::managerUri() const
{
    return d_ptr->managerUri();
}

/*!
 Returns the engine backend implementation version number
 */
int QLandmarkManagerEngineSymbian::managerVersion() const
{
    return d_ptr->managerVersion();
}

/* Filtering */
QList<QLandmarkId> QLandmarkManagerEngineSymbian::landmarkIds(const QLandmarkFilter &filter,
    int limit, int offset, const QList<QLandmarkSortOrder>& sortOrders,
    QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->landmarkIds(filter, limit, offset, sortOrders, error, errorString);
}

QList<QLandmarkCategoryId> QLandmarkManagerEngineSymbian::categoryIds(int limit, int offset,
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->categoryIds(limit, offset, nameSort, error, errorString);
}

/* Retrieval */
QLandmark QLandmarkManagerEngineSymbian::landmark(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->landmark(landmarkId, error, errorString);
}

QList<QLandmark> QLandmarkManagerEngineSymbian::landmarks(const QList<QLandmarkId> &landmarkIds,
    QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
    QString *errorString) const
{
    return d_ptr->landmarks(landmarkIds, errorMap, error, errorString);
}

QList<QLandmark> QLandmarkManagerEngineSymbian::landmarks(const QLandmarkFilter &filter, int limit,
    int offset, const QList<QLandmarkSortOrder>& sortOrders, QLandmarkManager::Error *error,
    QString *errorString) const
{
    return d_ptr->landmarks(filter, limit, offset, sortOrders, error, errorString);
}

QLandmarkCategory QLandmarkManagerEngineSymbian::category(
    const QLandmarkCategoryId &landmarkCategoryId, QLandmarkManager::Error *error,
    QString *errorString) const
{
    return d_ptr->category(landmarkCategoryId, error, errorString);
}

QList<QLandmarkCategory> QLandmarkManagerEngineSymbian::categories(
    const QList<QLandmarkCategoryId> &landmarkCategoryIds,
    QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
    QString *errorString) const
{
    return d_ptr->categories(landmarkCategoryIds, errorMap, error, errorString);
}

QList<QLandmarkCategory> QLandmarkManagerEngineSymbian::categories(int limit, int offset,
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->categories(limit, offset, nameSort, error, errorString);
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
bool QLandmarkManagerEngineSymbian::saveLandmark(QLandmark* landmark,
    QLandmarkManager::Error *error, QString *errorString)
{
    return d_ptr->saveLandmark(landmark, error, errorString);
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

 \sa QLandmarkManagerEngineSymbian::saveLandmark()
 */
bool QLandmarkManagerEngineSymbian::saveLandmarks(QList<QLandmark> * landmarks, QMap<int,
    QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error, QString *errorString)
{
    return d_ptr->saveLandmarks(landmarks, errorMap, error, errorString);
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
bool QLandmarkManagerEngineSymbian::removeLandmark(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString)
{
    return d_ptr->removeLandmark(landmarkId, error, errorString);
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

 \sa QLandmarkManagerEngineSymbian::removeLandmark()
 */
bool QLandmarkManagerEngineSymbian::removeLandmarks(const QList<QLandmarkId> &landmarkIds, QMap<
    int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error, QString *errorString)
{
    return d_ptr->removeLandmarks(landmarkIds, errorMap, error, errorString);
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
bool QLandmarkManagerEngineSymbian::saveCategory(QLandmarkCategory* category,
    QLandmarkManager::Error *error, QString *errorString)
{
    return d_ptr->saveCategory(category, error, errorString);
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
bool QLandmarkManagerEngineSymbian::removeCategory(const QLandmarkCategoryId &categoryId,
    QLandmarkManager::Error *error, QString *errorString)
{
    return d_ptr->removeCategory(categoryId, error, errorString);
}

bool QLandmarkManagerEngineSymbian::importLandmarks(QIODevice *device, const QString& format,
    QLandmarkManager::TransferOption option, const QLandmarkCategoryId& id,
    QLandmarkManager::Error *error, QString *errorString)
{
    return d_ptr->importLandmarks(device, format, option, id, error, errorString);
}

bool QLandmarkManagerEngineSymbian::exportLandmarks(QIODevice *device, const QString& format,
    const QList<QLandmarkId> &landmarkIds, QLandmarkManager::TransferOption option,
    QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->exportLandmarks(device, format, landmarkIds, option, error, errorString);
}

/*!
 Returns platform wide supported landmarks package formats can be used for import/export
 */
QStringList QLandmarkManagerEngineSymbian::supportedFormats(
    QLandmarkManager::TransferOperation operation, QLandmarkManager::Error *error,
    QString *errorString) const
{
    return d_ptr->supportedFormats(operation, error, errorString);
}

/*! Returns the support level the manager engine provides for the given \a filter.
 */
QLandmarkManager::SupportLevel QLandmarkManagerEngineSymbian::filterSupportLevel(
    const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->filterSupportLevel(filter, error, errorString);
}

/*! Returns the support level the manager engine provides for the given \a sort order.
 */
QLandmarkManager::SupportLevel QLandmarkManagerEngineSymbian::sortOrderSupportLevel(const
    QLandmarkSortOrder &sortOrder, QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->sortOrderSupportLevel(sortOrder, error, errorString);
}

/*!
 Returns true if the manager engine supports the given \a feature, otherwise returns false;
 */

bool QLandmarkManagerEngineSymbian::isFeatureSupported(QLandmarkManager::ManagerFeature feature,
    QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->isFeatureSupported(feature, error, errorString);
}

/*!
 Returns true if the manager engine is exclusively read only.  Meaning
 landmarks and categories cannot be added, modified or removed
 */
bool QLandmarkManagerEngineSymbian::isReadOnly(QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->isReadOnly(error, errorString);
}

/*!
 Returns true if the landmark identified by \a landmarkId
 considered read-only by the manager engine.

 If the \a landmarkId does not refer to an existing landmark,
 it is considered writable unless the manager engine. is exclusively read-only.

 */
bool QLandmarkManagerEngineSymbian::isReadOnly(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->isReadOnly(landmarkId, error, errorString);
}

/*!
 Returns true if the category identified by \a categoryId is
 considered read-only by the manager engine.

 If \a categoryId does not refer to an existing category,
 it is considered writable unless the manager engine is exclusively read-only.
 */
bool QLandmarkManagerEngineSymbian::isReadOnly(const QLandmarkCategoryId &categoryId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    return d_ptr->isReadOnly(categoryId, error, errorString);
}

/*!
 Notifies the manager engine that the givan \a request has been destroyed.
 */
void QLandmarkManagerEngineSymbian::requestDestroyed(QLandmarkAbstractRequest* request)
{
    d_ptr->requestDestroyed(request);
}

/*!
 Asks the manager engine to begin the given \a request
 which is currently in a re(startable) state.

 Returns true if the request was started successfully,
 else returns false.

 \sa QLandmarkAbstractRequest::start()
 */
bool QLandmarkManagerEngineSymbian::startRequest(QLandmarkAbstractRequest* request)
{
    return d_ptr->startRequest(request);
}

/*!
 Asks the manager engine to cancel the given \a request which was
 previously started and is currently in a cancellable state.
 Returns true if cancellation of the request was started successfully,
 otherwise returns false.

 \sa startRequest(), QLandmarkAbstractRequest::cancel()
 */
bool QLandmarkManagerEngineSymbian::cancelRequest(QLandmarkAbstractRequest* request)
{
    return d_ptr->cancelRequest(request);
}

/*!
 Blocks until the manager engine has completed the given \a request
 which was previously started, or until \a msecs milliseconds have passed.
 Returns true if the request was completed, and false if the request was not in the
 \c QLandmarkAbstractRequest::Active state or no progress could be reported.

 \sa startRequest()
 */
bool QLandmarkManagerEngineSymbian::waitForRequestFinished(QLandmarkAbstractRequest* request,
    int msecs)
{
    return d_ptr->waitForRequestFinished(request, msecs);
}

// non pure virtual methods impl
QStringList QLandmarkManagerEngineSymbian::landmarkAttributeKeys(QLandmarkManager::Error *error,
    QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return LandmarkUtility::landmarkAttributeKeys();
}

QStringList QLandmarkManagerEngineSymbian::categoryAttributeKeys(QLandmarkManager::Error *error,
    QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return LandmarkUtility::categoryAttributeKeys();
}

QStringList QLandmarkManagerEngineSymbian::searchableLandmarkAttributeKeys(
    QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return LandmarkUtility::searchableLandmarkAttributeKeys();
}

//protected methods

void QLandmarkManagerEngineSymbian::updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req,
    const QList<QLandmarkId>& result, QLandmarkManager::Error error, const QString &errorString,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkIdFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkFetchRequest(QLandmarkFetchRequest* req,
    const QList<QLandmark>& result, QLandmarkManager::Error error, const QString &errorString,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkSaveRequest(QLandmarkSaveRequest* req,
    const QList<QLandmark>& result, QLandmarkManager::Error error, const QString &errorString,
    const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkSaveRequest(req, result, error, errorString, errorMap,
        newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req,
    QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkRemoveRequest(req, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSymbian::updateRequestState(QLandmarkAbstractRequest *req,
    QLandmarkAbstractRequest::State state)
{
    QLandmarkManagerEngine::updateRequestState(req, state);
}

void QLandmarkManagerEngineSymbian::updateLandmarkCategoryIdFetchRequest(
    QLandmarkCategoryIdFetchRequest* req, const QList<QLandmarkCategoryId>& result,
    QLandmarkManager::Error error, const QString &errorString,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryIdFetchRequest(req, result, error, errorString,
        newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkCategoryFetchRequest(
    QLandmarkCategoryFetchRequest* req, const QList<QLandmarkCategory>& result,
    QLandmarkManager::Error error, const QString &errorString,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryFetchRequest(req, result, error, errorString,
        newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkCategorySaveRequest(
    QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory>& result,
    QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategorySaveRequest(req, result, error, errorString,
        errorMap, newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkCategoryRemoveRequest(
    QLandmarkCategoryRemoveRequest* req, QLandmarkManager::Error error, const QString &errorString,
    const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryRemoveRequest(req, error, errorString, errorMap,
        newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkFetchByIdRequest(QLandmarkFetchByIdRequest* req,
    const QList<QLandmark>& result, QLandmarkManager::Error error, const QString &errorString,
    const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkFetchByIdRequest(req, result, error, errorString,
        errorMap, newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkCategoryFetchByIdRequest(
    QLandmarkCategoryFetchByIdRequest *req, const QList<QLandmarkCategory>& result,
    QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryFetchByIdRequest(req, result, error, errorString,
        errorMap, newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkImportRequest(QLandmarkImportRequest *req,
    const QList<QLandmarkId> &ids, QLandmarkManager::Error error, const QString &errorString,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkImportRequest(req, ids, error, errorString, newState);
}

void QLandmarkManagerEngineSymbian::updateLandmarkExportRequest(QLandmarkExportRequest *req,
    QLandmarkManager::Error error, const QString &errorString,
    QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkExportRequest(req, error, errorString, newState);
}

QList<QLandmarkId> QLandmarkManagerEngineSymbian::sortLandmarks(const QList<QLandmark>& landmarks,
    const QList<QLandmarkSortOrder> &sortOrders)
{
    return QLandmarkManagerEngine::sortLandmarks(landmarks, sortOrders);
}

void QLandmarkManagerEngineSymbian::handleLandmarkEvent(LandmarkEventObserver::lmEvent event,
    QList<QLandmarkId> landmarkIds, QList<QLandmarkCategoryId> categoryIds)
{
    switch (event) {
    case LandmarkEventObserver::landmarkAdded:
    {
        emit landmarksAdded(landmarkIds);
        break;
    }
    case LandmarkEventObserver::landmarkUpdated:
    {
        emit landmarksChanged(landmarkIds);
        break;
    }
    case LandmarkEventObserver::landmarkRemoved:
    {
        emit landmarksRemoved(landmarkIds);
        break;
    }
    case LandmarkEventObserver::categoryAdded:
    {
        emit categoriesAdded(categoryIds);
        break;
    }
    case LandmarkEventObserver::categoryUpdated:
    {
        emit categoriesChanged(categoryIds);
        break;
    }
    case LandmarkEventObserver::categoryRemoved:
    {
        emit categoriesRemoved(categoryIds);
        break;
    }
    case LandmarkEventObserver::unknownChanges:
    {
        emit dataChanged();
        break;
    }
    } // switch closure
}

// end of file

