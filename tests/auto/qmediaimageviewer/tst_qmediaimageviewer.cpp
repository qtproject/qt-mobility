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

#include <QtTest/QtTest>

#include <QtCore/qdir.h>

#include <multimedia/qmediaimageviewer.h>
#include <multimedia/qmediaplaylist.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qvideooutputcontrol.h>
#include <multimedia/qvideorenderercontrol.h>
#include <multimedia/qvideowidgetcontrol.h>

#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

class tst_QMediaImageViewer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void isValid();
    void timeout();
    void setMedia_data();
    void setMedia();
    void setInvalidMedia();
    void playlist();
    void outputControl();
    void widgetControl();
#ifndef QT_NO_MULTIMEDIA
    void rendererControl();
#endif

public:
    tst_QMediaImageViewer() : m_imageDir(QLatin1String(TESTDATA_DIR)) {}

private:
    QUrl imageUri(const char *fileName) const {
        return QUrl::fromLocalFile(m_imageDir.absoluteFilePath(QLatin1String(fileName))); }

    QDir m_imageDir;
    QString m_fileProtocol;
};

Q_DECLARE_METATYPE(QMediaImageViewer::State)

#ifndef QT_NO_MULTIMEDIA
class QtTestVideoSurface : public QAbstractVideoSurface
{
public:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType) const {
        QList<QVideoFrame::PixelFormat> formats;
        if (handleType == QAbstractVideoBuffer::NoHandle) {
            formats << QVideoFrame::Format_RGB32;
        }
        return formats;
    }

    QVideoFrame frame() const { return m_frame;  }

    bool present(const QVideoFrame &frame) { m_frame = frame; return true; }

private:
    QVideoFrame m_frame;
};
#endif

void tst_QMediaImageViewer::initTestCase()
{
    qRegisterMetaType<QMediaImageViewer::State>();
}

void tst_QMediaImageViewer::isValid()
{
    QMediaImageViewer viewer;

    QVERIFY(viewer.isValid());
    QVERIFY(viewer.service() != 0);
}

void tst_QMediaImageViewer::timeout()
{
    QMediaImageViewer viewer;

    QCOMPARE(viewer.timeout(), 3000);

    viewer.setTimeout(0);
    QCOMPARE(viewer.timeout(), 0);

    viewer.setTimeout(45);
    QCOMPARE(viewer.timeout(), 45);

    viewer.setTimeout(-3000);
    QCOMPARE(viewer.timeout(), 0);
}

void tst_QMediaImageViewer::setMedia_data()
{
    QTest::addColumn<QMediaSource>("media");
    QTest::addColumn<QMediaResource>("currentMedia");

    {
        QMediaResource contentResource(imageUri("image.png"));
        QMediaSource source(contentResource);

        QTest::newRow("png image")
                << source
                << contentResource;
    } {
        QMediaResource contentResource(imageUri("song.mp3"));
        QMediaResource coverArtResource(imageUri("coverart.png"), QMediaResource::CoverArtRole);
        QMediaSource source(QMediaResourceList() << contentResource << coverArtResource);

        QTest::newRow("png cover art")
                << source
                << coverArtResource;
    } {
        QMediaResource contentResource(imageUri("movie.mp4"));
        QMediaResource posterResource(imageUri("poster.png"), QMediaResource::PosterRole);
        QMediaSource source(QMediaResourceList() << contentResource << posterResource);

        QTest::newRow("png poster")
                << source
                << posterResource;
    } {
        QMediaResource contentResource(imageUri("image.png"));
        QMediaResource coverArtResource(imageUri("coverart.png"), QMediaResource::CoverArtRole);
        QMediaResource posterResource(imageUri("poster.png"), QMediaResource::PosterRole);
        QMediaSource source(QMediaResourceList()
                << contentResource << coverArtResource << posterResource);

        QTest::newRow("png image with cover art and poster")
                << source
                << contentResource;
#ifdef QTEST_HAVE_JPEG
    } {
        QMediaResource contentResource(imageUri("image.jpg"));
        QMediaSource source(contentResource);

        QTest::newRow("jpg image")
                << source
                << contentResource;
#endif
    }
}

