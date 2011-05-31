/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "moduletest_configurability.h"

#include <QtTest/QtTest>
#include <QtDebug>
#include <QVariant>
#include <QValueSpaceSubscriber>
#include <QValueSpacePublisher>

#ifdef Q_OS_SYMBIAN
#include <featdiscovery.h>
#endif

const QString ownCrFullPath("/cr/0xE056F50B/1");
const QString ownCrPath("/cr/0xE056F50B");
const QString ownCrKey("1");
const QString ownPsFullPath("/ps/0xE056F50B/1");

// ======== LOCAL FUNCTIONS ========

bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000) {
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);

    loop.connect(sender, signal, SLOT(quit()));
    loop.connect(&timer, SIGNAL(timeout()), SLOT(quit()));
    timer.start();
    loop.exec();

    return timer.isActive();
}


// ======== MEMBER FUNCTIONS ========

void ModuletestConfigurability::initTestCase()
{
}

void ModuletestConfigurability::init()
{
    // set own cr key to default value
    QtMobility::QValueSpacePublisher publisher(ownCrPath, this);
    publisher.setValue(ownCrKey, QVariant(0));

    // remove own ps key
    QtMobility::QValueSpacePublisher publisher2(ownPsFullPath, this);
    publisher2.resetValue(QString());
}

void ModuletestConfigurability::cleanup()
{
}

void ModuletestConfigurability::cleanupTestCase()
{
}

void ModuletestConfigurability::readAndSetCenrep()
{
    QtMobility::QValueSpaceSubscriber subscriber(ownCrFullPath, this);
    QtMobility::QValueSpacePublisher publisher(ownCrFullPath, this);

    QVariant value = subscriber.value();
    QCOMPARE(value.type(), QVariant::Int);
    QCOMPARE(value.toInt(), 0);

    // using just signalspy doesn't emit connectNotify signal, so QValueSpaceSubscriber
    // doesn't start listening to change event.
    //   ==> connect the signal to dummy slot
    connect(&subscriber, SIGNAL(contentsChanged()), this, SLOT(dummy()));
    QSignalSpy spy(&subscriber, SIGNAL(contentsChanged()));

    publisher.setValue(QString(), QVariant(1234));

    // wait for the change notification (timout: 5 seconds)
    QVERIFY(waitForSignal(&subscriber, SIGNAL(contentsChanged()), 5000));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value().toInt(), 1234);
}

void ModuletestConfigurability::partialCenrepPath()
{
    QtMobility::QValueSpaceSubscriber subscriber(ownCrPath, this);
    QtMobility::QValueSpacePublisher publisher(ownCrPath, this);

    QCOMPARE(subscriber.value(ownCrKey).toInt(), 0);

    // It is not possible to get change notifications for partial numeric path.

    publisher.setValue(ownCrKey, QVariant(1234));

    QCOMPARE(subscriber.value(ownCrKey).toInt(), 1234);
}

void ModuletestConfigurability::readAndSetPubsub()
{
    QtMobility::QValueSpaceSubscriber subscriber(ownPsFullPath, this);
    QtMobility::QValueSpacePublisher publisher(ownPsFullPath, this);

    QVariant value = subscriber.value();
    QCOMPARE(value.type(), QVariant::Invalid);

    publisher.setValue(QString(), QVariant(1));

    QCOMPARE(subscriber.value().toInt(), 1);

    // we can start listening to change notifications only after the pubsub
    // key has some value (Qt Mobility restriction).
    connect(&subscriber, SIGNAL(contentsChanged()), this, SLOT(dummy()));
    QSignalSpy spy(&subscriber, SIGNAL(contentsChanged()));

    publisher.setValue(QString(), QVariant(1234));

    // wait for the change notification (timout: 5 seconds)
    QVERIFY(waitForSignal(&subscriber, SIGNAL(contentsChanged()), 5000));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(subscriber.value().toInt(), 1234);
}

