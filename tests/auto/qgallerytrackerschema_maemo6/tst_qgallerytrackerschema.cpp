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

#include <maemo6/qgallerytrackerschema_p.h>

#include <qdocumentgallery.h>
#include <maemo6/qgallerytrackerlistcolumn_p.h>
#include <maemo6/qgallerytrackerresultset_p.h>
#include <maemo6/qgallerytrackertyperesultset_p.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QDocumentGallery::Error)
Q_DECLARE_METATYPE(QVariant)
Q_DECLARE_METATYPE(QVector<QVariant>)
Q_DECLARE_METATYPE(QGalleryDBusInterfacePointer)
Q_DECLARE_METATYPE(QGalleryQueryRequest::Scope)
Q_DECLARE_METATYPE(QVector<QVariant::Type>)
Q_DECLARE_METATYPE(QGalleryProperty::Attributes)
Q_DECLARE_METATYPE(QVector<QGalleryProperty::Attributes>)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(QGalleryFilter)

#define QT_FILE_QUERY_ARGUMENTS_COUNT 9
#define QT_FILE_QUERY_SERVICE_POSITION 1
#define QT_FILE_QUERY_STRING_POSITION 5

#define QT_AGGREGATE_QUERY_ARGUMENTS_COUNT 6
#define QT_AGGREGATE_QUERY_SERVICE_POSITION 0
#define QT_AGGREGATE_QUERY_STRING_POSITION 2

class tst_QGalleryTrackerSchema : public QObject, public QGalleryDBusInterfaceFactory
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void fromItemType_data();
    void fromItemType();
    void fromItemId_data();
    void fromItemId();
    void serviceUpdateId_data();
    void serviceUpdateId();
    void supportedPropertyNames_data();
    void supportedPropertyNames();
    void propertyAttributes_data();
    void propertyAttributes();
    void prepareValidTypeResponse_data();
    void prepareValidTypeResponse();
    void prepareInvalidTypeResponse_data();
    void prepareInvalidTypeResponse();
    void prepareValidItemResponse_data();
    void prepareValidItemResponse();
    void prepareInvalidItemResponse_data();
    void prepareInvalidItemResponse();
    void queryResponseRootType_data();
    void queryResponseRootType();
    void queryResponseFilePropertyNames_data();
    void queryResponseFilePropertyNames();
    void queryResponseRootItem_data();
    void queryResponseRootItem();
    void queryResponseFilter_data();
    void queryResponseFilter();
    void queryResponseValueColumnToVariant_data();
    void queryResponseValueColumnToVariant();
    void queryResponseValueColumnToString_data();
    void queryResponseValueColumnToString();
    void queryResponseCompositeColumn_data();
    void queryResponseCompositeColumn();
    void prepareInvalidQueryResponse_data();
    void prepareInvalidQueryResponse();
    void serviceForType_data();
    void serviceForType();

private:
    QGalleryDBusInterfacePointer daemonInterface() { return m_daemonInterface; }
    QGalleryDBusInterfacePointer metaDataInterface() { return m_metaDataInterface; }
    QGalleryDBusInterfacePointer searchInterface() { return m_searchInterface; }
    QGalleryDBusInterfacePointer fileInterface() { return m_fileInterface; }
    QGalleryDBusInterfacePointer statisticsInterface() { return m_statisticsInterface; }
    QGalleryDBusInterfacePointer resourcesClassInterface( const QString &) { return m_resourcesClassInterface; }

    QGalleryDBusInterfacePointer m_daemonInterface;
    QGalleryDBusInterfacePointer m_metaDataInterface;
    QGalleryDBusInterfacePointer m_searchInterface;
    QGalleryDBusInterfacePointer m_fileInterface;
    QGalleryDBusInterfacePointer m_statisticsInterface;
    QGalleryDBusInterfacePointer m_resourcesClassInterface;
};

void tst_QGalleryTrackerSchema::initTestCase()
{
    qRegisterMetaType<QDocumentGallery::Error>();
    qRegisterMetaType<QVariant>();
    qRegisterMetaType<QVector<QVariant> >();
    qRegisterMetaType<QGalleryDBusInterfacePointer>();
    qRegisterMetaType<QGalleryQueryRequest::Scope>();
    qRegisterMetaType<QVector<QVariant::Type> >();
    qRegisterMetaType<QGalleryProperty::Attributes>();
    qRegisterMetaType<QVector<QGalleryProperty::Attributes> >();
    qRegisterMetaType<QVector<int> >();
    qRegisterMetaType<QGalleryFilter>();

    // Initialize dbus interfaces of some sort here.
}

void tst_QGalleryTrackerSchema::fromItemType_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("File")
            << QString::fromLatin1("File")
            << QString::fromLatin1("File")
            << true;
    QTest::newRow("AudioGenre")
            << QString::fromLatin1("AudioGenre")
            << QString::fromLatin1("AudioGenre")
            << true;
    QTest::newRow("Turtle")
            << QString::fromLatin1("Turtle")
            << QString()
            << false;
}

void tst_QGalleryTrackerSchema::fromItemType()
{
    QFETCH(QString, itemType);
    QFETCH(QString, expectedType);
    QFETCH(bool, isValid);

    QGalleryTrackerSchema schema(itemType);

    QCOMPARE(schema.itemType(), expectedType);
    QCOMPARE(schema.isValid(), isValid);
}

void tst_QGalleryTrackerSchema::fromItemId_data()
{
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("File")
            << QString::fromLatin1("file::/path/to/file.ext")
            << QString::fromLatin1("File")
            << true;
    QTest::newRow("AudioGenre")
            << QString::fromLatin1("audioGenre::Rock")
            << QString::fromLatin1("AudioGenre")
            << true;
    QTest::newRow("Turtle")
            << QString::fromLatin1("turtle::its/a/turtle")
            << QString()
            << false;
}

void tst_QGalleryTrackerSchema::fromItemId()
{
    QFETCH(QString, itemId);
    QFETCH(QString, itemType);
    QFETCH(bool, isValid);

    QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromItemId(itemId);

    QCOMPARE(schema.itemType(), itemType);
    QCOMPARE(schema.isValid(), isValid);
}

