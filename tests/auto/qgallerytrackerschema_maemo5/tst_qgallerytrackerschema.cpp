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

#include <maemo5/qgallerytrackerschema_p.h>

#include <qdocumentgallery.h>
#include <maemo5/qgallerytrackerlistcolumn_p.h>
#include <maemo5/qgallerytrackerresultset_p.h>
#include <maemo5/qgallerytrackertyperesultset_p.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

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
#define QT_FILE_QUERY_STRING_POSITION 5

#define QT_AGGREGATE_QUERY_ARGUMENTS_COUNT 6
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
    void uriFromItemId_data();
    void uriFromItemId();
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
    void queryResponseRootItem_data();
    void queryResponseRootItem();
    void queryResponseFilter_data();
    void queryResponseFilter();
    void prepareValidItemQueryResponse_data();
    void prepareValidItemQueryResponse();
    void prepareInvalidQueryResponse_data();
    void prepareInvalidQueryResponse();

private:
    QGalleryDBusInterfacePointer daemonInterface() { return m_daemonInterface; }
    QGalleryDBusInterfacePointer metaDataInterface() { return m_metaDataInterface; }
    QGalleryDBusInterfacePointer searchInterface() { return m_searchInterface; }
    QGalleryDBusInterfacePointer fileInterface() { return m_fileInterface; }

    QGalleryDBusInterfacePointer m_daemonInterface;
    QGalleryDBusInterfacePointer m_metaDataInterface;
    QGalleryDBusInterfacePointer m_searchInterface;
    QGalleryDBusInterfacePointer m_fileInterface;
};

void tst_QGalleryTrackerSchema::initTestCase()
{
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
    QTest::newRow("Album")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("Album")
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
    QTest::newRow("Album")
            << QString::fromLatin1("album::Self Titled/Greatest Hits")
            << QString::fromLatin1("Album")
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

void tst_QGalleryTrackerSchema::uriFromItemId_data()
{
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QString>("uri");
    QTest::addColumn<int>("result");

    QTest::newRow("File")
            << QString::fromLatin1("file::/path/to/file.ext")
            << QString::fromLatin1("/path/to/file.ext")
            << int(QGalleryQueryRequest::Succeeded);
    QTest::newRow("Image")
            << QString::fromLatin1("file::/path/to/image.png")
            << QString::fromLatin1("/path/to/image.png")
            << int(QGalleryQueryRequest::Succeeded);
    QTest::newRow("Album")
            << QString::fromLatin1("album::Self Titled/Greatest Hits")
            << QString()
            << int(QGalleryQueryRequest::InvalidItemError);
    QTest::newRow("Turtle")
            << QString::fromLatin1("turtle::its/a/turtle")
            << QString()
            << int(QGalleryQueryRequest::InvalidItemError);
}

void tst_QGalleryTrackerSchema::uriFromItemId()
{
    QFETCH(QString, itemId);
    QFETCH(QString, uri);
    QFETCH(int, result);

    int error = QGalleryQueryRequest::Succeeded;

    QCOMPARE(QGalleryTrackerSchema::uriFromItemId(&error, itemId), uri);
    QCOMPARE(error, result);
}

void tst_QGalleryTrackerSchema::serviceUpdateId_data()
{
    QTest::addColumn<QString>("service");
    QTest::addColumn<int>("updateId");

    QTest::newRow("Files")
            << QString::fromLatin1("Files")
            << 0x01;
    QTest::newRow("Other")
            << QString::fromLatin1("Other")
            << 0x01;
    QTest::newRow("Text")
            << QString::fromLatin1("Text")
            << 0x80;
    QTest::newRow("Development")
            << QString::fromLatin1("Development")
            << 0x80;
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
            << QLatin1String("copyright")
            << QLatin1String("fileName")
            << QLatin1String("path")
            << QLatin1String("filePath")
            << QLatin1String("url")
            << QLatin1String("fileSize")
            << QLatin1String("language")
            << QLatin1String("lastAccessed")
            << QLatin1String("lastModified")
            << QLatin1String("mimeType")
            << QLatin1String("author")
            << QLatin1String("description")
            << QLatin1String("keywords")
            << QLatin1String("rating")
            << QLatin1String("subject")
            << QLatin1String("title"));

    QTest::newRow("Album") << QString::fromLatin1("Album") << (QStringList()
            << QLatin1String("artist")
            << QLatin1String("duration")
            << QLatin1String("title")
            << QLatin1String("trackCount"));

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
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter);
    QTest::newRow("Audio.albumTitle")
            << QString::fromLatin1("Audio")
            << QString::fromLatin1("albumTitle")
            << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    QTest::newRow("Album.duration")
            << QString::fromLatin1("Album")
            << QString::fromLatin1("duration")
            << QGalleryProperty::Attributes(QGalleryProperty::CanRead);
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
    QCOMPARE(schema.prepareTypeResponse(&arguments, this), int(QGalleryAbstractRequest::Succeeded));

    QCOMPARE(arguments.accumulative, accumulative);
    QCOMPARE(arguments.updateMask, updateMask);
    QCOMPARE(arguments.queryInterface, queryInterface);
    QCOMPARE(arguments.queryMethod, queryMethod);
    QCOMPARE(arguments.queryArguments, queryArguments);
}

