/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/feedback

#include <QtTest/QtTest>

#include <qfeedbackeffect.h>
#include <qfeedbackactuator.h>

using namespace QTM_NAMESPACE;

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif


#define QTRY_WAIT(code, __expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)


class tst_QFeedbackHapticsEffect : public QObject
{
    Q_OBJECT
public:
    tst_QFeedbackHapticsEffect();
    ~tst_QFeedbackHapticsEffect();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void initialization();
    void envelope_data();
    void envelope();
    void startStop_data();
    void startStop();
    void themeSupport();

};

tst_QFeedbackHapticsEffect::tst_QFeedbackHapticsEffect()
{
}

tst_QFeedbackHapticsEffect::~tst_QFeedbackHapticsEffect()
{
}

void tst_QFeedbackHapticsEffect::initTestCase()
{
}

void tst_QFeedbackHapticsEffect::cleanupTestCase()
{
}

void tst_QFeedbackHapticsEffect::init()
{
#ifdef Q_OS_SYMBIAN
    //we need that head start for the eventloop to be responsive
    QTest::qWait(1000);
#endif
}

void tst_QFeedbackHapticsEffect::cleanup()
{
}


void tst_QFeedbackHapticsEffect::initialization()
{
    QFeedbackHapticsEffect effect;
    if (QFeedbackActuator::actuators().isEmpty()) {
        QVERIFY(!effect.actuator()->isValid());
    } else {
        QVERIFY(effect.actuator()->isValid());
        QCOMPARE(effect.actuator(), QFeedbackActuator::actuators().first());
    }

    // actuators from other plugins need to be verified.
    //test default values
    QCOMPARE(effect.duration(), 250); //250ms is the default length for effects
    QCOMPARE(effect.intensity(), qreal(1));
    QCOMPARE(effect.attackTime(), 0);
    QCOMPARE(effect.attackIntensity(), qreal(0));
    QCOMPARE(effect.fadeTime(), 0);
    QCOMPARE(effect.fadeIntensity(), qreal(0));
    QCOMPARE(effect.period(), -1);
    QCOMPARE(effect.state(), QFeedbackEffect::Stopped);

}

void tst_QFeedbackHapticsEffect::envelope_data()
{
    QTest::addColumn<int>("duration");
    QTest::addColumn<qreal>("intensity");
    QTest::addColumn<int>("attackTime");
    QTest::addColumn<qreal>("attackIntensity");
    QTest::addColumn<int>("fadeTime");
    QTest::addColumn<qreal>("fadeIntensity");
    QTest::addColumn<int>("period");

    QTest::newRow("TEST1")        <<  300 << qreal(.2) <<   1 << qreal(.2) <<   2 << qreal(1) <<  -1;
    QTest::newRow("TEST2")        <<  500 <<  qreal(1) << 100 << qreal(.5) <<   0 << qreal(0) <<  -1;
    QTest::newRow("TEST3")        <<  250 << qreal(.8) << 150 <<  qreal(1) << 150 << qreal(1) <<  -1;
    QTest::newRow("TEST period")  << 1000 <<  qreal(1) << 150 <<  qreal(1) << 150 << qreal(1) << 150;
}