void tst_QGalleryTrackerSchema::serviceUpdateId_data()
{
    QTest::addColumn<QString>("service");
    QTest::addColumn<int>("updateId");

    QTest::newRow("File")
            << QString::fromLatin1("nfo:FileDataObject")
            << 0x01;
    QTest::newRow("Folder")
            << QString::fromLatin1("nfo:Folder")
            << 0x02;
    QTest::newRow("Document")
            << QString::fromLatin1("nfo:Document")
            << 0x04;
    QTest::newRow("Audio")
            << QString::fromLatin1("nfo:Audio")
            << 0x08;
    QTest::newRow("Image")
            << QString::fromLatin1("nmm:Photo")
            << 0x10;
    QTest::newRow("Video")
            << QString::fromLatin1("nfo:Video")
            << 0x20;
    QTest::newRow("Playlist")
            << QString::fromLatin1("nmm:Playlist")
            << 0x40;
    QTest::newRow("Text")
            << QString::fromLatin1("nfo:TextDocument")
            << 0x80;
    QTest::newRow("Artist")
            << QString::fromLatin1("nmm:Artist")
            << 0x100;
    QTest::newRow("Album")
            << QString::fromLatin1("nmm:MusicAlbum")
            << 0x200;
    QTest::newRow("PhotoAlbum")
            << QString::fromLatin1("nmm:ImageList")
            << 0x400;
    QTest::newRow("Turtles")
            << QString::fromLatin1("Turtles")
            << 0x01;
}

void tst_QGalleryTrackerSchema::serviceUpdateId()
{
    QFETCH(QString, service);
    QFETCH(int, updateId);

    QCOMPARE(QGalleryTrackerSchema::serviceUpdateId(service), updateId);
}

void tst_QGalleryTrackerSchema::supportedPropertyNames_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QStringList>("propertyNames");

    QTest::newRow("File") << QString::fromLatin1("File") << (QStringList()
             << QLatin1String("author")
             << QLatin1String("fileExtension")
             << QLatin1String("fileName")
             << QLatin1String("filePath")
             << QLatin1String("fileSize")
             << QLatin1String("comments")
             << QLatin1String("copyright")
             << QLatin1String("description")
             << QLatin1String("keywords")
             << QLatin1String("language")
             << QLatin1String("lastAccessed")
             << QLatin1String("lastModified")
             << QLatin1String("mimeType")
             << QLatin1String("path")
             << QLatin1String("rating")
             << QLatin1String("subject")
             << QLatin1String("title")
             << QLatin1String("url")
            );

    QTest::newRow("AudioGenre") << QString::fromLatin1("AudioGenre") << (QStringList()
            << QLatin1String("duration")
            << QLatin1String("genre")
            << QLatin1String("title")
            << QLatin1String("trackCount")
            );

    QTest::newRow("Turtle")
            << QString::fromLatin1("Turtle")
            << QStringList();
}

void tst_QGalleryTrackerSchema::supportedPropertyNames()
{
    QFETCH(QString, itemType);
    QFETCH(QStringList, propertyNames);
    propertyNames.sort();

    QGalleryTrackerSchema schema(itemType);

    QStringList supportedPropertyNames = schema.supportedPropertyNames();
    supportedPropertyNames.sort();

    QCOMPARE(supportedPropertyNames, propertyNames);
}

void tst_QGalleryTrackerSchema::propertyAttributes_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QGalleryProperty::Attributes>("attributes");

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
            << QString::fromLatin1("fileName")
            << QGalleryProperty::Attributes();
    QTest::newRow("Invalid itemType, invalid propertyName")
            << QString::fromLatin1("Hello")
            << QString::fromLatin1("Goodbye")
            << QGalleryProperty::Attributes();
    QTest::newRow("Invalid itemType, valid propertyName")
            << QString::fromLatin1("Hello")
            << QString::fromLatin1("fileName")
            << QGalleryProperty::Attributes();
    QTest::newRow("Valid itemType, invalid propertyName")
            << QString::fromLatin1("File")
            << QString::fromLatin1("Goodbye")
            << QGalleryProperty::Attributes();
    QTest::newRow("File.fileName")
            << QString::fromLatin1("File")
            << QString::fromLatin1("fileName")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("File.filePath")
            << QString::fromLatin1("File")
            << QString::fromLatin1("filePath")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter);
    QTest::newRow("File.url")
            << QString::fromLatin1("File")
            << QString::fromLatin1("url")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Audio.albumTitle")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("albumTitle")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.title")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumTitle")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.albumTitle")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumTitle")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.duration")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("duration")
            << QGalleryProperty::Attributes(QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.turtle")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("turtle")
            << QGalleryProperty::Attributes();
}

void tst_QGalleryTrackerSchema::propertyAttributes()
{
    QFETCH(QString, itemType);
    QFETCH(QString, propertyName);
    QFETCH(QGalleryProperty::Attributes, attributes);

    QGalleryTrackerSchema schema(itemType);

    QCOMPARE(schema.propertyAttributes(propertyName), attributes);
}

void tst_QGalleryTrackerSchema::prepareValidTypeResponse_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<int>("updateMask");
    QTest::addColumn<QGalleryDBusInterfacePointer>("queryInterface");
    QTest::addColumn<QString>("queryMethod");
    QTest::addColumn<QVariantList>("queryArguments");

    QTest::newRow("File")
            << "File"
            << 0xFF
            << m_metaDataInterface
            << "Get"
            << QVariantList();

    QTest::newRow("Artist")
            << QString::fromLatin1("Artist")
            << 0x0100
            << m_metaDataInterface
            << "SparqlQuery"
            << (QVariantList() << QLatin1String(
                    "SELECT COUNT(DISTINCT ?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Artist}"
                        "{?y rdf:type nmm:MusicPiece}"
                        "FILTER(nmm:performer(?y)=?x)"
                    "}"));

    QTest::newRow("Album")
            << QString::fromLatin1("Album")
            << 0x0200
            << m_metaDataInterface
            << "Get"
            << QVariantList();

    QTest::newRow("AudioGenre")
            << "AudioGenre"
            << 0x08
            << m_metaDataInterface
            << "SparqlQuery"
            << (QVariantList() << QLatin1String(
                    "SELECT COUNT(DISTINCT nfo:genre(?x)) "
                    "WHERE {"
                        "{?x rdf:type nfo:Audio}"
                        "FILTER(nfo:genre(?x)!='')"
                    "}"));
}

void tst_QGalleryTrackerSchema::prepareValidTypeResponse()
{
    QFETCH(QString, itemType);
    QFETCH(int, updateMask);
    QFETCH(QGalleryDBusInterfacePointer, queryInterface);
    QFETCH(QString, queryMethod);
    QFETCH(QVariantList, queryArguments);

    QGalleryTrackerTypeResultSetArguments arguments;

    QGalleryTrackerSchema schema(itemType);
    QCOMPARE(schema.prepareTypeResponse(&arguments, this), QDocumentGallery::NoError);

    QCOMPARE(arguments.accumulative, false);
    QCOMPARE(arguments.updateMask, updateMask);
    QCOMPARE(arguments.queryInterface, queryInterface);
    QCOMPARE(arguments.queryMethod, queryMethod);
    QCOMPARE(arguments.queryArguments, queryArguments);
}

