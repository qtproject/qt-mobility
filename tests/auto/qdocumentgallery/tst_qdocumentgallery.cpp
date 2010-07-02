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

#include <QtTest/QtTest>

#include <qdocumentgallery.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGalleryProperty::Attributes);

class tst_QDocumentGallery : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void isRequestSupported();
    void itemTypeProperties_data();
    void itemTypeProperties();
    void propertyAttributes_data();
    void propertyAttributes();

private:
    QDocumentGallery gallery;
};

void tst_QDocumentGallery::isRequestSupported()
{
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
    const bool platformSupported = true;
#else
    const bool platformSupported = false;
#endif

    QCOMPARE(gallery.isRequestSupported(QGalleryAbstractRequest::Item), platformSupported);
    QCOMPARE(gallery.isRequestSupported(QGalleryAbstractRequest::Query), platformSupported);
    QCOMPARE(gallery.isRequestSupported(QGalleryAbstractRequest::Count), platformSupported);
    QCOMPARE(gallery.isRequestSupported(QGalleryAbstractRequest::Url), platformSupported);
    QCOMPARE(gallery.isRequestSupported(QGalleryAbstractRequest::Remove), platformSupported);
    QCOMPARE(gallery.isRequestSupported(QGalleryAbstractRequest::Type(1000)), false);
}

void tst_QDocumentGallery::itemTypeProperties_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QStringList>("propertyNames");

    QTest::newRow("null item type") << QString() << QStringList();
    QTest::newRow("non-existent item type") << QString::fromLatin1("Hello") << QStringList();

    const QStringList fileProperties = QStringList()
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
            << QDocumentGallery::copyright
            << QDocumentGallery::fileName
            << QDocumentGallery::fileSize
            << QDocumentGallery::language
            << QDocumentGallery::lastAccessed
            << QDocumentGallery::lastModified
            << QDocumentGallery::mimeType
#ifndef Q_WS_MAEMO_5
            << QDocumentGallery::previewImage
            << QDocumentGallery::previewPixmap
#endif
            << QDocumentGallery::thumbnailImage
            << QDocumentGallery::thumbnailPixmap
#endif
            ;

    QTest::newRow("File") << QString(QDocumentGallery::File) << (QStringList(fileProperties)
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
            << QDocumentGallery::author
            << QDocumentGallery::description
            << QDocumentGallery::keywords
            << QDocumentGallery::rating
            << QDocumentGallery::subject
            << QDocumentGallery::title
#endif
        );

    QTest::newRow("Audio") << QString(QDocumentGallery::Audio) << (QStringList(fileProperties)
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
            << QDocumentGallery::albumArtist
            << QDocumentGallery::albumTitle
            << QDocumentGallery::artist
            << QDocumentGallery::audioBitRate
            << QDocumentGallery::audioCodec
            << QDocumentGallery::channelCount
            << QDocumentGallery::description
            << QDocumentGallery::discNumber
            << QDocumentGallery::duration
            << QDocumentGallery::genre
            << QDocumentGallery::lastPlayed
            << QDocumentGallery::lyrics
            << QDocumentGallery::performer
            << QDocumentGallery::playCount
            << QDocumentGallery::sampleRate
            << QDocumentGallery::title
            << QDocumentGallery::trackNumber
#endif
        );

    QTest::newRow("Album") << QString(QDocumentGallery::Album) << (QStringList()
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
            << QDocumentGallery::artist
            << QDocumentGallery::duration
            << QDocumentGallery::title
            << QDocumentGallery::trackCount
#endif
        );
}

void tst_QDocumentGallery::itemTypeProperties()
{
    QFETCH(QString, itemType);
    QFETCH(QStringList, propertyNames);

    QStringList galleryPropertyNames = gallery.itemTypePropertyNames(itemType);
    propertyNames.sort();
    galleryPropertyNames.sort();

    QCOMPARE(galleryPropertyNames, propertyNames);
}

void tst_QDocumentGallery::propertyAttributes_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QGalleryProperty::Attributes>("propertyAttributes");

    QTest::newRow("Null itemType, propertyName")
            << QString()
            << QString()
            << QGalleryProperty::Attributes();
    QTest::newRow("Null itemType, invalid propertyName")
            << QString()
            << QString::fromLatin1("Goodbye")
            << QGalleryProperty::Attributes();
    QTest::newRow("Null itemType, valid propertyName")
            << QString()
            << QString(QDocumentGallery::fileName)
            << QGalleryProperty::Attributes();
    QTest::newRow("Invalid itemType, invalid propertyName")
            << QString::fromLatin1("Hello")
            << QString::fromLatin1("Goodbye")
            << QGalleryProperty::Attributes();
    QTest::newRow("Invalid itemType, valid propertyName")
            << QString::fromLatin1("Hello")
            << QString(QDocumentGallery::fileName)
            << QGalleryProperty::Attributes();
    QTest::newRow("Valid itemType, invalid propertyName")
            << QString(QDocumentGallery::File)
            << QString::fromLatin1("Goodbye")
            << QGalleryProperty::Attributes();
    QTest::newRow("File.fileName")
            << QString(QDocumentGallery::File)
            << QString(QDocumentGallery::fileName)
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
            << (QGalleryProperty::CanRead
                    | QGalleryProperty::CanFilter
                    | QGalleryProperty::CanSort);
#else
            << QGalleryProperty::Attributes();
#endif
    QTest::newRow("Audio.albumTitle")
            << QString(QDocumentGallery::Audio)
            << QString(QDocumentGallery::albumTitle)
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
            << (QGalleryProperty::CanRead
                    | QGalleryProperty::CanWrite
                    | QGalleryProperty::CanFilter
                    | QGalleryProperty::CanSort);
#else
            << QGalleryProperty::Attributes();
#endif
    QTest::newRow("Album.duration")
            << QString(QDocumentGallery::Album)
            << QString(QDocumentGallery::duration)
#if defined(Q_OS_UNIX) && !defined(QT_NO_DBUS)
            << QGalleryProperty::Attributes(QGalleryProperty::CanRead);
#else
            << QGalleryProperty::Attributes();
#endif
}

void tst_QDocumentGallery::propertyAttributes()
{
    QFETCH(QString, itemType);
    QFETCH(QString, propertyName);
    QFETCH(QGalleryProperty::Attributes, propertyAttributes);

    QCOMPARE(int(gallery.propertyAttributes(propertyName, itemType)), int(propertyAttributes));
}

#include "tst_qdocumentgallery.moc"

QTEST_MAIN(tst_QDocumentGallery)
