/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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

#include "multimedia/qvideowidget.h"

#include "multimedia/qabstractmediaobject.h"
#include "multimedia/qabstractmediaservice.h"
#include "multimedia/qvideooutputcontrol.h"
#include "multimedia/qvideowindowcontrol.h"

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
    QSize nativeSize() const { return m_nativeSize; }
    void setNativeSize(const QSize &size) { m_nativeSize = size; emit nativeSizeChanged(); }
private:
    QSize m_nativeSize;
};

class QtTestVideoService : public QAbstractMediaService
{
public:
    QtTestVideoService(QtTestOutputControl *output, QtTestWindowControl *window)
        : QAbstractMediaService(0)
        , outputControl(output)
        , windowControl(window)
    {
    }

    ~QtTestVideoService()
    {
        delete outputControl;
        delete windowControl;
    }

    QAbstractMediaControl *control(const char *name) const
    {
        if (qstrcmp(name, QVideoOutputControl_iid) == 0)
            return outputControl;
        else if (qstrcmp(name, QVideoWindowControl_iid) == 0)
            return windowControl;
        else
            return 0;
    }

    QtTestOutputControl *outputControl;
    QtTestWindowControl *windowControl;
};

class QtTestVideoObject : public QAbstractMediaObject
{
public:
    QtTestVideoObject()
        : testService(new QtTestVideoService(new QtTestOutputControl, new QtTestWindowControl))
    {
    }

    QtTestVideoObject(QtTestVideoService *service)
        : testService(service)
    {
    }

    ~QtTestVideoObject()
    {
        delete testService;
    }

    bool isValid() const { return true; }

    QAbstractMediaService *service() const { return testService; }

    QtTestVideoService *testService;
};

class tst_QVideoWidget : public QObject
{
    Q_OBJECT
public slots:
    void init();

private slots:
    void nullObject();
    void nullService();
    void nullOutputControl();
    void nullWindowControl();
    void show();
    void sizeHint();
    void displayMode();
    void brightness();
    void contrast();
    void hue();
    void saturation();
};

Q_DECLARE_METATYPE(QVideoWidget::DisplayMode);

void tst_QVideoWidget::init()
{
    qRegisterMetaType<QVideoWidget::DisplayMode>();
}

void tst_QVideoWidget::nullObject()
{
    QVideoWidget widget(0);

    QCOMPARE(widget.sizeHint(), QSize());

    widget.setDisplayMode(QVideoWidget::FullscreenDisplay);
    QCOMPARE(widget.displayMode(), QVideoWidget::WindowedDisplay);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 0);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 0);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 0);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 0);
}

void tst_QVideoWidget::nullService()
{
    QtTestVideoObject object(0);

    QVideoWidget widget(&object);

    QCOMPARE(widget.sizeHint(), QSize());

    widget.setDisplayMode(QVideoWidget::FullscreenDisplay);
    QCOMPARE(widget.displayMode(), QVideoWidget::WindowedDisplay);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 0);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 0);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 0);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 0);
}

void tst_QVideoWidget::nullOutputControl()
{
    QtTestVideoObject object(new QtTestVideoService(0, 0));

    QVideoWidget widget(&object);

    QCOMPARE(widget.sizeHint(), QSize());

    widget.setDisplayMode(QVideoWidget::FullscreenDisplay);
    QCOMPARE(widget.displayMode(), QVideoWidget::WindowedDisplay);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 0);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 0);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 0);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 0);
}

void tst_QVideoWidget::nullWindowControl()
{
    QtTestVideoObject object(new QtTestVideoService(new QtTestOutputControl, 0));

    QVideoWidget widget(&object);

    QCOMPARE(widget.sizeHint(), QSize());

    widget.setDisplayMode(QVideoWidget::FullscreenDisplay);
    QCOMPARE(widget.displayMode(), QVideoWidget::WindowedDisplay);

    widget.setBrightness(100);
    QCOMPARE(widget.brightness(), 0);

    widget.setContrast(100);
    QCOMPARE(widget.contrast(), 0);

    widget.setHue(100);
    QCOMPARE(widget.hue(), 0);

    widget.setSaturation(100);
    QCOMPARE(widget.saturation(), 0);
}

void tst_QVideoWidget::show()
{
    QtTestVideoObject object;
    QVideoWidget widget(&object);

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);
    QVERIFY(object.testService->windowControl->winId() == 0);

    widget.show();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::WindowOutput);
    QVERIFY(object.testService->windowControl->winId() != 0);

    widget.hide();

    QCOMPARE(object.testService->outputControl->output(), QVideoOutputControl::NoOutput);
}

void tst_QVideoWidget::sizeHint()
{
    const QSize size(720, 576);

    QtTestVideoObject object;
    QVideoWidget widget(&object);

    QCOMPARE(widget.sizeHint(), QSize());

    object.testService->windowControl->setNativeSize(size);

    QCOMPARE(widget.sizeHint(), size);
}

