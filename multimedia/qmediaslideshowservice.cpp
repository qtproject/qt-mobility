/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmediaslideshowservice_p.h"

#include "qabstractmediacontrol_p.h"
#include "qabstractmediaservice_p.h"

#include "qmediasource.h"
#include "qmediaresource.h"

#include <QtCore/qurl.h>
#include <QtGui/qimage.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>

class QMediaSlideShowServicePrivate : public QAbstractMediaServicePrivate
{
public:
    QMediaSlideShowServicePrivate()
        : slideControl(0)
        , network(0)
    {
    }

    QMediaSlideShowControl *slideControl;
    QNetworkAccessManager *network;
    QImage currentImage;
};

/*!
    \class QMediaSlideShowService
    \internal
*/

/*!
*/
QMediaSlideShowService::QMediaSlideShowService(QObject *parent)
    : QAbstractMediaService(*new QMediaSlideShowServicePrivate, parent)
{
    Q_D(QMediaSlideShowService);

    d->slideControl = new QMediaSlideShowControl(this);
}

/*!
*/
QMediaSlideShowService::~QMediaSlideShowService()
{
    Q_D(QMediaSlideShowService);

    delete d->slideControl;
}

/*!
*/
QAbstractMediaControl *QMediaSlideShowService::control(const char *name) const
{
    Q_D(const QMediaSlideShowService);

    if (qstrcmp(name, QMediaSlideShowControl_iid) == 0) {
        return d->slideControl;
    } else {
        return 0;
    }
}

/*!
*/
QNetworkAccessManager *QMediaSlideShowService::networkManager() const
{
    return d_func()->network;
}

/*!
*/
void QMediaSlideShowService::setNetworkManager(QNetworkAccessManager *manager)
{
    d_func()->network = manager;
}

/*!
*/
QImage QMediaSlideShowService::currentImage() const
{
    return d_func()->currentImage;
}

/*!
*/
void QMediaSlideShowService::setCurrentImage(const QImage &image)
{
    emit currentImageChanged(d_func()->currentImage = image);
}

/*!
    \fn QMediaSlideShowService::currentImageChanged(const QImage &image)
*/

class QMediaSlideShowControlPrivate : public QAbstractMediaControlPrivate
{
    Q_DECLARE_PUBLIC(QMediaSlideShowControl)
public:
    QMediaSlideShowControlPrivate()
        : service(0)
        , getReply(0)
        , headReply(0)
        , status(QMediaSlideShow::NoMedia)
    {
    }

    bool isImageType(const QUrl &uri, const QString &mimeType) const;

    void loadImage();
    void cancelRequests();

    void _q_getFinished();
    void _q_headFinished();

    QMediaSlideShowService *service;
    QNetworkReply *getReply;
    QNetworkReply *headReply;
    QMediaSlideShow::MediaStatus status;
    QMediaSource media;
    QMediaResource currentMedia;
    QList<QMediaResource> possibleResources;
};

bool QMediaSlideShowControlPrivate::isImageType(const QUrl &uri, const QString &mimeType) const
{
    if (!mimeType.isEmpty()) {
        return mimeType.startsWith(QLatin1String("image/"))
                || mimeType == QLatin1String("application/xml+svg");
    } else if (uri.scheme() == QLatin1String("file")) {
        QString path = uri.path();

        return path.endsWith(QLatin1String(".jpeg"), Qt::CaseInsensitive)
                || path.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive)
                || path.endsWith(QLatin1String(".png"), Qt::CaseInsensitive)
                || path.endsWith(QLatin1String(".bmp"), Qt::CaseInsensitive)
                || path.endsWith(QLatin1String(".svg"), Qt::CaseInsensitive)
                || path.endsWith(QLatin1String(".tiff"), Qt::CaseInsensitive);
    } else {
        return false;
    }
}

