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

#include "qlandmarkmanagerengine.h"

#include <QSqlDatabase>

QTM_BEGIN_NAMESPACE

class QLandmarkNameFilter;
class QLandmarkProximityFilter;
class QLandmarkNearestFilter;
class QLandmarkCategoryFilter;
class QLandmarkBoxFilter;
class QLandmarkIntersectionFilter;
class QLandmarkUnionFilter;
class QLandmarkCustomFilter;

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
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter* filter,
                                   const QList<const QLandmarkSortOrder*>& sortOrders,
                                   QLandmarkManager::Error *error,
                                   QString *errorString) const;
    QList<QLandmarkCategoryId> categoryIds(QLandmarkManager::Error *error,
                                           QString *errorString) const;

    /* Retrieval */
    QLandmark landmark(const QLandmarkId &landmarkId,
                       QLandmarkManager::Error *error,
                       QString *errorString) const;
    QList<QLandmark> landmarks(const QLandmarkFilter *filter,
                               const QList<const QLandmarkSortOrder*>& sortOrders,
                               QLandmarkManager::Error *error,
                               QString *errorString) const;
    QList<QLandmark> landmarks(const QList<QLandmarkId> &landmarkIds,
                               QMap<int, QLandmarkManager::Error> *errorMap,
                               QLandmarkManager::Error *error,
                               QString *errorString) const;
    QLandmarkCategory category(const QLandmarkCategoryId &landmarkCategoryId,
                               QLandmarkManager::Error *error,
                               QString *errorString) const;
    QList<QLandmarkCategory> categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                        QLandmarkManager::Error *error,
                                        QString *errorString);

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
                         QLandmarkManager::Format format,
                         QLandmarkManager::Error *error,
                         QString *errorString);
    bool exportLandmarks(QIODevice *device,
                         QLandmarkManager::Format format,
                         QList<QLandmarkId> landmarkIds,
                         QLandmarkManager::Error *error,
                         QString *errorString);

    bool isFilterSupported(QLandmarkFilter::FilterType filterType) const;

    /* Asynchronous Request Support */
    void requestDestroyed(QLandmarkAbstractRequest* request);
    bool startRequest(QLandmarkAbstractRequest* request);
    bool cancelRequest(QLandmarkAbstractRequest* request);
    bool waitForRequestFinished(QLandmarkAbstractRequest* request, int msecs);

private:
    QList<QLandmarkId> landmarkIdsDefault(const QLandmarkFilter* filter,
                                          QLandmarkManager::Error *error,
                                          QString *errorString) const;

    QList<QLandmarkId> landmarkIdsName(const QLandmarkNameFilter* filter,
                                       QLandmarkManager::Error *error,
                                       QString *errorString) const;

    QList<QLandmarkId> landmarkIdsProximity(const QLandmarkProximityFilter* filter,
                                            QLandmarkManager::Error *error,
                                            QString *errorString) const;

    QList<QLandmarkId> landmarkIdsNearest(const QLandmarkNearestFilter* filter,
                                          QLandmarkManager::Error *error,
                                          QString *errorString) const;

    QList<QLandmarkId> landmarkIdsCategory(const QLandmarkCategoryFilter* filter,
                                           QLandmarkManager::Error *error,
                                           QString *errorString) const;

    QList<QLandmarkId> landmarkIdsBox(const QLandmarkBoxFilter* filter,
                                      QLandmarkManager::Error *error,
                                      QString *errorString) const;

    QList<QLandmarkId> landmarkIdsIntersection(const QLandmarkIntersectionFilter* filter,
            QLandmarkManager::Error *error,
            QString *errorString) const;

    QList<QLandmarkId> landmarkIdsUnion(const QLandmarkUnionFilter* filter,
                                        QLandmarkManager::Error *error,
                                        QString *errorString) const;

    QList<QLandmarkId> landmarkIdsCustom(const QLandmarkCustomFilter* filter,
                                         QLandmarkManager::Error *error,
                                         QString *errorString) const;

    bool saveLandmarkInternal(QLandmark* landmark,
                              QLandmarkManager::Error *error,
                              QString *errorString,
                              bool *added,
                              bool *changed);
    bool removeLandmarkInternal(const QLandmarkId &landmarkId,
                                QLandmarkManager::Error *error,
                                QString *errorString,
                                bool *removed);
    QString m_dbFilename;
    mutable QSqlDatabase m_db;
};

QTM_END_NAMESPACE

#endif // QLANDMARKMANAGERENGINE_SQLITE_P_H
