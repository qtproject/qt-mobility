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
#include <QDebug>
#include <QTimer>

#include <qmediaobject.h>
#include <qmediacontrol.h>
#include <qmediaservice.h>
#include <qradiotunercontrol.h>
#include <qradiotuner.h>

QT_USE_NAMESPACE
class MockControl : public QRadioTunerControl
{
    Q_OBJECT

public:
    MockControl(QObject *parent):
        QRadioTunerControl(parent),
        m_active(false),
        m_searching(false),m_muted(false),m_stereo(true),
        m_volume(100),m_signal(0),m_frequency(0),
        m_band(QRadioTuner::FM) {}

    QRadioTuner::State state() const
    {
        return m_active ? QRadioTuner::ActiveState : QRadioTuner::StoppedState;
    }
    bool isAvailable() const
    {
        return true;
    }
    QtMultimedia::AvailabilityError availabilityError() const
    {
        return QtMultimedia::NoError;
    }

    QRadioTuner::Band band() const
    {
        return m_band;
    }

    void setBand(QRadioTuner::Band b)
    {
        m_band = b;
        emit bandChanged(m_band);
    }

    bool isBandSupported(QRadioTuner::Band b) const
    {
        if(b == QRadioTuner::FM || b == QRadioTuner::AM) return true;

        return false;
    }

    int frequency() const
    {
        return m_frequency;
    }

    QPair<int,int> frequencyRange(QRadioTuner::Band) const
    {
        return qMakePair<int,int>(1,2);
    }

    int frequencyStep(QRadioTuner::Band) const
    {
        return 1;
    }

    void setFrequency(int frequency)
    {
        m_frequency = frequency;
        emit frequencyChanged(m_frequency);
    }

    bool isStereo() const
    {
        return m_stereo;
    }

    void setStereo(bool stereo)
    {
        emit stereoStatusChanged(m_stereo = stereo);
    }


    QRadioTuner::StereoMode stereoMode() const
    {
        return m_stereoMode;
    }

    void setStereoMode(QRadioTuner::StereoMode mode)
    {
        m_stereoMode = mode;
    }

    QRadioTuner::Error error() const
    {
        return QRadioTuner::NoError;
    }

    QString errorString() const
    {
        return QString();
    }

    int signalStrength() const
    {
        return m_signal;
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
        emit mutedChanged(m_muted);
    }

    bool isSearching() const
    {
        return m_searching;
    }

    void searchForward()
    {
        m_searching = true;
        emit searchingChanged(m_searching);
    }

    void searchBackward()
    {
        m_searching = true;
        emit searchingChanged(m_searching);
    }

    void cancelSearch()
    {
        m_searching = false;
        emit searchingChanged(m_searching);
    }

    void start()
    {
        if (!m_active) {
            m_active = true;
            emit stateChanged(state());
        }
    }

    void stop()
    {
        if (m_active) {
            m_active = false;
            emit stateChanged(state());
        }
    }

public:
    bool m_active;
    bool m_searching;
    bool m_muted;
    bool m_stereo;
    int  m_volume;
    int  m_signal;
    int  m_frequency;
    QRadioTuner::StereoMode m_stereoMode;
    QRadioTuner::Band m_band;
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

class MockProvider : public QMediaServiceProvider
{
public:
    MockProvider(MockService *service):mockService(service) {}
    QMediaService *requestService(const QByteArray &, const QMediaServiceProviderHint &)
    {
        return mockService;
    }

    void releaseService(QMediaService *) {}

    MockService *mockService;
};

class tst_QRadioTuner: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testNullService();
    void testNullControl();
    void testBand();
    void testFrequency();
    void testMute();
    void testSearch();
    void testVolume();
    void testSignal();
    void testStereo();

private:
    MockControl     *mock;
    MockService     *service;
    MockProvider    *provider;
    QRadioTuner    *radio;
};

