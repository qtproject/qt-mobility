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

#include <multimedia/qmediaimageviewerservice_p.h>

#include <multimedia/qmediacontrol_p.h>
#include <multimedia/qmediaservice_p.h>

#include <multimedia/qmediasource.h>
#include <multimedia/qmediaresource.h>
#include <multimedia/qvideooutputcontrol.h>
#ifndef QT_NO_MULTIMEDIA
#include <multimedia/qvideorenderercontrol.h>
#endif
#include <multimedia/qvideowidgetcontrol.h>

#include <QtCore/qdebug.h>

#include <QtCore/qurl.h>
#include <QtGui/qimage.h>
#include <QtGui/qimagereader.h>
#include <QtGui/qpainter.h>

#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkrequest.h>

#ifndef QT_NO_MULTIMEDIA
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

class QMediaImageViewerRenderer : public QVideoRendererControl
{
    Q_OBJECT
public:
    QMediaImageViewerRenderer(QObject *parent = 0);
    ~QMediaImageViewerRenderer();

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

Q_SIGNALS:
    void surfaceChanged(QAbstractVideoSurface *surface);

private:
    QAbstractVideoSurface *m_surface;
};

QMediaImageViewerRenderer::QMediaImageViewerRenderer(QObject *parent)
    : QVideoRendererControl(parent)
    , m_surface(0)
{
}

QMediaImageViewerRenderer::~QMediaImageViewerRenderer()
{
}

QAbstractVideoSurface *QMediaImageViewerRenderer::surface() const
{
    return m_surface;
}

void QMediaImageViewerRenderer::setSurface(QAbstractVideoSurface *surface)
{
    emit surfaceChanged(m_surface = surface);
}
#endif

class QMediaImageViewerWidget : public QWidget
{
    Q_OBJECT
public:
    QMediaImageViewerWidget(QWidget *parent = 0);

    QImage image() const;
    void setImage(const QImage &image);

    QVideoWidget::AspectRatio aspectRatio() const;
    void setAspectRatio(QVideoWidget::AspectRatio ratio);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QVideoWidget::AspectRatio m_aspectRatio;
    QImage m_image;
};

QMediaImageViewerWidget::QMediaImageViewerWidget(QWidget *parent)
    : QWidget(parent)
    , m_aspectRatio(QVideoWidget::AspectRatioAuto)
{
}

QImage QMediaImageViewerWidget::image() const
{
    return m_image;
}

void QMediaImageViewerWidget::setImage(const QImage &image)
{
    m_image = image;

    update();
}

QVideoWidget::AspectRatio QMediaImageViewerWidget::aspectRatio() const
{
    return m_aspectRatio;
}

void QMediaImageViewerWidget::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    m_aspectRatio = ratio;

    update();
}

void QMediaImageViewerWidget::paintEvent(QPaintEvent *)
{
    if (!m_image.isNull()) {
        QPainter painter(this);

        QRect displayRect = rect();

        switch (m_aspectRatio) {
        case QVideoWidget::AspectRatioAuto:
            {
                QSize size = m_image.size();

                size.scale(displayRect.size(), Qt::KeepAspectRatio);

                QRect rect(QPoint(0, 0), size);
                rect.moveCenter(displayRect.center());

                displayRect = rect;
            }
            break;
        case QVideoWidget::AspectRatioWidget:
            break;
        case QVideoWidget::AspectRatioCustom:
            break;
        }

        painter.drawImage(displayRect, m_image);
    }
}

class QMediaImageViewerWidgetControl : public QVideoWidgetControl
{
    Q_OBJECT
public:
    QMediaImageViewerWidgetControl(QMediaImageViewerWidget *widget, QObject *parent = 0);

    QWidget *videoWidget();

    QVideoWidget::AspectRatio aspectRatio() const;
    void setAspectRatio(QVideoWidget::AspectRatio ratio);

    QSize customAspectRatio() const;
    void setCustomAspectRatio(const QSize &customRatio);

    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

private:
    QMediaImageViewerWidget *m_widget;
};

