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

#include <qmediaimageviewerservice_p.h>

#include <qmediacontrol_p.h>
#include <qmediaservice_p.h>

#include <qmediacontent.h>
#include <qmediaresource.h>
#include <qvideooutputcontrol.h>
#include <qmediaobject_p.h>
#ifndef QT_NO_MULTIMEDIA
#include <qvideorenderercontrol.h>
#endif
#include <qvideowidgetcontrol.h>

#include <QtCore/qdebug.h>

#include <QtCore/qurl.h>
#include <QtGui/qimagereader.h>
#include <QtGui/qpainter.h>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>

#ifndef QT_NO_MULTIMEDIA
# include <QtMultimedia/qabstractvideosurface.h>
# include <QtMultimedia/qvideosurfaceformat.h>
#endif

class QMediaImageViewerServicePrivate : public QMediaServicePrivate
{
public:
    QMediaImageViewerServicePrivate()
        : viewerControl(0)
        , outputControl(0)
#ifndef QT_NO_MULTIMEDIA
        , rendererControl(0)
#endif
        , network(0)
        , internalNetwork(0)
    {
    }

    bool load(QIODevice *device);
    void clear();
    void _q_outputChanged(QVideoOutputControl::Output output);

    QMediaImageViewerControl *viewerControl;
    QMediaImageViewerOutputControl *outputControl;
#ifndef QT_NO_MULTIMEDIA
    QMediaImageViewerRenderer *rendererControl;
#endif
    QMediaImageViewerWidgetControl *widgetControl;
    QMediaImageViewerWidget *widget;
    QNetworkAccessManager *network;
    QNetworkAccessManager *internalNetwork;
    QImage m_image;
};

#ifndef QT_NO_MULTIMEDIA

QMediaImageViewerRenderer::QMediaImageViewerRenderer(QObject *parent)
    : QVideoRendererControl(parent)
    , m_surface(0)
{
}

QMediaImageViewerRenderer::~QMediaImageViewerRenderer()
{
    if (m_surface)
        m_surface->stop();
}

QAbstractVideoSurface *QMediaImageViewerRenderer::surface() const
{
    return m_surface;
}

void QMediaImageViewerRenderer::setSurface(QAbstractVideoSurface *surface)
{
    if (m_surface)
        m_surface->stop();

    m_surface = surface;

    if (m_surface && !m_image.isNull()) {
        QVideoSurfaceFormat format(
                m_image.size(), QVideoFrame::pixelFormatFromImageFormat(m_image.format()));

        if (surface->start(format))
            surface->present(QVideoFrame(m_image));
    }
}

void QMediaImageViewerRenderer::showImage(const QImage &image)
{
    m_image = image;

    if (m_surface) {
        if (m_image.isNull()) {
            m_surface->stop();
        } else {
            QVideoSurfaceFormat format(
                    image.size(), QVideoFrame::pixelFormatFromImageFormat(image.format()));

            if (m_surface->start(format))
                m_surface->present(QVideoFrame(image));
        }
    }
}

#endif

QMediaImageViewerWidget::QMediaImageViewerWidget(QWidget *parent)
    : QWidget(parent)
    , m_aspectRatioMode(QVideoWidget::KeepAspectRatio)
{
}

void QMediaImageViewerWidget::showImage(const QImage &image)
{
    m_image = image;

    update();
}

void QMediaImageViewerWidget::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;

    updateGeometry();
}

QSize QMediaImageViewerWidget::sizeHint() const
{
    return m_image.size();
}

void QMediaImageViewerWidget::paintEvent(QPaintEvent *)
{
    if (!m_image.isNull()) {
        QPainter painter(this);

        QRect displayRect = rect();

        QSize size = m_image.size();

        switch (m_aspectRatioMode) {
        case QVideoWidget::KeepAspectRatio:
            size.scale(displayRect.size(), Qt::KeepAspectRatio);
            {
                QRect rect(QPoint(0, 0), size);
                rect.moveCenter(displayRect.center());

                displayRect = rect;
            }
            break;
        case QVideoWidget::IgnoreAspectRatio:
            break;
        }

        painter.drawImage(displayRect, m_image);
    }
}

QMediaImageViewerWidgetControl::QMediaImageViewerWidgetControl(
        QMediaImageViewerWidget *widget, QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(widget)
    , m_fullScreen(false)
{
}

QWidget *QMediaImageViewerWidgetControl::videoWidget()
{
    return m_widget;
}

void QMediaImageViewerWidgetControl::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    m_widget->setAspectRatioMode(mode);
}

void QMediaImageViewerWidgetControl::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen = fullScreen);
}

QMediaImageViewerOutputControl::QMediaImageViewerOutputControl(QObject *parent)
    : QVideoOutputControl(parent)
    , m_output(NoOutput)
{
}

