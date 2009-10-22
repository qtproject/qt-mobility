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

#include "qvideowidget.h"

#include "qmediaobject.h"
#include "qmediaservice.h"
#include "qvideooutputcontrol.h"
#include "qvideowindowcontrol.h"
#include "qvideowidgetcontrol.h"

#ifndef QT_NO_MULTIMEDIA
#include "qvideorenderercontrol.h"
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>
#endif

#include <QtGui/qapplication.h>

class tst_QVideoWidget : public QObject
{
    Q_OBJECT
private slots:
    void nullObject();
    void nullService();
    void nullOutputControl();
    void noOutputs();
    void serviceDestroyed();

    void showWindowControl();
    void fullScreenWindowControl();
    void aspectRatioWindowControl();
    void sizeHintWindowControl_data() { sizeHint_data(); }
    void sizeHintWindowControl();
    void brightnessWindowControl_data() { color_data(); }
    void brightnessWindowControl();
    void contrastWindowControl_data() { color_data(); }
    void contrastWindowControl();
    void hueWindowControl_data() { color_data(); }
    void hueWindowControl();
    void saturationWindowControl_data() { color_data(); }
    void saturationWindowControl();

    void showWidgetControl();
    void fullScreenWidgetControl();
    void aspectRatioWidgetControl();
    void sizeHintWidgetControl_data() { sizeHint_data(); }
    void sizeHintWidgetControl();
    void brightnessWidgetControl_data() { color_data(); }
    void brightnessWidgetControl();
    void contrastWidgetControl_data() { color_data(); }
    void contrastWidgetControl();
    void hueWidgetControl_data() { color_data(); }
    void hueWidgetControl();
    void saturationWidgetControl_data() { color_data(); }
    void saturationWidgetControl();

#ifndef QT_NO_MULTIMEDIA
    void showRendererControl();
    void fullScreenRendererControl();
    void aspectRatioRendererControl();
    void sizeHintRendererControl_data();
    void sizeHintRendererControl();
    void brightnessRendererControl_data() { color_data(); }
    void brightnessRendererControl();
    void contrastRendererControl_data() { color_data(); }
    void contrastRendererControl();
    void hueRendererControl_data() { color_data(); }
    void hueRendererControl();
    void saturationRendererControl_data() { color_data(); }
    void saturationRendererControl();

    void rendererSupportedFormat_data();
    void rendererSupportedFormat();

    void rendererPresent_data();
    void rendererPresent();
#endif

private:
    void sizeHint_data();
    void color_data();
};

Q_DECLARE_METATYPE(QVideoWidget::AspectRatioMode)
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

class QtTestWindowControl : public QVideoWindowControl
{
public:
    QtTestWindowControl()
        : m_winId(0)
        , m_repaintCount(0)
        , m_brightness(0)
        , m_contrast(0)
        , m_saturation(0)
        , m_aspectRatioMode(QVideoWidget::KeepAspectRatio)
        , m_fullScreen(0)
    {
    }

    WId winId() const { return m_winId; }
    void setWinId(WId id) { m_winId = id; }

    QRect displayRect() const { return m_displayRect; }
    void setDisplayRect(const QRect &rect) { m_displayRect = rect; }

    bool isFullScreen() const { return m_fullScreen; }
    void setFullScreen(bool fullScreen) { emit fullScreenChanged(m_fullScreen = fullScreen); }

    int repaintCount() const { return m_repaintCount; }
    void setRepaintCount(int count) { m_repaintCount = count; }
    void repaint() { ++m_repaintCount; }

    QSize nativeSize() const { return m_nativeSize; }
    void setNativeSize(const QSize &size) { m_nativeSize = size; emit nativeSizeChanged(); }

    QVideoWidget::AspectRatioMode aspectRatioMode() const { return m_aspectRatioMode; }
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode) { m_aspectRatioMode = mode; }

    int brightness() const { return m_brightness; }
    void setBrightness(int brightness) { emit brightnessChanged(m_brightness = brightness); }

    int contrast() const { return m_contrast; }
    void setContrast(int contrast) { emit contrastChanged(m_contrast = contrast); }

    int hue() const { return m_hue; }
    void setHue(int hue) { emit hueChanged(m_hue = hue); }

    int saturation() const { return m_saturation; }
    void setSaturation(int saturation) { emit saturationChanged(m_saturation = saturation); }

