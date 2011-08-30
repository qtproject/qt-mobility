/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <qfeedbackeffect.h>
#include <qfeedbackactuator.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

QTM_USE_NAMESPACE

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


Q_DECLARE_METATYPE(QFeedbackEffect::ThemeEffect);

class tst_qdeclarativefeedback : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativefeedback();

private slots:
    void hapticsEffect();
    void fileEffect();
    void actuator();
    void themeEffect();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativefeedback::tst_qdeclarativefeedback()
{
}

void tst_qdeclarativefeedback::hapticsEffect()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/hapticseffect.qml"));

    QObject *hapticsEffect = component.create();
    QVERIFY(hapticsEffect != 0);

    QCOMPARE(hapticsEffect->property("attackIntensity").toReal(), 0.0);
    QCOMPARE(hapticsEffect->property("attackTime").toInt(), 250);
    QCOMPARE(hapticsEffect->property("intensity").toReal(), 1.0);
    QCOMPARE(hapticsEffect->property("duration").toInt(), 100);
    QCOMPARE(hapticsEffect->property("fadeTime").toInt(), 250);
    QCOMPARE(hapticsEffect->property("fadeIntensity").toReal(), 0.0);
    QVERIFY(!hapticsEffect->property("actuator").isNull());
    QCOMPARE(hapticsEffect->property("state").toInt(), (int)(QFeedbackEffect::Stopped));

    if (!hapticsEffect->property("availableActuators").toList().isEmpty()) {
      QCOMPARE(hapticsEffect->property("running").toBool(), false);
      QCOMPARE(hapticsEffect->property("paused").toBool(), false);
      hapticsEffect->setProperty("running", true);
      QCOMPARE(hapticsEffect->property("running").toBool(), true);
      QCOMPARE(hapticsEffect->property("paused").toBool(), false);
      hapticsEffect->setProperty("paused", true);

      // XXX make sure we just test dummy backend
      QCOMPARE(hapticsEffect->property("running").toBool(), false);
      QCOMPARE(hapticsEffect->property("paused").toBool(), true);
    }
    delete hapticsEffect;
}

void tst_qdeclarativefeedback::fileEffect()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/fileeffect.qml"));

    QObject *fileEffect = component.create();
    QVERIFY(fileEffect != 0);

    QCOMPARE(fileEffect->property("source").toUrl(), QUrl("qrc:nonexistingfile.haptic"));
    QCOMPARE(fileEffect->property("loaded").toBool(), false);
    QTRY_COMPARE(fileEffect->property("state").toInt(), (int)(QFeedbackEffect::Stopped));

    QCOMPARE(fileEffect->property("running").toBool(), false);
    QCOMPARE(fileEffect->property("paused").toBool(), false);
    fileEffect->setProperty("running", true);
    fileEffect->setProperty("paused", true);

    // dummy backend
    QTRY_COMPARE(fileEffect->property("running").toBool(), false);
    QTRY_COMPARE(fileEffect->property("paused").toBool(), false);

    delete fileEffect;
}

void tst_qdeclarativefeedback::actuator()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/actuator.qml"));

    QObject *actuator = component.create();
    QVERIFY(actuator != 0);
    QCOMPARE(actuator->property("enabled").toBool(), false);

    delete actuator;
}

void tst_qdeclarativefeedback::themeEffect()
{
    QDeclarativeComponent component(&engine);
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/themeeffect.qml"));

    // We don't export this class, but we're just poking at properties anyway
    QObject *dte = component.create();
    QCOMPARE(component.errorString(), QString());
    QVERIFY(dte != 0);

    // Test the effect property gets assigned
    QMetaProperty p = dte->metaObject()->property(dte->metaObject()->indexOfProperty("effect"));
    QCOMPARE(p.read(dte).value<int>(), (int)QFeedbackEffect::ThemeBasicButton);

    p = dte->metaObject()->property(dte->metaObject()->indexOfProperty("supported"));
    QCOMPARE(p.read(dte).value<bool>(), QFeedbackEffect::supportsThemeEffect());

    delete dte;

    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/themeeffect2.qml"));

    dte = component.create();
    QCOMPARE(component.errorString(), QString());

    QVERIFY(dte != 0);
    // Effect property as a string
    p = dte->metaObject()->property(dte->metaObject()->indexOfProperty("effect"));
    QCOMPARE(p.read(dte).value<int>(), (int)QFeedbackEffect::ThemeRotateStep);

    delete dte;

    // Now test the default method
    component.loadUrl(QUrl::fromLocalFile(SRCDIR "/data/themeeffect3.qml"));

    dte = component.create();
    QCOMPARE(component.errorString(), QString());
    QVERIFY(dte != 0);

    // Well, we can only verify that through a dummy plugin (TODO). verified through qdebug atm :)

    delete dte;
}

QTEST_MAIN(tst_qdeclarativefeedback)

#include "tst_qdeclarativefeedback.moc"
