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
Q_DECLARE_METATYPE(QVector<QGalleryTrackerSortCriteria>)

#define QT_FILE_QUERY_ARGUMENTS_COUNT 9
#define QT_FILE_QUERY_SERVICE_POSITION 1
#define QT_FILE_QUERY_STRING_POSITION 5

#define QT_AGGREGATE_QUERY_ARGUMENTS_COUNT 6
#define QT_AGGREGATE_QUERY_SERVICE_POSITION 0
#define QT_AGGREGATE_QUERY_STRING_POSITION 2


bool operator ==(const QGalleryTrackerSortCriteria &left, const QGalleryTrackerSortCriteria &right)
{
    return left.column == right.column && left.flags == right.flags;
}

class tst_QGalleryTrackerSchema : public QObject, public QGalleryDBusInterfaceFactory
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void fromItemType_data();
    void fromItemType();
    void fromItemId_data();
    void fromItemId();
//    void uriFromItemId_data();
//    void uriFromItemId();
    void serviceUpdateId_data();
    void serviceUpdateId();
    void supportedPropertyNames_data();
    void supportedPropertyNames();
#if 0
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
    void queryResponseAggregatePropertyNames_data();
    void queryResponseAggregatePropertyNames();
    void queryResponseRootItem_data();
    void queryResponseRootItem();
    void queryResponseFilter_data();
    void queryResponseFilter();
    void queryResponseRootFileItems();
    void queryResponseRootFileItemsWithFilter();
    void queryResponseValueColumnToVariant_data();
    void queryResponseValueColumnToVariant();
    void queryResponseValueColumnToString_data();
    void queryResponseValueColumnToString();
    void queryResponseCompositeColumn_data();
    void queryResponseCompositeColumn();
    void prepareInvalidQueryResponse_data();
    void prepareInvalidQueryResponse();
#endif
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
    qRegisterMetaType<QVector<QGalleryTrackerSortCriteria> >();

    // Initialize dbus interfaces of some sort here.
}

void tst_QGalleryTrackerSchema::fromItemType_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QString>("expectedType");
    QTest::addColumn<bool>("isItemType");
    QTest::addColumn<bool>("isAggregateType");

    QTest::newRow("File")
            << QString::fromLatin1("File")
            << QString::fromLatin1("File")
            << true
            << false;
    QTest::newRow("AudioGenre")
            << QString::fromLatin1("AudioGenre")
            << QString::fromLatin1("AudioGenre")
            << false
            << true;
    QTest::newRow("Turtle")
            << QString::fromLatin1("Turtle")
            << QString()
            << false
            << false;
}

void tst_QGalleryTrackerSchema::fromItemType()
{
    QFETCH(QString, itemType);
    QFETCH(QString, expectedType);
    QFETCH(bool, isItemType);
    QFETCH(bool, isAggregateType);

    QGalleryTrackerSchema schema(itemType);

    QCOMPARE(schema.itemType(), expectedType);
    QCOMPARE(schema.isItemType(), isItemType);
    QCOMPARE(schema.isAggregateType(), isAggregateType);
}

void tst_QGalleryTrackerSchema::fromItemId_data()
{
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<bool>("isItemType");
    QTest::addColumn<bool>("isAggregateType");

    QTest::newRow("File")
            << QString::fromLatin1("file::/path/to/file.ext")
            << QString::fromLatin1("File")
            << true
            << false;
    QTest::newRow("AudioGenre")
            << QString::fromLatin1("audioGenre::Rock")
            << QString::fromLatin1("AudioGenre")
            << false
            << true;
    QTest::newRow("Turtle")
            << QString::fromLatin1("turtle::its/a/turtle")
            << QString()
            << false
            << false;
}

void tst_QGalleryTrackerSchema::fromItemId()
{
    QFETCH(QString, itemId);
    QFETCH(QString, itemType);
    QFETCH(bool, isItemType);
    QFETCH(bool, isAggregateType);

    QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromItemId(itemId);

    QCOMPARE(schema.itemType(), itemType);
    QCOMPARE(schema.isItemType(), isItemType);
    QCOMPARE(schema.isAggregateType(), isAggregateType);
}

#if 0
void tst_QGalleryTrackerSchema::uriFromItemId_data()
{
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QString>("uri");
    QTest::addColumn<QDocumentGallery::Error>("expectedError");

    QTest::newRow("File")
            << QString::fromLatin1("file::/path/to/file.ext")
            << QString::fromLatin1("/path/to/file.ext")
            << QDocumentGallery::NoError;
    QTest::newRow("Image")
            << QString::fromLatin1("file::/path/to/image.png")
            << QString::fromLatin1("/path/to/image.png")
            << QDocumentGallery::NoError;
    QTest::newRow("Album")
            << QString::fromLatin1("album::Self Titled/Greatest Hits")
            << QString()
            << QDocumentGallery::ItemIdError;
    QTest::newRow("Turtle")
            << QString::fromLatin1("turtle::its/a/turtle")
            << QString()
            << QDocumentGallery::ItemIdError;
}

void tst_QGalleryTrackerSchema::uriFromItemId()
{
    QFETCH(QString, itemId);
    QFETCH(QString, uri);
    QFETCH(QDocumentGallery::Error, expectedError);

    QDocumentGallery::Error error = QDocumentGallery::NoError;

    QCOMPARE(QGalleryTrackerSchema::uriFromItemId(&error, itemId), uri);
    QCOMPARE(error, expectedError);
}
#endif

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
             << QLatin1String("mimeType")
             << QLatin1String("fileName")
             << QLatin1String("fileSize")
             << QLatin1String("copyright")
             << QLatin1String("lastModified")
             << QLatin1String("lastAccessed")
             << QLatin1String("description")
             << QLatin1String("subject")
             << QLatin1String("title")
             << QLatin1String("keywords")
             << QLatin1String("filePath")
             << QLatin1String("url")
            );

    QTest::newRow("AudioGenre") << QString::fromLatin1("AudioGenre") << (QStringList()
             << QLatin1String("title")
            << QLatin1String("duration")
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

#if 0
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
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter);
    QTest::newRow("Audio.albumTitle")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("albumTitle")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.title")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumTitle")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.albumTitle")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumTitle")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.duration")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("duration")
            << QGalleryProperty::Attributes(QGalleryProperty::CanRead);
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
    QTest::addColumn<bool>("accumulative");
    QTest::addColumn<int>("updateMask");
    QTest::addColumn<QGalleryDBusInterfacePointer>("queryInterface");
    QTest::addColumn<QString>("queryMethod");
    QTest::addColumn<QVariantList>("queryArguments");

    QTest::newRow("File")
            << QString::fromLatin1("File")
            << false
            << 0xFF
            << m_metaDataInterface
            << QString::fromLatin1("GetCount")
            << (QVariantList() << QLatin1String("Files") << QLatin1String("*") << QString());

    QTest::newRow("Artist")
            << QString::fromLatin1("Artist")
            << false
            << 0x08
            << m_metaDataInterface
            << QString::fromLatin1("GetCount")
            << (QVariantList()
                    << QLatin1String("Music")
                    << QLatin1String("Audio:Artist")
                    << QString());

    QTest::newRow("Album")
            << QString::fromLatin1("Album")
            << true
            << 0x08
            << m_metaDataInterface
            << QString::fromLatin1("GetUniqueValuesWithCount")
            << (QVariantList()
                    << QLatin1String("Music")
                    << (QStringList() << QLatin1String("Audio:AlbumArtist"))
                    << QString()
                    << QLatin1String("Audio:Album")
                    << false);
}

