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
#include "qlandmarkmanagerengine_symbian_p.h"
// system includes
#include <qlandmark.h>
#include <qlandmarkid.h>
#include <qlandmarkcategory.h>
#include <qlandmarkcategoryid.h>
#include <qgeoplace.h>
#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <qlandmarknamefilter.h>
#include <qlandmarkproximityfilter.h>
#include <qlandmarkcategoryfilter.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkintersectionfilter.h>
#include <qlandmarkunionfilter.h>
#include <qlandmarkattributefilter.h>
#include <qlandmarksortorder.h>
#include <qlandmarknamesort.h>
#include <qlandmarkdistancesort.h>
#include <qlandmarkidfilter.h>
#include <qlandmarkabstractrequest.h>
#include <qlandmarkidfetchrequest.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarksaverequest.h>
#include <qlandmarkremoverequest.h>
#include <qlandmarkcategoryidfetchrequest.h>
#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategorysaverequest.h>
#include <qlandmarkcategoryremoverequest.h>
#include <qlandmarkimportrequest.h>
#include <qlandmarkexportrequest.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QSet>
#include <qnumeric.h>
// symbian landmarks apis includes
#include <EPos_Landmarks.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkCategory.h>
#include <EPos_CPosLmOperation.h>
#include <epos_poslandmarkserialization.h>
#include <EPos_CPosLandmarkEncoder.h>
#include <EPos_CPosLandmarkParser.h>
#include <EPos_CPosLmPartialReadParameters.h>
#include <epos_poslmcategoryserialization.h>
#include <EPos_CPosLmItemIterator.h>
#include <EPos_TPosLmSortPref.h>
#include <EPos_TPosLmDatabaseEvent.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmCategoryManager.h>
#include <EPos_CPosLmDatabaseManager.h>
// for searching
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmTextCriteria.h>
#include <EPos_CPosLmDisplayData.h>
#include <EPos_CPosLmItemIterator.h>
#include <EPos_CPosLmCatNameCriteria.h> 
#include <EPos_CPosLmNearestCriteria.h> 
#include <EPos_CPosLmIdListCriteria.h> 
#include <EPos_CPosLmCompositeCriteria.h> 
// user includes
#include "qlandmarkutility.h"
#include "qlandmarkmanagerengine_symbian.h"
// constants
_LIT(KDefaultTextCriteria,"*");
_LIT(KDefaultSpaceTextSearch,"* ");
_LIT(KWildChar,"*.*");
const TUint KExtrachars = 3;

/**
 * Constructs CLandmarkDbEventHandler
 */
class CLandmarkDbEventHandler LandmarkManagerEngineSymbianPrivate::iEventHandler;
LandmarkRequestHandler LandmarkManagerEngineSymbianPrivate::m_RequestHandler;

/*!
 Constructs a landmark manager engine.
 */
LandmarkManagerEngineSymbianPrivate::LandmarkManagerEngineSymbianPrivate(
    LandmarkEventObserver& lmOpObserver, const QString &filename) :
    m_dbFilename(filename), m_LmEventObserver(lmOpObserver)
{
    QT_TRAP_THROWING (
        // open default Landmark database
        m_LandmarkDb = CPosLandmarkDatabase::OpenL();
        // initialize the Landmark database
        ExecuteAndDeleteLD(m_LandmarkDb->InitializeL());
        // create Category Manager
        m_LandmarkCatMgr = CPosLmCategoryManager::NewL(*m_LandmarkDb);
    );

}

/*!
 Destructor.
 */
LandmarkManagerEngineSymbianPrivate::~LandmarkManagerEngineSymbianPrivate()
{
    delete m_LandmarkCatMgr;
    delete m_LandmarkDb;
    ReleaseLandmarkResources();
}

/*! Returns the manager name for this LandmarkManagerEngineSymbianPrivate */
QString LandmarkManagerEngineSymbianPrivate::managerName() const
{
    return QString("com.nokia.qt.landmarks.engines.symbian");
}

/*!
 Returns the parameters with which this engine was constructed.  Note that
 the engine may have discarded unused or invalid parameters at the time of
 construction, and these will not be returned.
 */
QMap<QString, QString> LandmarkManagerEngineSymbianPrivate::managerParameters() const
{
    QMap<QString, QString> parameters;
    parameters["filename"] = QString("file://C:eposlm.ldb");
    return parameters;
}

/*!
 Returns the engine backend implementation version number
 */
int LandmarkManagerEngineSymbianPrivate::managerVersion() const
{
    return 1;
}

/*!
 Returns the unique URI of this manager, which is built from the manager name and the parameters
 used to construct it.
 */
QString LandmarkManagerEngineSymbianPrivate::managerUri() const
{
    return QLandmarkManager::buildUri(managerName(), managerParameters(), managerVersion());
}

/**
 * Sets the landmark attribute keys defined by QLandmarkManagerEngine 
 */
void LandmarkManagerEngineSymbianPrivate::setLandmarkAttributeKeys(QStringList landmarkKeys)
{
    m_LandmarkAttributeKeys = landmarkKeys;
}

/*!
 Returns a list of landmark ids of landmarks that match the given \a filter, sorted
 according to the given \a sortOrders.  Depending on the backend, this filtering operation
 may involve retrieving all the landmarks.  Various fetch operation parameters may be specified by \a fetchHint.

 Any error which occurs will be saved in \a error and \a errorString.
 */
QList<QLandmarkId> LandmarkManagerEngineSymbianPrivate::landmarkIds(const QLandmarkFilter &filter,
    int limit, int offset, const QList<QLandmarkSortOrder>& sortOrders,
    QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    // This will hold the landmark Ids that will be returned to calling function
    QList<QLandmarkId> result;

    //fetchRequired will prevent multiple fetches from database
    bool sortRequired = false;

    // check emulated sorting is required or not
    if (sortOrders.size() > 1) {
        for (int i = 0; i < sortOrders.size(); ++i) {
            if (sortOrders.at(i).type() != QLandmarkSortOrder::NameSort) {
                sortRequired = true;
                break;
            }
        }
    }

    int maxMatches = limit + offset;

    switch (filter.type()) {
    case QLandmarkFilter::DefaultFilter:
    case QLandmarkFilter::NameFilter:
    case QLandmarkFilter::LandmarkIdFilter:
    case QLandmarkFilter::CategoryFilter:
    case QLandmarkFilter::BoxFilter:
    case QLandmarkFilter::ProximityFilter:
    case QLandmarkFilter::AttributeFilter:
    {
        if (filter.type() == QLandmarkFilter::LandmarkIdFilter && sortRequired) {
            QLandmarkIdFilter lmIdFilter = filter;
            result = lmIdFilter.landmarkIds();
            break;
        }
        int err;
        if (sortOrders.size() > 0) {
            TRAP(err, result = searchWithFilterL(filter,sortOrders.at(0),maxMatches);)
        }
        else {
            TRAP(err, result = searchWithFilterL(filter,QLandmarkSortOrder(),maxMatches);)
        }
        handleSymbianError(err, error, errorString);
        break;
    }
    case QLandmarkFilter::IntersectionFilter:
    {
        QLandmarkIntersectionFilter intersectionFilter = filter;
        QList<QLandmarkFilter> filters = intersectionFilter.filters();

        if (filters.size() == 0) {
            //do nothing
        }
        else if (filters.size() == 1) {
            result = landmarkIds(filters.at(0), limit, offset, QList<QLandmarkSortOrder> (), error,
                errorString);
            if (*error != QLandmarkManager::NoError) {
                result.clear();
                return result;
            }
        }
        else {
            QSet<QString> ids;
            QList<QLandmarkId> firstResult = landmarkIds(filters.at(0), limit, offset, QList<
                QLandmarkSortOrder> (), error, errorString);

            if (*error != QLandmarkManager::NoError) {
                result.clear();
                return result;
            }

            for (int j = 0; j < firstResult.size(); ++j) {
                if (firstResult.at(j).isValid())
                    ids.insert(firstResult.at(j).localId());
            }

            for (int i = 1; i < filters.size(); ++i) {
                QList<QLandmarkId> subResult = landmarkIds(filters.at(i), limit, offset, QList<
                    QLandmarkSortOrder> (), error, errorString);

                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
                QSet<QString> subIds;
                for (int j = 0; j < subResult.size(); ++j) {
                    if (subResult.at(j).isValid())
                        subIds.insert(subResult.at(j).localId());
                }
                ids &= subIds;
            }

            QList<QString> idList = ids.toList();
            for (int i = 0; i < idList.size(); ++i) {
                QLandmarkId id;
                id.setManagerUri(managerUri());
                id.setLocalId(idList.at(i));
                result << id;
            }
        }
        break;
    }
    case QLandmarkFilter::UnionFilter:
    {
        QLandmarkUnionFilter unionFilter = filter;
        QSet<QString> ids;
        QList<QLandmarkFilter> filters = unionFilter.filters();

        if (filters.size() == 0) {
            //do nothing
        }
        else if (filters.size() == 1) {
            result = landmarkIds(filters.at(0), limit, offset, QList<QLandmarkSortOrder> (), error,
                errorString);
            if (*error != QLandmarkManager::NoError) {
                result.clear();
                return result;
            }
        }
        else {
            for (int i = 0; i < filters.size(); ++i) {
                QList<QLandmarkId> subResult = landmarkIds(filters.at(i), limit, offset, QList<
                    QLandmarkSortOrder> (), error, errorString);

                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }

                for (int j = 0; j < subResult.size(); ++j) {
                    if (subResult.at(j).isValid())
                        ids.insert(subResult.at(j).localId());
                }
            }

            QList<QString> idList = ids.toList();
            for (int i = 0; i < idList.size(); ++i) {
                QLandmarkId id;
                id.setManagerUri(managerUri());
                id.setLocalId(idList.at(i));
                result << id;
            }
        }
        break;
    }
    default:
    {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = QString("Unrecognised filter type");
        result.clear();
        return result;
    }
    }//switch closure

    int resultcount = result.size();
    // if no search data found return empty list
    if (resultcount <= 0)
        return result;

    if (offset > resultcount) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = QString("Invalid fetchHint data, Invalid offset value.");

        return QList<QLandmarkId> ();
    }

    sortFetchedLmIds(limit, offset, sortOrders, result, error, errorString);
    return result;
}

/*!
 Returns a list of all category identifiers.  Any error which occurs will be saved in \a
 error and \a errorString.
 */
