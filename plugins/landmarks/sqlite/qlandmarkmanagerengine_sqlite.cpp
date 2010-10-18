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

#include "qlandmarkmanagerengine_sqlite_p.h"
#include "qlandmarkfilehandler_gpx_p.h"
#include "qlandmarkfilehandler_lmx_p.h"
#include "databaseoperations_p.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QThreadPool>
#include <QUuid>

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

QLandmarkManagerEngineSqlite::QLandmarkManagerEngineSqlite(const QString &filename, QLandmarkManager::Error * error,
                                                           QString *errorString)
        : m_dbFilename(filename),
        m_dbConnectionName(QUuid::createUuid().toString()),
        m_dbWatcher(NULL),
        m_latestLandmarkTimestamp(0),
        m_latestCategoryTimestamp(0),
        m_isCustomAttributesEnabled(false),
        m_databaseOperations()
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

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

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_dbConnectionName);
    m_databaseOperations.connectionName = m_dbConnectionName;

    if (m_dbFilename.isEmpty()) {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                           QLatin1String("Nokia"), QLatin1String("QtLandmarks"));
        QFileInfo fi(settings.fileName());
        QDir dir = fi.dir();
        dir.mkpath(dir.path());
        m_dbFilename = dir.path() + QDir::separator() + QString("QtLandmarks") +  QLatin1String(".db");

    }

    db.setDatabaseName(m_dbFilename);
    if (!db.open()) {
        qWarning() << db.lastError().text();
    }

    if (filename == ":memory:")
        return;

    // check for fk support
    QSqlQuery checkForeignKeys("PRAGMA foreign_keys;", db);
    bool result = false;
    while (checkForeignKeys.next()) {
        QString r = checkForeignKeys.value(0).toString();
        if ((r == "0") || (r == "1"))
            result = true;
    }

    QFile file;
    if (result)
        file.setFileName(":/qlandmarkmanagerengine_sqlite_fk.ddl");
    else
        file.setFileName(":/qlandmarkmanagerengine_sqlite_no_fk.ddl");

    // read in ddl to set up tables
    // all tables and indices use IF NOT EXISTS to avoid duplication
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString s = in.readAll();
        QStringList queries = s.split("@@@");

        bool transacting = db.transaction();

        {//check for database with old schema
            QSqlQuery query(db);
            query.exec("SELECT name from sqlite_master WHERE name = 'landmark'");
            if (query.next()) {
                query.exec("SELECT name from sqlite_master WHERE name = 'version'");
                if (!query.next()) {
                    *error = QLandmarkManager::VersionMismatchError;
                    *errorString = QString("Old landmarks database with incompatible schema detected, please delete this file and try again:") +
                                   this->m_dbFilename;
                    qWarning() << *errorString;
                    db.rollback();
                    return;
                }
            }
        }
        for (int i = 0; i < queries.size(); ++i) {
            QString q = queries.at(i).trimmed();
            if (q == "")
                continue;

            q += ";";
            QSqlQuery query(db);
            if (!query.exec(q)) {
                qWarning() << QString("Statement %1: %2").arg(i + 1).arg(query.lastError().databaseText());
            }
        }

        {
            QSqlQuery query(db);
            query.exec("SELECT verionNumber FROM version");
            if (query.next()) {
                int versionNumber = query.value(0).toInt();
                if (versionNumber != 1) {
                    *error =  QLandmarkManager::VersionMismatchError;
                    *errorString = "Sqlite landmark plugin only operates with version 1 of QtLandmarks.db";
                    db.rollback();
                    return;
                }
            } else {
                query.finish();
                query.clear();
                query.exec("INSERT INTO versionNumber VALUES(1)");
            }
        }
        if (transacting)
            db.commit();
    }
    file.close();
    m_databaseOperations.managerUri = managerUri();
}

