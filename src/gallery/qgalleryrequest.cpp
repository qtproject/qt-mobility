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

#include "qgalleryrequest.h"

#include "qgallery.h"

#include <QtCore/qstringlist.h>

class QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryAbstractRequest)
public:
    QGalleryAbstractRequestPrivate(QAbstractGallery *gallery, QGalleryAbstractRequest::Type type)
        : gallery(gallery)
        , response(0)
        , type(type)
        , state(QGalleryAbstractRequest::Inactive)
        , result(QGalleryAbstractRequest::NoResult)
        , currentProgress(0)
        , maximumProgress(0)
    {
    }

    virtual ~QGalleryAbstractRequestPrivate()
    {
        delete response;
    }

    void _q_finished();
    void _q_galleryDestroyed();
    void _q_progressChanged(int current, int maximum);

    QGalleryAbstractRequest *q_ptr;
    QAbstractGallery *gallery;
    QGalleryAbstractResponse *response;
    QGalleryAbstractRequest::Type type;
    QGalleryAbstractRequest::State state;
    int result;
    int currentProgress;
    int maximumProgress;
};

void QGalleryAbstractRequestPrivate::_q_finished()
{
    Q_Q(QGalleryAbstractRequest);

    if (result == QGalleryAbstractRequest::NoResult) {
        result = response->result();

        if (result == QGalleryAbstractRequest::NoResult)
            return;

        state = response->isIdle()
                ? QGalleryAbstractRequest::Idle
                : QGalleryAbstractRequest::Inactive;

        switch (result) {
        case QGalleryAbstractRequest::Succeeded:
            emit q->succeeded();
            break;
        case QGalleryAbstractRequest::Cancelled:
            emit q->cancelled();
            break;
        default:
            emit q->failed(result);
            break;
        }
        emit q->finished(result);
        emit q->resultChanged();
        emit q->stateChanged(state);
    } else if (state == QGalleryAbstractRequest::Idle && !response->isIdle()) {
        state = QGalleryAbstractRequest::Inactive;

        emit q->stateChanged(state);
    }
}

void QGalleryAbstractRequestPrivate::_q_galleryDestroyed()
{
    Q_Q(QGalleryAbstractRequest);

    gallery = 0;

    q->clear();

    emit q->supportedChanged();
}

void QGalleryAbstractRequestPrivate::_q_progressChanged(int current, int maximum)
{
    currentProgress = current;
    maximumProgress = maximum;

    emit q_func()->progressChanged(current, maximum);
}

/*!
    \class QGalleryAbstractRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryAbstractRequest class provides a base class for gallery
    requests.
*/

/*!
    \enum QGalleryAbstractRequest::State

    Identifies the state of a gallery request.

    \value Inactive The request has not been executed, or has finished.
    \value Active The request is currently executing.
    \value Cancelling The request was cancelled, but hasn't yet returned to the
    Inactive state.
    \value Idle The request has finished, and is monitoring its return values
    for changes.
*/

/*!
    \enum QGalleryAbstractRequest::Result

    Identifies the result of a gallery request.

    \value NoResult The request is still active and no result is available yet.
    \value Succeeded The request succeeded sucessfully.
    \value Cancelled The request was cancelled before it could finish.
    \value NoGallery The request cannot be executed because no \l gallery is
    set.
    \value NotSupported  The request isn't supported by the \l gallery.
    \value RequestError A minimum value for request specific error results.
*/

/*!
    \enum QGalleryAbstractRequest::Type

    Identifies the type of a request.

    \value Item The request is a QGalleryItemRequest.
    \value Insert The request is a QGalleryInsertRequest.
    \value Remove The request is a QGalleryRemoveRequest.
    \value Copy The request is a QGalleryCopyRequest.
    \value Move The request is a QGalleryMoveRequest.
*/

/*!
    Constructs a new gallery \a type request.

    The \a parent is passed to QObject.
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(Type type, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGalleryAbstractRequestPrivate(0, type))
{
    d_ptr->q_ptr = this;
}

/*!
    Constructs a new \a type request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(
        QAbstractGallery *gallery, Type type, QObject *parent)
    : QObject(parent)
    , d_ptr(new QGalleryAbstractRequestPrivate(gallery, type))
{
    d_ptr->q_ptr = this;
}

/*!
    \internal
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(
        QGalleryAbstractRequestPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&(dd))
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a gallery request.
*/