private:
    WId m_winId;
    int m_repaintCount;
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
    QRect m_displayRect;
    QSize m_nativeSize;
    bool m_fullScreen;
};

class QtTestWidgetControl : public QVideoWidgetControl
{
public:
    QtTestWidgetControl()
        : m_brightness(1.0)
        , m_contrast(1.0)
        , m_hue(1.0)
        , m_saturation(1.0)
        , m_aspectRatioMode(QVideoWidget::KeepAspectRatio)
        , m_fullScreen(false)
    {
    }

    bool isFullScreen() const { return m_fullScreen; }
    void setFullScreen(bool fullScreen) { emit fullScreenChanged(m_fullScreen = fullScreen); }

    QVideoWidget::AspectRatioMode aspectRatioMode() const { return m_aspectRatioMode; }
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode) { m_aspectRatioMode = mode; }

    int brightness() const { return m_brightness; }
    void setBrightness(int brightness) { emit brightnessChanged(m_brightness = brightness); }

    int contrast() const { return m_contrast; }
    void setContrast(int contrast) { emit contrastChanged(m_contrast = contrast); }

    int hue() const { return m_hue; }
    void setHue(int hue) { emit hueChanged(m_hue = hue); }

    int saturation() const { return m_saturation; }
    void setSaturation(int saturation) { emit saturationChanged(m_saturation = saturation); }

    void setSizeHint(const QSize &size) { m_widget.setSizeHint(size); }

    QWidget *videoWidget() { return &m_widget; }

private:
    class Widget : public QWidget
    {
    public:
        QSize sizeHint() const { return m_sizeHint; }
        void setSizeHint(const QSize &size) { m_sizeHint = size; }
    private:
        QSize m_sizeHint;
    } m_widget;
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
    QSize m_sizeHint;
    bool m_fullScreen;
};

#ifndef QT_NO_MULTIMEDIA
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
#else
class QtTestRendererControl;
#endif

class QtTestVideoService : public QMediaService
{
    Q_OBJECT
public:
    QtTestVideoService(
            QtTestOutputControl *output,
            QtTestWindowControl *window,
            QtTestWidgetControl *widget,
            QtTestRendererControl *renderer)
        : QMediaService(0)
        , outputControl(output)
        , windowControl(window)
        , widgetControl(widget)
        , rendererControl(renderer)
    {
    }

    ~QtTestVideoService()
    {
        delete outputControl;
        delete windowControl;
        delete widgetControl;
#ifndef QT_NO_MULTIMEDIA
        delete rendererControl;
#endif
    }

    QMediaControl *control(const char *name) const
    {
        if (qstrcmp(name, QVideoOutputControl_iid) == 0)
            return outputControl;
        else if (qstrcmp(name, QVideoWindowControl_iid) == 0)
            return windowControl;
        else if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
            return widgetControl;
#ifndef QT_NO_MULTIMEDIA
        else if (qstrcmp(name, QVideoRendererControl_iid) == 0)
            return rendererControl;
#endif
        else
            return 0;
    }

    QtTestOutputControl *outputControl;
    QtTestWindowControl *windowControl;
    QtTestWidgetControl *widgetControl;
    QtTestRendererControl *rendererControl;
};

