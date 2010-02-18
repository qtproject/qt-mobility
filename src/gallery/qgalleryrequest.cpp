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
{

}

/*!
    Constructs a new \a type request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryAbstractRequest::QGalleryAbstractRequest(
        QAbstractGallery *gallery, Type type, QObject *parent)
{

}

/*!
    Destroys a gallery request.
*/

QGalleryAbstractRequest::~QGalleryAbstractRequest()
{

}

/*!
    \property QGalleryAbstractRequest::gallery

    \brief The gallery service a request acts on.
*/

QAbstractGallery *QGalleryAbstractRequest::gallery() const
{

}

void QGalleryAbstractRequest::setGallery(QAbstractGallery *gallery)
{

}

/*!
    \property QGalleryAbstractRequest::supported

    \brief Whether a request is supported by its current \a gallery.
*/

bool QGalleryAbstractRequest::isSupported() const
{

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

}

/*!
    \property QGalleryAbstractRequest::state

    \brief The state of a request.
*/

QGalleryAbstractRequest::State QGalleryAbstractRequest::state() const
{

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

int QGalleryAbstractRequest::result()
{

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

}

/*!
    Executes a request.

    \bold {Note}: A valid \l gallery must be set before a request can be
    executed.
*/

void QGalleryAbstractRequest::execute()
{

}

/*!
    Cancels the execution of a request.  If the request is idle this will stop
    any future updates.
*/

void QGalleryAbstractRequest::cancel()
{

}

/*!
    Clears the results of a request.

    If the request is active or idle it will be cancelled.
*/

void QGalleryAbstractRequest::clear()
{

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

QGalleryAbstractResponse::QGalleryAbstractResponse(QObject *parent = 0)
{

}

/*!
    Destroys a gallery response.
*/

QGalleryAbstractResponse::~QGalleryAbstractResponse()
{

}

/*!
    Identifies if the documents returned by a response are being monitored for
    changes.

    Returns true if a response is in an idle state, and false otherwise.
*/
bool QGalleryAbstractResponse::isIdle() const
{

}

/*!
    Returns the result of a gallery request.
*/

int QGalleryAbstractResponse::result() const
{

}

/*!
    Returns the total number of documents related to a gallery response.
*/

int QGalleryAbstractResponse::totalDocumentCount() const
{

}

/*!
    Updates the total \a count of documents related to a gallery response.
*/

void QGalleryAbstractResponse::updateTotalDocumentCount(int count)
{

}

/*!
    Signals that the total number of documents related to a gallery response
    has changed.
*/

void QGalleryAbstractResponse::totalDocumentCountChanged()
{

}

/*!
    Cancels an active or idle gallery response.
*/

void QGalleryAbstractResponse::cancel()
{

}

/*!
    Finalizes a gallery response, and sets the \a result.

    If \a idle is true the documents returned by a response will be monitored
    for changes and updated as appropriate.
*/

void QGalleryAbstractResponse::finish(int result, bool idle)
{

}

/*!
    \fn QGalleryAbstractResponse::finished(int result, bool idle)

    Signals that a response has finished with the given \a result.

    If \a idle is true the documents returned by a response will be monitored
    for changes and updated as appropriate.
*/

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
{

}

/*!
    Contructs a new document request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryDocumentRequest::QGalleryDocumentRequest(QAbstractGallery *gallery, QObject *parent)
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

}

void QGalleryDocumentRequest::setFields(const QStringList &fields)
{

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

}

void QGalleryDocumentRequest::setSortFields(const QStringList &fields)
{

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

}

void QGalleryDocumentRequest::setLive(bool live)
{

}

/*!
    \property QGalleryDocumentRequest::startIndex

    \brief The index of the first document a request should return.

    By default this is 0.
*/

int QGalleryDocumentRequest::startIndex() const
{

}

void QGalleryDocumentRequest::setStartIndex(int index)
{

}

/*!
    \property QGalleryDocumentRequest::maximumCount

    \brief The maximum number of documents that should be returned by a request.

    If this is -1 the results will be unbounded.

    By default this is -1.
*/

int QGalleryDocumentRequest::maximumCount() const
{

}

void QGalleryDocumentRequest::setMaximumCount(int count)
{

}

/*!
    \property QGalleryDocumentRequest::documentType

    \brief The type of documents a request should return.

    If this is not set documents of all types will be returned.  If no filter is
    set all documents of this type will be returned.
*/

QString QGalleryDocumentRequest::documentType() const
{

}

void QGalleryDocumentRequest::setContentType(const QString &type)
{

}

/*!
    \property QGalleryDocumentRequest::filter

    \brief A filter identifying the documents a request should return.

    If no filter is set the results of the request will be determined soley
    by the \l documentType property.
*/

QGalleryFilter QGalleryDocumentRequest::filter() const
{

}

void QGalleryDocumentRequest::setFilter(const QGalleryFilter &filter)
{

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

}

void QGalleryDocumentRequest::setDocumentId(const QString &id)
{

}

/*!
    \fn QGalleryDocumentRequest::documentIdChanged()

    Signals the documentId propery has changed.
*/

/*!
    \property QGalleryDocumentRequest::documentIds

    \brief A list of IDs for documents a document request should return.

    This is equivalent to setting a \l filter of type QGalleryDocumentFilter.

    If the current \l filter is not of type QGalleryDocumentFilter this will be
    null.
*/

QStringList QGalleryDocumentRequest::documentIds() const
{

}

void QGalleryDocumentRequest::setDocumentIds(const QStringList &id)
{

}

/*!
    \fn QGalleryDocumentRequest::documentIdsChanged()

    Signals that the \l documentIds property has changed.
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

}

void QGalleryDocumentRequest::setDocumentUrl(const QUrl &url)
{

}

/*!
    \fn QGalleryDocumentRequest::documentUrlChanged()

    Signals that the \l documentUrl property has changed.
*/

/*!
    \property QGalleryDocumentRequest::documentUrls

    \brief A list of URLs for documents a document request should return.

    This is equivalent to setting a \l filter of type QGalleryDocumentUrlFilter.

    If the current \l filter is not of type QGalleryDocumentUrlFilter this will
    be null.
*/

QList<QUrl> QGalleryDocumentRequest::documentUrls() const
{

}

void QGalleryDocumentRequest::setDocumentUrls(const QList<QUrl> &urls)
{

}

/*!
    \fn QGalleryDocumentRequest::documentUrlsChanged()

    Signals that the \l documentUrls property has changed.
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

}

void QGalleryDocumentRequest::setContainerId(const QString &id)
{

}

/*!
    \fn QGalleryDocumentRequest::containerIdChanged()

    Signals that the \l containerId property has changed.
*/

/*!
    \property QGalleryDocumentRequest::documents

    \brief The documents returned by a request.
*/

QGalleryDocumentList *QGalleryDocumentRequest::documents() const
{

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

}

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
{

}

/*!
    Contructs a new insert request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryInsertRequest::QGalleryInsertRequest(QAbstractGallery *gallery, QObject *parent)
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

}

void QGalleryInsertRequest::setFields(const QStringList &fields)
{

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

}

void QGalleryInsertRequest::setSortFields(const QStringList &fields)
{

}

/*!
    \property QGalleryInsertRequest::documentUrl

    \brief The URL of an document to insert into a gallery.

    This is equivalent to \a documentUrls with a single value. If multiple
    \a documentUrls are set this will be null.
*/

QUrl QGalleryInsertRequest::documentUrl() const
{

}

void QGalleryInsertRequest::setDocumentUrl(const QUrl &url)
{

}

/*!
    \fn QGalleryInsertRequest::documentUrlChanged()

    Signals the \l documentUrl property has changed.
*/

/*!
    \property QGalleryInsertRequest::documentUrls

    \brief A list of URLs to insert into a gallery.

    If the list only contains one URL this is equivalent to \l documentUrl.
*/

QList<QUrl> QGalleryInsertRequest::documentUrls() const
{

}

void QGalleryInsertRequest::setDocumentUrls(const QList<QUrl> &urls)
{

}

/*!
    \fn QGalleryInsertRequest::documentUrlsChanged()

    Signals the \l documentUrls property has changed.
*/

/*!
    \property QGalleryInsertRequest::documents

    \brief The documents inserted into a gallery.
*/

QGalleryDocumentList *QGalleryInsertRequest::documents() const
{

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

}

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
{

}

/*!
    Contructs a new remove request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryRemoveRequest::QGalleryRemoveRequest(QAbstractGallery *gallery, QObject *parent)
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

}
void QGalleryRemoveRequest::setDocumentId(const QString &id)
{

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

}

void QGalleryRemoveRequest::setDocumentIds(const QStringList &id)
{

}

/*!
    \fn QGalleryRemoveRequest::documentIdsChanged()

    Signals that the documentIds property has changed.
*/

/*!
    \reimp
*/

void QGalleryRemoveRequest::setResponse(QGalleryAbstractResponse *response)
{

}

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
{

}

/*!
    Contructs a new copy request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryCopyRequest::QGalleryCopyRequest(QAbstractGallery *gallery, QObject *parent)
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

}

void QGalleryCopyRequest::setFields(const QStringList &fields)
{

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

}

void QGalleryCopyRequest::setSortFields(const QStringList &fields)
{

}

/*!
    \property QGalleryCopyRequest::documentId

    \brief The ID of an document to copy within a gallery.

    This is equivalent to \l documentIds with a single ID.  If there are
    multiple \l documentIds this will be null.
*/

QString QGalleryCopyRequest::documentId() const
{

}

void QGalleryCopyRequest::setDocumentId(const QString &id)
{

}

/*!
    \fn QGalleryCopyRequest::documentIdChanged()

    Signals the documentId property has changed.
*/

QStringList QGalleryCopyRequest::documentIds() const
{

}

/*!
    \property QGalleryCopyRequest::documentIds

    \brief A list of IDs of documents to copy within a document gallery.

    If the list only contains one ID this is equivalent to \l documentId.
*/

void QGalleryCopyRequest::setDocumentIds(const QStringList &id)
{

}

/*!
    \fn QGalleryCopyRequest::documentIdsChanged()

    Signals the documentIds property has changed.
*/

/*!
    \property QGalleryCopyRequest::destinationId

    \brief The ID of the container document a request will copy documents to.
*/

QString QGalleryCopyRequest::destinationId() const
{

}

void QGalleryCopyRequest::setDestinationId(const QString &id)
{

}

/*!
    \property QGalleryCopyRequest::documents

    \brief The documents copies created by a request.
*/

QGalleryDocumentList *QGalleryCopyRequest::documents() const
{

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

}

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
{

}

/*!
    Contructs a new move request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryMoveRequest::QGalleryMoveRequest(QAbstractGallery *gallery, QObject *parent)
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

}

void QGalleryMoveRequest::setFields(const QStringList &fields)
{

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

}

void QGalleryMoveRequest::setSortFields(const QStringList &fields)
{

}

/*!
    \property QGalleryMoveRequest::documentId

    \brief The ID of an document to copy within a gallery.

    This is equivalent to \l documentIds with a single ID.  If there are
    multiple \l documentIds this will be null.
*/

QString QGalleryMoveRequest::documentId() const
{

}

void QGalleryMoveRequest::setDocumentId(const QString &id)
{

}

/*!
    \fn QGalleryMoveRequest::documentIdChanged()

    Signals the \l documentId property has changed.
*/

/*!
    \property QGalleryMoveRequest::documentIds

    \brief A list of IDs of documents to copy within a gallery.

    If the list only contains one ID this is equivalent to \l documentId.
*/

QStringList QGalleryMoveRequest::documentIds() const
{

}

void QGalleryMoveRequest::setDocumentIds(const QStringList &id)
{

}

/*!
    \fn QGalleryMoveRequest::documentIdsChanged()

    Signals the \l documentIds property has changed.
*/

/*!
    \property QGalleryMoveRequest::destinationId

    \brief The ID of the container document a request will move documents to.
*/

QString QGalleryMoveRequest::destinationId() const
{

}

void QGalleryMoveRequest::setDestinationId(const QString &id)
{

}

/*!
    \property QGalleryMoveRequest::documents

    \brief The documents moved my a request.
*/

QGalleryDocumentList *QGalleryMoveRequest::documents() const
{

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

}
