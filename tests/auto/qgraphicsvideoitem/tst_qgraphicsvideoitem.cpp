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

#include <QtTest/QtTest>

#include "multimedia/qgraphicsvideoitem.h"

#include "multimedia/qmediaobject.h"
#include "multimedia/qmediaservice.h"
#include "multimedia/qvideooutputcontrol.h"
#include "multimedia/qvideorenderercontrol.h"

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
    QVERIFY(object.testService->rendererControl->surface() != 0);

    item.hide();
    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    item.show();
    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);
    QVERIFY(object.testService->rendererControl->surface() != 0);
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

QTEST_MAIN(tst_QGraphicsVideoItem)

#include "tst_qgraphicsvideoitem.moc"
