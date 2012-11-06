/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgraphicsvideoitem.h"
#include "qmediaplayer.h"

#include <QtCore/QFile>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QPaintEngine>
#include <QtTest/QtTest>

#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>

#ifndef QT_NO_EGL
#include <egl/egl.h>
#endif

QT_USE_NAMESPACE

static const QString FileName = "e:/test.mp4";

class QtTestGraphicsView;
class QtTestGraphicsVideoItem;

enum PreferredRenderingPath
{
    PreferredRenderingPathAuto,
    PreferredRenderingPathRenderer,
    PreferredRenderingPathDirect
};

enum ActualRenderingPath
{
    ActualRenderingPathNone,
    ActualRenderingPathRenderer = PreferredRenderingPathRenderer,
    ActualRenderingPathDirect = PreferredRenderingPathDirect
};

Q_DECLARE_METATYPE(PreferredRenderingPath)
Q_DECLARE_METATYPE(ActualRenderingPath)

#define WAIT_FOR_CONDITION(condition, ms) \
{ \
    Q_ASSERT(QCoreApplication::instance()); \
    QElapsedTimer timer; \
    timer.start(); \
    do { \
        QCoreApplication::processEvents(QEventLoop::AllEvents, ms); \
        QTest::qSleep(10); \
    } while (!(condition) && (timer.elapsed() < ms)); \
}

#define CHECKED_WAIT_FOR_CONDITION(condition, ms) \
    WAIT_FOR_CONDITION(condition, ms); \
    QVERIFY(condition);

class tst_QGraphicsVideoItemSymbian : public QObject
{
    Q_OBJECT
public slots:
    void initTestCase();
    void init();
    void cleanup();

    void play();
    void mediaPlayerError(QMediaPlayer::Error error);
    void resetInactivityTime();

private:
    void lockOrientation();
    void disableScreenSaver();
    void setPreferredRenderingPath(PreferredRenderingPath path);
    void setFullScreen(bool enabled);

    ActualRenderingPath actualRenderingPath() const;

private slots:
    void specifyPreferredRenderingPath();
    void specifyPreferredRenderingPath_data();
    void autoFullScreenIn();
    void autoFullScreenOut();

private:
    QMediaPlayer *m_mediaPlayer;
    QtTestGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QtTestGraphicsVideoItem *m_videoItem;
    bool m_eglEndpointSupport;
};

class QtTestGraphicsView : public QGraphicsView
{
public:
    QtTestGraphicsView(QWidget *parent = 0)
    :    QGraphicsView(parent)
    ,    m_paintEngineType(QPaintEngine::MaxUser)
    {

    }

    void drawBackground(QPainter *painter, const QRectF &rect)
    {
        m_paintEngineType = painter->paintEngine()->type();
        QGraphicsView::drawBackground(painter, rect);
    }

    QPaintEngine::Type paintEngineType() const
    {
        return m_paintEngineType;
    }

private:
    QPaintEngine::Type m_paintEngineType;
};

class QtTestGraphicsVideoItem : public QGraphicsVideoItem
{
public:
    QtTestGraphicsVideoItem(QGraphicsItem *parent = 0)
    :   QGraphicsVideoItem(parent)
    ,   m_prevPaintCount(0)
    ,   m_paintCount(0)
    {

    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        ++m_paintCount;
        QTestEventLoop::instance().exitLoop();
        QGraphicsVideoItem::paint(painter, option, widget);
    }

    void savePaintCount()
    {
        m_prevPaintCount = m_paintCount;
    }

    void waitForPaint(bool save = false)
    {
        if (save)
            savePaintCount();
        static const int required = 2;
        static const int secs = 5;
        CHECKED_WAIT_FOR_CONDITION(m_paintCount >= m_prevPaintCount + required, secs * 1000);
        m_prevPaintCount = m_paintCount;
    }

    int paintCount() const { return m_paintCount; }

private:
    int m_prevPaintCount;
    int m_paintCount;
};

void tst_QGraphicsVideoItemSymbian::initTestCase()
{
    lockOrientation();
    disableScreenSaver();
    m_mediaPlayer = 0;
    m_view = 0;
    m_scene = 0;
    m_videoItem = 0;
    m_eglEndpointSupport = false;
#ifndef QT_NO_EGL
    const EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display != EGL_NO_DISPLAY) {
        if (eglInitialize(display, 0, 0) == EGL_TRUE)
            m_eglEndpointSupport = (eglGetProcAddress("eglCreateEndpointNOK") != 0);
    }
#endif
    qRegisterMetaType<PreferredRenderingPath>();
    qRegisterMetaType<ActualRenderingPath>();
}

void tst_QGraphicsVideoItemSymbian::init()
{
    // Framework does not automatically call cleanup() if previous case skipped
    cleanup();
    if (QFile::exists(FileName)) {
        QVERIFY(!m_mediaPlayer);
        QVERIFY(!m_view);
        QVERIFY(!m_scene);
        QVERIFY(!m_videoItem);
        m_mediaPlayer = new QMediaPlayer();
        connect(m_mediaPlayer, SIGNAL(error(QMediaPlayer::Error)),
                this, SLOT(mediaPlayerError(QMediaPlayer::Error)));
        m_view = new QtTestGraphicsView();
        m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_scene = new QGraphicsScene();
        m_videoItem = new QtTestGraphicsVideoItem();
        m_videoItem->setSize(QSizeF(200, 200));
        m_view->setScene(m_scene);
        m_view->setBackgroundBrush(Qt::blue);
        m_view->showFullScreen();
        QTest::qWaitForWindowShown(m_view);
        const QPaintEngine::Type type = m_view->paintEngineType();
        if (QPaintEngine::OpenGL != type && QPaintEngine::OpenVG != type)
            QSKIP(QString("Test case is only valid when using opengl or openvg graphics system (paint engine type %1)").arg(type).toAscii(), SkipAll);
    } else {
        QSKIP(QString("Test file %1 not found").arg(FileName).toAscii(), SkipAll);
    }
}