void tst_QMediaImageViewer::setMedia()
{
    QFETCH(QMediaSource, media);
    QFETCH(QMediaResource, currentMedia);

    QMediaImageViewer viewer;

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    viewer.setMedia(media);
    QCOMPARE(viewer.media(), media);

    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

    QTestEventLoop::instance().enterLoop(2);

    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(viewer.currentMedia(), currentMedia);
}

void tst_QMediaImageViewer::setInvalidMedia()
{
    QMediaImageViewer viewer;

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    {
        QMediaSource source(imageUri("invalid.png"));

        viewer.setMedia(source);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.currentMedia(), QMediaResource());
        QCOMPARE(viewer.media(), source);
    } {
        QMediaSource source(imageUri("deleted.png"));

        viewer.setMedia(source);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.currentMedia(), QMediaResource());
        QCOMPARE(viewer.media(), source);
    } {
        QMediaResource invalidResource(imageUri("invalid.png"));
        QMediaResource deletedResource(imageUri("deleted.png"));
        QMediaSource source(QMediaResourceList() << invalidResource << deletedResource);

        viewer.setMedia(source);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.currentMedia(), QMediaResource());
        QCOMPARE(viewer.media(), source);
    } {
        QMediaResource resource(imageUri("image.png"), QLatin1String("audio/mpeg"));
        QMediaSource source(resource);

        viewer.setMedia(source);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.currentMedia(), QMediaResource());
        QCOMPARE(viewer.media(), source);
    } {
        QMediaResource audioResource(imageUri("image.png"), QLatin1String("audio/mpeg"));
        QMediaResource invalidResource(imageUri("invalid.png"));
        QMediaSource source(QMediaResourceList() << audioResource << invalidResource);

        viewer.setMedia(source);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

        QTestEventLoop::instance().enterLoop(2);
        QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::InvalidMedia);
        QCOMPARE(viewer.currentMedia(), QMediaResource());
        QCOMPARE(viewer.media(), source);
    }
}

void tst_QMediaImageViewer::playlist()
{
    QMediaSource imageMedia(imageUri("image.png"));
    QMediaSource posterMedia(imageUri("poster.png"));
    QMediaSource coverArtMedia(imageUri("coverart.png"));

    QMediaImageViewer viewer;
    viewer.setTimeout(250);

    QVideoWidget widget(&viewer);
    widget.show();

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    QSignalSpy stateSpy(&viewer, SIGNAL(stateChanged(QMediaImageViewer::State)));

    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);

    // No playlist so can't exit stopped state.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);
    viewer.pause();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);

    QMediaPlaylist playlist(&viewer);

    // Empty playlist so can't exit stopped state.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);
    viewer.pause();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);

    playlist.appendItem(imageMedia);
    playlist.appendItem(posterMedia);
    playlist.appendItem(coverArtMedia);

    // Play progresses immediately to the first image and starts loading.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(0).value(0)),
             QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 0);
    QCOMPARE(viewer.media(), imageMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 0);

    // Time out causes progression to second image, which starts loading.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 1);
    QCOMPARE(viewer.media(), posterMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 1);

    // Pausing stops progression at current image.
    viewer.pause();
    QCOMPARE(viewer.state(), QMediaImageViewer::PausedState);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(1).value(0)),
             QMediaImageViewer::PausedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 1);

    // No time out.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::PausedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 1);

    // Resuming playback moves progress directly to the next image.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(stateSpy.count(), 3);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(2).value(0)),
             QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 2);
    QCOMPARE(viewer.media(), coverArtMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 2);

    // Time out causes progression to end of list
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(3).value(0)),
             QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);
    QCOMPARE(viewer.media(), QMediaSource());

    // Paused, no time out.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);

    // Play progresses immediately to the first image and starts loading.
    viewer.play();
    QCOMPARE(viewer.state(), QMediaImageViewer::PlayingState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(4).value(0)),
             QMediaImageViewer::PlayingState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);
    QCOMPARE(playlist.currentPosition(), 0);
    QCOMPARE(viewer.media(), imageMedia);

    // Image is loaded asynchronously.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(playlist.currentPosition(), 0);

    // Stop ends progress and removed
    viewer.stop();
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(qvariant_cast<QMediaImageViewer::State>(stateSpy.value(5).value(0)),
             QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);
    QCOMPARE(viewer.media(), QMediaSource());

    // Stoppped, No time out.
    QTestEventLoop::instance().enterLoop(2);
    QCOMPARE(viewer.state(), QMediaImageViewer::StoppedState);
    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::NoMedia);
    QCOMPARE(playlist.currentPosition(), -1);
}