void tst_QGalleryTrackerSchema::prepareValidTypeResponse()
{
    QFETCH(QString, itemType);
    QFETCH(bool, accumulative);
    QFETCH(int, updateMask);
    QFETCH(QGalleryDBusInterfacePointer, queryInterface);
    QFETCH(QString, queryMethod);
    QFETCH(QVariantList, queryArguments);

    QGalleryTrackerTypeResultSetArguments arguments;

    QGalleryTrackerSchema schema(itemType);
    QCOMPARE(schema.prepareTypeResponse(&arguments, this), QDocumentGallery::NoError);

    QCOMPARE(arguments.accumulative, accumulative);
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
    QTest::addColumn<QString>("queryMethod");
    QTest::addColumn<QVariantList>("queryArguments");

    QTest::newRow("file:://path/to/file.ext")
            << QVariant(QLatin1String("file::/path/to/file.ext"))
            << QStringList()
            << (QVector<QVariant>() << QLatin1String("/path/to/file.ext") << QLatin1String("Files"))
            << QVariant(QUrl(QLatin1String("file:///path/to/file.ext")))
            << QVariant(QLatin1String("File"))
            << 0xFF
            << 2
            << 2
            << 2
            << 2
            << m_searchInterface
            << QString::fromLatin1("Query")
            << (QVariantList()
                    << 0
                    << QLatin1String("Files")
                    << QStringList()
                    << QString()
                    << QStringList()
                    << QLatin1String(
                            "<rdfq:Condition><rdfq:and>"
                                "<rdfq:equals><rdfq:Property name=\"File:Path\"/><rdf:String>/path/to</rdf:String></rdfq:equals>"
                                "<rdfq:equals><rdfq:Property name=\"File:Name\"/><rdf:String>file.ext</rdf:String></rdfq:equals>"
                            "</rdfq:and></rdfq:Condition>")
                    << false
                    << QStringList()
                    << false);

    QTest::newRow("album::Self Titled/Greatest Hits")
            << QVariant(QLatin1String("album::Self Titled/Greatest Hits"))
            << QStringList()
            << (QVector<QVariant>() << QLatin1String("Self Titled") << QLatin1String("Greatest Hits"))
            << QVariant()
            << QVariant(QLatin1String("Album"))
            << 0x08
            << 2
            << 2
            << 0
            << 2
            << m_metaDataInterface
            << QString::fromLatin1("GetUniqueValuesWithAggregates")
            << (QVariantList()
                    << QLatin1String("Music")
                    << (QStringList() << QLatin1String("Audio:AlbumArtist") << QLatin1String("Audio:Album"))
                    << QLatin1String(
                            "<rdfq:Condition><rdfq:and>"
                                "<rdfq:equals><rdfq:Property name=\"Audio:AlbumArtist\"/><rdf:String>Self Titled</rdf:String></rdfq:equals>"
                                "<rdfq:equals><rdfq:Property name=\"Audio:Album\"/><rdf:String>Greatest Hits</rdf:String></rdfq:equals>"
                            "</rdfq:and></rdfq:Condition>")
                    << QStringList()
                    << QStringList()
                    << false);
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
    QFETCH(QString, queryMethod);
    QFETCH(QVariantList, queryArguments);

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
    QCOMPARE(arguments.queryMethod, queryMethod);
    QCOMPARE(arguments.queryArguments, queryArguments);
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

    QTest::newRow("Relative file path")
            << QString::fromLatin1("file::file.ext")
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("Relative file path")
            << QString::fromLatin1("album::Greatest Hits")
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
    QTest::addColumn<QGalleryDBusInterfacePointer>("queryInterface");
    QTest::addColumn<QString>("queryMethod");
    QTest::addColumn<int>("argumentCount");
    QTest::addColumn<int>("serviceIndex");
    QTest::addColumn<QString>("service");
    QTest::addColumn<int>("updateMask");
    QTest::addColumn<int>("identityWidth");
    QTest::addColumn<QVector<QVariant> >("rowData");
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QVariant>("itemUrl");
    QTest::addColumn<QString>("itemType");

    QTest::newRow("File: Files")
            << QString::fromLatin1("File")
            << m_searchInterface
            << QString::fromLatin1("Query")
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Files")
            << 0xFF
            << 2
            << (QVector<QVariant>()
                    << QLatin1String("/path/to/file.ext")
                    << QLatin1String("Files"))
            << QString::fromLatin1("file::/path/to/file.ext")
            << QVariant(QUrl(QLatin1String("file:///path/to/file.ext")))
            << QString::fromLatin1("File");

    QTest::newRow("File: Images")
            << QString::fromLatin1("File")
            << m_searchInterface
            << QString::fromLatin1("Query")
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Files")
            << 0xFF
            << 2
            << (QVector<QVariant>()
                    << QLatin1String("/path/to/image.png")
                    << QLatin1String("Images"))
            << QString::fromLatin1("image::/path/to/image.png")
            << QVariant(QUrl(QLatin1String("file:///path/to/image.png")))
            << QString::fromLatin1("Image");

    QTest::newRow("File: Turtles")
            << QString::fromLatin1("File")
            << m_searchInterface
            << QString::fromLatin1("Query")
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Files")
            << 0xFF
            << 2
            << (QVector<QVariant>()
                    << QLatin1String("/path/to/turtle.ttl")
                    << QLatin1String("Turtles"))
            << QString::fromLatin1("file::/path/to/turtle.ttl")
            << QVariant(QUrl(QLatin1String("file:///path/to/turtle.ttl")))
            << QString::fromLatin1("File");

    QTest::newRow("Text: Text")
            << QString::fromLatin1("Text")
            << m_searchInterface
            << QString::fromLatin1("Query")
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Text")
            << 0x80
            << 2
            << (QVector<QVariant>()
                    << QLatin1String("/path/to/text.txt")
                    << QLatin1String("Text"))
            << QString::fromLatin1("text::/path/to/text.txt")
            << QVariant(QUrl(QLatin1String("file:///path/to/text.txt")))
            << QString::fromLatin1("Text");

    QTest::newRow("Text: Development")
            << QString::fromLatin1("Text")
            << m_searchInterface
            << QString::fromLatin1("Query")
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Text")
            << 0x80
            << 2
            << (QVector<QVariant>()
                    << QLatin1String("/path/to/code.cpp")
                    << QLatin1String("Development"))
            << QString::fromLatin1("text::/path/to/code.cpp")
            << QVariant(QUrl(QLatin1String("file:///path/to/code.cpp")))
            << QString::fromLatin1("Text");

    QTest::newRow("Artist")
            << QString::fromLatin1("Artist")
            << m_metaDataInterface
            << QString::fromLatin1("GetUniqueValuesWithAggregates")
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Music")
            << 0x08
            << 1
            << (QVector<QVariant>()
                    << QLatin1String("Self Titled"))
            << QString::fromLatin1("artist::Self Titled")
            << QVariant()
            << QString::fromLatin1("Artist");

    QTest::newRow("Album")
            << QString::fromLatin1("Album")
            << m_metaDataInterface
            << QString::fromLatin1("GetUniqueValuesWithAggregates")
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Music")
            << 0x08
            << 2
            << (QVector<QVariant>()
                << QLatin1String("Self Titled")
                << QLatin1String("Greatest Hits"))
            << QString::fromLatin1("album::Self Titled/Greatest Hits")
            << QVariant()
            << QString::fromLatin1("Album");

    QTest::newRow("Album: No artist")
            << QString::fromLatin1("Album")
            << m_metaDataInterface
            << QString::fromLatin1("GetUniqueValuesWithAggregates")
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Music")
            << 0x08
            << 2
            << (QVector<QVariant>()
                << QVariant()
                << QLatin1String("Greatest Hits"))
            << QString::fromLatin1("album::/Greatest Hits")
            << QVariant()
            << QString::fromLatin1("Album");

    QTest::newRow("Album: Artist with slash")
            << QString::fromLatin1("Album")
            << m_metaDataInterface
            << QString::fromLatin1("GetUniqueValuesWithAggregates")
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_SERVICE_POSITION
            << QString::fromLatin1("Music")
            << 0x08
            << 2
            << (QVector<QVariant>()
                << QLatin1String("Self/Titled")
                << QLatin1String("Greatest Hits"))
            << QString::fromLatin1("album::Self//Titled/Greatest Hits")
            << QVariant()
            << QString::fromLatin1("Album");
}

