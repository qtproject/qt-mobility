/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Solutions Commercial License Agreement provided
 ** with the Software or, alternatively, in accordance with the terms
 ** contained in a written agreement between you and Nokia.
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
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** Please note Third Party Software included with Qt Solutions may impose
 ** additional restrictions and it is the user's responsibility to ensure
 ** that they have met the licensing requirements of the GPL, LGPL, or Qt
 ** Solutions Commercial license and the relevant license of the Third
 ** Party Software they are using.
 **
 ** If you are unsure which license is appropriate for your use, please
 ** contact the sales department at qt-sales@nokia.com.
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
#include <qlandmarkidfilter.h>
#include <qlandmarkabstractrequest.h>
#include <qlandmarkidfetchrequest.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarkfetchbyidrequest.h>
#include <qlandmarksaverequest.h>
#include <qlandmarkremoverequest.h>
#include <qlandmarkcategoryidfetchrequest.h>
#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategoryfetchbyidrequest.h>
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
#include <QIODevice>
#include <QFile>
#include <QBuffer>
#include <qnumeric.h>
#include <QSysInfo>
#include <qglobal.h>
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
#include <f32file.h>  
#include <APGCLI.H>
#include <APMSTD.H>
// user includes
#include "qlandmarkutility.h"
#include "qlandmarkmanagerengine_symbian.h"
// constants
_LIT(KDefaultTextCriteria,"*");
_LIT(KDefaultSpaceTextSearch,"* ");
_LIT8( KPosMimeTypeLandmarkCollectionXml,"application/vnd.nokia.landmarkcollection+xml" );

#define KAllLandmarks -1
#define KDefaultIndex 0
#define KExtrachars 3

QTM_BEGIN_NAMESPACE
uint qHash(const QLandmarkId& key) {
    return qHash(key.localId());
}
QTM_END_NAMESPACE

int compareDistance(const QGeoCoordinate &a, const QGeoCoordinate &b, const QGeoCoordinate &c)
{
    int result = 0;

    if (a.isValid()) {
        if (b.isValid()) {
            qreal da = c.distanceTo(a);
            qreal db = c.distanceTo(b);

            if (qFuzzyCompare(da, db)) {
                result = 0;
            }
            else if (da < db) {
                result = -1;
            }
            else if (da > db) {
                result = 1;
            }
        }
        else {
            result = -1;
        }
    }
    else {
        if (b.isValid()) {
            result = 1;
        }
        else {
            result = 0;
        }
    }

    return result;
}

void addSortedPoint(QList<QLandmark>* sorted, const QLandmark& landmark,
    const QGeoCoordinate &center)
{

    for (int i = 0; i < sorted->size(); i++) {
        // check to see if the new landmark should be inserted here
        int comparison = compareDistance(sorted->at(i).coordinate(), landmark.coordinate(), center);
        if (comparison > 0) {
            sorted->insert(i, landmark);
            return;
        }
    }

    // hasn't been inserted yet?  append to the list.
    sorted->append(landmark);
}

/**
 * Constructs CLandmarkDbEventHandler
 */
CLandmarkDbEventHandler LandmarkManagerEngineSymbianPrivate::m_DbEventHandler;

/**
 * Constructs LandmarkRequestHandler
 */
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

        HBufC* dbName = m_LandmarkDb->DatabaseUriLC();
        m_dbFilename = QString((QChar*) (dbName->Ptr()), dbName->Length());
        CleanupStack::PopAndDestroy(dbName);
    );

    m_DbEventHandler.AddObsever(this);
}

/*!
 Destructor.
 */
LandmarkManagerEngineSymbianPrivate::~LandmarkManagerEngineSymbianPrivate()
{
    m_DbEventHandler.RemoveObsever(this);
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
    parameters["filename"] = m_dbFilename;
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

    if (sortOrders.count() > 0) {
        for (int i=0; i < sortOrders.count(); ++i) {
            if (sortOrders.at(i).type() == QLandmarkSortOrder::NameSort) {
                QLandmarkNameSort nameSort = sortOrders.at(i);
                if (nameSort.caseSensitivity() == Qt::CaseSensitive) {
                    *error = QLandmarkManager::NotSupportedError;
                    *errorString = "Case Sensitive sorting is not supported";
                    return result;
                }
            }
        }
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

        int maxMatches;
        if (limit == 0) {
            result.clear();
            return result;
        }
        else if (limit < 0)
            maxMatches = -1;
        else
            maxMatches = limit + offset;

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
            bool haveProximityFilter = false;
            QLandmarkProximityFilter proximityFilter;
            int originalFilterCount = filters.count();
            for (int i = 0; i < originalFilterCount; ++i) {
                if (filters.at(i).type() == QLandmarkFilter::ProximityFilter) {
                    proximityFilter = filters.takeAt(i);
                    haveProximityFilter = true;

                    break;
                }
            }

            QSet<QLandmarkId> ids;
            QList<QLandmarkId> firstResult = landmarkIds(filters.at(0), limit, offset, QList<
                QLandmarkSortOrder> (), error, errorString);

            if (*error != QLandmarkManager::NoError) {
                result.clear();
                return result;
            }

            ids = firstResult.toSet();

            for (int i = 1; i < filters.size(); ++i) {
                QList<QLandmarkId> subResult = landmarkIds(filters.at(i), limit, offset, QList<
                    QLandmarkSortOrder> (), error, errorString);

                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }

                ids &= subResult.toSet();
            }

            QList<QLandmarkId> idList = ids.toList();
            if (haveProximityFilter) {
                QMap<int, QLandmarkManager::Error> errorMap;
                QList<QLandmark> lms = landmarks(idList, &errorMap, error, errorString);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }

                QList<QLandmark> sortedLandmarks;

                qreal radius = proximityFilter.radius();
                QGeoCoordinate center = proximityFilter.center();

                for (int i = 0; i < lms.count(); ++i) {
                    if (radius < 0 || (lms.at(i).coordinate().distanceTo(center) < radius)
                        || qFuzzyCompare(lms.at(i).coordinate().distanceTo(center), radius)) {
                        addSortedPoint(&sortedLandmarks, lms.at(i), center);
                    }
                }
                for (int i = 0; i < sortedLandmarks.count(); ++i) {
                    result << sortedLandmarks.at(i).landmarkId();
                }
            }
            else {
                result << idList;
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

    sortFetchedLmIds(limit, offset, sortOrders, result, filter.type(), error, errorString);

    return result;
}

/*!
 Returns a list of all category identifiers.  Any error which occurs will be saved in \a
 error and \a errorString.
 */
QList<QLandmarkCategoryId> LandmarkManagerEngineSymbianPrivate::categoryIds(int limit, int offset,
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    QList<QLandmarkCategoryId> qtCatIds;
    TRAPD(err, qtCatIds = fetchCategoryIdsL(nameSort, error, errorString);)
    if (err == KErrNone) {
        // do fetch based on offset and max items
        int resultcount = qtCatIds.size();

        if (offset >= resultcount) {
            qtCatIds.clear();
            return qtCatIds;
        }

        if (offset <=0 ) {
            offset = 0;
        }

        return qtCatIds.mid(offset,limit);
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    QLandmark qtLm;
    TRAPD( err, qtLm = fetchLandmarkL(landmarkId, error, errorString);)
    if (err != KErrNone) {
        if (err == KErrNotFound) {
            *error = QLandmarkManager::LandmarkDoesNotExistError;
            *errorString = "The specified landmark does not exist";
        }
        else {
            handleSymbianError(err, error, errorString);
        }

        return QLandmark();
    }
    return qtLm;
}

/**
 * 
 */
QList<QLandmark> LandmarkManagerEngineSymbianPrivate::landmarks(
    const QList<QLandmarkId> &landmarkIds, QMap<int, QLandmarkManager::Error> *errorMap,
    QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    if (errorMap)
        errorMap->clear();

    QList<QLandmark> result;
    if (&landmarkIds == 0) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid landmark ids";
        return result;
    }

    if (landmarkIds.isEmpty()) {
        *error = QLandmarkManager::NoError;
        *errorString = "";
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
            result << QLandmark();

            *error = lastError;
            *errorString = lastErrorString;
        }
    }

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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    QList<QLandmark> result;

    QList<QLandmarkId> ids = landmarkIds(filter, limit, offset, sortOrders, error, errorString);
    if (*error != QLandmarkManager::NoError) {
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    QLandmarkCategory qtCat;
    TRAPD( err, qtCat = fetchCategoryL(categoryId, error, errorString);)
    if (err != KErrNone) {
        if (err == KErrNotFound) {
            *error = QLandmarkManager::CategoryDoesNotExistError;
            *errorString = "The specified category does not exist";
        }
        else {
            // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
            handleSymbianError(err, error, errorString);
        }
        return QLandmarkCategory();
    }
    return qtCat;
}

/*!
 Returns a list of categories identified by \a landmarkCategoryIds.

 Overall operation errors are stored in \a error and \a errorString.
 */
QList<QLandmarkCategory> LandmarkManagerEngineSymbianPrivate::categories(const QList<
    QLandmarkCategoryId> &landmarkCategoryIds, QMap<int, QLandmarkManager::Error> *errorMap,
    QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    if (errorMap)
        errorMap->clear();

    QList<QLandmarkCategory> result;
    if (&landmarkCategoryIds == 0) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid category ids or empty ids";
        return result;
    }

    if(landmarkCategoryIds.isEmpty()) {
        *error = QLandmarkManager::NoError;
        *errorString = "";
        return result;
    }

    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString = "";
    for (int i = 0; i < landmarkCategoryIds.size(); ++i) {
        QLandmarkCategory cat = category(landmarkCategoryIds.operator[](i), &lastError,
            &lastErrorString);

        if (lastError == QLandmarkManager::NoError) {
            result << cat;
        }
        else {
            if (errorMap)
                errorMap->insert(i, lastError);
            result << QLandmarkCategory();

            *error = lastError;
            *errorString = lastErrorString;
        }
    }

    return result;
}

/*!
 Returns a list of categories identified by \a landmarkCategoryIds.

 Overall operation errors are stored in \a error and \a errorString.
 */
QList<QLandmarkCategory> LandmarkManagerEngineSymbianPrivate::categories(int limit, int offset,
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    if (limit == 0) {
        return QList<QLandmarkCategory> ();
    }

    QList<QLandmarkCategoryId> qtCatIds;
    TRAPD(err, qtCatIds = fetchCategoryIdsL(nameSort, error, errorString);)
    if (err == KErrNone && *error == QLandmarkManager::NoError && qtCatIds.size() > 0) {
        QList<QLandmarkCategory> qtCats;
        TRAP(err, qtCats = LandmarkUtility::getCategoriesL(m_LandmarkCatMgr, qtCatIds); )
        if (err == KErrNone) {

            // do fetch based on offset and max items
            int resultcount = qtCats.size();

            if (offset >= resultcount) {
                qtCats.clear();
                return qtCats;
            }
            if (offset <= 0) {
                offset = 0;
            }

            return qtCats.mid(offset, limit);
        }
    } else if (err == KErrNone && *error == QLandmarkManager::NotSupportedError) {
        // do nothing, since error code should already be set.
    } else {
        handleSymbianError(err, error, errorString);
    }

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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    bool added = false;
    bool changed = false;
    bool result = false;
    TRAPD(err,
        result = saveLandmarkInternalL(landmark, error, errorString, &added, &changed);
    )
    if (err != KErrNone) {
        // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
        if (err == KErrNotFound) {
            *error = QLandmarkManager::LandmarkDoesNotExistError;
            *errorString = "The specified landmark to save does not exist";
        }
        else {
            handleSymbianError(err, error, errorString);
        }
    }

    // handle to emit appropriate signal
    QList<QLandmarkId> landmarkIds;
    if (added) {
        landmarkIds.append(landmark->landmarkId());
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkAdded, landmarkIds);
    }
    if (changed) {
        landmarkIds.append(landmark->landmarkId());
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkUpdated, landmarkIds);
    }

    if (!result)
        //qDebug() << "Error " << *error << " = " << *errorString;
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    if (errorMap)
        errorMap->clear();

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
        bool result = false;
        TRAPD(err,
            result = saveLandmarkInternalL(&(landmarks->operator [](i)), &loopError, &loopErrorString,
                &added, &changed);
        )
        if (err != KErrNone) {
            result = false;
            if (err == KErrNotFound) {
                loopError = QLandmarkManager::LandmarkDoesNotExistError;
                loopErrorString = "The specified landmark to save does not exist";
            }
            else {
                handleSymbianError(err, &loopError, &loopErrorString);
            }
        }

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
        *error = QLandmarkManager::NoError;
        *errorString = "";
    }
    else {
        *error = lastError;
        *errorString = lastErrorString;
    }

    // handle to emit appropriate signal
    if (addedIds.size() > 0) {
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkAdded, addedIds);
    }
    if (changedIds.size() > 0) {
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    bool removed = false;
    bool result = false;
    TRAPD( err,
        result = removeLandmarkInternalL(landmarkId, error, errorString, &removed);
    )
    if (err != KErrNone) {
        // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
        if (err == KErrNotFound) {
            *error = QLandmarkManager::LandmarkDoesNotExistError;
            *errorString = "The specified landmark does not exist";
        }
        else {
            handleSymbianError(err, error, errorString);
        }
    }

    // handle to emit appropriate signal
    if (removed) {
        QList<QLandmarkId> landmarkIds;
        landmarkIds.append(landmarkId);
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkRemoved, landmarkIds);
    }

    if (!result)
        //qDebug() << "Error " << *error << " = " << *errorString;
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    if (errorMap)
        errorMap->clear();

    QList<QLandmarkId> removedIds;

    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;

    for (int i = 0; i < landmarkIds.size(); ++i) {

        loopError = QLandmarkManager::NoError;
        loopErrorString.clear();

        bool removed = false;
        bool result = false;
        TRAPD(err,
            result = removeLandmarkInternalL(landmarkIds.at(i), &loopError, &loopErrorString, &removed);
        )
        if (err != KErrNone) {
            // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
            if (err == KErrNotFound) {
                loopError = QLandmarkManager::LandmarkDoesNotExistError;
                loopErrorString = "The specified landmark does not exist";
            }
            else {
                handleSymbianError(err, &loopError, &loopErrorString);
            }
        }

        if (removed) {
            removedIds << landmarkIds.at(i);
        }

        if (!result) {
            if (errorMap)
                errorMap->insert(i, loopError);

            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        }
    }

    if (noErrors) {
        *error = QLandmarkManager::NoError;
        *errorString = "";
    }
    else {
        *error = lastError;
        *errorString = lastErrorString;
    }

    // handle to emit appropriate signal
    if (removedIds.size() > 0) {
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    bool added = false;
    bool changed = false;
    bool result = false;
    TRAPD(err,
        result = saveCategoryInternalL(category, error, errorString, &added, &changed);
    )
    if (err != KErrNone) {
        if (err == KErrNotFound) {
            *error = QLandmarkManager::CategoryDoesNotExistError;
            *errorString = "The specified category to update does not exist";
        }
        else {
            handleSymbianError(err, error, errorString);
        }
    }

    // handle to emit appropriate signal
    QList<QLandmarkCategoryId> categoryIds;
    if (added) {
        categoryIds.append(category->categoryId());
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryAdded, QList<
            QLandmarkId> (), categoryIds);
    }
    if (changed) {
        categoryIds.append(category->categoryId());
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryUpdated, QList<
            QLandmarkId> (), categoryIds);
    }

    if (!result)
        //qDebug() << "Error " << *error << " = " << *errorString;
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    bool removed = false;
    bool result = false;
    TRAPD( err,
        result = removeCategoryInternalL(categoryId, error, errorString, &removed);
    )
    if (err != KErrNone) {
        if (err == KErrNotFound) {
            *error = QLandmarkManager::CategoryDoesNotExistError;
            *errorString = "The specified category does not exist";
        }
        else {
            // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
            handleSymbianError(err, error, errorString);
        }
    }

    // handle to emit appropriate signal
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (!device) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid io device pointer";
        return false;
    }

    *error = QLandmarkManager::NoError;
    *errorString = "";

    bool status = false;

    if (option == QLandmarkManager::AttachSingleCategory) {
        category(categoryId, error, errorString);
        if (*error != QLandmarkManager::NoError) {
            return false;
        }
    }

    QList<QLandmarkId> importedIds;
    TRAPD(err,importedIds = importLandmarksL(device,format,option,categoryId));
    if (err == KErrNone) {
        status = true;
        //m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkAdded, importedIds);
    }
    else
        handleSymbianError(err, error, errorString);

    return status;
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
    const QList<QLandmarkId> &landmarkIds, QLandmarkManager::TransferOption option,
    QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (!device) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid io device pointer";
        return false;
    }

    *error = QLandmarkManager::NoError;
    *errorString = "";

    bool status = false;
    QLandmarkIdFilter filter;
    QList<QLandmarkSortOrder> sortOrders;
    QLandmarkNameSort nameSort;
    sortOrders.append(nameSort);

    filter.setLandmarkIds(landmarkIds);

    // Using QLandmarkIdFilter to determine wether all the told landmarks exists
    QList<QLandmarkId> fetchedLandmarkId;

    if (landmarkIds.isEmpty())
        fetchedLandmarkId = this->landmarkIds(QLandmarkFilter(), KAllLandmarks, KDefaultIndex,
            sortOrders, error, errorString);
    else
        fetchedLandmarkId = this->landmarkIds(filter, KAllLandmarks, KDefaultIndex, sortOrders,
            error, errorString);

    if (fetchedLandmarkId.isEmpty() || *error != QLandmarkManager::NoError)
        return status;

    if (landmarkIds.count() !=0 && landmarkIds.count() != fetchedLandmarkId.count()) {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = "All of the specified landmarks could not be found";
        return false;
    }

    TRAPD(err,exportLandmarksL(device,format,landmarkIds,option));
    if (err == KErrNone)
        status = true;
    else
        handleSymbianError(err, error, errorString);

    return status;
}

