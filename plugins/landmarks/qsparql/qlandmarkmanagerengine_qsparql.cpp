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

#include "qlandmarkmanagerengine_qsparql_p.h"
#include "qlandmarkfilehandler_gpx_p.h"
#include "qlandmarkfilehandler_lmx_p.h"
#include "databaseoperations_p.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QMutexLocker>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThreadPool>
#include <QTimer>
#include <QUuid>

#include <QtSparql/QSparqlConnection>
#include <QtSparql/QSparqlConnectionOptions>
#include <QtSparql/QSparqlQuery>
#include <QtSparql/QSparqlResult>
#include <QtSparqlTrackerExtensions/TrackerChangeNotifier>

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


QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QLandmarkCategoryId>);
Q_DECLARE_METATYPE(QList<QLandmarkId>);
Q_DECLARE_METATYPE(QList<QLandmark>);
Q_DECLARE_METATYPE(QList<QLandmarkCategory>);
Q_DECLARE_METATYPE(QLandmarkAbstractRequest::State)
Q_DECLARE_METATYPE(QLandmarkAbstractRequest *)
Q_DECLARE_METATYPE(QLandmarkIdFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkFetchByIdRequest *)
Q_DECLARE_METATYPE(QLandmarkManager::Error)
Q_DECLARE_METATYPE(QLandmarkSaveRequest *)
Q_DECLARE_METATYPE(QLandmarkRemoveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategorySaveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryRemoveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryIdFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryFetchByIdRequest *)
Q_DECLARE_METATYPE(QLandmarkImportRequest *)
Q_DECLARE_METATYPE(QLandmarkExportRequest *)
Q_DECLARE_METATYPE(ERROR_MAP)