QList<QVideoOutputControl::Output> QMediaImageViewerOutputControl::availableOutputs() const
{
    return QList<Output>()
#ifndef QT_NO_MULTIMEDIA
            << RendererOutput
#endif
            << WidgetOutput;
}

void QMediaImageViewerOutputControl::setOutput(Output output)
{
    switch (output) {
#ifndef QT_NO_MULTIMEDIA
    case RendererOutput:
#endif
    case WidgetOutput:
        emit m_output = output;
        break;
    default:
        m_output = NoOutput;
    }
    emit outputChanged(m_output);
}

bool QMediaImageViewerServicePrivate::load(QIODevice *device)
{
    QImageReader reader(device);

    if (!reader.canRead()) {
        m_image = QImage();
    } else {
        m_image = reader.read();
    }

    if (outputControl->output() == QVideoOutputControl::WidgetOutput)
        widget->showImage(m_image);
#ifndef QT_NO_MULTIMEDIA
    else if (outputControl->output() == QVideoOutputControl::RendererOutput)
        rendererControl->showImage(m_image);
#endif

    return !m_image.isNull();
}

void QMediaImageViewerServicePrivate::clear()
{
    m_image = QImage();

    if (outputControl->output() == QVideoOutputControl::WidgetOutput)
        widget->showImage(m_image);
#ifndef QT_NO_MULTIMEDIA
    else if (outputControl->output() == QVideoOutputControl::RendererOutput)
        rendererControl->showImage(m_image);
#endif
}

void QMediaImageViewerServicePrivate::_q_outputChanged(QVideoOutputControl::Output output)
{
    if (output != QVideoOutputControl::WidgetOutput)
        widget->showImage(QImage());
#ifndef QT_NO_MULTIMEDIA
    if (output != QVideoOutputControl::RendererOutput)
        rendererControl->showImage(QImage());
#endif

    if (output == QVideoOutputControl::WidgetOutput)
        widget->showImage(m_image);
#ifndef QT_NO_MULTIMEDIA
    else if (output == QVideoOutputControl::RendererOutput)
        rendererControl->showImage(m_image);
#endif
}

/*!
    \class QMediaImageViewerService
    \internal
*/

/*!
*/
QMediaImageViewerService::QMediaImageViewerService(QObject *parent)
    : QMediaService(*new QMediaImageViewerServicePrivate, parent)
{
    Q_D(QMediaImageViewerService);

    d->viewerControl = new QMediaImageViewerControl(this);
    d->outputControl = new QMediaImageViewerOutputControl;
    connect(d->outputControl, SIGNAL(outputChanged(QVideoOutputControl::Output)),
            SLOT(_q_outputChanged(QVideoOutputControl::Output)));

#ifndef QT_NO_MULTIMEDIA
    d->rendererControl = new QMediaImageViewerRenderer;
#endif
    d->widget = new QMediaImageViewerWidget;
    d->widgetControl = new QMediaImageViewerWidgetControl(d->widget);
}

/*!
*/
QMediaImageViewerService::~QMediaImageViewerService()
{
    Q_D(QMediaImageViewerService);

    delete d->widgetControl;
    delete d->widget;
#ifndef QT_NO_MULTIMEDIA
    delete d->rendererControl;
#endif
    delete d->outputControl;
    delete d->viewerControl;
}

/*!
*/
QMediaControl *QMediaImageViewerService::control(const char *name) const
{
    Q_D(const QMediaImageViewerService);

    if (qstrcmp(name, QMediaImageViewerControl_iid) == 0) {
        return d->viewerControl;
    } else if (qstrcmp(name, QVideoOutputControl_iid) == 0) {
        return d->outputControl;
#ifndef QT_NO_MULTIMEDIA
    } else if (qstrcmp(name, QVideoRendererControl_iid) == 0) {
        return d->rendererControl;
#endif
    } else if (qstrcmp(name, QVideoWidgetControl_iid) == 0) {
        return d->widgetControl;
    } else {
        return 0;
    }
}

/*!
*/
QNetworkAccessManager *QMediaImageViewerService::networkManager() const
{
    Q_D(const QMediaImageViewerService);

    if (!d->network) {
        QMediaImageViewerServicePrivate *_d = const_cast<QMediaImageViewerServicePrivate *>(d);

        if (!_d->internalNetwork)
            _d->internalNetwork = new QNetworkAccessManager(
                    const_cast<QMediaImageViewerService *>(this));

        _d->network = d->internalNetwork;
    }

    return d->network;
}


void QMediaImageViewerService::setNetworkManager(QNetworkAccessManager *manager)
{
    d_func()->network = manager;
}