void tst_QFeedbackHapticsEffect::envelope()
{
    QFETCH(int, duration);
    QFETCH(qreal, intensity);
    QFETCH(int, attackTime);
    QFETCH(qreal, attackIntensity);
    QFETCH(int, fadeTime);
    QFETCH(qreal, fadeIntensity);
    QFETCH(int, period);

    QFeedbackHapticsEffect effect;

    // actuators from other plugins need to be verified.
    effect.setDuration(duration);
    QCOMPARE(effect.duration(), duration);

    effect.setIntensity(intensity);
    QCOMPARE(effect.intensity(), intensity);

    effect.setAttackTime(attackTime);
    QCOMPARE(effect.attackTime(), attackTime);

    effect.setAttackIntensity(attackIntensity);
    QCOMPARE(effect.attackIntensity(), attackIntensity);

    effect.setFadeTime(fadeTime);
    QCOMPARE(effect.fadeTime(), fadeTime);

    effect.setFadeIntensity(fadeIntensity);
    QCOMPARE(effect.fadeIntensity(), fadeIntensity);

    effect.setPeriod(period);
    QCOMPARE(effect.period(), period);

    //test all the values at once
    QCOMPARE(effect.duration(), duration);
    QCOMPARE(effect.intensity(), intensity);
    QCOMPARE(effect.attackTime(), attackTime);
    QCOMPARE(effect.attackIntensity(), attackIntensity);
    QCOMPARE(effect.fadeTime(), fadeTime);
    QCOMPARE(effect.fadeIntensity(), fadeIntensity);
    QCOMPARE(effect.period(), period);
}


void tst_QFeedbackHapticsEffect::startStop_data()
{
    envelope_data();
}

void tst_QFeedbackHapticsEffect::startStop()
{
    qRegisterMetaType<QFeedbackEffect::ErrorType>("QFeedbackEffect::ErrorType");
    if (QFeedbackActuator::actuators().isEmpty())
        QSKIP("this test requires to have actuators", SkipAll);

    QFETCH(int, duration);
    QFETCH(qreal, intensity);
    QFETCH(int, attackTime);
    QFETCH(qreal, attackIntensity);
    QFETCH(int, fadeTime);
    QFETCH(qreal, fadeIntensity);
    QFETCH(int, period);

    QFeedbackHapticsEffect effect;
    QSignalSpy errorspy(&effect, SIGNAL(error(QFeedbackEffect::ErrorType)));
    QSignalSpy stateSpy(&effect, SIGNAL(stateChanged()));

    effect.setDuration(duration);
    effect.setIntensity(intensity);
    effect.setAttackTime(attackTime);
    effect.setAttackIntensity(attackIntensity);
    effect.setFadeTime(fadeTime);
    effect.setFadeIntensity(fadeIntensity);
    effect.setPeriod(period);

    QCOMPARE(effect.state(), QFeedbackHapticsEffect::Stopped);

    QVERIFY(stateSpy.isEmpty());
    effect.start();
    QVERIFY(errorspy.isEmpty());

    // This seems a little risky
    QTRY_COMPARE(effect.state(), QFeedbackHapticsEffect::Running);
    QVERIFY(errorspy.isEmpty());
    QCOMPARE(stateSpy.count(), 1); // stopped to start

    effect.pause();
    QCOMPARE(effect.state(), QFeedbackHapticsEffect::Paused);
    QCOMPARE(stateSpy.count(), 2);

    effect.start();
    QCOMPARE(effect.state(), QFeedbackHapticsEffect::Running);
    QCOMPARE(stateSpy.count(),  3);

    QTRY_COMPARE(effect.state(), QFeedbackHapticsEffect::Stopped);
    QVERIFY(errorspy.isEmpty());
    QEXPECT_FAIL("", "No plugin API to report state changed to stopped :/", Continue);
    QCOMPARE(stateSpy.count(), 4);
}


void tst_QFeedbackHapticsEffect::themeSupport()
{
    const bool supportsTheme = QFeedbackEffect::supportsThemeEffect();

    if (!supportsTheme)
        QSKIP("No theme support", SkipAll);
    int numberOfSupportedThemeEffects = 0;
    for (int i = 0; i < QFeedbackEffect::NumberOfThemeEffects; ++i) {
        if (QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeEffect(i)))
            numberOfSupportedThemeEffects++;
        QTest::qWait(250); //let's make sure the device is ready again
    }

    QVERIFY(numberOfSupportedThemeEffects > 0);
}



QTEST_MAIN(tst_QFeedbackHapticsEffect)

#include "tst_qfeedbackhapticseffect.moc"