void ModuletestConfigurability::invalidPath_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<bool>("valid");
    QTest::newRow("cr: too many parts") << QString("/cr/0xE056F50B/0x1/0x2") << false;
    QTest::newRow("cr: too few parts") << QString("/cr/0xE056F50B") << false;
    QTest::newRow("fm: too many parts") << QString("/fm/0x1/0x2") << false;
    QTest::newRow("ps: number parsing") << QString("/ps/E056F50B/1") << false;
    QTest::newRow("cr: valid path") << QString("/cr/0xE056F50B/1") << true;
    QTest::newRow("fm: valid path") << QString("/fm/0x1") << true;
}

void ModuletestConfigurability::invalidPath()
{
    // basically this tests that the code doesn't crash with malformed paths.
    // if the path is illegal QValueSpaceSubscriber::value returns the default value.
    // (same as valid but undefined path)

    const QVariant KDefaultValue(-9999);
    QFETCH(QString, path);
    QFETCH(bool, valid);
    QtMobility::QValueSpaceSubscriber subscriber(path, this);

    QCOMPARE((subscriber.value(QString(), KDefaultValue) != KDefaultValue), valid);
}

// Dummy slot for connecting QValueSpaceSubscriber::contentsChanged signal.
// Subscriber doesn't start listening to change events before it is connected
// to some slot.
void ModuletestConfigurability::dummy()
{
}

void ModuletestConfigurability::featManagerSimpleSubscriber_data()
{
    QTest::addColumn<QString>("feature");
    QTest::addColumn<int>("expectedResult");

    QTest::newRow("valid 1.") << "/fm/0x6b8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 2.") << "/fm/0x6B8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 3.") << "fm/0x6b8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 4.") << "fm/0x6B8" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 5.") << "/fm/1720" << static_cast<int>(QVariant::Bool);
    QTest::newRow("valid 6.") << "/fm//0x6b8" << static_cast<int>(QVariant::Bool);
    
    // invalid
    QTest::newRow("invalid 1.") << "/fm/0/1" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 2.") << "/fm/0/2/5" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 3.") << "/fm/0/1" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 4.") << "/fm/a" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 5.") << "/fm/-1" << static_cast<int>(QVariant::Invalid);
    QTest::newRow("invalid 6.") << "/fm/zzz" << static_cast<int>(QVariant::Invalid);
}

void ModuletestConfigurability::featManagerSimpleSubscriber()
{
    QFETCH(QString, feature);

    QtMobility::QValueSpaceSubscriber subscriber1("");

    subscriber1.setPath(feature);
    QVariant result1 = subscriber1.value();

    QTEST(static_cast<int>(result1.type()), "expectedResult");

    QtMobility::QValueSpaceSubscriber subscriber2(feature);
    QVariant result2 = subscriber2.value();

    QTEST(static_cast<int>(result2.type()), "expectedResult");

}

#ifdef Q_OS_SYMBIAN