class QtTestVideoObject : public QMediaObject
{
    Q_OBJECT
public:
    QtTestVideoObject(
            QtTestWindowControl *window,
            QtTestWidgetControl *widget,
            QtTestRendererControl *renderer):
        QMediaObject(0, new QtTestVideoService(new QtTestOutputControl, window, widget, renderer))
    {
        testService = qobject_cast<QtTestVideoService*>(service());
        QList<QVideoOutputControl::Output> outputs;

        if (window)
            outputs.append(QVideoOutputControl::WindowOutput);
        if (widget)
            outputs.append(QVideoOutputControl::WidgetOutput);
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

class QtTestVideoWidget : public QVideoWidget
{
public:
    QtTestVideoWidget(QMediaObject *object, QWidget *parent = 0)
        : QVideoWidget(object, parent)
        , m_paintCount(0)
    {
    }

    bool waitForPaint(int secs)
    {
        const int paintCount = m_paintCount;

        QTestEventLoop::instance().enterLoop(secs);

        return m_paintCount != paintCount;
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        ++m_paintCount;

        QTestEventLoop::instance().exitLoop();

        QVideoWidget::paintEvent(event);
    }

private:
    int m_paintCount;
};

void tst_QVideoWidget::nullObject()
{
    QVideoWidget widget(0);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);

    {
        QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

        widget.setBrightness(100);
        QCOMPARE(widget.brightness(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setBrightness(100);
        QCOMPARE(widget.brightness(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setBrightness(-120);
        QCOMPARE(widget.brightness(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    } {
        QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

        widget.setContrast(100);
        QCOMPARE(widget.contrast(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setContrast(100);
        QCOMPARE(widget.contrast(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setContrast(-120);
        QCOMPARE(widget.contrast(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    } {
        QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

        widget.setHue(100);
        QCOMPARE(widget.hue(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setHue(100);
        QCOMPARE(widget.hue(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setHue(-120);
        QCOMPARE(widget.hue(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    } {
        QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

        widget.setSaturation(100);
        QCOMPARE(widget.saturation(), 100);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.value(0).value(0).toInt(), 100);

        widget.setSaturation(100);
        QCOMPARE(widget.saturation(), 100);
        QCOMPARE(spy.count(), 1);

        widget.setSaturation(-120);
        QCOMPARE(widget.saturation(), -100);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.value(1).value(0).toInt(), -100);
    }
}

void tst_QVideoWidget::nullService()
{
    QtTestVideoObject object(0);

    QVideoWidget widget(&object);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 100);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 100);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 100);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 100);
}

void tst_QVideoWidget::nullOutputControl()
{
    QtTestVideoObject object(new QtTestVideoService(0, 0, 0, 0));

    QVideoWidget widget(&object);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 100);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 100);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 100);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 100);
}

void tst_QVideoWidget::noOutputs()
{
    QtTestVideoObject object(0, 0, 0);

    QVideoWidget widget(&object);

    QVERIFY(widget.sizeHint().isEmpty());

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 100);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 100);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 100);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 100);
}

void tst_QVideoWidget::serviceDestroyed()
{
    QtTestVideoObject *object = new QtTestVideoObject(
            new QtTestWindowControl,
            new QtTestWidgetControl,
            0);

    QVideoWidget widget(object);

    widget.show();

    widget.setBrightness(100);
    widget.setContrast(100);
    widget.setHue(100);
    widget.setSaturation(100);

    delete object;

    QCOMPARE(widget.brightness(), 100);
    QCOMPARE(widget.contrast(), 100);
    QCOMPARE(widget.hue(), 100);
    QCOMPARE(widget.saturation(), 100);

    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);
}

void tst_QVideoWidget::showWindowControl()
{
    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setNativeSize(QSize(240, 180));

    QtTestVideoWidget widget(&object);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.show();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::WindowOutput);
    QVERIFY(object.testService->windowControl->winId() != 0);

    if (!widget.waitForPaint(1))
        QSKIP("Didn't receive a paint event in a timely fashion", SkipSingle);

    QVERIFY(object.testService->windowControl->repaintCount() > 0);

    widget.hide();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);
}

void tst_QVideoWidget::showWidgetControl()
{
    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    QVideoWidget widget(&object);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.show();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::WidgetOutput);
    QCOMPARE(object.testService->widgetControl->videoWidget()->isVisible(), true);

    widget.hide();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);
    QCOMPARE(object.testService->widgetControl->videoWidget()->isVisible(), false);
}

#ifndef QT_NO_MULTIMEDIA
void tst_QVideoWidget::showRendererControl()
{
    QtTestVideoObject object(0, 0, new QtTestRendererControl);
    QVideoWidget widget(&object);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);

    widget.show();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);

    widget.hide();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);
}
#endif