void tst_QRadioTuner::initTestCase()
{
    qRegisterMetaType<QRadioTuner::State>("QRadioTuner::State");
    qRegisterMetaType<QRadioTuner::Band>("QRadioTuner::Band");

    mock = new MockControl(this);
    service = new MockService(this, mock);
    provider = new MockProvider(service);
    radio = new QRadioTuner(0,provider);
    QVERIFY(radio->service() != 0);

    QSignalSpy stateSpy(radio, SIGNAL(stateChanged(QRadioTuner::State)));

    QCOMPARE(radio->state(), QRadioTuner::StoppedState);    
    radio->start();
    QCOMPARE(radio->state(), QRadioTuner::ActiveState);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.first()[0].value<QRadioTuner::State>(), QRadioTuner::ActiveState);
}

void tst_QRadioTuner::cleanupTestCase()
{
    QVERIFY(radio->error() == QRadioTuner::NoError);
    QVERIFY(radio->errorString().isEmpty());

    QSignalSpy stateSpy(radio, SIGNAL(stateChanged(QRadioTuner::State)));

    radio->stop();
    QCOMPARE(radio->state(), QRadioTuner::StoppedState);
    QCOMPARE(stateSpy.count(), 1);

    QCOMPARE(stateSpy.first()[0].value<QRadioTuner::State>(), QRadioTuner::StoppedState);

    delete radio;
    delete service;
    delete provider;
}

void tst_QRadioTuner::testNullService()
{
    const QPair<int, int> nullRange(0, 0);

    MockProvider provider(0);
    QRadioTuner radio(0, &provider);

    radio.start();
    QCOMPARE(radio.error(), QRadioTuner::ResourceError);
    QCOMPARE(radio.errorString(), QString());
    QCOMPARE(radio.band(), QRadioTuner::FM);
    QCOMPARE(radio.isBandSupported(QRadioTuner::AM), false);
    QCOMPARE(radio.isBandSupported(QRadioTuner::FM), false);
    QCOMPARE(radio.frequency(), 0);
    QCOMPARE(radio.frequencyStep(QRadioTuner::AM), 0);
    QCOMPARE(radio.frequencyStep(QRadioTuner::FM), 0);
    QCOMPARE(radio.frequencyRange(QRadioTuner::AM), nullRange);
    QCOMPARE(radio.frequencyRange(QRadioTuner::FM), nullRange);
    QCOMPARE(radio.isStereo(), false);
    QCOMPARE(radio.stereoMode(), QRadioTuner::Auto);
    QCOMPARE(radio.signalStrength(), 0);
    QCOMPARE(radio.volume(), 0);
    QCOMPARE(radio.isMuted(), false);
    QCOMPARE(radio.isSearching(), false);
    radio.stop();
}