QLandmarkManagerEngineQsparql::QLandmarkManagerEngineQsparql(const QString &filename, QLandmarkManager::Error * error,
                                                           QString *errorString)
        : m_filename(filename),
        m_dbConnectionName(QUuid::createUuid().toString()),
        m_isCustomAttributesEnabled(false),
        m_databaseOperations()
{
    m_changeNotificationsEnabled = false;
    *error = QLandmarkManager::NoError;
    *errorString ="";

    qRegisterMetaType<ERROR_MAP >();
    qRegisterMetaType<QList<QLandmarkCategoryId> >();
    qRegisterMetaType<QList<QLandmarkId> >();
    qRegisterMetaType<QList<QLandmark> >();
    qRegisterMetaType<QList<QLandmarkCategory> >();
    qRegisterMetaType<QLandmarkAbstractRequest::State>();
    qRegisterMetaType<QLandmarkAbstractRequest *>();
    qRegisterMetaType<QLandmarkIdFetchRequest *>();
    qRegisterMetaType<QLandmarkFetchRequest *>();
    qRegisterMetaType<QLandmarkFetchByIdRequest *>();
    qRegisterMetaType<QLandmarkSaveRequest *>();
    qRegisterMetaType<QLandmarkRemoveRequest *>();
    qRegisterMetaType<QLandmarkCategoryIdFetchRequest *>();
    qRegisterMetaType<QLandmarkCategoryFetchRequest *>();
    qRegisterMetaType<QLandmarkCategoryFetchByIdRequest *>();
    qRegisterMetaType<QLandmarkCategorySaveRequest *>();
    qRegisterMetaType<QLandmarkCategoryRemoveRequest *>();
    qRegisterMetaType<QLandmarkImportRequest *>();
    qRegisterMetaType<QLandmarkExportRequest *>();
    qRegisterMetaType<QLandmarkManager::Error>();

    if (m_filename.isEmpty()) {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                           QLatin1String("Nokia"), QLatin1String("QtLandmarks"));
        QFileInfo fi(settings.fileName());
        QDir dir = fi.dir();
        dir.mkpath(dir.path());
        m_filename = dir.path() + QDir::separator() + QString("QtLandmarks") + QLatin1String(".db");
    }

    if (filename == ":memory:")
        return;

    m_filename = filename;
    m_databaseOperations.managerUri = managerUri();

    QSparqlConnection conn("QTRACKER");
    QSparqlQuery landmarkQuery("select tracker:id(?u) ?u { ?u a slo:Landmark }");
    QSparqlResult* r = conn.exec(landmarkQuery);
    r->waitForFinished();
    if (!r->hasError()) {
        while (r->next()) {
            if (r->value(0).toInt() != 0 && !r->value(1).toString().isEmpty()) {
                m_landmarkHash.insert(r->value(1).toString(), r->value(0).toInt());
            }
        }
    }
    delete r;

    QSparqlQuery categoryQuery("select tracker:id(?u) ?u { ?u a slo:LandmarkCategory }");
    r = conn.exec(categoryQuery);
    r->waitForFinished();
    if (!r->hasError()) {
        while (r->next()) {
            if (r->value(0).toInt() != 0 && !r->value(1).toString().isEmpty()) {
                m_categoryHash.insert(r->value(1).toString(), r->value(0).toInt());
            }
        }
    }
    delete r;

    // Query: what's the current name of the landmark class?
    QString className;
    QSparqlQuery lq("select tracker:uri(tracker:id(slo:Landmark)) "
                   "{}");
    r = conn.exec(lq);
    r->waitForFinished();
    if (r->hasError()) {
        className = QString("slo:Landmark");
    }
    if (r->next()) {
        className = r->value(0).toString();
    }
    else
        className = QString("slo:Landmark");
    delete r;
    m_landmarkNotifier = new TrackerChangeNotifier(className, this);
    connect(m_landmarkNotifier, SIGNAL(changed(QList<TrackerChangeNotifier::Quad>, QList<TrackerChangeNotifier::Quad>)),
        this, SLOT(landmarksNotified(QList<TrackerChangeNotifier::Quad>, QList<TrackerChangeNotifier::Quad>)));

    // Query: what's the current name of the landmarkCategory class?
    QSparqlQuery cq("select tracker:uri(tracker:id(slo:LandmarkCategory)) "
                   "{}");
    r = conn.exec(cq);
    r->waitForFinished();
    if (r->hasError()) {
        className = QString("slo:LandmarkCategory");
    }
    if (r->next()) {
        className = r->value(0).toString();
    }
    else
        className = QString("slo:LandmarkCategory");
    delete r;
    m_categoryNotifier = new TrackerChangeNotifier("className", this);
    connect(m_categoryNotifier, SIGNAL(changed(QList<TrackerChangeNotifier::Quad>, QList<TrackerChangeNotifier::Quad>)),
        this, SLOT(categoriesNotified(QList<TrackerChangeNotifier::Quad>, QList<TrackerChangeNotifier::Quad>)));
}

QLandmarkManagerEngineQsparql::~QLandmarkManagerEngineQsparql()
{
    QThreadPool *threadPool = QThreadPool::globalInstance();
    threadPool->waitForDone();

    if (m_landmarkNotifier != 0)
        delete m_landmarkNotifier;
    if (m_categoryNotifier != 0)
        delete m_categoryNotifier;
}

/* URI reporting */
QString QLandmarkManagerEngineQsparql::managerName() const
{
    return "com.nokia.qt.landmarks.engines.qsparql";
}

QMap<QString, QString> QLandmarkManagerEngineQsparql::managerParameters() const
{
    QMap<QString, QString> parameters;

    parameters["filename"] = m_filename;

    return parameters;
}

int QLandmarkManagerEngineQsparql::managerVersion() const
{
    return 1;
}

QList<QLandmarkId> QLandmarkManagerEngineQsparql::landmarkIds(const QLandmarkFilter& filter,
        int limit, int offset,
        const QList<QLandmarkSortOrder>& sortOrders,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    return m_databaseOperations.landmarkIds(filter,sortOrders,limit,offset, error,errorString);
}

QList<QLandmarkCategoryId> QLandmarkManagerEngineQsparql::categoryIds(int limit, int offset, const QLandmarkNameSort &nameSort,
                                                            QLandmarkManager::Error *error, QString *errorString) const
{
    return m_databaseOperations.categoryIds(nameSort, limit, offset, error, errorString);
}

