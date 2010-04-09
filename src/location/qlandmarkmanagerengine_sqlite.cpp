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

QTM_BEGIN_NAMESPACE

QLandmarkManagerEngineSqlite::QLandmarkManagerEngineSqlite(const QString &filename)
    : m_dbFilename(filename)
{
    // check for exising
    // if doesn't exist (and it's not :memory:), create and populate
}

QLandmarkManagerEngineSqlite::~QLandmarkManagerEngineSqlite() {}

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

// TODO do in base class using other methods?
//QString QLandmarkManagerEngineSqlite::managerUri() const;

/* Filtering */
QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIds(const QLandmarkFilter& filter,
                               const QList<QLandmarkSortOrder>& sortOrders,
                               QLandmarkManager::Error *error,
                               QString *errorString) const
{
    return QList<QLandmarkId>();
}

QList<QLandmarkCategoryId> QLandmarkManagerEngineSqlite::categoryIds(QLandmarkManager::Error *error,
                                       QString *errorString) const
{
    return QList<QLandmarkId>();
}

/* Retrieval */
QLandmark QLandmarkManagerEngineSqlite::landmark(const QLandmarkId &landmarkId,
                   QLandmarkManager::Error *error,
                   QString *errorString) const
{
    return QLandmark;
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QLandmarkFilter &filter,
                           const QList<QLandmarkSortOrder>& sortOrders,
                           QLandmarkManager::Error *error,
                           QString *errorString) const
{
    return QList<QLandmark>();
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QList<QLandmarkId> &landmarkIds,
                           QMap<int, QLandmarkManager::Error> *errorMap,
                           QLandmarkManager::Error *error,
                           QString *errorString) const
{
    return QList<QLandmark>();
}

QLandmarkCategory QLandmarkManagerEngineSqlite::category(const QLandmarkCategoryId &landmarkCategoryId,
                           QLandmarkManager::Error *error,
                           QString *errorString) const
{
    return QLandmarkCategory();
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                    QLandmarkManager::Error *error,
                                    QString *errorString)
{
    return QList<QLandmarkCategory>();
}

/*saving and removing*/
bool QLandmarkManagerEngineSqlite::saveLandmark(QLandmark* landmark,
                  QLandmarkManager::Error *error,
                  QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::saveLandmarks(QList<QLandmark> * landmark,
                   QMap<int, QLandmarkManager::Error> *errorMap,
                   QLandmarkManager::Error *error,
                   QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::removeLandmark(const QLandmarkId &landmarkId,
                    QLandmarkManager::Error *error,
                    QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::removeLandmarks(const QList<QLandmarkId> &landmarkId,
                     QMap<int, QLandmarkManager::Error> *errorMap,
                     QLandmarkManager::Error *error,
                     QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::saveCategory(QLandmarkCategory* category,
                  QLandmarkManager::Error *error,
                  QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::removeCategory(const QLandmarkCategoryId &categoryId,
                    QLandmarkManager::Error *error,
                    QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::importLandmarks(QIODevice *device,
                     QLandmarkManager::Format format,
                     QLandmarkManager::Error *error,
                     QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::exportLandmarks(QIODevice *device,
                     QLandmarkManager::Format format,
                     QList<QLandmarkId> landmarkIds,
                     QLandmarkManager::Error *error,
                     QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::isFilterSupported(QLandmarkFilter::FilterType filterType) const
{
    return false;
}

/* Asynchronous Request Support */
void QLandmarkManagerEngineSqlite::requestDestroyed(QLandmarkAbstractRequest* request)
{
}

bool QLandmarkManagerEngineSqlite::startRequest(QLandmarkAbstractRequest* request)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::cancelRequest(QLandmarkAbstractRequest* request)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::waitForRequestFinished(QLandmarkAbstractRequest* request,
                                                          int msecs)
{
    return false;
}

#include "moc_qlandmarkmanagerengine_sqlite_p.cpp"

QTM_END_NAMESPACE