QGalleryAbstractRequest::~QGalleryAbstractRequest()
{
    delete d_ptr;
}

/*!
    \property QGalleryAbstractRequest::gallery

    \brief The gallery service a request acts on.
*/

QAbstractGallery *QGalleryAbstractRequest::gallery() const
{
    return d_ptr->gallery;
}

void QGalleryAbstractRequest::setGallery(QAbstractGallery *gallery)
{
    if (d_ptr->gallery != gallery) {
        if (d_ptr->gallery)
            disconnect(d_ptr->gallery, SIGNAL(destroyed()), this, SLOT(_q_galleryDestroyed()));

        d_ptr->gallery = gallery;

        if (d_ptr->gallery)
            connect(d_ptr->gallery, SIGNAL(destroyed()), this, SLOT(_q_galleryDestroyed()));

        clear();

        emit supportedChanged();
    }
}

/*!
    \property QGalleryAbstractRequest::supported

    \brief Whether a request is supported by its current \a gallery.
*/

bool QGalleryAbstractRequest::isSupported() const
{
    return d_ptr->gallery && d_ptr->gallery->isRequestSupported(d_ptr->type);
}

/*!
    \fn QGalleryAbstractRequest::supportedChanged()

    Signals the \l supported property has changed.
*/


/*!
    Returns the type of a request.
*/
QGalleryAbstractRequest::Type QGalleryAbstractRequest::type() const
{
    return d_ptr->type;
}

/*!
    \property QGalleryAbstractRequest::state

    \brief The state of a request.
*/

QGalleryAbstractRequest::State QGalleryAbstractRequest::state() const
{
    return d_ptr->state;
}

/*!
    \fn QGalleryAbstractRequest::stateChanged(QGalleryAbstractRequest::State state)

    Signals that the \a state of a request has changed.
*/

/*!
    \property QGalleryAbstractRequest::result

    \brief The result of a request.

    This will be a value of \l Result, or a request specific error value.
*/

int QGalleryAbstractRequest::result() const
{
    return d_ptr->result;
}

/*!
    \fn QGalleryAbstractRequest::resultChanged()

    Signals that the result of a request has changed.
*/

/*!
    \property QGalleryAbstractRequest::currentProgress

    \brief the current progress of a request.
*/

int QGalleryAbstractRequest::currentProgress() const
{
    return d_func()->currentProgress;
}

/*!
    \property QGalleryAbstractRequest::maximumProgress

    \brief the maximum value of \l currentProgress
*/

int QGalleryAbstractRequest::maximumProgress() const
{
    return d_func()->maximumProgress;
}

/*!
    \fn QGalleryAbstractRequest::progressChanged(int current, int maximum)

    Signals that the \a current or \a maximum progress value has changed.
*/

/*!
    Waits for \a msecs for the a request to finish.

    Returns true if the request has finished on return, and returns false if the
    wait time expires or the request is inactive or idle.
*/
bool QGalleryAbstractRequest::waitForFinished(int msecs)
{
    return d_ptr->response && d_ptr->state == Active
            ? d_ptr->response->waitForFinished(msecs)
            : true;
}

/*!
    Executes a request.

    \bold {Note}: A valid \l gallery must be set before a request can be
    executed.
*/

void QGalleryAbstractRequest::execute()
{
    if (!d_ptr->gallery) {
        d_ptr->result = NoGallery;

        emit failed(d_ptr->result);
        emit finished(d_ptr->result);
        emit resultChanged();
    } else {
        QGalleryAbstractResponse *oldResponse = d_ptr->response;
        int oldResult = d_ptr->result;
        State oldState = d_ptr->state;

        d_ptr->response = d_ptr->gallery->createResponse(this);

        if (d_ptr->response) {
            d_ptr->result = d_ptr->response->result();

            if (d_ptr->result == NoResult)
                d_ptr->state = Active;
            else if (d_ptr->response->isIdle())
                d_ptr->state = Idle;
            else
                d_ptr->state = Inactive;

            connect(d_ptr->response, SIGNAL(finished()), this, SLOT(_q_finished()));
            connect(d_ptr->response, SIGNAL(progressChanged(int,int)),
                    this, SLOT(_q_progressChanged(int,int)));

            setResponse(d_ptr->response);
        } else {
            d_ptr->result = NotSupported;
            d_ptr->state = Inactive;

            if (oldResponse)
                setResponse(0);
        }

        delete oldResponse;

        if (d_ptr->currentProgress != 0 || d_ptr->maximumProgress != 0) {
            d_ptr->currentProgress = 0;
            d_ptr->maximumProgress = 0;

            emit progressChanged(0, 0);
        }

        switch (d_ptr->result) {
        case QGalleryAbstractRequest::NoResult:
            break;
        case QGalleryAbstractRequest::Cancelled:
            break;
        case QGalleryAbstractRequest::Succeeded:
            emit succeeded();
            emit finished(d_ptr->result);
            break;
        default:
            emit failed(d_ptr->result);
            emit finished(d_ptr->result);
            break;
        }

        if (d_ptr->result != oldResult)
            emit resultChanged();

        if (d_ptr->state != oldState)
            emit stateChanged(d_ptr->state);
    }
}