void tst_QGalleryTrackerSchema::prepareInvalidTypeResponse_data()
{
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<int>("result");

    QTest::newRow("Turtle")
            << QString::fromLatin1("Turtle")
            << int(QGalleryAbstractRequest::ItemTypeError);
}

void tst_QGalleryTrackerSchema::prepareInvalidTypeResponse()
{
    QFETCH(QString, itemType);
    QFETCH(int, result);

    QGalleryTrackerTypeResultSetArguments arguments;

    QGalleryTrackerSchema schema(itemType);
    QCOMPARE(schema.prepareTypeResponse(&arguments, this), result);
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
    QTest::addColumn<QStringList>("queriedPropertyNames");
    QTest::addColumn<QStringList>("fieldNames");
    QTest::addColumn<QVector<QGalleryProperty::Attributes> >("propertyAttributes");
    QTest::addColumn<QVector<QVariant::Type> >("propertyTypes");
    QTest::addColumn<QStringList>("values");
    QTest::addColumn<QVector<QVariant> >("parsedValues");
    QTest::addColumn<QVector<QVariant> >("compositeValues");
    QTest::addColumn<QVector<int> >("aliasColumns");
    QTest::addColumn<QVector<int> >("resourceKeys");


    QTest::newRow("All Files")
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
                    << false)
            << QStringList()
            << QStringList()
            << QVector<QGalleryProperty::Attributes>()
            << QVector<QVariant::Type>()
            << QStringList()
            << QVector<QVariant>()
            << QVector<QVariant>()
            << QVector<int>()
            << QVector<int>();
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
    QFETCH(QStringList, queriedPropertyNames);
    QFETCH(QStringList, fieldNames);
    QFETCH(QVector<QGalleryProperty::Attributes>, propertyAttributes);
    QFETCH(QVector<QVariant::Type>, propertyTypes);
    QFETCH(QStringList, values);
    QFETCH(QVector<QVariant>, parsedValues);
    QFETCH(QVector<QVariant>, compositeValues);
    QFETCH(QVector<int>, aliasColumns);
    QFETCH(QVector<int>, resourceKeys);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromItemId(itemId.toString());
    QCOMPARE(
            schema.prepareItemResponse(&arguments, this, itemId.toString(), propertyNames),
            int(QGalleryAbstractRequest::Succeeded));

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

    QCOMPARE(arguments.propertyNames, queriedPropertyNames);
    QCOMPARE(arguments.fieldNames, fieldNames);
    QCOMPARE(arguments.propertyAttributes, propertyAttributes);
    QCOMPARE(arguments.propertyTypes, propertyTypes);

    QCOMPARE(values.count(), parsedValues.count()); // Verify test data.
    QCOMPARE(values.count(), arguments.valueColumns.count());
    for (int i = 0; i < arguments.valueColumns.count(); ++i) {
        QCOMPARE(arguments.valueColumns.at(i)->toVariant(values.at(i)), parsedValues.at(i));
    }

    QCOMPARE(compositeValues.count(), arguments.compositeColumns.count());
    for (int i = 0; i < arguments.compositeColumns.count(); ++i) {
        QCOMPARE(arguments.compositeColumns.at(i)->value(row.constBegin()), parsedValues.at(i));
    }

    QCOMPARE(arguments.aliasColumns, aliasColumns);
    QCOMPARE(arguments.sortCriteria, QVector<QGalleryTrackerSortCriteria>());
    QCOMPARE(arguments.resourceKeys, resourceKeys);
}