void tst_QMediaImageViewer::outputControl()
{
    QMediaImageViewer viewer;

    QMediaService *service = viewer.service();
    if (service == 0)
        QSKIP("Image viewer object has no service.", SkipSingle);

    QVideoOutputControl *outputControl = qobject_cast<QVideoOutputControl *>(
            service->control(QVideoOutputControl_iid));

    QVERIFY(outputControl != 0);

    QVERIFY(outputControl->availableOutputs().contains(QVideoOutputControl::WidgetOutput));
#ifndef QT_NO_MULTIMEDIA
    QVERIFY(outputControl->availableOutputs().contains(QVideoOutputControl::RendererOutput));
#endif
    QVERIFY(!outputControl->availableOutputs().contains(QVideoOutputControl::UserOutput));

    QCOMPARE(outputControl->output(), QVideoOutputControl::NoOutput);

    outputControl->setOutput(QVideoOutputControl::WidgetOutput);
    QCOMPARE(outputControl->output(), QVideoOutputControl::WidgetOutput);

#ifndef QT_NO_MULTIMEDIA
    outputControl->setOutput(QVideoOutputControl::RendererOutput);
    QCOMPARE(outputControl->output(), QVideoOutputControl::RendererOutput);
#endif

    outputControl->setOutput(QVideoOutputControl::UserOutput);
    QCOMPARE(outputControl->output(), QVideoOutputControl::NoOutput);
}

void tst_QMediaImageViewer::widgetControl()
{
    QMediaImageViewer viewer;

    QMediaService *service = viewer.service();
    if (service == 0)
        QSKIP("Image viewer object has no service.", SkipSingle);

    QVideoOutputControl *outputControl = qobject_cast<QVideoOutputControl *>(
            service->control(QVideoOutputControl_iid));
    if (outputControl == 0)
        QSKIP("Image viewer object has no video output control.", SkipSingle);

    QVideoWidgetControl *widgetControl = qobject_cast<QVideoWidgetControl *>(
            service->control(QVideoWidgetControl_iid));
    QVERIFY(widgetControl != 0);

    QWidget *widget = widgetControl->videoWidget();
    QVERIFY(widget != 0);

    outputControl->setOutput(QVideoOutputControl::WidgetOutput);

    // Test initial values.
    QCOMPARE(widgetControl->brightness(), 0);
    QCOMPARE(widgetControl->contrast(), 0);
    QCOMPARE(widgetControl->hue(), 0);
    QCOMPARE(widgetControl->saturation(), 0);
    QCOMPARE(widgetControl->isFullScreen(), false);
    QCOMPARE(widgetControl->aspectRatio(), QVideoWidget::AspectRatioAuto);
    QCOMPARE(widgetControl->customAspectRatio(), QSize(1, 1));

    // Test setting color adjustment values.  Unsupported, so they stay at 0.
    widgetControl->setBrightness(12);
    QCOMPARE(widgetControl->brightness(), 0);
    widgetControl->setContrast(12);
    QCOMPARE(widgetControl->contrast(), 0);
    widgetControl->setHue(12);
    QCOMPARE(widgetControl->hue(), 0);
    widgetControl->setSaturation(12);
    QCOMPARE(widgetControl->saturation(), 0);

    {   // Test full screen mode.  Doesn't do anything more than adknowledge the change in mode.
        QSignalSpy fullScreenSpy(widgetControl, SIGNAL(fullScreenChanged(bool)));

        widgetControl->setFullScreen(true);
        QCOMPARE(widgetControl->isFullScreen(), true);
        QCOMPARE(fullScreenSpy.count(), 1);
        QCOMPARE(fullScreenSpy.value(0).value(0).toBool(), true);

        widgetControl->setFullScreen(false);
        QCOMPARE(widgetControl->isFullScreen(), false);
        QCOMPARE(fullScreenSpy.count(), 2);
        QCOMPARE(fullScreenSpy.value(1).value(0).toBool(), false);
    }

    // Load an image so the viewer has some dimensions to work with.
    viewer.setMedia(QMediaSource(imageUri("image.png")));

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(2);

    if (viewer.mediaStatus() != QMediaImageViewer::LoadedMedia)
        QSKIP("failed to load test image", SkipSingle);

    // If the aspect ratio is determined automatically or follows the widget the size hint
    // should the size of the image.
    QCOMPARE(widget->sizeHint(), QSize(75, 50));

    widgetControl->setAspectRatio(QVideoWidget::AspectRatioWidget);
    QCOMPARE(widgetControl->aspectRatio(), QVideoWidget::AspectRatioWidget);
    QCOMPARE(widget->sizeHint(), QSize(75, 50));

    // If the aspect ratio is explicitly set the size hint should be scaled appropriately.
    widgetControl->setAspectRatio(QVideoWidget::AspectRatioCustom);
    QCOMPARE(widgetControl->aspectRatio(), QVideoWidget::AspectRatioCustom);
    QCOMPARE(widget->sizeHint(), QSize(75, 50));

    widgetControl->setCustomAspectRatio(QSize(4, 3));
    QCOMPARE(widget->sizeHint(), QSize(100, 50));

    // A non 1:1 aspect ratio should be ignored if aspectRatio() != CustomAspectRatio.
    widgetControl->setAspectRatio(QVideoWidget::AspectRatioWidget);
    QCOMPARE(widget->sizeHint(), QSize(75, 50));
    widgetControl->setAspectRatio(QVideoWidget::AspectRatioAuto);
    QCOMPARE(widget->sizeHint(), QSize(75, 50));
}