/*!
    Cancels the execution of a request.  If the request is idle this will stop
    any future updates.
*/

void QGalleryAbstractRequest::cancel()
{
    switch (d_ptr->state) {
    case Active:
        if (d_ptr->response) {
            d_ptr->state = Cancelling;
            d_ptr->response->cancel();

            if (d_ptr->state == Cancelling)
                emit stateChanged(d_ptr->state);
        }
        break;
    case Idle:
        if (d_ptr->response)
            d_ptr->response->cancel();
        break;
    default:
        break;
    }
}

/*!
    Clears the results of a request.

    If the request is active or idle it will be cancelled.
*/

void QGalleryAbstractRequest::clear()
{
    if (d_ptr->response) {
        bool wasFinished = d_ptr->result != NoResult;
        bool wasActive = d_ptr->state != Inactive;

        QGalleryAbstractResponse *oldResponse = d_ptr->response;

        d_ptr->response = 0;
        d_ptr->result = NoResult;
        d_ptr->state = Inactive;

        setResponse(0);

        delete oldResponse;

        if (wasFinished && d_ptr->result == NoResult)
            emit resultChanged();

        if (wasActive && d_ptr->state == Inactive)
            emit stateChanged(d_ptr->state);
    }
}

/*!
    \fn QGalleryAbstractRequest::succeeded()

    Signals that a request has completed successfully.
*/

/*!
    \fn QGalleryAbstractRequest::cancelled()

    Signals that a request was cancelled before it could finish.
*/

/*!
    \fn QGalleryAbstractRequest::failed(int result)

    Signals that a request failed with the given \a result.
*/

/*!
    \fn QGalleryAbstractRequest::finished(int result)

    Signals that a request has finished with the given \a result.
*/

/*!
    \fn QGalleryAbstractRequest::setResponse(QGalleryAbstractResponse *response)

    Sets the \a response to an executed request.
*/

class QGalleryAbstractResponsePrivate
{
public:
    QGalleryAbstractResponsePrivate()
        : result(QGalleryAbstractRequest::NoResult)
        , itemCount(0)
        , idle(false)
    {
    }

    virtual ~QGalleryAbstractResponsePrivate() {}

    int result;
    int itemCount;
    bool idle;
};

/*!
    \class QGalleryAbstractResponse

    \ingroup gallery

    \brief The QGalleryAbstractResponse class provides a base class for
    responses to gallery requests.
*/

/*!
    Constructs a new gallery response.

    The \a parent is passed to QObject.
*/

QGalleryAbstractResponse::QGalleryAbstractResponse(QObject *parent)
    : QGalleryItemList(parent)
    , d_ptr(new QGalleryAbstractResponsePrivate)
{
}

/*!
    Destroys a gallery response.
*/

QGalleryAbstractResponse::~QGalleryAbstractResponse()
{
    delete d_ptr;
}

/*!
    Identifies if the items returned by a response are being monitored for
    changes.

    Returns true if a response is in an idle state, and false otherwise.
*/
bool QGalleryAbstractResponse::isIdle() const
{
    return d_ptr->idle;
}

/*!
    Returns the result of a gallery request.
*/

int QGalleryAbstractResponse::result() const
{
    return d_ptr->result;
}

/*!
    Returns the number of items affected by a gallery response.
*/

int QGalleryAbstractResponse::itemCount() const
{
    return d_ptr->itemCount;
}

/*!
    Updates the \a count of items affected by a gallery response.
*/

void QGalleryAbstractResponse::updateItemCount(int count)
{
    if (d_ptr->itemCount != count) {
        d_ptr->itemCount = count;

        emit itemCountChanged();
    }
}