QLandmarkManagerEngineSqlite::~QLandmarkManagerEngineSqlite()
{
    QThreadPool *threadPool = QThreadPool::globalInstance();
    threadPool->waitForDone();

    if (m_dbWatcher !=0)
        delete m_dbWatcher;

    QSqlDatabase::database(m_dbConnectionName).close();
    QSqlDatabase::removeDatabase(m_dbConnectionName);
}

/* URI reporting */
QString QLandmarkManagerEngineSqlite::managerName() const
{
    return "com.nokia.qt.landmarks.engines.sqlite";
}

QMap<QString, QString> QLandmarkManagerEngineSqlite::managerParameters() const
{
    QMap<QString, QString> parameters;

    parameters["filename"] = m_dbFilename;

    return parameters;
}

int QLandmarkManagerEngineSqlite::managerVersion() const
{
    return 1;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIds(const QLandmarkFilter& filter,
        int limit, int offset,
        const QList<QLandmarkSortOrder>& sortOrders,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    return m_databaseOperations.landmarkIds(filter,sortOrders,limit,offset, error,errorString);
}

QList<QLandmarkCategoryId> QLandmarkManagerEngineSqlite::categoryIds(int limit, int offset, const QLandmarkNameSort &nameSort,
                                                            QLandmarkManager::Error *error, QString *errorString) const
{
    return m_databaseOperations.categoryIds(nameSort, limit, offset, error, errorString);
}

/* Retrieval */
QLandmark QLandmarkManagerEngineSqlite::landmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    return m_databaseOperations.retrieveLandmark(landmarkId, error, errorString);
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QLandmarkFilter &filter,
                                                         int limit, int offset,
                                                         const QList<QLandmarkSortOrder>& sortOrders,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{

    return m_databaseOperations.landmarks(filter, sortOrders, limit, offset, error, errorString);
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QList<QLandmarkId> &landmarkIds,
                                                         QMap<int, QLandmarkManager::Error> *errorMap,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{

    return m_databaseOperations.landmarks(landmarkIds, errorMap, error, errorString);
}

QLandmarkCategory QLandmarkManagerEngineSqlite::category(const QLandmarkCategoryId &landmarkCategoryId,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{
    return m_databaseOperations.category(landmarkCategoryId, error, errorString);
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                                                  QMap<int, QLandmarkManager::Error> *errorMap,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    return m_databaseOperations.categories(landmarkCategoryIds, errorMap, error, errorString);
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories(int limit, int offset,
                                                                  const QLandmarkNameSort &nameSort,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    QList<QLandmarkCategoryId> catIds;
    return m_databaseOperations.categories(catIds, nameSort, limit, offset, error, errorString, false);
}


bool QLandmarkManagerEngineSqlite::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString)
{

    return m_databaseOperations.saveLandmark(landmark, error, errorString);

}

bool QLandmarkManagerEngineSqlite::saveLandmarks(QList<QLandmark> * landmarks,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return m_databaseOperations.saveLandmarks(landmarks, errorMap, error, errorString);
}

bool QLandmarkManagerEngineSqlite::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return  m_databaseOperations.removeLandmark(landmarkId , error, errorString);
}

bool QLandmarkManagerEngineSqlite::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return  m_databaseOperations.removeLandmarks(landmarkIds , errorMap, error, errorString);
}

bool QLandmarkManagerEngineSqlite::saveCategory(QLandmarkCategory* category,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return m_databaseOperations.saveCategory(category, error, errorString);
}

bool QLandmarkManagerEngineSqlite::removeCategory(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return m_databaseOperations.removeCategory(categoryId, error, errorString);
}

bool QLandmarkManagerEngineSqlite::importLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   QLandmarkManager::TransferOption option,
                                                   const QLandmarkCategoryId &categoryId,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString)
{
    return m_databaseOperations.importLandmarks(device, format, option, categoryId, error, errorString);
}

bool QLandmarkManagerEngineSqlite::exportLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   QList<QLandmarkId> landmarkIds,
                                                   QLandmarkManager::TransferOption option,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString) const
{
    return m_databaseOperations.exportLandmarks(device, format, landmarkIds, option, error, errorString);
}