void tst_QGalleryTrackerSchema::queryResponseRootType()
{
    QFETCH(QString, rootType);
    QFETCH(QGalleryDBusInterfacePointer, queryInterface);
    QFETCH(QString, queryMethod);
    QFETCH(int, argumentCount);
    QFETCH(int, serviceIndex);
    QFETCH(QString, service);
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
                    QStringList()),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.queryInterface, queryInterface);
    QCOMPARE(arguments.queryMethod, queryMethod);

    QCOMPARE(arguments.queryArguments.count(), argumentCount);
    QCOMPARE(arguments.queryArguments.at(serviceIndex), QVariant(service));

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
    QTest::addColumn<QStringList>("fieldNames");
    QTest::addColumn<QStringList>("sortFieldNames");
    QTest::addColumn<bool>("sortDescending");
    QTest::addColumn<QStringList>("filteredPropertyNames");
    QTest::addColumn<QVector<int> >("aliasColumns");
    QTest::addColumn<QVector<QGalleryTrackerSortCriteria> >("sortCriteria");
    QTest::addColumn<QVector<int> >("resourceKeys");

    QTest::newRow("File: [fileName, mimeType], []")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QStringList() // sortPropertyNames
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << QStringList() // sortFieldNames
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << QVector<QGalleryTrackerSortCriteria>() // sortCriteria
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [turtle, mimeType], []")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("turtle")
                    << QLatin1String("mimeType"))
            << QStringList() // sortPropertyNames
            << 3 // tableWidth
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Mime"))
            << QStringList() // sortFieldNames
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << QVector<QGalleryTrackerSortCriteria>() // sortCriteria
            << (QVector<int>() // resourceKeys
                    << 2);

    QTest::newRow("File: [fileName, mimeType], [fileName, mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [fileName, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::ReverseSorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-fileName, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::ReverseSorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-fileName, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName], [fileName, mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << 4 // tableWidth
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << QVector<int>(); // resourceKeys

    QTest::newRow("File: [fileName], [fileName, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::ReverseSorted)))
            << QVector<int>(); // resourceKeys

    QTest::newRow("File: [fileName], [-fileName, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::ReverseSorted)))
            << QVector<int>(); // resourceKeys

    QTest::newRow("File: [fileName], [-fileName, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-fileName")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << QVector<int>(); // resourceKeys

    // turtle is an non-existent property.
    QTest::newRow("File: [fileName, mimeType], [+turtle, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+turtle")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-turtle, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-turtle")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [+turtle, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+turtle")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-turtle, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-turtle")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    // url cannot be sorted on.
    QTest::newRow("File: [fileName, mimeType], [+url, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+url")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-url, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-url")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [+url, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+url")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-url, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-url")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    // keywords cannot be sorted on.
    QTest::newRow("File: [fileName, mimeType], [+keywords, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+keywords")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-keywords, +mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-keywords")
                    << QLatin1String("+mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [+keywords, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+keywords")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);

    QTest::newRow("File: [fileName, mimeType], [-keywords, -mimeType]")
            << QString::fromLatin1("File") // rootType
            << (QStringList() // propertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-keywords")
                    << QLatin1String("-mimeType"))
            << 4 // tableWidth
            << 4 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("File:Name")
                    << QLatin1String("File:Mime"))
            << (QStringList() // sortFieldNames
                    << QLatin1String("File:Mime"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("fileName")
                    << QLatin1String("mimeType"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resourceKeys
                    << 3);
}

void tst_QGalleryTrackerSchema::queryResponseFilePropertyNames()
{
    QFETCH(QString, rootType);
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, sortPropertyNames);
    QFETCH(int, tableWidth);
    QFETCH(int, compositeOffset);
    QFETCH(QStringList, sortFieldNames);
    QFETCH(bool, sortDescending);
    QFETCH(QStringList, filteredPropertyNames);
    QFETCH(QStringList, fieldNames);
    QFETCH(QVector<int>, aliasColumns);
    QFETCH(QVector<QGalleryTrackerSortCriteria>, sortCriteria);
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
                    sortPropertyNames),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.tableWidth, tableWidth);
    QCOMPARE(arguments.valueOffset, 2);
    QCOMPARE(arguments.compositeOffset, compositeOffset);

    QCOMPARE(arguments.queryArguments.count(), 9);
    QCOMPARE(arguments.queryArguments.at(0), QVariant(0u));
    QCOMPARE(arguments.queryArguments.at(2), QVariant(fieldNames));
    QCOMPARE(arguments.queryArguments.at(3), QVariant(QString()));
    QCOMPARE(arguments.queryArguments.at(4), QVariant(QStringList()));
    QCOMPARE(arguments.queryArguments.at(6), QVariant(false));
    QCOMPARE(arguments.queryArguments.at(7), QVariant(sortFieldNames));
    QCOMPARE(arguments.queryArguments.at(8), QVariant(sortDescending));

    QCOMPARE(arguments.propertyNames, filteredPropertyNames);
    QCOMPARE(arguments.fieldNames, fieldNames);

    QCOMPARE(arguments.aliasColumns, aliasColumns);
    QCOMPARE(arguments.sortCriteria, sortCriteria);
    QCOMPARE(arguments.resourceKeys, resourceKeys);
}

