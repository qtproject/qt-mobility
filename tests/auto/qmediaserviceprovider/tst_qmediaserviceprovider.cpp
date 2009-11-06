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

#include <qmediaserviceprovider.h>

class MockMediaServiceProvider : public QMediaServiceProvider
{
    QMediaService* requestService(const QByteArray &type, const QMediaServiceProviderHint &)
    {
        Q_UNUSED(type);
        return 0;
    }

    void releaseService(QMediaService *service)
    {
        Q_UNUSED(service);
    }
};


class tst_QMediaServiceProvider : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultProviderAvailable();
    void testObtainService();
    void testHasSupport();
    void testDevices();
    void testProviderHints();
};

void tst_QMediaServiceProvider::testDefaultProviderAvailable()
{
    // Must always be a default provider available
    QVERIFY(QMediaServiceProvider::defaultServiceProvider() != 0);
}

void tst_QMediaServiceProvider::testObtainService()
{
    QMediaServiceProvider *provider = QMediaServiceProvider::defaultServiceProvider();

    if (provider == 0)
        QSKIP("No default provider", SkipSingle);

    QMediaService *service = 0;

    // Player
    service = provider->requestService(Q_MEDIASERVICE_MEDIAPLAYER);
    QVERIFY(service != 0);
    provider->releaseService(service);
}

void tst_QMediaServiceProvider::testHasSupport()
{
    MockMediaServiceProvider mockProvider;
    QCOMPARE(mockProvider.hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER), "video/ogv", QStringList()),
             QtMedia::MaybeSupported);

    QMediaServiceProvider *provider = QMediaServiceProvider::defaultServiceProvider();

    if (provider == 0)
        QSKIP("No default provider", SkipSingle);

    QCOMPARE(provider->hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER), "video/ogv", QStringList()),
             QtMedia::MaybeSupported);

    QCOMPARE(provider->hasSupport(QByteArray("non existing service"), "video/ogv", QStringList()),
             QtMedia::NotSupported);
}

void tst_QMediaServiceProvider::testDevices()
{
    MockMediaServiceProvider mockProvider;
    QVERIFY(mockProvider.devices(QByteArray(Q_MEDIASERVICE_CAMERA)).isEmpty());
    QVERIFY(mockProvider.deviceDescription(QByteArray(Q_MEDIASERVICE_CAMERA),
                                           QByteArray()).isEmpty());

    QMediaServiceProvider *provider = QMediaServiceProvider::defaultServiceProvider();

    if (provider == 0)
        QSKIP("No default provider", SkipSingle);

    //how to test this properly?
    //it depends on plugins installed
    //QVERIFY(!provider->devices(QByteArray(Q_MEDIASERVICE_CAMERA)).isEmpty());

    QVERIFY(provider->devices(QByteArray("non existing service")).isEmpty());
}


void tst_QMediaServiceProvider::testProviderHints()
{
    {
        QMediaServiceProviderHint hint;
        QVERIFY(hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::Null);
        QVERIFY(hint.device().isEmpty());
        QVERIFY(hint.mimeType().isEmpty());
        QVERIFY(hint.codecs().isEmpty());
        QCOMPARE(hint.features(), 0);
    }

    {
        QByteArray deviceName(QByteArray("testDevice"));
        QMediaServiceProviderHint hint(deviceName);
        QVERIFY(!hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::Device);
        QCOMPARE(hint.device(), deviceName);
        QVERIFY(hint.mimeType().isEmpty());
        QVERIFY(hint.codecs().isEmpty());
        QCOMPARE(hint.features(), 0);
    }

    {
        QMediaServiceProviderHint hint(QMediaServiceProviderHint::LowLatencyPlayback);
        QVERIFY(!hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::SupportedFeatures);
        QVERIFY(hint.device().isEmpty());
        QVERIFY(hint.mimeType().isEmpty());
        QVERIFY(hint.codecs().isEmpty());
        QCOMPARE(hint.features(), QMediaServiceProviderHint::LowLatencyPlayback);
    }

    {
        QString mimeType(QLatin1String("video/ogg"));
        QStringList codecs;
        codecs << "theora" << "vorbis";

        QMediaServiceProviderHint hint(mimeType,codecs);
        QVERIFY(!hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::ContentType);
        QVERIFY(hint.device().isEmpty());
        QCOMPARE(hint.mimeType(), mimeType);
        QCOMPARE(hint.codecs(), codecs);

        QMediaServiceProviderHint hint2(hint);

        QVERIFY(!hint2.isNull());
        QCOMPARE(hint2.type(), QMediaServiceProviderHint::ContentType);
        QVERIFY(hint2.device().isEmpty());
        QCOMPARE(hint2.mimeType(), mimeType);
        QCOMPARE(hint2.codecs(), codecs);

        QMediaServiceProviderHint hint3;
        QVERIFY(hint3.isNull());
        hint3 = hint;
        QVERIFY(!hint3.isNull());
        QCOMPARE(hint3.type(), QMediaServiceProviderHint::ContentType);
        QVERIFY(hint3.device().isEmpty());
        QCOMPARE(hint3.mimeType(), mimeType);
        QCOMPARE(hint3.codecs(), codecs);

        QCOMPARE(hint, hint2);
        QCOMPARE(hint3, hint2);

        QMediaServiceProviderHint hint4(mimeType,codecs);
        QCOMPARE(hint, hint4);

        QMediaServiceProviderHint hint5(mimeType,QStringList());
        QVERIFY(hint != hint5);
    }
}

QTEST_MAIN(tst_QMediaServiceProvider)

#include "tst_qmediaserviceprovider.moc"