void tst_QVideoWidget::aspectRatioWindowControl()
{
    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setAspectRatioMode(QVideoWidget::IgnoreAspectRatio);

    QVideoWidget widget(&object);

    // Test the aspect ratio defaults to keeping the aspect ratio.
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);

    // Test the control has been informed of the aspect ratio change, post show.
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
    QCOMPARE(object.testService->windowControl->aspectRatioMode(), QVideoWidget::KeepAspectRatio);

    // Test an aspect ratio change is enforced immediately while visible.
    widget.setAspectRatioMode(QVideoWidget::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::IgnoreAspectRatio);
    QCOMPARE(object.testService->windowControl->aspectRatioMode(), QVideoWidget::IgnoreAspectRatio);

    // Test an aspect ratio set while not visible is respected.
    widget.hide();
    widget.setAspectRatioMode(QVideoWidget::KeepAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
    QCOMPARE(object.testService->windowControl->aspectRatioMode(), QVideoWidget::KeepAspectRatio);
}

void tst_QVideoWidget::aspectRatioWidgetControl()
{
    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setAspectRatioMode(QVideoWidget::IgnoreAspectRatio);

    QVideoWidget widget(&object);

    // Test the aspect ratio defaults to keeping the aspect ratio.
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);

    // Test the control has been informed of the aspect ratio change, post show.
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
    QCOMPARE(object.testService->widgetControl->aspectRatioMode(), QVideoWidget::KeepAspectRatio);

    // Test an aspect ratio change is enforced immediately while visible.
    widget.setAspectRatioMode(QVideoWidget::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::IgnoreAspectRatio);
    QCOMPARE(object.testService->widgetControl->aspectRatioMode(), QVideoWidget::IgnoreAspectRatio);

    // Test an aspect ratio set while not visible is respected.
    widget.hide();
    widget.setAspectRatioMode(QVideoWidget::KeepAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
    QCOMPARE(object.testService->widgetControl->aspectRatioMode(), QVideoWidget::KeepAspectRatio);
}

#ifndef QT_NO_MULTIMEDIA
void tst_QVideoWidget::aspectRatioRendererControl()
{
    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget(&object);

    // Test the aspect ratio defaults to keeping the aspect ratio.
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);

    // Test the control has been informed of the aspect ratio change, post show.
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);

    // Test an aspect ratio change is enforced immediately while visible.
    widget.setAspectRatioMode(QVideoWidget::IgnoreAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::IgnoreAspectRatio);

    // Test an aspect ratio set while not visible is respected.
    widget.hide();
    widget.setAspectRatioMode(QVideoWidget::KeepAspectRatio);
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
    widget.show();
    QCOMPARE(widget.aspectRatioMode(), QVideoWidget::KeepAspectRatio);
}
#endif

void tst_QVideoWidget::sizeHint_data()
{
    QTest::addColumn<QSize>("size");
    
    QTest::newRow("720x576")
            << QSize(720, 576);
}

void tst_QVideoWidget::sizeHintWindowControl()
{
    QFETCH(QSize, size);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    QVideoWidget widget(&object);

    QVERIFY(widget.sizeHint().isEmpty());

    object.testService->windowControl->setNativeSize(size);

    QCOMPARE(widget.sizeHint(), size);
}

void tst_QVideoWidget::sizeHintWidgetControl()
{
    QFETCH(QSize, size);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    QVideoWidget widget(&object);

    QVERIFY(widget.sizeHint().isEmpty());

    object.testService->widgetControl->setSizeHint(size);

    QCOMPARE(widget.sizeHint(), size);
}

#ifndef QT_NO_MULTIMEDIA
void tst_QVideoWidget::sizeHintRendererControl_data()
{
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<QRect>("viewport");
    QTest::addColumn<QSize>("pixelAspectRatio");
    QTest::addColumn<QSize>("expectedSize");

    QTest::newRow("640x480")
            << QSize(640, 480)
            << QRect(0, 0, 640, 480)
            << QSize(1, 1)
            << QSize(640, 480);

    QTest::newRow("800x600, (80,60, 640x480) viewport")
            << QSize(800, 600)
            << QRect(80, 60, 640, 480)
            << QSize(1, 1)
            << QSize(640, 480);

    QTest::newRow("800x600, (80,60, 640x480) viewport, 4:3")
            << QSize(800, 600)
            << QRect(80, 60, 640, 480)
            << QSize(4, 3)
            << QSize(853, 480);

}