/* Retrieval */
QLandmark QLandmarkManagerEngineQsparql::landmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    return m_databaseOperations.retrieveLandmark(landmarkId, error, errorString);
}

QList<QLandmark> QLandmarkManagerEngineQsparql::landmarks(const QLandmarkFilter &filter,
                                                         int limit, int offset,
                                                         const QList<QLandmarkSortOrder>& sortOrders,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{

    return m_databaseOperations.landmarks(filter, sortOrders, limit, offset, error, errorString);
}

QList<QLandmark> QLandmarkManagerEngineQsparql::landmarks(const QList<QLandmarkId> &landmarkIds,
                                                         QMap<int, QLandmarkManager::Error> *errorMap,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{

    return m_databaseOperations.landmarks(landmarkIds, errorMap, error, errorString);
}

QLandmarkCategory QLandmarkManagerEngineQsparql::category(const QLandmarkCategoryId &landmarkCategoryId,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{
    return m_databaseOperations.category(landmarkCategoryId, error, errorString);
}

QList<QLandmarkCategory> QLandmarkManagerEngineQsparql::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                                                  QMap<int, QLandmarkManager::Error> *errorMap,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    return m_databaseOperations.categories(landmarkCategoryIds, errorMap, error, errorString);
}

QList<QLandmarkCategory> QLandmarkManagerEngineQsparql::categories(int limit, int offset,
                                                                  const QLandmarkNameSort &nameSort,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    QList<QLandmarkCategoryId> catIds;
    return m_databaseOperations.categories(catIds, nameSort, limit, offset, error, errorString, false);
}

bool QLandmarkManagerEngineQsparql::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return m_databaseOperations.saveLandmark(landmark, error, errorString);
}

bool QLandmarkManagerEngineQsparql::saveLandmarks(QList<QLandmark> * landmarks,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList <QLandmarkId> addedIds;
    QList <QLandmarkId> changedIds;
    return m_databaseOperations.saveLandmarks(landmarks, errorMap, error, errorString, &addedIds, &changedIds);
}

bool QLandmarkManagerEngineQsparql::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return  m_databaseOperations.removeLandmark(landmarkId , error, errorString);
}

bool QLandmarkManagerEngineQsparql::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList <QLandmarkId> removedIds;
    return  m_databaseOperations.removeLandmarks(landmarkIds , errorMap, error, errorString, &removedIds);
}

bool QLandmarkManagerEngineQsparql::saveCategory(QLandmarkCategory* category,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return m_databaseOperations.saveCategory(category, error, errorString);
}

bool QLandmarkManagerEngineQsparql::saveCategories(QList<QLandmarkCategory> * categories,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkCategoryId> addedIds;
    QList<QLandmarkCategoryId> changedIds;
    return m_databaseOperations.saveCategories(categories, errorMap, error, errorString, &addedIds, &changedIds);
}

bool QLandmarkManagerEngineQsparql::removeCategory(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return m_databaseOperations.removeCategory(categoryId, error, errorString);
}

bool QLandmarkManagerEngineQsparql::removeCategories(const QList<QLandmarkCategoryId> &categoryIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkCategoryId> removedIds;
    return  m_databaseOperations.removeCategories(categoryIds , errorMap, error, errorString, &removedIds);
}

bool QLandmarkManagerEngineQsparql::importLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   QLandmarkManager::TransferOption option,
                                                   const QLandmarkCategoryId &categoryId,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString)
{
    QList<QLandmarkId> addedLandmarkIds;
    QList<QLandmarkCategoryId> addedCategoryIds;
    return m_databaseOperations.importLandmarks(device, format, option, categoryId, error, errorString, &addedLandmarkIds, &addedCategoryIds);
}

bool QLandmarkManagerEngineQsparql::exportLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   const QList<QLandmarkId> &landmarkIds,
                                                   QLandmarkManager::TransferOption option,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString) const
{
    return m_databaseOperations.exportLandmarks(device, format, landmarkIds, option, error, errorString);
}

