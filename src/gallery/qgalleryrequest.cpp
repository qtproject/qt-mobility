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
    {
    }

    virtual ~QGalleryAbstractRequestPrivate()
    {
        delete response;
    }

    void _q_finished();
    void _q_galleryDestroyed();

    QGalleryAbstractRequest *q_ptr;
    QAbstractGallery *gallery;
    QGalleryAbstractResponse *response;
    QGalleryAbstractRequest::Type type;
    QGalleryAbstractRequest::State state;
    int result;
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

    \value Document The request is a QGalleryDocumentRequest.
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

            setResponse(d_ptr->response);
        } else {
            d_ptr->result = NotSupported;
            d_ptr->state = Inactive;

            if (oldResponse)
                setResponse(0);
        }

        delete oldResponse;

        switch (d_ptr->result) {
        case QGalleryAbstractRequest::NoResult:
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
        , documentCount(0)
        , idle(false)
    {
    }

    virtual ~QGalleryAbstractResponsePrivate() {}

    int result;
    int documentCount;
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
    : QGalleryDocumentList(parent)
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
    Identifies if the documents returned by a response are being monitored for
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
    Returns the number of documents affected by a gallery response.
*/

int QGalleryAbstractResponse::documentCount() const
{
    return d_ptr->documentCount;
}

/*!
    Updates the \a count of documents affected by a gallery response.
*/

void QGalleryAbstractResponse::updateDocumentCount(int count)
{
    if (d_ptr->documentCount != count) {
        d_ptr->documentCount = count;

        emit documentCountChanged();
    }
}

/*!
    \fn QGalleryAbstractResponse::waitForFinished(int msecs)

    Waits for \a msecs for the a response to finish.

    Returns true if the response has finished on return, and returns false if
    the wait time expires or the request is inactive or idle.
*/

/*!
    \fn QGalleryAbstractResponse::documentCountChanged()

    Signals that the number of documents related to a gallery response
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

    If \a idle is true the documents returned by a response will be monitored
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

    If \a idle is true the documents returned by a response will be monitored
    for changes and updated as appropriate.
*/

class QGalleryDocumentRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryDocumentRequest)
public:
    QGalleryDocumentRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Document)
        , startIndex(0)
        , maximumCount(-1)
        , totalCount(0)
        , live(false)
    {
    }

    void _q_documentCountChanged();

    int startIndex;
    int maximumCount;
    int totalCount;
    bool live;
    QStringList fields;
    QStringList sortFields;
    QString documentType;
    QGalleryFilter filter;
};

void QGalleryDocumentRequestPrivate::_q_documentCountChanged()
{
    const int count = response->documentCount();

    if (totalCount != count) {
        totalCount = count;

        emit q_func()->totalDocumentCountChanged();
    }
}

/*!
    \class QGalleryDocumentRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryDocumentRequest class provides a request for a set of
    documents from a document gallery.
*/

/*!
    \enum QGalleryDocumentRequest::Error

    Enumerates document request specific error results.

    \value InvalidFilter
*/

/*!
    Constructs a new gallery document request.

    The \a parent is passed to QObject.
*/

QGalleryDocumentRequest::QGalleryDocumentRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryDocumentRequestPrivate(0), parent)
{
}

/*!
    Contructs a new document request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryDocumentRequest::QGalleryDocumentRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryDocumentRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery document request.
*/

QGalleryDocumentRequest::~QGalleryDocumentRequest()
{
}

/*!
    \property QGalleryDocumentRequest::fields

    \brief A list of meta-data fields a request should return values for.
*/

QStringList QGalleryDocumentRequest::fields() const
{
    return d_func()->fields;
}

void QGalleryDocumentRequest::setFields(const QStringList &fields)
{
    d_func()->fields = fields;
}