void tst_QGalleryTrackerSchema::prepareInvalidItemResponse_data()
{
    QTest::addColumn<QString>("itemId");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<int>("result");

    QTest::newRow("Invalid Type")
            << QString::fromLatin1("turtle::its/a/turtle")
            << QStringList()
            << int(QGalleryAbstractRequest::InvalidItemError);

    QTest::newRow("Relative file path")
            << QString::fromLatin1("file::file.ext")
            << QStringList()
            << int(QGalleryAbstractRequest::InvalidItemError);
}

void tst_QGalleryTrackerSchema::prepareInvalidItemResponse()
{
    QFETCH(QString, itemId);
    QFETCH(QStringList, propertyNames);
    QFETCH(int, result);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema = QGalleryTrackerSchema::fromItemId(itemId);
    QCOMPARE(schema.prepareItemResponse(&arguments, this, itemId, propertyNames), result);
}

void tst_QGalleryTrackerSchema::queryResponseRootItem_data()
{
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<int>("argumentCount");
    QTest::addColumn<int>("queryStringIndex");
    QTest::addColumn<QString>("queryString");

    QTest::newRow("Folder, All File Descendents")
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

    QTest::newRow("Folder, Direct File Descendents")
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

    QTest::newRow("Album, All Audio Descendents")
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

    QTest::newRow("Album, All Audio Descendents, No Album")
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

    QTest::newRow("Album, All Audio Descendents, Album with slash")
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

    QTest::newRow("Album, Direct Audio Descendents")
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

    QTest::newRow("Album Artist, All Audio Descendents")
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

    QTest::newRow("Album Artist, Direct Audio Descendents")
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

    QTest::newRow("Album Artist, All Album Descendents")
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

    QTest::newRow("Album Artist, Direct Album Descendents")
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

    QTest::newRow("Artist, All Audio Descendents")
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

    QTest::newRow("Artist, Direct Audio Descendents")
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

    QTest::newRow("Artist, All Album Descendents")
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

    QTest::newRow("Artist, Direct Album Descendents")
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

    QTest::newRow("Audio Genre, All Audio Descendents")
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

    QTest::newRow("Audio Genre, Direct Audio Descendents")
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

    QTest::newRow("Audio Genre, All Album Descendents")
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

    QTest::newRow("Audio Genre, Direct Album Descendents")
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

    QTest::newRow("Photo Album, All Image Descendents")
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

    QTest::newRow("Photo Album, Direct Image Descendents")
            << QString::fromLatin1("Audio")
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
            int(QGalleryAbstractRequest::Succeeded));

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
        QGalleryFilter filter = QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String("file."), QGalleryFilter::StartsWith);

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
        QGalleryFilter filter = QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String(".ext"), QGalleryFilter::EndsWith);

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
        QGalleryFilter filter = QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String("ext"), QGalleryFilter::Contains);

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
        QGalleryFilter filter = QGalleryMetaDataFilter(
                QDocumentGallery::fileName, QLatin1String("file*ext"), QGalleryFilter::Wildcard);

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
        QGalleryFilter filter = QGalleryMetaDataFilter(
                QDocumentGallery::fileName,
                QLatin1String("(file|document).ext"),
                QGalleryFilter::RegExp);

        QTest::newRow("File.fileName.regExp((file|document).ext")
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

        QTest::newRow("Album.title == Greatest Hits")
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

        QTest::newRow("Album.title == Greatest Hits, Belonging to AlbumArtist")
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
            int(QGalleryAbstractRequest::Succeeded));

    QCOMPARE(arguments.queryArguments.count(), argumentCount);
    QCOMPARE(arguments.queryArguments.at(queryStringIndex), QVariant(queryString));
}

