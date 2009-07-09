/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
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
    void cleanupTestCase();

private slots:
    void invalidSession();

    void sessionProperties_data();
    void sessionProperties();

    void userChoiceSession_data();
    void userChoiceSession();

    void sessionOpenCloseStop_data();
    void sessionOpenCloseStop();

private:
    QNetworkConfigurationManager manager;

    uint inProcessSessionManagementCount;
};

void tst_QNetworkSession::initTestCase()
{
    qRegisterMetaType<QNetworkSession::State>("QNetworkSession::State");
    qRegisterMetaType<QNetworkSession::SessionError>("QNetworkSession::SessionError");

    inProcessSessionManagementCount = 0;

    QSignalSpy spy(&manager, SIGNAL(updateCompleted()));
    manager.updateConfigurations();
    QTRY_VERIFY(spy.count() == 1);
}

void tst_QNetworkSession::cleanupTestCase()
{
    if (!(manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport) &&
        (manager.capabilities() & QNetworkConfigurationManager::BearerManagement) &&
        inProcessSessionManagementCount == 0) {
        QFAIL("No usable configurations found to complete all possible "
              "tests in inProcessSessionManagement()");
    }
}

void tst_QNetworkSession::invalidSession()
{
    QNetworkSession session(QNetworkConfiguration(), 0);
    QVERIFY(!session.isActive());
    QVERIFY(session.state() == QNetworkSession::Invalid);
}

void tst_QNetworkSession::sessionProperties_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");

    QTest::newRow("invalid configuration") << QNetworkConfiguration();

    foreach (const QNetworkConfiguration &config, manager.allConfigurations()) {
        const QString name = config.name().isEmpty() ? QString("<Hidden>") : config.name();
        QTest::newRow(name.toLocal8Bit().constData()) << config;
    }
}

void tst_QNetworkSession::sessionProperties()
{
    QFETCH(QNetworkConfiguration, configuration);

    QNetworkSession session(configuration);

    QVERIFY(session.configuration() == configuration);

    QStringList validBearerNames = QStringList() << QString()
                                                 << QLatin1String("Ethernet")
                                                 << QLatin1String("WLAN")
                                                 << QLatin1String("2G")
                                                 << QLatin1String("CDMA2000")
                                                 << QLatin1String("WCDMA")
                                                 << QLatin1String("HSPA")
                                                 << QLatin1String("Bluetooth")
                                                 << QLatin1String("WiMAX");

    if (!configuration.isValid())
        QVERIFY(session.bearerName().isEmpty());
    else
        QVERIFY(validBearerNames.contains(session.bearerName()));

    // QNetworkSession::interface() should return an invalid interface unless
    // session is in the connected state.
    QCOMPARE(session.state() == QNetworkSession::Connected, session.interface().isValid());

    if (!configuration.isValid()) {
        QVERIFY(configuration.state() == QNetworkConfiguration::Undefined &&
                session.state() == QNetworkSession::Invalid);
    } else {
        switch (configuration.state()) {
        case QNetworkConfiguration::Undefined:
            QVERIFY(session.state() == QNetworkSession::NotAvailable);
            break;
        case QNetworkConfiguration::Defined:
            QVERIFY(session.state() == QNetworkSession::NotAvailable);
            break;
        case QNetworkConfiguration::Discovered:
            QVERIFY(session.state() == QNetworkSession::Connecting ||
                    session.state() == QNetworkSession::Disconnected);
            break;
        case QNetworkConfiguration::Active:
            QVERIFY(session.state() == QNetworkSession::Connected ||
                    session.state() == QNetworkSession::Closing ||
                    session.state() == QNetworkSession::Roaming);
            break;
        default:
            QFAIL("Invalid configuration state");
        };
    }
}

void tst_QNetworkSession::userChoiceSession_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");

    QNetworkConfiguration config = manager.defaultConfiguration();
    if (config.type() == QNetworkConfiguration::UserChoice)
        QTest::newRow("UserChoice") << config;
    else
        QSKIP("Default configuration is not a UserChoice configuration.", SkipAll);
}

