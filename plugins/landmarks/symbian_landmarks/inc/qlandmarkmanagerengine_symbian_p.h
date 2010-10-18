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

#ifndef QLANDMARKMANAGERENGINE_SYMBIAN_P_H
#define QLANDMARKMANAGERENGINE_SYMBIAN_P_H

//
// QT LANDMARK MANAGER ENGINE PRIVATE IMPLEMENTATION
// -----------------------------------------
//
// This file exists private implementation which having actual symbian apis 
// usage details for QLandmarkManagerEngine class.  
// This header file may change from version to version without notice, or 
// even be removed.
//
// We mean it.
//
//system includes
#include <qglobal.h>
#include <qlandmarkmanagerengine.h>
#include <EPos_CPosLmSearchCriteria.h> 
#include <EPos_CPosLmAreaCriteria.h> 
#include <EPos_CPosLmCategoryCriteria.h> 
// user includes
#include "qlandmarkdbeventhandler.h"
#include "qlandmarkrequesthandler.h"

class CPosLandmarkDatabase;
class CPosLmCategoryManager;
class CPosLandmark;
class CPosLandmarkCategory;
class CPosLandmarkSearch;
class LandmarkEventObserver;

QTM_USE_NAMESPACE

/**
 * The class defines the interface for receiving position
 * technology change event.The class interested in position
 * technology change should inherit from this class.
 */

class LandmarkManagerEngineSymbianPrivate: public MLandmarkRequestObserver,
    public MLandmarkDbEventObserver
{
public:
    LandmarkManagerEngineSymbianPrivate(LandmarkEventObserver& lmOpObserver,
        const QString &filename);
    ~LandmarkManagerEngineSymbianPrivate();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QString managerUri() const;

    /* Filtering */
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter &filter, int limit, int offset,
        const QList<QLandmarkSortOrder>& sortOrders, QLandmarkManager::Error *error,
        QString *errorString) const;

    QList<QLandmarkCategoryId>
    categoryIds(int limit, int offset, const QLandmarkNameSort &nameSort,
        QLandmarkManager::Error *error, QString *errorString) const;

    /* Retrieval */
    QLandmark landmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
        QString *errorString) const;

    QList<QLandmark>
    landmarks(const QList<QLandmarkId> &landmarkIds, QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error, QString *errorString) const;

    QList<QLandmark> landmarks(const QLandmarkFilter &filter, int limit, int offset, const QList<
        QLandmarkSortOrder>& sortOrders, QLandmarkManager::Error *error, QString *errorString) const;

    QLandmarkCategory category(const QLandmarkCategoryId &landmarkCategoryId,
        QLandmarkManager::Error *error, QString *errorString) const;

    QList<QLandmarkCategory> categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
        QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
        QString *errorString) const;

    QList<QLandmarkCategory> categories(int limit, int offset, const QLandmarkNameSort &nameSort,
        QLandmarkManager::Error *error, QString *errorString) const;

    /*saving and removing*/
    bool saveLandmark(QLandmark* landmark, QLandmarkManager::Error *error, QString *errorString);
    bool saveLandmarks(QList<QLandmark> * landmark, QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error, QString *errorString);
    bool removeLandmark(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
        QString *errorString);
    bool removeLandmarks(const QList<QLandmarkId> &landmarkId,
        QMap<int, QLandmarkManager::Error> *errorMap, QLandmarkManager::Error *error,
        QString *errorString);

    bool saveCategory(QLandmarkCategory* category, QLandmarkManager::Error *error,
        QString *errorString);
    bool removeCategory(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error,
        QString *errorString);

    bool importLandmarks(QIODevice *device, const QString& format,
        QLandmarkManager::TransferOption option, const QLandmarkCategoryId& id,
        QLandmarkManager::Error *error, QString *errorString);
    bool exportLandmarks(QIODevice *device, const QString& format, const QList<QLandmarkId> &landmarkIds,
        QLandmarkManager::TransferOption option, QLandmarkManager::Error *error,
        QString *errorString) const;
    QStringList supportedFormats(QLandmarkManager::TransferOperation operation,
        QLandmarkManager::Error *error, QString *errorString) const;

    QLandmarkManager::SupportLevel filterSupportLevel(const QLandmarkFilter &filter,
        QLandmarkManager::Error *error, QString *errorString) const;
    QLandmarkManager::SupportLevel sortOrderSupportLevel(
        const QLandmarkSortOrder &sortOrder, QLandmarkManager::Error *error,
        QString *errorString) const;
    bool isFeatureSupported(QLandmarkManager::ManagerFeature feature,
         QLandmarkManager::Error *error, QString *errorString) const;

    bool isReadOnly(QLandmarkManager::Error *error, QString *errorString) const;
    bool isReadOnly(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
        QString *errorString) const;
    bool isReadOnly(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error,
        QString *errorString) const;

    /* Asynchronous Request Support */
    void requestDestroyed(QLandmarkAbstractRequest* request);
    bool startRequest(QLandmarkAbstractRequest* request);
    bool cancelRequest(QLandmarkAbstractRequest* request);
    bool waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs);

    /* From MLandmarkRequestObserver */
    void HandleExecutionL(CLandmarkRequestData*, TRequestStatus&);
    void HandleCompletionL(CLandmarkRequestData*);