/*! Returns the support level the manager engine provides for the given \a filter.
 */
QLandmarkManager::SupportLevel LandmarkManagerEngineSymbianPrivate::filterSupportLevel(
    const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    QLandmarkManager::SupportLevel supportLevel = QLandmarkManager::NoSupport;

    switch (filter.type()) {

    case QLandmarkFilter::NameFilter:
    {
        QLandmarkNameFilter nameFilter = filter;
        if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive) {
            break;
        }

        if ((nameFilter.matchFlags() == QLandmarkFilter::MatchContains) && (QSysInfo::s60Version()
            == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version() == QSysInfo::SV_S60_3_2
            || QSysInfo::s60Version() == QSysInfo::SV_S60_5_0)) {

            break;
        }
    }
    case QLandmarkFilter::DefaultFilter:
    case QLandmarkFilter::LandmarkIdFilter:
    case QLandmarkFilter::CategoryFilter:
    case QLandmarkFilter::ProximityFilter:
    case QLandmarkFilter::BoxFilter:
    {
        supportLevel = QLandmarkManager::NativeSupport;
        break;
    }
    case QLandmarkFilter::AttributeFilter:
    {
        QLandmarkAttributeFilter attributeFilter = filter;

        QStringList keyList = attributeFilter.attributeKeys();
        QLandmarkAttributeFilter::OperationType opType = attributeFilter.operationType();

        if (opType == QLandmarkAttributeFilter::OrOperation && (QSysInfo::s60Version()
            == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version() == QSysInfo::SV_S60_3_2
            || QSysInfo::s60Version() == QSysInfo::SV_S60_5_0)) {

            break;
        }

        if (keyList.size() > 0) {

            //if any of the attribute matchflag is set to MatchCaseSensitive, then return KErrNotSupported 
            bool isSupported = true;
            for (int i = 0; i < keyList.size(); ++i) {
                if (attributeFilter.matchFlags(keyList.at(i)) & QLandmarkFilter::MatchCaseSensitive) {
                    isSupported = false;
                    break;
                }
                // if any of the attribute matchflag is set to MatchContains & operation type is AndOperation, 
                // then return KErrNotSupported
                if ((opType == QLandmarkAttributeFilter::AndOperation)
                    && attributeFilter.matchFlags(keyList.at(i)) & QLandmarkFilter::MatchContains) {
                    isSupported = false;
                    break;
                }
            }

            if(!isSupported)
                break;

            // symbian platform supported attributes
            QStringList lmkat = LandmarkUtility::searchableLandmarkAttributeKeys();

            for (int i = 0; i < keyList.size(); ++i) {
                if (!lmkat.contains(keyList.at(i))) {
                    isSupported = false;
                }
            }
            if (!isSupported)
                break;

            supportLevel = QLandmarkManager::NativeSupport;
        }
        break;
    }

    case QLandmarkFilter::IntersectionFilter:
    case QLandmarkFilter::UnionFilter:
    {
        supportLevel = QLandmarkManager::EmulatedSupport;
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
    const QLandmarkSortOrder &sortOrder, QLandmarkManager::Error *error,
    QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    QLandmarkManager::SupportLevel supportLevel = QLandmarkManager::NativeSupport;

    switch (sortOrder.type()) {
    case QLandmarkSortOrder::NoSort:
    case QLandmarkSortOrder::NameSort: {
            QLandmarkNameSort nameSort = sortOrder;
            if (nameSort.caseSensitivity() == Qt::CaseSensitive)
                supportLevel = QLandmarkManager::NoSupport;
        break;
    }
    default:
        supportLevel = QLandmarkManager::NoSupport;
        break;
    }

    return supportLevel;
}

/*!
 Returns true if the manager engine supports the given \a feature, otherwise returns false;
 */
bool LandmarkManagerEngineSymbianPrivate::isFeatureSupported(
    QLandmarkManager::ManagerFeature feature, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    switch (feature) {
    case (QLandmarkManager::ImportExportFeature):
    case (QLandmarkManager::NotificationsFeature):
        return true;
    default:
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid feature";
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return false;
}

/*!
 Returns true if the landmark identified by \a landmarkId
 considered read-only by the manager engine.

 If the \a landmarkId does not refer to an existing landmark,
 it is considered writable unless the manager engine. is exclusively read-only.

 */
bool LandmarkManagerEngineSymbianPrivate::isReadOnly(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    bool result = false;
    if (LandmarkUtility::validLocalId(categoryId.localId())) {
        result = LandmarkUtility::isGlobalCategoryId(m_LandmarkCatMgr,categoryId);
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    QStringList formats;

    if (operation == QLandmarkManager::ImportOperation) {

        if (QSysInfo::s60Version() == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version()
            == QSysInfo::SV_S60_3_2) {
            // for 3.1 & 3.2 platform only Lmx format supported.
            formats << QLandmarkManager::Lmx;
        }
        else if (QSysInfo::s60Version() == QSysInfo::SV_S60_5_0) {
            // for 5.0 only Lmx & Gpx format supported.
            formats << QLandmarkManager::Lmx;
            formats << QLandmarkManager::Gpx;
        }
        else {
            // from 9.2 onwards
            formats << QLandmarkManager::Lmx;
            formats << QLandmarkManager::Gpx;
            formats << QLandmarkManager::Kml;
            formats << QLandmarkManager::Kmz;
        }
    }
    else if (operation == QLandmarkManager::ExportOperation) {
        //Sqlite Plugin's GPX encoder can be used to support GPX export
        //formats << QLandmarkManager::Gpx;
        formats << QLandmarkManager::Lmx;
    }

    return formats;
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
    QLandmarkManager::Error error = QLandmarkManager::NoError;
    QString errorString = "";

    QLandmarkManagerEngineSymbian::updateRequestState(request,
        QLandmarkAbstractRequest::ActiveState);
    //qDebug() << "startRequestL-> RequestState = ActiveState ";
    TRAPD(errorId,
        result = startRequestL(request);
    )
    if (errorId == KErrNone) {

        if (request->type() == QLandmarkAbstractRequest::LandmarkIdFetchRequest) {

            QLandmarkIdFetchRequest *lmIdFetchRequest =
                static_cast<QLandmarkIdFetchRequest *> (request);

            if (lmIdFetchRequest->limit() == 0) {
                // for resultsAvailable signal
                QLandmarkManagerEngineSymbian::updateLandmarkIdFetchRequest(lmIdFetchRequest,
                    QList<QLandmarkId> (), error, errorString,
                    QLandmarkAbstractRequest::FinishedState);

                // for stateChanged signal
                QLandmarkManagerEngineSymbian::updateRequestState(request,
                    QLandmarkAbstractRequest::FinishedState);

                result = true;
            }

        }
        if (request->type() == QLandmarkAbstractRequest::LandmarkFetchRequest) {

            QLandmarkFetchRequest *lmfetchRequest = static_cast<QLandmarkFetchRequest*> (request);

            if (lmfetchRequest->limit() == 0) {
                // for resultsAvailable signal
                QLandmarkManagerEngineSymbian::updateLandmarkFetchRequest(lmfetchRequest, QList<
                    QLandmark> (), error, errorString, QLandmarkAbstractRequest::FinishedState);

                // for stateChanged signal
                QLandmarkManagerEngineSymbian::updateRequestState(request,
                    QLandmarkAbstractRequest::FinishedState);

                result = true;
            }
        }
    }
    else {

        handleSymbianError(errorId, &error, &errorString);

        switch (request->type()) {
        case QLandmarkAbstractRequest::LandmarkIdFetchRequest:
        {
            QLandmarkIdFetchRequest *lmIdFetchRequest =
                static_cast<QLandmarkIdFetchRequest *> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkIdFetchRequest(lmIdFetchRequest, QList<
                QLandmarkId> (), error, errorString, QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::CategoryIdFetchRequest:
        {
            QLandmarkCategoryIdFetchRequest *catIdFetchRequest =
                static_cast<QLandmarkCategoryIdFetchRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkCategoryIdFetchRequest(catIdFetchRequest,
                QList<QLandmarkCategoryId> (), error, errorString,
                QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::LandmarkFetchRequest:
        {
            QLandmarkFetchRequest *lmfetchRequest = static_cast<QLandmarkFetchRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkFetchRequest(lmfetchRequest, QList<
                QLandmark> (), error, errorString, QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::CategoryFetchRequest:
        {
            QLandmarkCategoryFetchRequest *catFetchRequest =
                static_cast<QLandmarkCategoryFetchRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkCategoryFetchRequest(catFetchRequest,
                QList<QLandmarkCategory> (), error, errorString,
                QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::LandmarkFetchByIdRequest:
        {
            QLandmarkFetchByIdRequest *lmFetchRequest =
                static_cast<QLandmarkFetchByIdRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkFetchByIdRequest(lmFetchRequest, QList<
                QLandmark> (), error, errorString, QMap<int, QLandmarkManager::Error> (),
                QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::CategoryFetchByIdRequest:
        {
            QLandmarkCategoryFetchByIdRequest *catFetchRequest =
                static_cast<QLandmarkCategoryFetchByIdRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkCategoryFetchByIdRequest(catFetchRequest,
                QList<QLandmarkCategory> (), error, errorString,
                QMap<int, QLandmarkManager::Error> (), QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::LandmarkSaveRequest:
        {
            QLandmarkSaveRequest *lmSaveRequest = static_cast<QLandmarkSaveRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkSaveRequest(lmSaveRequest,
                QList<QLandmark> (), error, errorString, QMap<int, QLandmarkManager::Error> (),
                QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::LandmarkRemoveRequest:
        {
            QLandmarkRemoveRequest *lmRemoveRequest =
                static_cast<QLandmarkRemoveRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkRemoveRequest(lmRemoveRequest, error,
                errorString, QMap<int, QLandmarkManager::Error> (),
                QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::CategorySaveRequest:
        {
            QLandmarkCategorySaveRequest *catSaveRequest =
                static_cast<QLandmarkCategorySaveRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkCategorySaveRequest(catSaveRequest, QList<
                QLandmarkCategory> (), error, errorString, QMap<int, QLandmarkManager::Error> (),
                QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::CategoryRemoveRequest:
        {
            QLandmarkCategoryRemoveRequest *catRemoveRequest =
                static_cast<QLandmarkCategoryRemoveRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkCategoryRemoveRequest(catRemoveRequest,
                error, errorString, QMap<int, QLandmarkManager::Error> (),
                QLandmarkAbstractRequest::FinishedState);

            break;
        }
        case QLandmarkAbstractRequest::ImportRequest:
        {
            QLandmarkImportRequest *importRequest = static_cast<QLandmarkImportRequest*> (request);

            // for resultsAvailable signal
            QLandmarkManagerEngineSymbian::updateLandmarkImportRequest(importRequest, QList<
                QLandmarkId> (), error, errorString, QLandmarkAbstractRequest::FinishedState);

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

        //qDebug() << "Request has been finished with error = " << error;

        result = true;
    }

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
        CPosLmCategoryManager::TCategorySortPref sortPref =
            CPosLmCategoryManager::ECategorySortOrderNone;

        if (request->type() == QLandmarkAbstractRequest::CategoryFetchRequest) {

            QLandmarkCategoryFetchRequest * catFetchRequest =
                static_cast<QLandmarkCategoryFetchRequest *> (request);

            if (catFetchRequest->sorting().caseSensitivity() == Qt::CaseSensitive)
                User::Leave(KErrNotSupported);

            if (catFetchRequest->sorting().direction() == Qt::AscendingOrder) {
                sortPref = CPosLmCategoryManager::ECategorySortOrderNameAscending;
            }
            else if (catFetchRequest->sorting().direction() == Qt::DescendingOrder) {
                sortPref = CPosLmCategoryManager::ECategorySortOrderNameDescending;
            }
        }
        else if (request->type() == QLandmarkAbstractRequest::CategoryIdFetchRequest) {

            QLandmarkCategoryIdFetchRequest * catIdFetchRequest =
                static_cast<QLandmarkCategoryIdFetchRequest *> (request);

            if (catIdFetchRequest->sorting().caseSensitivity() == Qt::CaseSensitive)
                User::Leave(KErrNotSupported);

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
        CleanupStack::Pop(landmarkSearch);
        CleanupStack::PopAndDestroy(catNameCriteria);

        // create request AO and start async request
        CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this, lmOperation);
        CleanupStack::PushL(requestAO);
        TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
        if (retn != KErrNone) {
            User::Leave(retn);
        }
        // start the request & transfer landmarkSearch object ownership
        requestAO->StartRequest(landmarkSearch);
        CleanupStack::Pop(requestAO);

        result = true;
        break;
    }
    case QLandmarkAbstractRequest::LandmarkIdFetchRequest:
    case QLandmarkAbstractRequest::LandmarkFetchRequest:
    {
        QLandmarkFilter filter;
        int maxMatches = 0;

        QList<QLandmarkSortOrder> sortOrders;

        if (request->type() == QLandmarkAbstractRequest::LandmarkIdFetchRequest) {
            QLandmarkIdFetchRequest *lmIdFetchRequest =
                static_cast<QLandmarkIdFetchRequest *> (request);

            filter = lmIdFetchRequest->filter();
            if (!lmIdFetchRequest->sorting().isEmpty()) {
                for (int i=0; i < lmIdFetchRequest->sorting().count(); ++i) {
                    if (lmIdFetchRequest->sorting().at(i).type() == QLandmarkSortOrder::NameSort) {
                        QLandmarkNameSort nameSort = QLandmarkNameSort(
                                lmIdFetchRequest->sorting().at(i));
                        if (nameSort.caseSensitivity() == Qt::CaseSensitive)
                            User::Leave(KErrNotSupported);
                    }
                }
                sortOrders = lmIdFetchRequest->sorting();
            }

            if (lmIdFetchRequest->limit() == 0) {
                User::Leave(KErrNone);
            }
            else if (lmIdFetchRequest->limit() < 0)
                maxMatches = -1;
            else
                maxMatches = lmIdFetchRequest->limit() + lmIdFetchRequest->offset();
        }

        if (request->type() == QLandmarkAbstractRequest::LandmarkFetchRequest) {

            QLandmarkFetchRequest *lmFetchRequest = static_cast<QLandmarkFetchRequest *> (request);

            filter = lmFetchRequest->filter();
            if (!lmFetchRequest->sorting().isEmpty()) {

                if (lmFetchRequest->sorting().at(0).type() == QLandmarkSortOrder::NameSort) {
                    QLandmarkNameSort nameSort = QLandmarkNameSort(lmFetchRequest->sorting().at(0));
                    if (nameSort.caseSensitivity() == Qt::CaseSensitive)
                        User::Leave(KErrNotSupported);
                }
                sortOrders = lmFetchRequest->sorting();
            }

            if (lmFetchRequest->limit() == 0) {
                User::Leave(KErrNone);
            }
            else if (lmFetchRequest->limit() < 0)
                maxMatches = -1;
            else
                maxMatches = lmFetchRequest->limit() + lmFetchRequest->offset();
        }

        bool multiplefetch = false;
        if (filter.type() == QLandmarkFilter::IntersectionFilter || filter.type()
            == QLandmarkFilter::UnionFilter) {
            multiplefetch = true;
        }

        if (filter.type() == QLandmarkFilter::LandmarkIdFilter || multiplefetch) {

            bool sortRequired = false;

            // check emulated sorting is required or not
            if (!sortOrders.isEmpty() && !multiplefetch) {
                for (int i = 0; i < sortOrders.size(); ++i) {
                    if (sortOrders.at(i).type() != QLandmarkSortOrder::NameSort) {
                        sortRequired = true;
                        break;
                    }
                }
            }

            if (sortRequired || multiplefetch) {

                // create request AO and start async request
                CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this);
                CleanupStack::PushL(requestAO);
                TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
                if (retn != KErrNone) {
                    CleanupStack::Pop(requestAO);
                    User::Leave(retn);
                }
                // start the request
                requestAO->StartRequest(NULL);
                CleanupStack::Pop(requestAO);
                break;
            }
        }

        CPosLmSearchCriteria* searchCriteria = getSearchCriteriaL(filter);
        if (!searchCriteria)
            return false;
        CPosLandmarkSearch* landmarkSearch = CPosLandmarkSearch::NewL(*m_LandmarkDb);
        CPosLmOperation* lmOperation = NULL;
        if (sortOrders.isEmpty())
            lmOperation = getOperationL(landmarkSearch, searchCriteria, QLandmarkSortOrder(),
                maxMatches);
        else
            lmOperation = getOperationL(landmarkSearch, searchCriteria, sortOrders.at(0),
                maxMatches);
        if (lmOperation == NULL) {
            delete landmarkSearch;
            landmarkSearch = NULL;
        }
        //delete searchCriteria;

        // create request AO and start async request
        CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this, lmOperation);
        CleanupStack::PushL(requestAO);
        TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
        if (retn != KErrNone) {
            User::Leave(retn);
        }

        // start the request & transfer landmarkSearch object ownership
        requestAO->StartRequest(landmarkSearch);

        CleanupStack::Pop(requestAO);
        result = true;
        break;
    }
    case QLandmarkAbstractRequest::LandmarkSaveRequest:
    case QLandmarkAbstractRequest::LandmarkRemoveRequest:
    case QLandmarkAbstractRequest::CategorySaveRequest:
    case QLandmarkAbstractRequest::CategoryRemoveRequest:
    case QLandmarkAbstractRequest::LandmarkFetchByIdRequest:
    case QLandmarkAbstractRequest::CategoryFetchByIdRequest:
    {
        CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this);
        CleanupStack::PushL(requestAO);

        TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
        if (retn != KErrNone) {
            CleanupStack::Pop(requestAO);
            User::Leave(retn);
        }
        requestAO->StartRequest(NULL);
        CleanupStack::Pop(requestAO);
        result = true;
        break;
    }
    case QLandmarkAbstractRequest::ImportRequest:
    {
        QLandmarkImportRequest * importRequest = static_cast<QLandmarkImportRequest*> (request);

        TPosLmItemId catId = LandmarkUtility::convertToSymbianLandmarkCategoryId(
            importRequest->categoryId());

        // check for AttachSingleCategory transfer option & association of provided category id.
        if ((catId == KPosLmNullItemId
            || (managerUri() != importRequest->categoryId().managerUri()))
            && (importRequest->transferOption() == QLandmarkManager::AttachSingleCategory))
            User::Leave(-20001);

        // check availability of category from provided category id.
        if (importRequest->transferOption() == QLandmarkManager::AttachSingleCategory) {
            QLandmarkManager::Error error;
            QString errorString = "";

            this->category(importRequest->categoryId(), &error, &errorString);
            if (error != QLandmarkManager::NoError)
                User::Leave(-20001);

            CPosLandmarkCategory *tempCategory = m_LandmarkCatMgr->ReadCategoryLC(catId);
            CleanupStack::PopAndDestroy(tempCategory);
        }

        QIODevice *outputdevice = 0;
        CPosLandmarkParser* parser = NULL;
        CPosLmOperation* importOperation = NULL;

        TInt transferOption = CPosLandmarkDatabase::EDefaultOptions;

        switch (importRequest->transferOption()) {
        case QLandmarkManager::IncludeCategoryData:
            transferOption = CPosLandmarkDatabase::EIncludeCategories;
            break;
        case QLandmarkManager::AttachSingleCategory:
            transferOption = CPosLandmarkDatabase::EDefaultOptions;
            break;
        case QLandmarkManager::ExcludeCategoryData:
            transferOption = CPosLandmarkDatabase::EDefaultOptions;
            break;
        default:
            break;
        }

        //Check if the import is from a file
        outputdevice = dynamic_cast<QFile *> (importRequest->device());
        if (outputdevice) {

            //Create a Qfile object to obtain the file from which to import
            QFile *filePath = (QFile *) importRequest->device();

            //Get the filename to make it symbian compatible
            QString filename = LandmarkUtility::preparePath(filePath->fileName());

            //Will hold the path and aid in symbian related import operations
            HBufC *importPath = HBufC::NewL(KMaxFileName);
            CleanupStack::PushL(importPath);

            TPtr pathPtr = importPath->Des();

            //RFs session to read a file
            RFs fs;
            CleanupClosePushL(fs);
            User::LeaveIfError(fs.Connect());

            //Enable sharing of the RFs session so that same can be used by symbian backend
            User::LeaveIfError(fs.ShareProtected());

            //Check and prepare the file name as needed by symbian 

            //Check the existence of the ":" in the path
            int colonpos = filename.indexOf(":", 0);

            //If : is present then its a full path that is provided, Try importing from the path provided
            if (colonpos == 1)
                pathPtr.Copy(filename.utf16());

            //If : the first char of the filename string, append the session path to prepare full path
            else if (colonpos == 0) {
                fs.SessionPath(pathPtr);
                filename.remove(":");
                if (filename.indexOf("\\", 0) == 0) {
                    filename.remove(0, 1);
                }
                pathPtr.Append(filename.utf16(), filename.size());
            }

            //if just the filename is provided try importing from the filename present in the session path
            else {
                fs.SessionPath(pathPtr);
                pathPtr.Append(filename.utf16(), filename.size());
            }

            //Check if the file exists
            if (!fs.IsValidName(pathPtr)) {
                fs.Close();
                CleanupStack::PopAndDestroy(&fs);
                CleanupStack::PopAndDestroy(importPath);

                User::Leave(KErrArgument);

            }

            //Open the file in the specified path
            RFile fileName;
            TInt err(fileName.Open(fs, pathPtr, EFileShareReadersOnly | EFileRead));

            //If opening of file failed leave
            if (err != KErrNone) {

                fs.Close();
                CleanupStack::PopAndDestroy(&fs);
                CleanupStack::PopAndDestroy(importPath);

                User::Leave(err);
            }

            //To determine if the Landmark package format used to create file is supported by parser
            RApaLsSession ls;
            TDataRecognitionResult dataType;

            err = ls.Connect();

            if (err != KErrNone) {
                CleanupStack::PopAndDestroy(importPath);
                delete filePath;

                User::Leave(err);
            }

            CleanupClosePushL(ls);

            //This is will get the mime type data of the file
            err = ls.RecognizeData(fileName, dataType);

            //If package format is not supported leave
            if (err == KErrNone && (dataType.iConfidence == CApaDataRecognizerType::EProbable
                || dataType.iConfidence == CApaDataRecognizerType::ECertain || dataType.iConfidence
                == CApaDataRecognizerType::EPossible)) {

                parser = CPosLandmarkParser::NewL(dataType.iDataType.Des8());
                CleanupStack::PushL(parser);
                parser->SetInputFileL(pathPtr);

            }

            else {
                fileName.Close();
                ls.Close();
                fs.Close();
                CleanupStack::PopAndDestroy(&ls);
                CleanupStack::PopAndDestroy(&fs);
                CleanupStack::PopAndDestroy(importPath);
                User::Leave(KErrArgument);
            }

            CleanupStack::Pop(parser);
            importOperation = m_LandmarkDb->ImportLandmarksL(*parser, transferOption);

            fileName.Close();
            fs.Close();
            ls.Close();
            CleanupStack::PopAndDestroy(&ls);
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(importPath);

        }
        else {
            // Check if the input as to be taken from a QBuffer            
            outputdevice = dynamic_cast<QBuffer *> (importRequest->device());
            if (outputdevice) {

                // check for format
                if (importRequest->format().isEmpty()) {
                    qDebug() << "Invalid Import Format Type";
                    User::Leave(KErrArgument);
                }

                // convert to symbian string format
                HBufC8* packageFormat = LandmarkUtility::landmarkPackageFormatsStrL(
                    importRequest->format());
                CleanupStack::PushL(packageFormat);

                TPtr8 ptrPath = packageFormat->Des();
                parser = CPosLandmarkParser::NewL(ptrPath);
                CleanupStack::PushL(parser);

                QBuffer* buffer = (QBuffer*) importRequest->device();

                HBufC8* lmxData = HBufC8::NewLC(buffer->size());
                TPtr8 lmDataPath = lmxData->Des();

                lmDataPath.Copy(reinterpret_cast<const unsigned char*> (buffer->data().data()));

                parser->SetInputBuffer(lmDataPath);

                CleanupStack::Pop(lmxData);
                CleanupStack::Pop(parser);
                CleanupStack::PopAndDestroy(packageFormat);

                importOperation = m_LandmarkDb->ImportLandmarksL(*parser, transferOption);

            }
            else {
                // invalid device has been provided.
                User::Leave(KErrArgument);
            }

        }

        // create request AO and start async request
        CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this, importOperation);
        CleanupStack::PushL(requestAO);
        TInt retn = m_RequestHandler.AddAsyncRequest(request, requestAO);
        if (retn != KErrNone) {
            User::Leave(retn);
        }

        // start the request & transfer landmarkSearch object ownership
        requestAO->StartRequest(NULL);

        CleanupStack::Pop(requestAO);

        result = true;

        break;
    }
    case QLandmarkAbstractRequest::ExportRequest:
    {
        QLandmarkExportRequest * exportRequest = static_cast<QLandmarkExportRequest*> (request);

        QLandmarkManager::Error error;
        QString errorString = "";
        QStringList exportFormats = supportedFormats(QLandmarkManager::ExportOperation, &error,
            &errorString);

        if (exportRequest->format().isEmpty()) {
            User::Leave(KErrArgument);
        }

        if (!exportFormats.contains(exportRequest->format(), Qt::CaseInsensitive)) {
            qDebug() << "Not Supported Export Format Type = " << exportRequest->format();
            User::Leave(KErrNotSupported);
        }

        CPosLmOperation* exportOperation = NULL;
        QIODevice *outputdevice = 0;
        QList<QLandmarkId> exportedLandmarkIds = exportRequest->landmarkIds();
        // Buffer to hold the Exported content temporarily
        CBufBase *bufferPath = NULL;
        HBufC *exportPath = NULL;

        // File system        
        RFs fs;

        RArray<TPosLmItemId> selectedLandmarks;

        // Convert the provided Qt landmark Ids to Symbian Ids
        if (exportedLandmarkIds.isEmpty()) {
            CPosLmItemIterator* iter = m_LandmarkDb->LandmarkIteratorL();
            CleanupStack::PushL(iter);

            TPosLmItemId lmID = KPosLmNullItemId;
            while ((lmID = iter->NextL()) != KPosLmNullItemId)
                selectedLandmarks.AppendL(lmID);
            CleanupStack::PopAndDestroy(iter);

        }
        else {

            QLandmarkIdFilter filter;
            QList<QLandmarkSortOrder> sortOrders;
            QLandmarkNameSort nameSort;
            sortOrders.append(nameSort);

            filter.setLandmarkIds(exportRequest->landmarkIds());

            // Using QLandmarkIdFilter to determine wether all the told landmarks exists
            QList<QLandmarkId> fetchedLandmarkIds;

            fetchedLandmarkIds = this->landmarkIds(filter, KAllLandmarks, KDefaultIndex,
                sortOrders, &error, &errorString);

            if (exportRequest->landmarkIds().count() != 0 && exportRequest->landmarkIds().count()
                != fetchedLandmarkIds.count()) {
                User::Leave(-20002); //errorId for QLandmarkManager::LandmarkDoesNotExistError
            }

            foreach(const QLandmarkId& id,fetchedLandmarkIds)
                    selectedLandmarks.AppendL(LandmarkUtility::convertToSymbianLandmarkId(id));
        }

        TInt transferOption = CPosLandmarkDatabase::EDefaultOptions;
        switch (exportRequest->transferOption()) {
        case QLandmarkManager::IncludeCategoryData:
            transferOption = CPosLandmarkDatabase::EIncludeCategories;
            break;
        case QLandmarkManager::ExcludeCategoryData:
            transferOption = CPosLandmarkDatabase::EDefaultOptions;
            break;
        case QLandmarkManager::AttachSingleCategory:
            transferOption = CPosLandmarkDatabase::EIncludeCategories;
            break;
        }

        // Encoder initialized with the supported landmark package format
        CPosLandmarkEncoder* encoder = CPosLandmarkEncoder::NewL(KPosMimeTypeLandmarkCollectionXml);

        // Check if the expected export is to a file
        outputdevice = dynamic_cast<QFile *> (exportRequest->device());
        if (outputdevice) {

            // create a Qfile object to obtain the file path
            QFile *filePath = (QFile *) outputdevice;

            QString filePathName = LandmarkUtility::preparePath(filePath->fileName());

            // Export path which will be used to prepare full path
            exportPath = HBufC::NewL(KMaxFileName);

            // RFs session to perform file save related operations
            RFs fs;
            User::LeaveIfError(fs.Connect());
            fs.ShareAuto();

            TPtr pathPtr = exportPath->Des();

            int colonpos = filePathName.indexOf(":", 0);

            if (colonpos == 1)
                pathPtr.Copy(filePathName.utf16());
            else if (colonpos == 0) {
                fs.SessionPath(pathPtr);
                filePathName.remove(":");
                pathPtr.Append(filePathName.utf16(), filePathName.size());
            }
            else {
                fs.SessionPath(pathPtr);
                pathPtr.Append(filePathName.utf16(), filePathName.size());
            }

            // Make sure file would be prepared in specified path
            TInt result(fs.MkDirAll(pathPtr));

            if (result != KErrNone && result != KErrAlreadyExists)
                User::Leave(result);

            // Delete the file if already exists, else export will fail
            result = fs.Delete(pathPtr);

            if (result != KErrNone && result != KErrNotFound) {
                User::Leave(result);
            }

            encoder->SetOutputFileL(pathPtr);

            // Performs export of landmarks  and cleanup of CPosLmOperation
            exportOperation = m_LandmarkDb->ExportLandmarksL(*encoder, selectedLandmarks,
                transferOption);

        }
        else {
            // Determine if the export path is a buffer
            outputdevice = dynamic_cast<QBuffer *> (exportRequest->device());
            if (outputdevice) {

                if (exportRequest->format().isEmpty()) {
                    User::Leave(KErrArgument);
                }

                // Set the encoder to write to a buffer
                bufferPath = encoder->SetUseOutputBufferL();

                // Performs export of landmarks  and cleanup of CPosLmOperation
                exportOperation = m_LandmarkDb->ExportLandmarksL(*encoder, selectedLandmarks,
                    transferOption);
            }
            else {
                // Invalid device has been provided.
                User::Leave(KErrArgument);
            }

        }

        // create request AO and start async request
        CLandmarkRequestAO* requestAO = CLandmarkRequestAO::NewL(this, exportOperation);
        CleanupStack::PushL(requestAO);
        TInt retn = m_RequestHandler.AddAsyncRequest(exportRequest, requestAO);
        if (retn != KErrNone) {
            User::Leave(retn);
        }

        // start the request & transfer landmarkSearch object ownership
        requestAO->StartRequest(NULL);
        requestAO->SetExportData(encoder, fs, exportPath, bufferPath, exportedLandmarkIds);
        CleanupStack::Pop(requestAO);

        result = true;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }// switch closure

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
    if (request->type() == QLandmarkAbstractRequest::ImportRequest) {
        // WFRF not supported for import request type operation.
        qDebug() << "Wait for Requst Finish for Import Operation not Supported";
        return false;
    }

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
bool LandmarkManagerEngineSymbianPrivate::saveLandmarkInternalL(QLandmark* landmark,
    QLandmarkManager::Error *error, QString *errorString, bool *added, bool *changed)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    Q_ASSERT(added);
    Q_ASSERT(changed);

    bool result = false;
    *added = false;
    *changed = false;

    if (landmark->name().size() > KPosLmMaxTextFieldLength) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString
            = "Landmark Name is greater than its maxlength i.e. KPosLmMaxTextFieldLength = 255.";
        return result;
    }

    if (landmark->description().size() > KPosLmMaxDescriptionLength) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString
            = "Landmark Desciption is greater than its maxlength i.e. KPosLmMaxDescriptionLength = 4095.";
        return result;
    }

    if (!landmark->viewport().isEmpty()) {
        *error = QLandmarkManager::NotSupportedError;
        *errorString = "Landmark viewport boundry settings not supported.";
        return result;
    }

    if (!landmark->landmarkId().managerUri().isEmpty() && landmark->landmarkId().managerUri()
        != managerUri()) {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = "Landmark id comes from different landmark manager.";
        return result;
    }

    bool categoryExists = LandmarkUtility::validCategoriesExist(m_LandmarkCatMgr, landmark,
        managerUri());
    if (!categoryExists) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString = "Landmark contains a category id that does not exist";
        return result;
    }

    if (landmark->iconUrl().toString().size() > KMaxFileName) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString
            = "Landmark Icon string is greater than its maxlength i.e. KMaxFileName = 256.";
        return result;
    }

    CPosLandmark* symbianLandmark = NULL;
    QLandmarkId landmarkId = landmark->landmarkId();

    // adding new landmark
    if (landmarkId.localId().isEmpty()) {
        symbianLandmark = LandmarkUtility::convertToSymbianLandmarkL(landmark);
        TPosLmItemId savedsymbianLmId = KPosLmNullItemId;
        savedsymbianLmId = m_LandmarkDb->AddLandmarkL(*symbianLandmark);
        QLandmarkId savedQtLmId = LandmarkUtility::convertToQtLandmarkId(managerUri(),
            savedsymbianLmId);
        landmark->setLandmarkId(savedQtLmId);

        m_CreatedLmIds << savedQtLmId.localId();

        //        qDebug() << "Landmark = " << landmark->name() << "LandmarkId = " << savedQtLmId.localId()
        //            << " Saved Successfully!";

        *added = true;
        result = true;
    }
    else if (!LandmarkUtility::validLocalId(landmarkId.localId())) {

        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = "Landmark not found because id is invalid.  For symbian the local "
            "id must be a string representation of an unsigned long type.";

        return result;
    }
    else {
        // check for existing landmark
        TPosLmItemId symbianLmId = LandmarkUtility::getSymbianLandmarkId(landmark);
        symbianLandmark = m_LandmarkDb->ReadLandmarkLC(symbianLmId);
        if (symbianLandmark) {
            // updating existing landmark
            LandmarkUtility::setSymbianLandmarkL(*symbianLandmark, landmark, m_LandmarkCatMgr);
            m_LandmarkDb->UpdateLandmarkL(*symbianLandmark);
            CleanupStack::PopAndDestroy(symbianLandmark);
            *changed = true;

            m_UpdatedLmIds << landmarkId.localId();

            result = true;
        }
    }
    return result;
}

/*
 * remove single landamrk
 */
bool LandmarkManagerEngineSymbianPrivate::removeLandmarkInternalL(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString, bool *removed)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    Q_ASSERT(removed);

    bool result = false;
    *removed = false;

    if (landmarkId.managerUri() != managerUri()) {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = "Landmark id comes from different landmark manager.";
        return result;
    }

    if (!LandmarkUtility::validLocalId(landmarkId.localId())) {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString
            = "LandmarkId not found because it is invalid. For Symbian the local id must be a string "
                "representing an unsigned long type.";
        return result;
    }

    TPosLmItemId symbianLmId = LandmarkUtility::convertToSymbianLandmarkId(landmarkId);

    CPosLandmark *lm = m_LandmarkDb->ReadLandmarkLC(symbianLmId);
    if (lm)
        CleanupStack::PopAndDestroy(lm);

    m_LandmarkDb->RemoveLandmarkL(symbianLmId);

    m_DeletedLmIds << landmarkId.localId();

    //    qDebug() << "Landmark id = " << landmarkId.localId() << "removed successfully.";

    *removed = true;
    result = true;

    return result;
}

/**
 * 
 */
QLandmark LandmarkManagerEngineSymbianPrivate::fetchLandmarkL(const QLandmarkId &landmarkId,
    QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (landmarkId.managerUri() != managerUri()) {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = "Landmark not found as landmark id comes from different landmark manager.";
        return QLandmark();
    }
    else if (!LandmarkUtility::validLocalId(landmarkId.localId())) {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString
            = "Landmark not found as local id is invalid. For symbian the local id is a string representation of "
                " an unsigned long type";
        return QLandmark();
    }

    TPosLmItemId symbianLmId = LandmarkUtility::convertToSymbianLandmarkId(landmarkId);
    CPosLandmark* symbianLandmark = m_LandmarkDb->ReadLandmarkLC(symbianLmId);
    if (symbianLandmark) {
        QLandmark* qtLandmark = LandmarkUtility::convertToQtLandmark(managerUri(), symbianLandmark);
        CleanupStack::PopAndDestroy(symbianLandmark);
        return *qtLandmark;
    }
    else {
        return QLandmark();
    }
}

/*
 * save single category
 */
bool LandmarkManagerEngineSymbianPrivate::saveCategoryInternalL(QLandmarkCategory* category,
    QLandmarkManager::Error *error, QString *errorString, bool *added, bool *changed)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    Q_ASSERT(added);
    Q_ASSERT(changed);

    bool result = false;
    *added = false;
    *changed = false;

    if (!category->categoryId().managerUri().isEmpty() && category->categoryId().managerUri()
        != managerUri()) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString = "Landmark Category id comes from different landmark manager.";
        return result;
    }

    if (category->name().size() > KPosLmMaxCategoryNameLength) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString
            = "Category Name is greater than its maxlength i.e. KPosLmMaxCategoryNameLength = 124.";
        return result;
    }

    if (category->iconUrl().toString().size() > KMaxFileName) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString
            = "Category Icon string is greater than its maxlength i.e. KMaxFileName = 256.";
        return result;
    }

    CPosLandmarkCategory* symbiancat = NULL;
    QLandmarkCategoryId categoryId = category->categoryId();

    if (categoryId.localId().isEmpty()) {
        symbiancat = LandmarkUtility::convertToSymbianLandmarkCategoryL(category);
        TPosLmItemId savedsymbianLmCatId = KPosLmNullItemId;
        savedsymbianLmCatId = m_LandmarkCatMgr->AddCategoryL(*symbiancat);
        QLandmarkCategoryId savedQtCategoryId = LandmarkUtility::convertToQtLandmarkCategoryId(
            managerUri(), savedsymbianLmCatId);
        category->setCategoryId(savedQtCategoryId);

        m_CreatedCatIds << savedQtCategoryId.localId();

        //        qDebug() << "category " << category->name() << " created successfully " << "cat Id ="
        //            << category->categoryId().localId();

        *added = true;
        result = true;
    }
    else if (!LandmarkUtility::validLocalId(categoryId.localId())) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Category not found because the local id is invalid.  "
            "For Symbian, the local id must the string representation of an unsigned long type.";
    }
    else {

        // check if category is global category 
        if (LandmarkUtility::isGlobalCategoryId(m_LandmarkCatMgr, category->categoryId())) {
            *error = QLandmarkManager::PermissionsError;
            *errorString = "Category is readonly, cannot be updated.";
            return result;
        }

        // check for existing category with category id
        TPosLmItemId symbianCatId = LandmarkUtility::convertToSymbianLandmarkCategoryId(
            category->categoryId());
        symbiancat = m_LandmarkCatMgr->ReadCategoryLC(symbianCatId);
        if (symbiancat) {
            LandmarkUtility::setSymbianLandmarkCategoryL(*symbiancat, category);
            m_LandmarkCatMgr->UpdateCategoryL(*symbiancat);

            //            qDebug() << "category " << category->name() << " updated successfully " << "cat Id ="
            //                << category->categoryId().localId();

            CleanupStack::PopAndDestroy(symbiancat);

            m_UpdatedCatIds << categoryId.localId();

            *changed = true;
            result = true;
        }
    }

    return result;
}

/*
 * remove single category
 */
bool LandmarkManagerEngineSymbianPrivate::removeCategoryInternalL(
    const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString,
    bool *removed)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    Q_ASSERT(removed);

    bool result = false;

    if (categoryId.managerUri() != managerUri()) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString
            = "Category not found because the id's manager uri refers to a different landmark manager.";
        return result;
    }
    else if (!LandmarkUtility::validLocalId(categoryId.localId())) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString = "Category not found because the local id is invalid.  "
            "For Symbian the local id must be the string representation of an unsigned long type";
        return result;
    }

    TPosLmItemId symbianCategoryId =
        LandmarkUtility::convertToSymbianLandmarkCategoryId(categoryId);

    CPosLandmarkCategory* cat = m_LandmarkCatMgr->ReadCategoryLC(symbianCategoryId);
    if (cat)
        CleanupStack::PopAndDestroy(cat);

    if (LandmarkUtility::isGlobalCategoryId(m_LandmarkCatMgr, categoryId)) {
        *error = QLandmarkManager::PermissionsError;
        *errorString = "Category is readonly, cannot be deleted.";
        return result;
    }

    ExecuteAndDeleteLD(m_LandmarkCatMgr->RemoveCategoryL(symbianCategoryId));

    m_DeletedCatIds << categoryId.localId();

    //    qDebug() << "category id = " << categoryId.localId() << " removed successfully";

    *removed = true;
    result = true;

    return result;
}

/*
 * fetch single category
 */
QLandmarkCategory LandmarkManagerEngineSymbianPrivate::fetchCategoryL(
    const QLandmarkCategoryId &landmarkCategoryId, QLandmarkManager::Error *error,
    QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (&landmarkCategoryId == 0) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString = "Category not found because the category id is invalid or empty";
        return QLandmarkCategory();
    }
    else if (landmarkCategoryId.managerUri() != managerUri()) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString
            = "Category not found because the id's manager uri refers to different manager";
        return QLandmarkCategory();
    }
    else if (!LandmarkUtility::validLocalId(landmarkCategoryId.localId())) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString
            = "Category not found because the local id is invalid.  For Symbian, the local id must the string representation of an unsigned long type.";
        return QLandmarkCategory();
    }

    TPosLmItemId symbianCategoryId = LandmarkUtility::convertToSymbianLandmarkCategoryId(
        landmarkCategoryId);
    CPosLandmarkCategory* symbiancat = symbiancat = m_LandmarkCatMgr->ReadCategoryLC(
        symbianCategoryId);
    if (symbiancat) {
        QLandmarkCategory* qtCat = LandmarkUtility::convertToQtLandmarkCategory(managerUri(),
            symbiancat);
        CleanupStack::PopAndDestroy(symbiancat);
        return *qtCat;
    }

    return QLandmarkCategory();
}

/*
 * fetch all category ids
 */
QList<QLandmarkCategoryId> LandmarkManagerEngineSymbianPrivate::fetchCategoryIdsL(
    const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (nameSort.caseSensitivity() == Qt::CaseSensitive) {
        *error = QLandmarkManager::NotSupportedError;
        *errorString
            = "Case Sensivity Support is not there, Please prefer CaseInsensitive to get result.";
        return QList<QLandmarkCategoryId> ();
    }

    QLandmarkNameSort Sort = nameSort;
    *error = QLandmarkManager::NoError;
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
        QLandmarkNameFilter nameFilter = filter;

        if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive) {
            qDebug() << "Symbian Landmarks Apis don't support MatchCaseSensitivity.";
            User::Leave(KErrNotSupported);
        }

        if ((nameFilter.matchFlags() & QLandmarkFilter::MatchContains) && (QSysInfo::s60Version()
            == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version() == QSysInfo::SV_S60_3_2
            || QSysInfo::s60Version() == QSysInfo::SV_S60_5_0)) {

            qDebug() << "3.1, 3.2, 5.0 don't support MatchContains.";
            User::Leave(KErrNotSupported);
        }

        HBufC* filterbuf = HBufC::NewL(nameFilter.name().size() + KExtrachars);
        TPtr filterName = filterbuf->Des();

        HBufC* filterbufcont = HBufC::NewL(nameFilter.name().size() + KExtrachars);
        TPtr filterNamecont = filterbufcont->Des();

        if (nameFilter.name().isEmpty() || nameFilter.name().isNull()) {
            filterName.Copy(KDefaultTextCriteria);
        }
        else {
            QString name = nameFilter.name();

            if (name.size() > (KPosLmMaxSearchStringLength - KExtrachars))
                User::Leave(KErrArgument);

            QLandmarkFilter::MatchFlags flag = nameFilter.matchFlags();
            switch (flag) {
            case QLandmarkFilter::MatchStartsWith:
            {
                filterName.Copy(name.utf16());
                filterName.Append(KDefaultTextCriteria);
                break;
            }
            case QLandmarkFilter::MatchEndsWith:
            {
                filterName.Copy(KDefaultTextCriteria);
                filterName.Append(name.utf16(), name.size());
                break;
            }
                //the text could be either *name* or * name*
            case QLandmarkFilter::MatchContains:
            {
                filterName.Copy(KDefaultTextCriteria);
                filterName.Append(name.utf16(), name.size());
                filterName.Append(KDefaultTextCriteria);

                filterNamecont.Copy(KDefaultSpaceTextSearch);
                filterNamecont.Append(name.utf16(), name.size());
                filterNamecont.Append(KDefaultTextCriteria);
                break;
            }
            case QLandmarkFilter::MatchExactly:
            case QLandmarkFilter::MatchFixedString:
            default:
            {
                filterName.Copy(name.utf16());
                break;
            }
            } //switch closure
        }

#ifndef SYMBIAN_OR_COMPOSITION_RESTRICTED            
        //used only for MatchContains case
        if (filterNamecont.Length() > 0) {
            //QString filterStr2((QChar*) (filterNamecont.Ptr()), filterNamecont.Length());
            //qDebug() << "filter string2  = " << filterStr2;

            CPosLmCompositeCriteria *compositeCriteria = CPosLmCompositeCriteria::NewLC(
                CPosLmCompositeCriteria::ECompositionOR);

            CPosLmTextCriteria* textNameCriteria = CPosLmTextCriteria::NewLC();
            textNameCriteria->SetTextL(filterName);
            textNameCriteria->SetAttributesToSearch(CPosLandmark::ELandmarkName);
            compositeCriteria->AddArgument(textNameCriteria);
            CleanupStack::Pop(textNameCriteria);

            CPosLmTextCriteria* textContainsName = CPosLmTextCriteria::NewLC();
            textContainsName->SetTextL(filterNamecont);
            textContainsName->SetAttributesToSearch(CPosLandmark::ELandmarkName);
            compositeCriteria->AddArgument(textNameCriteria);
            CleanupStack::Pop(textContainsName);

            CleanupStack::Pop(compositeCriteria);
            delete filterbuf;
            delete filterbufcont;

            return compositeCriteria;
        }
#endif // SYMBIAN_OR_COMPOSITION_RESTRICTED
        //QString filterStr((QChar*) (filterName.Ptr()), filterName.Length());
        //qDebug() << "filter string1  = " << filterStr;

        CPosLmTextCriteria* textNameCriteria = CPosLmTextCriteria::NewLC();
        textNameCriteria->SetTextL(filterName);
        textNameCriteria->SetAttributesToSearch(CPosLandmark::ELandmarkName);
        CleanupStack::Pop(textNameCriteria);

        delete filterbuf;
        delete filterbufcont;

        return textNameCriteria;
    }
    case QLandmarkFilter::LandmarkIdFilter:
    {
        QLandmarkIdFilter landmarkIdFilter = filter;
        QList<QLandmarkId> qtLmIds = landmarkIdFilter.landmarkIds();

        if (qtLmIds.isEmpty())
            User::Leave(KErrNone);

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
        CleanupStack::Pop(textCriteria);

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

        QGeoCoordinate qCoord = proximityFilter.center();
        if (LandmarkUtility::isValidLat(qCoord.latitude()) && LandmarkUtility::isValidLong(
            qCoord.longitude())) {
            symbianCoord.SetCoordinate(qCoord.latitude(), qCoord.longitude());
        }
        else {
            User::Leave(KErrArgument);
        }

        //check that proximity filer does not contain pole
        if (proximityFilter.radius() >= 0 && (proximityFilter.boundingCircle().contains(
            QGeoCoordinate(90, 0)) || proximityFilter.boundingCircle().contains(QGeoCoordinate(-90,
            0)))) {
            User::Leave(KErrArgument);
        }

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
        const TReal64& southlat = boxFilter.boundingBox().bottomRight().latitude();
        const TReal64& northlat = boxFilter.boundingBox().topLeft().latitude();
        const TReal64& westlong = boxFilter.boundingBox().topLeft().longitude();

        if (southlat > northlat)
            User::Leave(KErrArgument);
        if (LandmarkUtility::isValidLong(eastlong) && LandmarkUtility::isValidLat(southlat)
            && LandmarkUtility::isValidLat(northlat) && LandmarkUtility::isValidLong(westlong)) {
            CPosLmAreaCriteria* areaCriteria = CPosLmAreaCriteria::NewLC(southlat, northlat,
                westlong, eastlong);
            CleanupStack::Pop(areaCriteria);

            return areaCriteria;
        }
        else
            User::Leave(KErrArgument);

    }
    case QLandmarkFilter::AttributeFilter:
    {
        QLandmarkAttributeFilter attributeFilter = filter;

        QLandmarkAttributeFilter::OperationType opType = attributeFilter.operationType();

        if (opType == QLandmarkAttributeFilter::OrOperation && (QSysInfo::s60Version()
            == QSysInfo::SV_S60_3_1 || QSysInfo::s60Version() == QSysInfo::SV_S60_3_2
            || QSysInfo::s60Version() == QSysInfo::SV_S60_5_0)) {
            User::Leave(KErrNotSupported);
        }

        QStringList keyList = attributeFilter.attributeKeys();
        for (int i = 0; i < keyList.size(); ++i) {
            QLandmarkFilter::MatchFlags matchFlags = attributeFilter.matchFlags(keyList.at(i));
            matchFlags = (matchFlags & 3);
            if (matchFlags == QLandmarkFilter::MatchEndsWith) {
                //make sure we don't match with match ends with 0x3 (Match contains is 0x2)
            } else if (opType == QLandmarkAttributeFilter::AndOperation && (attributeFilter.matchFlags(
                keyList.at(i)) & QLandmarkFilter::MatchContains)) {
                User::Leave(KErrNotSupported);
            }
        }

        //if any of the attribute matchflag is set to MatchCaseSensitive, then return KErrNotSupported 
        for (int i = 0; i < keyList.size(); ++i) {
            if (attributeFilter.matchFlags(keyList.at(i)) & QLandmarkFilter::MatchCaseSensitive) {

                qDebug() << "Symbian Landmarks Apis don't support MatchCaseSensitive based search.";
                User::Leave(KErrNotSupported);
            }
        }

        // the attributes search which cannot be used in CPosLmTextCriteria
        if (keyList.contains("longitude") || keyList.contains("latitude") || keyList.contains(
            "radius") || keyList.contains("altitude") || keyList.contains("radius")
            || keyList.contains("iconurl")) {

            qDebug()
                << "Symbian Landmarks Apis don't support position based search & icon url based search.";
            User::Leave(KErrNotSupported);
        }

        CPosLmCompositeCriteria * compositeCriteria = NULL;
#ifndef SYMBIAN_OR_COMPOSITION_RESTRICTED
        compositeCriteria = CPosLmCompositeCriteria::NewLC(
            (CPosLmCompositeCriteria::TCompositionType) opType);
#else
        if (opType == QLandmarkAttributeFilter::AndOperation) {
            compositeCriteria = CPosLmCompositeCriteria::NewLC(
                CPosLmCompositeCriteria::ECompositionAND);
        }

#endif // SYMBIAN_OR_COMPOSITION_RESTRICTED 
        if (!compositeCriteria) {
            User::Leave(KErrNotSupported);
        }

        RArray<TUint> afieldArray;
        // Search with CPosLmTextCriteria fields only
        for (int i = 0; i < keyList.size(); ++i) {

            // skip not supported attributes
            if ((keyList.at(i) == "latitude") || (keyList.at(i) == "longitude") || (keyList.at(i)
                == "altitude") || (keyList.at(i) == "radius") || (keyList.at(i) == "iconurl"))
                continue;

            afieldArray.Reset();

            QVariant keyValue = attributeFilter.attribute(keyList.at(i));

            HBufC* filterbuf = HBufC::NewL(keyValue.toString().size() + KExtrachars);
            TPtr filterName = filterbuf->Des();

            HBufC* filterbufcont = HBufC::NewL(keyValue.toString().size() + KExtrachars);
            TPtr filterNamecont = filterbufcont->Des();

            // If filter attribute value is valid then search for that specific text based on matchflags, else return all the landmarks
            // whose that attribute is set to some valid value.
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
                    //the text could be either QString("*name*") or  QString("* *name*")
                case QLandmarkFilter::MatchContains:
                {
                    filterName.Copy(KDefaultTextCriteria);
                    filterName.Append(keyValue.toString().utf16(), keyValue.toString().size());
                    filterName.Append(KDefaultTextCriteria);

                    filterNamecont.Copy(KDefaultSpaceTextSearch);
                    filterNamecont.Copy(KDefaultTextCriteria);
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
                    User::Leave(KErrArgument);
                    break;
                }
            }
            else {
                filterName.Copy(KDefaultTextCriteria);
            }

            //qDebug() << "Attribute Key " << keyList.at(i);

            //QString filterStr1((QChar*) (filterName.Ptr()), filterName.Length());
            //qDebug() << "filter string1  = " << filterStr1;

            if (filterNamecont.Length() > 0) {
                QString filterStr2((QChar*) (filterNamecont.Ptr()), filterNamecont.Length());
                //qDebug() << "filter string2  = " << filterStr2;
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
                    compositeCriteria->AddArgument(textContainsName);
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
            else if (keyList.at(i) == "state") {

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
            else if (keyList.at(i) == "postcode") {
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
            else {
                qDebug() << "Not supported attribute provided";
                CleanupStack::PopAndDestroy(compositeCriteria);
                delete filterbuf;
                delete filterbufcont;
                User::Leave(KErrNotSupported);
            }

            delete filterbuf;
            delete filterbufcont;

        }

        CleanupStack::Pop(compositeCriteria);
        return compositeCriteria;
    }
    case QLandmarkFilter::IntersectionFilter:
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

    if (!landmarkSearch) {
        landmarkSearch = CPosLandmarkSearch::NewL(*m_LandmarkDb);
    }

    CPosLmPartialReadParameters* readParams = CPosLmPartialReadParameters::NewLC();
    readParams->SetRequestedAttributes(CPosLandmark::ELandmarkName);
    m_LandmarkDb->SetPartialReadParametersL(*readParams);
    if (maxMatches > 0)
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
    //delete searchCriteria;
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    switch (errorId) {
    case KErrNone:
    {
        *error = QLandmarkManager::NoError;
        *errorString = "";
        break;
    }
    case KErrPosLmUnknownFormat:
    {
        *error = QLandmarkManager::NotSupportedError;
        *errorString = "Unknown Landmark Package format file or data to import-export.";
        break;
    }
    case KErrNotSupported:
    {
        *error = QLandmarkManager::NotSupportedError;
        *errorString = "Not supported feature";
        break;
    }
    case KErrCancel:
    {
        *error = QLandmarkManager::CancelError;
        *errorString = "The request has been cancelled";
        break;
    }
    case KErrArgument:
    {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Invalid data provided";
        break;
    }
    case KErrBadName:
    {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Bad file name or bad file path";
        break;
    }
    case KErrPermissionDenied:
    {
        *error = QLandmarkManager::PermissionsError;
        *errorString = "Permission Denied, Invalid capabilities assigned.";
        break;
    }
    case KErrAccessDenied:
    {
        *error = QLandmarkManager::PermissionsError;
        *errorString = "The database is read only, access is denied.";
        break;
    }
    case KErrLocked:
    {
        *error = QLandmarkManager::LockedError;
        *errorString
            = "Database is locked for other operation, so no addition/updation/removal possible.";
        break;
    }
    case KErrNotFound:
    {
        *error = QLandmarkManager::DoesNotExistError;
        *errorString = "None of the existing landmarks/categories match the given id.";
        break;
    }
    case KErrPathNotFound:
    {
        *error = QLandmarkManager::DoesNotExistError;
        *errorString = "File Path does not exists.";
        break;
    }
    case KErrAlreadyExists:
    {
        // specific to category operation
        // this error code is returned while addition of new category, which name already exists in db.
        *error = QLandmarkManager::AlreadyExistsError;
        *errorString = "Category Name already exists, please provide unique category name.";
        break;
    }
    case KErrPosLmNotInitialized:
    {
        *error = QLandmarkManager::InvalidManagerError;
        *errorString = "Mangager is not constructed and initialized properly.";
        break;
    }
    case -20001: //reuse EHttpCannotFindServer to mean category does not exist
    {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString = "The specified category does not exist";
        break;
    }
    case -20002:
    {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = "The specified landmark does not exist";
        break;
    }
    default:
    {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Symbian Landmarks Error = %1").arg(errorId);
        qDebug() << *errorString;
        break;
    }
    } // switch closure
}

/**
 * HandleCompletion
 */
void LandmarkManagerEngineSymbianPrivate::HandleCompletionL(CLandmarkRequestData* aData)
{
    //qDebug() << " Back in HandleCompletionL()";

    QLandmarkManager::Error error = QLandmarkManager::NoError;
    QString errorString = 0;

    if (aData->error == QLandmarkManager::NoError) {
        handleSymbianError(aData->iErrorId, &error, &errorString);
    }
    else {
        error = aData->error;
        errorString = aData->errorString;
    }

    if (error != QLandmarkManager::NoError) {
        qDebug() << "Request Completed with " << error << " = " << errorString;
    }

    if (!aData->iQtRequest) {
        qDebug() << "Bad or Corrupted Request ";
        return;
    }

    switch (aData->iQtRequest->type()) {

    case QLandmarkAbstractRequest::LandmarkIdFetchRequest:
    {
        QLandmarkIdFetchRequest *lmIdFetchRequest =
            static_cast<QLandmarkIdFetchRequest *> (aData->iQtRequest);

        if (error != QLandmarkManager::NoError) {
            aData->iLandmarkIds.clear();
        }
        else if (aData->iLandmarkSearch) {

            CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
            RArray<TPosLmItemId> symbianLmIds;

            int lmcount = iterator->NumOfItemsL();

            if (lmcount > 0 && lmIdFetchRequest->offset() < lmcount) {
                iterator->GetItemIdsL(symbianLmIds, 0, iterator->NumOfItemsL());

                aData->iLandmarkIds = LandmarkUtility::convertToQtLandmarkIds(managerUri(),
                    symbianLmIds);
                symbianLmIds.Close();

            }
        }

        if (!aData->iLandmarkIds.isEmpty()) {

            sortFetchedLmIds(lmIdFetchRequest->limit(), lmIdFetchRequest->offset(),
                lmIdFetchRequest->sorting(), aData->iLandmarkIds,
                lmIdFetchRequest->filter().type(), &error, &errorString);

            if (lmIdFetchRequest->filter().type() == QLandmarkFilter::IntersectionFilter
                && !aData->iLandmarkIds.isEmpty()) {

                bool haveProximityFilter = false;
                QLandmarkProximityFilter proximityFilter;
                QLandmarkIntersectionFilter intersectionFilter = lmIdFetchRequest->filter();
                int originalFilterCount = intersectionFilter.filters().count();
                for (int i = 0; i < originalFilterCount; ++i) {
                    if (intersectionFilter.filters().at(i).type()
                        == QLandmarkFilter::ProximityFilter) {
                        proximityFilter = intersectionFilter.filters().takeAt(i);
                        haveProximityFilter = true;

                        break;
                    }
                }

                if (haveProximityFilter) {
                    QMap<int, QLandmarkManager::Error> errorMap;
                    QList<QLandmark> lms = landmarks(aData->iLandmarkIds, &errorMap, &error,
                        &errorString);
                    if (error != QLandmarkManager::NoError) {
                        aData->iLandmarkIds.clear();
                    }

                    QList<QLandmark> sortedLandmarks;

                    qreal radius = proximityFilter.radius();
                    QGeoCoordinate center = proximityFilter.center();

                    for (int i = 0; i < lms.count(); ++i) {
                        if (radius < 0 || (lms.at(i).coordinate().distanceTo(center) < radius)
                            || qFuzzyCompare(lms.at(i).coordinate().distanceTo(center), radius)) {
                            addSortedPoint(&sortedLandmarks, lms.at(i), center);
                        }
                    }
                    aData->iLandmarkIds.clear();
                    for (int i = 0; i < sortedLandmarks.count(); ++i) {
                        aData->iLandmarkIds << sortedLandmarks.at(i).landmarkId();
                    }
                }
            }
        }

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkIdFetchRequest(lmIdFetchRequest,
            aData->iLandmarkIds, error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::CategoryIdFetchRequest:
    {
        QLandmarkCategoryIdFetchRequest *catIdFetchRequest =
            static_cast<QLandmarkCategoryIdFetchRequest*> (aData->iQtRequest);

        if (error != QLandmarkManager::NoError) {
            aData->iCategoryIds.clear();
        }
        else if (aData->iLandmarkSearch) {
            CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
            RArray<TPosLmItemId> symbianCatIds;
            if (iterator->NumOfItemsL() > 0) {
                iterator->GetItemIdsL(symbianCatIds, 0, iterator->NumOfItemsL());
            }
            aData->iCategoryIds = LandmarkUtility::convertToQtLandmarkCategoryIds(managerUri(),
                symbianCatIds);
            symbianCatIds.Close();
        }

        int resultcount = aData->iCategoryIds.size();
        int offset = catIdFetchRequest->offset();
        if ( offset >= resultcount) {
            aData->iCategoryIds.clear();
        }

        if (offset <= 0) {
            offset = 0;
        }

        aData->iCategoryIds = aData->iCategoryIds.mid(offset, catIdFetchRequest->limit());

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategoryIdFetchRequest(catIdFetchRequest,
            aData->iCategoryIds, error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::LandmarkFetchRequest:
    {
        QLandmarkFetchRequest *lmfetchRequest =
            static_cast<QLandmarkFetchRequest*> (aData->iQtRequest);

        if (error != QLandmarkManager::NoError) {
            aData->iLandmarks.clear();
        }
        else if (aData->iLandmarkSearch) {

            CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
            RArray<TPosLmItemId> symbianLmIds;

            int lmcount = iterator->NumOfItemsL();

            //qDebug() << "lmcount = " << lmcount;

            if (lmcount > 0 && lmfetchRequest->offset() < lmcount) {

                iterator->GetItemIdsL(symbianLmIds, 0, iterator->NumOfItemsL());

                aData->iLandmarkIds = LandmarkUtility::convertToQtLandmarkIds(managerUri(),
                    symbianLmIds);
                symbianLmIds.Close();
            }
        }

        //qDebug() << "aData->iLandmarkIds.size() = " << aData->iLandmarkIds.size();

        if (!aData->iLandmarkIds.isEmpty()) {
            if (sortFetchedLmIds(lmfetchRequest->limit(), lmfetchRequest->offset(),
                lmfetchRequest->sorting(), aData->iLandmarkIds, lmfetchRequest->filter().type(),
                &error, &errorString)) {

                // get all landmark data
                QLandmark qtLandmark;
                aData->iLandmarks.clear();
                foreach (const QLandmarkId& lmId,aData->iLandmarkIds)
                    {
                        // use landmark fetch method to get landmark from landmark id
                        qtLandmark = landmark(lmId, &error, &errorString);
                        if (error != QLandmarkManager::NoError) {
                            aData->iLandmarks.clear();
                            break;
                        }
                        aData->iLandmarks.append(qtLandmark);
                    }
            }

            //qDebug() << "aData->iLandmarks.size() = " << aData->iLandmarks.size();

            if (lmfetchRequest->filter().type() == QLandmarkFilter::IntersectionFilter
                && !aData->iLandmarks.isEmpty()) {

                bool haveProximityFilter = false;
                QLandmarkProximityFilter proximityFilter;
                QLandmarkIntersectionFilter intersectionFilter = lmfetchRequest->filter();
                int originalFilterCount = intersectionFilter.filters().count();
                for (int i = 0; i < originalFilterCount; ++i) {
                    if (intersectionFilter.filters().at(i).type()
                        == QLandmarkFilter::ProximityFilter) {
                        proximityFilter = intersectionFilter.filters().takeAt(i);
                        haveProximityFilter = true;

                        break;
                    }
                }

                if (haveProximityFilter) {

                    QList<QLandmark> lms = aData->iLandmarks;
                    QList<QLandmark> sortedLandmarks;

                    qreal radius = proximityFilter.radius();
                    QGeoCoordinate center = proximityFilter.center();

                    for (int i = 0; i < lms.count(); ++i) {
                        if (radius < 0 || (lms.at(i).coordinate().distanceTo(center) < radius)
                            || qFuzzyCompare(lms.at(i).coordinate().distanceTo(center), radius)) {
                            addSortedPoint(&sortedLandmarks, lms.at(i), center);
                        }
                    }

                    aData->iLandmarks = sortedLandmarks;
                }
            }
        }
        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkFetchRequest(lmfetchRequest,
            aData->iLandmarks, error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::CategoryFetchRequest:
    {
        QLandmarkCategoryFetchRequest *catFetchRequest =
            static_cast<QLandmarkCategoryFetchRequest*> (aData->iQtRequest);

        if (error != QLandmarkManager::NoError) {
            aData->iCategories.clear();
        }
        else if (aData->iLandmarkSearch) {
            CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
            RArray<TPosLmItemId> symbianCatIds;
            if (iterator->NumOfItemsL() > 0) {
                iterator->GetItemIdsL(symbianCatIds, 0, iterator->NumOfItemsL());
            }
            aData->iCategoryIds = LandmarkUtility::convertToQtLandmarkCategoryIds(managerUri(),
                symbianCatIds);
            symbianCatIds.Close();

            // get all landmark category data
            QLandmarkCategory qtLmCategory;
            foreach (const QLandmarkCategoryId& lmCatId,aData->iCategoryIds)
                {
                    // use landmark fetch method to get landmark from landmark id
                    qtLmCategory = category(lmCatId, &error, &errorString);
                    if (error != QLandmarkManager::NoError) {
                        aData->iCategories.clear();
                        break;
                    }
                    aData->iCategories.append(qtLmCategory);
                }
        }

        int resultcount = aData->iCategories.size();
        int offset = catFetchRequest->offset();
        if ( offset >= resultcount) {
            aData->iCategories.clear();
        }

        if (offset <= 0) {
            offset = 0;
        }

        aData->iCategories = aData->iCategories.mid(offset, catFetchRequest->limit());

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategoryFetchRequest(catFetchRequest,
            aData->iCategories, error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::LandmarkFetchByIdRequest:
    {
        QLandmarkFetchByIdRequest *lmFetchRequest =
            static_cast<QLandmarkFetchByIdRequest*> (aData->iQtRequest);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkFetchByIdRequest(lmFetchRequest,
            aData->iLandmarks, error, errorString, aData->iErrorMap,
            QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::CategoryFetchByIdRequest:
    {
        QLandmarkCategoryFetchByIdRequest *catFetchRequest =
            static_cast<QLandmarkCategoryFetchByIdRequest*> (aData->iQtRequest);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategoryFetchByIdRequest(catFetchRequest,
            aData->iCategories, error, errorString, aData->iErrorMap,
            QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::LandmarkSaveRequest:
    {
        QLandmarkSaveRequest *lmSaveRequest =
            static_cast<QLandmarkSaveRequest*> (aData->iQtRequest);

        if (error == QLandmarkManager::CancelError) {
            for (int i = aData->iOpCount; i < lmSaveRequest->landmarks().size(); ++i) {
                aData->iErrorMap.insert(i, QLandmarkManager::CancelError);
            }
        }

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkSaveRequest(lmSaveRequest, aData->iLandmarks,
            error, errorString, aData->iErrorMap, QLandmarkAbstractRequest::FinishedState);

        // for landmarksAdded signal
        if (aData->iAddedLandmarkIds.size() > 0) {
            m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkAdded,
                aData->iAddedLandmarkIds);
        }

        // for landmarksChanged signal
        if (aData->iChangedLandmarkIds.size() > 0) {
            m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkUpdated,
                aData->iChangedLandmarkIds);
        }

        break;
    }
    case QLandmarkAbstractRequest::LandmarkRemoveRequest:
    {
        QLandmarkRemoveRequest *lmRemoveRequest =
            static_cast<QLandmarkRemoveRequest*> (aData->iQtRequest);

        if (error == QLandmarkManager::CancelError) {
            for (int i = aData->iOpCount; i < lmRemoveRequest->landmarkIds().size(); ++i) {
                aData->iErrorMap.insert(i, QLandmarkManager::CancelError);
            }
        }

        lmRemoveRequest->setLandmarkIds(aData->iLandmarkIds);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkRemoveRequest(lmRemoveRequest, error,
            errorString, aData->iErrorMap, QLandmarkAbstractRequest::FinishedState);

        // for landmarksRemoved signal
        if (aData->iRemovedLandmarkIds.size() > 0) {
            m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkRemoved,
                aData->iRemovedLandmarkIds);
        }

        break;
    }
    case QLandmarkAbstractRequest::CategorySaveRequest:
    {
        QLandmarkCategorySaveRequest *catSaveRequest =
            static_cast<QLandmarkCategorySaveRequest*> (aData->iQtRequest);

        if (error == QLandmarkManager::CancelError) {
            for (int i = aData->iOpCount; i < catSaveRequest->categories().size(); ++i) {
                aData->iErrorMap.insert(i, QLandmarkManager::CancelError);
            }
        }

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategorySaveRequest(catSaveRequest,
            aData->iCategories, error, errorString, aData->iErrorMap,
            QLandmarkAbstractRequest::FinishedState);

        // for categoriesAdded signal
        if (aData->iAddedCategoryIds.size() > 0) {
            m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryAdded, QList<
                QLandmarkId> (), aData->iAddedCategoryIds);
        }

        // for categoriesChanged signal
        if (aData->iChangedCategoryIds.size() > 0) {
            m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryUpdated, QList<
                QLandmarkId> (), aData->iChangedCategoryIds);
        }

        break;
    }
    case QLandmarkAbstractRequest::CategoryRemoveRequest:
    {
        QLandmarkCategoryRemoveRequest *catRemoveRequest =
            static_cast<QLandmarkCategoryRemoveRequest*> (aData->iQtRequest);

        if (error == QLandmarkManager::CancelError) {
            for (int i = aData->iOpCount; i < catRemoveRequest->categoryIds().size(); ++i) {
                aData->iErrorMap.insert(i, QLandmarkManager::CancelError);
            }
        }

        catRemoveRequest->setCategoryIds(aData->iCategoryIds);

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkCategoryRemoveRequest(catRemoveRequest, error,
            errorString, aData->iErrorMap, QLandmarkAbstractRequest::FinishedState);

        // for categoriesRemoved signal
        if (aData->iRemovedCategoryIds.size() > 0) {
            m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryRemoved, QList<
                QLandmarkId> (), aData->iRemovedCategoryIds);
        }

        break;
    }
    case QLandmarkAbstractRequest::ImportRequest:
    {
        QLandmarkImportRequest *importRequest =
            static_cast<QLandmarkImportRequest*> (aData->iQtRequest);

        QList<QLandmarkId> importedLandmarkIds;

        if (aData->iOwnerAO->GetOperation()) {

            CPosLmItemIterator* iter = m_LandmarkDb->ImportedLandmarksIteratorL(
                aData->iOwnerAO->GetOperation());
            CleanupStack::PushL(iter);

            RArray<TPosLmItemId> landmarkIds;

            TPosLmItemId lmID = KPosLmNullItemId;
            while ((lmID = iter->NextL()) != KPosLmNullItemId)
                landmarkIds.Append(lmID);

            CleanupStack::PopAndDestroy(iter);

            importedLandmarkIds
                = LandmarkUtility::convertToQtLandmarkIds(managerUri(), landmarkIds);

            //If add to single category is required  do that
            if (importRequest->transferOption() == QLandmarkManager::AttachSingleCategory) {

                TPosLmItemId catId = LandmarkUtility::convertToSymbianLandmarkCategoryId(
                    importRequest->categoryId());

                ExecuteAndDeleteLD(m_LandmarkCatMgr->AddCategoryToLandmarksL(catId, landmarkIds));

            }
        }

        // for resultsAvailable signal
        QLandmarkManagerEngineSymbian::updateLandmarkImportRequest(importRequest,
            importedLandmarkIds, error, errorString, QLandmarkAbstractRequest::FinishedState);

        break;
    }
    case QLandmarkAbstractRequest::ExportRequest:
    {
        QLandmarkExportRequest *exportRequest =
            static_cast<QLandmarkExportRequest*> (aData->iQtRequest);

        if (aData->iErrorId == KErrNone && aData->iLandmarkEncoder) {

            // finalize the encoder to save the data into the file
            ExecuteAndDeleteLD(aData->iLandmarkEncoder->FinalizeEncodingL());

            // Write to the QBuffer
            if (aData->iExportBuffer) {
                int n = aData->iExportBuffer->Size();
                HBufC8 *buffer = HBufC8::New(aData->iExportBuffer->Size() + 1);
                CleanupStack::PushL(buffer);
                TPtr8 ptr = buffer->Des();
                aData->iExportBuffer->Read(0, ptr, n);
                char* str = new (ELeave) char[ptr.Size() + 1];
                Mem::Copy(str, ptr.Ptr(), ptr.Size());
                str[ptr.Size()] = '\0';
                CleanupStack::PopAndDestroy(buffer);
                exportRequest->device()->write(str);
                delete aData->iExportBuffer;
                aData->iExportBuffer = NULL;
            }
            else {
                if (aData->iExportPath) {
                    delete aData->iExportPath;
                    aData->iExportPath = NULL;
                }
                aData->iFileSystem.Close();
            }

            delete aData->iLandmarkEncoder;
            aData->iLandmarkEncoder = NULL;

            exportRequest->setLandmarkIds(aData->iLandmarkIds);

        }
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
    QLandmarkManagerEngineSymbian::updateRequestState(aData->iQtRequest,
        QLandmarkAbstractRequest::FinishedState);

    /*
     qDebug() << "Completing the request successfully -> RequestState = FinishedState with error = "
     << error;
     */

    // reset the global request data.
    aData->Reset();
}

/**
 * handle sync operations execution
 */
void LandmarkManagerEngineSymbianPrivate::HandleExecutionL(CLandmarkRequestData* aData,
    TRequestStatus& aRequest)
{
    //qDebug() << "LandmarkManagerEngineSymbianPrivate::HandleExecutionL Started";

    QLandmarkManager::Error error = QLandmarkManager::NoError;
    QString errorString = 0;
    aRequest = KPosLmOperationNotComplete;

    switch (aData->iQtRequest->type()) {
    case QLandmarkAbstractRequest::LandmarkSaveRequest:
    {
        QLandmarkSaveRequest *lmSaveRequest =
            static_cast<QLandmarkSaveRequest *> (aData->iQtRequest);

        if (aData->iOpCount < lmSaveRequest->landmarks().size()) {

            bool added = false;
            bool changed = false;
            bool saveResult = false;

            QLandmark qtLm = (lmSaveRequest->landmarks()).at(aData->iOpCount);
            TRAPD(err,
                saveResult = saveLandmarkInternalL(&qtLm, &error, &errorString, &added, &changed);
            )
            if (err == KErrNone && saveResult) {
                aData->iLandmarks.append(qtLm);
            }
            else if (err != KErrNone) {
                // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
                if (err == KErrNotFound) {
                    error = QLandmarkManager::LandmarkDoesNotExistError;
                    errorString = "The specified landmark to update does not exist";
                }
                else {
                    handleSymbianError(err, &error, &errorString);
                }
            }

            if (!saveResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
                aData->error = error;
                aData->errorString = errorString;
                aData->iLandmarks.append((lmSaveRequest->landmarks()).at(aData->iOpCount));
            }

            if (added)
                aData->iAddedLandmarkIds << qtLm.landmarkId();
            if (changed)
                aData->iChangedLandmarkIds << qtLm.landmarkId();

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
            static_cast<QLandmarkRemoveRequest *> (aData->iQtRequest);

        if (aData->iOpCount < lmRemoveRequest->landmarkIds().size()) {
            bool removed = false;
            bool removeResult = false;

            QLandmarkId qtLmId = (lmRemoveRequest->landmarkIds()).at(aData->iOpCount);
            TRAPD(err,
                removeResult = removeLandmarkInternalL(qtLmId, &error, &errorString, &removed);
            )
            if (err == KErrNone && removeResult) {
                aData->iLandmarkIds.append(qtLmId);
            }
            else if (err != KErrNone) {
                // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
                if (err == KErrNotFound) {
                    error = QLandmarkManager::LandmarkDoesNotExistError;
                    errorString = "The specified landmark does not exist";
                }
                else {
                    handleSymbianError(err, &error, &errorString);
                }
            }

            if (!removeResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
                aData->error = error;
                aData->errorString = errorString;
                aData->iLandmarkIds.append(QLandmarkId());
            }

            if (removed) {
                aData->iRemovedLandmarkIds.append(qtLmId);
            }

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
            static_cast<QLandmarkCategorySaveRequest *> (aData->iQtRequest);

        if (aData->iOpCount < saveCategoryRequest->categories().count()) {

            bool added = false;
            bool changed = false;
            bool saveResult = false;

            QLandmarkCategory qtCat = (saveCategoryRequest->categories()).at(aData->iOpCount);
            TRAPD(err,
                saveResult = saveCategoryInternalL(&qtCat, &error, &errorString, &added, &changed);
            )
            if (err == KErrNone && saveResult) {
                aData->iCategories.append(qtCat);
            }
            else if (err != KErrNone) {
                if (err == KErrNotFound) {
                    error = QLandmarkManager::CategoryDoesNotExistError;
                    errorString = "The specified category to save does not exist";
                }
                else {
                    // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
                    handleSymbianError(err, &error, &errorString);
                }
            }

            if (!saveResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
                aData->error = error;
                aData->errorString = errorString;
                aData->iCategories.append((saveCategoryRequest->categories()).at(aData->iOpCount));
            }

            if (added)
                aData->iAddedCategoryIds << qtCat.categoryId();
            if (changed)
                aData->iChangedCategoryIds << qtCat.categoryId();

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
            static_cast<QLandmarkCategoryRemoveRequest *> (aData->iQtRequest);

        if (aData->iOpCount < catRemoveRequest->categoryIds().size()) {

            bool removed = false;
            bool removeResult = false;

            QLandmarkCategoryId qtCatId = (catRemoveRequest->categoryIds()).at(aData->iOpCount);
            TRAPD(err,
                removeResult = removeCategoryInternalL(qtCatId, &error, &errorString, &removed);
            )
            if (err == KErrNone && removeResult) {
                aData->iCategoryIds.append(qtCatId);
            }
            else if (err != KErrNone) {
                if (err == KErrNotFound) {
                    error = QLandmarkManager::CategoryDoesNotExistError;
                    errorString = "The specified category does not exist";
                }
                else {
                    // sets the appropriate QLandmarkManager Error code on Symbian Error codes.
                    handleSymbianError(err, &error, &errorString);
                }
            }

            if (!removeResult) {
                aData->iErrorMap.insert(aData->iOpCount, error);
                aData->error = error;
                aData->errorString = errorString;
                aData->iCategoryIds.append(QLandmarkCategoryId());
            }

            if (removed) {
                aData->iRemovedCategoryIds.append(qtCatId);
            }

            aData->iOpCount++;
        }
        else {
            // Set Complete.
            aRequest = KErrNone;
        }
        break;
    }
    case QLandmarkAbstractRequest::LandmarkFetchByIdRequest:
    {
        QLandmarkFetchByIdRequest *fetchRequest =
            static_cast<QLandmarkFetchByIdRequest *> (aData->iQtRequest);

        if (aData->iOpCount < fetchRequest->landmarkIds().size()) {

            QLandmarkId qtLmId = fetchRequest->landmarkIds().at(aData->iOpCount);
            QLandmark fetchedLandmark = landmark(qtLmId, &error, &errorString);

            if (error != QLandmarkManager::NoError) {
                aData->iErrorMap.insert(aData->iOpCount, error);
                aData->error = error;
                aData->errorString = errorString;
            }

            aData->iLandmarks.append(fetchedLandmark);

            aData->iOpCount++;
        }
        else {
            // Set Complete.
            aRequest = KErrNone;
        }

        break;
    }
    case QLandmarkAbstractRequest::CategoryFetchByIdRequest:
    {
        QLandmarkCategoryFetchByIdRequest *fetchRequest =
            static_cast<QLandmarkCategoryFetchByIdRequest *> (aData->iQtRequest);

        if (aData->iOpCount < fetchRequest->categoryIds().size()) {

            QLandmarkCategoryId qtCatId = fetchRequest->categoryIds().at(aData->iOpCount);
            QLandmarkCategory fetchedQtCategory = category(qtCatId, &error, &errorString);

            if (error != QLandmarkManager::NoError) {
                aData->iErrorMap.insert(aData->iOpCount, error);
                aData->error = error;
                aData->errorString = errorString;
            }

            aData->iCategories.append(fetchedQtCategory);

            aData->iOpCount++;
        }
        else {
            // Set Complete.
            aRequest = KErrNone;
        }

        break;
    }
    case QLandmarkAbstractRequest::LandmarkIdFetchRequest:
    case QLandmarkAbstractRequest::LandmarkFetchRequest:
    {
        QLandmarkFilter::FilterType filterType = QLandmarkFilter::DefaultFilter;
        QList<QLandmarkFilter> filters;
        QList<QLandmarkSortOrder> sortOrders;
        int maxMatches = KAllLandmarks;
        int limit = KAllLandmarks;
        int offset = KDefaultIndex;

        // for LandmarkIdFetchRequest
        if (aData->iQtRequest->type() == QLandmarkAbstractRequest::LandmarkIdFetchRequest) {

            QLandmarkIdFetchRequest *fetchRequest =
                static_cast<QLandmarkIdFetchRequest *> (aData->iQtRequest);

            filterType = fetchRequest->filter().type();

            if (filterType == QLandmarkFilter::LandmarkIdFilter) {
                // Set Complete.
                aRequest = KErrNone;
                break;
            }
            else if (filterType == QLandmarkFilter::IntersectionFilter) {
                QLandmarkIntersectionFilter intersectionFilter = fetchRequest->filter();
                filters = intersectionFilter.filters();
            }
            else if (filterType == QLandmarkFilter::UnionFilter) {
                QLandmarkUnionFilter unionFilter = fetchRequest->filter();
                filters = unionFilter.filters();
            }

            limit = fetchRequest->limit();
            offset = fetchRequest->offset();
            if (limit <= 0)
                maxMatches = -1;
            else
                maxMatches = limit + offset;
        }
        // for LandmarkFetchRequest
        else if (aData->iQtRequest->type() == QLandmarkAbstractRequest::LandmarkFetchRequest) {

            QLandmarkFetchRequest *fetchRequest =
                static_cast<QLandmarkFetchRequest *> (aData->iQtRequest);

            filterType = fetchRequest->filter().type();

            if (filterType == QLandmarkFilter::LandmarkIdFilter) {
                // Set Complete.
                aRequest = KErrNone;
                break;
            }
            else if (filterType == QLandmarkFilter::IntersectionFilter) {
                QLandmarkIntersectionFilter intersectionFilter = fetchRequest->filter();
                filters = intersectionFilter.filters();
            }
            else if (filterType == QLandmarkFilter::UnionFilter) {
                QLandmarkUnionFilter unionFilter = fetchRequest->filter();
                filters = unionFilter.filters();
            }

            limit = fetchRequest->limit();
            offset = fetchRequest->offset();
            if (limit <= 0)
                maxMatches = -1;
            else
                maxMatches = limit + offset;
        }

        //qDebug() << "aData->iOpCount = " << aData->iOpCount;
        //qDebug() << "filters.size() = " << filters.size();

        if (filters.isEmpty() || aData->iOpCount > filters.size()) {
            // Set Complete.
            aData->iOwnerAO->SetOperation(NULL);
            aRequest = KErrNone;
            break;
        }

        // update request result
        //if (aData->iOpCount > 0) {

        // collect searched landmark ids
        QList<QLandmarkId> result;

        // extract from symbian search
        if (aData->iLandmarkSearch) {
            CPosLmItemIterator *iterator = aData->iLandmarkSearch->MatchIteratorL();
            RArray<TPosLmItemId> symbianLmIds;

            if (iterator->NumOfItemsL() > 0) {

                //qDebug() << "lmCount = " << iterator->NumOfItemsL();
                iterator->GetItemIdsL(symbianLmIds, 0, iterator->NumOfItemsL());
                result = LandmarkUtility::convertToQtLandmarkIds(managerUri(), symbianLmIds);
                symbianLmIds.Close();
            }
            delete aData->iLandmarkSearch;
            aData->iLandmarkSearch = NULL;
        }

        else if (aData->iOpCount != filters.size() && filters.at(aData->iOpCount).type()
            == QLandmarkFilter::UnionFilter) {
            qDebug() << "nesting of union filter";
            result = this->landmarkIds(filters.at(aData->iOpCount), -1, 0, sortOrders, &error,
                &errorString);
            // add the first result for next and operation
            for (int j = 0; j < result.size(); ++j) {
                if (result.at(j).isValid())
                    aData->iLocalIds.insert(result.at(j).localId());
            }
            aData->iOwnerAO->SetOperation(NULL);
            aData->iOpCount++;
        }
        else if (aData->iOpCount != filters.size() && filters.at(aData->iOpCount).type()
            == QLandmarkFilter::IntersectionFilter) {
            qDebug() << "nesting of intersection filter";
            result = this->landmarkIds(filters.at(aData->iOpCount), -1, 0, sortOrders, &error,
                &errorString);
            // add the first result for next and operation
            for (int j = 0; j < result.size(); ++j) {
                if (result.at(j).isValid())
                    aData->iLocalIds.insert(result.at(j).localId());
            }
            aData->iOwnerAO->SetOperation(NULL);
            aData->iOpCount++;
        }

        // update the first result
        if (aData->iOpCount == 1) {

            // add the first result for next and operation
            for (int j = 0; j < result.size(); ++j) {
                if (result.at(j).isValid())
                    aData->iLocalIds.insert(result.at(j).localId());
            }
        } // if aData->iOpCount = 1

        // update request result with latest result
        else if (aData->iOpCount > 1 && aData->iOpCount <= filters.size()) {

            // for intersection filter
            if (filterType == QLandmarkFilter::IntersectionFilter) {
                QSet<QString> subIds;
                for (int j = 0; j < result.size(); ++j) {
                    if (result.at(j).isValid()) {
                        // add if not already exists
                        subIds.insert(result.at(j).localId());
                    }
                }
                // do and operation if intersection filter
                aData->iLocalIds &= subIds;
            }
            // for union filter
            else {
                for (int j = 0; j < result.size(); ++j) {
                    if (result.at(j).isValid()) {
                        // add if not already exists
                        aData->iLocalIds.insert(result.at(j).localId());
                    }
                }
            }

        } // if aData->iOpCount > 1

        //qDebug() << "aData->iLocalIds.size() = " << aData->iLocalIds.size();

        // for handle completion
        if (aData->iOpCount == filters.size()) {

            aData->iLandmarkIds.clear();
            QList<QString> idList = aData->iLocalIds.toList();
            for (int i = 0; i < idList.size(); ++i) {
                QLandmarkId id;
                id.setManagerUri(managerUri());
                id.setLocalId(idList.at(i));
                aData->iLandmarkIds << id;
            }

            aData->iOwnerAO->SetOperation(NULL);
            // Set Complete.
            aRequest = KErrNone;
            aData->iOpCount++;
            break;
        }

        // prepare next request
        CPosLmSearchCriteria* searchCriteria = getSearchCriteriaL(filters.at(aData->iOpCount));
        if (!searchCriteria) {
            aData->iOwnerAO->SetOperation(NULL);
            break;
        }

        //qDebug() << "search criteria type = " << searchCriteria->CriteriaType();

        aData->iLandmarkSearch = CPosLandmarkSearch::NewL(*m_LandmarkDb);
        CPosLmOperation* lmOperation = NULL;

        if (sortOrders.isEmpty())
            lmOperation = getOperationL(aData->iLandmarkSearch, searchCriteria,
                QLandmarkSortOrder(), maxMatches);
        else
            lmOperation = getOperationL(aData->iLandmarkSearch, searchCriteria, sortOrders.at(0),
                maxMatches);

        if (lmOperation == NULL && aData->iLandmarkSearch) {
            delete aData->iLandmarkSearch;
            aData->iLandmarkSearch = NULL;
        }

        delete searchCriteria;

        aData->iOwnerAO->SetOperation(lmOperation);
        aData->iOpCount++;

        break;
    }
    case QLandmarkAbstractRequest::ImportRequest:
    case QLandmarkAbstractRequest::ExportRequest:
    case QLandmarkAbstractRequest::CategoryIdFetchRequest:
    case QLandmarkAbstractRequest::CategoryFetchRequest:
    default:
    {
        // Set Complete.
        aRequest = KErrNone;
        break;
    }
    }// switch closure

    //qDebug() << "LandmarkManagerEngineSymbianPrivate::HandleExecutionL end";
}

/**
 * sorts if sorting required.
 * also filters the data with offset and limit
 */
bool LandmarkManagerEngineSymbianPrivate::sortFetchedLmIds(int limit, int offset, QList<
    QLandmarkSortOrder> sortOrders, QList<QLandmarkId>& landmarkIds,
    QLandmarkFilter::FilterType filterType, QLandmarkManager::Error *error, QString *errorString) const
{
    // if no search data found return empty list
    if (&landmarkIds == 0 || landmarkIds.isEmpty() || offset >= landmarkIds.size()) {
        landmarkIds.clear();
        return true;
    }

    //fetchRequired will prevent multiple fetches from database
    bool sortRequired = false;

    // check emulated sorting is required or not
    if (!sortOrders.isEmpty()) {
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
    }

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

    if ((filterType == QLandmarkFilter::IntersectionFilter || filterType
        == QLandmarkFilter::UnionFilter) && sortOrders.size() > 0) {
        QLandmarkIdFilter tmpFilter;
        tmpFilter.setLandmarkIds(landmarkIds);
        landmarkIds = this->landmarkIds(tmpFilter, KAllLandmarks, KDefaultIndex, sortOrders, error,
            errorString);
    }

    qDebug() << "result size = " << resultcount << " limit = " << limit << " offset = " << offset;
    return true;
}

/***
 * export landmarks
 */
void LandmarkManagerEngineSymbianPrivate::exportLandmarksL(QIODevice *device,
    const QString &format, const QList<QLandmarkId> &landmarkIds, QLandmarkManager::TransferOption option) const
{
    QIODevice *outputdevice = 0;

    // check for the format
    QLandmarkManager::Error error;
    QString errorString = "";
    QStringList exportFormats = supportedFormats(QLandmarkManager::ExportOperation, &error,
        &errorString);

    if (format.isEmpty()) {
        qDebug() << "Export Format Type is not provided ";
        User::Leave(KErrArgument);
    }

    if (!exportFormats.contains(format, Qt::CaseInsensitive)) {
        qDebug() << "Not Supported Export Format Type = " << format;
        User::Leave(KErrNotSupported);
    }

    // Encoder initialized with the supported landmark package format
    CPosLandmarkEncoder* encoder = CPosLandmarkEncoder::NewL(KPosMimeTypeLandmarkCollectionXml);
    CleanupStack::PushL(encoder);

    RArray<TPosLmItemId> selectedLandmarks;

    // Convert the provided Qt landmark Ids to Symbian Ids
    if (!landmarkIds.size()) {
        CPosLmItemIterator* iter = m_LandmarkDb->LandmarkIteratorL();

        CleanupStack::PushL(iter);

        TPosLmItemId lmID = KPosLmNullItemId;

        while ((lmID = iter->NextL()) != KPosLmNullItemId)
            selectedLandmarks.AppendL(lmID);
        CleanupStack::PopAndDestroy(iter);

    }
    else {
        foreach(const QLandmarkId& id,landmarkIds)
                selectedLandmarks.AppendL(LandmarkUtility::convertToSymbianLandmarkId(id));
    }

    TInt transferOption = CPosLandmarkDatabase::EDefaultOptions;
    switch (option) {
    case QLandmarkManager::IncludeCategoryData:
        transferOption = CPosLandmarkDatabase::EIncludeCategories;
        break;
    case QLandmarkManager::ExcludeCategoryData:
        transferOption = CPosLandmarkDatabase::EDefaultOptions;
        break;
    case QLandmarkManager::AttachSingleCategory:
        transferOption = CPosLandmarkDatabase::EIncludeCategories;
        break;
    default:
        break;
    }

    // Check if the expected export is to a file
    outputdevice = dynamic_cast<QFile *> (device);
    if (outputdevice) {

        // create a Qfile object to obtain the file path
        QFile *filePath = (QFile *) device;

        QString filePathName = LandmarkUtility::preparePath(filePath->fileName());

        // Export path which will be used to prepare full path
        HBufC *exportPath = HBufC::NewL(KMaxFileName);
        CleanupStack::PushL(exportPath);

        // RFs session to perform file save related operations
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        fs.ShareAuto();

        TPtr pathPtr = exportPath->Des();

        int colonpos = filePathName.indexOf(":", 0);

        if (colonpos == 1)
            pathPtr.Copy(filePathName.utf16());
        else if (colonpos == 0) {
            fs.SessionPath(pathPtr);
            filePathName.remove(":");
            pathPtr.Append(filePathName.utf16(), filePathName.size());
        }
        else {
            fs.SessionPath(pathPtr);
            pathPtr.Append(filePathName.utf16(), filePathName.size());
        }

        // Make sure file would be prepared in specified path
        TInt result(fs.MkDirAll(pathPtr));

        if (result != KErrNone && result != KErrAlreadyExists) {
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(exportPath);
            User::Leave(result);
        }

        // Delete the file if already exists, else export will fail
        result = fs.Delete(pathPtr);

        if (result != KErrNone && result != KErrNotFound) {
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(exportPath);
            User::Leave(result);
        }

        encoder->SetOutputFileL(pathPtr);

        // Performs export of landmarks  and cleanup of CPosLmOperation
        ExecuteAndDeleteLD(m_LandmarkDb->ExportLandmarksL(*encoder, selectedLandmarks,
            transferOption));

        ExecuteAndDeleteLD(encoder->FinalizeEncodingL());
        fs.Close();
        CleanupStack::PopAndDestroy(&fs);
        CleanupStack::PopAndDestroy(exportPath);
        CleanupStack::PopAndDestroy(encoder);

    }
    else {

        // Determine if the export path is a buffer
        outputdevice = dynamic_cast<QBuffer *> (device);
        if (outputdevice) {

            if (!device->isWritable())
                User::Leave(KErrArgument);

            // Buffer to hold the Exported content temporarily
            CBufBase *bufferPath;

            // Set the encoder to write to a buffer
            bufferPath = encoder->SetUseOutputBufferL();

            // Performs export of landmarks  and cleanup of CPosLmOperation
            ExecuteAndDeleteLD(m_LandmarkDb->ExportLandmarksL(*encoder, selectedLandmarks,
                transferOption));

            ExecuteAndDeleteLD(encoder->FinalizeEncodingL());

            //Compress the buffer file to save space
            bufferPath->Compress();

            //Allocate sufficient memory to read from bufer
            HBufC8 *buffer = HBufC8::New(bufferPath->Size() + 1);
            CleanupStack::PushL(buffer);
            TPtr8 ptr = buffer->Des();

            //Read the buffer contents
            bufferPath->Read(0, ptr, bufferPath->Size());

            //covert to char*
            char* str = new (ELeave) char[ptr.Size() + 1];
            Mem::Copy(str, ptr.Ptr(), ptr.Size());
            str[ptr.Size()] = '\0';

            //Write to the device
            device->write(str);

            CleanupStack::PopAndDestroy(buffer);
            CleanupStack::PopAndDestroy(encoder);

        }
        else {

            // The provided destination not supported
            User::Leave(KErrNotSupported);

        }
    }

}

/**
 * import landmarks
 */
QList<QLandmarkId> LandmarkManagerEngineSymbianPrivate::importLandmarksL(QIODevice *device,
    const QString &format, QLandmarkManager::TransferOption option,
    const QLandmarkCategoryId& categoryId) const
{
    QIODevice *outputdevice = 0;
    CPosLandmarkParser* parser = NULL;
    QList<QLandmarkId> importedLmIds;
    RArray<TPosLmItemId> landmarkIds;
    bool addSinglecategoryrequired = false;

    TPosLmItemId catId = LandmarkUtility::convertToSymbianLandmarkCategoryId(categoryId);
    if ((catId == KPosLmNullItemId || (managerUri() != categoryId.managerUri())) && (option
        == QLandmarkManager::AttachSingleCategory)) {

        User::Leave(KErrArgument);

    }

    if (option == QLandmarkManager::AttachSingleCategory) {

        CPosLandmarkCategory *tempCategory = m_LandmarkCatMgr->ReadCategoryLC(catId);
        CleanupStack::PopAndDestroy(tempCategory);

    }

    // TODO : uncomment if needed to check format, 
    // else the format will be autodetected internally using mimetype for file input.
    // check for the format
    /*
     if (format.isEmpty()) {
     qDebug() << "Invalid Format Type";
     User::Leave(KErrArgument);
     }
     */

    TInt transferOption = CPosLandmarkDatabase::EDefaultOptions;

    switch (option) {

    case QLandmarkManager::IncludeCategoryData:
        transferOption = CPosLandmarkDatabase::EIncludeCategories;
        break;
    case QLandmarkManager::AttachSingleCategory:
        transferOption = CPosLandmarkDatabase::EDefaultOptions;
        addSinglecategoryrequired = true;
        break;
    case QLandmarkManager::ExcludeCategoryData:
        transferOption = CPosLandmarkDatabase::EDefaultOptions;
        break;
    default:
        break;

    }

    //Check if the import is from a file
    outputdevice = dynamic_cast<QFile *> (device);
    if (outputdevice) {
        //Create a Qfile object to obtain the file from which to import
        QFile *filePath = (QFile *) device;

        //Get the filename to make it symbian compatible
        QString filename = LandmarkUtility::preparePath(filePath->fileName());

        //Will hold the path and aid in symbian related import operations
        HBufC *importPath = HBufC::NewL(KMaxFileName);
        CleanupStack::PushL(importPath);

        TPtr pathPtr = importPath->Des();

        //RFs session to read a file
        RFs fs;
        CleanupClosePushL(fs);
        User::LeaveIfError(fs.Connect());

        //Enable sharing of the RFs session so that same can be used by symbian backend
        User::LeaveIfError(fs.ShareProtected());

        //Check and prepare the file name as needed by symbian 

        //Check the existence of the ":" in the path
        int colonpos = filename.indexOf(":", 0);

        //If : is present then its a full path that is provided, Try importing from the path provided
        if (colonpos == 1)
            pathPtr.Copy(filename.utf16());

        //If : the first char of the filename string, append the session path to prepare full path
        else if (colonpos == 0) {

            fs.SessionPath(pathPtr);
            filename.remove(":");
            if (filename.indexOf("\\", 0) == 0) {
                filename.remove(0, 1);
            }
            pathPtr.Append(filename.utf16(), filename.size());

        }
        //if just the filename is provided try importing from the filename present in the session path
        else {

            fs.SessionPath(pathPtr);
            pathPtr.Append(filename.utf16(), filename.size());

        }

        //Check if the file exists
        if (!fs.IsValidName(pathPtr)) {

            fs.Close();
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(importPath);

            User::Leave(KErrArgument);

        }

        //Open the file in the specified path
        RFile fileName;
        TInt err(fileName.Open(fs, pathPtr, EFileShareReadersOnly | EFileRead));

        //If opening of file failed leave
        if (err != KErrNone) {

            fs.Close();
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(importPath);

            User::Leave(err);
        }

        //To determine if the Landmark package format used to create file is supported by parser
        RApaLsSession ls;
        TDataRecognitionResult dataType;

        err = ls.Connect();

        if (err != KErrNone) {
            CleanupStack::PopAndDestroy(importPath);
            delete filePath;

            User::Leave(err);
        }

        CleanupClosePushL(ls);

        //This is will get the mime type data of the file
        err = ls.RecognizeData(fileName, dataType);

        //If package format is not supported leave
        if (err == KErrNone && (dataType.iConfidence == CApaDataRecognizerType::EProbable
            || dataType.iConfidence == CApaDataRecognizerType::ECertain || dataType.iConfidence
            == CApaDataRecognizerType::EPossible)) {

            parser = CPosLandmarkParser::NewL(dataType.iDataType.Des8());
            CleanupStack::PushL(parser);
            parser->SetInputFileL(pathPtr);

        }
        else {

            fileName.Close();
            ls.Close();
            fs.Close();
            CleanupStack::PopAndDestroy(&ls);
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(importPath);
            User::Leave(KErrArgument);

        }

        CPosLmOperation* op = m_LandmarkDb->ImportLandmarksL(*parser, transferOption);
        CleanupStack::PushL(op);

        // Execute the operation in one batch
        op->ExecuteL();

        CPosLmItemIterator* iter = m_LandmarkDb->ImportedLandmarksIteratorL(op);
        CleanupStack::PushL(iter);

        TPosLmItemId lmID = KPosLmNullItemId;
        while ((lmID = iter->NextL()) != KPosLmNullItemId)
            landmarkIds.Append(lmID);

        CleanupStack::PopAndDestroy(iter);
        fileName.Close();
        fs.Close();
        ls.Close();
        CleanupStack::PopAndDestroy(op);
        CleanupStack::PopAndDestroy(parser);
        CleanupStack::PopAndDestroy(&ls);
        CleanupStack::PopAndDestroy(&fs);
        CleanupStack::PopAndDestroy(importPath);

    }
    else {

        // Check if the input as to be taken from a QBuffer
        outputdevice = dynamic_cast<QBuffer *> (device);
        if (outputdevice) {

            if (format.isEmpty()) {
                qDebug() << "Invalid Format Type, format type is mandatory for buffer input";
                User::Leave(KErrArgument);
            }

            //
            HBufC8* packageFormat = LandmarkUtility::landmarkPackageFormatsStrL(format);
            CleanupStack::PushL(packageFormat);

            TPtr8 ptrPath = packageFormat->Des();
            parser = CPosLandmarkParser::NewL(ptrPath);
            CleanupStack::PushL(parser);

            QBuffer* buffer = (QBuffer*) device;

            HBufC8* lmxData = HBufC8::NewLC(buffer->size());
            TPtr8 lmDataPath = lmxData->Des();

            lmDataPath.Copy(reinterpret_cast<const unsigned char*> (buffer->data().data()));

            parser->SetInputBuffer(lmDataPath);

            CPosLmOperation* op = m_LandmarkDb->ImportLandmarksL(*parser, transferOption);
            CleanupStack::PushL(op);

            // Execute the operation in one batch
            op->ExecuteL();

            CPosLmItemIterator* iter = m_LandmarkDb->ImportedLandmarksIteratorL(op);
            CleanupStack::PushL(iter);

            TPosLmItemId lmID = KPosLmNullItemId;
            while ((lmID = iter->NextL()) != KPosLmNullItemId)
                landmarkIds.Append(lmID);

            CleanupStack::PopAndDestroy(iter);
            CleanupStack::PopAndDestroy(op);
            CleanupStack::PopAndDestroy(lmxData);
            CleanupStack::PopAndDestroy(parser);
            CleanupStack::PopAndDestroy(packageFormat);

        }
        else {

            // not supported device
            User::Leave(KErrNotSupported);

        }

    }

    //If add to single category is required  do that
    if (addSinglecategoryrequired) {
        ExecuteAndDeleteLD(m_LandmarkCatMgr->AddCategoryToLandmarksL(catId, landmarkIds));
    }

    importedLmIds = LandmarkUtility::convertToQtLandmarkIds(managerUri(), landmarkIds);
    return importedLmIds;
}

/**
 * from MLandmarkDbEventObserver
 */
void LandmarkManagerEngineSymbianPrivate::handleDatabaseEvent(const TPosLmEvent& aEvent)
{
    //qDebug() << "aEvent.iLandmarkItemId = " << aEvent.iLandmarkItemId;
    //qDebug() << "aEvent.iEventType = " << aEvent.iEventType;

    TInt id = aEvent.iLandmarkItemId;
    QString localId;
    localId.append(QString("%1").arg(id));

    switch (aEvent.iEventType) {
    case EPosLmEventLandmarkCreated:
    {
        if (m_CreatedLmIds.contains(localId, Qt::CaseInsensitive)) {

            for (int i = 0; i < m_CreatedLmIds.size(); ++i) {
                if (m_CreatedLmIds.at(i) == localId) {
                    m_CreatedLmIds.removeAt(i);
                    break;
                }
            }
        }
        else {
            QLandmarkId lmid;
            lmid.setLocalId(localId);
            lmid.setManagerUri(managerUri());
            QList<QLandmarkId> lmids;
            lmids << lmid;
            if (id != 0) {
                m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkAdded, lmids);
                qDebug() << "landmark created";
            }
        }

        break;
    }
    case EPosLmEventLandmarkDeleted:
    {
        if (m_DeletedLmIds.contains(localId, Qt::CaseInsensitive)) {

            for (int i = 0; i < m_DeletedLmIds.size(); ++i) {
                if (m_DeletedLmIds.at(i) == localId) {
                    m_DeletedLmIds.removeAt(i);
                    break;
                }
            }
        }
        else {
            QLandmarkId lmid;
            lmid.setLocalId(localId);
            lmid.setManagerUri(managerUri());
            QList<QLandmarkId> lmids;
            lmids << lmid;
            if (id != 0) {
                m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkRemoved, lmids);
                qDebug() << "landmark deleted";
            }
        }
        break;
    }
    case EPosLmEventLandmarkUpdated: // 103
    case EPosLmEventLandmarkUnknownChanges: // 100
    {
        if (m_UpdatedLmIds.contains(localId, Qt::CaseInsensitive)) {

            for (int i = 0; i < m_UpdatedLmIds.size(); ++i) {
                if (m_UpdatedLmIds.at(i) == localId) {
                    m_UpdatedLmIds.removeAt(i);
                    break;
                }
            }
        }
        else {
            QLandmarkId lmid;
            lmid.setLocalId(localId);
            lmid.setManagerUri(managerUri());
            QList<QLandmarkId> lmids;
            lmids << lmid;
            if (id != 0) {
                m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::landmarkUpdated, lmids);
                qDebug() << "landmark updated";
            }
        }
        break;
    }
    case EPosLmEventCategoryCreated:
    {
        if (m_CreatedCatIds.contains(localId, Qt::CaseInsensitive)) {

            for (int i = 0; i < m_CreatedCatIds.size(); ++i) {
                if (m_CreatedCatIds.at(i) == localId) {
                    m_CreatedCatIds.removeAt(i);
                    break;
                }
            }
        }
        else {
            QLandmarkCategoryId catid;
            catid.setLocalId(localId);
            catid.setManagerUri(managerUri());
            QList<QLandmarkCategoryId> catids;
            catids << catid;
            if (id != 0) {
                m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryAdded, QList<
                    QLandmarkId> (), catids);
                qDebug() << "landmark category created";
            }
        }
        break;
    }
    case EPosLmEventCategoryDeleted: // 202
    {
        if (m_DeletedCatIds.contains(localId, Qt::CaseInsensitive)) {

            for (int i = 0; i < m_DeletedCatIds.size(); ++i) {
                if (m_DeletedCatIds.at(i) == localId) {
                    m_DeletedCatIds.removeAt(i);
                    break;
                }
            }
        }
        else {
            QLandmarkCategoryId catid;
            catid.setLocalId(localId);
            catid.setManagerUri(managerUri());
            QList<QLandmarkCategoryId> catids;
            catids << catid;
            if (id != 0) {
                m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryRemoved,
                    QList<QLandmarkId> (), catids);
                qDebug() << "landmark category deleted";
            }
        }
        break;
    }
    case EPosLmEventCategoryUpdated: //203
    case EPosLmEventCategoryUnknownChanges: //200
    {
        if (m_UpdatedCatIds.contains(localId, Qt::CaseInsensitive)) {

            for (int i = 0; i < m_UpdatedCatIds.size(); ++i) {
                if (m_UpdatedCatIds.at(i) == localId) {
                    m_UpdatedCatIds.removeAt(i);
                    break;
                }
            }
        }
        else {
            QLandmarkCategoryId catid;
            catid.setLocalId(localId);
            catid.setManagerUri(managerUri());
            QList<QLandmarkCategoryId> catids;
            catids << catid;
            if (id != 0) {
                m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::categoryUpdated,
                    QList<QLandmarkId> (), catids);
                qDebug() << "landmark category updated";
            }
        }
        break;
    }
        //    case EPosLmEventUnknownChanges: //0
        //    case EPosLmEventNewDefaultDatabaseLocation: //10
        //    case EPosLmEventMediaRemoved: //11
    default:
    {
        qDebug() << "Unknown database event (TPosLmEventType) = " << aEvent.iEventType;
        m_LmEventObserver.handleLandmarkEvent(LandmarkEventObserver::unknownChanges);
    }

    } // switch closure    

}

// end of file
