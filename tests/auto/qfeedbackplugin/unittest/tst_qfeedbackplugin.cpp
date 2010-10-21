/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

class tst_QFeedbackPlugin : public QObject
{
    Q_OBJECT
public:
    tst_QFeedbackPlugin();
    ~tst_QFeedbackPlugin();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testPlugin();
    void testFileEffect();
    void testThemeEffect();

private:
    QFeedbackHapticsEffect m_testEffect;
};

tst_QFeedbackPlugin::tst_QFeedbackPlugin()
{
}

tst_QFeedbackPlugin::~tst_QFeedbackPlugin()
{
}

void tst_QFeedbackPlugin::initTestCase()
{
}

void tst_QFeedbackPlugin::cleanupTestCase()
{
}

void tst_QFeedbackPlugin::init()
{
}

void tst_QFeedbackPlugin::cleanup()
{
}

void tst_QFeedbackPlugin::testThemeEffect()
{
    QVERIFY(QFeedbackEffect::supportsThemeEffect());
    QVERIFY(QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasic));
    QVERIFY(!QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasicButton));
}

void tst_QFeedbackPlugin::testFileEffect()
{
    QFeedbackFileEffect fileEffect;
    QVERIFY(QFeedbackFileEffect::supportedMimeTypes().contains("x-test/this is a test"));

    QVERIFY(fileEffect.state() == QFeedbackEffect::Stopped);

    fileEffect.setSource(QUrl("load")); // this should call load
    QVERIFY(fileEffect.state() == QFeedbackEffect::Loading);

    fileEffect.setSource(QUrl("ignored")); // not stopped, should fail
    QVERIFY(fileEffect.source() == QUrl("load"));

    QVERIFY(fileEffect.isLoaded());
    fileEffect.setLoaded(true); // should do nothing
    QVERIFY(fileEffect.isLoaded());
    QCOMPARE(fileEffect.duration(), 5678); // from the plugin

    fileEffect.unload(); // should fail, since we're not STOPPED (HMM!!)
    QVERIFY(fileEffect.isLoaded());

    fileEffect.stop();
    QVERIFY(fileEffect.state() == QFeedbackEffect::Stopped);
    // Now we should be able to change things again

    QVERIFY(fileEffect.isLoaded());
    fileEffect.unload();
    QVERIFY(!fileEffect.isLoaded());
    QVERIFY(fileEffect.state() == QFeedbackEffect::Stopped);
    QCOMPARE(fileEffect.duration(), 0); // unloaded, shouldn't call?

    // Change the url
    fileEffect.setSource(QUrl("failload"));
    QVERIFY(!fileEffect.isLoaded());
    QVERIFY(fileEffect.state() == QFeedbackEffect::Stopped);
    QCOMPARE(fileEffect.duration(), 0); // unknown

    fileEffect.setSource(QUrl("load"));
    QVERIFY(fileEffect.isLoaded());
    QVERIFY(fileEffect.state() == QFeedbackEffect::Loading);
    fileEffect.start();
    QVERIFY(fileEffect.state() == QFeedbackEffect::Running);
    fileEffect.start();
    QVERIFY(fileEffect.state() == QFeedbackEffect::Running);
    fileEffect.stop();
    QVERIFY(fileEffect.state() == QFeedbackEffect::Stopped);
    fileEffect.pause();
    QVERIFY(fileEffect.state() == QFeedbackEffect::Paused); // XXX this is a strange transition
}

void tst_QFeedbackPlugin::testPlugin()
{
    // first get the actuators.  we want to use the test plugin actuator.
    QFeedbackActuator* testActuator;
    QList<QFeedbackActuator*> actuators = QFeedbackActuator::actuators();
    QCOMPARE(actuators.count(), 2);

    QCOMPARE(actuators.at(0)->name(), QString("test plugin"));
    QCOMPARE(actuators.at(0)->id(), 0);
    QCOMPARE(actuators.at(1)->name(), QString("5555"));
    QCOMPARE(actuators.at(1)->id(), 1);

    // make sure we found the test actuator...
    testActuator = actuators.at(0);

    QCOMPARE(testActuator->name(), QString("test plugin"));
    QCOMPARE(testActuator->id(), 0); // test
    QVERIFY(testActuator->isCapabilitySupported(QFeedbackActuator::Period));
    testActuator->setEnabled(true);
    QVERIFY(!testActuator->isEnabled()); // the test plugin always returns enabled = false.
    testActuator->setEnabled(false);
    QVERIFY(!testActuator->isEnabled()); // the test plugin always returns enabled = false.
    testActuator->setEnabled(true);
    QVERIFY(!testActuator->isEnabled()); // the test plugin always returns enabled = false.
    QCOMPARE(testActuator->state(), QFeedbackActuator::Unknown); // and it always returns state = unknown.
    // XXX TODO: ensure that a "working" plugin returns real values..

    // then, ensure that the test effect uses this actuator.
    m_testEffect.setActuator(testActuator);

    // it will do nothing, so stick some values in and play it.
    m_testEffect.setAttackIntensity(0.0);
    m_testEffect.setAttackTime(250);
    m_testEffect.setIntensity(1.0);
    m_testEffect.setDuration(100);
    m_testEffect.setFadeTime(250);
    m_testEffect.setFadeIntensity(0.0);
    m_testEffect.start();
    QVERIFY(m_testEffect.state() == QFeedbackHapticsEffect::Running);
    m_testEffect.pause();
    QVERIFY(m_testEffect.state() == QFeedbackHapticsEffect::Paused);
    m_testEffect.start();
    QVERIFY(m_testEffect.state() == QFeedbackHapticsEffect::Running);
    m_testEffect.stop();
    QVERIFY(m_testEffect.state() == QFeedbackHapticsEffect::Stopped);
}

QTEST_MAIN(tst_QFeedbackPlugin)

#include "tst_qfeedbackplugin.moc"
