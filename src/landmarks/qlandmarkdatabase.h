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
class QLandmarkDatabasePrivate;

class Q_LANDMARKS_EXPORT QLandmarkDatabase: public QObject
{
    Q_OBJECT
public:
    enum Error {NoError, NonWritablePlatform};
    enum FetchStatus {Active, Cancelled, Done};
    static QLandmarkDatabase instance(const QString &name = QString());
    virtual ~QLandmarkDatabase();

    bool saveLandmark(QLandmark *landmark);
    bool saveLandmarks(QList<QLandmark *> landmarks);
    bool removeLandmark(const QLandmarkId &landmarkId);
    bool removeLandmarks(const QList<QLandmarkId> &landmarksIds);

    bool saveCategory(QLandmarkCategory *category);
    bool removeCategory(const QLandmarkCategoryId &categoryId);
    bool removeCategories(const QList<QLandmarkCategoryId> &categoryIds);

//not needed??
//    bool saveCategories(const QList<QLandmarkCategory *> categoriesIds);
//    bool removeCategories(const QList<QLandmarkCategoryId> &categoryIds);

    //It might be better to having a command object to 
    //perform all these variations of requests...
    QLandmarkCategory category(const QLandmarkCategoryId &categoryId);
    QList<QLandmarkCategory> categories() const;
    QList<QLandmarkCategoryId> categoryIds() const;

    QList<QLandmarkCategory> fetchCategories() const;
    QList<QLandmarkCategoryId> fetchCategoryIds() const;
    FetchStatus categoryFetchStatus() const;
    bool cancelCategoryFetch() const;

    QLandmark landmark(const QLandmarkId &landmarkId);
    QList<QLandmarkId> landmarks(const QLandmarkFilter &filter,
                                const QLandmarkSortOrder &sortOrder) const;

    QList<QLandmark> landmarkIds(const QLandmarkFilter &filter,
                                const QLandmarkSortOrder &sortOrder) const;

    void fetchLandmarks(const QLandmarkFilter &filter,
                                const QLandmarkSortOrder &sortOrder) const;

    void fetchLandmarkIds(const QLandmarkFilter &filter,
                                const QLandmarkSortOrder &sortOrder) const;

    FetchStatus landmarkFetchStatus() const;
    bool cancelLandmarkFetch() const;

    //TODO: find a better way to have asynchronous and syncrhonous
    //import and exports besides, should a filename string be passed, qiodevice?
    bool importDatabase(const QFile &file);
    bool exportDatabase(const QFile &file, QList<QLandmarkId> landmarkIds = QList<QLandmarkId>());

    void importDatabaseAsynch(const QFile &file);
    void exportDatabaseAsynch(const QFile &file, QList<QLandmarkId> landmarkIds = QList<QLandmarkId>());

    Error error() const;
    QString errorString() const;

Q_SIGNALS:
    void landmarkAdded(const QLandmarkId &landmark);
    void landmarkUpdated(const QLandmarkId &landmark);
    void landmarkDeleted(const QLandmarkId &landmark);

    void categoryAdded(const QLandmarkCategoryId &categoryId);
    void categoryUpdated(const QLandmarkCategoryId &categoryId);
    void categoryDeleted(const QLandmarkCategoryId &categoryId);

    void importFinished(QList<QLandmarkId> &landmarks);
    void exportFinished();

    void landmarkFetchFinished(QList<QLandmarkId> landmarkIds);
    void categoryFetchFinished(QList<QLandmarkCategoryId> categoryIds);

private:
    QLandmarkDatabase();
    QLandmarkDatabasePrivate *d;
};

class QLandmarkCodec;
class QLandmarkCodecPluginPrivate;
class QLandmarkCodecPlugin : public QObject
{
    Q_OBJECT
public:
    QLandmarkCodecPlugin(QObject *parent =0);
    ~QLandmarkCodecPlugin();
    virtual QLandmarkCodec *create();
    QStringList keys(); //landmark formats
private:
    QLandmarkCodecPluginPrivate *d;
};


class QIODevice;
class QLandmarkCodecPrivate;
class QLandmarkCodec
{
public:
    QLandmarkCodec();

    virtual ~QLandmarkCodec();
    QIODevice *device() const;
    void setDevice(QIODevice *device);
    QLandmarkDatabase database();
    void setDatabase(const QLandmarkDatabase &database);
    QString format();
    void setFormat(const QString &format);

    virtual bool decode();
    virtual bool encode(QList<QLandmarkId> landmarkIds =QList<QLandmarkId>());
private:
    QLandmarkCodecPrivate *d;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