private:
    bool saveLandmarkInternalL(QLandmark* landmark, QLandmarkManager::Error *error,
        QString *errorString, bool *added, bool *changed);
    bool removeLandmarkInternalL(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
        QString *errorString, bool *removed);

    QLandmark fetchLandmarkL(const QLandmarkId &landmarkId, QLandmarkManager::Error *error,
        QString *errorString) const;

    bool saveCategoryInternalL(QLandmarkCategory* category, QLandmarkManager::Error *error,
        QString *errorString, bool *added, bool *changed);
    bool removeCategoryInternalL(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error, QString *errorString, bool *removed);

    QLandmarkCategory fetchCategoryL(const QLandmarkCategoryId &landmarkCategoryId,
        QLandmarkManager::Error *error, QString *errorString) const;

    QList<QLandmarkCategoryId> fetchCategoryIdsL(const QLandmarkNameSort &nameSort,
        QLandmarkManager::Error *error, QString *errorString) const;

    bool startRequestL(QLandmarkAbstractRequest* request);

    // methods for searching
    CPosLmSearchCriteria* getSearchCriteriaL(QLandmarkFilter filter) const;

    CPosLmOperation* getOperationL(CPosLandmarkSearch* landmarkSearch,
        CPosLmSearchCriteria* searchCriteria, QLandmarkSortOrder qtSortOrder, int maxMatches) const;

    QList<QLandmarkId> searchWithFilterL(QLandmarkFilter filter, QLandmarkSortOrder qtSortOrder,
        int maxMatches) const;

    bool sortFetchedLmIds(int limit, int offset, QList<QLandmarkSortOrder> sortOrders, QList<
        QLandmarkId>& landmarkIds, QLandmarkFilter::FilterType filterType,
        QLandmarkManager::Error *error, QString *errorString) const;

    // to handle symbian errors to assign appropriate manager error and error description.
    void
    handleSymbianError(TInt errorId, QLandmarkManager::Error *error, QString *errorString) const;

    // import export internal impl
    void exportLandmarksL(QIODevice *device, const QString &format, const QList<QLandmarkId> &landmarkIds,
        QLandmarkManager::TransferOption option) const;
    QList<QLandmarkId> importLandmarksL(QIODevice *device, const QString &format,
        QLandmarkManager::TransferOption option, const QLandmarkCategoryId& categoryId) const;

private:
    void handleDatabaseEvent(const TPosLmEvent& aEvent);

public:
    static CLandmarkDbEventHandler m_DbEventHandler;
    static LandmarkRequestHandler m_RequestHandler;

private:
    QString m_dbFilename;

    CPosLandmarkDatabase* m_LandmarkDb;
    CPosLmCategoryManager* m_LandmarkCatMgr;
    LandmarkEventObserver& m_LmEventObserver;

    QStringList m_CreatedLmIds;
    QStringList m_UpdatedLmIds;
    QStringList m_DeletedLmIds;

    QStringList m_CreatedCatIds;
    QStringList m_UpdatedCatIds;
    QStringList m_DeletedCatIds;

    QStringList m_UnknownIds;

private:
    friend class QLandmarkDbEventObserver;
};

#endif // QLANDMARKMANAGERENGINE_SYMBIAN_P_H
// end of file