#ifndef QT_NO_MULTIMEDIA
void tst_QMediaImageViewer::rendererControl()
{
    QtTestVideoSurface surface;

    QMediaImageViewer viewer;

    QMediaService *service = viewer.service();
    if (service == 0)
        QSKIP("Image viewer object has no service.", SkipSingle);

    QVideoOutputControl *outputControl = qobject_cast<QVideoOutputControl *>(
            service->control(QVideoOutputControl_iid));
    if (outputControl == 0)
        QSKIP("Image viewer object has no video output control.", SkipSingle);

    QVideoRendererControl *rendererControl = qobject_cast<QVideoRendererControl *>(
            service->control(QVideoRendererControl_iid));
    QVERIFY(rendererControl != 0);

    rendererControl->setSurface(&surface);
    QCOMPARE(rendererControl->surface(), &surface);

    outputControl->setOutput(QVideoOutputControl::RendererOutput);

    // Load an image so the viewer has some dimensions to work with.
    viewer.setMedia(QMediaSource(imageUri("image.png")));

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(2);

    if (viewer.mediaStatus() != QMediaImageViewer::LoadedMedia)
        QSKIP("failed to load test image", SkipSingle);

    QCOMPARE(surface.isStarted(), true);

    QVideoSurfaceFormat format = surface.surfaceFormat();
    QCOMPARE(format.handleType(), QAbstractVideoBuffer::NoHandle);
    QCOMPARE(format.pixelFormat(), QVideoFrame::Format_RGB32);
    QCOMPARE(format.frameSize(), QSize(75, 50));

    QVideoFrame frame = surface.frame();
    QCOMPARE(frame.handleType(), QAbstractVideoBuffer::NoHandle);
    QCOMPARE(frame.pixelFormat(), QVideoFrame::Format_RGB32);
    QCOMPARE(frame.size(), QSize(75, 50));

    viewer.setMedia(QMediaSource());

    QCOMPARE(surface.isStarted(), false);
}
#endif

QTEST_MAIN(tst_QMediaImageViewer)

#include "tst_qmediaimageviewer.moc"