void tst_QRadioTuner::testNullControl()
{
    const QPair<int, int> nullRange(0, 0);

    MockService service(0, 0);
    MockProvider provider(&service);
    QRadioTuner radio(0, &provider);

    radio.start();

    QCOMPARE(radio.error(), QRadioTuner::ResourceError);
    QCOMPARE(radio.errorString(), QString());

    QCOMPARE(radio.band(), QRadioTuner::FM);
    QCOMPARE(radio.isBandSupported(QRadioTuner::AM), false);
    QCOMPARE(radio.isBandSupported(QRadioTuner::FM), false);
    QCOMPARE(radio.frequency(), 0);
    QCOMPARE(radio.frequencyStep(QRadioTuner::AM), 0);
    QCOMPARE(radio.frequencyStep(QRadioTuner::FM), 0);
    QCOMPARE(radio.frequencyRange(QRadioTuner::AM), nullRange);
    QCOMPARE(radio.frequencyRange(QRadioTuner::FM), nullRange);
    {
        QSignalSpy bandSpy(&radio, SIGNAL(bandChanged(QRadioTuner::Band)));
        QSignalSpy frequencySpy(&radio, SIGNAL(frequencyChanged(int)));

        radio.setFrequency(107500);
        QCOMPARE(radio.band(), QRadioTuner::FM);
        QCOMPARE(radio.frequency(), 0);
        QCOMPARE(bandSpy.count(), 0);
        QCOMPARE(frequencySpy.count(), 0);

        radio.setBand(QRadioTuner::AM);
        QCOMPARE(radio.band(), QRadioTuner::FM);
        QCOMPARE(radio.frequency(), 0);
        QCOMPARE(bandSpy.count(), 0);
        QCOMPARE(frequencySpy.count(), 0);
    }
    QCOMPARE(radio.isStereo(), false);
    QCOMPARE(radio.stereoMode(), QRadioTuner::Auto);

    radio.setStereoMode(QRadioTuner::ForceStereo);
    QCOMPARE(radio.stereoMode(), QRadioTuner::Auto);

    QCOMPARE(radio.signalStrength(), 0);

    QCOMPARE(radio.volume(), 0);
    QCOMPARE(radio.isMuted(), false);
    {
        QSignalSpy volumeSpy(&radio, SIGNAL(volumeChanged(int)));
        QSignalSpy muteSpy(&radio, SIGNAL(mutedChanged(bool)));

        radio.setVolume(76);
        QCOMPARE(radio.volume(), 0);
        QCOMPARE(volumeSpy.count(), 0);

        radio.setMuted(true);
        QCOMPARE(radio.isMuted(), false);
        QCOMPARE(muteSpy.count(), 0);
    }
    QCOMPARE(radio.isSearching(), false);
    {
        QSignalSpy spy(&radio, SIGNAL(searchingChanged(bool)));

        radio.searchBackward();
        QCOMPARE(radio.isSearching(), false);
        QCOMPARE(spy.count(), 0);

        radio.searchForward();
        QCOMPARE(radio.isSearching(), false);
        QCOMPARE(spy.count(), 0);

        radio.cancelSearch();
        QCOMPARE(radio.isSearching(), false);
        QCOMPARE(spy.count(), 0);
    }

    radio.stop();
}

void tst_QRadioTuner::testBand()
{
    QVERIFY(radio->isBandSupported(QRadioTuner::FM));
    QVERIFY(!radio->isBandSupported(QRadioTuner::SW));

    if(radio->isBandSupported(QRadioTuner::AM)) {
        QSignalSpy readSignal(radio, SIGNAL(bandChanged(QRadioTuner::Band)));
        radio->setBand(QRadioTuner::AM);
        QTestEventLoop::instance().enterLoop(1);
        QVERIFY(radio->band() == QRadioTuner::AM);
        QVERIFY(readSignal.count() == 1);
    }
}

void tst_QRadioTuner::testFrequency()
{
    QSignalSpy readSignal(radio, SIGNAL(frequencyChanged(int)));
    radio->setFrequency(104500000);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->frequency() == 104500000);
    QVERIFY(readSignal.count() == 1);

    QVERIFY(radio->frequencyStep(QRadioTuner::FM) == 1);
    QPair<int,int> test = radio->frequencyRange(QRadioTuner::FM);
    QVERIFY(test.first == 1);
    QVERIFY(test.second == 2);
}

void tst_QRadioTuner::testMute()
{
    QSignalSpy readSignal(radio, SIGNAL(mutedChanged(bool)));
    radio->setMuted(true);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->isMuted());
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioTuner::testSearch()
{
    QSignalSpy readSignal(radio, SIGNAL(searchingChanged(bool)));
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

void tst_QRadioTuner::testVolume()
{
    QVERIFY(radio->volume() == 100);
    QSignalSpy readSignal(radio, SIGNAL(volumeChanged(int)));
    radio->setVolume(50);
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(radio->volume() == 50);
    QVERIFY(readSignal.count() == 1);
}

void tst_QRadioTuner::testSignal()
{
    QVERIFY(radio->signalStrength() == 0);
    // There is no set of this only a get, do nothing else.
}

void tst_QRadioTuner::testStereo()
{
    QVERIFY(radio->isStereo());
    radio->setStereoMode(QRadioTuner::ForceMono);
    QVERIFY(radio->stereoMode() == QRadioTuner::ForceMono);
}

QTEST_MAIN(tst_QRadioTuner)

#include "tst_qradiotuner.moc"
