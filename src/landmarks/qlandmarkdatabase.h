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

#ifndef QLANDMARKDATABASE_H
#define QLANDMARKDATABASE_H

#include "qmobilityglobal.h"
#include "qlandmark.h"

#include <QObject>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE
class QLandmarkCategory;
class QLandmarkFilter;
class QLandmarkSortOrder;
class QFile;
class QLandmarkSearchResult;

class Q_LANDMARKS_EXPORT QLandmarkDatabase: public QObject
{
    Q_OBJECT
public:
    enum Error {NoError, NonWritablePlatform};
    enum FetchStatus {Active, Cancelled, Done};
    enum FileFormat {Landmarks, GPX, CSV, Keyhole, Auto};
    static QLandmarkDatabase instance(const QString &name = QString());
    virtual ~QLandmarkDatabase();

    bool saveLandmark(const QLandmark &landmark);
    bool removeLandmark(const QLandmark &landmark);

    bool saveCategory(const QLandmarkCategory &category);
    bool removeCategory(const QLandmarkCategory &category);

    QList<QLandmarkCategory> categories() const;

    QList<QLandmark> landmarks(const QLandmarkFilter &filter,
                                const QLandmarkSortOrder &sortOrder) const;

    QList<QLandmark> fetchLandmarks(const QLandmarkFilter &filter,
                                const QLandmarkSortOrder &sortOrder) const;

    FetchStatus landmarkFetchStatus() const;
    bool cancelLandmarkFetch() const;

    //TODO: find a good way to have asynchronous and syncrhonous
    //import and exports
    bool importDatabase(const QFile &file, FileFormat format);
    bool exportDatabase(const QFile &file, FileFormat format);

    Error error() const;
    QString errorString() const;

Q_SIGNALS:
    void landmarkAdded(const QLandmark &landmark);
    void landmarkUpdated(const QLandmark &landmark);
    void landmarkDeleted(const QLandmark &landmark);

    void categoryAdded(const QLandmarkCategory &category);
    void categoryUpdated(const QLandmarkCategory &category);
    void categoryDeleted(const QLandmarkCategory &category);

    void importFinished(QList<QLandmark> &landmarks);
    void exportFinished();

    void landmarkFetchFinished(QList<QLandmark> landmarks);

private:
    QLandmarkDatabase();
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
