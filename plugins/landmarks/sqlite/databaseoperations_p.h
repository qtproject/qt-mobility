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

#ifndef DATABASEOPERATIONS_P_H
#define DATABASEOPERATIONS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qlandmarkmanager.h>
#include <QRunnable>
#include <QMap>

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE
class QLandmark;
class QLandmarkAbstractRequest;
class QLandmarkCategory;
class QLandmarkSortOrder;
class QLandmarkNameSort;
class QLandmarkFilter;
QTM_END_NAMESPACE

typedef QMap<int, QLandmarkManager::Error>  ERROR_MAP;
class QLandmarkManagerEngineSqlite;
class QLandmarkFileHandlerGpx;

namespace DatabaseOperations {
    class QueryRun : public QRunnable
    {
    public:
        QueryRun(QLandmarkAbstractRequest *req =0,
                 const QString &uri=QString(),
                 QLandmarkManagerEngineSqlite *eng =0);
        ~QueryRun();
        void run();

        QLandmarkAbstractRequest *request;
        QString connectionName;
        QLandmarkManager::Error error;
        QString errorString;
        QMap<int, QLandmarkManager::Error> errorMap;
        QString managerUri;
        volatile bool isCanceled;
        QLandmarkManagerEngineSqlite *engine;
        QLandmarkFileHandlerGpx *gpxHandler;
    };

    QLandmark retrieveLandmark(const QString &connectionName, const QLandmarkId &landmarkId,
                               QLandmarkManager::Error *error, QString *errorString,
                               const QString &managerUri, QueryRun *queryRun=0);

    QList<QLandmarkId> landmarkIds(const QString &connectionName, const QLandmarkFilter& filter,
                                   const QList<QLandmarkSortOrder>& sortOrders,
                                   int limit, int offset,
                                   QLandmarkManager::Error *error, QString *errorString,
                                   const QString &managerUri, QueryRun * queryRun =0);

    QList<QLandmark> landmarks(const QString &connectionName, const QLandmarkFilter& filter,
                               const QList<QLandmarkSortOrder>& sortOrders,
                               int limit, int offset,
                               QLandmarkManager::Error *error, QString *errorString,
                               const QString &managerUri, QueryRun *queryRun =0);

    bool saveLandmarkHelper(const QString &connectionName, QLandmark* landmark,
                      QLandmarkManager::Error *error, QString *errorString,
                      const QString &managerUri);

    bool saveLandmark(const QString &connectionName, QLandmark* landmark,
                      QLandmarkManager::Error *error, QString *errorString,
                      const QString &managerUri);

    bool saveLandmarks(const QString &connectionName, QList<QLandmark> * landmark,
                       QMap<int, QLandmarkManager::Error> *errorMap,
                       QLandmarkManager::Error *error, QString *errorString,
                       const QString &managerUri, QueryRun *queryRun =0);

    bool removeLandmark(const QString &connectionName, const QLandmarkId &landmarkId,
                        QLandmarkManager::Error *error, QString *errorString,
                        const QString &managerUri);

    bool removeLandmarks(const QString &connectionName, const QList<QLandmarkId> &landmarkIds,
                         QMap<int, QLandmarkManager::Error> *errorMap,
                         QLandmarkManager::Error *error,
                         QString *errorString, const QString &managerUri,
                         QueryRun *queryRun = 0);

    QList<QLandmarkCategoryId> categoryIds(const QString &connectionName,
                                           const QLandmarkNameSort &nameSort,
                                           int limit, int offet,
                                           QLandmarkManager::Error *error, QString *errorString,
                                           const QString &managerUri,
                                           QueryRun *queryRun = 0);

    QLandmarkCategory category(const QString &connectionName, const QLandmarkCategoryId &landmarkCategoryId,
                               QLandmarkManager::Error *error,
                               QString *errorString, const QString &managerUri);

    QList<QLandmarkCategory> categories(const QString &connectionName,
                                        const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                        const QLandmarkNameSort &nameSort,
                                        int limit, int offset,
                                        QLandmarkManager::Error *error, QString *errorString,
                                        const QString &managerUri, bool needAll,
                                        QueryRun *queryRun = 0);

    bool saveCategoryHelper(const QString &connectionName, QLandmarkCategory *category,
                      QLandmarkManager::Error *error, QString *errorString,
                      const QString &managerUri);

    bool saveCategory(const QString &connectionName, QLandmarkCategory *category,
                      QLandmarkManager::Error *error, QString *errorString,
                      const QString &managerUri);

    bool saveCategories(const QString &connectionName, QList<QLandmarkCategory> * categories,
                        QMap<int, QLandmarkManager::Error> *errorMap,
                        QLandmarkManager::Error *error, QString *errorString,
                        const QString &managerUri);

    bool removeCategory(const QString &connectionName, const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString, const QString &managerUri);

    bool removeCategories(const QString &connectionName, const QList<QLandmarkCategoryId> &categoryIds,
                          QMap<int, QLandmarkManager::Error> *errorMap,
                          QLandmarkManager::Error *error,
                          QString *errorString, const QString &managerUri);

    bool importLandmarks(const QString &connectionName,
                         QIODevice *device,
                         const QString &format,
                         QLandmarkManager::ImportExportOption option,
                         const QLandmarkCategoryId &categoryId,
                         QLandmarkManager::Error *error,
                         QString *errorString, const QString &managerUri,
                         QueryRun *queryRun = 0);

    bool exportLandmarks(const QString &connectionName,
                         QIODevice *device,
                         const QString &format,
                         QList<QLandmarkId> landmarkIds,
                         QLandmarkManager::Error *error,
                         QString *errorString,
                         const QString &managerUri);
}

#endif