void tst_QGalleryTrackerSchema::queryResponseAggregatePropertyNames_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QStringList>("sortPropertyNames");
    QTest::addColumn<int>("tableWidth");
    QTest::addColumn<int>("valueOffset");
    QTest::addColumn<int>("compositeOffset");
    QTest::addColumn<QStringList>("fieldNames");
    QTest::addColumn<QStringList>("aggregateFieldNames");
    QTest::addColumn<QStringList>("aggregates");
    QTest::addColumn<bool>("sortDescending");
    QTest::addColumn<QStringList>("filteredPropertyNames");
    QTest::addColumn<QVector<int> >("aliasColumns");
    QTest::addColumn<QVector<QGalleryTrackerSortCriteria> >("sortCriteria");

    QTest::newRow("AudioGenre: [title, trackCount, duration], []")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QStringList() // sortPropertyNames
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>());

    QTest::newRow("AudioGenre: [title, trackCount, duration, turtle], []")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration")
                    << QLatin1String("turtle"))
            << QStringList() // sortPropertyNames
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>());

    QTest::newRow("AudioGenre: [genre, trackCount, duration], []")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("genre")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QStringList() // sortPropertyNames
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration")
                    << QLatin1String("genre"))
            << (QVector<int>() // aliasColumns
                    << 0)
            << (QVector<QGalleryTrackerSortCriteria>());

    QTest::newRow("AudioGenre: [trackCount, duration, title], []")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("trackCount")
                    << QLatin1String("duration")
                    << QLatin1String("title"))
            << QStringList() // sortPropertyNames
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>());

    QTest::newRow("AudioGenre: [genre, trackCount, duration, title], []")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("genre")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration")
                    << QLatin1String("title"))
            << QStringList() // sortPropertyNames
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration")
                    << QLatin1String("genre"))
            << (QVector<int>() // aliasColumns
                    << 0)
            << (QVector<QGalleryTrackerSortCriteria>());

    QTest::newRow("AudioGenre: [title, trackCount, duration], [title]")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("title"))
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("AudioGenre: [title, trackCount, duration], [+title]")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+title"))
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("AudioGenre: [title, trackCount, duration], [-title]")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-title"))
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << true // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)));


    QTest::newRow("AudioGenre: [title, trackCount, duration], [+title, trackCount]")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+title")
                    << QLatin1String("trackCount"))
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << false // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("AudioGenre: [title, trackCount, duration], [+duration, -title]")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+duration")
                    << QLatin1String("-title"))
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << true // sortDescending
            << (QStringList() // filteredPropertyName
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("AudioGenre: [title, trackCount, duration], [-duration, -title]")
            << QString::fromLatin1("AudioGenre")
            << (QStringList() // propertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-duration")
                    << QLatin1String("-title"))
            << 3 // tableWidth
            << 0 // valueOffset
            << 3 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Genre"))
            << (QStringList() // aggregateFieldNames
                    << QLatin1String("*")
                    << QLatin1String("Audio:Duration"))
            << (QStringList() // aggregates
                    << QLatin1String("COUNT")
                    << QLatin1String("SUM"))
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("title")
                    << QLatin1String("trackCount")
                    << QLatin1String("duration"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>()
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("Album: [], []")
            << QString::fromLatin1("Album")
            << QStringList() // propertyNames
            << QStringList() // sortPropertyNames
            << 2 // tableWidth
            << 0 // valueOffset
            << 2 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:AlbumArtist")
                    << QLatin1String("Audio:Album"))
            << QStringList() // aggregateFieldNames
            << QStringList() // aggregates
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("artist")
                    << QLatin1String("title"))
            << QVector<int>() // aliasColumns
            << QVector<QGalleryTrackerSortCriteria>(); // sortCriteria

    QTest::newRow("Album: [], [artist]")
            << QString::fromLatin1("Album")
            << QStringList() // propertyNames
            << (QStringList() // sortPropertyNames
                    << QLatin1String("artist"))
            << 2 // tableWidth
            << 0 // valueOffset
            << 2 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:AlbumArtist")
                    << QLatin1String("Audio:Album"))
            << QStringList() // aggregateFieldNames
            << QStringList() // aggregates
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("artist")
                    << QLatin1String("title"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("Album: [], [-artist]")
            << QString::fromLatin1("Album")
            << QStringList() // propertyNames
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-artist"))
            << 2 // tableWidth
            << 0 // valueOffset
            << 2 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:AlbumArtist")
                    << QLatin1String("Audio:Album"))
            << QStringList() // aggregateFieldNames
            << QStringList() // aggregates
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("artist")
                    << QLatin1String("title"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("Album: [], [+title]")
            << QString::fromLatin1("Album")
            << QStringList() // propertyNames
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+title"))
            << 2 // tableWidth
            << 0 // valueOffset
            << 2 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Album")
                    << QLatin1String("Audio:AlbumArtist"))
            << QStringList() // aggregateFieldNames
            << QStringList() // aggregates
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("title")
                    << QLatin1String("artist"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("Album: [], [-title]")
            << QString::fromLatin1("Album")
            << QStringList() // propertyNames
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-title"))
            << 2 // tableWidth
            << 0 // valueOffset
            << 2 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Album")
                    << QLatin1String("Audio:AlbumArtist"))
            << QStringList() // aggregateFieldNames
            << QStringList() // aggregates
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("title")
                    << QLatin1String("artist"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted)));

    QTest::newRow("Album: [], [+title, -albumArtist]")
            << QString::fromLatin1("Album")
            << QStringList() // propertyNames
            << (QStringList() // sortPropertyNames
                    << QLatin1String("+title")
                    << QLatin1String("-albumArtist"))
            << 2 // tableWidth
            << 0 // valueOffset
            << 2 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Album")
                    << QLatin1String("Audio:AlbumArtist"))
            << QStringList() // aggregateFieldNames
            << QStringList() // aggregates
            << false // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("title")
                    << QLatin1String("artist"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(1, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::ReverseSorted)));

    QTest::newRow("Album: [], [-title, +albumArtist]")
            << QString::fromLatin1("Album")
            << QStringList() // propertyNames
            << (QStringList() // sortPropertyNames
                    << QLatin1String("-title")
                    << QLatin1String("+albumArtist"))
            << 2 // tableWidth
            << 0 // valueOffset
            << 2 // compositeOffset
            << (QStringList() // fieldNames
                    << QLatin1String("Audio:Album")
                    << QLatin1String("Audio:AlbumArtist"))
            << QStringList() // aggregateFieldNames
            << QStringList() // aggregates
            << true // sortDescending
            << (QStringList() // filteredPropertyNames
                    << QLatin1String("title")
                    << QLatin1String("artist"))
            << QVector<int>() // aliasColumns
            << (QVector<QGalleryTrackerSortCriteria>() // sortCriteria
                    << (QGalleryTrackerSortCriteria(0, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(1, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::ReverseSorted)));
}