QStringList QLandmarkManagerEngineSqlite::supportedFormats(QLandmarkManager::TransferOperation operation, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";

    QStringList formats;
    formats << QLandmarkManager::Gpx;
    formats << QLandmarkManager::Lmx;
    return formats;
}

QLandmarkManager::SupportLevel QLandmarkManagerEngineSqlite::filterSupportLevel(const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return m_databaseOperations.filterSupportLevel(filter);
}

QLandmarkManager::SupportLevel QLandmarkManagerEngineSqlite::sortOrderSupportLevel(const QList<QLandmarkSortOrder> &sortOrders,
                                                            QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return m_databaseOperations.sortOrderSupportLevel(sortOrders);
}

bool QLandmarkManagerEngineSqlite::isFeatureSupported(QLandmarkManager::LandmarkFeature feature, QLandmarkManager::Error *error, QString *errorString) const
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

QStringList QLandmarkManagerEngineSqlite::searchableLandmarkAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";
    //TODO: optimize
    QStringList commonKeys = DatabaseOperations::supportedSearchableAttributes;
    commonKeys.sort();
    return commonKeys;
}

bool QLandmarkManagerEngineSqlite::isReadOnly(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return false;
}

bool QLandmarkManagerEngineSqlite::isReadOnly(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return false;
}

bool QLandmarkManagerEngineSqlite::isReadOnly(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return false;
}

/* Asynchronous Request Support */
void QLandmarkManagerEngineSqlite::requestDestroyed(QLandmarkAbstractRequest* request)
{
    QMutexLocker ml(&m_mutex);
    if (m_requestRunHash.contains(request)) {
        m_requestRunHash.value(request)->isDeleted = true;
        m_requestRunHash.remove(request);
    }
    if (m_activeRequestsRunIdHash.contains(request))
        m_activeRequestsRunIdHash.remove(request);
}

bool QLandmarkManagerEngineSqlite::startRequest(QLandmarkAbstractRequest* request)
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

bool QLandmarkManagerEngineSqlite::cancelRequest(QLandmarkAbstractRequest* request)
{
    QMutexLocker ml(&m_mutex);
    if (!m_requestRunHash.contains(request))
        return false;

    m_requestRunHash.value(request)->isCanceled = true;

    return true;
}

bool QLandmarkManagerEngineSqlite::waitForRequestFinished(QLandmarkAbstractRequest* request,
        int msecs)
{
    return false;
}

