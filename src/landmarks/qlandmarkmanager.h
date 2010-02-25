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

#ifndef QLANDMARKMANAGER_H
#define QLANDMARKMANAGER_H

#include "qmobilityglobal.h"
#include "qlandmark.h"

#include <QObject>
#include <QMap>
class QIODevice;
class QStringList;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE
class QLandmarkCategory;
class QLandmarkFilter;
class QLandmarkSortOrder;
class QLandmarkSearchResult;
class QLandmarkManagerPrivate;


class Q_LANDMARKS_EXPORT QLandmarkManager: public QObject
{
    Q_OBJECT
public:
    enum Error {NoError, ReadOnly};
    enum Format{LandmarkExchange, GPSExchange, KeyholeMarkupLanguage, CommaSeparatedValues, Custom};

    QLandmarkManager(const QString &name = QString(), QObject *parent =0);
    virtual ~QLandmarkManager();

    bool saveLandmark(QLandmark *landmark);
    bool saveLandmarks(QList<QLandmark *> landmarks, QMap<int, QLandmarkManager::Error> *errorMap); //default of 0?
    bool removeLandmark(const QLandmarkId &landmarkId);
    bool removeLandmarks(const QList<QLandmarkId> &landmarksIds, QMap<int, QLandmarkManager::Error> *errorMap);

    bool saveCategory(QLandmarkCategory *category);
    bool removeCategory(const QLandmarkCategoryId &categoryId);

    QLandmarkCategory category(const QLandmarkCategoryId &categoryId);
    QList<QLandmarkCategoryId> categoryIds() const;

    QLandmark landmark(const QLandmarkId &landmarkId);
    QList<QLandmarkId> landmarkIds(const QLandmarkFilter &filter,
                                const QLandmarkSortOrder &sortOrder) const;

    bool importLandmarks(QIODevice *device, Format format);
    bool exportLandmarks(QIODevice *device, Format format, QList<QLandmarkId> landmarkIds = QList<QLandmarkId>());

    Error error() const;
    QString errorString() const;

    static QStringList availableManagers();
    static bool addManager(const QString &name, const QString &uri);
    static bool removeManager(const QString &name);
    static QString defaultManager();
    static bool setDefaultManager(const QString &name);

Q_SIGNALS:
    void landmarkAdded(const QLandmarkId &landmarkId);
    void landmarkUpdated(const QLandmarkId &landmarkId);
    void landmarkRemoved(const QLandmarkId &landmarkId);

    void categoryAdded(const QLandmarkCategoryId &categoryId);
    void categoryUpdated(const QLandmarkCategoryId &categoryId);
    void categoryRemoved(const QLandmarkCategoryId &categoryId);

private:
    QLandmarkManagerPrivate *d;
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
    QLandmarkManager database();
    void setDatabase(const QLandmarkManager &database);
    QString format();
    void setFormat(const QString &format);

    virtual bool decode();
    virtual bool encode(QList<QLandmarkId> landmarkIds =QList<QLandmarkId>());
private:
    QLandmarkCodecPrivate *d;
};

class QLandmarkAbstractRequestPrivate;
class QLandmarkAbstractRequest : public QObject
{
    Q_OBJECT
public:
    enum State{Inactive, Active, Canceling, Canceled, Finished};
    QLandmarkAbstractRequest();
    virtual ~QLandmarkAbstractRequest();

    State state();
    QLandmarkManager::Error error() const;
    QString errorString();

    QLandmarkManager * landmarkManager();
    void setLandmarkManager(QLandmarkManager *database);

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);

signals:
    void resultsAvailable();
    void stateChanged(QLandmarkAbstractRequest::State newState);

private:
    QLandmarkAbstractRequestPrivate *d;
};

class QLandmarkIdFetchRequestPrivate;
class QLandmarkIdFetchRequest : QLandmarkAbstractRequest
{
public:
    QLandmarkIdFetchRequest();
    ~QLandmarkIdFetchRequest();

