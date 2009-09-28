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
#include <multimedia/qvideowidget.h>

class tst_QMediaImageViewer : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void isValid();
    void setMedia_data();
    void setMedia();
    void playlist();

public:
    tst_QMediaImageViewer()
        : m_imageDir(QLatin1String(TESTDATA_DIR))
#ifndef Q_OS_WIN
        , m_fileProtocol(QLatin1String("file://"))
#else
        , m_fileProtocol(QLatin1String("file:///"))
#endif
    {}

private:
    QUrl imageUri(const char *fileName) const {
        return QUrl(m_fileProtocol + m_imageDir.absoluteFilePath(QLatin1String(fileName))); }

    QDir m_imageDir;
    QString m_fileProtocol;
};

Q_DECLARE_METATYPE(QMediaImageViewer::State)

void tst_QMediaImageViewer::initTestCase()
{
    qRegisterMetaType<QMediaImageViewer::State>();
}

void tst_QMediaImageViewer::isValid()
{
    QMediaImageViewer viewer;

    QVERIFY(viewer.isValid());
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
    QVideoWidget widget(&viewer);
    widget.show();

    connect(&viewer, SIGNAL(mediaStatusChanged(QMediaImageViewer::MediaStatus)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    viewer.setMedia(media);
    QCOMPARE(viewer.media(), media);

    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadingMedia);

    QTestEventLoop::instance().enterLoop(2);

    QCOMPARE(viewer.mediaStatus(), QMediaImageViewer::LoadedMedia);
    QCOMPARE(viewer.currentMedia(), currentMedia);
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

QTEST_MAIN(tst_QMediaImageViewer)

#include "tst_qmediaimageviewer.moc"
