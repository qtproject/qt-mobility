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

#include "qgallerydocumentlist.h"

#include "qgalleryfilter.h"

#include <QtCore/qstringlist.h>

class QAbstractGallery;
class QGalleryAbstractResponse;
class QGalleryFilter;
class QUrl;

class QGalleryAbstractRequestPrivate;

class QGalleryAbstractRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery)
    Q_PROPERTY(bool supported READ isSupported NOTIFY supportedChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(int result READ result NOTIFY resultChanged)
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
        RequestError = 100
    };

    enum Type
    {
        Document,
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

protected:
    virtual void setResponse(QGalleryAbstractResponse *response) = 0;

    explicit QGalleryAbstractRequest(QGalleryAbstractRequestPrivate &dd, QObject *parent);

    QGalleryAbstractRequestPrivate *d_ptr;
};

class QGalleryAbstractResponsePrivate;

class QGalleryAbstractResponse : public QGalleryDocumentList
{
    Q_OBJECT
public:
    QGalleryAbstractResponse(QObject *parent = 0);
    ~QGalleryAbstractResponse();

    int result() const;
    bool isIdle() const;

    int documentCount() const;

    virtual void cancel();

    virtual bool waitForFinished(int msecs) = 0;

Q_SIGNALS:
    void finished(int result, bool idle);
    void documentCountChanged();

protected:
    void finish(int result, bool idle = false);
    void updateDocumentCount(int count);

    QGalleryAbstractResponsePrivate *d_ptr;
};

class QGalleryDocumentRequestPrivate;

class QGalleryDocumentRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryDocumentRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QStringList sortFields READ sortFields WRITE setSortFields)
    Q_PROPERTY(bool live READ isLive WRITE setLive)
    Q_PROPERTY(int startIndex READ startIndex WRITE setStartIndex)
    Q_PROPERTY(int maximumCount READ maximumCount WRITE setMaximumCount)
    Q_PROPERTY(QString documentType READ documentType WRITE setDocumentType)
    Q_PROPERTY(QGalleryFilter filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString documentId READ documentId WRITE setDocumentId NOTIFY documentIdsChanged)
    Q_PROPERTY(QStringList documentIds READ documentIds WRITE setDocumentIds NOTIFY documentIdsChanged)
    Q_PROPERTY(QUrl documentUrl READ documentUrl WRITE setDocumentUrl NOTIFY documentUrlsChanged)
    Q_PROPERTY(QList<QUrl> documentUrls READ documentUrls WRITE setDocumentUrls NOTIFY documentUrlsChanged)
    Q_PROPERTY(QString containerId READ containerId WRITE setContainerId)
    Q_PROPERTY(QUrl containerUrl READ containerUrl WRITE setContainerUrl)
    Q_PROPERTY(QGalleryDocumentList* documents READ documents NOTIFY documentsChanged)
    Q_PROPERTY(int totalDocumentCount READ totalDocumentCount NOTIFY totalDocumentCountChanged)
    Q_ENUMS(Error)
public:
    enum Error
    {
        InvalidFilter = RequestError
    };

    explicit QGalleryDocumentRequest(QObject *parent = 0);
    explicit QGalleryDocumentRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryDocumentRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QStringList sortFields() const;
    void setSortFields(const QStringList &fields);

    bool isLive() const;
    void setLive(bool live);

    int startIndex() const;
    void setStartIndex(int index);

    int maximumCount() const;
    void setMaximumCount(int count);

    QString documentType() const;
    void setDocumentType(const QString &type);

    QGalleryFilter filter() const;
    void setFilter(const QGalleryFilter &filter);

    QString documentId() const;
    void setDocumentId(const QString &id);

    QStringList documentIds() const;
    void setDocumentIds(const QStringList &id);

    QUrl documentUrl() const;
    void setDocumentUrl(const QUrl &url);

    QList<QUrl> documentUrls() const;
    void setDocumentUrls(const QList<QUrl> &urls);

    QString containerId() const;
    void setContainerId(const QString &id);

    QUrl containerUrl() const;
    void setContainerUrl(const QUrl &url);

    QGalleryDocumentList *documents() const;

    int totalDocumentCount() const;

Q_SIGNALS:
    void filterChanged();
    void documentIdsChanged();
    void documentUrlsChanged();
    void containerIdChanged();
    void containerUrlChanged();
    void documentsChanged();
    void totalDocumentCountChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryInsertRequestPrivate;

class QGalleryInsertRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryInsertRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QStringList sortFields READ sortFields WRITE setSortFields)
    Q_PROPERTY(QUrl documentUrl READ documentUrl WRITE setDocumentUrl NOTIFY documentUrlsChanged)
    Q_PROPERTY(QList<QUrl> documentUrls READ documentUrls WRITE setDocumentUrls NOTIFY documentUrlsChanged)
    Q_PROPERTY(QGalleryDocumentList *documents READ documents NOTIFY documentsChanged)
public:
    explicit QGalleryInsertRequest(QObject *parent = 0);
    explicit QGalleryInsertRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryInsertRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QStringList sortFields() const;
    void setSortFields(const QStringList &fields);

    QUrl documentUrl() const;
    void setDocumentUrl(const QUrl &url);

    QList<QUrl> documentUrls() const;
    void setDocumentUrls(const QList<QUrl> &urls);

    QGalleryDocumentList *documents() const;

Q_SIGNALS:
    void documentUrlsChanged();
    void documentsChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryRemoveRequestPrivate;

class QGalleryRemoveRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryRemoveRequest)
    Q_PROPERTY(QString documentId READ documentId WRITE setDocumentId NOTIFY documentIdsChanged);
    Q_PROPERTY(QStringList documentIds READ documentIds WRITE setDocumentIds NOTIFY documentIdsChanged)
public:
    explicit QGalleryRemoveRequest(QObject *parent = 0);
    explicit QGalleryRemoveRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryRemoveRequest();

    QString documentId() const;
    void setDocumentId(const QString &id);

    QStringList documentIds() const;
    void setDocumentIds(const QStringList &id);

Q_SIGNALS:
    void documentIdsChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryCopyRequestPrivate;

class QGalleryCopyRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryCopyRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QStringList sortFields READ sortFields WRITE setSortFields)
    Q_PROPERTY(QString documentId READ documentId WRITE setDocumentId NOTIFY documentIdsChanged);
    Q_PROPERTY(QStringList documentIds READ documentIds WRITE setDocumentIds NOTIFY documentIdsChanged)
    Q_PROPERTY(QString destinationId READ destinationId WRITE setDestinationId)
    Q_PROPERTY(QGalleryDocumentList* documents READ documents NOTIFY documentsChanged)
public:
    explicit QGalleryCopyRequest(QObject *parent = 0);
    explicit QGalleryCopyRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryCopyRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QStringList sortFields() const;
    void setSortFields(const QStringList &fields);

    QString documentId() const;
    void setDocumentId(const QString &id);

    QStringList documentIds() const;
    void setDocumentIds(const QStringList &id);

    QString destinationId() const;
    void setDestinationId(const QString &id);

    QGalleryDocumentList *documents() const;

Q_SIGNALS:
    void documentIdsChanged();
    void documentsChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

class QGalleryMoveRequestPrivate;

class QGalleryMoveRequest : public QGalleryAbstractRequest
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGalleryMoveRequest)
    Q_PROPERTY(QStringList fields READ fields WRITE setFields)
    Q_PROPERTY(QStringList sortFields READ sortFields WRITE setSortFields)
    Q_PROPERTY(QString documentId READ documentId WRITE setDocumentId NOTIFY documentIdsChanged);
    Q_PROPERTY(QStringList documentIds READ documentIds WRITE setDocumentIds NOTIFY documentIdsChanged)
    Q_PROPERTY(QString destinationId READ destinationId WRITE setDestinationId)
    Q_PROPERTY(QGalleryDocumentList* documents READ documents NOTIFY documentsChanged)
public:
    QGalleryMoveRequest(QObject *parent = 0);
    QGalleryMoveRequest(QAbstractGallery *gallery, QObject *parent = 0);
    ~QGalleryMoveRequest();

    QStringList fields() const;
    void setFields(const QStringList &fields);

    QStringList sortFields() const;
    void setSortFields(const QStringList &fields);

    QString documentId() const;
    void setDocumentId(const QString &id);

    QStringList documentIds() const;
    void setDocumentIds(const QStringList &id);

    QString destinationId() const;
    void setDestinationId(const QString &id);

    QGalleryDocumentList *documents() const;

Q_SIGNALS:
    void documentIdsChanged();
    void documentsChanged();

protected:
    void setResponse(QGalleryAbstractResponse *response);
};

#endif