QList<QLandmarkCategoryId> LandmarkManagerEngineSymbianPrivate::categoryIds(int limit, int offset,
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmarkCategoryId> qtCatIds;
    TRAPD(err, qtCatIds = fetchCategoryIdsL(nameSort, error, errorString);)
    if (err == KErrNone) {
        // do fetch based on offset and max items
        int resultcount = qtCatIds.size();
        if (limit > 0) {
            if (offset < 0)
                offset = 0;
            return qtCatIds.mid(offset, limit);
        }
        else if (offset > 0) {
            return qtCatIds.mid(offset, resultcount);
        }
        else
            return qtCatIds;
    }
    else {
        handleSymbianError(err, error, errorString);
        return QList<QLandmarkCategoryId> ();
    }
}

/*!
 Returns the landmark in the datastore identified by \a landmarkId.

 Any errors encountered are:stored in \a error and \a errorString.
 */
QLandmark LandmarkManagerEngineSymbianPrivate::landmark(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    TPosLmItemId symbianLmId = LandmarkUtility::convertToSymbianLandmarkId(landmarkId);
    CPosLandmark* symbianLandmark = NULL;
    TRAPD(err, symbianLandmark = m_LandmarkDb->ReadLandmarkLC(symbianLmId);
        if(symbianLandmark != NULL) CleanupStack::Pop(symbianLandmark);)

    handleSymbianError(err, error, errorString);

    if (err == KErrNone) {
        QLandmark* qtLandmark = LandmarkUtility::convertToQtLandmark(managerUri(), symbianLandmark);
        delete symbianLandmark;
        symbianLandmark = NULL;

        return *qtLandmark;
    }
    else {
        return QLandmark();
    }
}

QList<QLandmark> LandmarkManagerEngineSymbianPrivate::landmarks(
    const QList<QLandmarkId> &landmarkIds, QMap<int, QLandmarkManager::Error> *errorMap,
    QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmark> result;
    if (&landmarkIds == 0 || landmarkIds.isEmpty()) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid landmark ids or empty ids";
        return result;
    }

    QLandmark lm;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString = "";
    for (int i = 0; i < landmarkIds.count(); ++i) {
        lm = landmark(landmarkIds.at(i), &lastError, &lastErrorString);
        if (lastError == QLandmarkManager::NoError) {
            result << lm;
        }
        else {
            if (errorMap)
                errorMap->insert(i, lastError);
        }
    }

    *error = lastError;
    *errorString = lastErrorString;
    return result;
}

/*!
 Returns a list of landmarks which match the given \a filter and are sorted according to the \a sortOrders.
 Various fetch operation parameters are specified by \a fetchHint.

 Overall operation errors are stored in \a error and \a errorString.
 */
QList<QLandmark> LandmarkManagerEngineSymbianPrivate::landmarks(const QLandmarkFilter &filter,
    int limit, int offset, const QList<QLandmarkSortOrder>& sortOrders,
    QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmark> result;

    QList<QLandmarkId> ids = landmarkIds(filter, limit, offset, sortOrders, error, errorString);
    if (error && *error != QLandmarkManager::NoError) {
        return result;
    }

    QLandmark lm;
    foreach(const QLandmarkId &id, ids)
        {
            lm = landmark(id, error, errorString);
            if (lm.landmarkId().isValid() && *error == QLandmarkManager::NoError) {
                result.append(lm);
            }
            else {
                result.clear();
                return result;
            }
        }

    *error = QLandmarkManager::NoError;
    *errorString = "";
    return result;
}

/*!
 Returns the cateory in the datastore identified by \a categoryId.

 Any errors encountered are stored in \a error and \a errorString.
 */
QLandmarkCategory LandmarkManagerEngineSymbianPrivate::category(
    const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) const
{
    return fetchCategory(categoryId, error, errorString);
}

/*!
 Returns a list of categories identified by \a landmarkCategoryIds.

 Overall operation errors are stored in \a error and \a errorString.
 */
QList<QLandmarkCategory> LandmarkManagerEngineSymbianPrivate::categories(const QList<
    QLandmarkCategoryId> &landmarkCategoryIds, QMap<int, QLandmarkManager::Error> *errorMap,
    QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmarkCategory> result;
    if (&landmarkCategoryIds == 0 || landmarkCategoryIds.isEmpty()) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid category ids or empty ids";
        return result;
    }

    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString = "";
    for (int i = 0; i < landmarkCategoryIds.size(); ++i) {
        QLandmarkCategory cat = fetchCategory(landmarkCategoryIds.operator[](i), &lastError,
            &lastErrorString);

        if (lastError == QLandmarkManager::NoError) {
            result << cat;
        }
        else {
            if (errorMap)
                errorMap->insert(i, lastError);
        }

    }

    *error = lastError;
    *errorString = lastErrorString;

    if (landmarkCategoryIds.size() != result.size()) {
        return QList<QLandmarkCategory> ();
    }
    else {
        return result;
    }
}

/*!
 Returns a list of categories identified by \a landmarkCategoryIds.

 Overall operation errors are stored in \a error and \a errorString.
 */
QList<QLandmarkCategory> LandmarkManagerEngineSymbianPrivate::categories(int limit, int offset,
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmarkCategoryId> qtCatIds;
    TRAPD(err, qtCatIds = fetchCategoryIdsL(nameSort, error, errorString);)
    if (err == KErrNone && *error == QLandmarkManager::NoError && qtCatIds.size() > 0) {
        QList<QLandmarkCategory> qtCats;
        TRAP(err, qtCats = LandmarkUtility::getCategoriesL(m_LandmarkCatMgr, qtCatIds); )
        if (err == KErrNone) {
            // do fetch based on offset and max items
            int resultcount = qtCats.size();
            if (limit > 0) {
                if (offset < 0)
                    offset = 0;
                return qtCats.mid(offset, limit);
            }
            else if (offset > 0) {
                return qtCats.mid(offset, resultcount);
            }
            else
                return qtCats;
        }
    }

    handleSymbianError(err, error, errorString);
    return QList<QLandmarkCategory> ();
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
bool LandmarkManagerEngineSymbianPrivate::saveLandmark(QLandmark* landmark,
    QLandmarkManager::Error *error, QString *errorString)
{
    bool added = false;
    bool changed = false;

    bool result = saveLandmarkInternal(landmark, error, errorString, &added, &changed);

    QList<QLandmarkId> landmarkIds;
    landmarkIds.append(landmark->landmarkId());

    if (added) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkAdded, landmarkIds);
    }
    if (changed) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkUpdated, landmarkIds);
    }

    return result;
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

 \sa LandmarkManagerEngineSymbianPrivate::saveLandmark()
 */
bool LandmarkManagerEngineSymbianPrivate::saveLandmarks(QList<QLandmark> * landmarks, QMap<int,
    QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error, QString *errorString)
{
    QList<QLandmarkId> addedIds;
    QList<QLandmarkId> changedIds;
    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    for (int i = 0; i < landmarks->size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString = "";
        bool added = false;
        bool changed = false;

        bool result = saveLandmarkInternal(&(landmarks->operator [](i)), &loopError,
            &loopErrorString, &added, &changed);

        if (!result) {
            if (errorMap)
                errorMap->insert(i, loopError);

            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        }

        if (added)
            addedIds << landmarks->at(i).landmarkId();
        if (changed)
            changedIds << landmarks->at(i).landmarkId();
    }

    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }
    else {
        if (error)
            *error = lastError;
        if (errorString)
            *errorString = lastErrorString;
    }

    if (addedIds.size() != 0) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkAdded, addedIds);
    }
    if (changedIds.size() != 0) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkUpdated, changedIds);
    }

    return noErrors;
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
bool LandmarkManagerEngineSymbianPrivate::removeLandmark(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString)
{
    bool removed = false;
    bool result = removeLandmarkInternal(landmarkId, error, errorString, &removed);

    if (removed) {
        QList<QLandmarkId> landmarkIds;
        landmarkIds.append(landmarkId);
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkRemoved, landmarkIds);
    }

    return result;
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

 \sa LandmarkManagerEngineSymbianPrivate::removeLandmark()
 */
bool LandmarkManagerEngineSymbianPrivate::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
    QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
    QString *errorString)
{
    QList<QLandmarkId> removedIds;

    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    for (int i = 0; i < landmarkIds.size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString.clear();

        bool result = removeLandmark(landmarkIds.at(i), &loopError, &loopErrorString);

        if (!result) {
            if (errorMap)
                errorMap->insert(i, loopError);

            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        }

        if (result)
            removedIds << landmarkIds.at(i);
    }

    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }
    else {
        if (error)
            *error = lastError;
        if (errorString)
            *errorString = lastErrorString;
    }

    if (removedIds.size() != 0) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkRemoved, removedIds);
    }

    return noErrors;
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
bool LandmarkManagerEngineSymbianPrivate::saveCategory(QLandmarkCategory* category,
    QLandmarkManager::Error *error, QString *errorString)
{
    bool added = false;
    bool changed = false;

    bool result = saveCategoryInternal(category, error, errorString, &added, &changed);

    QList<QLandmarkCategoryId> categoryIds;
    categoryIds.append(category->categoryId());

    if (added) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryAdded, QList<
            QLandmarkId> (), categoryIds);
    }
    if (changed) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryUpdated, QList<
            QLandmarkId> (), categoryIds);
    }

    return result;
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
bool LandmarkManagerEngineSymbianPrivate::removeCategory(const QLandmarkCategoryId &categoryId,
    QLandmarkManager::Error *error, QString *errorString)
{
    bool removed = false;
    bool result = removeCategoryInternal(categoryId, error, errorString, &removed);

    if (removed) {
        QList<QLandmarkCategoryId> categoryIds;
        categoryIds.append(categoryId);
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryRemoved, QList<
            QLandmarkId> (), categoryIds);
    }

    return result;
}

/*!
 Reads landmarks from the given \a device and saves them.  The data from the \a device
 is expected to adhere to the provided \a format.  If no \a format is provided,
 the manager engine tries to autodetect the \a format.

 The \a option can be used to control whether categories in the imported
 file will be added during the import.  If the \c AttachSingleCategory option is used, then
 all the landmarks in the import file are assigned to the category identified by
 \a categoryId, in all other cirumstances \a categoryId is ignored.  If \a categoryId
 doesn't exist when using \c AttachSingleCategory, QLandmarkManager::DoesNotExist error is returned.  Note that
 some file formats may not support categories at all.

 Returns true if all landmarks could be imported, otherwise returns false.
 It may be possible that only a subset of landmarks are imported.

 Overall operational errors are stored in \a error and
 \a errorString.
 */
bool LandmarkManagerEngineSymbianPrivate::importLandmarks(QIODevice *device, const QString &format,
    QLandmarkManager::TransferOption option, const QLandmarkCategoryId& categoryId,
    QLandmarkManager::Error *error, QString *errorString)
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";
    return false;
}