void tst_QGalleryTrackerSchema::prepareValidItemQueryResponse_data()
{
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QGalleryFilter>("filter");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QStringList>("sortPropertyNames");
    QTest::addColumn<QVector<QVariant> >("row");
    QTest::addColumn<QVariant>("itemId");
    QTest::addColumn<QVariant>("itemUrl");
    QTest::addColumn<QVariant>("itemType");
    QTest::addColumn<int>("updateMask");
    QTest::addColumn<int>("tableWidth");
    QTest::addColumn<int>("compositeOffset");
    QTest::addColumn<QString>("service");
    QTest::addColumn<QStringList>("fieldNames");
    QTest::addColumn<QString>("queryString");
    QTest::addColumn<QStringList>("sortFieldNames");
    QTest::addColumn<bool>("sortDescending");
    QTest::addColumn<QStringList>("queriedPropertyNames");
    QTest::addColumn<QVector<QGalleryProperty::Attributes> >("propertyAttributes");
    QTest::addColumn<QVector<QVariant::Type> >("propertyTypes");
    QTest::addColumn<QStringList>("values");
    QTest::addColumn<QVector<QVariant> >("compositeValues");
    QTest::addColumn<QVector<int> >("aliasColumns");
    QTest::addColumn<QVector<QGalleryTrackerSortCriteria> >("sortCriteria");
    QTest::addColumn<QVector<int> >("resourceKeys");

    QTest::newRow("All Files")
            << QString() // rootItem
            << QGalleryQueryRequest::AllDescendants // scope
            << QString::fromLatin1("File") // rootType
            << QGalleryFilter() // filter
            << QStringList() // propertyNames
            << QStringList() // sortPropertyNames
            << (QVector<QVariant>()  // row data
                    << QLatin1String("/path/to/file.ext") // uri
                    << QLatin1String("Files")) // service
            << QVariant(QLatin1String("file::/path/to/file.ext")) // itemId
            << QVariant(QUrl(QLatin1String("file:///path/to/file.ext"))) // itemUrl
            << QVariant(QLatin1String("File")) // itemType
            << 0xFF // updateMask
            << 2 // tableWidth
            << 2 // compositeOffset
            << QString::fromLatin1("Files") // service
            << QStringList()    // fieldNames
            << QString() // rdfq query
            << QStringList() // sort field names
            << false // sort descending
            << QStringList() // queried property names
            << QVector<QGalleryProperty::Attributes>() // property attributes
            << QVector<QVariant::Type>() // property types
            << QStringList() // row values
            << QVector<QVariant>() // composite values
            << QVector<int>() // alias columns
            << QVector<QGalleryTrackerSortCriteria>() // sort order
            << QVector<int>(); // resource keys

    QTest::newRow("Images")
            << QString() // rootItem
            << QGalleryQueryRequest::AllDescendants // scope
            << QString::fromLatin1("Image") // rootType
            << QGalleryFilter() // filter
            << (QStringList() // propertyNames
                    << QLatin1String("url")
                    << QLatin1String("title")
                    << QLatin1String("rating"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("url")
                    << QLatin1String("-rating")
                    << QLatin1String("+dateTaken"))
            << (QVector<QVariant>()  // row data
                    << QLatin1String("/path/to/image.png") // uri
                    << QLatin1String("Images") // // service
                    << QLatin1String("The Beach") // title
                    << 3 // rating
                    << QDateTime(QDate(2007, 12, 12), QTime(7, 29))) // dateTaken
            << QVariant(QLatin1String("image::/path/to/image.png")) // itemId
            << QVariant(QUrl(QLatin1String("file:///path/to/image.png"))) // itemUrl
            << QVariant(QLatin1String("Image")) // itemType
            << 0x10 // updateMask
            << 5 // tableWidth
            << 4 // compositeOffset
            << QString::fromLatin1("Images") // service
            << (QStringList() // field names
                    << QLatin1String("Image:Title")
                    << QLatin1String("Image:Rating")
                    << QLatin1String("Image:Date"))
            << QString() // rdfq query
            << (QStringList() // sortFieldNames
                    << QLatin1String("Image:Rating")
                    << QLatin1String("Image:Date"))
            << true // sort descending
            << (QStringList() // queried property names
                    << QLatin1String("title")
                    << QLatin1String("rating")
                    << QLatin1String("url"))
            << (QVector<QGalleryProperty::Attributes>() // property attributes
                    << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanSort | QGalleryProperty::CanFilter)
                    << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanSort | QGalleryProperty::CanFilter)
                    << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter))
            << (QVector<QVariant::Type>() // property types
                    << QVariant::String
                    << QVariant::Int
                    << QVariant::Url)
            << (QStringList() // row values
                    << QLatin1String("The Beach") // title
                    << QLatin1String("3") // rating
                    << QLatin1String("2007-12-12T07:29:00")) // dateTaken
            << (QVector<QVariant>() // composite values
                    << QVariant(QUrl(QLatin1String("file:///path/to/image.png")))) // url
            << QVector<int>() // alias columns
            << (QVector<QGalleryTrackerSortCriteria>() // sort order
                << (QGalleryTrackerSortCriteria(3, QGalleryTrackerSortCriteria::Descending | QGalleryTrackerSortCriteria::Sorted))
                << (QGalleryTrackerSortCriteria(4, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::ReverseSorted)))
            << QVector<int>(); // resource keys

    QTest::newRow("Music")
            << QString::fromLatin1("audio::/path/to/music") // rootItem
            << QGalleryQueryRequest::AllDescendants // scope
            << QString::fromLatin1("Audio") // rootType
            << QGalleryFilter() // filter
            << (QStringList() // propertyNames
                    << QLatin1String("trackNumber")
                    << QLatin1String("title")
                    << QLatin1String("turtle")
                    << QLatin1String("trackNumber")
                    << QLatin1String("duration"))
            << (QStringList() // sortPropertyNames
                    << QLatin1String("albumArtist")
                    << QLatin1String("albumTitle")
                    << QLatin1String("trackNumber"))
            << (QVector<QVariant>()  // row data
                    << QLatin1String("/path/to/music/interlude.mp3") // uri
                    << QLatin1String("Music") // // service
                    << 4 // trackNumber
                    << QLatin1String("Interlude") // title
                    << 63 // duration
                    << QLatin1String("Self Titled") // albumArtist
                    << QLatin1String("Greatest Hits")) // albumTitle
            << QVariant(QLatin1String("audio::/path/to/music/interlude.mp3")) // itemId
            << QVariant(QUrl(QLatin1String("file:///path/to/music/interlude.mp3"))) // itemUrl
            << QVariant(QLatin1String("Audio")) // itemType
            << 0x08 // updateMask
            << 7 // tableWidth
            << 5 // compositeOffset
            << QString::fromLatin1("Music") // service
            << (QStringList() // field names
                    << QLatin1String("Audio:TrackNo")
                    << QLatin1String("Audio:Title")
                    << QLatin1String("Audio:Duration")
                    << QLatin1String("Audio:AlbumArtist")
                    << QLatin1String("Audio:Album"))
            << QString::fromLatin1( // rdfq query
                    "<rdfq:Condition>"
                    "<rdfq:or>"
                    "<rdfq:equals><rdfq:Property name=\"File:Path\"/><rdf:String>/path/to/music</rdf:String></rdfq:equals>"
                    "<rdfq:startsWith><rdfq:Property name=\"File:Path\"/><rdf:String>/path/to/music/</rdf:String></rdfq:startsWith>"
                    "</rdfq:or>"
                    "</rdfq:Condition>")
            << (QStringList() // sortFieldNames
                    << QLatin1String("Audio:AlbumArtist")
                    << QLatin1String("Audio:Album")
                    << QLatin1String("Audio:TrackNo"))
            << false // sort descending
            << (QStringList() // queried property names
                << QLatin1String("trackNumber")
                << QLatin1String("title")
                << QLatin1String("duration"))
            << (QVector<QGalleryProperty::Attributes>() // property attributes
                    << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanSort | QGalleryProperty::CanFilter)
                    << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanSort | QGalleryProperty::CanFilter)
                    << (QGalleryProperty::CanRead | QGalleryProperty::CanSort | QGalleryProperty::CanFilter))
            << (QVector<QVariant::Type>() // property types
                    << QVariant::Int
                    << QVariant::String
                    << QVariant::Int)
            << (QStringList() // row values
                    << QLatin1String("4") // trackNumber
                    << QLatin1String("Interlude") // title
                    << QLatin1String("63") // duration
                    << QLatin1String("Self Titled") // albumArtist
                    << QLatin1String("Greatest Hits")) // albumTitle
            << (QVector<QVariant>()) // composite values
            << QVector<int>() // alias columns
            << (QVector<QGalleryTrackerSortCriteria>() // sort order
                    << (QGalleryTrackerSortCriteria(5, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(6, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted))
                    << (QGalleryTrackerSortCriteria(2, QGalleryTrackerSortCriteria::Ascending | QGalleryTrackerSortCriteria::Sorted)))
            << (QVector<int>() // resource keys
                    << 4); // duration
}

