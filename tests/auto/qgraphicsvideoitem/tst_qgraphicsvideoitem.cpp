/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
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

#include <QtTest/QtTest>

#include "qgraphicsvideoitem.h"

#include "qmediaobject.h"
#include "qmediaservice.h"
#include "qvideooutputcontrol.h"
#include "qvideorenderercontrol.h"

#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

#include <QtGui/qapplication.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicsview.h>

class tst_QGraphicsVideoItem : public QObject
{
    Q_OBJECT
private slots:
    void nullObject();
    void nullService();
    void nullOutputControl();
    void noOutputs();
    void serviceDestroyed();

    void show();

    void boundingRect_data();
    void boundingRect();

    void rendererSupportedFormat_data();
    void rendererSupportedFormat();

    void rendererPresent_data();
    void rendererPresent();
};

Q_DECLARE_METATYPE(const uchar *)

class QtTestOutputControl : public QVideoOutputControl
{
public:
    QtTestOutputControl() : m_output(NoOutput) {}

    QList<Output> availableOutputs() const { return m_outputs; }
    void setAvailableOutputs(const QList<Output> outputs) { m_outputs = outputs; }

    Output output() const { return m_output; }
    virtual void setOutput(Output output) { m_output = output; }

private:
    Output m_output;
    QList<Output> m_outputs;
};

class QtTestRendererControl : public QVideoRendererControl
{
public:
    QtTestRendererControl()
        : m_surface(0)
    {
    }

    QAbstractVideoSurface *surface() const { return m_surface; }
    void setSurface(QAbstractVideoSurface *surface) { m_surface = surface; }

private:
    QAbstractVideoSurface *m_surface;
};

class QtTestVideoService : public QMediaService
{
    Q_OBJECT
public:
    QtTestVideoService(
            QtTestOutputControl *output,
            QtTestRendererControl *renderer)
        : QMediaService(0)
        , outputControl(output)
        , rendererControl(renderer)
    {
    }

    ~QtTestVideoService()
    {
        delete outputControl;
        delete rendererControl;
    }

    QMediaControl *control(const char *name) const
    {
        if (qstrcmp(name, QVideoOutputControl_iid) == 0)
            return outputControl;
        else if (qstrcmp(name, QVideoRendererControl_iid) == 0)
            return rendererControl;
        else
            return 0;
    }

    QtTestOutputControl *outputControl;
    QtTestRendererControl *rendererControl;
};

class QtTestVideoObject : public QMediaObject
{
    Q_OBJECT
public:
    QtTestVideoObject(QtTestRendererControl *renderer):
        QMediaObject(0, new QtTestVideoService(new QtTestOutputControl, renderer))
    {
        testService = qobject_cast<QtTestVideoService*>(service());
        QList<QVideoOutputControl::Output> outputs;

        if (renderer)
            outputs.append(QVideoOutputControl::RendererOutput);

        testService->outputControl->setAvailableOutputs(outputs);
    }

    QtTestVideoObject(QtTestVideoService *service):
        QMediaObject(0, service),
        testService(service)
    {
    }

    ~QtTestVideoObject()
    {
        delete testService;
    }

    QtTestVideoService *testService;
};

class QtTestGraphicsVideoItem : public QGraphicsVideoItem
{
public:
    QtTestGraphicsVideoItem(QMediaObject *object, QGraphicsItem *parent = 0)
        : QGraphicsVideoItem(object, parent)
        , m_paintCount(0)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        ++m_paintCount;

        QTestEventLoop::instance().exitLoop();

        QGraphicsVideoItem::paint(painter, option, widget);
    }

    bool waitForPaint(int secs)
    {
        const int paintCount = m_paintCount;

        QTestEventLoop::instance().enterLoop(secs);

        return m_paintCount != paintCount;
    }

private:
    int m_paintCount;
};

void tst_QGraphicsVideoItem::nullObject()
{
    QGraphicsVideoItem item(0);

    QVERIFY(item.boundingRect().isEmpty());
}

void tst_QGraphicsVideoItem::nullService()
{
    QtTestVideoService *service = 0;

    QtTestVideoObject object(service);

    QGraphicsVideoItem item(&object);

    QVERIFY(item.boundingRect().isEmpty());

}

void tst_QGraphicsVideoItem::nullOutputControl()
{
    QtTestVideoObject object(new QtTestVideoService(0, 0));

    QGraphicsVideoItem item(&object);

    QVERIFY(item.boundingRect().isEmpty());
}

void tst_QGraphicsVideoItem::noOutputs()
{
    QtTestRendererControl *control = 0;
    QtTestVideoObject object(control);

    QGraphicsVideoItem item(&object);

    QVERIFY(item.boundingRect().isEmpty());
}