void tst_QVideoWidget::sizeHintRendererControl()
{
    QFETCH(QSize, frameSize);
    QFETCH(QRect, viewport);
    QFETCH(QSize, pixelAspectRatio);
    QFETCH(QSize, expectedSize);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);
    QVideoWidget widget(&object);

    widget.show();

    QVideoSurfaceFormat format(frameSize, QVideoFrame::Format_ARGB32);
    format.setViewport(viewport);
    format.setPixelAspectRatio(pixelAspectRatio);

    QVERIFY(object.testService->rendererControl->surface()->start(format));

    QCOMPARE(widget.sizeHint(), expectedSize);
}

#endif

void tst_QVideoWidget::fullScreenWindowControl()
{
    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    QVideoWidget widget(&object);
    widget.show();

    QSignalSpy spy(&widget, SIGNAL(fullScreenChanged(bool)));

    // Test showing full screen with setFullScreen(true).
    widget.setFullScreen(true);
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toBool(), true);

    // Test returning to normal with setFullScreen(false).
    widget.setFullScreen(false);
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toBool(), false);

    // Test showing full screen with showFullScreen().
    widget.showFullScreen();
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toBool(), true);

    // Test returning to normal with showNormal().
    widget.showNormal();
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toBool(), false);

    // Test setFullScreen(false) and showNormal() do nothing when isFullScreen() == false.
    widget.setFullScreen(false);
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    widget.showNormal();
    QCOMPARE(object.testService->windowControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);

    // Test setFullScreen(true) and showFullScreen() do nothing when isFullScreen() == true.
    widget.showFullScreen();
    widget.setFullScreen(true);
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
    widget.showFullScreen();
    QCOMPARE(object.testService->windowControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);

    // Test if the window control exits full screen mode, the widget follows suit.
    object.testService->windowControl->setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
    QCOMPARE(spy.value(5).value(0).toBool(), false);

    // Test if the window control enters full screen mode, the widget does nothing.
    object.testService->windowControl->setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
}

void tst_QVideoWidget::fullScreenWidgetControl()
{
    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    QVideoWidget widget(&object);
    widget.show();

    QSignalSpy spy(&widget, SIGNAL(fullScreenChanged(bool)));

    // Test showing full screen with setFullScreen(true).
    widget.setFullScreen(true);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toBool(), true);

    // Test returning to normal with setFullScreen(false).
    widget.setFullScreen(false);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toBool(), false);

    // Test showing full screen with showFullScreen().
    widget.showFullScreen();
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toBool(), true);

    // Test returning to normal with showNormal().
    widget.showNormal();
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toBool(), false);

    // Test setFullScreen(false) and showNormal() do nothing when isFullScreen() == false.
    widget.setFullScreen(false);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    widget.showNormal();
    QCOMPARE(object.testService->widgetControl->isFullScreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);

    // Test setFullScreen(true) and showFullScreen() do nothing when isFullScreen() == true.
    widget.showFullScreen();
    widget.setFullScreen(true);
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
    widget.showFullScreen();
    QCOMPARE(object.testService->widgetControl->isFullScreen(), true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);

    // Test if the window control exits full screen mode, the widget follows suit.
    object.testService->widgetControl->setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
    QCOMPARE(spy.value(5).value(0).toBool(), false);

    // Test if the window control enters full screen mode, the widget does nothing.
    object.testService->widgetControl->setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 6);
}

#ifndef QT_NO_MULTIMEDIA