void tst_QGalleryTrackerSchema::prepareInvalidTypeResponse_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDocumentGallery::Error>("error");

    QTest::newRow("Turtle")
            << QString::fromLatin1("Turtle")
            << QDocumentGallery::ItemTypeError;
}

void tst_QGalleryTrackerSchema::prepareInvalidTypeResponse()
{
    QFETCH(QString, itemType);
    QFETCH(QDocumentGallery::Error, error);

    QGalleryTrackerTypeResultSetArguments arguments;

    QGalleryTrackerSchema schema(itemType);
    QCOMPARE(schema.prepareTypeResponse(&arguments, this), error);
}

void tst_QGalleryTrackerSchema::prepareValidItemResponse_data()
{
    QTest::addColumn<QVariant>("itemId");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QVector<QVariant> >("row");
    QTest::addColumn<QVariant>("itemUrl");
    QTest::addColumn<QVariant>("itemType");
    QTest::addColumn<int>("updateMask");
    QTest::addColumn<int>("identityWidth");
    QTest::addColumn<int>("tableWidth");
    QTest::addColumn<int>("valueOffset");
    QTest::addColumn<int>("compositeOffset");
    QTest::addColumn<QGalleryDBusInterfacePointer>("queryInterface");
    QTest::addColumn<QString>("sparql");

    QTest::newRow("file:://path/to/file.ext")
            << QVariant(QLatin1String("file::uuid:ff172362-d959-99e0-a792-0ddafdd2c559"))
            << QStringList()
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/file.ext")
                    << 0)
            << QVariant(QUrl(QLatin1String("file:///path/to/file.ext")))
            << QVariant(QLatin1String("File"))
            << 0xFF
            << 1
            << 3
            << 3
            << 3
            << m_metaDataInterface
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:FileDataObject}"
                    "FILTER(?x=<uuid:ff172362-d959-99e0-a792-0ddafdd2c559>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("album::album:Greatest Hits")
            << QVariant(QLatin1String("album::album:Greatest Hits"))
            << QStringList()
            << (QVector<QVariant>()
                    << QLatin1String("album:Greatest Hits")
                    << QVariant()
                    << 10)
            << QVariant()
            << QVariant(QLatin1String("Album"))
            << 0x0200
            << 1
            << 1
            << 1
            << 1
            << m_metaDataInterface
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                    "FILTER(?x=<album:Greatest Hits>)"
                "} "
                "GROUP BY ?x";
}

void tst_QGalleryTrackerSchema::prepareValidItemResponse()
{
    QFETCH(QVariant, itemId);
    QFETCH(QStringList, propertyNames);
    QFETCH(QVector<QVariant>, row);
    QFETCH(QVariant, itemUrl);
    QFETCH(QVariant, itemType);
    QFETCH(int, updateMask);
    QFETCH(int, identityWidth);
    QFETCH(int, tableWidth);
    QFETCH(int, valueOffset);
    QFETCH(int, compositeOffset);
    QFETCH(QGalleryDBusInterfacePointer, queryInterface);
    QFETCH(QString, sparql);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromItemId(itemId.toString());
    QCOMPARE(
            schema.prepareItemResponse(&arguments, this, itemId.toString(), propertyNames),
            QDocumentGallery::NoError);

    QVERIFY(arguments.idColumn != 0);
    QCOMPARE(arguments.idColumn->value(row.constBegin()), itemId);

    QVERIFY(arguments.urlColumn != 0);
    QCOMPARE(arguments.urlColumn->value(row.constBegin()), itemUrl);

    QVERIFY(arguments.typeColumn != 0);
    QCOMPARE(arguments.typeColumn->value(row.constBegin()), itemType);

    QCOMPARE(arguments.updateMask, updateMask);
    QCOMPARE(arguments.identityWidth, identityWidth);
    QCOMPARE(arguments.tableWidth, tableWidth);
    QCOMPARE(arguments.valueOffset, valueOffset);
    QCOMPARE(arguments.compositeOffset, compositeOffset);

    QCOMPARE(arguments.queryInterface, queryInterface);
    QCOMPARE(arguments.sparql, sparql);
}

void tst_QGalleryTrackerSchema::prepareInvalidItemResponse_data()
{
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QDocumentGallery::Error>("error");

    QTest::newRow("Invalid Type")
            << QString::fromLatin1("turtle::its/a/turtle")
            << QStringList()
            << QDocumentGallery::ItemIdError;
}

void tst_QGalleryTrackerSchema::prepareInvalidItemResponse()
{
    QFETCH(QString, itemId);
    QFETCH(QStringList, propertyNames);
    QFETCH(QDocumentGallery::Error, error);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromItemId(itemId);
    QCOMPARE(schema.prepareItemResponse(&arguments, this, itemId, propertyNames), error);
}

void tst_QGalleryTrackerSchema::queryResponseRootType_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("sparql");
    QTest::addColumn<int>("updateMask");
    QTest::addColumn<int>("identityWidth");
    QTest::addColumn<QVector<QVariant> >("rowData");
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QVariant>("itemUrl");
    QTest::addColumn<QString>("itemType");

    QTest::newRow("File: Files")
            << "File"
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x"
            << 0xFF
            << 1
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/file.ext")
                    << 0)
            << "file::uuid:ff172362-d959-99e0-a792-0ddafdd2c559"
            << QVariant(QUrl(QLatin1String("file:///path/to/file.ext")))
            << "File";

    QTest::newRow("File: Images")
            << "File"
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x"
            << 0xFF
            << 1
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/image.png")
                    << 4)
            << "image::uuid:ff172362-d959-99e0-a792-0ddafdd2c559"
            << QVariant(QUrl(QLatin1String("file:///path/to/image.png")))
            << "Image";

    QTest::newRow("Text: Text")
            << "Text"
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {{?x rdf:type nfo:TextDocument}} "
                "GROUP BY ?x"
            << 0x80
            << 1
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/text.txt")
                    << 7)
            << "text::uuid:ff172362-d959-99e0-a792-0ddafdd2c559"
            << QVariant(QUrl(QLatin1String("file:///path/to/text.txt")))
            << "Text";

    QTest::newRow("Artist")
            << "Artist"
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:Artist}"
                    "{?y rdf:type nmm:MusicPiece}"
                    "FILTER(nmm:performer(?y)=?x)"
                "} "
                "GROUP BY ?x"
            << 0x0100
            << 1
            << (QVector<QVariant>()
                    << QLatin1String("artist:Self%20Titled")
                    << QString()
                    << 8)
            << QString::fromLatin1("artist::artist:Self%20Titled")
            << QVariant()
            << "Artist";

    QTest::newRow("AlbumArtist")
            << "AlbumArtist"
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:Artist}"
                    "{?y rdf:type nmm:MusicAlbum}"
                    "FILTER(nmm:albumArtist(?y)=?x)"
                "} "
                "GROUP BY ?x"
            << 0x0100
            << 1
            << (QVector<QVariant>()
                    << QLatin1String("artist:Self%20Titled")
                    << QString()
                    << 9)
            << QString::fromLatin1("albumArtist::artist:Self%20Titled")
            << QVariant()
            << "AlbumArtist";

    QTest::newRow("Album")
            << "Album"
            <<  "SELECT ?x "
                "WHERE {{?x rdf:type nmm:MusicAlbum}} "
                "GROUP BY ?x"
            << 0x0200
            << 1
            << (QVector<QVariant>()
                    << QLatin1String("musicAlbum:Greatest%20Hits")
                    << QString()
                    << 9)
            << QString::fromLatin1("album::musicAlbum:Greatest%20Hits")
            << QVariant()
            << "Album";
}