/*!
    \fn QGalleryAbstractResponse::waitForFinished(int msecs)

    Waits for \a msecs for the a response to finish.

    Returns true if the response has finished on return, and returns false if
    the wait time expires or the request is inactive or idle.
*/

/*!
    \fn QGalleryAbstractResponse::itemCountChanged()

    Signals that the number of items related to a gallery response
    has changed.
*/

/*!
    Cancels an active or idle gallery response.

    The default implementation finishes the an active response with the
    \l QGalleryAbstractRequest::Cancelled result.  If the reponse is idle the
    \l finished() signal will be re-emitted with idle
*/

void QGalleryAbstractResponse::cancel()
{
    if (d_ptr->result == QGalleryAbstractRequest::NoResult) {
        d_ptr->result = QGalleryAbstractRequest::Cancelled;
        d_ptr->idle = false;

        emit finished();
    } else if (d_ptr->idle) {
        d_ptr->idle = false;

        emit finished();
    }
}

/*!
    Finalizes a gallery response, and sets the \a result.

    If \a idle is true the items returned by a response will be monitored
    for changes and updated as appropriate.
*/

void QGalleryAbstractResponse::finish(int result, bool idle)
{
    if (d_ptr->result == QGalleryAbstractRequest::NoResult
            && result != QGalleryAbstractRequest::NoResult) {
        d_ptr->result = result;
        d_ptr->idle = idle;

        emit finished();
    }
}

/*!
    \fn QGalleryAbstractResponse::finished(int result, bool idle)

    Signals that a response has finished with the given \a result.

    If \a idle is true the items returned by a response will be monitored
    for changes and updated as appropriate.
*/

class QGalleryItemRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryItemRequest)
public:
    QGalleryItemRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Item)
        , startIndex(0)
        , maximumCount(-1)
        , totalCount(0)
        , live(false)
    {
    }

    int startIndex;
    int maximumCount;
    int totalCount;
    bool live;
    QStringList fields;
    QStringList sortFields;
    QString itemType;
    QGalleryFilter filter;
};

/*!
    \class QGalleryItemRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryItemRequest class provides a request for a set of
    items from a gallery.
*/

/*!
    \enum QGalleryItemRequest::Error

    Enumerates item request specific error results.

    \value InvalidFilter
*/

/*!
    Constructs a new gallery item request.

    The \a parent is passed to QObject.
*/

QGalleryItemRequest::QGalleryItemRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryItemRequestPrivate(0), parent)
{
}

/*!
    Contructs a new item request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryItemRequest::QGalleryItemRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryItemRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery item request.
*/

QGalleryItemRequest::~QGalleryItemRequest()
{
}

/*!
    \property QGalleryItemRequest::fields

    \brief A list of meta-data fields a request should return values for.
*/

QStringList QGalleryItemRequest::fields() const
{
    return d_func()->fields;
}

void QGalleryItemRequest::setFields(const QStringList &fields)
{
    d_func()->fields = fields;
}

/*!
    \property QGalleryItemRequest::sortFields

    \brief A list of meta-data fields a request should sort its results on.

    Prefixing a field name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryItemRequest::sortFields() const
{
    return d_func()->sortFields;
}

void QGalleryItemRequest::setSortFields(const QStringList &fields)
{
    d_func()->sortFields = fields;
}

/*!
    \property QGalleryItemRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/

bool QGalleryItemRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryItemRequest::setLive(bool live)
{
    d_func()->live = live;
}

/*!
    \property QGalleryItemRequest::startIndex

    \brief The index of the first item a request should return.

    By default this is 0.
*/

int QGalleryItemRequest::startIndex() const
{
    return d_func()->startIndex;
}

void QGalleryItemRequest::setStartIndex(int index)
{
    d_func()->startIndex = index;
}

/*!
    \property QGalleryItemRequest::maximumCount

    \brief The maximum number of items that should be returned by a request.

    If this is -1 the results will be unbounded.

    By default this is -1.
*/

int QGalleryItemRequest::maximumCount() const
{
    return d_func()->maximumCount;
}

void QGalleryItemRequest::setMaximumCount(int count)
{
    d_func()->maximumCount = count;
}

/*!
    \property QGalleryItemRequest::itemType

    \brief The type of items a request should return.

    If this is not set items of all types will be returned.  If no filter is
    set all items of this type will be returned.
*/