void tst_QGraphicsVideoItem::serviceDestroyed()
{
    QtTestVideoObject *object = new QtTestVideoObject(new QtTestRendererControl);

    QGraphicsVideoItem item(object);

    delete object;

    QVERIFY(item.boundingRect().isEmpty());
}

void tst_QGraphicsVideoItem::show()
{
    QtTestVideoObject object(new QtTestRendererControl);
    QGraphicsVideoItem item(&object);

    // Graphics items are visible by default
    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);

    item.hide();
    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    item.show();
    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);
}

void tst_QGraphicsVideoItem::boundingRect_data()
{
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<QRect>("viewport");
    QTest::addColumn<QSize>("pixelAspectRatio");
    QTest::addColumn<QRectF>("expectedRect");

    QTest::newRow("640x480")
            << QSize(640, 480)
            << QRect(0, 0, 640, 480)
            << QSize(1, 1)
            << QRectF(-319, -239, 640, 480);

    QTest::newRow("800x600, (80,60, 640x480) viewport")
            << QSize(800, 600)
            << QRect(80, 60, 640, 480)
            << QSize(1, 1)
            << QRectF(-319,-239, 640, 480);

    QTest::newRow("800x600, (80,60, 640x480) viewport, 4:3")
            << QSize(800, 600)
            << QRect(80, 60, 640, 480)
            << QSize(4, 3)
            << QRectF(-426, -239, 853, 480);

}

void tst_QGraphicsVideoItem::boundingRect()
{
    QFETCH(QSize, frameSize);
    QFETCH(QRect, viewport);
    QFETCH(QSize, pixelAspectRatio);
    QFETCH(QRectF, expectedRect);

    QtTestVideoObject object(new QtTestRendererControl);
    QGraphicsVideoItem item(&object);

    QVideoSurfaceFormat format(frameSize, QVideoFrame::Format_ARGB32);
    format.setViewport(viewport);
    format.setPixelAspectRatio(pixelAspectRatio);

    QVERIFY(object.testService->rendererControl->surface()->start(format));

    QCOMPARE(item.boundingRect(), expectedRect);
}

static const uchar rgb32ImageData[] =
{
    0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00,
    0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00
};

static const uchar argb32ImageData[] =
{
    0x00, 0xff, 0x00, 0x00, 0xcc, 0x00, 0xff, 0xcc,
    0x77, 0x00, 0x00, 0x77, 0x00, 0xff, 0xff, 0x00
};

static const uchar rgb24ImageData[] =
{
    0x00, 0xff, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x00,
    0xcc, 0x00, 0xcc, 0x77, 0xff, 0x77, 0x00, 0x00
};

static const uchar rgb565ImageData[] =
{
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00
};