void tst_QGalleryTrackerSchema::queryResponseAggregatePropertyNames()
{
    QFETCH(QString, rootType);
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, sortPropertyNames);
    QFETCH(int, tableWidth);
    QFETCH(int, valueOffset);
    QFETCH(int, compositeOffset);
    QFETCH(QStringList, aggregateFieldNames);
    QFETCH(QStringList, aggregates);
    QFETCH(bool, sortDescending);
    QFETCH(QStringList, filteredPropertyNames);
    QFETCH(QStringList, fieldNames);
    QFETCH(QVector<int>, aliasColumns);
    QFETCH(QVector<QGalleryTrackerSortCriteria>, sortCriteria);

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
                    sortPropertyNames),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.tableWidth, tableWidth);
    QCOMPARE(arguments.valueOffset, valueOffset);
    QCOMPARE(arguments.compositeOffset, compositeOffset);

    QCOMPARE(arguments.queryArguments.count(), 6);
    QCOMPARE(arguments.queryArguments.at(1), QVariant(fieldNames));
    QCOMPARE(arguments.queryArguments.at(3), QVariant(aggregates));
    QCOMPARE(arguments.queryArguments.at(4), QVariant(aggregateFieldNames));
    QCOMPARE(arguments.queryArguments.at(5), QVariant(sortDescending));

    QCOMPARE(arguments.propertyNames, filteredPropertyNames);

    QCOMPARE(arguments.aliasColumns, aliasColumns);
    QCOMPARE(arguments.sortCriteria, sortCriteria);
    QCOMPARE(arguments.resourceKeys, QVector<int>());

}

void tst_QGalleryTrackerSchema::queryResponseRootItem_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<int>("argumentCount");
    QTest::addColumn<int>("queryStringIndex");
    QTest::addColumn<QString>("queryString");

    QTest::newRow("Folder, All File Descendants")
            << QString::fromLatin1("File")
            << QString::fromLatin1("folder::/path/to")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:or>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"File:Path\"/>"
                                "<rdf:String>/path/to</rdf:String>"
                            "</rdfq:equals>"
                            "<rdfq:startsWith>"
                                "<rdfq:Property name=\"File:Path\"/>"
                                "<rdf:String>/path/to/</rdf:String>"
                            "</rdfq:startsWith>"
                        "</rdfq:or>"
                    "</rdfq:Condition>");

    QTest::newRow("Folder, Direct File Descendants")
            << QString::fromLatin1("File")
            << QString::fromLatin1("folder::/path/to")
            << QGalleryQueryRequest::DirectDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"File:Path\"/>"
                            "<rdf:String>/path/to</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Album, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("album::Self Titled/Greatest Hits")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:and>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                                "<rdf:String>Self Titled</rdf:String>"
                            "</rdfq:equals>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:Album\"/>"
                                "<rdf:String>Greatest Hits</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:and>"
                    "</rdfq:Condition>");

    QTest::newRow("Album, All Audio Descendants, No Album")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("album::/Greatest Hits")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:and>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                                "<rdf:String></rdf:String>"
                            "</rdfq:equals>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:Album\"/>"
                                "<rdf:String>Greatest Hits</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:and>"
                    "</rdfq:Condition>");

    QTest::newRow("Album, All Audio Descendants, Album with slash")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("album::Self//Titled/Greatest Hits")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:and>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                                "<rdf:String>Self/Titled</rdf:String>"
                            "</rdfq:equals>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:Album\"/>"
                                "<rdf:String>Greatest Hits</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:and>"
                    "</rdfq:Condition>");

    QTest::newRow("Album, All Audio Descendants, No album title")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("album::Self Titled/")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:and>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                                "<rdf:String>Self Titled</rdf:String>"
                            "</rdfq:equals>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:Album\"/>"
                                "<rdf:String></rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:and>"
                    "</rdfq:Condition>");

    QTest::newRow("Album, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("album::Self Titled/Greatest Hits")
            << QGalleryQueryRequest::DirectDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:and>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                                "<rdf:String>Self Titled</rdf:String>"
                            "</rdfq:equals>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:Album\"/>"
                                "<rdf:String>Greatest Hits</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:and>"
                    "</rdfq:Condition>");

    QTest::newRow("Album Artist, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("albumArtist::Self Titled")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Album Artist, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("albumArtist::Self Titled")
            << QGalleryQueryRequest::DirectDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Album Artist, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumArtist::Self Titled")
            << QGalleryQueryRequest::AllDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Album Artist, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("albumArtist::Self Titled")
            << QGalleryQueryRequest::DirectDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Artist, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("artist::Self Titled")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Artist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Artist, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("artist::Self Titled")
            << QGalleryQueryRequest::DirectDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Artist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Artist, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("artist::Self Titled")
            << QGalleryQueryRequest::AllDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Artist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Artist, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("artist::Self Titled")
            << QGalleryQueryRequest::DirectDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Artist\"/>"
                            "<rdf:String>Self Titled</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Audio Genre, All Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Genre\"/>"
                            "<rdf:String>Rock</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Audio Genre, Direct Audio Descendants")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::DirectDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Genre\"/>"
                            "<rdf:String>Rock</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Audio Genre, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::AllDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Genre\"/>"
                            "<rdf:String>Rock</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Audio Genre, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("audioGenre::Rock")
            << QGalleryQueryRequest::DirectDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Audio:Genre\"/>"
                            "<rdf:String>Rock</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Photo Album, All Image Descendants")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("photoAlbum::Camping")
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Image:Album\"/>"
                            "<rdf:String>Camping</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("Photo Album, Direct Image Descendants")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("photoAlbum::Camping")
            << QGalleryQueryRequest::DirectDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString::fromLatin1(
                    "<rdfq:Condition>"
                        "<rdfq:equals>"
                            "<rdfq:Property name=\"Image:Album\"/>"
                            "<rdf:String>Camping</rdf:String>"
                        "</rdfq:equals>"
                    "</rdfq:Condition>");

    QTest::newRow("No Root Item, All Image Descendants")
            << QString::fromLatin1("Image")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QT_FILE_QUERY_ARGUMENTS_COUNT
            << QT_FILE_QUERY_STRING_POSITION
            << QString();

    QTest::newRow("No Root Item, All Album Descendants")
            << QString::fromLatin1("Album")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString();

    QTest::newRow("No Root Item, Direct Album Descendants")
            << QString::fromLatin1("Album")
            << QString()
            << QGalleryQueryRequest::DirectDescendants
            << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
            << QT_AGGREGATE_QUERY_STRING_POSITION
            << QString();
}

void tst_QGalleryTrackerSchema::queryResponseRootItem()
{
    QFETCH(QString, rootType);
    QFETCH(QString, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(int, argumentCount);
    QFETCH(int, queryStringIndex);
    QFETCH(QString, queryString);

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
                    QStringList()),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.queryArguments.count(), argumentCount);
    QCOMPARE(arguments.queryArguments.at(queryStringIndex), QVariant(queryString));
}