void tst_QNetworkSession::userChoiceSession()
{
    QFETCH(QNetworkConfiguration, configuration);

    QVERIFY(configuration.type() == QNetworkConfiguration::UserChoice);

    QNetworkSession session(configuration);

    QVERIFY(session.configuration() == configuration);

    QVERIFY(!session.isActive());

    QVERIFY(session.property("ActiveConfigurationIdentifier").toString().isEmpty());


    // The remaining tests require the session to be not NotAvailable.
    if (session.state() == QNetworkSession::NotAvailable)
        QSKIP("Network is not available.", SkipSingle);

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

            const QString userChoiceIdentifier =
                session.property("UserChoiceConfigurationIdentifier").toString();

            QVERIFY(!userChoiceIdentifier.isEmpty());
            QVERIFY(userChoiceIdentifier != configuration.identifier());

            QNetworkConfiguration userChoiceConfiguration =
                manager.configurationFromIdentifier(userChoiceIdentifier);

            QVERIFY(userChoiceConfiguration.isValid());
            QVERIFY(userChoiceConfiguration.type() != QNetworkConfiguration::UserChoice);

            const QString activeIdentifier =
                session.property("ActiveConfigurationIdentifier").toString();

            QVERIFY(!activeIdentifier.isEmpty());
            QVERIFY(activeIdentifier != configuration.identifier());

            QNetworkConfiguration activeConfiguration =
                manager.configurationFromIdentifier(activeIdentifier);

            QVERIFY(activeConfiguration.isValid());
            QVERIFY(activeConfiguration.type() == QNetworkConfiguration::InternetAccessPoint);

            if (userChoiceConfiguration.type() == QNetworkConfiguration::InternetAccessPoint) {
                QVERIFY(userChoiceConfiguration == activeConfiguration);
            } else {
                QVERIFY(userChoiceConfiguration.type() == QNetworkConfiguration::ServiceNetwork);
                QVERIFY(userChoiceConfiguration.children().contains(activeConfiguration));
            }
        } else {
            QFAIL("Timeout waiting for session to open.");
        }
    }
}

void tst_QNetworkSession::sessionOpenCloseStop_data()
{
    QTest::addColumn<QNetworkConfiguration>("configuration");
    QTest::addColumn<bool>("forceSessionStop");

    foreach (const QNetworkConfiguration &config, manager.allConfigurations()) {
        const QString name = config.name().isEmpty() ? QString("<Hidden>") : config.name();
        QTest::newRow((name + QLatin1String(" close")).toLocal8Bit().constData())
            << config << false;
        QTest::newRow((name + QLatin1String(" stop")).toLocal8Bit().constData())
            << config << true;
    }
}