/*!
 Writes landmarks to the given \a device.  The landmarks will be written
 according to the specified \a format.  If  \a landmarkIds is empty, then
 all landmarks will be exported, otherwise only those landmarks that
 match \a landmarkIds will be exported.

 The \a option can be used to control whether categories will be exported or not.
 Note that the \c AttachSingleCategory option has no meaning during
 export and the manager will export as if \a option was \c IncludeCategoryData.
 Also, be aware that some file formats may not support categories at all and for
 these formats, the \a option is always treated as if it was \c ExcludeCategoryData.

 Returns true if all specified landmarks were successfully exported,
 otherwise returns false.  It may be possible that only a subset
 of landmarks are exported.

 Overall operation errors are stored in \a error and
 \a errorString.
 */
bool LandmarkManagerEngineSymbianPrivate::exportLandmarks(QIODevice *device, const QString &format,
    QList<QLandmarkId> landmarkIds, QLandmarkManager::TransferOption option,
    QLandmarkManager::Error *error, QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";
    return false;
}

/*! Returns the support level the manager engine provides for the given \a filter.
 */
QLandmarkManager::SupportLevel LandmarkManagerEngineSymbianPrivate::filterSupportLevel(
    const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    QLandmarkManager::SupportLevel supportLevel = QLandmarkManager::None;

    switch (filter.type()) {
    case QLandmarkFilter::DefaultFilter:
    case QLandmarkFilter::NameFilter:
    case QLandmarkFilter::LandmarkIdFilter:
    case QLandmarkFilter::CategoryFilter:
    case QLandmarkFilter::ProximityFilter:
    case QLandmarkFilter::BoxFilter:
    case QLandmarkFilter::AttributeFilter:
    {
        supportLevel = QLandmarkManager::Native;
        break;
    }
    case QLandmarkFilter::IntersectionFilter:
    case QLandmarkFilter::UnionFilter:
    {
        supportLevel = QLandmarkManager::Emulated;
        break;
    }
    default:
    {
        break;
    }
    }//switch closure

    return supportLevel;
}

/*! Returns the support level the manager engine provides for the given \a sort orders.
 */
QLandmarkManager::SupportLevel LandmarkManagerEngineSymbianPrivate::sortOrderSupportLevel(
    const QList<QLandmarkSortOrder>& sortOrders, QLandmarkManager::Error *error,
    QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    QLandmarkManager::SupportLevel supportLevel = QLandmarkManager::Native;

    if (sortOrders.size() > 1) {
        supportLevel = QLandmarkManager::Emulated;
    }

    return supportLevel;
}

/*!
 Returns true if the manager engine supports the given \a feature, otherwise returns false;
 */
bool LandmarkManagerEngineSymbianPrivate::isFeatureSupported(
    QLandmarkManager::LandmarkFeature feature, QLandmarkManager::Error *error, QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    switch (feature) {
    case (QLandmarkManager::ExtendedAttributes):
    case (QLandmarkManager::CustomAttributes):
        return true;
    default:
        return false;
    }
}

/*!
 Returns true if the manager engine is exclusively read only.  Meaning
 landmarks and categories cannot be added, modified or removed
 */
bool LandmarkManagerEngineSymbianPrivate::isReadOnly(QLandmarkManager::Error *error,
    QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NotSupportedError;
    if (errorString)
        *errorString
            = "Symbian Landmark Datastore doesnt support readonly. But it can restict any updation if already any updation is happening in background";
    return false;
}

/*!
 Returns true if the landmark identified by \a landmarkId
 considered read-only by the manager engine.

 If the \a landmarkId does not refer to an existing landmark,
 it is considered writable unless the manager engine. is exclusively read-only.

 */
bool LandmarkManagerEngineSymbianPrivate::isReadOnly(const QLandmarkId &/*landmarkId*/,
    QLandmarkManager::Error *error, QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";
    return false;
}

/*!
 Returns true if the category identified by \a categoryId is
 considered read-only by the manager engine.

 If \a categoryId does not refer to an existing category,
 it is considered writable unless the manager engine is exclusively read-only.
 */
bool LandmarkManagerEngineSymbianPrivate::isReadOnly(const QLandmarkCategoryId &categoryId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    bool result = false;
    if (LandmarkUtility::validLocalId(categoryId.localId())) {
        result = LandmarkUtility::isGlobalCategoryId(m_LandmarkCatMgr, categoryId);
    }
    return result;
}

/*!
 Returns platform wide supported landmarks package formats can be used for import/export
 */
QStringList LandmarkManagerEngineSymbianPrivate::supportedFormats(
    QLandmarkManager::TransferOperation operation, QLandmarkManager::Error *error,
    QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    QStringList formats;

    if (operation == QLandmarkManager::ImportOperation) {
        formats << QLandmarkManager::Gpx;
        formats << QLandmarkManager::Lmx;
        formats << QLandmarkManager::Kml;
        formats << QLandmarkManager::Kmz;
    }

    if (operation == QLandmarkManager::ExportOperation) {
        //formats << QLandmarkManager::Gpx;
        formats << QLandmarkManager::Lmx;
    }

    return formats;
}

/*!
 Returns whether extended attributes specific to this manager are enabled or not.
 If extended attributes are enabled, retrieved landmarks will have
 extra attribute keys accessible through the QLandmark::attribute() function.
 Extended attributes must be enabled to save any landmarks which possess
 extended attributes.  This same behaviour will also apply to categories
 if extended category attributes are supported.
 Errors are stored in \a error and \a errorString.
 */
bool LandmarkManagerEngineSymbianPrivate::isExtendedAttributesEnabled(
    QLandmarkManager::Error *error, QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    return m_isExtendedAttributesEnabled;
}

/*!
 Sets whether extended attributes are \a enabled or not.
 Errors are stored in \a error and \a errorString.
 */
void LandmarkManagerEngineSymbianPrivate::setExtendedAttributesEnabled(bool enabled,
    QLandmarkManager::Error *error, QString *errorString)
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    m_isExtendedAttributesEnabled = enabled;
}

/*!
 Returns whether custom attributes are enabled or not. Custom attributes
 are arbitrary attributes created by the application for a landmark.
 If custom attributes are enabled (and the manager supports them),
 retrieved landmarks will have extra attributes accessible
 using QLandmark::customAttributes().  Custom attributes must be enabled
 to save any landmarks with possess custom attributes.  This same behaviour
 applies to categories if custom category attributes are supported.
 Errors are stored in \a error and \a errorString.
 */
bool LandmarkManagerEngineSymbianPrivate::isCustomAttributesEnabled(QLandmarkManager::Error *error,
    QString *errorString) const
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    return m_isCustomAttributesEnabled;
}

/*!
 Sets whether custom attributes are \a enabled or not.
 Errors are stored in \a error and \a errorString.
 */
void LandmarkManagerEngineSymbianPrivate::setCustomAttributesEnabled(bool enabled,
    QLandmarkManager::Error *error, QString *errorString)
{
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    m_isCustomAttributesEnabled = enabled;
}

/*!
 Notifies the manager engine that the givan \a request has been destroyed.
 */
void LandmarkManagerEngineSymbianPrivate::requestDestroyed(QLandmarkAbstractRequest* request)
{
    m_RequestHandler.RemoveAsyncRequest(request);
}

/*!
 Asks the manager engine to begin the given \a request
 which is currently in a re(startable) state.

 Returns true if the request was started successfully,
 else returns false.

 \sa QLandmarkAbstractRequest::start()
 */
bool LandmarkManagerEngineSymbianPrivate::startRequest(QLandmarkAbstractRequest* request)
{
    bool result = false;
    TRAPD(errorId, result = startRequestL(request);)
    if (errorId != KErrNone)
        result = false;

    return result;
}

/**
 * start landmark async request
 */