void tst_QGalleryTrackerSchema::prepareValidItemQueryResponse()
{
    QFETCH(QString, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(QString, rootType);
    QFETCH(QGalleryFilter, filter);
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, sortPropertyNames);
    QFETCH(QVector<QVariant>, row);
    QFETCH(QVariant, itemId);
    QFETCH(QVariant, itemUrl);
    QFETCH(QVariant, itemType);
    QFETCH(int, updateMask);
    QFETCH(int, tableWidth);
    QFETCH(int, compositeOffset);
    QFETCH(QString, service);
    QFETCH(QString, queryString);
    QFETCH(QStringList, sortFieldNames);
    QFETCH(bool, sortDescending);
    QFETCH(QStringList, queriedPropertyNames);
    QFETCH(QStringList, fieldNames);
    QFETCH(QVector<QGalleryProperty::Attributes>, propertyAttributes);
    QFETCH(QVector<QVariant::Type>, propertyTypes);
    QFETCH(QStringList, values);
    QFETCH(QVector<QVariant>, compositeValues);
    QFETCH(QVector<int>, aliasColumns);
    QFETCH(QVector<QGalleryTrackerSortCriteria>, sortCriteria);
    QFETCH(QVector<int>, resourceKeys);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments, this, scope, rootItem, filter, propertyNames, sortPropertyNames),
            int(QGalleryAbstractRequest::Succeeded));

    QVERIFY(arguments.idColumn != 0);
    QCOMPARE(arguments.idColumn->value(row.constBegin()), itemId);

    QVERIFY(arguments.urlColumn != 0);
    QCOMPARE(arguments.urlColumn->value(row.constBegin()), itemUrl);

    QVERIFY(arguments.typeColumn != 0);
    QCOMPARE(arguments.typeColumn->value(row.constBegin()), itemType);

    QCOMPARE(arguments.updateMask, updateMask);
    QCOMPARE(arguments.identityWidth, 2);
    QCOMPARE(arguments.tableWidth, tableWidth);
    QCOMPARE(arguments.valueOffset, 2);
    QCOMPARE(arguments.compositeOffset, compositeOffset);

    QCOMPARE(arguments.queryInterface, m_searchInterface);
    QCOMPARE(arguments.queryMethod, QLatin1String("Query"));
    QCOMPARE(arguments.queryArguments.count(), 9);
    QCOMPARE(arguments.queryArguments.at(0), QVariant(0u));
    QCOMPARE(arguments.queryArguments.at(1), QVariant(service));
    QCOMPARE(arguments.queryArguments.at(2), QVariant(fieldNames));
    QCOMPARE(arguments.queryArguments.at(3), QVariant(QString()));
    QCOMPARE(arguments.queryArguments.at(4), QVariant(QStringList()));
    QCOMPARE(arguments.queryArguments.at(5), QVariant(queryString));
    QCOMPARE(arguments.queryArguments.at(6), QVariant(false));
    QCOMPARE(arguments.queryArguments.at(7), QVariant(sortFieldNames));
    QCOMPARE(arguments.queryArguments.at(8), QVariant(sortDescending));

    QCOMPARE(arguments.propertyNames, queriedPropertyNames);
    QCOMPARE(arguments.fieldNames, fieldNames);
    QCOMPARE(arguments.propertyAttributes, propertyAttributes);
    QCOMPARE(arguments.propertyTypes, propertyTypes);

    QCOMPARE(values.count(), row.count() - 2); // Verify test data.
    QCOMPARE(values.count(), arguments.valueColumns.count());
    for (int i = 0; i < arguments.valueColumns.count(); ++i) {
        QCOMPARE(arguments.valueColumns.at(i)->toVariant(values.at(i)), row.at(i + 2));
    }

    QCOMPARE(compositeValues.count(), arguments.compositeColumns.count());
    for (int i = 0; i < arguments.compositeColumns.count(); ++i) {
        QCOMPARE(arguments.compositeColumns.at(i)->value(row.constBegin()), compositeValues.at(i));
    }

    QCOMPARE(arguments.aliasColumns, aliasColumns);
    QCOMPARE(arguments.sortCriteria, sortCriteria);
    QCOMPARE(arguments.resourceKeys, resourceKeys);
}

