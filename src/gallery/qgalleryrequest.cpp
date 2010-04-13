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

#include "qgalleryitemlist_p.h"

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
    \value NotSupported The request isn't supported by the \l gallery.
    \value ConnectionError The request could not be executed due to a problem
    connecting to the gallery.
    \value InvalidItemError The request could not be executed due to an invalid
    item ID.
    \value ItemTypeError The request could not be executed due to an invalid
    item type.
    \value InvalidPropertyError The request could not be executed because a
    filter referenced an invalid property.
    \value PropertyTypeError The request could not be executed because a filter
    compared a property to an invalid variant type.
    \value UnsupportedFilterTypeError The request could not be executed because
    a filter is not supported by the gallery.
    \value UnsupportedFilterOptionError The request could not be executed
    because an option on a filter is not supported by the gallery.
    \value PermissionsError The request could not be executed because the
    user has insufficient permissions.
    \value InvalidDestinationError  The request could not be executed because
    the given destination ID is invalid.
    \value InvalidUrlError The request could not be executed due to an invalid
    URL.
    \value RequestError A minimum value for request specific error results.
*/

/*!
    \enum QGalleryAbstractRequest::Type

    Identifies the type of a request.

    \value Item The request is a QGalleryItemRequest.
    \value Url The request is a QGalleryUrlRequest.
    \value Container The request is a QGalleryContainerRequest.
    \value Filter The request is a QGalleryFilterRequest.
    \value Count The request is a QGalleryCountRequest.
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

class QGalleryAbstractResponsePrivate : public QGalleryItemListPrivate
{
public:
    QGalleryAbstractResponsePrivate()
        : result(QGalleryAbstractRequest::NoResult)
        , idle(false)
    {
    }
    int result;
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
    : QGalleryItemList(*new QGalleryAbstractResponsePrivate, parent)
{
}

/*!
    \internal
*/

QGalleryAbstractResponse::QGalleryAbstractResponse(
        QGalleryAbstractResponsePrivate &dd, QObject *parent)
    : QGalleryItemList(dd, parent)
{
}

/*!
    Destroys a gallery response.
*/

QGalleryAbstractResponse::~QGalleryAbstractResponse()
{
}

/*!
    Identifies if the items returned by a response are being monitored for
    changes.

    Returns true if a response is in an idle state, and false otherwise.
*/
bool QGalleryAbstractResponse::isIdle() const
{
    return d_func()->idle;
}

/*!
    Returns the result of a gallery request.
*/

int QGalleryAbstractResponse::result() const
{
    return d_func()->result;
}

/*!
    \fn QGalleryAbstractResponse::waitForFinished(int msecs)

    Waits for \a msecs for the a response to finish.

    Returns true if the response has finished on return, and returns false if
    the wait time expires or the request is inactive or idle.
*/

/*!
    Cancels an active or idle gallery response.

    The default implementation finishes the an active response with the
    \l QGalleryAbstractRequest::Cancelled result.  If the reponse is idle the
    \l finished() signal will be re-emitted with idle
*/

void QGalleryAbstractResponse::cancel()
{
    Q_D(QGalleryAbstractResponse);

    if (d->result == QGalleryAbstractRequest::NoResult) {
        d->result = QGalleryAbstractRequest::Cancelled;
        d->idle = false;

        emit finished();
    } else if (d->idle) {
        d->idle = false;

        emit finished();
    }
}

/*!
    \fn QGalleryAbstractResponse::progressChanged(int current, int maximum)

    Signals that the \a current or \a maximum progress of a request has
    changed.
*/

/*!
    Finalizes a gallery response, and sets the \a result.

    If \a idle is true the items returned by a response will be monitored
    for changes and updated as appropriate.
*/

void QGalleryAbstractResponse::finish(int result, bool idle)
{
    Q_D(QGalleryAbstractResponse);

    if (d->result == QGalleryAbstractRequest::NoResult
            && result != QGalleryAbstractRequest::NoResult) {
        d->result = result;
        d->idle = idle;

        emit finished();
    }
}

/*!
    \fn QGalleryAbstractResponse::finished()

    Signals that a response has finished.
*/

class QGalleryItemRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryItemRequest)
public:
    QGalleryItemRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Item)
        , live(false)
    {
    }

    bool live;
    QStringList propertyNames;
    QString itemId;
};