void tst_QGalleryTrackerSchema::queryResponseRootType()
{
    QFETCH(QString, rootType);
    QFETCH(QString, sparql);
    QFETCH(int, updateMask);
    QFETCH(int, identityWidth);
    QFETCH(QVector<QVariant>, rowData);
    QFETCH(QString, itemId);
    QFETCH(QVariant, itemUrl);
    QFETCH(QString, itemType);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    QGalleryQueryRequest::AllDescendants,
                    QString(),
                    QGalleryFilter(),
                    QStringList(),
                    QStringList(),
                    0,
                    0),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.queryInterface, m_metaDataInterface);
    QCOMPARE(arguments.sparql, sparql);

    QCOMPARE(arguments.updateMask, updateMask);
    QCOMPARE(arguments.identityWidth, identityWidth);

    QVERIFY(arguments.idColumn != 0);
    QCOMPARE(arguments.idColumn->value(rowData.constBegin()), QVariant(itemId));

    QVERIFY(arguments.urlColumn != 0);
    QCOMPARE(arguments.urlColumn->value(rowData.constBegin()), itemUrl);

    QVERIFY(arguments.typeColumn != 0);
    QCOMPARE(arguments.typeColumn->value(rowData.constBegin()), QVariant(itemType));
}

void tst_QGalleryTrackerSchema::queryResponseFilePropertyNames_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QStringList>("sortPropertyNames");
    QTest::addColumn<int>("tableWidth");
    QTest::addColumn<int>("compositeOffset");
    QTest::addColumn<QString>("sparql");
    QTest::addColumn<QStringList>("fieldNames");
    QTest::addColumn<QStringList>("filteredPropertyNames");
    QTest::addColumn<QVector<int> >("aliasColumns");
    QTest::addColumn<QVector<int> >("resourceKeys");

    QTest::newRow("File: [fileName, mimeType], []")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QStringList() // sortPropertyNames
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [turtle, mimeType], []")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("turtle")
                    << QLatin1String("mimeType"))
            << QStringList() // sortPropertyNames
            << 4 // tableWidth
            << 4 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x"
            << (QStringList() // fieldNames
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [fileName, mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nfo:fileName(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [fileName, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nfo:fileName(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-fileName, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("+mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nfo:fileName(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-fileName, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nfo:fileName(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName], [fileName, mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nfo:fileName(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << QVector<int>(); // resourceKeys

    QTest::newRow("File: [fileName], [fileName, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nfo:fileName(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                << QLatin1String("nfo:fileName"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << QVector<int>(); // resourceKeys

    QTest::newRow("File: [fileName], [-fileName, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nfo:fileName(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                << QLatin1String("nfo:fileName"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << QVector<int>(); // resourceKeys

    QTest::newRow("File: [fileName], [-fileName, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nfo:fileName(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                << QLatin1String("nfo:fileName"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << QVector<int>(); // resourceKeys

    // turtle is an non-existent property.
    QTest::newRow("File: [fileName, mimeType], [+turtle, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+turtle")
                    << QLatin1String("+mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-turtle, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-turtle")
                    << QLatin1String("+mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [+turtle, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+turtle")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-turtle, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-turtle")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [+url, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+url")
                    << QLatin1String("+mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nie:url(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-url, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-url")
                    << QLatin1String("+mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nie:url(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [+url, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+url")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nie:url(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-url, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-url")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nie:url(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [+keywords, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+keywords")
                    << QLatin1String("+mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nie:keyword(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-keywords, +mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-keywords")
                    << QLatin1String("+mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nie:keyword(?x)) ASC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                << QLatin1String("nfo:fileName")
                << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [+keywords, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+keywords")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY ASC(nie:keyword(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                << QLatin1String("nfo:fileName")
                << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);

    QTest::newRow("File: [fileName, mimeType], [-keywords, -mimeType]")
            << "File" // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-keywords")
                    << QLatin1String("-mimeType"))
            << 5 // tableWidth
            << 5 // compositeOffset
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) nfo:fileName(?x) nie:mimeType(?x) "
                "WHERE {{?x rdf:type nfo:FileDataObject}} "
                "GROUP BY ?x "
                "ORDER BY DESC(nie:keyword(?x)) DESC(nie:mimeType(?x))"
            << (QStringList() // fieldNames
                    << QLatin1String("nfo:fileName")
                    << QLatin1String("nie:mimeType"))
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<int>() // resourceKeys
                    << 4);
}

void tst_QGalleryTrackerSchema::queryResponseFilePropertyNames()
{
    QFETCH(QString, rootType);
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, sortPropertyNames);
    QFETCH(int, tableWidth);
    QFETCH(int, compositeOffset);
    QFETCH(QString, sparql);
    QFETCH(QStringList, filteredPropertyNames);
    QFETCH(QStringList, fieldNames);
    QFETCH(QVector<int>, aliasColumns);
    QFETCH(QVector<int>, resourceKeys);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    QGalleryQueryRequest::AllDescendants,
                    QString(),
                    QGalleryFilter(),
                    propertyNames,
                    sortPropertyNames,
                    0,
                    0),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.tableWidth, tableWidth);
    QCOMPARE(arguments.valueOffset, 3);
    QCOMPARE(arguments.compositeOffset, compositeOffset);

    QCOMPARE(arguments.sparql, sparql);

    QCOMPARE(arguments.propertyNames, filteredPropertyNames);
    QCOMPARE(arguments.fieldNames, fieldNames);

    QCOMPARE(arguments.aliasColumns, aliasColumns);
    QCOMPARE(arguments.resourceKeys, resourceKeys);
}

void tst_QGalleryTrackerSchema::queryResponseRootItem_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<QString>("sparql");

    QTest::newRow("Folder, All File Descendants")
            << QString::fromLatin1("File")
            << QString::fromLatin1("folder::uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:FileDataObject}"
                    "FILTER(nie:url(?x) > fn:concat(nie:url(<uuid:ff172362-d959-99e0-a792-0ddafdd2c559>),'/') "
                        "&& nie:url(?x) < fn:concat(nie:url(<uuid:ff172362-d959-99e0-a792-0ddafdd2c559>),'0'))"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Folder, Direct File Descendants")
            << QString::fromLatin1("File")
            << QString::fromLatin1("folder::uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:FileDataObject}"
                    "FILTER(nfo:belongsToContainer(?x)=<uuid:ff172362-d959-99e0-a792-0ddafdd2c559>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Album, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("album::musicAlbum:Greatest%20Hits")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nmm:musicAlbum(?x)=<musicAlbum:Greatest%20Hits>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Album, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("album::musicAlbum:Greatest%20Hits")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nmm:musicAlbum(?x)=<musicAlbum:Greatest%20Hits>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Album Artist, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("albumArtist::artist:Self%20Titled")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nmm:albumArtist(nmm:musicAlbum(?x))=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Album Artist, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("albumArtist::artist:Self%20Titled")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nmm:albumArtist(nmm:musicAlbum(?x))=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Album Artist, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumArtist::artist:Self%20Titled")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                    "FILTER(nmm:albumArtist(?x)=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Album Artist, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumArtist::artist:Self%20Titled")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                    "FILTER(nmm:albumArtist(?x)=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Artist, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("artist::artist:Self%20Titled")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nmm:performer(?x)=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Artist, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("artist::artist:Self%20Titled")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nmm:performer(?x)=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Artist, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("artist::artist:Self%20Titled")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                    "{?track nie:isLogicalPartOf ?x}"
                    "FILTER(nmm:performer(?track)=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Artist, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("artist::artist:Self%20Titled")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                    "{?track nie:isLogicalPartOf ?x}"
                    "FILTER(nmm:performer(?track)=<artist:Self%20Titled>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nfo:genre(?x)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nfo:Audio}"
                    "FILTER(nfo:genre(?x)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                    "{?track nie:isLogicalPartOf ?x}"
                    "FILTER(nfo:genre(?track)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                    "{?track nie:isLogicalPartOf ?x}"
                    "FILTER(nfo:genre(?track)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, All Artist Descendants")
            << QString::fromLatin1("Artist")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:Artist}"
                    "{?y rdf:type nmm:MusicPiece}"
                    "FILTER(nmm:performer(?y)=?x && nfo:genre(?y)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, Direct Artist Descendants")
            << QString::fromLatin1("Artist")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:Artist}"
                    "{?y rdf:type nmm:MusicPiece}"
                    "FILTER(nmm:performer(?y)=?x && nfo:genre(?y)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, All AlbumArtist Descendants")
            << QString::fromLatin1("AlbumArtist")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:Artist}"
                    "{?y rdf:type nmm:MusicAlbum}"
                    "{?track nie:isLogicalPartOf ?y}"
                    "FILTER(nmm:albumArtist(?y)=?x && nfo:genre(?track)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Audio Genre, Direct AlbumArtist Descendants")
            << QString::fromLatin1("AlbumArtist")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:Artist}"
                    "{?y rdf:type nmm:MusicAlbum}"
                    "{?track nie:isLogicalPartOf ?y}"
                    "FILTER(nmm:albumArtist(?y)=?x && nfo:genre(?track)='Rock')"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Photo Album, All Image Descendants")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("photoAlbum::photoAlbum:Camping")
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nmm:Photo}"
                    "FILTER(nie:isLogicalPartOf(?x)=<photoAlbum:Camping>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("Photo Album, Direct Image Descendants")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("photoAlbum::photoAlbum:Camping")
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nmm:Photo}"
                    "FILTER(nie:isLogicalPartOf(?x)=<photoAlbum:Camping>)"
                "} "
                "GROUP BY ?x";

    QTest::newRow("No Root Item, All Image Descendants")
            << QString::fromLatin1("Image")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                "WHERE {"
                    "{?x rdf:type nmm:Photo}"
                "} "
                "GROUP BY ?x";

    QTest::newRow("No Root Item, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                "} "
                "GROUP BY ?x";

    QTest::newRow("No Root Item, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString()
            << QGalleryQueryRequest::DirectDescendants
            <<  "SELECT ?x "
                "WHERE {"
                    "{?x rdf:type nmm:MusicAlbum}"
                "} "
                "GROUP BY ?x";
}