QString QGalleryItemRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryItemRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}

/*!
    \property QGalleryItemRequest::filter

    \brief A filter identifying the items a request should return.

    If no filter is set the results of the request will be determined soley
    by the \l itemType property.
*/

QGalleryFilter QGalleryItemRequest::filter() const
{
    return d_func()->filter;
}

void QGalleryItemRequest::setFilter(const QGalleryFilter &filter)
{
    Q_D(QGalleryItemRequest);

    QGalleryFilter::Type oldType = d->filter.type();

    d->filter = filter;

    emit filterChanged();

    switch (oldType) {
    case QGalleryFilter::Item:
        emit itemIdsChanged();
        break;
    case QGalleryFilter::ItemUrl:
        emit itemUrlsChanged();
        break;
    case QGalleryFilter::Container:
        emit containerIdChanged();
        break;
    case QGalleryFilter::ContainerUrl:
        emit containerUrlChanged();
        break;
    default:
        break;
    }

    QGalleryFilter::Type newType = d->filter.type();

    if (oldType != newType) {
        switch (newType) {
        case QGalleryFilter::Item:
            emit itemIdsChanged();
            break;
        case QGalleryFilter::ItemUrl:
            emit itemUrlsChanged();
            break;
        case QGalleryFilter::Container:
            emit containerIdChanged();
            break;
        case QGalleryFilter::ContainerUrl:
            emit containerUrlChanged();
            break;
        default:
            break;
        }
    }
}

/*!
    \fn QGalleryItemRequest::filterChanged()

    Signals the \l filter property has changed.
*/

/*!
    \property QGalleryItemRequest::itemId

    \brief The ID of a item a item request should return.

    This is equivalent to setting a \l filter of type QGalleryItemFilter.

    If the current \l filter is not of type QGalleryItemFilter or multiple
    items are specified in \l itemIds this will be null.
*/

QString QGalleryItemRequest::itemId() const
{
    return d_func()->filter.toItemFilter().itemId();
}

void QGalleryItemRequest::setItemId(const QString &id)
{
    setFilter(QGalleryItemFilter(id));
}

/*!
    \property QGalleryItemRequest::itemIds

    \brief A list of IDs for items a item request should return.

    This is equivalent to setting a \l filter of type QGalleryItemFilter.

    If the current \l filter is not of type QGalleryItemFilter this will be
    null.
*/

QStringList QGalleryItemRequest::itemIds() const
{
    return d_func()->filter.toItemFilter().itemIds();
}

void QGalleryItemRequest::setItemIds(const QStringList &ids)
{
    setFilter(QGalleryItemFilter(ids));
}

/*!
    \fn QGalleryItemRequest::itemIdsChanged()

    Signals that the itemId and \l itemIds properties have changed.
*/

/*!
    \property QGalleryItemRequest::itemUrl

    \brief The URL of an item a item request should return.

    This is equivalent to setting a \l filter of type QGalleryItemUrlFilter.

    If the current \l filter is not of type QGalleryItemUrlFilter or
    multiple items are specified in \l itemUrls this will be null.
*/

QUrl QGalleryItemRequest::itemUrl() const
{
    return d_func()->filter.toItemUrlFilter().itemUrl();
}

void QGalleryItemRequest::setItemUrl(const QUrl &url)
{
    setFilter(QGalleryItemUrlFilter(url));
}

/*!
    \property QGalleryItemRequest::itemUrls

    \brief A list of URLs for items a item request should return.

    This is equivalent to setting a \l filter of type QGalleryItemUrlFilter.

    If the current \l filter is not of type QGalleryItemUrlFilter this will
    be null.
*/

QList<QUrl> QGalleryItemRequest::itemUrls() const
{
    return d_func()->filter.toItemUrlFilter().itemUrls();
}

void QGalleryItemRequest::setItemUrls(const QList<QUrl> &urls)
{
    setFilter(QGalleryItemUrlFilter(urls));
}

/*!
    \fn QGalleryItemRequest::itemUrlsChanged()

    Signals that the \l itemUrl and \l itemUrls properties have
    changed.
*/

/*!
    \property QGalleryItemRequest::containerId

    \brief The ID of container item a item request should return the
    contents of.

    This is equivalent to setting a \l filter of type QGalleryContainerFilter.

    If the current \l filter is not of type QGalleryContainerFilter this will be
    null.
*/

