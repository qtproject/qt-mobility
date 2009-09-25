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
#include <QTimer>

#include <multimedia/qmediaobject.h>
#include <multimedia/qmediacontrol.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qradioplayercontrol.h>
#include <multimedia/qradioplayer.h>

class MockControl : public QRadioPlayerControl
{
    Q_OBJECT

public:
    MockControl(QObject *parent):
        QRadioPlayerControl(parent),
        m_searching(false),m_muted(false),m_stereo(true),
        m_volume(100),m_signal(0),m_frequency(0),
        m_duration(0),m_band(QRadioPlayer::FM) {}

    QRadioPlayer::Band band() const
    {
        return m_band;
    }

    void setBand(QRadioPlayer::Band b)
    {
        m_band = b;
        emit bandChanged(m_band);
    }

    bool isSupportedBand(QRadioPlayer::Band b) const
    {
        if(b == QRadioPlayer::FM || b == QRadioPlayer::AM) return true;

        return false;
    }

    int frequency() const
    {
        return m_frequency;
    }

    void setFrequency(int frequency)
    {
        m_frequency = frequency;
        emit frequencyChanged(m_frequency);
        //NOTE: for testing emit duration change on freq set
        // this would normally be done on a time interval from the backend.
        m_duration+=1000; // add 1 sec
        emit durationChanged(m_duration);
    }

    bool isStereo() const
    {
        return m_stereo;
    }

    void setStereo(bool stereo)
    {
        m_stereo = stereo;
        emit stereoStatusChanged(m_stereo);
    }

    int signalStrength() const
    {
        return m_signal;
    }

    qint64 duration() const
    {
        return m_duration;
    }

    int volume() const
    {
        return m_volume;
    }

    void setVolume(int volume)
    {
        m_volume = volume;
        emit volumeChanged(m_volume);
    }

    bool isMuted() const
    {
        return m_muted;
    }

    void setMuted(bool muted)
    {
        m_muted = muted;
        emit mutingChanged(m_muted);
    }

    bool isSearching() const
    {
        return m_searching;
    }

    void searchForward()
    {
        m_searching = true;
        emit searchingStatusChanged(m_searching);
    }

    void searchBackward()
    {
        m_searching = true;
        emit searchingStatusChanged(m_searching);
    }

    void cancelSearch()
    {
        m_searching = false;
        emit searchingStatusChanged(m_searching);
    }

public:
    bool m_searching;
    bool m_muted;
    bool m_stereo;
    int  m_volume;
    int  m_signal;
    int  m_frequency;
    qint64 m_duration;
    QRadioPlayer::Band m_band;
};

class MockService : public QMediaService
{
    Q_OBJECT
public:
    MockService(QObject *parent, QMediaControl *control):
        QMediaService(parent),
        mockControl(control) {}

    QMediaControl* control(const char *) const
    {
        return mockControl;
    }

    QMediaControl   *mockControl;
};

class MockObject : public QMediaObject
{
    Q_OBJECT
public:
    MockObject(QObject *parent, QMediaControl *control):
        QMediaObject(parent, new MockService(this, control))
    {
        mockService = service();
    }

    bool isValid() const
    {
        return true;
    }

    QMediaService *mockService;
};

class MockProvider : public QMediaServiceProvider
{
public:
    MockProvider(MockService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray &, const QList<QByteArray> &)
    {
        return mockService;
    }

    void releaseService(QMediaService *) {}

    MockService *mockService;
};

class tst_QRadioPlayer: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testBand();
    void testFrequency();
    void testStereo();
    void testMute();
    void testSearch();
    void testVolume();
    void testSignal();
    void testDuration();

private:
    MockObject      *object;
    MockControl     *mock;
    MockProvider    *provider;
    QRadioPlayer    *radio;
};

void tst_QRadioPlayer::init()
{
    mock = new MockControl(this);
    object = new MockObject(this, mock);
    provider = new MockProvider(static_cast<MockService*>(object->service()));
    radio = new QRadioPlayer(0,provider);
    QVERIFY(radio->service() != 0);
    QVERIFY(radio->isValid());
}

void tst_QRadioPlayer::cleanup()
{
    delete radio;
    delete provider;
}

void tst_QRadioPlayer::testBand()
{
    QVERIFY(radio->isSupportedBand(QRadioPlayer::FM));
    QVERIFY(!radio->isSupportedBand(QRadioPlayer::SW));

    if(radio->isSupportedBand(QRadioPlayer::AM)) {
        QSignalSpy readSignal(radio, SIGNAL(bandChanged(QRadioPlayer::Band)));
        radio->setBand(QRadioPlayer::AM);
        QTestEventLoop::instance().enterLoop(1);
        QVERIFY(radio->band() == QRadioPlayer::AM);
        QVERIFY(readSignal.count() == 1);
    }
}

void tst_QRadioPlayer::testFrequency()
{
    QSignalSpy readSignal(radio, SIGNAL(frequencyChanged(int)));
    radio->setFrequency(104500000);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->frequency() == 104500000);
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioPlayer::testStereo()
{
    QSignalSpy readSignal(radio, SIGNAL(stereoStatusChanged(bool)));
    radio->setStereo(false);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(!radio->isStereo());
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioPlayer::testMute()
{
    QSignalSpy readSignal(radio, SIGNAL(mutingChanged(bool)));
    radio->setMuted(true);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->isMuted());
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioPlayer::testSearch()
{
    QSignalSpy readSignal(radio, SIGNAL(searchingStatusChanged(bool)));
    QVERIFY(!radio->isSearching());

    radio->searchForward();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->isSearching());
    QVERIFY(readSignal.count() == 1);

    radio->cancelSearch();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(!radio->isSearching());
    QVERIFY(readSignal.count() == 2);

    radio->searchBackward();
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->isSearching());
    QVERIFY(readSignal.count() == 3);

    radio->cancelSearch();
    QVERIFY(!radio->isSearching());
}

void tst_QRadioPlayer::testVolume()
{
    QVERIFY(radio->volume() == 100);
    QSignalSpy readSignal(radio, SIGNAL(volumeChanged(int)));
    radio->setVolume(50);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->volume() == 50);
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioPlayer::testSignal()
{
    QVERIFY(radio->signalStrength() == 0);
    // There is no set of this only a get, do nothing else.
}

void tst_QRadioPlayer::testDuration()
{
    QVERIFY(radio->duration() == 0);
    QSignalSpy readSignal(radio, SIGNAL(durationChanged(qint64)));
    radio->setFrequency(104500000);
    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(readSignal.count() > 0);
    QVERIFY(radio->duration() > 0);
}


QTEST_MAIN(tst_QRadioPlayer)

#include "tst_qradioplayer.moc"