/*!
    \property QGalleryDocumentRequest::sortFields

    \brief A list of meta-data fields a request should sort its results on.

    Prefixing a field name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryDocumentRequest::sortFields() const
{
    return d_func()->sortFields;
}

void QGalleryDocumentRequest::setSortFields(const QStringList &fields)
{
    d_func()->sortFields = fields;
}

/*!
    \property QGalleryDocumentRequest::live

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/

bool QGalleryDocumentRequest::isLive() const
{
    return d_func()->live;
}

void QGalleryDocumentRequest::setLive(bool live)
{
    d_func()->live = live;
}

/*!
    \property QGalleryDocumentRequest::startIndex

    \brief The index of the first document a request should return.

    By default this is 0.
*/

int QGalleryDocumentRequest::startIndex() const
{
    return d_func()->startIndex;
}

void QGalleryDocumentRequest::setStartIndex(int index)
{
    d_func()->startIndex = index;
}

/*!
    \property QGalleryDocumentRequest::maximumCount

    \brief The maximum number of documents that should be returned by a request.

    If this is -1 the results will be unbounded.

    By default this is -1.
*/

int QGalleryDocumentRequest::maximumCount() const
{
    return d_func()->maximumCount;
}

void QGalleryDocumentRequest::setMaximumCount(int count)
{
    d_func()->maximumCount = count;
}

/*!
    \property QGalleryDocumentRequest::documentType

    \brief The type of documents a request should return.

    If this is not set documents of all types will be returned.  If no filter is
    set all documents of this type will be returned.
*/

QString QGalleryDocumentRequest::documentType() const
{
    return d_func()->documentType;
}

void QGalleryDocumentRequest::setDocumentType(const QString &type)
{
    d_func()->documentType = type;
}

/*!
    \property QGalleryDocumentRequest::filter

    \brief A filter identifying the documents a request should return.

    If no filter is set the results of the request will be determined soley
    by the \l documentType property.
*/

QGalleryFilter QGalleryDocumentRequest::filter() const
{
    return d_func()->filter;
}