bool LandmarkManagerEngineSymbianPrivate::startRequestL(QLandmarkAbstractRequest* request)
{
    bool result = false;

    switch (request->type()) {
    case QLandmarkAbstractRequest::CategoryIdFetchRequest:
    case QLandmarkAbstractRequest::CategoryFetchRequest:
    {
        if (request->type() == QLandmarkAbstractRequest::CategoryFetchRequest) {

            QLandmarkCategoryFetchRequest* catFetchRequest =
                static_cast<QLandmarkCategoryFetchRequest *> (request);

            CPosLmCategoryManager::TCategorySortPref sortPref =
                CPosLmCategoryManager::ECategorySortOrderNone;

            if (request->type() == QLandmarkAbstractRequest::CategoryFetchRequest) {
                if (catFetchRequest->sorting().direction() == Qt::AscendingOrder) {
                    sortPref = CPosLmCategoryManager::ECategorySortOrderNameAscending;
                }
                else if (catFetchRequest->sorting().direction() == Qt::DescendingOrder) {
                    sortPref = CPosLmCategoryManager::ECategorySortOrderNameDescending;
                }
            }
            else if (request->type() == QLandmarkAbstractRequest::CategoryIdFetchRequest) {

                QLandmarkCategoryIdFetchRequest* catIdFetchRequest =
                    static_cast<QLandmarkCategoryIdFetchRequest *> (request);

                if (catIdFetchRequest->sorting().direction() == Qt::AscendingOrder) {
                    sortPref = CPosLmCategoryManager::ECategorySortOrderNameAscending;
                }
                else if (catIdFetchRequest->sorting().direction() == Qt::DescendingOrder) {
                    sortPref = CPosLmCategoryManager::ECategorySortOrderNameDescending;
                }
            }

            CPosLmCatNameCriteria* catNameCriteria = CPosLmCatNameCriteria::NewLC();
            catNameCriteria->SetSearchPatternL(KDefaultTextCriteria);
            CPosLandmarkSearch* landmarkSearch = CPosLandmarkSearch::NewL(*m_LandmarkDb);
            CleanupStack::PushL(landmarkSearch);
            CPosLmOperation* lmOperation = landmarkSearch->StartCategorySearchL(*catNameCriteria,
                sortPref);

            // create request AO and start async request
            CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this, lmOperation);
            CleanupStack::PushL(requestAO);
            TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
            if (retn != KErrNone) {
                return false;
            }

            // start the request
            requestAO->StartRequest();

            CleanupStack::Pop(requestAO);
            CleanupStack::Pop(landmarkSearch);
            CleanupStack::PopAndDestroy(catNameCriteria);

            // take ownership of search object
            CLandmarkRequestData* requestData = m_RequestHandler.FetchAsyncRequest(request);
            // ownership transfered
            requestData->iLandmarkSearch = landmarkSearch;

            result = true;
            break;
        }

    }
    case QLandmarkAbstractRequest::LandmarkIdFetchRequest:
    case QLandmarkAbstractRequest::LandmarkFetchRequest:
    {
        QLandmarkFilter filter;
        QList<QLandmarkSortOrder> sortOrders;
        int maxMatches = 0;

        if (request->type() == QLandmarkAbstractRequest::LandmarkIdFetchRequest) {
            QLandmarkIdFetchRequest *lmIdFetchRequest =
                static_cast<QLandmarkIdFetchRequest *> (request);

            filter = lmIdFetchRequest->filter();
            sortOrders = lmIdFetchRequest->sorting();
            maxMatches = lmIdFetchRequest->limit() + lmIdFetchRequest->offset();
        }

        if (request->type() == QLandmarkAbstractRequest::LandmarkFetchRequest) {

            QLandmarkFetchRequest *lmFetchRequest = static_cast<QLandmarkFetchRequest *> (request);

            filter = lmFetchRequest->filter();
            sortOrders = lmFetchRequest->sorting();
            maxMatches = lmFetchRequest->limit() + lmFetchRequest->offset();
        }

        if (filter.type() == QLandmarkFilter::LandmarkIdFilter) {

            bool sortRequired = false;

            // check emulated sorting is required or not
            if (sortOrders.size() > 1) {
                for (int i = 0; i < sortOrders.size(); ++i) {
                    if (sortOrders.at(i).type() != QLandmarkSortOrder::NameSort) {
                        sortRequired = true;
                        break;
                    }
                }
            }

            if (sortRequired) {

                // create request AO and start async request
                CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this);
                CleanupStack::PushL(requestAO);
                TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
                if (retn != KErrNone) {
                    CleanupStack::Pop(requestAO);
                    return false;
                }
                // start the request
                requestAO->StartRequest();
                CleanupStack::Pop(requestAO);
                break;
            }
        }

        CPosLmSearchCriteria* searchCriteria = getSearchCriteriaL(filter);
        if (!searchCriteria)
            return false;

        CPosLandmarkSearch* landmarkSearch = NULL;
        CPosLmOperation* lmOperation = getOperationL(landmarkSearch, searchCriteria, sortOrders.at(
            0), maxMatches);

        // create request AO and start async request
        CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this, lmOperation);
        CleanupStack::PushL(requestAO);

        TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
        if (retn != KErrNone) {
            return false;
        }

        // start the request
        requestAO->StartRequest();

        CleanupStack::Pop(requestAO);
        delete searchCriteria;

        // take ownership of search object
        CLandmarkRequestData* requestData = m_RequestHandler.FetchAsyncRequest(request);
        // ownership transfered
        requestData->iLandmarkSearch = landmarkSearch;

        result = true;
        break;
    }
    case QLandmarkAbstractRequest::LandmarkSaveRequest:
    case QLandmarkAbstractRequest::LandmarkRemoveRequest:
    case QLandmarkAbstractRequest::CategorySaveRequest:
    case QLandmarkAbstractRequest::CategoryRemoveRequest:
    {
        CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this);
        CleanupStack::PushL(requestAO);

        TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
        if (retn != KErrNone) {
            CleanupStack::Pop(requestAO);
            return false;
        }
        requestAO->StartRequest();
        CleanupStack::Pop(requestAO);
        result = true;
        break;
    }
    case QLandmarkAbstractRequest::ImportRequest:
    {
        break;
    }
    case QLandmarkAbstractRequest::ExportRequest:
    {
        break;
    }
    default:
    {
        break;
    }
    }// switch closure

    QLandmarkManagerEngineSymbian::updateRequestState(request,
        QLandmarkAbstractRequest::ActiveState);

    return result;
}

/*!
 Asks the manager engine to cancel the given \a request which was
 previously started and is currently in a cancellable state.
 Returns true if cancellation of the request was started successfully,
 otherwise returns false.

 \sa startRequest(), QLandmarkAbstractRequest::cancel()
 */
bool LandmarkManagerEngineSymbianPrivate::cancelRequest(QLandmarkAbstractRequest* request)
{
    CLandmarkRequestData* currentRequestData = NULL;
    currentRequestData = m_RequestHandler.FetchAsyncRequest(request);
    if (!currentRequestData)
        return false;
    return currentRequestData->iOwnerAO->CancelRequest();
}

/*!
 Blocks until the manager engine has completed the given \a request
 which was previously started, or until \a msecs milliseconds have passed.
 Returns true if the request was completed, and false if the request was not in the
 \c QLandmarkAbstractRequest::Active state or no progress could be reported.

 \sa startRequest()
 */
bool LandmarkManagerEngineSymbianPrivate::waitForRequestFinished(QLandmarkAbstractRequest* request,
    int msecs)
{
    CLandmarkRequestData* currentRequestData = NULL;
    currentRequestData = m_RequestHandler.FetchAsyncRequest(request);
    if (!currentRequestData)
        return false;
    return currentRequestData->iOwnerAO->WaitForFinished(msecs);
}

/****
 * 
 * INTERNAL PRIVATE IMPLEMENTATIONS
 * 
 */

/*
 * save single landamrk
 */
bool LandmarkManagerEngineSymbianPrivate::saveLandmarkInternal(QLandmark* landmark,
    QLandmarkManager::Error *error, QString *errorString, bool *added, bool *changed)
{
    bool result = false;

    if (added)
        *added = false;
    if (changed)
        *changed = false;

    if (landmark->name().size() > KPosLmMaxTextFieldLength) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Landmark Name is greater than its maxlength i.e. KPosLmMaxTextFieldLength = 255.";
        return result;
    }

    if (landmark->description().size() > KPosLmMaxDescriptionLength) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Landmark Desciption is greater than its maxlength i.e. KPosLmMaxDescriptionLength = 4095.";
        return result;
    }

    if (!landmark->viewport().isEmpty()) {
        if (error)
            *error = QLandmarkManager::NotSupportedError;
        if (errorString)
            *errorString = "Landmark viewport boundry settings not supported.";
        return result;
    }

    if (!landmark->landmarkId().managerUri().isEmpty() && landmark->landmarkId().managerUri()
        != managerUri()) {
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return result;
    }

    bool categoryExists = LandmarkUtility::validCategoriesExist(m_LandmarkCatMgr, landmark);
    if (!categoryExists) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Landmark contains a category id that does not exist";
        return result;
    }

    CPosLandmark* symbianLandmark = NULL;
    TInt err = KErrNone;

    QLandmarkId landmarkId = landmark->landmarkId();

    // adding new landmark
    if (landmarkId.localId().isEmpty()) {
        TRAP(err, symbianLandmark = LandmarkUtility::convertToSymbianLandmarkL(landmark);)
        if (err == KErrNone) {
            TPosLmItemId savedsymbianLmId = KPosLmNullItemId;
            TRAP(err, savedsymbianLmId = m_LandmarkDb->AddLandmarkL(*symbianLandmark);)
            if (err == KErrNone) {
                QLandmarkId savedQtLmId = LandmarkUtility::convertToQtLandmarkId(managerUri(),
                    savedsymbianLmId);
                landmark->setLandmarkId(savedQtLmId);
                *added = true;
                result = true;
            }
        }
    }
    else if (!LandmarkUtility::validLocalId(landmarkId.localId())) {

        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Bad Local Id assigned, Symbian Database Supports only unsigned long values for Landmark Id.";

        return result;
    }
    else {
        // check for existing landmark
        TPosLmItemId symbianLmId = LandmarkUtility::getSymbianLandmarkId(landmark);
        TRAP( err, symbianLandmark = m_LandmarkDb->ReadLandmarkLC(symbianLmId );
            if(symbianLmId!=NULL) CleanupStack::Pop(symbianLandmark);)
        if (err == KErrNone) {
            // updating existing landmark
            TRAP(err,LandmarkUtility::setSymbianLandmarkL(*symbianLandmark, landmark);)
            if (err == KErrNone) {
                TRAP(err, m_LandmarkDb->UpdateLandmarkL(*symbianLandmark);)
                if (err == KErrNone) {
                    *changed = true;
                    result = true;
                }
            }
        }
    }

    // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
    handleSymbianError(err, error, errorString);

    return result;
}

/*
 * remove single landamrk
 */
bool LandmarkManagerEngineSymbianPrivate::removeLandmarkInternal(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString, bool *removed)
{
    bool result = false;

    if (landmarkId.managerUri() != managerUri()) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return result;
    }

    if (!LandmarkUtility::validLocalId(landmarkId.localId())) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Bad LandmarkId : Invalid local id is assigned. Symbian Supports unsigned double type for Landmark Ids";
        return result;
    }

    TPosLmItemId symbianLmId = LandmarkUtility::convertToSymbianLandmarkId(landmarkId);
    CPosLandmark* symbianLandmark = NULL;
    TRAPD( err, symbianLandmark = m_LandmarkDb->ReadLandmarkLC(symbianLmId );
        if(symbianLandmark != NULL) CleanupStack::PopAndDestroy(symbianLandmark);)
    if (err == KErrNone) {
        TRAP(err, m_LandmarkDb->RemoveLandmarkL(symbianLmId));
        if (err == KErrNone) {
            *removed = true;
            result = true;
        }
    }

    // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
    handleSymbianError(err, error, errorString);

    return result;
}

/*
 * save single category
 */
