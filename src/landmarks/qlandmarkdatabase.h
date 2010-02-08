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

#include <QObject>

class QLandmark;
class QLandmarkCategory;
class QSearchFilter;
class QLandmarkSortOrder;
class QFile;
class QLandmarkSearchResult;

class QLandmarkDatabase: public QObject
{
    Q_OBJECT
public:
    enum Error {NoError, NonWritablePlatform};
    enum SearchStatus {Active, Cancelled, Done};
    enum FileFormat {LMX, GPX, CSV, KML, Auto};
    static QLandmarkDatabase instance(const QString &name = QString());
    virtual ~QLandmarkDatabase() {}

    bool addLandmark(const QLandmark &landmark);
    bool updateLandmark(const QLandmark &oldLandmark, const QLandmark &newLandmark);
    bool deleteLandmark(const QLandmark &landmark);

    bool addCategory(const QLandmarkCategory &category);
    bool updateCategory(const QLandmarkCategory &oldCategory);
    bool deleteCategory(const QLandmarkCategory &category);
    bool deleteCategory(const QString &);
    QList<QLandmarkCategory> allCategories() const;
    bool isCategoryModifiable(const QLandmarkCategory &category);

    QList<QLandmark> search(QList<QSearchFilter> filters,
                            QList<QLandmarkSortOrder> sortOrders) const;
    QList<QLandmark> search(QSearchFilter filter,
                            QLandmarkSortOrder sortOrder) const;
    bool searchRequest(QList<QSearchFilter> filters,
                       QList<QLandmarkSortOrder> sortOrders) const;
    bool searchRequest(QSearchFilter filter, QLandmarkSortOrder) const;
    SearchStatus searchStatus() const;
    bool cancelSearchRequest();

    bool importDatabase(const QFile &file, FileFormat format);
    bool exportDatabase(QFile &file, FileFormat format);

    Error error() const;
    QString errorString() const;

Q_SIGNALS:
    void landmarkAdded(const QLandmark &landmark);
    void landmarkUpdated(const QLandmark &landmark);
    void landmarkDeleted(const QLandmark &landmark);

    void categoryAdded(const QString &category);
    void categoryRenamed(const QString &oldCategory);
    void categoryDeleted(const QString &category);

    void searchResult(const QLandmarkSearchResult &result);
private:
    QLandmarkDatabase();
};

#endif