void tst_QGalleryTrackerSchema::queryResponseFilter_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<QGalleryFilter>("filter");
    QTest::addColumn<int>("argumentCount");
    QTest::addColumn<int>("queryStringIndex");
    QTest::addColumn<QString>("queryString");

    {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl::fromLocalFile(QLatin1String("/path/to/file.ext"));

        QTest::newRow("File.url == file:///path/to/file.ext")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Path\"/>"
                                    "<rdf:String>/path/to</rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Name\"/>"
                                    "<rdf:String>file.ext</rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl::fromLocalFile(QLatin1String("/"));

        QTest::newRow("File.url == file:///")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Path\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Name\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl(QLatin1String("http://example.com"));

        QTest::newRow("File.url == http://example.com")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Path\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Name\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter
                = QDocumentGallery::url == QUrl(QLatin1String("http://example.com/index.html"));

        QTest::newRow("File.url == http://example.com/index.html")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Path\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Name\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::filePath == QLatin1String("/path/to/file.ext");

        QTest::newRow("File.filePath == /path/to/file.ext")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Path\"/>"
                                    "<rdf:String>/path/to</rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Name\"/>"
                                    "<rdf:String>file.ext</rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::filePath == QLatin1String("file.ext");

        QTest::newRow("File.filePath == file.ext")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Path\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Name\"/>"
                                    "<rdf:String></rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::fileName == QLatin1String("file.ext");

        QTest::newRow("File.fileName == file.ext")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"File:Name\"/>"
                                "<rdf:String>file.ext</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.startsWith(QLatin1String("file."));

        QTest::newRow("File.fileName.startsWith(file.)")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:startsWith>"
                                "<rdfq:Property name=\"File:Name\"/>"
                                "<rdf:String>file.</rdf:String>"
                            "</rdfq:startsWith>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.endsWith(QLatin1String(".ext"));

        QTest::newRow("File.fileName.endsWith(.ext)")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"File:Name\"/>"
                                "<rdf:String>*.ext</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.contains(QLatin1String("ext"));

        QTest::newRow("File.fileName.contains(ext)")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:contains>"
                                "<rdfq:Property name=\"File:Name\"/>"
                                "<rdf:String>ext</rdf:String>"
                            "</rdfq:contains>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::fileName.wildcard(QLatin1String("file*ext"));

        QTest::newRow("File.fileName.wildcard(file*ext")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"File:Name\"/>"
                                "<rdf:String>file*ext</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter
                = QDocumentGallery::fileName.regExp(QLatin1String("(file|document).ext"));

        QTest::newRow("File.fileName.regExp((file|document).ext)")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:regex>"
                                "<rdfq:Property name=\"File:Name\"/>"
                                "<rdf:String>(file|document).ext</rdf:String>"
                            "</rdfq:regex>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter
                = QDocumentGallery::fileName.regExp(QRegExp(QLatin1String("(file|document).ext")));

        QTest::newRow("File.fileName.regExp(QRegExp((file|document).ext))")
                << QString::fromLatin1("File")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:regex>"
                                "<rdfq:Property name=\"File:Name\"/>"
                                "<rdf:String>(file|document).ext</rdf:String>"
                            "</rdfq:regex>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter
                = QDocumentGallery::description == QUrl(QLatin1String("http://example.com/index.html"));

        QTest::newRow("Image.description == http://example.com/index.html")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Image:Description\"/>"
                                "<rdf:String>http://example.com/index.html</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::width > 1024;

        QTest::newRow("Image.width > 1024")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:greaterThan>"
                                "<rdfq:Property name=\"Image:Width\"/>"
                                "<rdf:Integer>1024</rdf:Integer>"
                            "</rdfq:greaterThan>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::width >= 1024u;

        QTest::newRow("Video.width >= 1024u")
                << QString::fromLatin1("Video")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:greaterThanEqual>"
                                "<rdfq:Property name=\"Video:Width\"/>"
                                "<rdf:Integer>1024</rdf:Integer>"
                            "</rdfq:greaterThanEqual>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::height < Q_INT64_C(1024);

        QTest::newRow("Image.height < 1024ll")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:lessThan>"
                                "<rdfq:Property name=\"Image:Height\"/>"
                                "<rdf:Integer>1024</rdf:Integer>"
                            "</rdfq:lessThan>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::height <= Q_UINT64_C(1024);

        QTest::newRow("Video.height <= 1024ull")
                << QString::fromLatin1("Video")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:lessThanEqual>"
                                "<rdfq:Property name=\"Video:Height\"/>"
                                "<rdf:Integer>1024</rdf:Integer>"
                            "</rdfq:lessThanEqual>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::focalLength <= 1.9;

        QTest::newRow("Image.focalLength <= 1.9")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:lessThanEqual>"
                                "<rdfq:Property name=\"Image:FocalLength\"/>"
                                "<rdf:Float>1.9</rdf:Float>"
                            "</rdfq:lessThanEqual>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::focalLength > 0.25f;

        QTest::newRow("Image.focalLength <= 0.25f")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:greaterThan>"
                                "<rdfq:Property name=\"Image:FocalLength\"/>"
                                "<rdf:Float>0.25</rdf:Float>"
                            "</rdfq:greaterThan>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter
                = QDocumentGallery::dateTaken > QDateTime(QDate(2008, 06, 01), QTime(12, 5, 8));

        QTest::newRow("Image.dateTaken > 2008-06-01T12:05:08")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:greaterThan>"
                                "<rdfq:Property name=\"Image:Date\"/>"
                                "<rdf:Date>2008-06-01T12:05:08</rdf:Date>"
                            "</rdfq:greaterThan>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = !(
                QDocumentGallery::dateTaken > QDateTime(QDate(2008, 06, 01), QTime(12, 5, 8)));

        QTest::newRow("!(Image.dateTaken > 2008-06-01T12:05:08")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:not>"
                                "<rdfq:greaterThan>"
                                    "<rdfq:Property name=\"Image:Date\"/>"
                                    "<rdf:Date>2008-06-01T12:05:08</rdf:Date>"
                                "</rdfq:greaterThan>"
                            "</rdfq:not>"
                        "</rdfq:Condition>");

    } {
        QGalleryIntersectionFilter filter;
        filter.append(QDocumentGallery::width > 1024);
        filter.append(QDocumentGallery::height > 768);

        QTest::newRow("Image.width > 1024 && Image.height > 768")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:greaterThan>"
                                    "<rdfq:Property name=\"Image:Width\"/>"
                                    "<rdf:Integer>1024</rdf:Integer>"
                                "</rdfq:greaterThan>"
                                "<rdfq:greaterThan>"
                                    "<rdfq:Property name=\"Image:Height\"/>"
                                    "<rdf:Integer>768</rdf:Integer>"
                                "</rdfq:greaterThan>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryIntersectionFilter filter;
        filter.append(QDocumentGallery::width > 1024);

        QTest::newRow("Image.width > 1024 (Single intersection item)")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:greaterThan>"
                                    "<rdfq:Property name=\"Image:Width\"/>"
                                    "<rdf:Integer>1024</rdf:Integer>"
                                "</rdfq:greaterThan>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryUnionFilter filter;
        filter.append(QDocumentGallery::width < 1920);
        filter.append(QDocumentGallery::height < 1024);

        QTest::newRow("Image.width < 1920 || Image.height < 1024")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:or>"
                                "<rdfq:lessThan>"
                                    "<rdfq:Property name=\"Image:Width\"/>"
                                    "<rdf:Integer>1920</rdf:Integer>"
                                "</rdfq:lessThan>"
                                "<rdfq:lessThan>"
                                    "<rdfq:Property name=\"Image:Height\"/>"
                                    "<rdf:Integer>1024</rdf:Integer>"
                                "</rdfq:lessThan>"
                            "</rdfq:or>"
                        "</rdfq:Condition>");
    } {
        QGalleryUnionFilter filter;
        filter.append(QDocumentGallery::width < 1920);

        QTest::newRow("Image.width < 1920 (Single union item")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:or>"
                                "<rdfq:lessThan>"
                                    "<rdfq:Property name=\"Image:Width\"/>"
                                    "<rdf:Integer>1920</rdf:Integer>"
                                "</rdfq:lessThan>"
                            "</rdfq:or>"
                        "</rdfq:Condition>");
    } {
        QGalleryUnionFilter filter;

        QTest::newRow("Image (Empty union filter)")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition/>");
    } {
        QGalleryIntersectionFilter filter;

        QTest::newRow("Image (Empty intersection filter)")
                << QString::fromLatin1("Image")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition/>");
    } {
        QGalleryFilter filter = QDocumentGallery::fileName == QLatin1String("file.ext");

        QTest::newRow("File.fileName == file.ext, In folder")
                << QString::fromLatin1("File")
                << QString::fromLatin1("folder::/path/to")
                << QGalleryQueryRequest::DirectDescendants
                << filter
                << QT_FILE_QUERY_ARGUMENTS_COUNT
                << QT_FILE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Path\"/>"
                                    "<rdf:String>/path/to</rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"File:Name\"/>"
                                    "<rdf:String>file.ext</rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits (All)")
                << QString::fromLatin1("Album")
                << QString()
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
                << QT_AGGREGATE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:Album\"/>"
                                "<rdf:String>Greatest Hits</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits (Direct)")
                << QString::fromLatin1("Album")
                << QString()
                << QGalleryQueryRequest::DirectDescendants
                << QGalleryFilter(filter)
                << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
                << QT_AGGREGATE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:equals>"
                                "<rdfq:Property name=\"Audio:Album\"/>"
                                "<rdf:String>Greatest Hits</rdf:String>"
                            "</rdfq:equals>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits, Belonging to AlbumArtist (All)")
                << QString::fromLatin1("Album")
                << QString::fromLatin1("albumArtist::Self Titled")
                << QGalleryQueryRequest::AllDescendants
                << QGalleryFilter(filter)
                << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
                << QT_AGGREGATE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                                    "<rdf:String>Self Titled</rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"Audio:Album\"/>"
                                    "<rdf:String>Greatest Hits</rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    } {
        QGalleryFilter filter = QDocumentGallery::title == QLatin1String("Greatest Hits");

        QTest::newRow("Album.title == Greatest Hits, Belonging to AlbumArtist (Direct)")
                << QString::fromLatin1("Album")
                << QString::fromLatin1("albumArtist::Self Titled")
                << QGalleryQueryRequest::DirectDescendants
                << QGalleryFilter(filter)
                << QT_AGGREGATE_QUERY_ARGUMENTS_COUNT
                << QT_AGGREGATE_QUERY_STRING_POSITION
                << QString::fromLatin1(
                        "<rdfq:Condition>"
                            "<rdfq:and>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"Audio:AlbumArtist\"/>"
                                    "<rdf:String>Self Titled</rdf:String>"
                                "</rdfq:equals>"
                                "<rdfq:equals>"
                                    "<rdfq:Property name=\"Audio:Album\"/>"
                                    "<rdf:String>Greatest Hits</rdf:String>"
                                "</rdfq:equals>"
                            "</rdfq:and>"
                        "</rdfq:Condition>");
    }
}

