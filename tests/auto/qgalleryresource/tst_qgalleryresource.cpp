/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

//TESTED_COMPONENT=src/documentgallery

#include <QtTest/QtTest>

#include <qgalleryitemlist.h>


Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QGalleryResource))

QTM_USE_NAMESPACE

class tst_QGalleryResource : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void null();
    void url();
    void attributes();
    void equality_data();
    void equality();
    void copy();
};

void tst_QGalleryResource::null()
{
    const int sampleRateKey = 3;
    const int audioCodecKey = 6;
    const int videoCodecKey = 9;
    const int invalidKey = 4;

    const QMap<int, QVariant> attributes;

    QGalleryResource resource;

    QCOMPARE(resource.url(), QUrl());
    QCOMPARE(resource.attributes(), attributes);
    QCOMPARE(resource.attribute(sampleRateKey), QVariant());
    QCOMPARE(resource.attribute(audioCodecKey), QVariant());
    QCOMPARE(resource.attribute(videoCodecKey), QVariant());
    QCOMPARE(resource.attribute(invalidKey), QVariant());
}

void tst_QGalleryResource::url()
{
    const int sampleRateKey = 3;
    const int audioCodecKey = 6;
    const int videoCodecKey = 9;
    const int invalidKey = 4;

    const QMap<int, QVariant> attributes;

    const QUrl url("file:///a/local/video/file.m4v");

    QGalleryResource resource(url);

    QCOMPARE(resource.url(), url);
    QCOMPARE(resource.attributes(), attributes);
    QCOMPARE(resource.attribute(sampleRateKey), QVariant());
    QCOMPARE(resource.attribute(audioCodecKey), QVariant());
    QCOMPARE(resource.attribute(videoCodecKey), QVariant());
    QCOMPARE(resource.attribute(invalidKey), QVariant());
}

void tst_QGalleryResource::attributes()
{
    const int sampleRateKey = 3;
    const int audioCodecKey = 6;
    const int videoCodecKey = 9;
    const int invalidKey = 4;

    const QVariant sampleRate = 44100;
    const QVariant audioCodec = QLatin1String("aac");
    const QVariant videoCodec = QLatin1String("h264");

    QMap<int, QVariant> attributes;
    attributes.insert(sampleRateKey, sampleRate);
    attributes.insert(audioCodecKey, audioCodec);
    attributes.insert(videoCodecKey, videoCodec);

    const QUrl url("file:///a/local/video/file.m4v");

    QGalleryResource resource(url, attributes);

    QCOMPARE(resource.url(), url);
    QCOMPARE(resource.attributes(), attributes);
    QCOMPARE(resource.attribute(sampleRateKey), sampleRate);
    QCOMPARE(resource.attribute(audioCodecKey), audioCodec);
    QCOMPARE(resource.attribute(videoCodecKey), videoCodec);
    QCOMPARE(resource.attribute(invalidKey), QVariant());
}

void tst_QGalleryResource::equality_data()
{
    const int sampleRateKey = 3;
    const int audioCodecKey = 6;
    const int videoCodecKey = 9;

    const QUrl url1("file:///a/local/video/file.m4v");
    const QUrl url2("http://example.com/video.mpg");

    QMap<int, QVariant> attributes1;
    attributes1.insert(sampleRateKey, 44100);
    attributes1.insert(audioCodecKey, QLatin1String("aac"));
    attributes1.insert(videoCodecKey,  QLatin1String("h264"));

    QMap<int, QVariant> attributes2;
    attributes2.insert(sampleRateKey, 22050);

    const QGalleryResource resource1(url1);
    const QGalleryResource resource2(url2);
    const QGalleryResource resource3(url1, attributes1);
    const QGalleryResource resource4(url1, attributes2);
    const QGalleryResource resource5(url2, attributes1);

    QTest::addColumn<QGalleryResource>("resource1");
    QTest::addColumn<QGalleryResource>("resource2");
    QTest::addColumn<bool>("isEqual");

    QTest::newRow("Null resources")
            << QGalleryResource()
            << QGalleryResource()
            << true;
    QTest::newRow("Same resource (No attributes)")
            << resource1
            << resource1
            << true;
    QTest::newRow("Same resource (Attributes)")
            << resource3
            << resource3
            << true;
    QTest::newRow("Equal resource (No attributes)")
            << QGalleryResource(url1)
            << QGalleryResource(url1)
            << true;
    QTest::newRow("Equal resource (Attributes)")
            << QGalleryResource(url1, attributes1)
            << QGalleryResource(url1, attributes1)
            << true;

    QTest::newRow("Null resource != Non-null (No attributes)")
            << QGalleryResource()
            << resource1
            << false;
    QTest::newRow("Null resource != Non-null (Attributes)")
            << QGalleryResource()
            << resource3
            << false;
    QTest::newRow("Different URL, no attributes")
            << resource1
            << resource2
            << false;
    QTest::newRow("Same URL, With/without attributes")
            << resource1
            << resource3
            << false;
    QTest::newRow("Same Url, Different attributes")
            << resource3
            << resource4
            << false;
    QTest::newRow("Different URL, same attributes")
            << resource3
            << resource5
            << false;
}

void tst_QGalleryResource::equality()
{
    QFETCH(QGalleryResource, resource1);
    QFETCH(QGalleryResource, resource2);
    QFETCH(bool, isEqual);

    QCOMPARE(resource1 == resource2, isEqual);
    QCOMPARE(resource2 == resource1 ,isEqual);
    QCOMPARE(resource1 != resource2, !isEqual);
    QCOMPARE(resource2 != resource1, !isEqual);
}


void tst_QGalleryResource::copy()
{
    const int sampleRateKey = 3;
    const int audioCodecKey = 6;
    const int videoCodecKey = 9;

    const QUrl url1("file:///a/local/video/file.m4v");
    const QUrl url2("http://example.com/video.mpg");

    QMap<int, QVariant> attributes1;
    attributes1.insert(sampleRateKey, 44100);
    attributes1.insert(audioCodecKey, QLatin1String("aac"));
    attributes1.insert(videoCodecKey,  QLatin1String("h264"));

    QMap<int, QVariant> attributes2;
    attributes2.insert(sampleRateKey, 22050);

    QGalleryResource resource1(url1);
    QGalleryResource resource2(url1, attributes1);
    QGalleryResource resource3(url2, attributes2);

    QCOMPARE(resource1.url(), url1);
    QCOMPARE(resource1.attributes().isEmpty(), true);

    QCOMPARE(resource2.url(), url1);
    QCOMPARE(resource2.attributes(), attributes1);

    QCOMPARE(resource3.url(), url2);
    QCOMPARE(resource3.attributes(), attributes2);

    resource1 = resource2;
    QCOMPARE(resource1.url(), url1);
    QCOMPARE(resource1.attributes(), attributes1);

    resource2 = resource3;
    QCOMPARE(resource2.url(), url2);
    QCOMPARE(resource2.attributes(), attributes2);
}


QTEST_MAIN(tst_QGalleryResource)

#include "tst_qgalleryresource.moc"