void tst_QGalleryTrackerSchema::queryResponseRootItem()
{
    QFETCH(QString, rootType);
    QFETCH(QString, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(QString, sparql);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    scope,
                    rootItem,
                    QGalleryFilter(),
                    QStringList(),
                    QStringList(),
                    0,
                    0),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.sparql, sparql);
}

void tst_QGalleryTrackerSchema::queryResponseFilter_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<QGalleryFilter>("filter");
    QTest::addColumn<QString>("sparql");

    {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl::fromLocalFile(QLatin1String("/path/to/file.ext"));

        QTest::newRow("File.url == file:///path/to/file.ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)='file:///path/to/file.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl::fromLocalFile(QLatin1String("/"));

        QTest::newRow("File.url == file:///")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)='file:///'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl(QLatin1String("http://example.com"));

        QTest::newRow("File.url == http://example.com")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)='http://example.com'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl(QLatin1String("http://example.com/index.html"));

        QTest::newRow("File.url == http://example.com/index.html")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)='http://example.com/index.html'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::filePath == QLatin1String("/path/to/file.ext");

        QTest::newRow("File.filePath == /path/to/file.ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)='file:///path/to/file.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::filePath > QLatin1String("/path/to/file.ext");

        QTest::newRow("File.filePath > /path/to/file.ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)>'file:///path/to/file.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::filePath >= QLatin1String("/path/to/file.ext");

        QTest::newRow("File.filePath >= /path/to/file.ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)>='file:///path/to/file.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::filePath < QLatin1String("/path/to/file.ext");

        QTest::newRow("File.filePath < /path/to/file.ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)<'file:///path/to/file.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::filePath <= QLatin1String("/path/to/file.ext");

        QTest::newRow("File.filePath <= /path/to/file.ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:url(?x)<='file:///path/to/file.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::path.startsWith(QLatin1String("/path/"));

        QTest::newRow("File.path.startsWith(/path/)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:starts-with(nie:url(nfo:belongsToContainer(?x)),'file:///path/'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::path.endsWith(QLatin1String("/to"));

        QTest::newRow("File.path.endsWith(/to)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:ends-with(nie:url(nfo:belongsToContainer(?x)),'/to'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::path.contains(QLatin1String("path"));

        QTest::newRow("File.path.contains(path)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:contains(nie:url(nfo:belongsToContainer(?x)),'path'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::path.wildcard(QLatin1String("/*/to"));

        QTest::newRow("File.path.wildcard(/*/to)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:contains(nie:url(nfo:belongsToContainer(?x)),'file:///*/to'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::fileExtension == QLatin1String("ext");

        QTest::newRow("File.fileExtension == ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:ends-with(nfo:fileName(?x),'.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::fileName == QLatin1String("file.ext");

        QTest::newRow("File.fileName == file.ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nfo:fileName(?x)='file.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.startsWith(QLatin1String("file."));

        QTest::newRow("File.fileName.startsWith(file.)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:starts-with(nfo:fileName(?x),'file.'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.endsWith(QLatin1String(".ext"));

        QTest::newRow("File.fileName.endsWith(.ext)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:ends-with(nfo:fileName(?x),'.ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.contains(QLatin1String("ext"));

        QTest::newRow("File.fileName.contains(ext)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:contains(nfo:fileName(?x),'ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.wildcard(QLatin1String("file*ext"));

        QTest::newRow("File.fileName.wildcard(file*ext")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(fn:contains(nfo:fileName(?x),'file*ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter
                = QDocumentGallery::fileName.regExp(QLatin1String("(file|document).ext"));

        QTest::newRow("File.fileName.regExp((file|document).ext)")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(REGEX(nfo:fileName(?x),'(file|document).ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter
                = QDocumentGallery::fileName.regExp(QRegExp(QLatin1String("(file|document).ext")));

        QTest::newRow("File.fileName.regExp(QRegExp((file|document).ext))")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(REGEX(nfo:fileName(?x),'(file|document).ext'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter
                = QDocumentGallery::description == QUrl(QLatin1String("http://example.com/index.html"));

        QTest::newRow("File.description == http://example.com/index.html")
                << "File"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER((nie:description(?x)='http://example.com/index.html'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::width > 1024;

        QTest::newRow("Image.width > 1024")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER((nfo:width(?x)>'1024'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::width >= 1024u;

        QTest::newRow("Video.width >= 1024u")
                << "Video"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:Video}"
                        "FILTER((nfo:width(?x)>='1024'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::height < Q_INT64_C(1024);

        QTest::newRow("Image.height < 1024ll")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER((nfo:height(?x)<'1024'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::height <= Q_UINT64_C(1024);

        QTest::newRow("Video.height <= 1024ull")
                << "Video"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:Video}"
                        "FILTER((nfo:height(?x)<='1024'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::focalLength <= 1.9;

        QTest::newRow("Image.focalLength <= 1.9")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER((nmm:focalLength(?x)<='1.9'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::focalLength < 0.25f;

        QTest::newRow("Image.focalLength <= 0.25f")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER((nmm:focalLength(?x)<'0.25'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter
                = QDocumentGallery::lastModified > QDateTime(QDate(2008, 06, 01), QTime(12, 5, 8));

        QTest::newRow("Image.lastModified > 2008-06-01T12:05:08")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER((nfo:fileLastModified(?x)>'2008-06-01T12:05:08'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = !(
                QDocumentGallery::lastModified > QDateTime(QDate(2008, 06, 01), QTime(12, 5, 8)));

        QTest::newRow("!(Image.lastModified > 2008-06-01T12:05:08")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER(!(nfo:fileLastModified(?x)>'2008-06-01T12:05:08'))"
                    "} "
                    "GROUP BY ?x";

    } {
        QGalleryIntersectionFilter filter;
        filter.append(QDocumentGallery::width > 1024);
        filter.append(QDocumentGallery::height > 768);

        QTest::newRow("Image.width > 1024 && Image.height > 768")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER(((nfo:width(?x)>'1024')&&(nfo:height(?x)>'768')))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryIntersectionFilter filter;
        filter.append(QDocumentGallery::width > 1024);

        QTest::newRow("Image.width > 1024 (Single intersection item)")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER(((nfo:width(?x)>'1024')))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryUnionFilter filter;
        filter.append(QDocumentGallery::width < 1920);
        filter.append(QDocumentGallery::height < 1024);

        QTest::newRow("Image.width < 1920 || Image.height < 1024")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER(((nfo:width(?x)<'1920')||(nfo:height(?x)<'1024')))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryUnionFilter filter;
        filter.append(QDocumentGallery::width < 1920);

        QTest::newRow("Image.width < 1920 (Single union item")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                        "FILTER(((nfo:width(?x)<'1920')))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryUnionFilter filter;

        QTest::newRow("Image (Empty union filter)")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryIntersectionFilter filter;

        QTest::newRow("Image (Empty intersection filter)")
                << "Image"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nmm:Photo}"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::fileName == QLatin1String("file.ext");

        QTest::newRow("File.fileName == file.ext, In folder")
                << "File"
                << "folder::uuid:ff172362-d959-99e0-a792-0ddafdd2c559"
                << QGalleryQueryRequest::DirectDescendants
                << filter
                <<  "SELECT ?x nie:url(?x) rdf:type(?x) "
                    "WHERE {"
                        "{?x rdf:type nfo:FileDataObject}"
                        "FILTER(nfo:belongsToContainer(?x)=<uuid:ff172362-d959-99e0-a792-0ddafdd2c559> "
                            "&& (nfo:fileName(?x)='file.ext'))"
                        "} "
                        "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits (All)")
                << "Album"
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x "
                    "WHERE {"
                        "{?x rdf:type nmm:MusicAlbum}"
                        "FILTER((nmm:albumTitle(?x)='Greatest Hits'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits (Direct)")
                << "Album"
                << QString()
                << QGalleryQueryRequest::DirectDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x "
                    "WHERE {"
                        "{?x rdf:type nmm:MusicAlbum}"
                        "FILTER((nmm:albumTitle(?x)='Greatest Hits'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits, Belonging to AlbumArtist (All)")
                << "Album"
                << "albumArtist::artist:Self%20Titled"
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x "
                    "WHERE {"
                        "{?x rdf:type nmm:MusicAlbum}"
                        "FILTER(nmm:albumArtist(?x)=<artist:Self%20Titled> "
                            "&& (nmm:albumTitle(?x)='Greatest Hits'))"
                    "} "
                    "GROUP BY ?x";
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits, Belonging to AlbumArtist (Direct)")
                << "Album"
                << "albumArtist::artist:Self%20Titled"
                << QGalleryQueryRequest::DirectDescendants
                << QGalleryFilter(filter)
                <<  "SELECT ?x "
                    "WHERE {"
                        "{?x rdf:type nmm:MusicAlbum}"
                        "FILTER(nmm:albumArtist(?x)=<artist:Self%20Titled> "
                            "&& (nmm:albumTitle(?x)='Greatest Hits'))"
                    "} "
                    "GROUP BY ?x";
    }
}

void tst_QGalleryTrackerSchema::queryResponseFilter()
{
    QFETCH(QString, rootType);
    QFETCH(QString, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(QGalleryFilter, filter);
    QFETCH(QString, sparql);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    scope,
                    rootItem,
                    filter,
                    QStringList(),
                    QStringList(),
                    0,
                    0),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.sparql, sparql);
}

void tst_QGalleryTrackerSchema::queryResponseValueColumnToVariant_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QString>("string");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("File.fileName")
            << "File"
            << "fileName"
            << "file.ext"
            << QVariant(QLatin1String("file.ext"));

    QTest::newRow("File.fileName (Empty)")
            << "File"
            << "fileName"
            << QString()
            << QVariant(QString());

    QTest::newRow("File.keywords (1)")
            << "File"
            << "keywords"
            << "Holiday"
            << QVariant(QStringList() << QLatin1String("Holiday"));

    QTest::newRow("File.keywords (2)")
            << "File"
            << "keywords"
            << "Holiday|Summer"
            << QVariant(QStringList() << QLatin1String("Holiday") << QLatin1String("Summer"));

    QTest::newRow("File.keywords (3)")
            << "File"
            << "keywords"
            << "2009|Holiday|Summer"
            << QVariant(QStringList()
                    << QLatin1String("2009")
                    << QLatin1String("Holiday")
                    << QLatin1String("Summer"));

    QTest::newRow("File.keywords (Empty")
            << "File"
            << "keywords"
            << QString()
            << QVariant(QStringList());

    QTest::newRow("Image.lastModified")
            << "Image"
            << "lastModified"
            << "2009-08-23T09:03:32"
            << QVariant(QDateTime(QDate(2009, 8, 23), QTime(9, 3, 32)));

    QTest::newRow("Image.lastModfied (Invalid)")
            << "Image"
            << "lastModified"
            << "Yesterday"
            << QVariant();

    QTest::newRow("Image.dateTaken (Empty)")
            << "Image"
            << "lastModified"
            << QString()
            << QVariant();

    QTest::newRow("Image.focalLength (12)")
            << "Image"
            << "focalLength"
            << "12"
            << QVariant(12.0);

    QTest::newRow("Image.focalLength (12.5)")
            << "Image"
            << "focalLength"
            << "12.5"
            << QVariant(12.5);

    QTest::newRow("Image.focalLength (Invalid)")
            << "Image"
            << "focalLength"
            << "twelve point 5"
            << QVariant();

    QTest::newRow("Image.focalLength (Empty")
            << "Image"
            << "focalLength"
            << QString()
            << QVariant();

    QTest::newRow("Image.height")
            << "Image"
            << "height"
            << "720"
            << QVariant(720);

    QTest::newRow("Image.height (Invalid")
            << "Image"
            << "height"
            << "seven twenty"
            << QVariant();

    QTest::newRow("Image.height (Empty)")
            << "Image"
            << "height"
            << QString()
            << QVariant();
}

void tst_QGalleryTrackerSchema::queryResponseValueColumnToVariant()
{
    QFETCH(QString, rootType);
    QFETCH(QString, propertyName);
    QFETCH(QString, string);
    QFETCH(QVariant, value);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    QGalleryQueryRequest::AllDescendants,
                    QString(),
                    QGalleryFilter(),
                    QStringList() << propertyName,
                    QStringList(),
                    0,
                    0),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.valueColumns.count(), 4);
    QCOMPARE(arguments.valueColumns.at(3)->toVariant(string), value);
}

void tst_QGalleryTrackerSchema::queryResponseValueColumnToString_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("string");

    QTest::newRow("File.fileName")
            << "File"
            << "fileName"
            << QVariant(QLatin1String("file.ext"))
            << "file.ext";

    QTest::newRow("File.fileName (Empty)")
            << "File"
            << "fileName"
            << QVariant(QString())
            << QString();

    QTest::newRow("File.keywords (1)")
            << "File"
            << "keywords"
            << QVariant(QStringList() << QLatin1String("Holiday"))
            << "Holiday";

    QTest::newRow("File.keywords (2)")
            << "File"
            << "keywords"
            << QVariant(QStringList() << QLatin1String("Holiday") << QLatin1String("Summer"))
            << "Holiday|Summer";

    QTest::newRow("File.keywords (3)")
            << "File"
            << "keywords"
            << QVariant(QStringList()
                    << QLatin1String("2009")
                    << QLatin1String("Holiday")
                    << QLatin1String("Summer"))
            << "2009|Holiday|Summer";

    QTest::newRow("File.keywords (QString)")
            << "File"
            << "keywords"
            << QVariant(QLatin1String("Holiday"))
            << "Holiday";

    QTest::newRow("File.keywords (Empty")
            << "File"
            << "keywords"
            << QVariant(QStringList())
            << QString();

    QTest::newRow("Image.lastModified")
            << "Image"
            << "lastModified"
            << QVariant(QDateTime(QDate(2009, 8, 23), QTime(9, 3, 32)))
            << "2009-08-23T09:03:32";

    QTest::newRow("Image.lastModified (Invalid)")
            << "Image"
            << "lastModified"
            << QVariant(QDateTime(QDate(2009, 8, 56), QTime(95, 3, 32)))
            << QString();

    QTest::newRow("Image.lastModified (Empty)")
            << "Image"
            << "lastModified"
            << QVariant()
            << QString();

    QTest::newRow("Image.focalLength (12)")
            << "Image"
            << "focalLength"
            << QVariant(12.0)
            << "12";

    QTest::newRow("Image.focalLength (12.5)")
            << "Image"
            << "focalLength"
            << QVariant(12.5)
            << "12.5";

    QTest::newRow("Image.focalLength (Empty")
            << "Image"
            << "focalLength"
            << QVariant()
            << QString();

    QTest::newRow("Image.height")
            << "Image"
            << "height"
            << QVariant(720)
            << "720";

    QTest::newRow("Image.height (Empty)")
            << "Image"
            << "height"
            << QVariant()
            << QString();
}

void tst_QGalleryTrackerSchema::queryResponseValueColumnToString()
{
    QFETCH(QString, rootType);
    QFETCH(QString, propertyName);
    QFETCH(QVariant, value);
    QFETCH(QString, string);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    QGalleryQueryRequest::AllDescendants,
                    QString(),
                    QGalleryFilter(),
                    QStringList() << propertyName,
                    QStringList(),
                    0,
                    0),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.valueColumns.count(), 4);
    QCOMPARE(arguments.valueColumns.at(3)->toString(value), string);
}

void tst_QGalleryTrackerSchema::queryResponseCompositeColumn_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVector<QVariant> >("rowData");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("File.filePath")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("filePath")
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/file.ext")
                    << QLatin1String("Files"))
            << QVariant(QLatin1String("/path/to/file.ext"));

    QTest::newRow("File.path")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("path")
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/file.ext")
                    << QLatin1String("Files"))
            << QVariant(QLatin1String("/path/to"));

    QTest::newRow("File.path (empty fileName)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("path")
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/")
                    << QLatin1String("Files"))
            << QVariant(QLatin1String("/path/to"));

    QTest::newRow("File.fileExtension")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("fileExtension")
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to/file.ext")
                    << QLatin1String("Files"))
            << QVariant(QLatin1String("ext"));


    QTest::newRow("File.fileExtension (no extension)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("fileExtension")
            << (QVector<QVariant>()
                    << QLatin1String("uuid:ff172362-d959-99e0-a792-0ddafdd2c559")
                    << QLatin1String("file:///path/to")
                    << QLatin1String("Files"))
            << QVariant();
}

void tst_QGalleryTrackerSchema::queryResponseCompositeColumn()
{
    QFETCH(QString, rootType);
    QFETCH(QString, propertyName);
    QFETCH(QVector<QVariant>, rowData);
    QFETCH(QVariant, value);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    QGalleryQueryRequest::AllDescendants,
                    QString(),
                    QGalleryFilter(),
                    QStringList() << propertyName,
                    QStringList(),
                    0,
                    0),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.compositeColumns.count(), 1);
    QCOMPARE(arguments.compositeColumns.at(0)->value(rowData.constBegin()), value);
}

void tst_QGalleryTrackerSchema::prepareInvalidQueryResponse_data()
{
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QGalleryFilter>("filter");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QStringList>("sortPropertyNames");
    QTest::addColumn<QDocumentGallery::Error>("error");

    QTest::addColumn<QVariant>("itemId");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QDocumentGallery::Error>("error");

    QTest::newRow("Invalid Type, No Filter")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "Turtle"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemTypeError;

    QTest::newRow("Invalid Type, With Filter")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "Turtle"
            << QGalleryFilter(QDocumentGallery::fileName == QLatin1String("file.ext"))
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemTypeError;

    QTest::newRow("No Type")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString()
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemTypeError;

    QTest::newRow("Invalid Root Item Type")
            << "turtle::urn:turtle:/its/a/turtle"
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("File.filePath.regExp(/path)")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QDocumentGallery::filePath.regExp(QLatin1String("/path")))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.filePath.regExp(/path) (within union)")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QGalleryUnionFilter(
                    QDocumentGallery::filePath.regExp(QLatin1String("/path"))))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.filePath.regExp(/path) (within intersection)")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QGalleryIntersectionFilter(
                    QDocumentGallery::filePath.regExp(QLatin1String("/path"))))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.filePath ? /path")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QGalleryMetaDataFilter(
                    QLatin1String("filePath"),
                    QLatin1String("file.ext"),
                    QGalleryFilter::Comparator(1200)))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.filePath == QPoint(12, 44)")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QDocumentGallery::filePath == QPoint(12, 44))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.fileName ? file.ext")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QGalleryMetaDataFilter(
                    QLatin1String("fileName"),
                    QLatin1String("file.ext"),
                    QGalleryFilter::Comparator(1200)))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.fileName == QPoint(12, 44)")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QDocumentGallery::fileName == QPoint(12, 44))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.fileExtension > ext")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QDocumentGallery::fileExtension > QLatin1String("ext"))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.fileExtension == 13")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter(QDocumentGallery::fileExtension == 13)
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File, File Descendants")
            << "file::uuid:ff172362-d959-99e0-a792-0ddafdd2c559"
            << QGalleryQueryRequest::AllDescendants
            << "File"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;


    QTest::newRow("Folder, All Descendants")
            << "folder::uuid:ff172362-d959-99e0-a792-0ddafdd2c559"
            << QGalleryQueryRequest::AllDescendants
            << "Album"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("Album, Image Descendants")
            << "album::musicAlbum:Greatest%20Hits"
            << QGalleryQueryRequest::AllDescendants
            << "Image"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("Album Artist, Image Descendants")
            << "albumArtist::artist:Self%20Titled"
            << QGalleryQueryRequest::AllDescendants
            << "Image"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("Album Artist, Image Descendants")
            << "artist::artist:Self%20Titled"
            << QGalleryQueryRequest::AllDescendants
            << "Image"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("Album Artist, Image Descendants")
            << "audioGenre::Rock"
            << QGalleryQueryRequest::AllDescendants
            << "Image"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("PhotoAlbum, Audio Descendants")
            << "photoAlbum::photoAlbum:Camping"
            << QGalleryQueryRequest::AllDescendants
            << "Audio"
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;
}