    //QLandmarkFilter filter() const;//is this needed it's not like
    //                               //you can query values from the filter?
    void setFilter(const QLandmarkFilter &filter);

    //QLandmarkSortOrder sorting();  //is this needed, it's no like
    //                               //you can query values from the filter?
    void setSorting(const QLandmarkSortOrder &sorting);
    QList<QLandmarkId> landmarkIds();
private:
    QLandmarkIdFetchRequestPrivate *d;
};

class QLandmarkSaveRequestPrivate;
class QLandmarkSaveRequest : QLandmarkAbstractRequest
{
public:
    QLandmarkSaveRequest();
    ~QLandmarkSaveRequest();

    QList<QLandmark> landmarks() const;
    void setLandmarks(const QList<QLandmark> &landmarks);
private:
    Q_DISABLE_COPY(QLandmarkSaveRequest)
    QLandmarkSaveRequestPrivate *d;
};

class QLandmarkRemoveRequestPrivate;
class QLandmarkRemoveRequest : QLandmarkAbstractRequest
{
public:
    QLandmarkRemoveRequest();
    ~QLandmarkRemoveRequest();

    QList<QLandmarkId> landmarkIds() const;
    void setLandmarkIds(const QList<QLandmarkId> &landmarkIds);
private:
    Q_DISABLE_COPY(QLandmarkRemoveRequest);
    QLandmarkRemoveRequestPrivate *d;
};
/*
class QLandmarkFetchRequestPrivate;
class QLandmarkFetchRequest : QLandmarkAbstractRequest
{
public:
    QLandmarkFetchRequest();
    ~QLandmarkFetchRequest();

    QList<QLandmark> landmarks();
private:
    QLandmarkIdFetchRequestPrivate *d;
};*/

class QLandmarkImportRequestPrivate;
class QLandmarkImportRequest : public QLandmarkAbstractRequest
{
public:
    QLandmarkImportRequest();
    ~QLandmarkImportRequest();

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    QLandmarkManager::Format format() const;
    void setFormat(QLandmarkManager::Format format);
private:
    Q_DISABLE_COPY(QLandmarkImportRequest)
    QLandmarkImportRequestPrivate *d;
};

class QLandmarkExportRequestPrivate;
class QLandmarkExportRequest : public QLandmarkAbstractRequest
{
public:
    QLandmarkExportRequest();
    ~QLandmarkExportRequest();

    QIODevice *device() const;
    void setDevice(QIODevice *device);

    QLandmarkManager::Format format() const;
    void setFormat(QLandmarkManager::Format format);

    QList<QLandmarkId> landmarkIds() const;
    void setLandmarkIds(const QList<QLandmarkId> landmarkIds);

private:
    Q_DISABLE_COPY(QLandmarkExportRequest);
    QLandmarkExportRequestPrivate *d;
};

/*
class QLandmarkCategoryFetchRequestPrivate;
class QLandmarkCategoryFetchRequest : public QObject
{
    Q_OBJECT
public:
    enum State{Inactive, Active, Canceling, Canceled, Finished};
    enum SortOrder{Ascending, Descending};
    QLandmarkCategoryFetchRequest();
    virtual ~QLandmarkCategoryFetchRequest();

    State state();
    QLandmarkManager::Error error() const;
    QString errorString();

    SortOrder sorting();
    void setSorting(SortOrder sort);

    QLandmarkManager * landmarkManager();
    void setLandmarkManager(QLandmarkManager *database);

public slots:
    bool start();
    bool cancel();
    bool waitForFinished(int msecs = 0);

signals:
    void resultsAvailable();
    void stateChanged(QLandmarkAbstractFetchRequest::State newState);

private:
    QLandmarkCategoryFetchRequestPrivate *d;
};*/

QTM_END_NAMESPACE

QT_END_HEADER

#endif
