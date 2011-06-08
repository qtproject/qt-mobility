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

//TESTED_COMPONENT=src/feedback

#include <QtTest/QtTest>

#include <qfeedbackeffect.h>
#include <qfeedbackactuator.h>
#include <QSignalSpy>

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


class tst_QFeedbackMMK : public QObject
{
    Q_OBJECT
public:
    tst_QFeedbackMMK();
    ~tst_QFeedbackMMK();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void goodFile();
    void badFile();

private:
    QUrl url;
};

tst_QFeedbackMMK::tst_QFeedbackMMK()
{
}

tst_QFeedbackMMK::~tst_QFeedbackMMK()
{
}

void tst_QFeedbackMMK::initTestCase()
{
    // Some of this comes from the qsoundeffect testcase . .. ...
#ifdef QT_QFEEDBACKMMK_USEAPPLICATIONPATH
    url = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + QString("/test.wav"));
#else
    url = QUrl::fromLocalFile(QString(SRCDIR "test.wav"));
#endif
}

void tst_QFeedbackMMK::cleanupTestCase()
{
}

void tst_QFeedbackMMK::init()
{
#ifdef Q_OS_SYMBIAN
    //we need that head start for the eventloop to be responsive
    QTest::qWait(1000);
#endif
}

void tst_QFeedbackMMK::cleanup()
{
}

Q_DECLARE_METATYPE(QFeedbackEffect::ErrorType);

void tst_QFeedbackMMK::goodFile()
{
    QFeedbackFileEffect fe;
    qRegisterMetaType<QFeedbackEffect::ErrorType>();
    QSignalSpy errorSpy(&fe, SIGNAL(error(QFeedbackEffect::ErrorType)));
    QSignalSpy stateSpy(&fe, SIGNAL(stateChanged()));

    QFileInfo fi(url.toLocalFile());
    qDebug() << "URL for test data:" << url << url.toLocalFile() << fi.exists();

    fe.setSource(url);

    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Loading);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 1); // Stopped to Loading

    // Wait for it to be loaded
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Stopped);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 2); // Stopped to Loading to Stopped

    // Now play!
    fe.start();

    // Now wait for it to be playing
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Running);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 3); // Stopped to Loading to Stopped to Running
    QVERIFY(fe.isLoaded());
    QVERIFY(fe.duration() == 0); // unsupported :/

    // Try pausing - not supported
    fe.pause(); // XXX this emits stateChanged even when it fails
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Running);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 4); // Stopped to Loading to Stopped to Running

    // It should run out, eventually
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Stopped);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 5); // Stopped to Loading to Stopped to Running to Stopped

    // Play it again..
    fe.start();
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Running);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 6); // Stopped to Loading to Stopped to Running to Stopped to Running

    fe.stop();
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Stopped);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 7); // Stopped to Loading to Stopped to Running to Stopped to Running to Stopped

    fe.unload();
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Stopped);
    QCOMPARE(stateSpy.count(), 7); // no change
    QCOMPARE(fe.isLoaded(), false);
    QCOMPARE(fe.duration(), 0);

    // now load again
    fe.load();
    QTRY_COMPARE((int)fe.state(),  (int)QFeedbackFileEffect::Loading);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 8); // Stopped to Loading

    // Now wait for it to be loaded and playing
    QTRY_COMPARE((int)fe.state(), (int) QFeedbackFileEffect::Stopped);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 9); // Stopped to Loading to Stopped
    QVERIFY(fe.isLoaded());

    // destroy it while playing
}

void tst_QFeedbackMMK::badFile()
{
    QFeedbackFileEffect fe;
    qRegisterMetaType<QFeedbackEffect::ErrorType>();
    QSignalSpy errorSpy(&fe, SIGNAL(error(QFeedbackEffect::ErrorType)));
    QSignalSpy stateSpy(&fe, SIGNAL(stateChanged()));

    fe.setSource(QUrl("file:///does/not/exist/ever.wav"));

    // Depending on event loops we might miss the Loading state.
    QTRY_COMPARE(stateSpy.count(), 2);    // Loading & Stopped
    QTRY_COMPARE(fe.state(), QFeedbackEffect::Stopped);

    QCOMPARE(errorSpy.count(), 1);
    QVERIFY(fe.isLoaded() == false);

    fe.start(); // this actually causes a load, so it goes into LOADING, then fails, should go to STOPPED
    QTRY_COMPARE(stateSpy.count(), 4);    // Loading & Stopped
    QTRY_COMPARE(fe.state(), QFeedbackEffect::Stopped);
    QCOMPARE(errorSpy.count(), 2);
    QVERIFY(fe.isLoaded() == false);
}

QTEST_MAIN(tst_QFeedbackMMK)

#include "tst_qfeedbackmmk.moc"