void QGalleryDocumentRequest::setFilter(const QGalleryFilter &filter)
{
    Q_D(QGalleryDocumentRequest);

    QGalleryFilter::Type oldType = d->filter.type();

    d->filter = filter;

    emit filterChanged();

    switch (oldType) {
    case QGalleryFilter::Document:
        emit documentIdsChanged();
        break;
    case QGalleryFilter::DocumentUrl:
        emit documentUrlsChanged();
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
        case QGalleryFilter::Document:
            emit documentIdsChanged();
            break;
        case QGalleryFilter::DocumentUrl:
            emit documentUrlsChanged();
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
    \fn QGalleryDocumentRequest::filterChanged()

    Signals the \l filter property has changed.
*/

/*!
    \property QGalleryDocumentRequest::documentId

    \brief The ID of a document a document request should return.

    This is equivalent to setting a \l filter of type QGalleryDocumentFilter.

    If the current \l filter is not of type QGalleryDocumentFilter or multiple
    documents are specified in \l documentIds this will be null.
*/

QString QGalleryDocumentRequest::documentId() const
{
    return d_func()->filter.toDocumentFilter().documentId();
}

void QGalleryDocumentRequest::setDocumentId(const QString &id)
{
    setFilter(QGalleryDocumentFilter(id));
}

/*!
    \property QGalleryDocumentRequest::documentIds

    \brief A list of IDs for documents a document request should return.

    This is equivalent to setting a \l filter of type QGalleryDocumentFilter.

    If the current \l filter is not of type QGalleryDocumentFilter this will be
    null.
*/

QStringList QGalleryDocumentRequest::documentIds() const
{
    return d_func()->filter.toDocumentFilter().documentIds();
}

void QGalleryDocumentRequest::setDocumentIds(const QStringList &ids)
{
    setFilter(QGalleryDocumentFilter(ids));
}

/*!
    \fn QGalleryDocumentRequest::documentIdsChanged()

    Signals that the documentId and \l documentIds properties have changed.
*/

/*!
    \property QGalleryDocumentRequest::documentUrl

    \brief The URL of an document a document request should return.

    This is equivalent to setting a \l filter of type QGalleryDocumentUrlFilter.

    If the current \l filter is not of type QGalleryDocumentUrlFilter or
    multiple documents are specified in \l documentUrls this will be null.
*/

QUrl QGalleryDocumentRequest::documentUrl() const
{
    return d_func()->filter.toDocumentUrlFilter().documentUrl();
}

void QGalleryDocumentRequest::setDocumentUrl(const QUrl &url)
{
    setFilter(QGalleryDocumentUrlFilter(url));
}

/*!
    \property QGalleryDocumentRequest::documentUrls

    \brief A list of URLs for documents a document request should return.

    This is equivalent to setting a \l filter of type QGalleryDocumentUrlFilter.

    If the current \l filter is not of type QGalleryDocumentUrlFilter this will
    be null.
*/

QList<QUrl> QGalleryDocumentRequest::documentUrls() const
{
    return d_func()->filter.toDocumentUrlFilter().documentUrls();
}

void QGalleryDocumentRequest::setDocumentUrls(const QList<QUrl> &urls)
{
    setFilter(QGalleryDocumentUrlFilter(urls));
}

/*!
    \fn QGalleryDocumentRequest::documentUrlsChanged()

    Signals that the \l documentUrl and \l documentUrls properties have
    changed.
*/

/*!
    \property QGalleryDocumentRequest::containerId

    \brief The ID of container document a document request should return the
    contents of.

    This is equivalent to setting a \l filter of type QGalleryContainerFilter.

    If the current \l filter is not of type QGalleryContainerFilter this will be
    null.
*/

QString QGalleryDocumentRequest::containerId() const
{
    return d_func()->filter.toContainerFilter().containerId();
}

void QGalleryDocumentRequest::setContainerId(const QString &id)
{
    setFilter(QGalleryContainerFilter(id));
}

/*!
    \fn QGalleryDocumentRequest::containerIdChanged()

    Signals that the \l containerId property has changed.
*/

/*!
    \property QGalleryDocumentRequest::containerUrl

    \brief The URL of container document a document request should return the
    contents of.

    This is equivalent to setting a \l filter of type
    QGalleryContainerUrlFilter.

    If the current \l filter is not of type QGalleryContainerUrlFilter this
    will be null.
*/

QUrl QGalleryDocumentRequest::containerUrl() const
{
    return d_func()->filter.toContainerUrlFilter().containerUrl();
}

void QGalleryDocumentRequest::setContainerUrl(const QUrl &url)
{
    setFilter(QGalleryContainerUrlFilter(url));
}

/*!
    \fn QGalleryDocumentRequest::containerUrlChanged()

    Signals that the \l containerUrl property has changed.
*/

/*!
    \property QGalleryDocumentRequest::documents

    \brief The documents returned by a request.
*/

QGalleryDocumentList *QGalleryDocumentRequest::documents() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryDocumentRequest::documentsChanged()

    Signals that the \l documents property has changed.
*/

/*!
    \property QGalleryDocumentRequest::totalDocumentCount

    \brief The total number of documents matching \l filter.
*/

int QGalleryDocumentRequest::totalDocumentCount() const
{
    return d_func()->totalCount;
}

/*!
    \fn QGalleryDocumentRequest::totalDocumentCountChanged();

    Signals that the totalDocumentCount property has changed.
*/

/*!
    \reimp
*/

void QGalleryDocumentRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryDocumentRequest);

    int oldCount = d->totalCount;

    d->totalCount = response ? response->documentCount() : 0;

    emit documentsChanged();

    if (oldCount != d->totalCount)
        emit totalDocumentCountChanged();
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
    QList<QUrl> documentUrls;
};

/*!
    \class QGalleryInsertRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryInsertRequest class provides a request which inserts
    existing documents into a gallery.
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

    \brief A list of meta-data fields a request should return values for.
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
    \property QGalleryInsertRequest::documentUrl

    \brief The URL of an document to insert into a gallery.

    This is equivalent to \a documentUrls with a single value. If multiple
    \a documentUrls are set this will be null.
*/

QUrl QGalleryInsertRequest::documentUrl() const
{
    Q_D(const QGalleryInsertRequest);

    return d->documentUrls.count() == 1
            ? d->documentUrls.first()
            : QUrl();
}