QMediaImageViewerWidgetControl::QMediaImageViewerWidgetControl(
        QMediaImageViewerWidget *widget, QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(widget)
{
}

QWidget *QMediaImageViewerWidgetControl::videoWidget()
{
    return m_widget;
}

QVideoWidget::AspectRatio QMediaImageViewerWidgetControl::aspectRatio() const
{
    return m_widget->aspectRatio();
}

void QMediaImageViewerWidgetControl::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    m_widget->setAspectRatio(ratio);
}

QSize QMediaImageViewerWidgetControl::customAspectRatio() const
{
    return QSize(1, 1);
}

void QMediaImageViewerWidgetControl::setCustomAspectRatio(const QSize &customRatio)
{
    Q_UNUSED(customRatio);
}

bool QMediaImageViewerWidgetControl::isFullScreen() const
{
    return m_widget->isFullScreen();
}

void QMediaImageViewerWidgetControl::setFullScreen(bool fullScreen)
{
    if (fullScreen) {
        m_widget->setWindowFlags(m_widget->windowFlags() | Qt::Window | Qt::WindowStaysOnTopHint);
        m_widget->setWindowState(m_widget->windowState() | Qt::WindowFullScreen);

        m_widget->show();

        emit fullScreenChanged(m_widget->isFullScreen());
    } else {
        m_widget->setWindowFlags(m_widget->windowFlags() & ~(Qt::Window | Qt::WindowStaysOnTopHint));
        m_widget->setWindowState(m_widget->windowState() & ~Qt::WindowFullScreen);

        m_widget->show();

        emit fullScreenChanged(m_widget->isFullScreen());
    }
}

int QMediaImageViewerWidgetControl::brightness() const
{
    return 0;
}

void QMediaImageViewerWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int QMediaImageViewerWidgetControl::contrast() const
{
    return 0;
}

void QMediaImageViewerWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int QMediaImageViewerWidgetControl::hue() const
{
    return 0;
}

void QMediaImageViewerWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int QMediaImageViewerWidgetControl::saturation() const
{
    return 0;
}

void QMediaImageViewerWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

class QMediaImageViewerOutputControl : public QVideoOutputControl
{
    Q_OBJECT
public:
    QMediaImageViewerOutputControl(QObject *parent = 0);

    QList<Output> availableOutputs() const;

    Output output() const;
    void setOutput(Output output);

Q_SIGNALS:
    void outputChanged(QVideoOutputControl::Output output);

private:
    Output m_output;
};

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

QVideoOutputControl::Output QMediaImageViewerOutputControl::output() const
{
    return m_output;
}

void QMediaImageViewerOutputControl::setOutput(Output output)
{
    switch (output) {
#ifndef QT_NO_MULTIMEDIA
    case RendererOutput:
#endif
    case WidgetOutput:
        m_output = output;
        break;
    default:
        m_output = NoOutput;
    }
}

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
#ifndef QT_NO_MULTIMEDIA
        , surface(0)
#endif
    {
    }

    bool load(QIODevice *device);
    void clear();
#ifndef QT_NO_MULTIMEDIA
    void _q_surfaceChanged(QAbstractVideoSurface *surface);
#endif
    void _q_outputChanged(QVideoOutputControl::Output output);

    QMediaImageViewerControl *viewerControl;
    QMediaImageViewerOutputControl *outputControl;
#ifndef QT_NO_MULTIMEDIA
    QMediaImageViewerRenderer *rendererControl;
#endif
    QMediaImageViewerWidgetControl *widgetControl;
    QMediaImageViewerWidget *widget;
    QNetworkAccessManager *network;

#ifndef QT_NO_MULTIMEDIA
    QAbstractVideoSurface *surface;
#endif
};