QStringList QLandmarkManagerEngineQsparql::supportedFormats(QLandmarkManager::TransferOperation operation, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_UNUSED(operation);
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";

    QStringList formats;
    formats << QLandmarkManager::Gpx;
    formats << QLandmarkManager::Lmx;
    return formats;
}

QLandmarkManager::SupportLevel QLandmarkManagerEngineQsparql::filterSupportLevel(const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return m_databaseOperations.filterSupportLevel(filter);
}

QLandmarkManager::SupportLevel QLandmarkManagerEngineQsparql::sortOrderSupportLevel(const QLandmarkSortOrder &sortOrder,
                                                            QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return m_databaseOperations.sortOrderSupportLevel(sortOrder);
}

bool QLandmarkManagerEngineQsparql::isFeatureSupported(QLandmarkManager::ManagerFeature feature, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    switch(feature) {
    case (QLandmarkManager::NotificationsFeature):
    case (QLandmarkManager::ImportExportFeature):
        return true;
    default:
        return false;
    }
}

QStringList QLandmarkManagerEngineQsparql::landmarkAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    QStringList commonKeys = DatabaseOperations::coreAttributes +
                             DatabaseOperations::coreGenericAttributes;
    commonKeys.sort();

    return commonKeys;
}

QStringList QLandmarkManagerEngineQsparql::categoryAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    QStringList commonKeys = DatabaseOperations::coreCategoryAttributes +
                             DatabaseOperations::coreGenericCategoryAttributes;
    commonKeys.clear();
    return commonKeys;
}

QStringList QLandmarkManagerEngineQsparql::searchableLandmarkAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";
    QStringList commonKeys = DatabaseOperations::supportedSearchableAttributes;
    commonKeys.sort();
    return commonKeys;
}

bool QLandmarkManagerEngineQsparql::isReadOnly(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return false;
}

bool QLandmarkManagerEngineQsparql::isReadOnly(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_UNUSED(landmarkId);
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return false;
}

bool QLandmarkManagerEngineQsparql::isReadOnly(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return m_databaseOperations.isReadOnly(categoryId, error, errorString);
}

/* Asynchronous Request Support */
void QLandmarkManagerEngineQsparql::requestDestroyed(QLandmarkAbstractRequest* request)
{
    QMutexLocker ml(&m_mutex);
    if (m_requestRunHash.contains(request)) {
        m_requestRunHash.value(request)->isDeleted = true;
        m_requestRunHash.remove(request);
    }
    if (m_activeRequestsRunIdHash.contains(request))
        m_activeRequestsRunIdHash.remove(request);
}

bool QLandmarkManagerEngineQsparql::startRequest(QLandmarkAbstractRequest* request)
{

    QMutexLocker ml(&m_mutex);
    QueryRun *queryRun;
    static unsigned int runId = 0;

    if (m_activeRequestsRunIdHash.contains(request)) {
        //this shouldn't be possible
        qWarning() << "Staring request which is already active";
        return false;
    }

    unsigned int currentRunId = runId;
    runId++;

    m_activeRequestsRunIdHash.insert(request,currentRunId);

    if (!m_requestRunHash.contains(request)) {
        queryRun= new QueryRun(request, managerUri(), this, currentRunId);
        m_requestRunHash.insert(request, queryRun);
        ml.unlock();
    } else {
         ml.unlock();
        //this shouldn't be possible
        qWarning() << "Landmark Request trying to be started twice within engine.";
        return false;
    }

    QThreadPool::globalInstance()->start(queryRun);
    return true;
}

bool QLandmarkManagerEngineQsparql::cancelRequest(QLandmarkAbstractRequest* request)
{
    QMutexLocker ml(&m_mutex);
    if (!m_requestRunHash.contains(request))
        return false;

    m_requestRunHash.value(request)->isCanceled = true;

    return true;
}

