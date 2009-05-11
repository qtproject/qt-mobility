/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include <QtTest/QtTest>
#include "../qbearertestcommon.h"
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

Q_DECLARE_METATYPE(QNetworkConfiguration)
Q_DECLARE_METATYPE(QNetworkSession::State);
Q_DECLARE_METATYPE(QNetworkSession::SessionError);

class tst_QNetworkSession : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();

private slots:
    void invalidSession();

    void inProcessSessionManagement_data();
    void inProcessSessionManagement();

private:
    QNetworkConfigurationManager manager;
};

void tst_QNetworkSession::initTestCase()
{
    qRegisterMetaType<QNetworkSession::State>("QNetworkSession::State");
    qRegisterMetaType<QNetworkSession::SessionError>("QNetworkSession::SessionError");

    QSignalSpy spy(&manager, SIGNAL(updateCompleted()));
    manager.updateConfigurations();
    QTRY_VERIFY(spy.count() == 1);
}

void tst_QNetworkSession::inProcessSessionManagement_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");

    foreach (const QNetworkConfiguration &config, manager.allConfigurations())
        QTest::newRow(config.name().toLocal8Bit().constData()) << config;
}

void tst_QNetworkSession::invalidSession()
{
    QNetworkSession session(QNetworkConfiguration(), 0);
    QVERIFY(!session.isActive());
    QVERIFY(session.state() == QNetworkSession::Invalid);
}

