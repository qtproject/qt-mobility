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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
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

void tst_QFeedbackPlugin::testPlugin()
{
    // first get the actuators.  we want to use the test plugin actuator.
    QFeedbackActuator* testActuator;
    QList<QFeedbackActuator*> actuators = QFeedbackActuator::actuators();
    foreach (QFeedbackActuator* temp, actuators) {
        if (temp->name() == "test plugin") {
            testActuator = temp;
            break;
        }
    }

    // make sure we found the test actuator...
    QCOMPARE(testActuator->name(), QString("test plugin"));
    QCOMPARE(testActuator->id(), 7357); // test
    QVERIFY(testActuator->isCapabilitySupported(QFeedbackActuator::Period));
    testActuator->setEnabled(true);
    QVERIFY(!testActuator->isEnabled()); // the test plugin always returns enabled = false.
    testActuator->setEnabled(false);
    QVERIFY(!testActuator->isEnabled()); // the test plugin always returns enabled = false.
    testActuator->setEnabled(true);
    QVERIFY(!testActuator->isEnabled()); // the test plugin always returns enabled = false.
    QCOMPARE(testActuator->state(), QFeedbackActuator::Unknown); // and it always returns state = unknown.
    // XXX TODO: ensure that a "working" plugin returns real values..

#if 0
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
#endif
}

QTEST_MAIN(tst_QFeedbackPlugin)

#include "tst_qfeedbackplugin.moc"