void ModuletestConfigurability::featManagerAdvSubscriber_data()
{
    QTest::addColumn<QString>("feature");
    QTest::addColumn<int>("featureNum");

    QList<int> featList;
    QList<QString> formatStrings;

    // list of tested features
    featList
    << 0x6b8 // KFeatureId3DRingingTones
    << 0xfc // KFeatureIdFfAbcAgressiveUi
    << 0x8c // KFeatureIdFfAdaptiveWlanScanningSupport
    << 0xf1 // KFeatureIdFfAdditionalFonts
    << 0xac // KFeatureIdFfAiWlansnifferWidget
    << 0x13b // KFeatureIdFfAlternativeCallWaitingTone
    << 0x152 // KFeatureIdArabicHandwritingRecognitionInput
    << 0x6a7 // KFeatureIdAsynchFileSaveQueue
    << 0x3f // KFeatureIdFfAudt
    << 0x14a // KFeatureIdFfAutomotivestack
    << 0x6bb // KFeatureIdAutoRedialForVideoCall
    << 0xd3 // KFeatureIdFfAvkonEmotionIconEnabled
    << 0x16c // KFeatureIdFfBigclockScreensaver
    << 0xbd // KFeatureIdFfBirthdayImprovement
    << 0x10f // KFeatureIdFfBluetoothDataProfilesDisabled
    << 0x10e // KFeatureIdFfBluetoothDisabled
    << 0x162 // KFeatureIdFfBoxHwrInput
    << 0x169 // KFeatureIdBrowserGracefulSocketShutdown
    << 0x6bc // KFeatureIdBrowserProgressiveDownload
    << 0x132 // KFeatureIdFfCalDayView
    << 0x133 // KFeatureIdFfCalScrollablePreview
    << 0x1490 // KFeatureIdFfCalendarWidget
    << 0x128 // KFeatureIdFfCalMeetingRequestUi
    << 0x17e // KFeatureIdFfCameraLongKeyPress
    << 0xb7 // KFeatureIdFfCapacitiveDisplay
    << 0x16d // KFeatureIdFfChineseEngineImprove
    << 0x8a // KFeatureIdChineseFontsHires
    << 0xe9 // KFeatureIdFfChineseSelectionKey
    << 0xbe // KFeatureIdFfClockMultiprocolSupport
    << 0xf7 // KFeatureIdFfCmailIntegration
    << 0x43 // KFeatureIdCommsDatCreationFromXml
    << 0x17d // KFeatureIdFfConferenceBridgePinDialing
    << 0xb3 // KFeatureIdFfConnectionOverride
    << 0xec // KFeatureIdFfContactsCompanyNames
    << 0x10c // KFeatureIdFfContactsMerge
    << 0x10b // KFeatureIdffContactsMycard
    << 0x183 // KFeatureIdFfContactsNewContact
    << 0xb6 // KFeatureIdFfContactsNickname
    << 0x176 // KFeatureIdFfContactsNonEditableMycardNumber
    << 0xeb // KFeatureIdFfContactsPredictiveSearch
    << 0xea // KFeatureIdFfContactsRemoteLookup
    << 0x111 // KFeatureIdFfContactsSocial
    << 0x186 // KFeatureIdFfContactsStatusesInNamesList
    << 0x165 // KFeatureIdFfContactsVcardSyncGroup
    << 0xfe // KFeatureIdFfContextProfiles
    << 0xfa // KFeatureIdFfDeviceEncryptionFeature
    << 0x134 // KFeatureIdFfDisableOmaSuplV2
    << 0x1461 // KFeatureIdFfDisplayNetworkNameAfterCsRegistration
    << 0x115 // KFeatureIdFfDmConnmoAdapter
    << 0x16f // KFeatureIdFfDmIapOverWrite
    << 0x13c9 // KFeatureIdDmInstallSecurity
    << 0x19d // FfDolbyHeadphone
    << 0xc2 // KFeatureIdFfDualLanguageSupport
    << 0xe6 // KFeatureIdDualModem
    << 0x1a4 // KFeatureIdFfEePnnSpn
    << 0x158 // KFeatureIdFfEmailConversationView
    << 0x87 // KFeatureIdFfEmailFramework
    << 0x145 // KFeatureIdFfEmailFullscreenviewer
    << 0x89 // KFeatureIdFfEmailIpsServices
    << 0x7e // KFeatureIdEmailMceIntegration
    << 0x144 // KFeatureIdFfEmailPredictivesearch
    << 0xd7 // KFeatureIdFfEmailProtocolPlugins
    << 0x88 // KFeatureIdFfEmailUi
    << 0x143 // KFeatureIdFfEmailUiSplitscreen
    << 0x157 // KFeatureIdFfEmergencyCbs
    << 0x135 // KFeatureIdEnableIsiCommunicationInUsbChargingMode
    << 0x127 // KFeatureIdFfEnhancedCalendarEventUi
    << 0x1499 // KFeatureIdFfEnhUplinkNoiceCancel
    << 0x1469 // KFeatureIdFfEntryPointForVideoShare
    << 0xd5 // KFeatureIdFfFarsiCalendar
    << 0x161 // KFeatureIdFfFingerHwrInput
    << 0x6a9 // KFeatureIdFmtx
    << 0x170 // KFeatureIdFfFmtxAutoTune
    << 0xdc // KFeatureIdFfFmtxRdsText
    << 0x181 // FfFontTable
    << 0x146 // KFeatureIdFfFotaCheckLimitation
    << 0x16b // KFeatureIdFfFotaDecryptMemBeforeUpdate
    << 0x42 // KFeatureIdFullscrHwrInput
    << 0x175 // KFeatureIdFfFuzzyPinyinSettingAndContactsImportingForChineseInput
    << 0x101 // KFeatureIdFfGenericAccessNetwork
    << 0xbc // KFeatureIdFfGenericFontsAssets
    << 0x17f // KFeatureIdFfGstreamerIadUpdatable
    << 0x149a // KFeatureIdFfGstreamerPlaybackEnablerPlugins
    << 0xa9 // KFeatureIdFfHdmi
    << 0xa4 // KFeatureIdFfHelpTutorialMerge
    << 0xd4 // KFeatureIdFfHijriCalendar
    << 0x6ac // KFeatureIdHitchcockAppShell
    << 0x9a // KFeatureIdFfHomescreenSwitchableRoot
    << 0x13dd // KFeatureIdProductIncludesHomeScreenEasyDialing
    << 0x13ea // KFeatureIdFfHomeScreenVanityDialing
    << 0x1e // KFeatureIdIAUpdate
    << 0xb9 // KFeatureIdFfIaupdatePhase2
    << 0x180 // KFeatureIdFfIaupdateStartAfterFota
    << 0x15d // KFeatureIdFfIce
    << 0xff // KFeatureIdFfImageEditor
    << 0x146e // KFeatureIdFfImsDeregistrationInOffline
    << 0x146f // KFeatureIdFfImsDeregistrationInRfs
    << 0x1459 // KFeatureIdFfImsDeregistrationInVpn
    << 0xc0 // KFeatureIdFfInputsPerOrientation
    << 0xd8 // KFeatureIdFfIntelligentTextInput
    << 0x198 // FfIsimGbaAuthentication
    ;

    // format strings for features
    formatStrings
        << "/fm/%#x"
        << "/fm/%#X"
        << "/fm/%d"
        << "fm/%#x"
        << "fm/%#X"
        << "fm/%d"
        << "fm//%#x"
        << "fm//%#X"
        << "fm//%d"
        << "fm//%#x/"
        << "fm//%#X/"
        << "fm//%d/"
        ;

    foreach (int featureNum, featList) {
        foreach (QString formatString, formatStrings) {
            QString featureString = QString().sprintf(formatString.toLatin1(), featureNum);

            QTest::newRow(QString("%1:%2").arg(featureNum).arg(formatString).toLatin1().data())
                        << featureString
                        << featureNum;
        }
    }

}

