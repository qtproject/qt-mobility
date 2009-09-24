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

#include <qmediaslideshowservice_p.h>

#include <qabstractmediacontrol_p.h>
#include <qabstractmediaservice_p.h>

#include <qmediasource.h>
#include <qmediaresource.h>
#include <qvideooutputcontrol.h>
#ifndef QT_NO_MULTIMEDIA
#include <qvideorenderercontrol.h>
#endif
#include <qvideowidgetcontrol.h>

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

class QMediaSlideShowWidget : public QWidget
{
    Q_OBJECT
public:
    QMediaSlideShowWidget(QWidget *parent = 0);

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

QMediaSlideShowWidget::QMediaSlideShowWidget(QWidget *parent)
    : QWidget(parent)
    , m_aspectRatio(QVideoWidget::AspectRatioAuto)
{
}

QImage QMediaSlideShowWidget::image() const
{
    return m_image;
}

void QMediaSlideShowWidget::setImage(const QImage &image)
{
    m_image = image;

    update();
}

QVideoWidget::AspectRatio QMediaSlideShowWidget::aspectRatio() const
{
    return m_aspectRatio;
}

void QMediaSlideShowWidget::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    m_aspectRatio = ratio;

    update();
}

void QMediaSlideShowWidget::paintEvent(QPaintEvent *)
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

class QMediaSlideShowWidgetControl : public QVideoWidgetControl
{
    Q_OBJECT
public:
    QMediaSlideShowWidgetControl(QMediaSlideShowWidget *widget, QObject *parent = 0);

    QWidget *videoWidget();

    QVideoWidget::AspectRatio aspectRatio() const;
    void setAspectRatio(QVideoWidget::AspectRatio ratio);

    QSize customAspectRatio() const;
    void setCustomAspectRatio(const QSize &customRatio);

    bool isFullscreen() const;
    void setFullscreen(bool fullscreen);

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

private:
    QMediaSlideShowWidget *m_widget;
};

QMediaSlideShowWidgetControl::QMediaSlideShowWidgetControl(
        QMediaSlideShowWidget *widget, QObject *parent)
    : QVideoWidgetControl(parent)
    , m_widget(widget)
{
}

QWidget *QMediaSlideShowWidgetControl::videoWidget()
{
    return m_widget;
}

QVideoWidget::AspectRatio QMediaSlideShowWidgetControl::aspectRatio() const
{
    return m_widget->aspectRatio();
}

void QMediaSlideShowWidgetControl::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    m_widget->setAspectRatio(ratio);
}

QSize QMediaSlideShowWidgetControl::customAspectRatio() const
{
    return QSize(1, 1);
}

void QMediaSlideShowWidgetControl::setCustomAspectRatio(const QSize &customRatio)
{
    Q_UNUSED(customRatio);
}

bool QMediaSlideShowWidgetControl::isFullscreen() const
{
    return m_widget->isFullScreen();
}

void QMediaSlideShowWidgetControl::setFullscreen(bool fullscreen)
{
    if (fullscreen) {
        m_widget->setWindowFlags(m_widget->windowFlags() | Qt::Window | Qt::WindowStaysOnTopHint);
        m_widget->setWindowState(m_widget->windowState() | Qt::WindowFullScreen);

        m_widget->show();

        emit fullscreenChanged(m_widget->isFullScreen());
    } else {
        m_widget->setWindowFlags(m_widget->windowFlags() & ~(Qt::Window | Qt::WindowStaysOnTopHint));
        m_widget->setWindowState(m_widget->windowState() & ~Qt::WindowFullScreen);

        m_widget->show();

        emit fullscreenChanged(m_widget->isFullScreen());
    }
}

int QMediaSlideShowWidgetControl::brightness() const
{
    return 0;
}

void QMediaSlideShowWidgetControl::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int QMediaSlideShowWidgetControl::contrast() const
{
    return 0;
}

void QMediaSlideShowWidgetControl::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int QMediaSlideShowWidgetControl::hue() const
{
    return 0;
}