void tst_QNetworkSession::sessionOpenCloseStop()
{
    QFETCH(QNetworkConfiguration, configuration);
    QFETCH(bool, forceSessionStop);

    QNetworkSession session(configuration);

    // Test initial state of the session.
    {
        QVERIFY(session.configuration() == configuration);
        QVERIFY(!session.isActive());
        QVERIFY(session.state() != QNetworkSession::Invalid);
        QVERIFY(session.error() == QNetworkSession::UnknownSessionError);
    }

    // The remaining tests require the session to be not NotAvailable.
    if (session.state() == QNetworkSession::NotAvailable)
        QSKIP("Network is not available.", SkipSingle);

    QSignalSpy sessionOpenedSpy(&session, SIGNAL(sessionOpened()));
    QSignalSpy sessionClosedSpy(&session, SIGNAL(sessionClosed()));
    QSignalSpy stateChangedSpy(&session, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy errorSpy(&session, SIGNAL(error(QNetworkSession::SessionError)));

    // Test opening the session.
    {
        QNetworkSession::State previousState = session.state();
        bool expectStateChange = previousState != QNetworkSession::Connected;

        session.open();

        QTRY_VERIFY(!sessionOpenedSpy.isEmpty() || !errorSpy.isEmpty());
        if (!errorSpy.isEmpty()) {
            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));

            QVERIFY(session.state() == previousState);

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
            } else if (error == QNetworkSession::UnknownSessionError) {
                    QSKIP("Unknown Session error.", SkipSingle);
            } else {
                QFAIL("Error opening session.");
            }
        } else if (!sessionOpenedSpy.isEmpty()) {
            QCOMPARE(sessionOpenedSpy.count(), 1);
            QVERIFY(sessionClosedSpy.isEmpty());
            QVERIFY(errorSpy.isEmpty());

            if (expectStateChange) {
                QTRY_VERIFY(stateChangedSpy.count() >= 2);

                QNetworkSession::State state =
                    qvariant_cast<QNetworkSession::State>(stateChangedSpy.at(0).at(0));
                QVERIFY(state == QNetworkSession::Connecting);

                state = qvariant_cast<QNetworkSession::State>(stateChangedSpy.at(1).at(0));
                QVERIFY(state == QNetworkSession::Connected);
            }

            QVERIFY(session.state() == QNetworkSession::Connected);
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
    QSignalSpy stateChangedSpy2(&session2, SIGNAL(stateChanged(QNetworkSession::State)));
    QSignalSpy errorSpy2(&session2, SIGNAL(error(QNetworkSession::SessionError)));

    // Test opening a second session.
    {
        QVERIFY(session2.configuration() == configuration);
        QVERIFY(!session2.isActive());
        QVERIFY(session2.state() == QNetworkSession::Connected);
        QVERIFY(session.error() == QNetworkSession::UnknownSessionError);

        session2.open();

        QTRY_VERIFY(!sessionOpenedSpy2.isEmpty());

        QVERIFY(session.isActive());
        QVERIFY(session2.isActive());
        QVERIFY(session.state() == QNetworkSession::Connected);
        QVERIFY(session2.state() == QNetworkSession::Connected);
    }

    sessionOpenedSpy2.clear();

    if (forceSessionStop) {
        // Test forcing the second session to stop the interface.
        QNetworkSession::State previousState = session.state();
        bool expectStateChange = previousState != QNetworkSession::Disconnected;

        session2.stop();

        QTRY_VERIFY(!sessionClosedSpy2.isEmpty() || !errorSpy2.isEmpty());

        QVERIFY(!session2.isActive());

        if (!errorSpy2.isEmpty()) {
            // check for SessionAbortedError
            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy.first().at(0));
            QNetworkSession::SessionError error2 =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy2.first().at(0));

            QVERIFY(error == QNetworkSession::SessionAbortedError);
            QVERIFY(error2 == QNetworkSession::SessionAbortedError);

            QCOMPARE(errorSpy.count(), 1);
            QCOMPARE(errorSpy2.count(), 1);

            errorSpy.clear();
            errorSpy2.clear();
        }

        QVERIFY(errorSpy.isEmpty());
        QVERIFY(errorSpy2.isEmpty());

        if (expectStateChange)
            QTRY_VERIFY(stateChangedSpy2.count() >= 2 || !errorSpy2.isEmpty());

        if (!errorSpy2.isEmpty()) {
            QVERIFY(session2.state() == previousState);
            QVERIFY(session.state() == previousState);

            QNetworkSession::SessionError error =
                qvariant_cast<QNetworkSession::SessionError>(errorSpy2.first().at(0));
            if (error == QNetworkSession::OperationNotSupportedError) {
                // The session needed to bring down the interface,
                // but the operation is not supported.
                QSKIP("Configuration does not support stop().", SkipSingle);
            } else if (error == QNetworkSession::InvalidConfigurationError) {
                // The session needed to bring down the interface, but it is not possible for the
                // specified configuration.
                if ((session.configuration().state() & QNetworkConfiguration::Discovered) ==
                    QNetworkConfiguration::Discovered) {
                    QFAIL("Failed to stop session for Discovered configuration.");
                } else {
                    QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                }
            } else {
                QFAIL("Error stopping session.");
            }
        } else if (!sessionClosedSpy2.isEmpty()) {
            if (expectStateChange) {
                QCOMPARE(stateChangedSpy2.count(), 2);

                QNetworkSession::State state =
                    qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(0).at(0));
                QVERIFY(state == QNetworkSession::Closing);

                state = qvariant_cast<QNetworkSession::State>(stateChangedSpy2.at(1).at(0));
                QVERIFY(state == QNetworkSession::Disconnected);

                QVERIFY(session.state() == QNetworkSession::Disconnected);
                QVERIFY(session2.state() == QNetworkSession::Disconnected);
            }

            QVERIFY(errorSpy.isEmpty());
            QVERIFY(errorSpy2.isEmpty());

            ++inProcessSessionManagementCount;
        } else {
            QFAIL("Timeout waiting for session to stop.");
        }

        QVERIFY(!sessionClosedSpy.isEmpty());
        QVERIFY(!sessionClosedSpy2.isEmpty());

        QVERIFY(!session.isActive());
        QVERIFY(!session2.isActive());
    } else {
        // Test closing the second session.
        {
            session2.close();

            QTRY_VERIFY(!sessionClosedSpy2.isEmpty());

            QVERIFY(sessionClosedSpy.isEmpty());

            QVERIFY(session.isActive());
            QVERIFY(!session2.isActive());
            QVERIFY(session.state() == QNetworkSession::Connected);
            QVERIFY(session2.state() == QNetworkSession::Connected);
        }

        sessionClosedSpy2.clear();

        // Test closing the first session.
        {
            bool expectStateChange = session.state() != QNetworkSession::Disconnected &&
                                     manager.capabilities() & QNetworkConfigurationManager::SystemSessionSupport;

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
                        QFAIL("Failed to close session for Discovered configuration.");
                    } else {
                        QSKIP("Cannot test session for non-Discovered configuration.", SkipSingle);
                    }
                } else {
                    QFAIL("Error closing session.");
                }
            } else if (!sessionClosedSpy.isEmpty()) {
                QVERIFY(sessionOpenedSpy.isEmpty());
                QCOMPARE(sessionClosedSpy.count(), 1);
                if (expectStateChange)
                    QVERIFY(!stateChangedSpy.isEmpty());
                QVERIFY(errorSpy.isEmpty());

                if (expectStateChange)
                    QVERIFY(session.state() == QNetworkSession::Disconnected);

                ++inProcessSessionManagementCount;
            } else {
                QFAIL("Timeout waiting for session to close.");
            }
        }
    }
}

QTEST_MAIN(tst_QNetworkSession)

#include "tst_qnetworksession.moc"