void tst_QVideoWidget::displayMode()
{
    QtTestVideoObject object;
    QVideoWidget widget(&object);

    QSignalSpy spy(&widget, SIGNAL(displayModeChanged(QVideoWidget::DisplayMode)));

    widget.setDisplayMode(QVideoWidget::FullscreenDisplay);

    QCOMPARE(object.testService->windowControl->isFullscreen(), true);
    QCOMPARE(widget.displayMode(), QVideoWidget::FullscreenDisplay);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(qvariant_cast<QVideoWidget::DisplayMode>(spy.value(0).value(0)),
             QVideoWidget::FullscreenDisplay);

    widget.setDisplayMode(QVideoWidget::WindowedDisplay);

    QCOMPARE(object.testService->windowControl->isFullscreen(), false);
    QCOMPARE(widget.isFullScreen(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(qvariant_cast<QVideoWidget::DisplayMode>(spy.value(1).value(0)),
             QVideoWidget::WindowedDisplay);
}

void tst_QVideoWidget::brightness()
{
    QtTestVideoObject object;
    QVideoWidget widget(&object);

    QSignalSpy spy(&widget, SIGNAL(brightnessChanged(int)));

    widget.setBrightness(12);
    QCOMPARE(widget.brightness(), 12);
    QCOMPARE(object.testService->windowControl->brightness(), 12);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), 12);

    widget.setBrightness(-56);
    QCOMPARE(widget.brightness(), -56);
    QCOMPARE(object.testService->windowControl->brightness(), -56);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), -56);

    widget.setBrightness(1249);
    QCOMPARE(widget.brightness(), 100);
    QCOMPARE(object.testService->windowControl->brightness(), 100);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toInt(), 100);

    widget.setBrightness(-102);
    QCOMPARE(widget.brightness(), -100);
    QCOMPARE(object.testService->windowControl->brightness(), -100);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toInt(), -100);
}

void tst_QVideoWidget::contrast()
{
    QtTestVideoObject object;
    QVideoWidget widget(&object);

    QSignalSpy spy(&widget, SIGNAL(contrastChanged(int)));

    widget.setContrast(12);
    QCOMPARE(widget.contrast(), 12);
    QCOMPARE(object.testService->windowControl->contrast(), 12);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), 12);

    widget.setContrast(-56);
    QCOMPARE(widget.contrast(), -56);
    QCOMPARE(object.testService->windowControl->contrast(), -56);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), -56);

    widget.setContrast(1249);
    QCOMPARE(widget.contrast(), 100);
    QCOMPARE(object.testService->windowControl->contrast(), 100);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toInt(), 100);

    widget.setContrast(-102);
    QCOMPARE(widget.contrast(), -100);
    QCOMPARE(object.testService->windowControl->contrast(), -100);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toInt(), -100);
}

void tst_QVideoWidget::hue()
{
    QtTestVideoObject object;
    QVideoWidget widget(&object);

    QSignalSpy spy(&widget, SIGNAL(hueChanged(int)));

    widget.setHue(12);
    QCOMPARE(widget.hue(), 12);
    QCOMPARE(object.testService->windowControl->hue(), 12);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), 12);

    widget.setHue(-56);
    QCOMPARE(widget.hue(), -56);
    QCOMPARE(object.testService->windowControl->hue(), -56);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), -56);

    widget.setHue(1249);
    QCOMPARE(widget.hue(), 100);
    QCOMPARE(object.testService->windowControl->hue(), 100);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toInt(), 100);

    widget.setHue(-102);
    QCOMPARE(widget.hue(), -100);
    QCOMPARE(object.testService->windowControl->hue(), -100);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toInt(), -100);
}

void tst_QVideoWidget::saturation()
{
    QtTestVideoObject object;
    QVideoWidget widget(&object);

    QSignalSpy spy(&widget, SIGNAL(saturationChanged(int)));

    widget.setSaturation(12);
    QCOMPARE(widget.saturation(), 12);
    QCOMPARE(object.testService->windowControl->saturation(), 12);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.value(0).value(0).toInt(), 12);

    widget.setSaturation(-56);
    QCOMPARE(widget.saturation(), -56);
    QCOMPARE(object.testService->windowControl->saturation(), -56);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.value(1).value(0).toInt(), -56);

    widget.setSaturation(1249);
    QCOMPARE(widget.saturation(), 100);
    QCOMPARE(object.testService->windowControl->saturation(), 100);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.value(2).value(0).toInt(), 100);

    widget.setSaturation(-102);
    QCOMPARE(widget.saturation(), -100);
    QCOMPARE(object.testService->windowControl->saturation(), -100);
    QCOMPARE(spy.count(), 4);
    QCOMPARE(spy.value(3).value(0).toInt(), -100);
}

QTEST_MAIN(tst_QVideoWidget)

#include "tst_qvideowidget.moc"