void ModuletestConfigurability::featManagerAdvSubscriber()
{
    QFETCH(QString, feature);
    QFETCH(int, featureNum);

    CFeatureDiscovery *featDiscovery = 0;
    try {
        QT_TRAP_THROWING(featDiscovery = CFeatureDiscovery::NewL());
    } catch (std::exception &e) {
        delete featDiscovery;
        QFAIL(e.what());
    }

    QtMobility::QValueSpaceSubscriber subscriber("");

    TUid uid = TUid::Uid(featureNum);

    TBool isSupportedOrig = featDiscovery->IsSupported(uid);

    subscriber.setPath(feature);
    QVERIFY2(subscriber.value() == isSupportedOrig, QString("Feature manager: wrong result for feature: %1").arg(feature).toLatin1().data());

    delete featDiscovery;
}

#endif

void ModuletestConfigurability::featManagerMapperCase_data()
{
    QTest::addColumn<QString>("feature");
    QTest::addColumn<QString>("featureReference");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("c") << "/featuremanager/c" << "0x6b8" << true;
    QTest::newRow("d") << "/featuremanager/d" << "0xfc" << true;
    QTest::newRow("e") << "/featuremanager/e" << "0x8c" << true;
    QTest::newRow("i") << "/featuremanager/invalid" << "invalid" << false;

}

void ModuletestConfigurability::featManagerMapperCase()
{
    QFETCH(QString, feature);
    QFETCH(QString, featureReference);

    QtMobility::QValueSpaceSubscriber subscriber(feature);
    QtMobility::QValueSpaceSubscriber referenceSubscriber("/fm/" + featureReference);

    QTEST(subscriber.value().type() == QVariant::Bool, "expectedResult");

    QCOMPARE(subscriber.value(), referenceSubscriber.value());
}


QTEST_MAIN(ModuletestConfigurability)