void QMediaSlideShowControlPrivate::loadImage()
{
    cancelRequests();

    QNetworkAccessManager *network = service->networkManager();

    if (!network) {
        possibleResources.clear();

        status = QMediaSlideShow::InvalidMedia;
        currentMedia = QMediaResource();
    } else {
        while (!possibleResources.isEmpty() && !headReply && !getReply) {
            currentMedia = possibleResources.takeFirst();

            QUrl uri = currentMedia.uri();
            QString mimeType = currentMedia.mimeType();

            if (isImageType(uri, mimeType)) {
                getReply = network->get(QNetworkRequest(uri));

                QObject::connect(getReply, SIGNAL(finished()), q_func(), SLOT(_q_getFinished()));
            } else if (mimeType.isEmpty() && uri.scheme() != QLatin1String("file")) {
                headReply = network->head(QNetworkRequest(currentMedia.uri()));

                QObject::connect(headReply, SIGNAL(finished()), q_func(), SLOT(_q_headFinished()));
            }
        }
    }

    emit q_func()->currentMediaChanged(currentMedia);
    emit q_func()->mediaStatusChanged(status);
}

void QMediaSlideShowControlPrivate::cancelRequests()
{
    if (getReply) {
        getReply->abort();
        getReply->deleteLater();
        getReply = 0;
    }

    if (headReply) {
        headReply->abort();
        headReply->deleteLater();
        headReply = 0;
    }
}

void QMediaSlideShowControlPrivate::_q_getFinished()
{
    QImage image;

    if (image.load(getReply, 0)) {
        possibleResources.clear();

        service->setCurrentImage(image);

        status = QMediaSlideShow::LoadedMedia;

        emit q_func()->mediaStatusChanged(status);
    } else {
        loadImage();
    }
}

void QMediaSlideShowControlPrivate::_q_headFinished()
{
    QString mimeType = headReply->header(QNetworkRequest::ContentTypeHeader)
            .toString().section(QLatin1Char(';'), 0, 0);
    QUrl uri = headReply->url();

    headReply->deleteLater();
    headReply = 0;

    if (isImageType(uri, mimeType)) {
        QNetworkAccessManager *network = service->networkManager();

        getReply = network->get(QNetworkRequest(uri));

        QObject::connect(getReply, SIGNAL(finished()), q_func(), SLOT(_q_getFinished()));
    } else {
        loadImage();
    }
}

/*!
    \class QMediaSlideShowControl
    \internal
*/
QMediaSlideShowControl::QMediaSlideShowControl(QMediaSlideShowService *parent)
    : QAbstractMediaControl(*new QMediaSlideShowControlPrivate, parent)
{
    Q_D(QMediaSlideShowControl);

    d->service = parent;
}

/*!
*/
QMediaSlideShowControl::~QMediaSlideShowControl()
{
    Q_D(QMediaSlideShowControl);

    delete d->getReply;
}

/*!
*/
QMediaSlideShow::MediaStatus QMediaSlideShowControl::mediaStatus() const
{
    return d_func()->status;
}

/*!
    \fn mediaStatusChanged(QMediaSlideShow::MediaStatus status);
*/

/*!
*/
QMediaSource QMediaSlideShowControl::media() const
{
    return d_func()->media;
}

/*!
*/
void QMediaSlideShowControl::setMedia(const QMediaSource &media)
{
    Q_D(QMediaSlideShowControl);

    d->media = media;
    d->currentMedia = QMediaResource();

    if (media.isNull()) {    
        d->cancelRequests();

        d->status = QMediaSlideShow::NoMedia;

        emit currentMediaChanged(d->currentMedia);
        emit mediaStatusChanged(d->status);
    } else {
        d->possibleResources
                = media.resources(QMediaResource::ContentRole)
                + media.resources(QMediaResource::PreviewRole)
                + media.resources(QMediaResource::PosterRole)
                + media.resources(QMediaResource::CoverArtRole);

        d->loadImage();
    }

    emit mediaChanged(d->media);

}

/*!
*/
QMediaResource QMediaSlideShowControl::currentMedia() const
{
    return d_func()->currentMedia;
}

/*!
    \fn currentMediaChanged(const QMediaResource &media)
*/

#include "moc_qmediaslideshowservice_p.cpp"