QString QGalleryItemRequest::containerId() const
{
    return d_func()->filter.toContainerFilter().containerId();
}

void QGalleryItemRequest::setContainerId(const QString &id)
{
    setFilter(QGalleryContainerFilter(id));
}

/*!
    \fn QGalleryItemRequest::containerIdChanged()

    Signals that the \l containerId property has changed.
*/

/*!
    \property QGalleryItemRequest::containerUrl

    \brief The URL of container item a item request should return the
    contents of.

    This is equivalent to setting a \l filter of type
    QGalleryContainerUrlFilter.

    If the current \l filter is not of type QGalleryContainerUrlFilter this
    will be null.
*/

QUrl QGalleryItemRequest::containerUrl() const
{
    return d_func()->filter.toContainerUrlFilter().containerUrl();
}

void QGalleryItemRequest::setContainerUrl(const QUrl &url)
{
    setFilter(QGalleryContainerUrlFilter(url));
}

/*!
    \fn QGalleryItemRequest::containerUrlChanged()

    Signals that the \l containerUrl property has changed.
*/

/*!
    \property QGalleryItemRequest::items

    \brief The items returned by a request.
*/

QGalleryItemList *QGalleryItemRequest::items() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryItemRequest::itemsChanged()

    Signals that the \l items property has changed.
*/

/*!
    \reimp
*/

void QGalleryItemRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemsChanged();
}

class QGalleryInsertRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryInsertRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Insert)
    {
    }

    QStringList fields;
    QStringList sortFields;
    QList<QUrl> itemUrls;
};

/*!
    \class QGalleryInsertRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryInsertRequest class provides a request which inserts
    existing items into a gallery.
*/

/*!
    Constructs a new gallery insert request.

    The \a parent is passed to QObject.
*/

QGalleryInsertRequest::QGalleryInsertRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryInsertRequestPrivate(0), parent)
{
}

/*!
    Contructs a new insert request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryInsertRequest::QGalleryInsertRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryInsertRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery insert request.
*/

QGalleryInsertRequest::~QGalleryInsertRequest()
{

}

/*!
    \property QGalleryInsertRequest::fields

    \brief A list of meta-data fields the \l insertedItems list should contain values for.
*/

QStringList QGalleryInsertRequest::fields() const
{
    return d_func()->fields;
}

void QGalleryInsertRequest::setFields(const QStringList &fields)
{
    d_func()->fields = fields;
}

/*!
    \property QGalleryInsertRequest::sortFields

    \brief A list of meta-data fields a request should sort its results on.

    Prefixing a field name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryInsertRequest::sortFields() const
{
    return d_func()->sortFields;
}

void QGalleryInsertRequest::setSortFields(const QStringList &fields)
{
    d_func()->sortFields = fields;
}

/*!
    \property QGalleryInsertRequest::itemUrl

    \brief The URL of an item to insert into a gallery.

    This is equivalent to \a itemUrls with a single value. If multiple
    \a itemUrls are set this will be null.
*/

QUrl QGalleryInsertRequest::itemUrl() const
{
    Q_D(const QGalleryInsertRequest);

    return d->itemUrls.count() == 1
            ? d->itemUrls.first()
            : QUrl();
}

void QGalleryInsertRequest::setItemUrl(const QUrl &url)
{
    Q_D(QGalleryInsertRequest);

    d->itemUrls.clear();

    if (!url.isEmpty())
        d->itemUrls.append(url);

    emit itemUrlsChanged();
}

/*!
    \property QGalleryInsertRequest::itemUrls

    \brief A list of URLs to insert into a gallery.

    If the list only contains one URL this is equivalent to \l itemUrl.
*/

QList<QUrl> QGalleryInsertRequest::itemUrls() const
{
    return d_func()->itemUrls;
}

void QGalleryInsertRequest::setItemUrls(const QList<QUrl> &urls)
{
    d_func()->itemUrls = urls;

    emit itemUrlsChanged();
}

/*!
    \fn QGalleryInsertRequest::itemUrlsChanged()

    Signals the \l itemUrl and \l itemUrls properties have changed.
*/

/*!
    \property QGalleryInsertRequest::insertedItems

    \brief A list of items inserted into a gallery.
*/

QGalleryItemList *QGalleryInsertRequest::insertedItems() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryInsertRequest::insertedItemsChanged()

    Signals the \l insertedItems property has changed.
*/

/*!
    \reimp
*/

void QGalleryInsertRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit insertedItemsChanged();
}

class QGalleryRemoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryRemoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Remove)
    {
    }

    QStringList fields;
    QStringList itemIds;
};

/*!
    \class QGalleryRemoveRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryRemoveRequest class provides a request which removes
    items from a gallery.
*/

/*!
    Constructs a new gallery remove request.

    The \a parent is passed to QObject.
*/

QGalleryRemoveRequest::QGalleryRemoveRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryRemoveRequestPrivate(0), parent)
{

}

/*!
    Contructs a new remove request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryRemoveRequest::QGalleryRemoveRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryRemoveRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery remove request.
*/

QGalleryRemoveRequest::~QGalleryRemoveRequest()
{
}

/*!
    \property QGalleryRemoveRequest::fields

    \brief A list of meta-data fields the \l currentItem list should contain
    values for.
*/

QStringList QGalleryRemoveRequest::fields() const
{
    return d_func()->fields;
}

void QGalleryRemoveRequest::setFields(const QStringList &fields)
{
    d_func()->fields = fields;
}

/*!
    \property QGalleryRemoveRequest::itemId

    \brief The ID of an item to remove from a gallery.

    This is equivalent to \l itemIds with a single ID.  If there are
    multiple \l itemIds this will be null.
*/

QString QGalleryRemoveRequest::itemId() const
{
    Q_D(const QGalleryRemoveRequest);

    return d->itemIds.count() == 1
            ? d->itemIds.first()
            : QString();
}

void QGalleryRemoveRequest::setItemId(const QString &id)
{
    Q_D(QGalleryRemoveRequest);

    d->itemIds.clear();

    if (!id.isNull())
        d->itemIds.append(id);

    emit itemIdsChanged();
}

/*!
    \fn QGalleryRemoveRequest::itemIdChanged()

    Signals the \l itemId property has changed.
*/

/*!
    \property QGalleryRemoveRequest::itemIds

    \brief A list of IDs of items to remove from a gallery.

    If the list only contains one ID this is equivalent to \l itemId.
*/

QStringList QGalleryRemoveRequest::itemIds() const
{
    return d_func()->itemIds;
}

void QGalleryRemoveRequest::setItemIds(const QStringList &ids)
{
    d_func()->itemIds = ids;

    emit itemIdsChanged();
}

/*!
    \fn QGalleryRemoveRequest::itemIdsChanged()

    Signals that the itemId and itemIds properties have changed.
*/

/*!
    \property QGalleryRemoveRequest::currentItem

    \brief A list containing the current item being removed by a request.
*/

QGalleryItemList *QGalleryRemoveRequest::currentItem() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryRemoveRequest::currentItemChanged()

    Signals the \l currentItem property has changed.
*/

/*!
    \reimp
*/

void QGalleryRemoveRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit currentItemChanged();
}

class QGalleryCopyRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryCopyRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Copy)
    {
    }

    QStringList fields;
    QStringList itemIds;
    QString destinationId;
};

/*!
    \class QGalleryCopyRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryCopyRequest class provides a request which copies
    items within a gallery.
*/

/*!
    Constructs a new gallery copy request.

    The \a parent is passed to QObject.
*/

QGalleryCopyRequest::QGalleryCopyRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryCopyRequestPrivate(0), parent)
{
}

/*!
    Contructs a new copy request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryCopyRequest::QGalleryCopyRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryCopyRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery copy request.
*/

QGalleryCopyRequest::~QGalleryCopyRequest()
{
}

/*!
    \property QGalleryCopyRequest::fields

    \brief A list of meta-data fields the \l currentItem list should contain
    values for.
*/

QStringList QGalleryCopyRequest::fields() const
{
    return d_func()->fields;
}

void QGalleryCopyRequest::setFields(const QStringList &fields)
{
    d_func()->fields = fields;
}

/*!
    \property QGalleryCopyRequest::itemId

    \brief The ID of an item to copy within a gallery.

    This is equivalent to \l itemIds with a single ID.  If there are
    multiple \l itemIds this will be null.
*/

QString QGalleryCopyRequest::itemId() const
{
    Q_D(const QGalleryCopyRequest);

    return d->itemIds.count() == 1
            ? d->itemIds.first()
            : QString();
}

void QGalleryCopyRequest::setItemId(const QString &id)
{
    Q_D(QGalleryCopyRequest);

    d->itemIds.clear();

    if (!id.isNull())
        d->itemIds.append(id);

    emit itemIdsChanged();
}