bool LandmarkManagerEngineSymbianPrivate::saveCategoryInternal(QLandmarkCategory* category,
    QLandmarkManager::Error *error, QString *errorString, bool *added, bool *changed)
{
    bool result = false;

    if (added)
        *added = false;
    if (changed)
        *changed = false;

    if (!category->categoryId().managerUri().isEmpty() && category->categoryId().managerUri()
        != managerUri()) {
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "Landmark Category id comes from different landmark manager.";
        return result;
    }

    if (category->name().size() > KPosLmMaxCategoryNameLength) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Category Name is greater than its maxlength i.e. KPosLmMaxCategoryNameLength = 124.";
        return result;
    }

    CPosLandmarkCategory* symbiancat = NULL;
    TInt err = KErrNone;

    QLandmarkCategoryId categoryId = category->categoryId();

    if (categoryId.localId().isEmpty()) {
        TRAP(err, symbiancat = LandmarkUtility::convertToSymbianLandmarkCategoryL(category);)
        if (err == KErrNone) {
            TPosLmItemId savedsymbianLmCatId = KPosLmNullItemId;
            TRAP(err, savedsymbianLmCatId = m_LandmarkCatMgr->AddCategoryL(*symbiancat);)
            if (err == KErrNone) {
                QLandmarkCategoryId savedQtCategoryId =
                    LandmarkUtility::convertToQtLandmarkCategoryId(managerUri(),
                        savedsymbianLmCatId);
                category->setCategoryId(savedQtCategoryId);
                *added = true;
                result = true;
            }
        }
    }
    else if (!LandmarkUtility::validLocalId(categoryId.localId())) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Bad Local Id assigned, Symbian Database Supports only unsigned long values for Landmark Id.";
    }
    else {
        // check for existing category with category id
        TPosLmItemId symbianCatId = LandmarkUtility::convertToSymbianLandmarkCategoryId(
            category->categoryId());
        TRAP(err, symbiancat = m_LandmarkCatMgr->ReadCategoryLC(symbianCatId);
            if (symbiancat!=NULL) CleanupStack::Pop( symbiancat ); )
        if (err == KErrNone) {
            TRAP(err, LandmarkUtility::setSymbianLandmarkCategoryL(*symbiancat, category);)
            if (err == KErrNone) {
                TRAP(err, m_LandmarkCatMgr->UpdateCategoryL(*symbiancat);)
                if (err == KErrNone) {
                    *changed = true;
                    result = true;
                }
            }
        }
    }

    // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
    handleSymbianError(err, error, errorString);

    return result;
}

/*
 * remove single category
 */
bool LandmarkManagerEngineSymbianPrivate::removeCategoryInternal(
    const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString,
    bool *removed)
{
    bool result = false;

    if (categoryId.managerUri() != managerUri()) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Landmark cateogry id comes from different landmark manager.";
        return result;
    }

    if (!LandmarkUtility::validLocalId(categoryId.localId())) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Bad CategoryId : Invalid local id is assigned. Symbian Supports unsigned double type for Category Ids";
        return result;
    }

    TPosLmItemId symbianCategoryId =
        LandmarkUtility::convertToSymbianLandmarkCategoryId(categoryId);
    CPosLandmarkCategory* symbiancat = NULL;
    TRAPD(err, symbiancat = m_LandmarkCatMgr->ReadCategoryLC(symbianCategoryId);
        if (symbiancat!=NULL) CleanupStack::PopAndDestroy( symbiancat ); )
    if (err == KErrNone) {
        TRAP(err, ExecuteAndDeleteLD( m_LandmarkCatMgr->RemoveCategoryL(symbianCategoryId));)
        if (err == KErrNone) {
            *removed = true;
            result = true;
        }
    }

    // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
    handleSymbianError(err, error, errorString);

    return result;
}

/*
 * fetch single category
 */
QLandmarkCategory LandmarkManagerEngineSymbianPrivate::fetchCategory(
    const QLandmarkCategoryId &landmarkCategoryId, QLandmarkManager::Error *error,
    QString *errorString) const
{
    if (&landmarkCategoryId == 0) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid category id or empty id.";
        return QLandmarkCategory();
    }

    if (landmarkCategoryId.managerUri() != managerUri()) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Category id comes from different landmark manager.";
        return QLandmarkCategory();
    }

    if (!LandmarkUtility::validLocalId(landmarkCategoryId.localId())) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString
                = "Bad CategoryId : Invalid local id is assigned. Symbian Supports unsigned double type for Category Ids";
        return QLandmarkCategory();
    }

    TPosLmItemId symbianCategoryId = LandmarkUtility::convertToSymbianLandmarkCategoryId(
        landmarkCategoryId);
    CPosLandmarkCategory* symbiancat = NULL;
    TInt err = KErrNotFound;

    TRAP(err, symbiancat = m_LandmarkCatMgr->ReadCategoryLC(symbianCategoryId);
        if (symbiancat!=NULL) CleanupStack::Pop( symbiancat ); )

    if (err == KErrNone) {
        QLandmarkCategory* qtCat = LandmarkUtility::convertToQtLandmarkCategory(managerUri(),
            symbiancat);

        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";

        delete symbiancat;
        symbiancat = NULL;

        return *qtCat;
    }

    // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
    handleSymbianError(err, error, errorString);

    return QLandmarkCategory();
}

/*
 * fetch all category ids
 */
QList<QLandmarkCategoryId> LandmarkManagerEngineSymbianPrivate::fetchCategoryIdsL(
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    if (nameSort.caseSensitivity() == Qt::CaseSensitive) {
        if (error)
            *error = QLandmarkManager::NotSupportedError;
        if (errorString)
            *errorString
                = "Case Sensivity Support is not there, Please prefer CaseInsensitive to get result.";

        return QList<QLandmarkCategoryId> ();
    }

    QLandmarkNameSort Sort = nameSort;
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    CPosLmCategoryManager::TCategorySortPref sortPref;
    if (nameSort.direction() == Qt::AscendingOrder) {
        sortPref = CPosLmCategoryManager::ECategorySortOrderNameAscending;
    }
    else if (nameSort.direction() == Qt::DescendingOrder) {
        sortPref = CPosLmCategoryManager::ECategorySortOrderNameDescending;
    }
    else {
        sortPref = CPosLmCategoryManager::ECategorySortOrderNone;
    }

    RArray<TPosLmItemId> idArray;
    CPosLmItemIterator* iterator = m_LandmarkCatMgr->CategoryIteratorL(sortPref);
    CleanupStack::PushL(iterator);

    int count = iterator->NumOfItemsL();
    if (count > 0) { // can only be called if there are some items
        iterator->GetItemIdsL(idArray, 0, count); // array is first reseted
    }
    CleanupStack::PopAndDestroy(iterator);

    QList<QLandmarkCategoryId> qtCatIds = LandmarkUtility::convertToQtLandmarkCategoryIds(
        managerUri(), idArray);
    idArray.Close();
    return qtCatIds;
}

/*
 * get landmarks search criteria from qt filter to search
 * 
 */