void tst_QGalleryTrackerSchema::queryResponseFilter()
{
    QFETCH(QString, rootType);
    QFETCH(QString, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(QGalleryFilter, filter);
    QFETCH(int, argumentCount);
    QFETCH(int, queryStringIndex);
    QFETCH(QString, queryString);

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
                    QStringList()),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.queryArguments.count(), argumentCount);
    QCOMPARE(arguments.queryArguments.at(queryStringIndex), QVariant(queryString));
}

void tst_QGalleryTrackerSchema::queryResponseRootFileItems()
{
    QRegExp regExp(QLatin1String(
            "<rdfq:Condition>"
                "<rdfq:inSet>"
                    "<rdfq:Property name=\"File:Path\"/>"
                    "<rdf:String>.*</rdf:String>"
                "</rdfq:inSet>"
            "</rdfq:Condition>"));
    regExp.setMinimal(true);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(QLatin1String("File"));

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    QGalleryQueryRequest::DirectDescendants,
                    QString(),
                    QGalleryFilter(),
                    QStringList(),
                    QStringList()),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.queryArguments.count(), QT_FILE_QUERY_ARGUMENTS_COUNT);
    QVERIFY(regExp.exactMatch(
            arguments.queryArguments.at(QT_FILE_QUERY_STRING_POSITION).toString()));
}

void tst_QGalleryTrackerSchema::queryResponseRootFileItemsWithFilter()
{
    QRegExp regExp(QLatin1String(
            "<rdfq:Condition>"
                "<rdfq:and>"
                    "<rdfq:inSet>"
                        "<rdfq:Property name=\"File:Path\"/>"
                        "<rdf:String>.*</rdf:String>"
                    "</rdfq:inSet>"
                    "<rdfq:equals>"
                        "<rdfq:Property name=\"File:Name\"/>"
                        "<rdf:String>file\\.ext</rdf:String>"
                    "</rdfq:equals>"
                "</rdfq:and>"
            "</rdfq:Condition>"));

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(QLatin1String("File"));

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments,
                    this,
                    QGalleryQueryRequest::DirectDescendants,
                    QString(),
                    QDocumentGallery::fileName == QLatin1String("file.ext"),
                    QStringList(),
                    QStringList()),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.queryArguments.count(), QT_FILE_QUERY_ARGUMENTS_COUNT);
    QVERIFY(regExp.exactMatch(
            arguments.queryArguments.at(QT_FILE_QUERY_STRING_POSITION).toString()));
}