void tst_QGraphicsVideoItemSymbian::cleanup()
{
    delete m_videoItem;
    m_videoItem = 0;
    delete m_scene;
    m_scene = 0;
    delete m_view;
    m_view = 0;
    delete m_mediaPlayer;
    m_mediaPlayer = 0;
}

void tst_QGraphicsVideoItemSymbian::play()
{
    m_mediaPlayer->setMedia(QUrl::fromLocalFile(FileName));
    m_mediaPlayer->setVideoOutput(m_videoItem);
    m_scene->addItem(m_videoItem);
    m_view->centerOn(m_videoItem);
    m_mediaPlayer->play();
    CHECKED_WAIT_FOR_CONDITION(QMediaPlayer::BufferedMedia == m_mediaPlayer->mediaStatus(), 3000);
}

void tst_QGraphicsVideoItemSymbian::mediaPlayerError(QMediaPlayer::Error error)
{
    QVERIFY(QMediaPlayer::NoError == error);
}

void tst_QGraphicsVideoItemSymbian::setPreferredRenderingPath(PreferredRenderingPath path)
{
    static const QString property = "_q_preferredVideoRenderingPath";
    QString value;
    switch (path) {
    case PreferredRenderingPathAuto:
        value = "auto";
        break;
    case PreferredRenderingPathRenderer:
        value = "renderer";
        break;
    case PreferredRenderingPathDirect:
        value = "direct";
        break;
    }
    m_videoItem->setProperty(property.toAscii(), value);
}

void tst_QGraphicsVideoItemSymbian::setFullScreen(bool enabled)
{
    const QSizeF size = enabled ? QApplication::desktop()->size() : QSizeF(200, 200);
    m_videoItem->setSize(size);
    const Qt::AspectRatioMode mode = enabled ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio;
    m_videoItem->setAspectRatioMode(mode);
}

ActualRenderingPath tst_QGraphicsVideoItemSymbian::actualRenderingPath() const
{
    ActualRenderingPath path = ActualRenderingPathNone;
    static const QString property = "_q_currentVideoRenderingPath";
    const QString value = m_videoItem->property(property.toAscii()).value<QString>();
    if ("direct" == value)
        path = ActualRenderingPathDirect;
    else if ("renderer" == value)
        path = ActualRenderingPathRenderer;
    return path;
}

void tst_QGraphicsVideoItemSymbian::lockOrientation()
{
    CAknAppUi* appUi = dynamic_cast<CAknAppUi*>(CEikonEnv::Static()->AppUi());
    QVERIFY(appUi);
    TRAPD(err, appUi->SetOrientationL(CAknAppUi::EAppUiOrientationLandscape));
    QVERIFY(!err);
}

void tst_QGraphicsVideoItemSymbian::disableScreenSaver()
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(resetInactivityTime()));
    timer->start(100);
    resetInactivityTime();
}

void tst_QGraphicsVideoItemSymbian::resetInactivityTime()
{
    User::ResetInactivityTime();
}

void tst_QGraphicsVideoItemSymbian::specifyPreferredRenderingPath()
{
    QFETCH(PreferredRenderingPath, preferredRenderingPath);
    QFETCH(ActualRenderingPath, actualRenderingPath);
    setPreferredRenderingPath(preferredRenderingPath);
    play();
    QVERIFY(this->actualRenderingPath() == actualRenderingPath);
}

void tst_QGraphicsVideoItemSymbian::specifyPreferredRenderingPath_data()
{
    QTest::addColumn<PreferredRenderingPath>("preferredRenderingPath");
    QTest::addColumn<ActualRenderingPath>("actualRenderingPath");
        QTest::newRow("direct") << PreferredRenderingPathDirect
                                << ActualRenderingPathDirect;
    if (m_eglEndpointSupport)
        QTest::newRow("renderer") << PreferredRenderingPathRenderer
                                  << ActualRenderingPathRenderer;
    else
        QTest::newRow("renderer") << PreferredRenderingPathRenderer
                                  << ActualRenderingPathDirect;
}

void tst_QGraphicsVideoItemSymbian::autoFullScreenIn()
{
    setPreferredRenderingPath(PreferredRenderingPathAuto);
    play();
    if (m_eglEndpointSupport)
        QVERIFY(actualRenderingPath() == ActualRenderingPathRenderer);
    else
        QVERIFY(actualRenderingPath() == ActualRenderingPathDirect);
    m_videoItem->savePaintCount();
    setFullScreen(true);
    m_videoItem->waitForPaint();
    QVERIFY(actualRenderingPath() == ActualRenderingPathDirect);
}

void tst_QGraphicsVideoItemSymbian::autoFullScreenOut()
{
    setPreferredRenderingPath(PreferredRenderingPathAuto);
    play();
    setFullScreen(true);
    m_videoItem->waitForPaint();
    QVERIFY(actualRenderingPath() == ActualRenderingPathDirect);
    m_videoItem->savePaintCount();
    setFullScreen(false);
    m_videoItem->waitForPaint();
    if (m_eglEndpointSupport)
        QVERIFY(actualRenderingPath() == ActualRenderingPathRenderer);
    else
        QVERIFY(actualRenderingPath() == ActualRenderingPathDirect);
}

QTEST_MAIN(tst_QGraphicsVideoItemSymbian)

#include "tst_qgraphicsvideoitem_symbian.moc"