void tst_QGraphicsVideoItem::rendererSupportedFormat_data()
{
    QTest::addColumn<QAbstractVideoBuffer::HandleType>("handleType");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormat");
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<bool>("supportedPixelFormat");
    QTest::addColumn<bool>("supportedFormat");

    QTest::newRow("rgb32 640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB32
            << QSize(640, 480)
            << true
            << true;
    QTest::newRow("rgb32 -640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB32
            << QSize(-640, 480)
            << true
            << false;
    QTest::newRow("rgb24 1024x768")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(1024, 768)
            << true
            << true;
    QTest::newRow("rgb24 -1024x-768")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(-1024, -768)
            << true
            << false;
    QTest::newRow("rgb565 0x0")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(0, 0)
            << true
            << false;
    QTest::newRow("rgb32 (OpenGL) 640x480")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_RGB32
            << QSize(640, 480)
            << false
            << false;
}

void tst_QGraphicsVideoItem::rendererSupportedFormat()
{
    QFETCH(QAbstractVideoBuffer::HandleType, handleType);
    QFETCH(QVideoFrame::PixelFormat, pixelFormat);
    QFETCH(QSize, frameSize);
    QFETCH(bool, supportedPixelFormat);
    QFETCH(bool, supportedFormat);

    QtTestVideoObject object(new QtTestRendererControl);
    QGraphicsVideoItem item(&object);

    QAbstractVideoSurface *surface = object.testService->rendererControl->surface();

    QVERIFY(surface);

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface->supportedPixelFormats(handleType);

    QCOMPARE(pixelFormats.contains(pixelFormat), QBool(supportedPixelFormat));

    QVideoSurfaceFormat format(frameSize, pixelFormat, handleType);

    QCOMPARE(surface->isFormatSupported(format), supportedFormat);
    QCOMPARE(surface->start(format), supportedFormat);
}

void tst_QGraphicsVideoItem::rendererPresent_data()
{
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormatA");
    QTest::addColumn<QSize>("frameSizeA");
    QTest::addColumn<const uchar *>("frameDataA");
    QTest::addColumn<int>("bytesA");
    QTest::addColumn<int>("bytesPerLineA");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormatB");
    QTest::addColumn<QSize>("frameSizeB");
    QTest::addColumn<const uchar *>("frameDataB");
    QTest::addColumn<int>("bytesB");
    QTest::addColumn<int>("bytesPerLineB");

    QTest::newRow("rgb32 -> argb32")
            << QVideoFrame::Format_RGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb32ImageData)
            << int(sizeof(rgb32ImageData))
            << 8
            << QVideoFrame::Format_ARGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(argb32ImageData)
            << int(sizeof(argb32ImageData))
            << 8;

    QTest::newRow("rgb32 -> rgb24")
            << QVideoFrame::Format_RGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb32ImageData)
            << int(sizeof(rgb32ImageData))
            << 8
            << QVideoFrame::Format_RGB24
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb24ImageData)
            << int(sizeof(rgb24ImageData))
            << 8;

    QTest::newRow("rgb32 -> rgb565")
            << QVideoFrame::Format_RGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb32ImageData)
            << int(sizeof(rgb32ImageData))
            << 8
            << QVideoFrame::Format_RGB565
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb565ImageData)
            << int(sizeof(rgb565ImageData))
            << 4;

    QTest::newRow("rgb24 -> rgb565")
            << QVideoFrame::Format_RGB24
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb24ImageData)
            << int(sizeof(rgb24ImageData))
            << 8
            << QVideoFrame::Format_RGB565
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb565ImageData)
            << int(sizeof(rgb565ImageData))
            << 4;
}

void tst_QGraphicsVideoItem::rendererPresent()
{
    QFETCH(QVideoFrame::PixelFormat, pixelFormatA);
    QFETCH(QSize, frameSizeA);
    QFETCH(const uchar *, frameDataA);
    QFETCH(int, bytesA);
    QFETCH(int, bytesPerLineA);
    QFETCH(QVideoFrame::PixelFormat, pixelFormatB);
    QFETCH(QSize, frameSizeB);
    QFETCH(const uchar *, frameDataB);
    QFETCH(int, bytesB);
    QFETCH(int, bytesPerLineB);

    QtTestVideoObject object(new QtTestRendererControl);

    QtTestGraphicsVideoItem *item = new QtTestGraphicsVideoItem(&object);
    QGraphicsScene graphicsScene;
    graphicsScene.addItem(item);
    QGraphicsView graphicsView(&graphicsScene);
    graphicsView.show();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);
    QVERIFY(object.testService->rendererControl->surface());

    QAbstractVideoSurface *surface = object.testService->rendererControl->surface();

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface->supportedPixelFormats();

    QVideoSurfaceFormat formatA(frameSizeA, pixelFormatA);

    QVERIFY(surface->start(formatA));
    QVERIFY(surface->isStarted());

    QVideoFrame frameA(bytesA, frameSizeA, bytesPerLineA, pixelFormatA);

    frameA.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frameA.bits(), frameDataA, frameA.numBytes());
    frameA.unmap();

    QVERIFY(surface->present(frameA));

    QVERIFY(item->waitForPaint(1));

    QVERIFY(surface->isStarted());

    // Try switching to a different format after starting.
    QVideoSurfaceFormat formatB(frameSizeB, pixelFormatB);

    QVERIFY(surface->start(formatB));
    QVERIFY(surface->isStarted());

    QVideoFrame frameB(bytesB, frameSizeB, bytesPerLineB, pixelFormatB);

    frameB.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frameB.bits(), frameDataB, frameB.numBytes());
    frameB.unmap();

    QVERIFY(surface->present(frameB));

    QVERIFY(item->waitForPaint(1));

    QVERIFY(surface->isStarted());

    surface->stop();

    QVERIFY(!surface->isStarted());

    // Try presenting a frame while stopped.
    QVERIFY(!surface->present(frameB));
    QCOMPARE(surface->error(), QAbstractVideoSurface::StoppedError);

    // Try presenting a frame with a different format.
    QVERIFY(surface->start(formatB));
    QVERIFY(!surface->present(frameA));
    QVERIFY(!surface->isStarted());
    QCOMPARE(surface->error(), QAbstractVideoSurface::IncorrectFormatError);
}


QTEST_MAIN(tst_QGraphicsVideoItem)

#include "tst_qgraphicsvideoitem.moc"