void tst_QNetworkSession::inProcessSessionManagement()
{
    if (manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport) {
        QSKIP("System Session support is available, "
              "not testing in process sessions for this platform.", SkipAll);
    }

    QFETCH(QNetworkConfiguration, configuration);

    QNetworkSession session(configuration);

    // Test initial state of the session.
    {
        QVERIFY(session.configuration() == configuration);
        QVERIFY(!session.isActive());
        QVERIFY(session.state() != QNetworkSession::Invalid);
        QVERIFY(session.error() == QNetworkSession::UnknownSessionError);
        QCOMPARE(session.property(QLatin1String("SessionReferenceCount")).toUInt(), 0U);
    }

    // The remaining tests require the session to be not NotAvailable.
    if (session.state() == QNetworkSession::NotAvailable)
        QSKIP("Network is not available.", SkipSingle);

    bool sessionInitiallyConnected = session.state() == QNetworkSession::Connected;

    QSignalSpy sessionOpenedSpy(&session, SIGNAL(sessionOpened()));
    QSignalSpy sessionClosedSpy(&session, SIGNAL(sessionClosed()));
    QSignalSpy stateChangedSpy(&session, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy errorSpy(&session, SIGNAL(error(QNetworkSession::SessionError)));

    // Test opening the session.
    {
        bool expectStateChange = session.state() != QNetworkSession::Connected;

        session.open();

        QTRY_VERIFY(!sessionOpenedSpy.isEmpty() || !errorSpy.isEmpty());
        if (!errorSpy.isEmpty()) {
            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));
            if (error == QNetworkSession::OperationNotSupportedError) {
                // The session needed to bring up the interface,
                // but the operation is not supported.
                QSKIP("Configuration does not support open().", SkipSingle);
            } else if (error == QNetworkSession::InvalidConfigurationError) {
                // The session needed to bring up the interface, but it is not possible for the
                // specified configuration.
                if ((session.configuration().state() & QNetworkConfiguration::Discovered) ==
                    QNetworkConfiguration::Discovered) {
                    QFAIL("Failed to open session for Discovered configuration.");
                } else {
                    QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                }
            } else {
                QFAIL("Error opening session.");
            }
        } else if (!sessionOpenedSpy.isEmpty()) {
            QCOMPARE(sessionOpenedSpy.count(), 1);
            QVERIFY(sessionClosedSpy.isEmpty());
            QVERIFY(errorSpy.isEmpty());

            if (expectStateChange)
                QTRY_VERIFY(!stateChangedSpy.isEmpty());

            QVERIFY(session.state() == QNetworkSession::Connected);

            QCOMPARE(session.property(QLatin1String("SessionReferenceCount")).toUInt(), 1U);
        } else {
            QFAIL("Timeout waiting for session to open.");
        }
    }

    sessionOpenedSpy.clear();
    sessionClosedSpy.clear();
    stateChangedSpy.clear();
    errorSpy.clear();

    QNetworkSession session2(configuration);

    QSignalSpy sessionOpenedSpy2(&session2, SIGNAL(sessionOpened()));
    QSignalSpy sessionClosedSpy2(&session2, SIGNAL(sessionClosed()));

    // Test opening a second session.
    {
        QVERIFY(session2.configuration() == configuration);
        QVERIFY(!session2.isActive());
        QVERIFY(session2.state() == QNetworkSession::Connected);
        QVERIFY(session.error() == QNetworkSession::UnknownSessionError);
        QCOMPARE(session.property(QLatin1String("SessionReferenceCount")).toUInt(), 1U);

        session2.open();

        QTRY_VERIFY(!sessionOpenedSpy2.isEmpty());

        QVERIFY(session.isActive());
        QVERIFY(session2.isActive());
        QVERIFY(session.state() == QNetworkSession::Connected);
        QVERIFY(session2.state() == QNetworkSession::Connected);
        QCOMPARE(session.property(QLatin1String("SessionReferenceCount")).toUInt(), 2U);
        QCOMPARE(session2.property(QLatin1String("SessionReferenceCount")).toUInt(), 2U);
    }

    sessionOpenedSpy2.clear();

    // Test closing the second session.
    {
        session2.close();

        QTRY_VERIFY(!sessionClosedSpy2.isEmpty());

        QVERIFY(sessionClosedSpy.isEmpty());

        QVERIFY(session.isActive());
        QVERIFY(!session2.isActive());
        QVERIFY(session.state() == QNetworkSession::Connected);
        QVERIFY(session2.state() == QNetworkSession::Connected);
        QCOMPARE(session.property(QLatin1String("SessionReferenceCount")).toUInt(), 1U);
        QCOMPARE(session2.property(QLatin1String("SessionReferenceCount")).toUInt(), 1U);
    }

    sessionClosedSpy2.clear();

    // Test closing the first session.
    {
        bool expectStateChange = session.state() != QNetworkSession::Disconnected;

        session.close();

        QTRY_VERIFY(!sessionClosedSpy.isEmpty() || !errorSpy.isEmpty());

        QVERIFY(!session.isActive());

        if (expectStateChange)
            QTRY_VERIFY(!stateChangedSpy.isEmpty() || !errorSpy.isEmpty());

        if (!errorSpy.isEmpty()) {
            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));
            if (error == QNetworkSession::OperationNotSupportedError) {
                // The session needed to bring down the interface,
                // but the operation is not supported.
                QSKIP("Configuration does not support close().", SkipSingle);
            } else if (error == QNetworkSession::InvalidConfigurationError) {
                // The session needed to bring down the interface, but it is not possible for the
                // specified configuration.
                if ((session.configuration().state() & QNetworkConfiguration::Discovered) ==
                    QNetworkConfiguration::Discovered) {
                    QFAIL("Failed to open session for Discovered configuration.");
                } else {
                    QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                }
            } else {
                QFAIL("Error opening session.");
            }
        } else if (!sessionClosedSpy.isEmpty()) {
            QVERIFY(sessionOpenedSpy.isEmpty());
            QCOMPARE(sessionClosedSpy.count(), 1);
            QVERIFY(errorSpy.isEmpty());

            QVERIFY(session.state() == QNetworkSession::Disconnected);

            QCOMPARE(session.property(QLatin1String("SessionReferenceCount")).toUInt(), 0U);
        } else {
            QFAIL("Timeout waiting for session to open.");
        }
    }
}

QTEST_MAIN(tst_QNetworkSession)

#include "tst_qnetworksession.moc"