/*!
    \class QGalleryItemRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryItemRequest class provides a request for an item from a
    gallery.
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
    \property QGalleryItemRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/

QStringList QGalleryItemRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryItemRequest::setPropertyNames(const QStringList &propertyNames)
{
    d_func()->propertyNames = propertyNames;
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
    \property QGalleryItemRequest::itemId

    \brief The ID of a item a item request should return.
*/

QString QGalleryItemRequest::itemId() const
{
    return d_func()->itemId;
}

void QGalleryItemRequest::setItemId(const QString &id)
{
    d_func()->itemId = id;
}

/*!
    \property QGalleryItemRequest::item

    \brief The item returned by a request.
*/

QGalleryItemList *QGalleryItemRequest::item() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryItemRequest::itemChanged()

    Signals that the \l item property has changed.
*/

/*!
    \reimp
*/

void QGalleryItemRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemChanged();
}


class QGalleryUrlRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryUrlRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Url)
        , live(false)
    {
    }

    bool live;
    bool create;
    QStringList propertyNames;
    QUrl itemUrl;
};

/*!
    \class QGalleryUrlRequest

    \brief The QGalleryItemRequest class provides a request for an item
    identified by a URL from a gallery.
*/

/*!
    Constructs a new gallery url request.

    The \a parent is passed to QObject.
*/

QGalleryUrlRequest::QGalleryUrlRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryUrlRequestPrivate(0), parent)
{
}

/*!
    Contructs a new url request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryUrlRequest::QGalleryUrlRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryUrlRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery url request.
*/

QGalleryUrlRequest::~QGalleryUrlRequest()
{
}

/*!
    \property QGalleryUrlRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/

QStringList QGalleryUrlRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryUrlRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryUrlRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/

bool QGalleryUrlRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryUrlRequest::setLive(bool live)
{
    d_func()->live = live;
}

/*!
    \property QGalleryUrlRequest::itemUrl

    \brief The url of the item a request should return.
*/

QUrl QGalleryUrlRequest::itemUrl() const
{
    return d_func()->itemUrl;
}

void QGalleryUrlRequest::setItemUrl(const QUrl &url)
{
    d_func()->itemUrl = url;
}

/*!
    \property QGalleryUrlRequest::create

    \biref Whether a gallery should create a new entry for the requested item
    if one does not already exist.
*/

bool QGalleryUrlRequest::create() const
{
    return d_func()->create;
}

void QGalleryUrlRequest::setCreate(bool create)
{
    d_func()->create = create;
}

/*!
    \property QGalleryUrlRequest::item

    \brief A list containing the item returned by the request.
*/

QGalleryItemList *QGalleryUrlRequest::item() const
{
    return d_func()->response;
}

/*!
    \fn void QGalleryUrlRequest::itemChanged()

    Signals that the \l item property has changed.
*/

void QGalleryUrlRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemChanged();
}

class QGalleryContainerRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryItemRequest)
public:
    QGalleryContainerRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Container)
        , initialCursorPosition(0)
        , minimumPagedItems(200)
        , live(false)
    {
    }

    int initialCursorPosition;
    int minimumPagedItems;
    bool live;
    QStringList propertyNames;
    QStringList sortPropertyNames;
    QString itemType;
    QString containerId;
};

/*!
    \class QGalleryContainerRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryContainerRequest class provides a request for the
    immediate children of an item in a gallery.
*/

/*!
    Constructs a new gallery container request.

    The \a parent is passed to QObject.
*/


QGalleryContainerRequest::QGalleryContainerRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryContainerRequestPrivate(0), parent)
{
}
/*!
    Contructs a new container request for the given \a gallery.

    The \a parent is passed to QObject.
*/


QGalleryContainerRequest::QGalleryContainerRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryContainerRequestPrivate(gallery), parent)
{
}
/*!
    Destroys a gallery container request.
*/


QGalleryContainerRequest::~QGalleryContainerRequest()
{
}
/*!
    \property QGalleryContainerRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/


QStringList QGalleryContainerRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryContainerRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryContainerRequest::sortPropertyNames

    \brief A list of names of meta-data properties a request should sort its
    results on.

    Prefixing a property name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryContainerRequest::sortPropertyNames() const
{
    return d_func()->sortPropertyNames;
}

void QGalleryContainerRequest::setSortPropertyNames(const QStringList &names)
{
    d_func()->sortPropertyNames = names;
}
/*!
    \property QGalleryContainerRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/


bool QGalleryContainerRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryContainerRequest::setLive(bool live)
{
    d_func()->live = live;
}
/*!
    \property QGalleryContainerRequest::initialCursorPosition

    \brief The index of the first item a request should return.

    By default this is 0.
*/

