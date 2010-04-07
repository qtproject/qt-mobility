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

#ifndef QGALLERYREQUEST_H
#define QGALLERYREQUEST_H

#include "qgalleryitemlist.h"

#include "qgalleryfilter.h"

#include <QtCore/qstringlist.h>

class QAbstractGallery;
class QGalleryAbstractResponse;
class QGalleryFilter;
class QUrl;

class QGalleryAbstractRequestPrivate;

class Q_GALLERY_EXPORT QGalleryAbstractRequest : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryAbstractRequest)
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery)
    Q_PROPERTY(bool supported READ isSupported NOTIFY supportedChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int result READ result NOTIFY resultChanged)
    Q_PROPERTY(int currentProgress READ currentProgress NOTIFY progressChanged)
    Q_PROPERTY(int maximumProgress READ maximumProgress NOTIFY progressChanged)
    Q_ENUMS(State)
    Q_ENUMS(Result)
    Q_ENUMS(Type)
public:
    enum State
    {
        Inactive,
        Active,
        Cancelling,
        Idle
    };

    enum Result
    {
        NoResult,
        Succeeded,
        Cancelled,
        NoGallery,
        NotSupported,
        ConnectionError,
        InvalidFilter,
        RequestError = 100
    };

    enum Type
    {
        Item,
        Count,
        Insert,
        Remove,
        Copy,
        Move
    };

    explicit QGalleryAbstractRequest(Type type, QObject *parent = 0);
    explicit QGalleryAbstractRequest(QAbstractGallery *gallery, Type type, QObject *parent = 0);
    ~QGalleryAbstractRequest();

    QAbstractGallery *gallery() const;
    void setGallery(QAbstractGallery *gallery);

    bool isSupported() const;

    Type type() const;
    State state() const;
    int result() const;

    int currentProgress() const;
    int maximumProgress() const;

    bool waitForFinished(int msecs);

public Q_SLOTS:
    void execute();
    void cancel();
    void clear();

Q_SIGNALS:
    void supportedChanged();
    void succeeded();
    void cancelled();
    void failed(int result);
    void finished(int result);
    void stateChanged(QGalleryAbstractRequest::State state);
    void resultChanged();
    void progressChanged(int current, int maximum);

protected:
    virtual void setResponse(QGalleryAbstractResponse *response) = 0;

    explicit QGalleryAbstractRequest(QGalleryAbstractRequestPrivate &dd, QObject *parent);

    QGalleryAbstractRequestPrivate *d_ptr;

private:
    Q_PRIVATE_SLOT(d_ptr, void _q_finished())
    Q_PRIVATE_SLOT(d_ptr, void _q_galleryDestroyed())
    Q_PRIVATE_SLOT(d_ptr, void _q_progressChanged(int, int))
};

class QGalleryAbstractResponsePrivate;

class Q_GALLERY_EXPORT QGalleryAbstractResponse : public QGalleryItemList
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryAbstractResponse)
public:
    QGalleryAbstractResponse(QObject *parent = 0);
    ~QGalleryAbstractResponse();

    int result() const;
    bool isIdle() const;

    virtual void cancel();

    virtual bool waitForFinished(int msecs) = 0;

Q_SIGNALS:
    void finished();
    void progressChanged(int current, int maximum);

protected:
    QGalleryAbstractResponse(QGalleryAbstractResponsePrivate &dd, QObject *parent);

    void finish(int result, bool idle = false);
};

class QGalleryItemRequestPrivate;

class Q_GALLERY_EXPORT QGalleryItemRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryItemRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QStringList sortFields READ sortFields WRITE setSortFields)
    Q_PROPERTY(bool live READ isLive WRITE setLive)
    Q_PROPERTY(int initialCursorPosition READ initialCursorPosition WRITE setInitialCursorPosition)
    Q_PROPERTY(int minimumPagedItems READ minimumPagedItems WRITE setMinimumPagedItems)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType)
    Q_PROPERTY(QGalleryFilter filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString itemId READ itemId WRITE setItemId NOTIFY itemIdsChanged)
    Q_PROPERTY(QStringList itemIds READ itemIds WRITE setItemIds NOTIFY itemIdsChanged)
    Q_PROPERTY(QUrl itemUrl READ itemUrl WRITE setItemUrl NOTIFY itemUrlsChanged)
    Q_PROPERTY(QList<QUrl> itemUrls READ itemUrls WRITE setItemUrls NOTIFY itemUrlsChanged)
    Q_PROPERTY(QString containerId READ containerId WRITE setContainerId)
    Q_PROPERTY(QUrl containerUrl READ containerUrl WRITE setContainerUrl)
    Q_PROPERTY(QGalleryItemList* items READ items NOTIFY itemsChanged)
public:
    explicit QGalleryItemRequest(QObject *parent = 0);
    explicit QGalleryItemRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryItemRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QStringList sortFields() const;
    void setSortFields(const QStringList &fields);

    bool isLive() const;
    void setLive(bool live);

    int initialCursorPosition() const;
    void setInitialCursorPosition(int index);

    int minimumPagedItems() const;
    void setMinimumPagedItems(int size);

    QString itemType() const;
    void setItemType(const QString &type);

    QGalleryFilter filter() const;
    void setFilter(const QGalleryFilter &filter);

    QString itemId() const;
    void setItemId(const QString &id);

    QStringList itemIds() const;
    void setItemIds(const QStringList &id);

    QUrl itemUrl() const;
    void setItemUrl(const QUrl &url);

    QList<QUrl> itemUrls() const;
    void setItemUrls(const QList<QUrl> &urls);

    QString containerId() const;
    void setContainerId(const QString &id);

    QUrl containerUrl() const;
    void setContainerUrl(const QUrl &url);

    QGalleryItemList *items() const;