bool QLandmarkManagerEngineQsparql::waitForRequestFinished(QLandmarkAbstractRequest* request,
        int msecs)
{
    //Aside: the request at least already be in the active state for this function to
    //to be called.

    QMutexLocker ml(&m_mutex);
    if (!m_requestRunHash.contains(request))
        return false;
    ml.unlock();
    QEventLoop eventLoop;

    if (msecs > 0)
        QTimer::singleShot(msecs, &eventLoop, SLOT(quit()));

    QObject::connect(request, SIGNAL(stateChanged(QLandmarkAbstractRequest::State)),
                     &eventLoop,SLOT(quit()));
    if (request->state() == QLandmarkAbstractRequest::FinishedState)
        return true;
    else
        eventLoop.exec();

    if (request->state() == QLandmarkAbstractRequest::FinishedState)
        return true;

    return false;
}

void QLandmarkManagerEngineQsparql::landmarksNotified(QList<TrackerChangeNotifier::Quad>, QList<TrackerChangeNotifier::Quad>)
{
    if (m_changeNotificationsEnabled) {
        QLandmarkId landmarkId;
        landmarkId.setManagerUri(managerUri());
        QList<QLandmarkId> addedLandmarkIds;
        QList<QLandmarkId> changedLandmarkIds;
        QList<QLandmarkId> removedLandmarkIds;
        QHash<QString, int> landmarkHash;

        QSparqlConnection conn("QTRACKER");
        QSparqlQuery landmarkQuery("select tracker:id(?u) ?u { ?u a slo:Landmark }");
        QSparqlResult* r = conn.exec(landmarkQuery);
        r->waitForFinished();
        if (!r->hasError()) {
            while (r->next()) {
                if (r->value(0).toInt() != 0 && !r->value(1).toString().isEmpty()) {
                    landmarkHash.insert(r->value(1).toString(), r->value(0).toInt());
                }
            }
        }
        delete r;

        foreach(QString localId, m_landmarkHash.keys()) {
            if (!landmarkHash.contains(localId)) {
                landmarkId.setLocalId(localId);
                removedLandmarkIds << landmarkId;
            }
        }
        foreach(QString localId, landmarkHash.keys()) {
            if (!m_landmarkHash.contains(localId)) {
                landmarkId.setLocalId(localId);
                addedLandmarkIds << landmarkId;
            } else {
                if(m_landmarkHash.value(localId) != landmarkHash.value(localId)) {
                    landmarkId.setLocalId(localId);
                    changedLandmarkIds << landmarkId;
                }
            }
        }
        int totalChangeCount = addedLandmarkIds.count() + changedLandmarkIds.count() + removedLandmarkIds.count();

        if (totalChangeCount > 50 ) {
            emit dataChanged();
        } else {
            if (addedLandmarkIds.count() > 0)
                emit landmarksAdded(addedLandmarkIds);
            if (changedLandmarkIds.count() > 0)
                emit landmarksChanged(changedLandmarkIds);
            if (removedLandmarkIds.count() > 0)
                emit landmarksRemoved(removedLandmarkIds);
        }
        m_landmarkHash.clear();
        m_landmarkHash = landmarkHash;
    }
}