CPosLmSearchCriteria* LandmarkManagerEngineSymbianPrivate::getSearchCriteriaL(
    QLandmarkFilter filter) const
{
    switch (filter.type()) {
    case QLandmarkFilter::DefaultFilter:
    case QLandmarkFilter::NameFilter:
    {
        QLandmarkNameFilter nameFilter;

        if (filter.type() == QLandmarkFilter::DefaultFilter) {
            nameFilter.setName("*");
        }
        else {
            nameFilter = filter;
        }

        //Create textcriteria which contains both search string and wild * for search 
        HBufC* filterbuf = HBufC::NewL(nameFilter.name().size() + KExtrachars);
        TPtr filterName = filterbuf->Des();
        filterName.Copy(nameFilter.name().utf16());
        filterName.Append(KDefaultTextCriteria);
        CPosLmTextCriteria* textSearchCriteria = CPosLmTextCriteria::NewLC();
        textSearchCriteria->SetTextL(filterName);
        textSearchCriteria->SetAttributesToSearch(CPosLandmark::ELandmarkName);
        CleanupStack::Pop(textSearchCriteria);

        return textSearchCriteria;
    }
    case QLandmarkFilter::LandmarkIdFilter:
    {
        QLandmarkIdFilter landmarkIdFilter = filter;
        QList<QLandmarkId> qtLmIds = landmarkIdFilter.landmarkIds();
        RArray<TPosLmItemId> symLmIds = LandmarkUtility::getSymbianLandmarkIds(qtLmIds);
        if (symLmIds.Count() <= 0) {
            User::Leave(KErrArgument);
        }

        // create composite criteria 
        CPosLmCompositeCriteria *compCriteria = CPosLmCompositeCriteria::NewLC(
            CPosLmCompositeCriteria::ECompositionAND);
        // create id list criteria & add it into composite criteria
        CPosLmIdListCriteria *idListCriteria = CPosLmIdListCriteria::NewLC();
        idListCriteria->SetLandmarkIdsL(symLmIds);
        compCriteria->AddArgument(idListCriteria);
        CleanupStack::Pop(idListCriteria);
        // create text criteria & add it into composite criteria
        CPosLmTextCriteria *textCriteria = CPosLmTextCriteria::NewLC();
        textCriteria->SetAttributesToSearch(CPosLandmark::ELandmarkName);
        textCriteria->SetTextL(KDefaultTextCriteria);
        compCriteria->AddArgument(textCriteria);
        CleanupStack::Pop(idListCriteria);
        CleanupStack::Pop(compCriteria);

        return compCriteria;
    }
    case QLandmarkFilter::CategoryFilter:
    {
        QLandmarkCategoryFilter categoryFilter = filter;
        CPosLmCategoryCriteria* categorySearchCriteria = CPosLmCategoryCriteria::NewLC();
        categorySearchCriteria->SetCategoryItemId(
            LandmarkUtility::convertToSymbianLandmarkCategoryId(categoryFilter.categoryId()));
        CleanupStack::Pop(categorySearchCriteria);

        return categorySearchCriteria;
    }
    case QLandmarkFilter::ProximityFilter:
    {
        QLandmarkProximityFilter proximityFilter = filter;
        // set the coordinate values
        TCoordinate symbianCoord;
        QGeoCoordinate qCoord = proximityFilter.coordinate();
        symbianCoord.SetCoordinate(qCoord.latitude(), qCoord.longitude());

        // set the nearest criteria
        CPosLmNearestCriteria* nearestCriteria = CPosLmNearestCriteria::NewLC(symbianCoord);

        if (proximityFilter.radius() >= 0)
            nearestCriteria->SetMaxDistance(proximityFilter.radius());

        CleanupStack::Pop(nearestCriteria);

        return nearestCriteria;
    }
    case QLandmarkFilter::BoxFilter:
    {
        QLandmarkBoxFilter boxFilter = filter;

        const TReal64& eastlong = boxFilter.boundingBox().bottomRight().longitude();
        const TReal64& southlat = boxFilter.boundingBox().bottomLeft().latitude();
        const TReal64& northlat = boxFilter.boundingBox().topRight().latitude();
        const TReal64& westlong = boxFilter.boundingBox().topLeft().longitude();

        CPosLmAreaCriteria* areaCriteria = CPosLmAreaCriteria::NewLC(southlat, northlat, westlong,
            eastlong);
        CleanupStack::Pop(areaCriteria);

        return areaCriteria;

    }
    case QLandmarkFilter::AttributeFilter:
    {
        QLandmarkAttributeFilter attributeFilter = filter;

        QStringList keyList = attributeFilter.attributeKeys();

        //if any of the attribute matchflag is set to MatchCaseSensitive, then return KErrNotSupported 
        for (int i = 0; i < keyList.size(); ++i) {
            if (attributeFilter.matchFlags(keyList.at(i)) & QLandmarkFilter::MatchCaseSensitive) {
                User::Leave(KErrNotSupported);
            }
        }

        QLandmarkAttributeFilter::OperationType opType = attributeFilter.operationType();

        // create composite criteria to add all attribute criterias
        CPosLmCompositeCriteria* compositeCriteria = CPosLmCompositeCriteria::NewLC(
            (CPosLmCompositeCriteria::TCompositionType) opType);

        RArray<TUint> afieldArray;

        //the attributes search which cannot be used in CPosLmTextCriteria
        if (keyList.contains("longitude") || keyList.contains("latitude") || keyList.contains(
            "radius") || keyList.contains("altitude") || keyList.contains("radius")
            || keyList.contains("iconurl")) {

            QLandmarkManager::Error error = QLandmarkManager::NoError;
            QString errorString = "";
            QList<QLandmark> lmks = landmarks(QLandmarkFilter(), -1, 0,
                QList<QLandmarkSortOrder> (), &error, &errorString);

            RArray<TPosLmItemId> lmksid;

            foreach(QLandmark lmk, lmks)
                {
                    if (keyList.contains("longitude")) {
                        // if value of attr is valid , and if it is exist in current landmark
                        // then adding into list
                        if (attributeFilter.attribute("longitude").isValid()) {
                            if (lmk.coordinate().longitude() == attributeFilter.attribute(
                                "longitude").toDouble())
                                lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                    lmk.landmarkId()));
                        }
                        // if invalid attr value, then add landmark id of all landmarks
                        // having valid longitude value.
                        else if (lmk.coordinate().longitude() != qQNaN())
                            lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                lmk.landmarkId()));
                    }
                    if (keyList.contains("latitude")) {
                        if (attributeFilter.attribute("latitude").isValid()) {
                            if (lmk.coordinate().latitude()
                                == attributeFilter.attribute("latitude").toDouble())
                                lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                    lmk.landmarkId()));
                        }
                        else if (lmk.coordinate().latitude() != qQNaN())
                            lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                lmk.landmarkId()));
                    }
                    if (keyList.contains("altitude")) {
                        if (attributeFilter.attribute("altitude").isValid()) {
                            if (lmk.coordinate().altitude()
                                == attributeFilter.attribute("altitude").toDouble())
                                lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                    lmk.landmarkId()));
                        }
                        else if (lmk.coordinate().altitude() != qQNaN())
                            lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                lmk.landmarkId()));
                    }
                    if (keyList.contains("radius")) {
                        if (attributeFilter.attribute("radius").isValid()) {
                            if (lmk.radius() == attributeFilter.attribute("radius").toDouble())
                                lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                    lmk.landmarkId()));
                        }
                        else if (lmk.radius() > 0)
                            lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                lmk.landmarkId()));
                    }
                    if (keyList.contains("iconurl")) {
                        if (attributeFilter.attribute("iconurl").isValid()) {
                            if (lmk.iconUrl() == attributeFilter.attribute("iconurl"))
                                lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                    lmk.landmarkId()));
                        }
                        else if (lmk.iconUrl().isValid())
                            lmksid.Append(LandmarkUtility::convertToSymbianLandmarkId(
                                lmk.landmarkId()));
                    }
                }
            CPosLmIdListCriteria *idListCriteria = CPosLmIdListCriteria::NewLC();
            idListCriteria->SetLandmarkIdsL(lmksid);
            CleanupStack::Pop(idListCriteria);
            User::LeaveIfError(compositeCriteria->AddArgument(idListCriteria));
        }

        //search with CPosLmTextCriteria fields only
        for (int i = 0; i < keyList.size(); ++i) {
            if ((keyList.at(i) == "latitude") || (keyList.at(i) == "longitude") || (keyList.at(i)
                == "altitude") || (keyList.at(i) == "radius") || (keyList.at(i) == "iconurl"))
                continue;

            afieldArray.Reset();

            QVariant keyValue = attributeFilter.attribute(keyList.at(i));

            HBufC* filterbuf = HBufC::NewL(keyValue.toString().size() + KExtrachars);
            TPtr filterName = filterbuf->Des();

            HBufC* filterbufcont = HBufC::NewL(keyValue.toString().size() + KExtrachars);
            TPtr filterNamecont = filterbuf->Des();

            //If filter attribute value is valid then search for that specific text based on matchflags, else return all the landmarks
            //whose that attribute is set to some valid value.
            if (keyValue.isValid()) {

                QLandmarkFilter::MatchFlags flag = attributeFilter.matchFlags(keyList.at(i));

                switch (flag) {
                case QLandmarkFilter::MatchExactly:
                {
                    filterName.Copy(keyValue.toString().utf16());
                    break;
                }
                case QLandmarkFilter::MatchStartsWith:
                {
                    filterName.Copy(keyValue.toString().utf16());
                    filterName.Append(KDefaultTextCriteria);
                    break;
                }
                case QLandmarkFilter::MatchEndsWith:
                {
                    filterName.Copy(KDefaultTextCriteria);
                    filterName.Append(keyValue.toString().utf16(), keyValue.toString().size());
                    break;
                }
                    //the text could be either *name* or  * name*
                case QLandmarkFilter::MatchContains:
                {
                    filterName.Copy(KDefaultTextCriteria);
                    filterName.Append(keyValue.toString().utf16(), keyValue.toString().size());
                    filterName.Append(KDefaultTextCriteria);

                    filterNamecont.Copy(KDefaultSpaceTextSearch);
                    filterNamecont.Append(keyValue.toString().utf16(), keyValue.toString().size());
                    filterNamecont.Append(KDefaultTextCriteria);

                    break;
                }
                case QLandmarkFilter::MatchFixedString:
                {
                    filterName.Copy(keyValue.toString().utf16());
                    break;
                }
                default:
                    break;
                }
            }
            else {
                filterName.Copy(KWildChar);
            }

            if (keyList.at(i) == "name") {

                CPosLmTextCriteria* textNameCriteria = CPosLmTextCriteria::NewLC();
                textNameCriteria->SetTextL(filterName);
                textNameCriteria->SetAttributesToSearch(CPosLandmark::ELandmarkName);
                compositeCriteria->AddArgument(textNameCriteria);
                CleanupStack::Pop(textNameCriteria);
                //used only for MatchContains case
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsName = CPosLmTextCriteria::NewLC();
                    textContainsName->SetTextL(filterNamecont);
                    textContainsName->SetAttributesToSearch(CPosLandmark::ELandmarkName);
                    compositeCriteria->AddArgument(textNameCriteria);
                    CleanupStack::Pop(textContainsName);
                }
            }
            else if (keyList.at(i) == "description") {

                CPosLmTextCriteria* textDescCriteria = CPosLmTextCriteria::NewLC();
                textDescCriteria->SetTextL(filterName);
                textDescCriteria->SetAttributesToSearch(CPosLandmark::EDescription);
                compositeCriteria->AddArgument(textDescCriteria);
                CleanupStack::Pop(textDescCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsDesc = CPosLmTextCriteria::NewLC();
                    textContainsDesc->SetTextL(filterNamecont);
                    textContainsDesc->SetAttributesToSearch(CPosLandmark::ELandmarkName);
                    compositeCriteria->AddArgument(textContainsDesc);
                    CleanupStack::Pop(textContainsDesc);
                }
            }
            else if (keyList.at(i) == "phone") {

                CPosLmTextCriteria* textPhoneCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textPhoneCriteria->SetTextL(filterName);
                textPhoneCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textPhoneCriteria);
                CleanupStack::Pop(textPhoneCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsPhone = CPosLmTextCriteria::NewLC();
                    textContainsPhone->SetTextL(filterNamecont);
                    textContainsPhone->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsPhone);
                    CleanupStack::Pop(textContainsPhone);
                }
            }
            else if (keyList.at(i) == "country") {

                CPosLmTextCriteria* textCountryCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textCountryCriteria->SetTextL(filterName);
                textCountryCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textCountryCriteria);
                CleanupStack::Pop(textCountryCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsCountry = CPosLmTextCriteria::NewLC();
                    textContainsCountry->SetTextL(filterNamecont);
                    textContainsCountry->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsCountry);
                    CleanupStack::Pop(textContainsCountry);
                }
            }
            else if (keyList.at(i) == "countryCode") {

                CPosLmTextCriteria* textCountryCodeCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textCountryCodeCriteria->SetTextL(filterName);
                textCountryCodeCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textCountryCodeCriteria);
                CleanupStack::Pop(textCountryCodeCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsCC = CPosLmTextCriteria::NewLC();
                    textContainsCC->SetTextL(filterNamecont);
                    textContainsCC->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsCC);
                    CleanupStack::Pop(textContainsCC);
                }
            }
            else if (keyList.at(i) == "State") {

                CPosLmTextCriteria* textStateCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textStateCriteria->SetTextL(filterName);
                textStateCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textStateCriteria);
                CleanupStack::Pop(textStateCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsState = CPosLmTextCriteria::NewLC();
                    textContainsState->SetTextL(filterNamecont);
                    textContainsState->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsState);
                    CleanupStack::Pop(textContainsState);
                }
            }
            else if (keyList.at(i) == "county") {
                CPosLmTextCriteria* textCountyCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textCountyCriteria->SetTextL(filterName);
                textCountyCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textCountyCriteria);
                CleanupStack::Pop(textCountyCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsCounty = CPosLmTextCriteria::NewLC();
                    textContainsCounty->SetTextL(filterNamecont);
                    textContainsCounty->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsCounty);
                    CleanupStack::Pop(textContainsCounty);
                }
            }
            else if (keyList.at(i) == "city") {
                CPosLmTextCriteria* textCityCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textCityCriteria->SetTextL(filterName);
                textCityCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textCityCriteria);
                CleanupStack::Pop(textCityCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsCity = CPosLmTextCriteria::NewLC();
                    textContainsCity->SetTextL(filterNamecont);
                    textContainsCity->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsCity);
                    CleanupStack::Pop(textContainsCity);
                }
            }
            else if (keyList.at(i) == "district") {
                CPosLmTextCriteria* textDistrictCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textDistrictCriteria->SetTextL(filterName);
                textDistrictCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textDistrictCriteria);
                CleanupStack::Pop(textDistrictCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsDist = CPosLmTextCriteria::NewLC();
                    textContainsDist->SetTextL(filterNamecont);
                    textContainsDist->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsDist);
                    CleanupStack::Pop(textContainsDist);
                }
            }
            else if (keyList.at(i) == "street") {
                CPosLmTextCriteria* textStreetCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textStreetCriteria->SetTextL(filterName);
                textStreetCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textStreetCriteria);
                CleanupStack::Pop(textStreetCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsStreet = CPosLmTextCriteria::NewLC();
                    textContainsStreet->SetTextL(filterNamecont);
                    textContainsStreet->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsStreet);
                    CleanupStack::Pop(textContainsStreet);
                }
            }
            else if (keyList.at(i) == "streetNumber") {
                CPosLmTextCriteria* textStrNumCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textStrNumCriteria->SetTextL(filterName);
                textStrNumCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textStrNumCriteria);
                CleanupStack::Pop(textStrNumCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsStrNum = CPosLmTextCriteria::NewLC();
                    textContainsStrNum->SetTextL(filterNamecont);
                    textContainsStrNum->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsStrNum);
                    CleanupStack::Pop(textContainsStrNum);
                }
            }
            else if (keyList.at(i) == "postCode") {
                CPosLmTextCriteria* textPostCodeCriteria = CPosLmTextCriteria::NewLC();
                afieldArray.InsertInOrder(LandmarkUtility::positionFieldId(keyList.at(i)));
                textPostCodeCriteria->SetTextL(filterName);
                textPostCodeCriteria->SetPositionFieldsToSearchL(afieldArray);
                compositeCriteria->AddArgument(textPostCodeCriteria);
                CleanupStack::Pop(textPostCodeCriteria);
                if (filterNamecont.Length() > 0) {
                    CPosLmTextCriteria* textContainsPostCode = CPosLmTextCriteria::NewLC();
                    textContainsPostCode->SetTextL(filterNamecont);
                    textContainsPostCode->SetPositionFieldsToSearchL(afieldArray);
                    compositeCriteria->AddArgument(textContainsPostCode);
                    CleanupStack::Pop(textContainsPostCode);
                }
            }
        }

        CleanupStack::Pop(compositeCriteria);
        return compositeCriteria;
    }
    case QLandmarkFilter::IntersectionFilter:
    {
        QLandmarkIntersectionFilter intersectionFilter = filter;

        CPosLmCompositeCriteria *compCriteria = CPosLmCompositeCriteria::NewLC(
            CPosLmCompositeCriteria::ECompositionAND);

        QList<QLandmarkFilter> filters = intersectionFilter.filters();
        foreach (const QLandmarkFilter& filter,filters)
            {
                if (filter.type() == QLandmarkFilter::IntersectionFilter || filter.type()
                    == QLandmarkFilter::UnionFilter) {
                    User::Leave(KErrArgument);
                }
                if (filter.type() == QLandmarkFilter::DefaultFilter || filter.type()
                    == QLandmarkFilter::BoxFilter || filter.type()
                    == QLandmarkFilter::CategoryFilter || filter.type()
                    == QLandmarkFilter::NameFilter || filter.type()
                    == QLandmarkFilter::ProximityFilter) {
                    compCriteria->AddArgument(getSearchCriteriaL(filter));
                }
                if (filter.type() == QLandmarkFilter::LandmarkIdFilter) {
                    QLandmarkIdFilter landmarkIdFilter = filter;
                    QList<QLandmarkId> qtLmIds = landmarkIdFilter.landmarkIds();
                    RArray<TPosLmItemId> symLmIds = LandmarkUtility::getSymbianLandmarkIds(qtLmIds);
                    if (symLmIds.Count() <= 0) {
                        User::Leave(KErrArgument);
                    }
                    // create id list criteria & add it into composite criteria
                    CPosLmIdListCriteria *idListCriteria = CPosLmIdListCriteria::NewLC();
                    idListCriteria->SetLandmarkIdsL(symLmIds);
                    compCriteria->AddArgument(idListCriteria);
                    CleanupStack::Pop(idListCriteria);
                }
                if (filter.type() == QLandmarkFilter::AttributeFilter) {
                    compCriteria->AddArgument(getSearchCriteriaL(filter));
                }
            }

        CleanupStack::Pop(compCriteria);
        return compCriteria;
    }
    case QLandmarkFilter::UnionFilter:
    case QLandmarkFilter::InvalidFilter:
    default:
    {
        break;
    }
    } // switch closure
    return NULL;
}

