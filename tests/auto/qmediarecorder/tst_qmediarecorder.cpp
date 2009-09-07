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
#include <QDebug>
#include <qabstractmediaobject.h>
#include <qabstractmediacontrol.h>
#include <qabstractmediaservice.h>
#include <qmediarecordercontrol.h>
#include <qmediarecorder.h>

class MockProvider : public QMediaRecorderControl
{
    Q_OBJECT

public:
    MockProvider(QObject *parent):
        QMediaRecorderControl(parent),
    m_state(QMediaRecorder::StoppedState),
    m_position(0) {}

    QUrl sink() const
    {
        return m_sink;
    }

    bool setSink(const QUrl &sink)
    {
        m_sink = sink;
        return true;
    }

    QMediaRecorder::State state() const
    {
        return m_state;
    }

    qint64 duration() const
    {
        return m_position;
    }

public slots:
    void record()
    {
        m_state = QMediaRecorder::RecordingState;
        m_position=1;
        emit stateChanged(m_state);
        emit durationChanged(m_position);
    }

    void pause()
    {
        m_state = QMediaRecorder::PausedState;
        emit stateChanged(m_state);
    }

    void stop()
    {
        m_position=0;
        m_state = QMediaRecorder::StoppedState;
        emit stateChanged(m_state);
    }

public:
    QUrl       m_sink;
    QMediaRecorder::State m_state;
    qint64     m_position;
};

class MockService : public QAbstractMediaService
{
    Q_OBJECT
public:
    MockService(QObject *parent, QAbstractMediaControl *control):
        QAbstractMediaService(parent),
        mockControl(control) {}

    QAbstractMediaControl* control(const char *) const
    {
        return mockControl;
    }

    QAbstractMediaControl   *mockControl;
};

class MockObject : public QAbstractMediaObject
{
    Q_OBJECT
public:
    MockObject(QObject *parent, QAbstractMediaControl *control):
        QAbstractMediaObject(parent),
        mockService(new MockService(this, control))
    {
    }

    bool isValid() const
    {
        return true;
    }

    QAbstractMediaService* service() const
    {
        return mockService;
    }

    QAbstractMediaService *mockService;
};

class tst_QMediaRecorder: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testSink();
    void testRecord();

private:
    MockObject      *object;
    MockProvider    *mock;
    QMediaRecorder  *capture;
};

void tst_QMediaRecorder::init()
{
    mock = new MockProvider(this);
    object = new MockObject(this, mock);
    capture = new QMediaRecorder(object);
    QVERIFY(capture->isValid());
}

void tst_QMediaRecorder::cleanup()
{
    delete capture;
}

void tst_QMediaRecorder::testSink()
{
    capture->setSink(QUrl("test.tmp"));
    QUrl s = capture->sink();
    QCOMPARE(s.toString(), QString("test.tmp"));
}

void tst_QMediaRecorder::testRecord()
{
    QSignalSpy stateSignal(capture,SIGNAL(stateChanged(QMediaRecorder::State)));
    QSignalSpy progressSignal(capture, SIGNAL(durationChanged(qint64)));
    capture->record();

    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 1);
    QVERIFY(progressSignal.count() == 1);
    capture->pause();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 2);
    capture->stop();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(stateSignal.count() == 3);
}

QTEST_MAIN(tst_QMediaRecorder)

#include "tst_qmediarecorder.moc"