QStringList QGalleryCopyRequest::itemIds() const
{
    return d_func()->itemIds;
}

/*!
    \property QGalleryCopyRequest::itemIds

    \brief A list of IDs of items to copy within a item gallery.

    If the list only contains one ID this is equivalent to \l itemId.
*/

void QGalleryCopyRequest::setItemIds(const QStringList &ids)
{
    d_func()->itemIds = ids;

    emit itemIdsChanged();
}

/*!
    \fn QGalleryCopyRequest::itemIdsChanged()

    Signals the \l itemId and \l itemIds properties have changed.
*/

/*!
    \property QGalleryCopyRequest::destinationId

    \brief The ID of the container item a request will copy items to.
*/

QString QGalleryCopyRequest::destinationId() const
{
    return d_func()->destinationId;
}

void QGalleryCopyRequest::setDestinationId(const QString &id)
{
    d_func()->destinationId = id;
}

/*!
    \property QGalleryCopyRequest::currentItem

    \brief A list containing the item currently being copied by a request.
*/

QGalleryItemList *QGalleryCopyRequest::currentItem() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryCopyRequest::currentItemChanged()

    Signals the \l currentItem property has changed.
*/

/*!
    \reimp
*/

void QGalleryCopyRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit currentItemChanged();
}

class QGalleryMoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryMoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Move)
    {
    }

    QStringList fields;
    QStringList itemIds;
    QString destinationId;
};

/*!
    \class QGalleryMoveRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryMoveRequest class provides a request which moves
    items within a gallery.
*/

/*!
    Constructs a new gallery move request.

    The \a parent is passed to QObject.
*/

QGalleryMoveRequest::QGalleryMoveRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryMoveRequestPrivate(0), parent)
{
}

/*!
    Contructs a new move request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryMoveRequest::QGalleryMoveRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryMoveRequestPrivate(gallery), parent)
{
}

/*!
*/

QGalleryMoveRequest::~QGalleryMoveRequest()
{
}

/*!
    \property QGalleryMoveRequest::fields

    \brief A list of meta-data fields the \l currentItem list should contain
    values for.
*/

QStringList QGalleryMoveRequest::fields() const
{
    return d_func()->fields;
}

void QGalleryMoveRequest::setFields(const QStringList &fields)
{
    d_func()->fields = fields;
}

/*!
    \property QGalleryMoveRequest::itemId

    \brief The ID of an item to copy within a gallery.

    This is equivalent to \l itemIds with a single ID.  If there are
    multiple \l itemIds this will be null.
*/

QString QGalleryMoveRequest::itemId() const
{
    Q_D(const QGalleryMoveRequest);

    return d->itemIds.count() == 1
            ? d->itemIds.first()
            : QString();
}

void QGalleryMoveRequest::setItemId(const QString &id)
{
    Q_D(QGalleryMoveRequest);

    d->itemIds.clear();

    if (!id.isNull())
        d->itemIds.append(id);

    emit itemIdsChanged();
}

/*!
    \property QGalleryMoveRequest::itemIds

    \brief A list of IDs of items to copy within a gallery.

    If the list only contains one ID this is equivalent to \l itemId.
*/

QStringList QGalleryMoveRequest::itemIds() const
{
    return d_func()->itemIds;
}

void QGalleryMoveRequest::setItemIds(const QStringList &ids)
{
    d_func()->itemIds = ids;

    emit itemIdsChanged();
}

/*!
    \fn QGalleryMoveRequest::itemIdsChanged()

    Signals the \l itemId and \l itemIds properties have changed.
*/

/*!
    \property QGalleryMoveRequest::destinationId

    \brief The ID of the container item a request will move items to.
*/

QString QGalleryMoveRequest::destinationId() const
{
    return d_func()->destinationId;
}

void QGalleryMoveRequest::setDestinationId(const QString &id)
{
    d_func()->destinationId = id;
}

/*!
    \property QGalleryMoveRequest::currentItem

    \brief A list containing the current item being moved by a request.
*/

QGalleryItemList *QGalleryMoveRequest::currentItem() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryMoveRequest::currentItemChanged()

    Signals the \l currentItem property has changed.
*/

/*!
    \reimp
*/

void QGalleryMoveRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit currentItemChanged();
}

#include "moc_qgalleryrequest.cpp"