CPosLmOperation* LandmarkManagerEngineSymbianPrivate::getOperationL(
    CPosLandmarkSearch* landmarkSearch, CPosLmSearchCriteria* searchCriteria,
    QLandmarkSortOrder qtSortOrder, int maxMatches) const
{
    CPosLmOperation* operation = NULL;
    bool isOwned = false;

    if (!landmarkSearch) {
        landmarkSearch = CPosLandmarkSearch::NewL(*m_LandmarkDb);
        CleanupStack::PushL(landmarkSearch);
        isOwned = true;
    }

    CPosLmPartialReadParameters* readParams = CPosLmPartialReadParameters::NewLC();
    readParams->SetRequestedAttributes(CPosLandmark::ELandmarkName);
    m_LandmarkDb->SetPartialReadParametersL(*readParams);
    landmarkSearch->SetMaxNumOfMatches(maxMatches);

    if (qtSortOrder.type() == QLandmarkSortOrder::NameSort) {
        if (qtSortOrder.direction() == Qt::AscendingOrder) {
            TPosLmSortPref sortOrder(CPosLandmark::ELandmarkName, TPosLmSortPref::EAscending);
            operation = landmarkSearch->StartLandmarkSearchL(*searchCriteria, sortOrder);
        }
        else {
            TPosLmSortPref sortOrder(CPosLandmark::ELandmarkName, TPosLmSortPref::EDescending);
            operation = landmarkSearch->StartLandmarkSearchL(*searchCriteria, sortOrder);
        }
    }
    else
        operation = landmarkSearch->StartLandmarkSearchL(*searchCriteria);

    CleanupStack::Pop(readParams);
    if (isOwned)
        CleanupStack::Pop(landmarkSearch);

    return operation;
}

/*
 * search landmarks with landmarkfilter
 * 
 */
QList<QLandmarkId> LandmarkManagerEngineSymbianPrivate::searchWithFilterL(QLandmarkFilter filter,
    QLandmarkSortOrder qtSortOrder, int maxMatches) const
{
    QList<QLandmarkId> result;
    // use native serarch
    CPosLmSearchCriteria *searchCriteria = getSearchCriteriaL(filter);
    CPosLandmarkSearch* landmarkSearch = CPosLandmarkSearch::NewL(*m_LandmarkDb);
    CleanupStack::PushL(landmarkSearch);
    ExecuteAndDeleteLD(getOperationL(landmarkSearch, searchCriteria, qtSortOrder, maxMatches));
    TPosLmItemId lmId;
    CPosLmItemIterator *iterator = landmarkSearch->MatchIteratorL();
    while ((lmId = iterator->NextL()) != KPosLmNullItemId) {
        result.append(LandmarkUtility::convertToQtLandmarkId(managerUri(), lmId));
    }
    CleanupStack::PopAndDestroy(landmarkSearch);
    delete searchCriteria;
    // return searched landmark ids
    return result;
}

/*
 * handle the symbian error codes
 * to map to QLandmarkManager Error code
 */
void LandmarkManagerEngineSymbianPrivate::handleSymbianError(TInt errorId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    switch (errorId) {
    case KErrNone:
    {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
        break;
    }
    case KErrArgument:
    {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Invalid data, invalid landmark/category id value";
        break;
    }
    case KErrPermissionDenied:
    {
        if (error)
            *error = QLandmarkManager::PermissionsError;
        if (errorString)
            *errorString = "Permission Denied, Please assign appropriate capabilities to client.";
        break;
    }
    case KErrAccessDenied:
    case KErrLocked:
    {
        if (error)
            *error = QLandmarkManager::LockedError;
        if (errorString)
            *errorString
                = "Database is locked for other operation, so no addition/updation/removal possible.";
        break;
    }
    case KErrNotFound:
    {
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "None of the existing landmarks/categories match the given id.";
        break;
    }
    case KErrAlreadyExists:
    {
        // specific to category operation
        // this error code is returned while addition of new category, which name already exists in db.
        if (error)
            *error = QLandmarkManager::AlreadyExistsError;
        if (errorString)
            *errorString = "Category Name already exists, please provide unique category name.";
        break;
    }
    case KErrPosLmNotInitialized:
    {
        if (error)
            *error = QLandmarkManager::InvalidManagerError;
        if (errorString)
            *errorString = "Mangager is not constructed and initialized properly.";
        break;
    }
    default:
    {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString) {
            *errorString = QString("Landmarks Error = %d").arg(errorId);
        }
        break;
    }
    } // switch closure
}

/**
 * HandleCompletion
 */