void QGalleryInsertRequest::setDocumentUrl(const QUrl &url)
{
    Q_D(QGalleryInsertRequest);

    d->documentUrls.clear();

    if (!url.isEmpty())
        d->documentUrls.append(url);

    emit documentUrlsChanged();
}

/*!
    \property QGalleryInsertRequest::documentUrls

    \brief A list of URLs to insert into a gallery.

    If the list only contains one URL this is equivalent to \l documentUrl.
*/

QList<QUrl> QGalleryInsertRequest::documentUrls() const
{
    return d_func()->documentUrls;
}

void QGalleryInsertRequest::setDocumentUrls(const QList<QUrl> &urls)
{
    d_func()->documentUrls = urls;

    emit documentUrlsChanged();
}

/*!
    \fn QGalleryInsertRequest::documentUrlsChanged()

    Signals the \l documentUrl and \l documentUrls properties have changed.
*/

/*!
    \property QGalleryInsertRequest::documents

    \brief The documents inserted into a gallery.
*/

QGalleryDocumentList *QGalleryInsertRequest::documents() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryInsertRequest::documentsChanged()

    Signals the \l documents property has changed.
*/

/*!
    \reimp
*/

void QGalleryInsertRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit documentsChanged();
}

class QGalleryRemoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryRemoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Remove)
    {
    }

    QStringList documentIds;
};

/*!
    \class QGalleryRemoveRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryRemoveRequest class provides a request which removes
    documents from a gallery.
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
    \property QGalleryRemoveRequest::documentId

    \brief The ID of an document to remove from a gallery.

    This is equivalent to \l documentIds with a single ID.  If there are
    multiple \l documentIds this will be null.
*/

QString QGalleryRemoveRequest::documentId() const
{
    Q_D(const QGalleryRemoveRequest);

    return d->documentIds.count() == 1
            ? d->documentIds.first()
            : QString();
}

void QGalleryRemoveRequest::setDocumentId(const QString &id)
{
    Q_D(QGalleryRemoveRequest);

    d->documentIds.clear();

    if (!id.isNull())
        d->documentIds.append(id);

    emit documentIdsChanged();
}

/*!
    \fn QGalleryRemoveRequest::documentIdChanged()

    Signals the \l documentId property has changed.
*/

/*!
    \property QGalleryRemoveRequest::documentIds

    \brief A list of IDs of documents to remove from a gallery.

    If the list only contains one ID this is equivalent to \l documentId.
*/

QStringList QGalleryRemoveRequest::documentIds() const
{
    return d_func()->documentIds;
}

void QGalleryRemoveRequest::setDocumentIds(const QStringList &ids)
{
    d_func()->documentIds = ids;

    emit documentIdsChanged();
}

/*!
    \fn QGalleryRemoveRequest::documentIdsChanged()

    Signals that the documentId and documentIds properties have changed.
*/

/*!
    \reimp
*/

void QGalleryRemoveRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);
}

class QGalleryCopyRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryCopyRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Copy)
    {
    }

    QStringList fields;
    QStringList sortFields;
    QStringList documentIds;
    QString destinationId;
};

/*!
    \class QGalleryCopyRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryCopyRequest class provides a request which copies
    documents within a gallery.
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

    \brief A list of meta-data fields a request should return values for.
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
    \property QGalleryCopyRequest::sortFields

    \brief A list of meta-data fields a request should sort its results on.

    Prefixing a field name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryCopyRequest::sortFields() const
{
    return d_func()->sortFields;
}

void QGalleryCopyRequest::setSortFields(const QStringList &fields)
{
    d_func()->sortFields = fields;
}

/*!
    \property QGalleryCopyRequest::documentId

    \brief The ID of an document to copy within a gallery.

    This is equivalent to \l documentIds with a single ID.  If there are
    multiple \l documentIds this will be null.
*/

QString QGalleryCopyRequest::documentId() const
{
    Q_D(const QGalleryCopyRequest);

    return d->documentIds.count() == 1
            ? d->documentIds.first()
            : QString();
}