void tst_QGalleryTrackerSchema::prepareInvalidQueryResponse_data()
{
    QTest::addColumn<QString>("rootItem");
    QTest::addColumn<QGalleryQueryRequest::Scope>("scope");
    QTest::addColumn<QString>("rootType");
    QTest::addColumn<QGalleryFilter>("filter");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QStringList>("sortPropertyNames");
    QTest::addColumn<int>("result");

    QTest::addColumn<QVariant>("itemId");
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<int>("result");

    QTest::newRow("Invalid Type")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("Turtle")
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << int(QGalleryAbstractRequest::ItemTypeError);

    QTest::newRow("No Type")
            << QString()
            << QGalleryQueryRequest::AllDescendants
            << QString()
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << int(QGalleryAbstractRequest::ItemTypeError);

    QTest::newRow("Invalid Root Item Type")
            << QString::fromLatin1("turtle::/its/a/turtle")
            << QGalleryQueryRequest::AllDescendants
            << QString::fromLatin1("File")
            << QGalleryFilter()
            << QStringList()
            << QStringList()
            << int(QGalleryAbstractRequest::InvalidItemError);
}

void tst_QGalleryTrackerSchema::prepareInvalidQueryResponse()
{
    QFETCH(QString, rootItem);
    QFETCH(QGalleryQueryRequest::Scope, scope);
    QFETCH(QString, rootType);
    QFETCH(QGalleryFilter, filter);
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, sortPropertyNames);
    QFETCH(int, result);

    QGalleryTrackerResultSetArguments arguments;

    QGalleryTrackerSchema schema(rootType);

    QCOMPARE(
            schema.prepareQueryResponse(
                    &arguments, this, scope, rootItem, filter, propertyNames, sortPropertyNames),
            result);
}

QTEST_MAIN(tst_QGalleryTrackerSchema)

#include "tst_qgallerytrackerschema.moc"