bool QMediaImageViewerServicePrivate::load(QIODevice *device)
{
    QImageReader reader(device);

    if (!reader.canRead())
        return false;

    if (outputControl->output() == QVideoOutputControl::WidgetOutput) {
        QImage image = reader.read();

        widget->setImage(image);

        return !image.isNull();
#ifndef QT_NO_MULTIMEDIA
    } else if (outputControl->output() == QVideoOutputControl::WidgetOutput && surface != 0) {
        if (!device->isSequential()
                && reader.supportsOption(QImageIOHandler::ImageFormat)
                && reader.supportsOption(QImageIOHandler::Size)) {

            QImage::Format imageFormat = reader.imageFormat();

            QVideoSurfaceFormat format(
                    reader.size(), QVideoFrame::equivalentPixelFormat(imageFormat));
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
    } else {
        qWarning("QMediaImageViewerService: no video output connected");
    }
    return false;
}

void QMediaImageViewerServicePrivate::clear()
{
#ifndef QT_NO_MULTIMEDIA
    if (surface)
        surface->stop();
#endif
    widget->setImage(QImage());
}

#ifndef QT_NO_MULTIMEDIA
void QMediaImageViewerServicePrivate::_q_surfaceChanged(QAbstractVideoSurface *surface)
{
    this->surface = surface;
}
#endif

void QMediaImageViewerServicePrivate::_q_outputChanged(QVideoOutputControl::Output output)
{
    if (output != QVideoOutputControl::WidgetOutput) {
        widget->setImage(QImage());
#ifndef QT_NO_MULTIMEDIA
    } else if (output != QVideoOutputControl::RendererOutput && surface != 0) {
        surface->stop();
#endif
    }
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

    connect(d->rendererControl, SIGNAL(surfaceChanged(QAbstractVideoSurface*)),
            this, SLOT(_q_surfaceChanged(QAbstractVideoSurface*)));
#endif
    d->widget = new QMediaImageViewerWidget;
    d->widgetControl = new QMediaImageViewerWidgetControl(d->widget);

    d->network = new QNetworkAccessManager(this);
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
    return d_func()->network;
}

class QMediaImageViewerControlPrivate : public QMediaControlPrivate
{
    Q_DECLARE_PUBLIC(QMediaImageViewerControl)
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
    QMediaSource media;
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

    if (!network) {
        qWarning("QMediaImageViewerControlPrivate: No network manager");

        possibleResources.clear();
    } else {
        while (!possibleResources.isEmpty() && !headReply && !getReply) {
            currentMedia = possibleResources.takeFirst();

            QUrl uri = currentMedia.uri();
            QString mimeType = currentMedia.mimeType();

            if (isImageType(uri, mimeType)) {
                status = QMediaImageViewer::LoadingMedia;

                getReply = network->get(QNetworkRequest(uri));

                QObject::connect(getReply, SIGNAL(finished()), q_func(), SLOT(_q_getFinished()));
            } else if (mimeType.isEmpty() && uri.scheme() != QLatin1String("file")) {
                status = QMediaImageViewer::LoadingMedia;

                headReply = network->head(QNetworkRequest(currentMedia.uri()));

                QObject::connect(headReply, SIGNAL(finished()), q_func(), SLOT(_q_headFinished()));
            }
        }
    }

    if (status == QMediaImageViewer::InvalidMedia)
        currentMedia = QMediaResource();

    if (status != currentStatus)
        emit q_func()->mediaStatusChanged(status);

    emit q_func()->currentMediaChanged(currentMedia);
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
QMediaSource QMediaImageViewerControl::media() const
{
    return d_func()->media;
}

/*!
*/
void QMediaImageViewerControl::setMedia(const QMediaSource &media)
{
    Q_D(QMediaImageViewerControl);

    d->media = media;
    d->currentMedia = QMediaResource();

    if (media.isNull()) {
        d->cancelRequests();
        d->service->d_func()->clear();

        d->status = QMediaImageViewer::NoMedia;

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
}

/*!
*/
QMediaResource QMediaImageViewerControl::currentMedia() const
{
    return d_func()->currentMedia;
}

/*!
    \fn QMediaImageViewerControl::currentMediaChanged(const QMediaResource &media)
*/

#include "moc_qmediaimageviewerservice_p.cpp"
#include "qmediaimageviewerservice.moc"
