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
#include "qvideooutputcontrol.h"
#ifndef QT_NO_VIDEOSURFACE
#include "qvideorenderercontrol.h"
#endif

#include <QtCore/qurl.h>
#include <QtGui/qimage.h>
#include <QtGui/qimagereader.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>

#ifndef QT_NO_VIDEOSURFACE
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

class QMediaSlideShowRenderer : public QVideoRendererControl
{
    Q_OBJECT
public:
    QMediaSlideShowRenderer(QObject *parent = 0);
    ~QMediaSlideShowRenderer();

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

Q_SIGNALS:
    void surfaceChanged(QAbstractVideoSurface *surface);

private:
    QAbstractVideoSurface *m_surface;
};

QMediaSlideShowRenderer::QMediaSlideShowRenderer(QObject *parent)
    : QVideoRendererControl(parent)
    , m_surface(0)
{
}

QMediaSlideShowRenderer::~QMediaSlideShowRenderer()
{
}

QAbstractVideoSurface *QMediaSlideShowRenderer::surface() const
{
    return m_surface;
}

void QMediaSlideShowRenderer::setSurface(QAbstractVideoSurface *surface)
{
    emit surfaceChanged(m_surface = surface);
}
#endif

class QMediaSlideShowOutputControl : public QVideoOutputControl
{
    Q_OBJECT
public:
    QMediaSlideShowOutputControl(QObject *parent = 0);

    QList<Output> availableOutputs() const;

    Output output() const;
    void setOutput(Output output);

private:
};

QMediaSlideShowOutputControl::QMediaSlideShowOutputControl(QObject *parent)
    : QVideoOutputControl(parent)
{
}


QList<QVideoOutputControl::Output> QMediaSlideShowOutputControl::availableOutputs() const
{
    return QList<Output>() << RendererOutput;
}

QVideoOutputControl::Output QMediaSlideShowOutputControl::output() const
{
    return RendererOutput;
}

void QMediaSlideShowOutputControl::setOutput(Output output)
{
    Q_UNUSED(output);
}

class QMediaSlideShowServicePrivate : public QAbstractMediaServicePrivate
{
public:
    QMediaSlideShowServicePrivate()
        : slideControl(0)
        , outputControl(0)
#ifndef QT_NO_VIDEOSURFACE
        , rendererControl(0)
#endif
        , network(0)
    {
    }

    bool load(QIODevice *device);
    void clear();
#ifndef QT_NO_VIDEOSURFACE
    void _q_surfaceChanged(QAbstractVideoSurface *surface);
#endif
    QMediaSlideShowControl *slideControl;
    QMediaSlideShowOutputControl *outputControl;
#ifndef QT_NO_VIDEOSURFACE
    QMediaSlideShowRenderer *rendererControl;
#endif
    QNetworkAccessManager *network;

#ifndef QT_NO_VIDEOSURFACE
    QAbstractVideoSurface *surface;
#endif
};

bool QMediaSlideShowServicePrivate::load(QIODevice *device)
{
#ifndef QT_NO_VIDEOSURFACE
    if (!surface)
        return false;

    QImageReader reader(device);

    if (!reader.canRead())
        return false;

    if (!device->isSequential()
            && reader.supportsOption(QImageIOHandler::ImageFormat)
            && reader.supportsOption(QImageIOHandler::Size)) {

        QImage::Format imageFormat = reader.imageFormat();

        QVideoSurfaceFormat format(reader.size(), QVideoFrame::equivalentPixelFormat(imageFormat));
        QVideoSurfaceFormat preferredFormat;

        bool supported = surface->isFormatSupported(format, &preferredFormat);

        if (preferredFormat.isValid()
                && preferredFormat.viewport().size() != format.frameSize()
                && (reader.supportsOption(QImageIOHandler::ScaledSize) || !supported)) {
            preferredFormat = QVideoSurfaceFormat(
                    preferredFormat.viewport().size(), format.pixelFormat());

            if (surface->isFormatSupported(preferredFormat)) {
                reader.setScaledSize(preferredFormat.frameSize());
                format = preferredFormat;
                supported = true;
            }
        }

        if (supported && surface->start(format))
            return surface->present(QVideoFrame(reader.read()));
    } else {
        QImage image = reader.read();

        if (!image.isNull()) {
            QVideoSurfaceFormat format(
                    image.size(), QVideoFrame::equivalentPixelFormat(image.format()));

            if (surface->start(format))
                return surface->present(QVideoFrame(image));
        }
    }
#endif
    return false;

}

void QMediaSlideShowServicePrivate::clear()
{
#ifndef QT_NO_VIDEOSURFACE
    if (surface)
        surface->stop();
#endif
}

#ifndef QT_NO_VIDEOSURFACE
void QMediaSlideShowServicePrivate::_q_surfaceChanged(QAbstractVideoSurface *surface)
{
    this->surface = surface;
}
#endif

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
    d->outputControl = new QMediaSlideShowOutputControl;

#ifndef QT_NO_VIDEOSURFACE
    d->rendererControl = new QMediaSlideShowRenderer;

    connect(d->rendererControl, SIGNAL(surfaceChanged(QAbstractVideoSurface*)),
            this, SLOT(_q_surfaceChanged(QAbstractVideoSurface*)));
#endif
}

/*!
*/
QMediaSlideShowService::~QMediaSlideShowService()
{
    Q_D(QMediaSlideShowService);
#ifndef QT_NO_VIDEOSURFACE
    delete d->rendererControl;
#endif
    delete d->outputControl;
    delete d->slideControl;
}

/*!
*/
QAbstractMediaControl *QMediaSlideShowService::control(const char *name) const
{
    Q_D(const QMediaSlideShowService);

    if (qstrcmp(name, QMediaSlideShowControl_iid) == 0) {
        return d->slideControl;
    } else if (qstrcmp(name, QVideoOutputControl_iid) == 0) {
        return d->outputControl;
#ifndef QT_NO_VIDEOSURFACE
    } else if (qstrcmp(name, QVideoRendererControl_iid) == 0) {
        return d->rendererControl;
#endif
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

    if (service->d_func()->load(getReply)) {
        possibleResources.clear();

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
        d->service->d_func()->clear();

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
#include "qmediaslideshowservice.moc"