void QLandmarkManagerEngineQsparql::categoriesNotified(QList<TrackerChangeNotifier::Quad>, QList<TrackerChangeNotifier::Quad>)
{
    if (m_changeNotificationsEnabled) {
        QLandmarkCategoryId categoryId;
        categoryId.setManagerUri(managerUri());
        QList<QLandmarkCategoryId> addedCategoryIds;
        QList<QLandmarkCategoryId> changedCategoryIds;
        QList<QLandmarkCategoryId> removedCategoryIds;
        QHash<QString, int> categoryHash;

        QSparqlConnection conn("QTRACKER");
        QSparqlQuery landmarkQuery("select tracker:id(?u) ?u { ?u a slo:Landmark }");
        QSparqlResult* r = conn.exec(landmarkQuery);
        r->waitForFinished();
        if (!r->hasError()) {
            while (r->next()) {
                if (r->value(0).toInt() != 0 && !r->value(1).toString().isEmpty()) {
                    categoryHash.insert(r->value(1).toString(), r->value(0).toInt());
                }
            }
        }
        delete r;

        foreach(QString localId, m_categoryHash.keys()) {
            if (!categoryHash.contains(localId)) {
                categoryId.setLocalId(localId);
                removedCategoryIds << categoryId;
            }
        }
        foreach(QString localId, categoryHash.keys()) {
            if (!m_categoryHash.contains(localId)) {
                categoryId.setLocalId(localId);
                addedCategoryIds << categoryId;
            } else {
                if(m_categoryHash.value(localId) != categoryHash.value(localId)) {
                    categoryId.setLocalId(localId);
                    changedCategoryIds << categoryId;
                }
            }
        }
        int totalChangeCount = addedCategoryIds.count() + changedCategoryIds.count() + removedCategoryIds.count();

        if (totalChangeCount > 50 ) {
            emit dataChanged();
        } else {
            if (addedCategoryIds.count() > 0)
                emit categoriesAdded(addedCategoryIds);
            if (changedCategoryIds.count() > 0)
                emit categoriesChanged(changedCategoryIds);
            if (removedCategoryIds.count() > 0)
                emit categoriesRemoved(removedCategoryIds);
        }
        m_categoryHash.clear();
        m_categoryHash = categoryHash;
    }
}

void QLandmarkManagerEngineQsparql::setChangeNotificationsEnabled(bool enabled)
{
    m_changeNotificationsEnabled = enabled;
}

void QLandmarkManagerEngineQsparql::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(landmarksAdded(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksChanged(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksRemoved(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)))
        {
            setChangeNotificationsEnabled(true);
        }
    QObject::connectNotify(signal);
}
void QLandmarkManagerEngineQsparql::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(landmarksAdded(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksChanged(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksRemoved(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>))) {
        if (receivers(SIGNAL(landmarksAdded(QList<QLandmarkId>))) == 0
            && receivers(SIGNAL(landmarksChanged(QList<QLandmarkId>))) == 0
            && receivers(SIGNAL(landmarksRemoved(QList<QLandmarkId>))) == 0
            && receivers(SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))) == 0
            && receivers(SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))) == 0
            && receivers(SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>))) == 0
            )
            setChangeNotificationsEnabled(false);
    }
    QObject::disconnectNotify(signal);
}

void QLandmarkManagerEngineQsparql::updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req, const QList<QLandmarkId>& result,
                                  QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState,
                                  unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkIdFetchRequest(req, result, error, errorString, newState);
    }

}

void QLandmarkManagerEngineQsparql::updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState,
                            unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkFetchRequest(req, result, error, errorString,newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkFetchByIdRequest(QLandmarkFetchByIdRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState,
                            unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkFetchByIdRequest(req, result, error, errorString, errorMap, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState,
                            unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkSaveRequest(req, result, error, errorString, errorMap, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error,
                             const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState,
                             unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkRemoveRequest(req, error, errorString, errorMap, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state, unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (state == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateRequestState(req,state);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest* req, const QList<QLandmarkCategoryId>& result,
        QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState, unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkCategoryIdFetchRequest(req, result, error, errorString, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest* req, const QList<QLandmarkCategory>& result,
        QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState, unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkCategoryFetchRequest(req, result, error, errorString, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkCategoryFetchByIdRequest(QLandmarkCategoryFetchByIdRequest* req, const QList<QLandmarkCategory>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState,
                            unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkCategoryFetchByIdRequest(req, result, error, errorString, errorMap, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState,
                            unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkCategorySaveRequest(req, result, error, errorString, errorMap, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest* req,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState,
                            unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkCategoryRemoveRequest(req, error, errorString, errorMap, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkImportRequest(QLandmarkImportRequest *req, const QList<QLandmarkId> &ids,
                                 QLandmarkManager::Error error, const QString &errorString,
                                 QLandmarkAbstractRequest::State newState, unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkImportRequest(req, ids, error, errorString, newState);
    }
}

void QLandmarkManagerEngineQsparql::updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                 QLandmarkAbstractRequest::State newState, unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (newState == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateLandmarkExportRequest(req, error, errorString, newState);
    }
}