void QMediaSlideShowWidgetControl::setHue(int hue)
{
    Q_UNUSED(hue);
}

int QMediaSlideShowWidgetControl::saturation() const
{
    return 0;
}

void QMediaSlideShowWidgetControl::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

class QMediaSlideShowOutputControl : public QVideoOutputControl
{
    Q_OBJECT
public:
    QMediaSlideShowOutputControl(QObject *parent = 0);

    QList<Output> availableOutputs() const;

    Output output() const;
    void setOutput(Output output);

Q_SIGNALS:
    void outputChanged(QVideoOutputControl::Output output);

private:
    Output m_output;
};

QMediaSlideShowOutputControl::QMediaSlideShowOutputControl(QObject *parent)
    : QVideoOutputControl(parent)
    , m_output(NoOutput)
{
}


QList<QVideoOutputControl::Output> QMediaSlideShowOutputControl::availableOutputs() const
{
    return QList<Output>()
#ifndef QT_NO_MULTIMEDIA
            << RendererOutput
#endif
            << WidgetOutput;
}

QVideoOutputControl::Output QMediaSlideShowOutputControl::output() const
{
    return m_output;
}

void QMediaSlideShowOutputControl::setOutput(Output output)
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

class QMediaSlideShowServicePrivate : public QAbstractMediaServicePrivate
{
public:
    QMediaSlideShowServicePrivate()
        : slideControl(0)
        , outputControl(0)
#ifndef QT_NO_MULTIMEDIA
        , rendererControl(0)
#endif
        , network(0)
    {
    }

    bool load(QIODevice *device);
    void clear();
#ifndef QT_NO_MULTIMEDIA
    void _q_surfaceChanged(QAbstractVideoSurface *surface);
#endif
    void _q_outputChanged(QVideoOutputControl::Output output);

    QMediaSlideShowControl *slideControl;
    QMediaSlideShowOutputControl *outputControl;
#ifndef QT_NO_MULTIMEDIA
    QMediaSlideShowRenderer *rendererControl;
#endif
    QMediaSlideShowWidgetControl *widgetControl;
    QMediaSlideShowWidget *widget;
    QNetworkAccessManager *network;

#ifndef QT_NO_MULTIMEDIA
    QAbstractVideoSurface *surface;
#endif
};

bool QMediaSlideShowServicePrivate::load(QIODevice *device)
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
    }
    return false;
}

void QMediaSlideShowServicePrivate::clear()
{
#ifndef QT_NO_MULTIMEDIA
    if (surface)
        surface->stop();
#endif
    widget->setImage(QImage());
}

#ifndef QT_NO_MULTIMEDIA
void QMediaSlideShowServicePrivate::_q_surfaceChanged(QAbstractVideoSurface *surface)
{
    this->surface = surface;
}
#endif

void QMediaSlideShowServicePrivate::_q_outputChanged(QVideoOutputControl::Output output)
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
    connect(d->outputControl, SIGNAL(outputChanged(QVideoOutputControl::Output)),
            SLOT(_q_outputChanged(QVideoOutputControl::Output)));

#ifndef QT_NO_MULTIMEDIA
    d->rendererControl = new QMediaSlideShowRenderer;

    connect(d->rendererControl, SIGNAL(surfaceChanged(QAbstractVideoSurface*)),
            this, SLOT(_q_surfaceChanged(QAbstractVideoSurface*)));
#endif
    d->widget = new QMediaSlideShowWidget;
    d->widgetControl = new QMediaSlideShowWidgetControl(d->widget);
}

/*!
*/
QMediaSlideShowService::~QMediaSlideShowService()
{
    Q_D(QMediaSlideShowService);

    delete d->widgetControl;
    delete d->widget;
#ifndef QT_NO_MULTIMEDIA
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
    \fn QMediaSlideShowControl::mediaStatusChanged(QMediaSlideShow::MediaStatus status);
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
    \fn QMediaSlideShowControl::currentMediaChanged(const QMediaResource &media)
*/

#include "moc_qmediaslideshowservice_p.cpp"
#include "qmediaslideshowservice.moc"
