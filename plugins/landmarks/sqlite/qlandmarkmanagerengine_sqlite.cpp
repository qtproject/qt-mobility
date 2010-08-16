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

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <QThreadPool>
#include <QUuid>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include "databaseoperations_p.h"

using namespace DatabaseOperations;

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

QLandmarkManagerEngineSqlite::QLandmarkManagerEngineSqlite(const QString &filename)
        : m_dbFilename(filename),
        m_dbConnectionName(QUuid::createUuid().toString()),
        m_dbWatcher(NULL),
        m_latestTimestamp(0.0)
{
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
        if (transacting)
            db.commit();
    }
    file.close();
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
    return DatabaseOperations::landmarkIds(m_dbConnectionName,filter,sortOrders,limit,offset, error,errorString, managerUri() );
}

QList<QLandmarkCategoryId> QLandmarkManagerEngineSqlite::categoryIds(int limit, int offset, const QLandmarkNameSort &nameSort,
                                                            QLandmarkManager::Error *error, QString *errorString) const
{
    return DatabaseOperations::categoryIds(m_dbConnectionName, nameSort, limit, offset, error, errorString, managerUri());
}

/* Retrieval */
QLandmark QLandmarkManagerEngineSqlite::landmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    return DatabaseOperations::retrieveLandmark(m_dbConnectionName, landmarkId, error, errorString, managerUri());
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QLandmarkFilter &filter,
                                                         int limit, int offset,
                                                         const QList<QLandmarkSortOrder>& sortOrders,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{

    return DatabaseOperations::landmarks(m_dbConnectionName, filter, sortOrders, limit, offset, error, errorString, managerUri());
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QList<QLandmarkId> &landmarkIds,
                                                         QMap<int, QLandmarkManager::Error> *errorMap,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{

    return DatabaseOperations::landmarks(m_dbConnectionName, landmarkIds, errorMap, error, errorString, managerUri());
}

QLandmarkCategory QLandmarkManagerEngineSqlite::category(const QLandmarkCategoryId &landmarkCategoryId,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{
    return DatabaseOperations::category(m_dbConnectionName, landmarkCategoryId, error, errorString, managerUri());
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                                                  QMap<int, QLandmarkManager::Error> *errorMap,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    return DatabaseOperations::categories(m_dbConnectionName, landmarkCategoryIds, errorMap, error, errorString, managerUri());
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories(int limit, int offset,
                                                                  const QLandmarkNameSort &nameSort,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    QList<QLandmarkCategoryId> catIds;
    return DatabaseOperations::categories(m_dbConnectionName, catIds, nameSort, limit, offset, error, errorString, managerUri(), false);
}


bool QLandmarkManagerEngineSqlite::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString)
{

    return DatabaseOperations::saveLandmark(m_dbConnectionName, landmark, error, errorString, managerUri());

}

bool QLandmarkManagerEngineSqlite::saveLandmarks(QList<QLandmark> * landmarks,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return DatabaseOperations::saveLandmarks(m_dbConnectionName, landmarks, errorMap, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return  DatabaseOperations::removeLandmark(m_dbConnectionName, landmarkId , error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return  DatabaseOperations::removeLandmarks(m_dbConnectionName, landmarkIds , errorMap, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::saveCategory(QLandmarkCategory* category,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return DatabaseOperations::saveCategory(m_dbConnectionName, category, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::removeCategory(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return DatabaseOperations::removeCategory(m_dbConnectionName, categoryId, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::importLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   QLandmarkManager::TransferOption option,
                                                   const QLandmarkCategoryId &categoryId,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString)
{
    return DatabaseOperations::importLandmarks(m_dbConnectionName, device, format, option, categoryId, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::exportLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   QList<QLandmarkId> landmarkIds,
                                                   QLandmarkManager::TransferOption option,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString) const
{
    return DatabaseOperations::exportLandmarks(m_dbConnectionName, device, format, landmarkIds, option, error, errorString, managerUri());
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

    return DatabaseOperations::filterSupportLevel(filter);
}

QLandmarkManager::SupportLevel QLandmarkManagerEngineSqlite::sortOrderSupportLevel(const QList<QLandmarkSortOrder>&, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return QLandmarkManager::Native;
}

bool QLandmarkManagerEngineSqlite::isFeatureSupported(QLandmarkManager::LandmarkFeature feature, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    switch(feature) {
        case (QLandmarkManager::ExtendedAttributes):
        case (QLandmarkManager::CustomAttributes):
            return true;
            break;
        default:
            return false;
    }
}

QStringList QLandmarkManagerEngineSqlite::landmarkAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return QLandmarkManagerEngine::landmarkAttributeKeys(error, errorString);
}

QStringList QLandmarkManagerEngineSqlite::categoryAttributeKeys(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return QLandmarkManagerEngine::categoryAttributeKeys(error, errorString);
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

bool QLandmarkManagerEngineSqlite::isExtendedAttributesEnabled(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return m_isExtendedAttributesEnabled;
}

void QLandmarkManagerEngineSqlite::setExtendedAttributesEnabled(bool enabled, QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    m_isExtendedAttributesEnabled = enabled;
}

bool QLandmarkManagerEngineSqlite::isCustomAttributesEnabled(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return m_isCustomAttributesEnabled;
}

void QLandmarkManagerEngineSqlite::setCustomAttributesEnabled(bool enabled, QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    m_isCustomAttributesEnabled = enabled;
}

/* Asynchronous Request Support */
void QLandmarkManagerEngineSqlite::requestDestroyed(QLandmarkAbstractRequest* request)
{
    QueryRun *queryRun;
    queryRun = m_requestRunHash.value(request);
    m_requestRunHash.remove(request);
    delete queryRun;
    queryRun = 0;
}

bool QLandmarkManagerEngineSqlite::startRequest(QLandmarkAbstractRequest* request)
{
    QueryRun *queryRun;
    if (!m_requestRunHash.contains(request)) {
        queryRun= new QueryRun(request, managerUri(), this);
        queryRun->setAutoDelete(false);
        m_requestRunHash.insert(request, queryRun);
    } else {
        queryRun = m_requestRunHash.value(request);
    }

    QThreadPool::globalInstance()->start(queryRun);
    return true;
}

bool QLandmarkManagerEngineSqlite::cancelRequest(QLandmarkAbstractRequest* request)
{
    m_requestRunHash.value(request)->isCanceled = true;

    if (request->type() == QLandmarkAbstractRequest::ImportRequest) {
        if (m_requestRunHash.value(request)->gpxHandler) {
            m_requestRunHash.value(request)->gpxHandler->cancel();
            QMetaObject::invokeMethod(m_requestRunHash.value(request)->gpxHandler,
                                "cancel", Qt::QueuedConnection);
        }
    }
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

    qreal latestLandmarkTimestamp = m_latestTimestamp;
    QSqlQuery query(db);
    if (!query.prepare("SELECT landmarkId,action, timestamp FROM landmark_notification WHERE timestamp > ?")) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not prepare statement: " << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    query.addBindValue(latestLandmarkTimestamp);
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
    qreal timestamp;

    while(query.next()) {
        timestamp = query.value(2).toDouble(&ok);
        if (!ok) //this should never happen
            continue;

        if (timestamp > latestLandmarkTimestamp)
            latestLandmarkTimestamp = timestamp;

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
    qreal latestCategoryTimestamp = m_latestTimestamp;
    if (!query.prepare("SELECT categoryId,action, timestamp FROM category_notification WHERE timestamp > ?")) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not prepare statement: " << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    query.addBindValue(latestCategoryTimestamp);
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
        timestamp = query.value(2).toDouble(&ok);
        if (!ok) //this should never happen
            continue;

        if (timestamp > latestCategoryTimestamp)
            latestCategoryTimestamp = timestamp;

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

    if (addedCategoryIds.count() > 0) {
        emit categoriesAdded(addedCategoryIds);
     }

    if (changedCategoryIds.count() > 0)
        emit categoriesChanged(changedCategoryIds);

    if (removedCategoryIds.count() > 0) {
        emit categoriesRemoved(removedCategoryIds);
       }

    if (latestLandmarkTimestamp > m_latestTimestamp)
        m_latestTimestamp = latestLandmarkTimestamp;
    if (latestCategoryTimestamp > m_latestTimestamp)
        m_latestTimestamp = latestCategoryTimestamp;
}

void QLandmarkManagerEngineSqlite::setChangeNotificationsEnabled(bool enabled)
{
    if (!m_dbWatcher) {
        m_dbWatcher = new DatabaseFileWatcher(m_dbFilename);
        connect(m_dbWatcher, SIGNAL(notifyChange()),this,SLOT(databaseChanged()));
    }
    m_dbWatcher->setEnabled(enabled);
    if (enabled)
        m_latestTimestamp = QDateTime::currentDateTime().toTime_t();
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
                                  QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkIdFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkFetchRequest(req, result, error, errorString,newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkFetchByIdRequest(QLandmarkFetchByIdRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkFetchByIdRequest(req, result, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkSaveRequest(req, result, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error,
                             const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkRemoveRequest(req, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state)
{
    QLandmarkManagerEngine::updateRequestState(req,state);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest* req, const QList<QLandmarkCategoryId>& result,
        QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryIdFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest* req, const QList<QLandmarkCategory>& result,
        QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryFetchByIdRequest(QLandmarkCategoryFetchByIdRequest* req, const QList<QLandmarkCategory>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryFetchByIdRequest(req, result, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategorySaveRequest(req, result, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest* req,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryRemoveRequest(req, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkImportRequest(QLandmarkImportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                 QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkImportRequest(req, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                 QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkExportRequest(req, error, errorString, newState);
}