void QGalleryCopyRequest::setDocumentId(const QString &id)
{
    Q_D(QGalleryCopyRequest);

    d->documentIds.clear();

    if (!id.isNull())
        d->documentIds.append(id);

    emit documentIdsChanged();
}

QStringList QGalleryCopyRequest::documentIds() const
{
    return d_func()->documentIds;
}

/*!
    \property QGalleryCopyRequest::documentIds

    \brief A list of IDs of documents to copy within a document gallery.

    If the list only contains one ID this is equivalent to \l documentId.
*/

void QGalleryCopyRequest::setDocumentIds(const QStringList &ids)
{
    d_func()->documentIds = ids;

    emit documentIdsChanged();
}

/*!
    \fn QGalleryCopyRequest::documentIdsChanged()

    Signals the \l documentId and \l documentIds properties have changed.
*/

/*!
    \property QGalleryCopyRequest::destinationId

    \brief The ID of the container document a request will copy documents to.
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
    \property QGalleryCopyRequest::documents

    \brief The documents copies created by a request.
*/

QGalleryDocumentList *QGalleryCopyRequest::documents() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryCopyRequest::documentsChanged()

    Signals the \l documents property has changed.
*/

/*!
    \reimp
*/

void QGalleryCopyRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit documentsChanged();
}

class QGalleryMoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryMoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::Move)
    {
    }

    QStringList fields;
    QStringList sortFields;
    QStringList documentIds;
    QString destinationId;
};

/*!
    \class QGalleryMoveRequest

    \ingroup gallery
    \ingroup gallery-requests

    \brief The QGalleryMoveRequest class provides a request which moves
    documents within a gallery.
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

    \brief A list of meta-data fields a request should return values for.
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
    \property QGalleryMoveRequest::sortFields

    \brief A list of meta-data fields a request should sort its results on.

    Prefixing a field name with the '+' character indicates it should be sorted
    in ascending order, and a '-' character prefix indicates a descending order.
    If there is no prefix ascending order is assumed.
*/

QStringList QGalleryMoveRequest::sortFields() const
{
    return d_func()->sortFields;
}

void QGalleryMoveRequest::setSortFields(const QStringList &fields)
{
    d_func()->sortFields = fields;
}

/*!
    \property QGalleryMoveRequest::documentId

    \brief The ID of an document to copy within a gallery.

    This is equivalent to \l documentIds with a single ID.  If there are
    multiple \l documentIds this will be null.
*/

QString QGalleryMoveRequest::documentId() const
{
    Q_D(const QGalleryMoveRequest);

    return d->documentIds.count() == 1
            ? d->documentIds.first()
            : QString();
}

void QGalleryMoveRequest::setDocumentId(const QString &id)
{
    Q_D(QGalleryMoveRequest);

    d->documentIds.clear();

    if (!id.isNull())
        d->documentIds.append(id);

    emit documentIdsChanged();
}

/*!
    \property QGalleryMoveRequest::documentIds

    \brief A list of IDs of documents to copy within a gallery.

    If the list only contains one ID this is equivalent to \l documentId.
*/

QStringList QGalleryMoveRequest::documentIds() const
{
    return d_func()->documentIds;
}

void QGalleryMoveRequest::setDocumentIds(const QStringList &ids)
{
    d_func()->documentIds = ids;

    emit documentIdsChanged();
}

/*!
    \fn QGalleryMoveRequest::documentIdsChanged()

    Signals the \l documentId and \l documentIds properties have changed.
*/

/*!
    \property QGalleryMoveRequest::destinationId

    \brief The ID of the container document a request will move documents to.
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
    \property QGalleryMoveRequest::documents

    \brief The documents moved my a request.
*/

QGalleryDocumentList *QGalleryMoveRequest::documents() const
{
    return d_func()->response;
}

/*!
    \fn QGalleryMoveRequest::documentsChanged()

    Signals the \l documents property has changed.
*/

/*!
    \reimp
*/

void QGalleryMoveRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);

    emit documentsChanged();
}

#include "moc_qgalleryrequest.cpp"