void tst_QGalleryTrackerSchema::queryResponseValueColumnToVariant_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QString>("string");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("File.fileName")
            << QString::fromLatin1("File")
            << QString::fromLatin1("fileName")
            << QString::fromLatin1("file.ext")
            << QVariant(QLatin1String("file.ext"));

    QTest::newRow("File.fileName (Empty)")
            << QString::fromLatin1("File")
            << QString::fromLatin1("fileName")
            << QString()
            << QVariant(QString());

    QTest::newRow("Image.keywords (1)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QString::fromLatin1("Holiday")
            << QVariant(QStringList() << QLatin1String("Holiday"));

    QTest::newRow("Image.keywords (2)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QString::fromLatin1("Holiday|Summer")
            << QVariant(QStringList() << QLatin1String("Holiday") << QLatin1String("Summer"));

    QTest::newRow("Image.keywords (3)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QString::fromLatin1("2009|Holiday|Summer")
            << QVariant(QStringList()
                    << QLatin1String("2009")
                    << QLatin1String("Holiday")
                    << QLatin1String("Summer"));

    QTest::newRow("Image.keywords (Empty")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QString()
            << QVariant(QStringList());

    QTest::newRow("Image.dateTaken")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("dateTaken")
            << QString::fromLatin1("2009-08-23T09:03:32")
            << QVariant(QDateTime(QDate(2009, 8, 23), QTime(9, 3, 32)));

    QTest::newRow("Image.dateTaken (Invalid)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("dateTaken")
            << QString::fromLatin1("Yesterday")
            << QVariant();

    QTest::newRow("Image.dateTaken (Empty)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("dateTaken")
            << QString()
            << QVariant();

    QTest::newRow("Image.focalLength (12)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("focalLength")
            << QString::fromLatin1("12")
            << QVariant(12.0);

    QTest::newRow("Image.focalLength (12.5)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("focalLength")
            << QString::fromLatin1("12.5")
            << QVariant(12.5);

    QTest::newRow("Image.focalLength (Invalid)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("focalLength")
            << QString::fromLatin1("twelve point 5")
            << QVariant();

    QTest::newRow("Image.focalLength (Empty")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("focalLength")
            << QString()
            << QVariant();

    QTest::newRow("Image.height")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("height")
            << QString("720")
            << QVariant(720);

    QTest::newRow("Image.height (Invalid")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("height")
            << QString::fromLatin1("seven twenty")
            << QVariant();

    QTest::newRow("Image.height (Empty)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("height")
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
                    QStringList()),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.valueColumns.count(), 1);
    QCOMPARE(arguments.valueColumns.at(0)->toVariant(string), value);
}

void tst_QGalleryTrackerSchema::queryResponseValueColumnToString_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("value");
    QTest::addColumn<QString>("string");

    QTest::newRow("File.fileName")
            << QString::fromLatin1("File")
            << QString::fromLatin1("fileName")
            << QVariant(QLatin1String("file.ext"))
            << QString::fromLatin1("file.ext");

    QTest::newRow("File.fileName (Empty)")
            << QString::fromLatin1("File")
            << QString::fromLatin1("fileName")
            << QVariant(QString())
            << QString();

    QTest::newRow("Image.keywords (1)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QVariant(QStringList() << QLatin1String("Holiday"))
            << QString::fromLatin1("Holiday");

    QTest::newRow("Image.keywords (2)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QVariant(QStringList() << QLatin1String("Holiday") << QLatin1String("Summer"))
            << QString::fromLatin1("Holiday|Summer");

    QTest::newRow("Image.keywords (3)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QVariant(QStringList()
                    << QLatin1String("2009")
                    << QLatin1String("Holiday")
                    << QLatin1String("Summer"))
            << QString::fromLatin1("2009|Holiday|Summer");

    QTest::newRow("Image.keywords (QString)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QVariant(QLatin1String("Holiday"))
            << QString::fromLatin1("Holiday");

    QTest::newRow("Image.keywords (Empty")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("keywords")
            << QVariant(QStringList())
            << QString();

    QTest::newRow("Image.dateTaken")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("dateTaken")
            << QVariant(QDateTime(QDate(2009, 8, 23), QTime(9, 3, 32)))
            << QString::fromLatin1("2009-08-23T09:03:32");

    QTest::newRow("Image.dateTaken (Invalid)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("dateTaken")
            << QVariant(QDateTime(QDate(2009, 8, 56), QTime(95, 3, 32)))
            << QString();

    QTest::newRow("Image.dateTaken (Empty)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("dateTaken")
            << QVariant()
            << QString();

    QTest::newRow("Image.focalLength (12)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("focalLength")
            << QVariant(12.0)
            << QString::fromLatin1("12");

    QTest::newRow("Image.focalLength (12.5)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("focalLength")
            << QVariant(12.5)
            << QString::fromLatin1("12.5");

    QTest::newRow("Image.focalLength (Empty")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("focalLength")
            << QVariant()
            << QString();

    QTest::newRow("Image.height")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("height")
            << QVariant(720)
            << QString("720");

    QTest::newRow("Image.height (Empty)")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("height")
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
                    QStringList()),
            QDocumentGallery::NoError);

    QCOMPARE(arguments.valueColumns.count(), 1);
    QCOMPARE(arguments.valueColumns.at(0)->toString(value), string);
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
            << (QVector<QVariant>() << QLatin1String("/path/to/file.ext") << QLatin1String("Files"))
            << QVariant(QLatin1String("/path/to/file.ext"));

    QTest::newRow("File.url")
            << QString::fromLatin1("Image")
            << QString::fromLatin1("url")
            << (QVector<QVariant>() << QLatin1String("/path/to/file.ext") << QLatin1String("Files"))
            << QVariant(QUrl::fromLocalFile(QLatin1String("/path/to/file.ext")));
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
                    QStringList()),
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
            << QString::fromLatin1("Turtle")
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemTypeError;

    QTest::newRow("Invalid Type, With Filter")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("Turtle")
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
            << QString::fromLatin1("turtle::/its/a/turtle")
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("Invalid Album ID")
            << QString::fromLatin1("album::Greatest Hits")
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("Audio")
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << QDocumentGallery::ItemIdError;

    QTest::newRow("File.filePath > /path")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
            << QGalleryFilter(QDocumentGallery::filePath > QLatin1String("/path"))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.url > file:///path")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
            << QGalleryFilter(QDocumentGallery::url > QUrl::fromLocalFile(QLatin1String("/path")))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.filePath > /path (within union)")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
            << QGalleryFilter(QGalleryUnionFilter(
                    QDocumentGallery::filePath > QLatin1String("/path")))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.filePath > /path (within intersection)")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
            << QGalleryFilter(QGalleryIntersectionFilter(
                    QDocumentGallery::filePath > QLatin1String("/path")))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;

    QTest::newRow("File.fileName ? /path")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
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
            << QString::fromLatin1("File")
            << QGalleryFilter(QDocumentGallery::fileName == QPoint(12, 44))
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;


    QTest::newRow("File.url == 125")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
            << QGalleryFilter(QDocumentGallery::url == 125)
            << QStringList()
            << QStringList()
            << QDocumentGallery::FilterError;
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
                    &arguments, this, scope, rootItem, filter, propertyNames, sortPropertyNames),
            error);
}
#endif
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