void tst_QGalleryTrackerSchema::prepareInvalidQueryResponse()
{
    QFETCH(QString, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(QString, rootType);
    QFETCH(QGalleryFilter, filter);
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, sortPropertyNames);
    QFETCH(QDocumentGallery::Error, error);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments, this, scope, rootItem, filter, propertyNames, sortPropertyNames, 0, 0),
            error);
}

void tst_QGalleryTrackerSchema::serviceForType_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QString>("expectedService");

    QTest::newRow("File")
            << QString::fromLatin1("File")
            << QString::fromLatin1("nfo:FileDataObject");
    QTest::newRow("Folder")
            << QString::fromLatin1("Folder")
            << QString::fromLatin1("nfo:Folder");
    QTest::newRow("Document")
            << QString::fromLatin1("Document")
            << QString::fromLatin1("nfo:Document");
    QTest::newRow("Audio")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("nfo:Audio");
    QTest::newRow("Image")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("nmm:Photo");
    QTest::newRow("Video")
            << QString::fromLatin1("Video")
            << QString::fromLatin1("nfo:Video");
    QTest::newRow("Playlist")
            << QString::fromLatin1("Playlist")
            << QString::fromLatin1("nmm:Playlist");
    QTest::newRow("Text")
            << QString::fromLatin1("Text")
            << QString::fromLatin1("nfo:TextDocument");
    QTest::newRow("Artist")
            << QString::fromLatin1("Artist")
            << QString::fromLatin1("nmm:Artist");
    QTest::newRow("Album")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("nmm:MusicAlbum");
    QTest::newRow("PhotoAlbum")
            << QString::fromLatin1("PhotoAlbum")
            << QString::fromLatin1("nmm:ImageList");
}

void tst_QGalleryTrackerSchema::serviceForType()
{
    QFETCH(QString, itemType);
    QFETCH(QString, expectedService);

    QCOMPARE(
            QGalleryTrackerSchema::serviceForType( itemType),
            expectedService);
}

QTEST_MAIN(tst_QGalleryTrackerSchema)

#include "tst_qgallerytrackerschema.moc"