Q_SIGNALS:
    void filterChanged();
    void itemIdsChanged();
    void itemUrlsChanged();
    void containerIdChanged();
    void containerUrlChanged();
    void itemsChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryCountRequestPrivate;

class Q_GALLERY_EXPORT QGalleryCountRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryCountRequest)
    Q_PROPERTY(bool live READ isLive WRITE setLive)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType)
    Q_PROPERTY(QGalleryFilter filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString containerId READ containerId WRITE setContainerId)
    Q_PROPERTY(QUrl containerUrl READ containerUrl WRITE setContainerUrl)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_ENUMS(Error)
public:
    explicit QGalleryCountRequest(QObject *parent = 0);
    explicit QGalleryCountRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryCountRequest();

    bool isLive() const;
    void setLive(bool live);

    QString itemType() const;
    void setItemType(const QString &type);

    QGalleryFilter filter() const;
    void setFilter(const QGalleryFilter &filter);

    QString containerId() const;
    void setContainerId(const QString &id);

    QUrl containerUrl() const;
    void setContainerUrl(const QUrl &url);

    int count() const;

Q_SIGNALS:
    void filterChanged();
    void containerIdChanged();
    void containerUrlChanged();
    void countChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_itemsChanged())
};

class QGalleryInsertRequestPrivate;

class Q_GALLERY_EXPORT QGalleryInsertRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryInsertRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QStringList sortFields READ sortFields WRITE setSortFields)
    Q_PROPERTY(QUrl itemUrl READ itemUrl WRITE setItemUrl NOTIFY itemUrlsChanged)
    Q_PROPERTY(QList<QUrl> itemUrls READ itemUrls WRITE setItemUrls NOTIFY itemUrlsChanged)
    Q_PROPERTY(QGalleryItemList *insertedItems READ insertedItems NOTIFY insertedItemsChanged)
public:
    explicit QGalleryInsertRequest(QObject *parent = 0);
    explicit QGalleryInsertRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryInsertRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QStringList sortFields() const;
    void setSortFields(const QStringList &fields);

    QUrl itemUrl() const;
    void setItemUrl(const QUrl &url);

    QList<QUrl> itemUrls() const;
    void setItemUrls(const QList<QUrl> &urls);

    QGalleryItemList *insertedItems() const;

Q_SIGNALS:
    void itemUrlsChanged();
    void insertedItemsChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryRemoveRequestPrivate;

class Q_GALLERY_EXPORT QGalleryRemoveRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryRemoveRequest)
    Q_PROPERTY(QString itemId READ itemId WRITE setItemId NOTIFY itemIdsChanged);
    Q_PROPERTY(QStringList itemIds READ itemIds WRITE setItemIds NOTIFY itemIdsChanged)
    Q_PROPERTY(QGalleryItemList *currentItem READ currentItem NOTIFY currentItemChanged)
public:
    explicit QGalleryRemoveRequest(QObject *parent = 0);
    explicit QGalleryRemoveRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryRemoveRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QString itemId() const;
    void setItemId(const QString &id);

    QStringList itemIds() const;
    void setItemIds(const QStringList &id);

    QGalleryItemList *currentItem() const;

Q_SIGNALS:
    void itemIdsChanged();
    void currentItemChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryCopyRequestPrivate;

class Q_GALLERY_EXPORT QGalleryCopyRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryCopyRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QString itemId READ itemId WRITE setItemId NOTIFY itemIdsChanged);
    Q_PROPERTY(QStringList itemIds READ itemIds WRITE setItemIds NOTIFY itemIdsChanged)
    Q_PROPERTY(QString destinationId READ destinationId WRITE setDestinationId)
    Q_PROPERTY(QGalleryItemList* currentItem READ currentItem NOTIFY currentItemChanged)
public:
    explicit QGalleryCopyRequest(QObject *parent = 0);
    explicit QGalleryCopyRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryCopyRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QString itemId() const;
    void setItemId(const QString &id);

    QStringList itemIds() const;
    void setItemIds(const QStringList &id);

    QString destinationId() const;
    void setDestinationId(const QString &id);

    QGalleryItemList *currentItem() const;

Q_SIGNALS:
    void itemIdsChanged();
    void currentItemChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryMoveRequestPrivate;

class Q_GALLERY_EXPORT QGalleryMoveRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryMoveRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QString itemId READ itemId WRITE setItemId NOTIFY itemIdsChanged);
    Q_PROPERTY(QStringList itemIds READ itemIds WRITE setItemIds NOTIFY itemIdsChanged)
    Q_PROPERTY(QString destinationId READ destinationId WRITE setDestinationId)
    Q_PROPERTY(QGalleryItemList* currentItem READ currentItem NOTIFY currentItemChanged)
public:
    QGalleryMoveRequest(QObject *parent = 0);
    QGalleryMoveRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryMoveRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QString itemId() const;
    void setItemId(const QString &id);

    QStringList itemIds() const;
    void setItemIds(const QStringList &id);

    QString destinationId() const;
    void setDestinationId(const QString &id);

    QGalleryItemList *currentItem() const;

Q_SIGNALS:
    void itemIdsChanged();
    void currentItemChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

#endif