void QLandmarkManagerEngineSqlite::databaseChanged()
{
    QSqlDatabase db = QSqlDatabase::database(m_dbConnectionName);

    QSqlQuery query(db);
    if (!query.prepare("SELECT landmarkId,action, timestamp FROM landmark_notification WHERE timestamp >= ?")) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not prepare statement: " << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    query.addBindValue(m_latestLandmarkTimestamp);
    if (!query.exec()) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not execute statement:" << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }

    QList<QLandmarkId> addedLandmarkIds;
    QList<QLandmarkId> changedLandmarkIds;
    QList<QLandmarkId> removedLandmarkIds;

    QString action;
    QLandmarkId landmarkId;
    landmarkId.setManagerUri(managerUri());
    bool ok;
    qint64 timestamp;

    while(query.next()) {
        timestamp = query.value(2).toLongLong(&ok);
        if (!ok) //this should never happen
            continue;

        if (timestamp > m_latestLandmarkTimestamp)
            m_latestLandmarkTimestamp = timestamp;

        action = query.value(1).toString();
        landmarkId.setLocalId((query.value(0).toString()));

        if (action == "ADD") {
            addedLandmarkIds << landmarkId;
        } else if (action == "CHANGE") {
            changedLandmarkIds << landmarkId;
        } else if (action == "REMOVE") {
            removedLandmarkIds << landmarkId;
        }
    }

    if (addedLandmarkIds.count() > 0)
        emit landmarksAdded(addedLandmarkIds);

    if (changedLandmarkIds.count() > 0)
        emit landmarksChanged(changedLandmarkIds);

    if (removedLandmarkIds.count() > 0)
        emit landmarksRemoved(removedLandmarkIds);

    //now check for added/modified/removed categories
    if (!query.prepare("SELECT categoryId,action, timestamp FROM category_notification WHERE timestamp >= ?")) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not prepare statement: " << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    query.addBindValue(m_latestCategoryTimestamp);
    if (!query.exec()) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not execute statement:" << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    QList<QLandmarkCategoryId> addedCategoryIds;
    QList<QLandmarkCategoryId> changedCategoryIds;
    QList<QLandmarkCategoryId> removedCategoryIds;

    QLandmarkCategoryId categoryId;
    categoryId.setManagerUri(managerUri());

    while(query.next()) {
        timestamp = query.value(2).toLongLong(&ok);
        if (!ok) //this should never happen
            continue;

        if (timestamp > m_latestCategoryTimestamp)
            m_latestCategoryTimestamp = timestamp;

        action = query.value(1).toString();
        categoryId.setLocalId(query.value(0).toString());

        if (action == "ADD") {
            addedCategoryIds << categoryId;
        } else if (action == "CHANGE") {
            changedCategoryIds << categoryId;
        } else if (action == "REMOVE") {
            removedCategoryIds << categoryId;
        }
    }

    if (addedCategoryIds.count() > 0)
        emit categoriesAdded(addedCategoryIds);

    if (changedCategoryIds.count() > 0)
        emit categoriesChanged(changedCategoryIds);

    if (removedCategoryIds.count() > 0)
        emit categoriesRemoved(removedCategoryIds);

    m_latestLandmarkTimestamp +=1;
    m_latestCategoryTimestamp +=1;
}

void QLandmarkManagerEngineSqlite::setChangeNotificationsEnabled(bool enabled)
{
    if (!m_dbWatcher) {
        m_dbWatcher = new DatabaseFileWatcher(m_dbFilename);
        connect(m_dbWatcher, SIGNAL(notifyChange()),this,SLOT(databaseChanged()));
    }
    m_dbWatcher->setEnabled(enabled);
    if (enabled) {
        QDateTime dateTime= QDateTime::currentDateTime();
        m_latestLandmarkTimestamp = (qint64)dateTime.toTime_t() *1000 + dateTime.time().msec();
        m_latestCategoryTimestamp = (qint64)dateTime.toTime_t() *1000 + dateTime.time().msec();
    }
}

void QLandmarkManagerEngineSqlite::connectNotify(const char *signal)
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
}
void QLandmarkManagerEngineSqlite::disconnectNotify(const char *signal)
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
}

void QLandmarkManagerEngineSqlite::updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req, const QList<QLandmarkId>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkFetchByIdRequest(QLandmarkFetchByIdRequest* req, const QList<QLandmark>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error,
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

void QLandmarkManagerEngineSqlite::updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state, unsigned int runId)
{
    QMutexLocker ml(&m_mutex);
    if (m_activeRequestsRunIdHash.contains(req) && m_activeRequestsRunIdHash.value(req) == runId) {
        if (state == QLandmarkAbstractRequest::FinishedState)
            m_activeRequestsRunIdHash.remove(req);

        ml.unlock();
        QLandmarkManagerEngine::updateRequestState(req,state);
    }
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest* req, const QList<QLandmarkCategoryId>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest* req, const QList<QLandmarkCategory>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryFetchByIdRequest(QLandmarkCategoryFetchByIdRequest* req, const QList<QLandmarkCategory>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory>& result,
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

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest* req,
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

void QLandmarkManagerEngineSqlite::updateLandmarkImportRequest(QLandmarkImportRequest *req, const QList<QLandmarkId> &ids,
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

void QLandmarkManagerEngineSqlite::updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
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