class QMediaImageViewerControlPrivate : public QMediaControlPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QMediaImageViewerControl)
public:
    QMediaImageViewerControlPrivate()
        : service(0)
        , getReply(0)
        , headReply(0)
        , status(QMediaImageViewer::NoMedia)
    {
    }

    bool isImageType(const QUrl &uri, const QString &mimeType) const;

    void loadImage();
    void cancelRequests();

    void _q_getFinished();
    void _q_headFinished();

    QMediaImageViewerService *service;
    QNetworkReply *getReply;
    QNetworkReply *headReply;
    QMediaImageViewer::MediaStatus status;
    QMediaContent media;
    QMediaResource currentMedia;
    QList<QMediaResource> possibleResources;
};

bool QMediaImageViewerControlPrivate::isImageType(const QUrl &uri, const QString &mimeType) const
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

void QMediaImageViewerControlPrivate::loadImage()
{
    cancelRequests();

    QMediaImageViewer::MediaStatus currentStatus = status;
    status = QMediaImageViewer::InvalidMedia;

    QNetworkAccessManager *network = service->networkManager();

    while (!possibleResources.isEmpty() && !headReply && !getReply) {
        currentMedia = possibleResources.takeFirst();

        QUrl uri = currentMedia.uri();
        QString mimeType = currentMedia.mimeType();

        if (isImageType(uri, mimeType)) {
            getReply = network->get(QNetworkRequest(uri));
            QObject::connect(getReply, SIGNAL(finished()), q_func(), SLOT(_q_getFinished()));

            status = QMediaImageViewer::LoadingMedia;
        } else if (mimeType.isEmpty() && uri.scheme() != QLatin1String("file")) {
            headReply = network->head(QNetworkRequest(currentMedia.uri()));
            QObject::connect(headReply, SIGNAL(finished()), q_func(), SLOT(_q_headFinished()));

            status = QMediaImageViewer::LoadingMedia;
        }
    }

    if (status == QMediaImageViewer::InvalidMedia)
        currentMedia = QMediaResource();

    if (status != currentStatus)
        emit q_func()->mediaStatusChanged(status);
}

void QMediaImageViewerControlPrivate::cancelRequests()
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

void QMediaImageViewerControlPrivate::_q_getFinished()
{
    if (getReply != q_func()->sender())
        return;

    QImage image;

    if (service->d_func()->load(getReply)) {
        possibleResources.clear();

        status = QMediaImageViewer::LoadedMedia;

        emit q_func()->mediaStatusChanged(status);
    } else {
        loadImage();
    }
}

void QMediaImageViewerControlPrivate::_q_headFinished()
{
    if (headReply != q_func()->sender())
        return;

    QString mimeType = headReply->header(QNetworkRequest::ContentTypeHeader)
            .toString().section(QLatin1Char(';'), 0, 0);
    QUrl uri = headReply->url();
    if (uri.isEmpty())
        uri = headReply->request().url();

    headReply->deleteLater();
    headReply = 0;

    if (isImageType(uri, mimeType) || mimeType.isEmpty()) {
        QNetworkAccessManager *network = service->networkManager();

        getReply = network->get(QNetworkRequest(uri));

        QObject::connect(getReply, SIGNAL(finished()), q_func(), SLOT(_q_getFinished()));
    } else {
        loadImage();
    }
}

/*!
    \class QMediaImageViewerControl
    \internal
*/
QMediaImageViewerControl::QMediaImageViewerControl(QMediaImageViewerService *parent)
    : QMediaControl(*new QMediaImageViewerControlPrivate, parent)
{
    Q_D(QMediaImageViewerControl);

    d->service = parent;
}

/*!
*/
QMediaImageViewerControl::~QMediaImageViewerControl()
{
    Q_D(QMediaImageViewerControl);

    delete d->getReply;
}

/*!
*/
QMediaImageViewer::MediaStatus QMediaImageViewerControl::mediaStatus() const
{
    return d_func()->status;
}

/*!
    \fn QMediaImageViewerControl::mediaStatusChanged(QMediaImageViewer::MediaStatus status);
*/

/*!
*/
void QMediaImageViewerControl::showMedia(const QMediaContent &media)
{
    Q_D(QMediaImageViewerControl);

    d->media = media;
    d->currentMedia = QMediaResource();
    d->cancelRequests();

    if (media.isNull()) {
        d->service->d_func()->clear();

        d->status = QMediaImageViewer::NoMedia;

        emit mediaStatusChanged(d->status);
    } else {
        d->possibleResources = media.resources();

        QUrl posterUri = media.posterUri();
        if (!posterUri.isEmpty())
            d->possibleResources << QMediaResource(posterUri);

        QUrl coverUriLarge = media.coverArtUriLarge();
        if (!coverUriLarge.isEmpty())
            d->possibleResources << QMediaResource(coverUriLarge);

        QUrl coverUriSmall = media.coverArtUriSmall();
        if (!coverUriSmall.isEmpty())
            d->possibleResources << QMediaResource(coverUriSmall);

        d->loadImage();
    }
}


#include "moc_qmediaimageviewerservice_p.cpp"