int QGalleryContainerRequest::initialCursorPosition() const
{
    return d_func()->initialCursorPosition;
}

void QGalleryContainerRequest::setInitialCursorPosition(int index)
{
    d_func()->initialCursorPosition = index;
}
/*!
    \property QGalleryContainerRequest::minimumPagedItems

    \brief The minimum number of consecutive items the list returned by a
    request should cache.

    By default this is 200.
*/


int QGalleryContainerRequest::minimumPagedItems() const
{
    return d_func()->minimumPagedItems;
}

void QGalleryContainerRequest::setMinimumPagedItems(int size)
{
    d_func()->minimumPagedItems = size;
}
/*!
    \property QGalleryContainerRequest::itemType

    \brief The type of items a request should return.

    If this is not set items of all types will be returned.  If no filter is
    set all items of this type will be returned.
*/


QString QGalleryContainerRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryContainerRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}
/*!
    \property QGalleryContainerRequest::containerId

    \brief The ID of container item a item request should return the
    immediate children of.
*/


QString QGalleryContainerRequest::containerId() const
{
    return d_func()->containerId;
}

void QGalleryContainerRequest::setContainerId(const QString &id)
{
    d_func()->containerId = id;
}

/*!
    \property QGalleryContainerRequest::items

    \brief The items returned by a request.
*/

QGalleryItemList *QGalleryContainerRequest::items() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryContainerRequest::itemsChanged()
*/

/*!
    \reimp
*/
void QGalleryContainerRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemsChanged();
}

class QGalleryFilterRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryFilterRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Filter)
        , initialCursorPosition(0)
        , minimumPagedItems(200)
        , live(false)
    {
    }

    int initialCursorPosition;
    int minimumPagedItems;
    bool live;
    QStringList propertyNames;
    QStringList sortPropertyNames;
    QString itemType;
    QString containerId;
    QGalleryFilter filter;
};

/*!
    \class QGalleryFilterRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryItemRequest class provides a request for a set of
    items from a gallery.

*/
/*!
    Constructs a new gallery filter request.

    The \a parent is passed to QObject.
*/


QGalleryFilterRequest::QGalleryFilterRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryFilterRequestPrivate(0), parent)
{
}
/*!
    Contructs a new filter request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryFilterRequest::QGalleryFilterRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryFilterRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery filter request.
*/

QGalleryFilterRequest::~QGalleryFilterRequest()
{
}
/*!
    \property QGalleryFilterRequest::propertyNames

    \brief A list of names of meta-data properties a request should return values for.
*/


QStringList QGalleryFilterRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryFilterRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryFilterRequest::sortPropertyNames

    \brief A list of names of meta-data properties a request should sort its
    results on.

    Prefixing a property name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryFilterRequest::sortPropertyNames() const
{
    return d_func()->sortPropertyNames;
}

void QGalleryFilterRequest::setSortPropertyNames(const QStringList &names)
{
    d_func()->sortPropertyNames = names;
}

/*!
    \property QGalleryFilterRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/


bool QGalleryFilterRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryFilterRequest::setLive(bool live)
{
    d_func()->live = live;
}
/*!
    \property QGalleryFilterRequest::initialCursorPosition

    \brief The index of the first item a request should return.

    By default this is 0.
*/


int QGalleryFilterRequest::initialCursorPosition() const
{
    return d_func()->initialCursorPosition;
}

void QGalleryFilterRequest::setInitialCursorPosition(int index)
{
    d_func()->initialCursorPosition = index;
}
/*!
    \property QGalleryFilterRequest::minimumPagedItems

    \brief The minimum number of consecutive items the list returned by a
    request should cache.

    By default this is 200.
*/


int QGalleryFilterRequest::minimumPagedItems() const
{
    return d_func()->minimumPagedItems;
}

void QGalleryFilterRequest::setMinimumPagedItems(int size)
{
    d_func()->minimumPagedItems = size;
}
/*!
    \property QGalleryFilterRequest::itemType

    \brief The type of items a request should return.
*/


QString QGalleryFilterRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryFilterRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}

/*!
    \property QGalleryFilterRequest::containerId

    \brief The ID of a container item a request should return the descendents
    of.

*/

QString QGalleryFilterRequest::containerId() const
{
    return d_func()->containerId;
}

