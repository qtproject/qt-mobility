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

#ifndef QLANDMARKMANAGERENGINE_SQLITE_P_H
#define QLANDMARKMANAGERENGINE_SQLITE_P_H

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

#include "qlandmarkmanagerengine.h"

#include <QSqlDatabase>
#include <QHash>

QTM_USE_NAMESPACE
typedef QMap<int, QLandmarkManager::Error>  ERROR_MAP;

class QueryRun;

class QLandmarkManagerEngineSqlite : public QLandmarkManagerEngine
{
    Q_OBJECT
public:
    QLandmarkManagerEngineSqlite(const QString &filename = QString(":memory:"));
    ~QLandmarkManagerEngineSqlite();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    /* Filtering */
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter& filter,
                                   const QList<QLandmarkSortOrder>& sortOrders,
                                   const QLandmarkFetchHint &fetchHint,
                                   QLandmarkManager::Error *error,
                                   QString *errorString) const;
    QList<QLandmarkCategoryId> categoryIds(QLandmarkManager::Error *error,
                                           QString *errorString) const;

    /* Retrieval */
    QLandmark landmark(const QLandmarkId &landmarkId,
                       QLandmarkManager::Error *error,
                       QString *errorString) const;
    QList<QLandmark> landmarks(const QLandmarkFilter& filter,
                               const QList<QLandmarkSortOrder>& sortOrders,
                               const QLandmarkFetchHint &fetchHint,
                               QLandmarkManager::Error *error,
                               QString *errorString) const;
    QLandmarkCategory category(const QLandmarkCategoryId &landmarkCategoryId,
                               QLandmarkManager::Error *error,
                               QString *errorString) const;
    QList<QLandmarkCategory> categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                        QLandmarkManager::Error *error,
                                        QString *errorString) const;

    /*saving and removing*/
    bool saveLandmark(QLandmark* landmark,
                      QLandmarkManager::Error *error,
                      QString *errorString);
    bool saveLandmarks(QList<QLandmark> * landmark,
                       QMap<int, QLandmarkManager::Error> *errorMap,
                       QLandmarkManager::Error *error,
                       QString *errorString);
    bool removeLandmark(const QLandmarkId &landmarkId,
                        QLandmarkManager::Error *error,
                        QString *errorString);
    bool removeLandmarks(const QList<QLandmarkId> &landmarkId,
                         QMap<int, QLandmarkManager::Error> *errorMap,
                         QLandmarkManager::Error *error,
                         QString *errorString);

    bool saveCategory(QLandmarkCategory* category,
                      QLandmarkManager::Error *error,
                      QString *errorString);
    bool removeCategory(const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString);

    bool importLandmarks(QIODevice *device,
                         const QByteArray &format,
                         QLandmarkManager::Error *error,
                         QString *errorString);
    bool exportLandmarks(QIODevice *device,
                         const QByteArray &format,
                         QList<QLandmarkId> landmarkIds,
                         QLandmarkManager::Error *error,
                         QString *errorString);

    QLandmarkManager::FilterSupportLevel filterSupportLevel(const QLandmarkFilter &filter) const;
    bool isFeatureSupported(QLandmarkManager::LandmarkFeature feature) const;

    bool isReadOnly() const;
    bool isReadOnly(const QLandmarkId &landmarkId) const;
    bool isReadOnly(const QLandmarkCategoryId &categoryId) const;

    /* Asynchronous Request Support */
    void requestDestroyed(QLandmarkAbstractRequest* request);
    bool startRequest(QLandmarkAbstractRequest* request);
    bool cancelRequest(QLandmarkAbstractRequest* request);
    bool waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs);

public slots:
    void updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState);
    void updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result,
                                    QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error,
                                    const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory> &categories, QLandmarkManager::Error error,
                                     const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState);
    void updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state);

public:
    static QList<QLandmarkId> sortLandmarks(const QList<QLandmark>& landmarks, const QList<QLandmarkSortOrder> &sortOrders) {
        return QLandmarkManagerEngine::sortLandmarks(landmarks,sortOrders);
    }


private:
    bool saveLandmarkInternal(QLandmark* landmark,
                              QLandmarkManager::Error *error,
                              QString *errorString,
                              bool *added,
                              bool *changed);
    bool removeLandmarkInternal(const QLandmarkId &landmarkId,
                                QLandmarkManager::Error *error,
                                QString *errorString,
                                bool *removed);

    bool importLandmarksLmx(QIODevice *device,
                            QLandmarkManager::Error *error,
                            QString *errorString);
    bool importLandmarksGpx(QIODevice *device,
                            QLandmarkManager::Error *error,
                            QString *errorString);

    bool exportLandmarksLmx(QIODevice *device,
                            QList<QLandmarkId> landmarkIds,
                            QLandmarkManager::Error *error,
                            QString *errorString);
    bool exportLandmarksGpx(QIODevice *device,
                            QList<QLandmarkId> landmarkIds,
                            QLandmarkManager::Error *error,
                            QString *errorString);

    QString m_dbFilename;
    QString m_dbConnectionName;
    QHash<QLandmarkAbstractRequest *, QueryRun *> m_requestRunHash;
    friend class QueryRun;
};

#endif // QLANDMARKMANAGERENGINE_SQLITE_P_H