void LandmarkManagerEngineSymbianPrivate::HandleCompletion(CLandmarkRequestData* aData)
{
    qDebug() << " Back in HandleCompletionL()";
    QLandmarkAbstractRequest* request = aData->iQtRequest;

    QLandmarkManager::Error error = QLandmarkManager::NoError;
    QString errorString = 0;

    handleSymbianError(aData->iErrorId, &error, &errorString);

    // for stateChanged signal
    QLandmarkManagerEngineSymbian::updateRequestState(request,
        QLandmarkAbstractRequest::FinishedState);

    switch (request->type()) {
    case QLandmarkAbstractRequest::LandmarkIdFetchRequest:
    {
        QLandmarkIdFetchRequest *lmIdFetchRequest =
            static_cast<QLandmarkIdFetchRequest *> (request);

        QList<QLandmarkId> landmarkIds;

        if (aData->iLandmarkSearch) {
            TRAP_IGNORE(
                CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
                RArray<TPosLmItemId> symbianLmIds;
                if (iterator->NumOfItemsL() > 0) {
                    iterator->GetItemIdsL(symbianLmIds, 0, iterator->NumOfItemsL());
                }

                landmarkIds = LandmarkUtility::convertToQtLandmarkIds(managerUri(), symbianLmIds);
                symbianLmIds.Close();
            )
        }

        sortFetchedLmIds(lmIdFetchRequest->limit(), lmIdFetchRequest->offset(),
            lmIdFetchRequest->sorting(), landmarkIds, &error, &errorString);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkIdFetchRequest(lmIdFetchRequest, landmarkIds,
            error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::CategoryIdFetchRequest:
    {
        QLandmarkCategoryIdFetchRequest *catIdFetchRequest =
            static_cast<QLandmarkCategoryIdFetchRequest*> (request);

        QList<QLandmarkCategoryId> qtCatIds;

        if (aData->iLandmarkSearch) {
            TRAP_IGNORE(
                CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
                RArray<TPosLmItemId> symbianCatIds;
                if (iterator->NumOfItemsL() > 0) {
                    iterator->GetItemIdsL(symbianCatIds, 0, iterator->NumOfItemsL());
                }

                qtCatIds = LandmarkUtility::convertToQtLandmarkCategoryIds(managerUri(), symbianCatIds);
                symbianCatIds.Close();
            )
        }
        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategoryIdFetchRequest(catIdFetchRequest,
            qtCatIds, error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::LandmarkFetchRequest:
    {
        QLandmarkFetchRequest *lmfetchRequest = static_cast<QLandmarkFetchRequest*> (request);

        QList<QLandmark> landmarks;
        QList<QLandmarkId> landmarkIds;

        if (aData->iLandmarkSearch) {
            TRAP_IGNORE(

                CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
                RArray<TPosLmItemId> symbianLmIds;
                if (iterator->NumOfItemsL() > 0) {
                    iterator->GetItemIdsL(symbianLmIds, 0, iterator->NumOfItemsL());
                }

                landmarkIds = LandmarkUtility::convertToQtLandmarkIds(managerUri(), symbianLmIds);
                symbianLmIds.Close();
            )
        }

        if (sortFetchedLmIds(lmfetchRequest->limit(), lmfetchRequest->offset(),
            lmfetchRequest->sorting(), landmarkIds, &error, &errorString)) {

            // get all landmark data
            QLandmark qtLandmark;
            foreach (const QLandmarkId& lmId,landmarkIds)
                {
                    // use landmark fetch method to get landmark from landmark id
                    qtLandmark = landmark(lmId, &error, &errorString);
                    if (error != QLandmarkManager::NoError) {
                        landmarks.clear();
                        break;
                    }
                    landmarks.append(qtLandmark);
                }
        }

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkFetchRequest(lmfetchRequest, landmarks, error,
            errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::CategoryFetchRequest:
    {
        QLandmarkCategoryFetchRequest *catFetchRequest =
            static_cast<QLandmarkCategoryFetchRequest*> (request);

        QList<QLandmarkCategoryId> qtCatIds;
        QList<QLandmarkCategory> qtCategories;

        if (aData->iLandmarkSearch) {
            TRAP_IGNORE(
                CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
                RArray<TPosLmItemId> symbianCatIds;
                if (iterator->NumOfItemsL() > 0) {
                    iterator->GetItemIdsL(symbianCatIds, 0, iterator->NumOfItemsL());
                }
                qtCatIds = LandmarkUtility::convertToQtLandmarkCategoryIds(managerUri(), symbianCatIds);
                symbianCatIds.Close();
            )
        }

        if (qtCatIds.size() > 0) {

            for (int i = 0; i < qtCatIds.size(); ++i) {
                QLandmarkCategory cat = fetchCategory(qtCatIds.operator[](i), &error, &errorString);

                if (error != QLandmarkManager::NoError) {
                    qtCategories.clear();
                    break;
                }

                qtCategories.append(cat);
            }
        }

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategoryFetchRequest(catFetchRequest,
            qtCategories, error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::LandmarkSaveRequest:
    {
        QLandmarkSaveRequest *lmSaveRequest = static_cast<QLandmarkSaveRequest*> (request);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkSaveRequest(lmSaveRequest, aData->iLandmarks,
            error, errorString, aData->iErrorMap, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::LandmarkRemoveRequest:
    {
        QLandmarkRemoveRequest *lmRemoveRequest = static_cast<QLandmarkRemoveRequest*> (request);

        lmRemoveRequest->setLandmarkIds(aData->iLandmarkIds);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkRemoveRequest(lmRemoveRequest, error,
            errorString, aData->iErrorMap, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::CategorySaveRequest:
    {
        QLandmarkCategorySaveRequest *catSaveRequest =
            static_cast<QLandmarkCategorySaveRequest*> (request);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategorySaveRequest(catSaveRequest,
            aData->iCategories, error, errorString, aData->iErrorMap,
            QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::CategoryRemoveRequest:
    {
        QLandmarkCategoryRemoveRequest *catRemoveRequest =
            static_cast<QLandmarkCategoryRemoveRequest*> (request);

        catRemoveRequest->setCategoryIds(aData->iCategoryIds);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategoryRemoveRequest(catRemoveRequest, error,
            errorString, aData->iErrorMap, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::ImportRequest:
    {
        QLandmarkImportRequest *importRequest = static_cast<QLandmarkImportRequest*> (request);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkImportRequest(importRequest, error,
            errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::ExportRequest:
    {
        QLandmarkExportRequest *exportRequest = static_cast<QLandmarkExportRequest*> (request);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkExportRequest(exportRequest, error,
            errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    default:
    {
        break;
    }
    }// switch closure

    // for stateChanged signal
    QLandmarkManagerEngineSymbian::updateRequestState(request,
        QLandmarkAbstractRequest::FinishedState);

    // reset the global request data.
    aData->Reset();
}

/**
 * handle sync operations execution
 */
void LandmarkManagerEngineSymbianPrivate::HandleExecution(CLandmarkRequestData* aData,
    TRequestStatus& aRequest)
{
    qDebug() << " Back in HandleExecutionL()";
    QLandmarkAbstractRequest* landmarkRequest = aData->iQtRequest;

    QLandmarkManager::Error error = QLandmarkManager::NoError;
    QString errorString = 0;

    switch (landmarkRequest->type()) {
    case QLandmarkAbstractRequest::LandmarkSaveRequest:
    {
        QLandmarkSaveRequest *lmSaveRequest = static_cast<QLandmarkSaveRequest *> (landmarkRequest);

        if (aData->iOpCount < lmSaveRequest->landmarks().size()) {
            QLandmark qtLm = (lmSaveRequest->landmarks()).at(aData->iOpCount);
            bool saveResult = saveLandmark(&qtLm, &error, &errorString);
            if (!saveResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
            }
            aData->iLandmarks.append(qtLm);

            aRequest = KPosLmOperationNotComplete;
            aData->iOpCount++;
        }
        else {
            // Set Complete.
            aRequest = KErrNone;
        }
        break;
    }
    case QLandmarkAbstractRequest::LandmarkRemoveRequest:
    {
        QLandmarkRemoveRequest *lmRemoveRequest =
            static_cast<QLandmarkRemoveRequest *> (landmarkRequest);

        if (aData->iOpCount < lmRemoveRequest->landmarkIds().size()) {
            QLandmarkId qtLmId = (lmRemoveRequest->landmarkIds()).at(aData->iOpCount);
            bool removeResult = removeLandmark(qtLmId, &error, &errorString);
            if (!removeResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
            }
            aData->iLandmarkIds.append(qtLmId);

            aRequest = KPosLmOperationNotComplete;
            aData->iOpCount++;
        }
        else {
            // Set Complete.
            aRequest = KErrNone;
        }
        break;
    }
    case QLandmarkAbstractRequest::CategorySaveRequest:
    {
        QLandmarkCategorySaveRequest *saveCategoryRequest =
            static_cast<QLandmarkCategorySaveRequest *> (landmarkRequest);

        if (aData->iOpCount < saveCategoryRequest->categories().count()) {
            QLandmarkCategory qtCat = (saveCategoryRequest->categories()).at(aData->iOpCount);
            bool saveResult = saveCategory(&qtCat, &error, &errorString);
            if (!saveResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
            }
            aData->iCategories.append(qtCat);

            aRequest = KPosLmOperationNotComplete;
            aData->iOpCount++;
        }
        else {
            // Set Complete.
            aRequest = KErrNone;
        }
        break;
    }
    case QLandmarkAbstractRequest::CategoryRemoveRequest:
    {
        QLandmarkCategoryRemoveRequest *catRemoveRequest =
            static_cast<QLandmarkCategoryRemoveRequest *> (landmarkRequest);

        if (aData->iOpCount < catRemoveRequest->categoryIds().size()) {
            QLandmarkCategoryId qtCatId = (catRemoveRequest->categoryIds()).at(aData->iOpCount);
            bool removeResult = removeCategory(qtCatId, &error, &errorString);
            if (!removeResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
            }
            aData->iCategoryIds.append(qtCatId);

            aRequest = KPosLmOperationNotComplete;
            aData->iOpCount++;
        }
        else {
            // Set Complete.
            aRequest = KErrNone;
        }
        break;
    }
    case QLandmarkAbstractRequest::ImportRequest:
    case QLandmarkAbstractRequest::ExportRequest:
    case QLandmarkAbstractRequest::LandmarkIdFetchRequest:
    case QLandmarkAbstractRequest::LandmarkFetchRequest:
    case QLandmarkAbstractRequest::CategoryIdFetchRequest:
    case QLandmarkAbstractRequest::CategoryFetchRequest:
    default:
    {
        // Set Complete.
        aRequest = KErrNone;
        break;
    }
    }// switch closure
}

/**
 * sorts if sorting required.
 * also filters the data with offset and limit
 */
bool LandmarkManagerEngineSymbianPrivate::sortFetchedLmIds(int limit, int offset, QList<
    QLandmarkSortOrder> sortOrders, QList<QLandmarkId>& landmarkIds,
    QLandmarkManager::Error *error, QString *errorString) const
{
    // if no search data found return empty list
    if (&landmarkIds == 0 || landmarkIds.isEmpty() || offset >= landmarkIds.size()) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = QString("Invalid fetchHint data, Invalid offset value.");
        return false;
    }

    //fetchRequired will prevent multiple fetches from database
    bool sortRequired = false;

    // check emulated sorting is required or not
    if (sortOrders.size() > 1) {
        for (int i = 0; i < sortOrders.size(); ++i) {
            if (sortOrders.at(i).type() != QLandmarkSortOrder::NameSort) {
                sortRequired = true;
                break;
            }
        }
    }

    // do sorting if required
    if (sortRequired) {
        // get all landmark data
        QList<QLandmark> landmarks;
        QLandmark qtLandmark;
        foreach (const QLandmarkId& lmId,landmarkIds)
            {
                // use landmark fetch method to get landmark from landmark id
                qtLandmark = landmark(lmId, error, errorString);
                if (*error != QLandmarkManager::NoError) {
                    landmarkIds.clear();
                    return false;
                }
                landmarks.append(qtLandmark);
            }

        landmarkIds.clear();
        landmarkIds = QLandmarkManagerEngineSymbian::sortLandmarks(landmarks, sortOrders);

        int resultcount = landmarkIds.size();
        if (offset < resultcount) {
            // do fetch based on offset and max items
            if (limit > 0) {
                if (offset < 0)
                    offset = 0;
                landmarkIds = landmarkIds.mid(offset, limit);
            }
            else if (offset > 0) {
                landmarkIds = landmarkIds.mid(offset, resultcount);
            }
        }
    }
    return true;
}

// end of file