void QGalleryFilterRequest::setContainerId(const QString &id)
{
    d_func()->containerId = id;
}
/*!
    \property QGalleryFilterRequest::filter

    \brief A filter identifying the items a request should return.

    If no filter is set the results of the request will be determined soley
    by the \l itemType property.
*/

QGalleryFilter QGalleryFilterRequest::filter() const
{
    return d_func()->filter;
}

void QGalleryFilterRequest::setFilter(const QGalleryFilter &filter)
{
    d_func()->filter = filter;
}
/*!
    \property QGalleryFilterRequest::items

    \brief The items returned by a request.
*/


QGalleryItemList *QGalleryFilterRequest::items() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryFilterRequest::itemsChanged()

    Signals that the items property has changed.
*/

/*!
    \reimp
*/

void QGalleryFilterRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit itemsChanged();
}


class QGalleryCountRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryCountRequest)
public:
    QGalleryCountRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Count)
        , count(0)
        , live(false)
    {
    }

    void _q_itemsChanged();

    int count;
    bool live;
    QString itemType;
    QString containerId;
    QGalleryFilter filter;
};

void QGalleryCountRequestPrivate::_q_itemsChanged()
{
    count = response->count();

    emit q_func()->countChanged();
}

/*!
    \class QGalleryCountRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryCountRequest class provides a request for the number of
    items in a gallery matching some criteria.
*/

/*!
    Constructs a new gallery count request.

    The \a parent is passed to QObject.
*/

QGalleryCountRequest::QGalleryCountRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryCountRequestPrivate(0), parent)
{
}

/*!
    Contructs a new count request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryCountRequest::QGalleryCountRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryCountRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a count request.
*/

QGalleryCountRequest::~QGalleryCountRequest()
{
}

/*!
    \property QGalleryCountRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/

bool QGalleryCountRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryCountRequest::setLive(bool live)
{
    d_func()->live = live;
}

/*!
    \property QGalleryCountRequest::itemType

    \brief The type of item a request should count.
*/

QString QGalleryCountRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryCountRequest::setItemType(const QString &type)
{
    d_func()->itemType = type;
}

/*!
    \property QGalleryCountRequest::containerId

    \brief The ID of a container item a request should count the descendents
    of.

*/

QString QGalleryCountRequest::containerId() const
{
    return d_func()->containerId;
}

void QGalleryCountRequest::setContainerId(const QString &id)
{
    d_func()->containerId = id;
}

/*!
    \property QGalleryCountRequest::filter

    \brief A filter used to restrict the items counted by a request.

*/

QGalleryFilter QGalleryCountRequest::filter() const
{
    return d_func()->filter;
}

void QGalleryCountRequest::setFilter(const QGalleryFilter &filter)
{
    d_func()->filter = filter;
}

/*!
    \property QGalleryCountRequest::count

    \brief The number of gallery items matching a requests filtering criteria.
*/

int QGalleryCountRequest::count() const
{
    return d_func()->count;
}

/*!
    \fn QGalleryCountRequest::countChanged()

    Signals that the \l count property has changed.
*/

/*!
    \reimp
*/

void QGalleryCountRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryCountRequest);

    int count  = 0;

    if (response) {
        count = response->count();

        connect(response, SIGNAL(inserted(int,int)), this, SLOT(_q_itemsChanged()));
        connect(response, SIGNAL(removed(int,int)), this, SLOT(_q_itemsChanged()));
    }

    if (d->count != count) {
        d->count = count;

        emit countChanged();
    }
}

class QGalleryRemoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryRemoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Remove)
    {
    }

    QStringList propertyNames;
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
    \property QGalleryRemoveRequest::propertyNames

    \brief A list of name of meta-data properties the \l currentItem list
    should contain values for.
*/

QStringList QGalleryRemoveRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryRemoveRequest::setPropertyNames(const QStringList &propertyNames)
{
    d_func()->propertyNames = propertyNames;
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

    QStringList propertyNames;
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
    \property QGalleryCopyRequest::propertyNames

    \brief A list of names of meta-data properties the \l currentItem list
    should contain values for.
*/

QStringList QGalleryCopyRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryCopyRequest::setPropertyNames(const QStringList &propertyNames)
{
    d_func()->propertyNames = propertyNames;
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

    QStringList propertyNames;
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
    \property QGalleryMoveRequest::propertyNames

    \brief A list of names of meta-data properties the \l currentItem list
    should contain values for.
*/

QStringList QGalleryMoveRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryMoveRequest::setPropertyNames(const QStringList &propertyNames)
{
    d_func()->propertyNames = propertyNames;
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