void tst_QVideoWidget::fullScreenRendererControl()
{
    QtTestVideoObject object(0, 0, new QtTestRendererControl);
    QVideoWidget widget(&object);
    widget.show();

    QSignalSpy spy(&widget, SIGNAL(fullScreenChanged(bool)));

    // Test showing full screen with setFullScreen(true).
    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toBool(), true);

    // Test returning to normal with setFullScreen(false).
    widget.setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toBool(), false);

    // Test showing full screen with showFullScreen().
    widget.showFullScreen();
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toBool(), true);

    // Test returning to normal with showNormal().
    widget.showNormal();
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toBool(), false);

    // Test setFullScreen(false) and showNormal() do nothing when isFullScreen() == false.
    widget.setFullScreen(false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);
    widget.showNormal();
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 4);

    // Test setFullScreen(true) and showFullScreen() do nothing when isFullScreen() == true.
    widget.showFullScreen();
    widget.setFullScreen(true);
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
    widget.showFullScreen();
    QCOMPARE(widget.isFullScreen(), true);
    QCOMPARE(spy.count(), 5);
}

#endif

void tst_QVideoWidget::color_data()
{
    QTest::addColumn<int>("controlValue");
    QTest::addColumn<int>("value");
    QTest::addColumn<int>("expectedValue");

    QTest::newRow("12")
            << 0
            << 12
            << 12;
    QTest::newRow("-56")
            << 87
            << -56
            << -56;
    QTest::newRow("100")
            << 32
            << 100
            << 100;
    QTest::newRow("1294")
            << 0
            << 1294
            << 100;
    QTest::newRow("-102")
            << 34
            << -102
            << -100;
}

void tst_QVideoWidget::brightnessWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setBrightness(controlValue);

    QVideoWidget widget(&object);
    widget.show();

    // Test the video widget resets the controls starting brightness to the default.
    QCOMPARE(widget.brightness(), 0);

    QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

    // Test the video widget sets the brightness value, bounded if necessary and emits a changed
    // signal.
    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->windowControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    // Test the changed signal isn't emitted if the value is unchanged.
    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->windowControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);

    // Test the changed signal is emitted if the brightness is changed internally.
    object.testService->windowControl->setBrightness(controlValue);
    QCOMPARE(widget.brightness(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::brightnessWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setBrightness(controlValue);

    QVideoWidget widget(&object);
    QCOMPARE(widget.brightness(), 0);

    widget.show();

    QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->widgetControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(object.testService->widgetControl->brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setBrightness(controlValue);
    QCOMPARE(widget.brightness(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

#ifndef QT_NO_MULTIMEDIA

void tst_QVideoWidget::brightnessRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget(&object);
    widget.show();

    QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setBrightness(value);
    QCOMPARE(widget.brightness(), expectedValue);
    QCOMPARE(spy.count(), 1);
}

#endif

void tst_QVideoWidget::contrastWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setContrast(controlValue);

    QVideoWidget widget(&object);

    QCOMPARE(widget.contrast(), 0);
    widget.show();
    QCOMPARE(widget.contrast(), 0);

    QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->windowControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->windowControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->windowControl->setContrast(controlValue);
    QCOMPARE(widget.contrast(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::contrastWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setContrast(controlValue);

    QVideoWidget widget(&object);
    QCOMPARE(widget.contrast(), 0);
    widget.show();
    QCOMPARE(widget.contrast(), 0);

    QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->widgetControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(object.testService->widgetControl->contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setContrast(controlValue);
    QCOMPARE(widget.contrast(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

#ifndef QT_NO_MULTIMEDIA

void tst_QVideoWidget::contrastRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget(&object);
    widget.show();

    QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setContrast(value);
    QCOMPARE(widget.contrast(), expectedValue);
    QCOMPARE(spy.count(), 1);
}

#endif

void tst_QVideoWidget::hueWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setHue(controlValue);

    QVideoWidget widget(&object);
    QCOMPARE(widget.hue(), 0);
    widget.show();
    QCOMPARE(widget.hue(), 0);

    QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->windowControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->windowControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->windowControl->setHue(controlValue);
    QCOMPARE(widget.hue(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::hueWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setHue(controlValue);

    QVideoWidget widget(&object);
    QCOMPARE(widget.hue(), 0);
    widget.show();
    QCOMPARE(widget.hue(), 0);

    QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->widgetControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(object.testService->widgetControl->hue(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setHue(controlValue);
    QCOMPARE(widget.hue(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

#ifndef QT_NO_MULTIMEDIA

void tst_QVideoWidget::hueRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget(&object);
    widget.show();

    QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setHue(value);
    QCOMPARE(widget.hue(), expectedValue);
    QCOMPARE(spy.count(), 1);
}

#endif

void tst_QVideoWidget::saturationWindowControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(new QtTestWindowControl, 0, 0);
    object.testService->windowControl->setSaturation(controlValue);

    QVideoWidget widget(&object);
    QCOMPARE(widget.saturation(), 0);
    widget.show();
    QCOMPARE(widget.saturation(), 0);

    QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->windowControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->windowControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->windowControl->setSaturation(controlValue);
    QCOMPARE(widget.saturation(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);
}

void tst_QVideoWidget::saturationWidgetControl()
{
    QFETCH(int, controlValue);
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, new QtTestWidgetControl, 0);
    object.testService->widgetControl->setSaturation(controlValue);

    QVideoWidget widget(&object);

    QCOMPARE(widget.saturation(), 0);
    widget.show();
    QCOMPARE(widget.saturation(), 0);

    QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->widgetControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(object.testService->widgetControl->saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);

    object.testService->widgetControl->setSaturation(controlValue);
    QCOMPARE(widget.saturation(), controlValue);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), controlValue);

}

#ifndef QT_NO_MULTIMEDIA

void tst_QVideoWidget::saturationRendererControl()
{
    QFETCH(int, value);
    QFETCH(int, expectedValue);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget(&object);
    widget.show();
    QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), expectedValue);

    widget.setSaturation(value);
    QCOMPARE(widget.saturation(), expectedValue);
    QCOMPARE(spy.count(), 1);
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

static const uchar yuvPlanarImageData[] =
{
    0x00, 0x00, 0x0f, 0xff, 0xff, 0x0f, 0x00, 0x00,
    0x00, 0x0f, 0xff, 0x0f, 0x0f, 0xff, 0x0f, 0x00,
    0x00, 0xff, 0x0f, 0x00, 0x00, 0x0f, 0xff, 0x00,
    0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff,
    0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff,
    0x0f, 0xff, 0x0f, 0x00, 0x00, 0x0f, 0xff, 0x0f,
    0x00, 0x0f, 0xff, 0x0f, 0x0f, 0xff, 0x0f, 0x00,
    0x00, 0x00, 0x0f, 0xff, 0xff, 0x0f, 0x00, 0x00,
    0x00, 0x0f, 0x0f, 0x00,
    0x0f, 0x00, 0x00, 0x0f,
    0x0f, 0x00, 0x00, 0x0f,
    0x00, 0x0f, 0x0f, 0x00,
    0x00, 0x0f, 0x0f, 0x00,
    0x0f, 0x00, 0x00, 0x0f,
    0x0f, 0x00, 0x00, 0x0f,
    0x00, 0x0f, 0x0f, 0x00,
};

void tst_QVideoWidget::rendererSupportedFormat_data()
{
    QTest::addColumn<QAbstractVideoBuffer::HandleType>("handleType");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormat");
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<bool>("supportedPixelFormat");
    QTest::addColumn<bool>("supportedFormat");
    QTest::addColumn<bool>("glOnly");

    QTest::newRow("rgb32 640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB32
            << QSize(640, 480)
            << true
            << true
            << false;
    QTest::newRow("rgb32 -640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB32
            << QSize(-640, 480)
            << true
            << false
            << false;
    QTest::newRow("rgb24 1024x768")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(1024, 768)
            << true
            << true
            << false;
    QTest::newRow("rgb24 -1024x-768")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(-1024, -768)
            << true
            << false
            << false;    
    QTest::newRow("rgb565 0x0")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_RGB24
            << QSize(0, 0)
            << true
            << false
            << false;
    QTest::newRow("YUV420P 640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_YUV420P
            << QSize(640, 480)
            << true
            << true
            << true;
    QTest::newRow("YUV420P 640x-480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_YUV420P
            << QSize(640, -480)
            << true
            << false
            << true;
    QTest::newRow("Y8 640x480")
            << QAbstractVideoBuffer::NoHandle
            << QVideoFrame::Format_Y8
            << QSize(640, 480)
            << false
            << false
            << false;
    QTest::newRow("rgb32 (OpenGL) 640x480")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_RGB32
            << QSize(640, 480)
            << true
            << true
            << true;
    QTest::newRow("yuv420p (OpenGL) 640x480")
            << QAbstractVideoBuffer::GLTextureHandle
            << QVideoFrame::Format_YUV420P
            << QSize(640, 480)
            << false
            << false
            << false;
}

void tst_QVideoWidget::rendererSupportedFormat()
{
    QFETCH(QAbstractVideoBuffer::HandleType, handleType);
    QFETCH(QVideoFrame::PixelFormat, pixelFormat);
    QFETCH(QSize, frameSize);
    QFETCH(bool, supportedPixelFormat);
    QFETCH(bool, supportedFormat);
    QFETCH(bool, glOnly);

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QVideoWidget widget(&object);
    widget.show();

    QAbstractVideoSurface *surface = object.testService->rendererControl->surface();

    QVERIFY(surface);

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface->supportedPixelFormats(handleType);

    // Some formats are not supported without platform support for OpenGL shaders.
    if (glOnly && !pixelFormats.contains(pixelFormat))
        QSKIP("Pixel format unsupported on this platform", SkipSingle);

    QCOMPARE(pixelFormats.contains(pixelFormat), QBool(supportedPixelFormat));

    QVideoSurfaceFormat format(frameSize, pixelFormat, handleType);

    QCOMPARE(surface->isFormatSupported(format), supportedFormat);
    QCOMPARE(surface->start(format), supportedFormat);
}

void tst_QVideoWidget::rendererPresent_data()
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

    QTest::newRow("rgb32 -> yuv420p")
            << QVideoFrame::Format_RGB32
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb32ImageData)
            << int(sizeof(rgb32ImageData))
            << 8
            << QVideoFrame::Format_YUV420P
            << QSize(8, 8)
            << static_cast<const uchar *>(yuvPlanarImageData)
            << int(sizeof(yuvPlanarImageData))
            << 8;

    QTest::newRow("yv12 -> rgb24")
            << QVideoFrame::Format_YV12
            << QSize(8, 8)
            << static_cast<const uchar *>(yuvPlanarImageData)
            << int(sizeof(yuvPlanarImageData))
            << 8
            << QVideoFrame::Format_RGB24
            << QSize(2, 2)
            << static_cast<const uchar *>(rgb24ImageData)
            << int(sizeof(rgb24ImageData))
            << 8;

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

void tst_QVideoWidget::rendererPresent()
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

    QtTestVideoObject object(0, 0, new QtTestRendererControl);

    QtTestVideoWidget widget(&object);
    widget.show();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::RendererOutput);
    QVERIFY(object.testService->rendererControl->surface());

    QAbstractVideoSurface *surface = object.testService->rendererControl->surface();

    const QList<QVideoFrame::PixelFormat> pixelFormats = surface->supportedPixelFormats();

    if (!pixelFormats.contains(pixelFormatA) || !pixelFormats.contains(pixelFormatB))
        QSKIP("Pixel format unsupported on this platform", SkipSingle);

    QVideoSurfaceFormat formatA(frameSizeA, pixelFormatA);

    QVERIFY(surface->start(formatA));
    QVERIFY(surface->isStarted());

    QVideoFrame frameA(bytesA, frameSizeA, bytesPerLineA, pixelFormatA);

    frameA.map(QAbstractVideoBuffer::WriteOnly);
    memcpy(frameA.bits(), frameDataA, frameA.numBytes());
    frameA.unmap();

    QVERIFY(surface->present(frameA));

    QCoreApplication::processEvents();

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

    QCoreApplication::processEvents();

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

#endif

QTEST_MAIN(tst_QVideoWidget)

#include "tst_qvideowidget.moc"
