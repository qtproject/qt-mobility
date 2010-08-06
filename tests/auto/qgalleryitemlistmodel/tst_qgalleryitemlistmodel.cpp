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

#include <qgalleryitemlistmodel.h>

#include <qgalleryitemlist.h>

Q_DECLARE_METATYPE(QModelIndex)

QTM_USE_NAMESPACE

class QtTestGalleryItemList;

class tst_QGalleryItemListModel : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void itemList();
    void data();
    void flags();
    void headerData();
    void addColumn();
    void insertColumn();
    void removeColumn();
    void setRoleProperties();
    void cursorPosition();
    void itemsInserted();
    void itemsRemoved();
    void itemsMoved();
    void metaDataChanged();
    void invalidIndex();
    void hierarchy();

private:
    void populateItemList(QtTestGalleryItemList *itemList) const;

    QHash<int, QString> albumProperties;
    QHash<int, QString> titleProperties;
    QHash<int, QString> durationProperties;
    QHash<int, QString> ratingProperties;
    QHash<int, QString> turtleProperties;
};

class QtTestGalleryItemList : public QGalleryItemList
{
public:
    QtTestGalleryItemList()
        : m_outOfRangeCount(0), m_minimumPagedItems(0), m_insertIndex(0), m_insertCount(0) {}

    QStringList propertyNames() const { return m_propertyNames; }
    void setPropertyNames(const QStringList &propertyNames) {
        m_propertyNames = propertyNames; }

    int propertyKey(const QString &propertyName) const {
        return m_propertyNames.indexOf(propertyName); }

    QGalleryProperty::Attributes propertyAttributes(int key) const {
        return m_propertyAttributes.value(key); }
    void setPropertyAttributes(const QVector<QGalleryProperty::Attributes> &attributes) {
        m_propertyAttributes = attributes; }

    QVariant::Type propertyType(int) const { return QVariant::Invalid; }

    int minimumPagedItems() const { return m_minimumPagedItems; }
    void setMinimumPagedItems(int items) { m_minimumPagedItems = items; }

    int count() const { return m_rows.count() + m_outOfRangeCount; }

    QVariant id(int) const { return QVariant(); }
    QUrl url(int) const { return QUrl(); }
    QString type(int) const { return QString(); }
    QList<QGalleryResource> resources(int) const { return QList<QGalleryResource>(); }
    ItemStatus status(int index) const
    {
        ItemStatus status;
        if (index > m_rows.count())
            status |= OutOfRange;
        return status;
    }

    QVariant metaData(int index, int key) const {
        return index < m_rows.count() ? m_rows.at(index).metaData.at(key) : QVariant(); }
    void setMetaData(int index, int key, const QVariant &value)
    {
        if (index < m_rows.count()) {
            m_rows[index].metaData[key] = value;
            emit metaDataChanged(index, 1, QList<int>() << key);
        }
    }

    void beginInsertRows(int index) { m_insertIndex = index; m_insertCount = 0; }
    void addRow(const QVector<QVariant> &metaData) {
        m_rows.insert(m_insertIndex + m_insertCount++, Row(metaData)); }
    void endInsertRows() { emit inserted(m_insertIndex, m_insertCount); }

    void appendOutOfRangeItems(int count) {
        m_outOfRangeCount = count; emit inserted(m_rows.count(), count); }

    void removeRows(int index, int count) {
        m_rows.remove(index, count); emit removed(index, count); }

    using QGalleryItemList::metaDataChanged;
    using QGalleryItemList::moved;

private:
    struct Row
    {
        Row() {}
        Row(const QVector<QVariant> &metaData) : metaData(metaData) {}
        QVector<QVariant> metaData;
    };

    QStringList m_propertyNames;
    QVector<QGalleryProperty::Attributes> m_propertyAttributes;
    QVector<Row> m_rows;
    int m_outOfRangeCount;
    int m_minimumPagedItems;
    int m_insertIndex;
    int m_insertCount;
};

void tst_QGalleryItemListModel::initTestCase()
{
    qRegisterMetaType<QModelIndex>();

    albumProperties.insert(Qt::DisplayRole, QLatin1String("albumTitle"));
    albumProperties.insert(Qt::UserRole, QLatin1String("albumArtist"));
    albumProperties.insert(Qt::UserRole + 1, QLatin1String("albumId"));

    titleProperties.insert(Qt::DisplayRole, QLatin1String("displayName"));
    titleProperties.insert(Qt::EditRole, QLatin1String("title"));

    durationProperties.insert(Qt::DisplayRole, QLatin1String("duration"));

    ratingProperties.insert(Qt::DisplayRole, QLatin1String("rating"));

    turtleProperties.insert(Qt::DisplayRole, QLatin1String("turtle"));
}

void tst_QGalleryItemListModel::populateItemList(QtTestGalleryItemList *itemList) const
{
    itemList->setPropertyNames(QStringList()
            << QLatin1String("displayName")
            << QLatin1String("title")
            << QLatin1String("albumTitle")
            << QLatin1String("albumArtist")
            << QLatin1String("albumId")
            << QLatin1String("duration")
            << QLatin1String("rating"));
    itemList->setPropertyAttributes(QVector<QGalleryProperty::Attributes>()
            << (QGalleryProperty::CanRead)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite)
            << (QGalleryProperty::CanRead)
            << (QGalleryProperty::CanRead)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite)
            << (QGalleryProperty::CanRead)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanWrite));

    itemList->beginInsertRows(0);
    itemList->addRow(QVector<QVariant>()
            << QLatin1String("Interlude")
            << QLatin1String("Interlude")
            << QLatin1String("Greatest Hits")
            << QLatin1String("Self Titled")
            << QLatin1String("album:SelfTitled:GreatestHits")
            << 120100
            << 4);
    itemList->addRow(QVector<QVariant>()
            << QLatin1String("beep.wav")
            << QVariant()
            << QVariant()
            << QVariant()
            << QVariant()
            << 600
            << QVariant());
    itemList->endInsertRows();
}

void tst_QGalleryItemListModel::itemList()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);

    QSignalSpy insertSpy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy removeSpy(&model, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    QVERIFY(model.itemList() == 0);
    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 5);

    QCOMPARE(model.index(-1, -1).isValid(), false);
    QCOMPARE(model.index(-1,  0).isValid(), false);
    QCOMPARE(model.index(-1,  2).isValid(), false);
    QCOMPARE(model.index(-1,  3).isValid(), false);
    QCOMPARE(model.index(-1,  4).isValid(), false);
    QCOMPARE(model.index(-1,  5).isValid(), false);
    QCOMPARE(model.index( 0, -1).isValid(), false);
    QCOMPARE(model.index( 0,  0).isValid(), false);
    QCOMPARE(model.index( 0,  2).isValid(), false);
    QCOMPARE(model.index( 0,  3).isValid(), false);
    QCOMPARE(model.index( 0,  4).isValid(), false);
    QCOMPARE(model.index( 0,  5).isValid(), false);
    QCOMPARE(model.index( 1, -1).isValid(), false);
    QCOMPARE(model.index( 1,  0).isValid(), false);
    QCOMPARE(model.index( 1,  2).isValid(), false);
    QCOMPARE(model.index( 1,  3).isValid(), false);
    QCOMPARE(model.index( 1,  4).isValid(), false);
    QCOMPARE(model.index( 1,  5).isValid(), false);
    QCOMPARE(model.index( 4, -1).isValid(), false);
    QCOMPARE(model.index( 4,  0).isValid(), false);
    QCOMPARE(model.index( 4,  2).isValid(), false);
    QCOMPARE(model.index( 4,  3).isValid(), false);
    QCOMPARE(model.index( 4,  4).isValid(), false);
    QCOMPARE(model.index( 4,  5).isValid(), false);
    QCOMPARE(model.index( 5, -1).isValid(), false);
    QCOMPARE(model.index( 5,  0).isValid(), false);
    QCOMPARE(model.index( 5,  2).isValid(), false);
    QCOMPARE(model.index( 5,  3).isValid(), false);
    QCOMPARE(model.index( 5,  4).isValid(), false);
    QCOMPARE(model.index( 5,  5).isValid(), false);

    model.setItemList(&itemList);
    QVERIFY(model.itemList() == &itemList);

    QCOMPARE(model.rowCount(), 5);
    QCOMPARE(model.columnCount(), 5);

    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 0);
    QCOMPARE(insertSpy.last().at(2).toInt(), 4);

    QCOMPARE(model.index(-1, -1).isValid(), false);
    QCOMPARE(model.index(-1,  0).isValid(), false);
    QCOMPARE(model.index(-1,  2).isValid(), false);
    QCOMPARE(model.index(-1,  3).isValid(), false);
    QCOMPARE(model.index(-1,  4).isValid(), false);
    QCOMPARE(model.index(-1,  5).isValid(), false);
    QCOMPARE(model.index( 0, -1).isValid(), false);
    QCOMPARE(model.index( 0,  0).isValid(), true);
    QCOMPARE(model.index( 0,  2).isValid(), true);
    QCOMPARE(model.index( 0,  3).isValid(), true);
    QCOMPARE(model.index( 0,  4).isValid(), true);
    QCOMPARE(model.index( 0,  5).isValid(), false);
    QCOMPARE(model.index( 1, -1).isValid(), false);
    QCOMPARE(model.index( 1,  0).isValid(), true);
    QCOMPARE(model.index( 1,  2).isValid(), true);
    QCOMPARE(model.index( 1,  3).isValid(), true);
    QCOMPARE(model.index( 1,  4).isValid(), true);
    QCOMPARE(model.index( 1,  5).isValid(), false);
    QCOMPARE(model.index( 4, -1).isValid(), false);
    QCOMPARE(model.index( 4,  0).isValid(), true);
    QCOMPARE(model.index( 4,  2).isValid(), true);
    QCOMPARE(model.index( 4,  3).isValid(), true);
    QCOMPARE(model.index( 4,  4).isValid(), true);
    QCOMPARE(model.index( 4,  5).isValid(), false);
    QCOMPARE(model.index( 5, -1).isValid(), false);
    QCOMPARE(model.index( 5,  0).isValid(), false);
    QCOMPARE(model.index( 5,  2).isValid(), false);
    QCOMPARE(model.index( 5,  3).isValid(), false);
    QCOMPARE(model.index( 5,  4).isValid(), false);
    QCOMPARE(model.index( 5,  5).isValid(), false);

    model.setItemList(0);
    QVERIFY(model.itemList() == 0);

    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 5);

    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 0);
    QCOMPARE(removeSpy.last().at(2).toInt(), 4);

    QCOMPARE(model.index(-1, -1).isValid(), false);
    QCOMPARE(model.index(-1,  0).isValid(), false);
    QCOMPARE(model.index(-1,  2).isValid(), false);
    QCOMPARE(model.index(-1,  3).isValid(), false);
    QCOMPARE(model.index(-1,  4).isValid(), false);
    QCOMPARE(model.index(-1,  5).isValid(), false);
    QCOMPARE(model.index( 0, -1).isValid(), false);
    QCOMPARE(model.index( 0,  0).isValid(), false);
    QCOMPARE(model.index( 0,  2).isValid(), false);
    QCOMPARE(model.index( 0,  3).isValid(), false);
    QCOMPARE(model.index( 0,  4).isValid(), false);
    QCOMPARE(model.index( 0,  5).isValid(), false);
    QCOMPARE(model.index( 1, -1).isValid(), false);
    QCOMPARE(model.index( 1,  0).isValid(), false);
    QCOMPARE(model.index( 1,  2).isValid(), false);
    QCOMPARE(model.index( 1,  3).isValid(), false);
    QCOMPARE(model.index( 1,  4).isValid(), false);
    QCOMPARE(model.index( 1,  5).isValid(), false);
    QCOMPARE(model.index( 4, -1).isValid(), false);
    QCOMPARE(model.index( 4,  0).isValid(), false);
    QCOMPARE(model.index( 4,  2).isValid(), false);
    QCOMPARE(model.index( 4,  3).isValid(), false);
    QCOMPARE(model.index( 4,  4).isValid(), false);
    QCOMPARE(model.index( 4,  5).isValid(), false);
    QCOMPARE(model.index( 5, -1).isValid(), false);
    QCOMPARE(model.index( 5,  0).isValid(), false);
    QCOMPARE(model.index( 5,  2).isValid(), false);
    QCOMPARE(model.index( 5,  3).isValid(), false);
    QCOMPARE(model.index( 5,  4).isValid(), false);
    QCOMPARE(model.index( 5,  5).isValid(), false);
}

void tst_QGalleryItemListModel::data()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);
    model.setItemList(&itemList);

    QModelIndex index;

    index = model.index(0, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Greatest Hits")));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant(QLatin1String("Self Titled")));
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:SelfTitled:GreatestHits")));

    index = model.index(0, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 2);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(120100));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 3);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(4));
    QCOMPARE(index.data(Qt::EditRole), QVariant(4));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 4);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(1, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    QCOMPARE(model.setData(index, QLatin1String("Noise"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());

    QCOMPARE(model.setData(index, QLatin1String("Noise"), Qt::EditRole), false);
    QCOMPARE(index.data(Qt::EditRole), QVariant());

    QCOMPARE(model.setData(index, QLatin1String("Effects People"), Qt::UserRole), false);
    QCOMPARE(index.data(Qt::UserRole), QVariant());

    QCOMPARE(model.setData(index, QLatin1String("album:EffectsPeople:Noise"), Qt::UserRole + 1), true);
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:EffectsPeople:Noise")));

    index = model.index(1, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));

    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::EditRole), true);
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Beep Sound")));

    index = model.index(1, 2);
    QCOMPARE(index.isValid(), true);
    QCOMPARE(index.flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(600));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    QCOMPARE(model.setData(index, 1000, Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(600));

    QCOMPARE(model.setData(index, 1000, Qt::EditRole), false);
    QCOMPARE(index.data(Qt::EditRole), QVariant());

    index = model.index(1, 3);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    QCOMPARE(model.setData(index, 2, Qt::DisplayRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(2));
    QCOMPARE(index.data(Qt::EditRole), QVariant(2));

    QCOMPARE(model.setData(index, 5, Qt::EditRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(5));
    QCOMPARE(index.data(Qt::EditRole), QVariant(5));

    index = model.index(1, 4);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    // Out Of Range.
    index = model.index(4, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(4, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(4, 2);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(4, 3);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(4, 4);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());
}

void tst_QGalleryItemListModel::flags()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);
    model.setItemList(&itemList);

    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 1).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 2).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 3).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 4).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QCOMPARE(model.index(1, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);;
    QCOMPARE(model.index(1, 1).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(1, 2).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(1, 3).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(1, 4).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QCOMPARE(model.index(4, 0).flags(), Qt::ItemFlags());
    QCOMPARE(model.index(4, 1).flags(), Qt::ItemFlags());
    QCOMPARE(model.index(4, 2).flags(), Qt::ItemFlags());
    QCOMPARE(model.index(4, 3).flags(), Qt::ItemFlags());
    QCOMPARE(model.index(4, 4).flags(), Qt::ItemFlags());
}

void tst_QGalleryItemListModel::headerData()
{
    QGalleryItemListModel model;

    model.addColumn(QLatin1String("title"));
    model.addColumn(QLatin1String("artist"));
    model.addColumn(QLatin1String("duration"));

    QCOMPARE(model.headerData(-1, Qt::Horizontal, Qt::DisplayRole), QVariant());
    QCOMPARE(model.headerData(3, Qt::Horizontal, Qt::DisplayRole), QVariant());
    QCOMPARE(model.headerData(-1, Qt::Vertical, Qt::DisplayRole), QVariant());
    QCOMPARE(model.headerData(3, Qt::Vertical, Qt::DisplayRole), QVariant());
    QCOMPARE(model.headerData(0, Qt::Vertical, Qt::DisplayRole), QVariant());
    QCOMPARE(model.headerData(2, Qt::Vertical, Qt::DisplayRole), QVariant());

    QCOMPARE(model.setHeaderData(-1, Qt::Horizontal, QLatin1String("Hello"), Qt::DisplayRole), false);
    QCOMPARE(model.setHeaderData(3, Qt::Horizontal, QLatin1String("Hello"), Qt::DisplayRole), false);
    QCOMPARE(model.setHeaderData(-1, Qt::Vertical, QLatin1String("Hello"), Qt::DisplayRole), false);
    QCOMPARE(model.setHeaderData(3, Qt::Vertical, QLatin1String("Hello"), Qt::DisplayRole), false);
    QCOMPARE(model.setHeaderData(0, Qt::Vertical, QLatin1String("Hello"), Qt::DisplayRole), false);
    QCOMPARE(model.setHeaderData(2, Qt::Vertical, QLatin1String("Hello"), Qt::DisplayRole), false);

    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QLatin1String("Title"), Qt::DisplayRole), true);
    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole), QVariant(QLatin1String("Title")));
    QCOMPARE(model.setHeaderData(1, Qt::Horizontal, QLatin1String("Artist"), Qt::DisplayRole), true);
    QCOMPARE(model.headerData(1, Qt::Horizontal, Qt::DisplayRole), QVariant(QLatin1String("Artist")));
    QCOMPARE(model.setHeaderData(2, Qt::Horizontal, QLatin1String("Duration"), Qt::DisplayRole), true);
    QCOMPARE(model.headerData(2, Qt::Horizontal, Qt::DisplayRole), QVariant(QLatin1String("Duration")));

    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::EditRole), QVariant(QLatin1String("Title")));
    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QLatin1String("Edit"), Qt::EditRole), true);
    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::EditRole), QVariant(QLatin1String("Edit")));

    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::UserRole), QVariant());
    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QLatin1String("User"), Qt::UserRole), true);
    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::UserRole), QVariant(QLatin1String("User")));

    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::DisplayRole), QVariant(QLatin1String("Edit")));
}

void tst_QGalleryItemListModel::addColumn()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;
    model.setItemList(&itemList);

    QSignalSpy columnSpy(&model, SIGNAL(columnsInserted(QModelIndex,int,int)));

    QCOMPARE(model.columnCount(), 0);

    QCOMPARE(model.index(0, 0).isValid(), false);
    QCOMPARE(model.index(0, 1).isValid(), false);
    QCOMPARE(model.index(0, 2).isValid(), false);
    QCOMPARE(model.index(0, 3).isValid(), false);
    QCOMPARE(model.index(0, 4).isValid(), false);

    model.addColumn(albumProperties);
    QCOMPARE(model.columnCount(), 1);
    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QLatin1String("Album")), true);

    QCOMPARE(columnSpy.count(), 1);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 0);
    QCOMPARE(columnSpy.last().at(2).toInt(), 0);

    model.addColumn(titleProperties);
    QCOMPARE(model.columnCount(), 2);
    QCOMPARE(model.setHeaderData(1, Qt::Horizontal, QLatin1String("Title")), true);

    QCOMPARE(columnSpy.count(), 2);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 1);
    QCOMPARE(columnSpy.last().at(2).toInt(), 1);

    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    QCOMPARE(model.columnCount(), 3);
    QCOMPARE(model.setHeaderData(2, Qt::Horizontal, QLatin1String("Duration")), true);

    QCOMPARE(columnSpy.count(), 3);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 2);
    QCOMPARE(columnSpy.last().at(2).toInt(), 2);

    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    QCOMPARE(model.columnCount(), 4);
    QCOMPARE(model.setHeaderData(3, Qt::Horizontal, QLatin1String("Rating")), true);

    QCOMPARE(columnSpy.count(), 4);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 3);
    QCOMPARE(columnSpy.last().at(2).toInt(), 3);

    model.addColumn(QLatin1String("turtle"));
    QCOMPARE(model.columnCount(), 5);
    QCOMPARE(model.setHeaderData(4, Qt::Horizontal, QLatin1String("Turtle")), true);

    QCOMPARE(columnSpy.count(), 5);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 4);
    QCOMPARE(columnSpy.last().at(2).toInt(), 4);

    QCOMPARE(model.roleProperties(0), albumProperties);
    QCOMPARE(model.roleProperties(1), titleProperties);
    QCOMPARE(model.roleProperties(2), durationProperties);
    QCOMPARE(model.roleProperties(3), ratingProperties);
    QCOMPARE(model.roleProperties(4), turtleProperties);

    QCOMPARE(model.index(0, 0).isValid(), true);
    QCOMPARE(model.index(0, 1).isValid(), true);
    QCOMPARE(model.index(0, 2).isValid(), true);
    QCOMPARE(model.index(0, 3).isValid(), true);
    QCOMPARE(model.index(0, 4).isValid(), true);

    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 1).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 2).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 3).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 4).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant(QLatin1String("Album")));
    QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant(QLatin1String("Title")));
    QCOMPARE(model.headerData(2, Qt::Horizontal), QVariant(QLatin1String("Duration")));
    QCOMPARE(model.headerData(3, Qt::Horizontal), QVariant(QLatin1String("Rating")));
    QCOMPARE(model.headerData(4, Qt::Horizontal), QVariant(QLatin1String("Turtle")));

    QModelIndex index;

    index = model.index(0, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Greatest Hits")));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant(QLatin1String("Self Titled")));
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:SelfTitled:GreatestHits")));

    index = model.index(0, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 2);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(120100));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 3);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(4));
    QCOMPARE(index.data(Qt::EditRole), QVariant(4));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 4);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(1, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Noise"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());

    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Noise"), Qt::EditRole), false);
    QCOMPARE(index.data(Qt::EditRole), QVariant());

    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Effects People"), Qt::UserRole), false);
    QCOMPARE(index.data(Qt::UserRole), QVariant());

    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("album:EffectsPeople:Noise"), Qt::UserRole + 1), true);
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:EffectsPeople:Noise")));

    index = model.index(1, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));
    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));

    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::EditRole), true);
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Beep Sound")));

    index = model.index(1, 2);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(600));
    QCOMPARE(model.setData(index, 1000, Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(600));

    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(model.setData(index, 1000, Qt::EditRole), false);
    QCOMPARE(index.data(Qt::EditRole), QVariant());

    index = model.index(1, 3);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(model.setData(index, 2, Qt::DisplayRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(2));
    QCOMPARE(index.data(Qt::EditRole), QVariant(2));
    QCOMPARE(model.setData(index, 5, Qt::EditRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(5));
    QCOMPARE(index.data(Qt::EditRole), QVariant(5));
}

void tst_QGalleryItemListModel::insertColumn()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;
    model.setItemList(&itemList);

    QSignalSpy columnSpy(&model, SIGNAL(columnsInserted(QModelIndex,int,int)));

    QCOMPARE(model.columnCount(), 0);
    QCOMPARE(model.index(0, 0).isValid(), false);
    QCOMPARE(model.index(0, 1).isValid(), false);
    QCOMPARE(model.index(0, 2).isValid(), false);
    QCOMPARE(model.index(0, 3).isValid(), false);
    QCOMPARE(model.index(0, 4).isValid(), false);

    model.insertColumn(0, albumProperties);
    QCOMPARE(model.columnCount(), 1);
    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QLatin1String("Album")), true);
    QCOMPARE(model.index(0, 0).data(Qt::DisplayRole), QVariant(QLatin1String("Greatest Hits")));

    QCOMPARE(columnSpy.count(), 1);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 0);
    QCOMPARE(columnSpy.last().at(2).toInt(), 0);

    model.insertColumn(0, titleProperties);
    QCOMPARE(model.columnCount(), 2);
    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QLatin1String("Title")), true);

    QCOMPARE(columnSpy.count(), 2);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 0);
    QCOMPARE(columnSpy.last().at(2).toInt(), 0);

    model.insertColumn(1, QLatin1String("duration"), Qt::DisplayRole);
    QCOMPARE(model.columnCount(), 3);
    QCOMPARE(model.setHeaderData(1, Qt::Horizontal, QLatin1String("Duration")), true);

    QCOMPARE(columnSpy.count(), 3);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 1);
    QCOMPARE(columnSpy.last().at(2).toInt(), 1);

    model.insertColumn(2, QLatin1String("rating"), Qt::DisplayRole);
    QCOMPARE(model.columnCount(), 4);
    QCOMPARE(model.setHeaderData(2, Qt::Horizontal, QLatin1String("Rating")), true);

    QCOMPARE(columnSpy.count(), 4);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 2);
    QCOMPARE(columnSpy.last().at(2).toInt(), 2);

    model.insertColumn(4, QLatin1String("turtle"));
    QCOMPARE(model.columnCount(), 5);
    QCOMPARE(model.setHeaderData(4, Qt::Horizontal, QLatin1String("Turtle")), true);

    QCOMPARE(columnSpy.count(), 5);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 4);
    QCOMPARE(columnSpy.last().at(2).toInt(), 4);

    QCOMPARE(model.roleProperties(0), titleProperties);
    QCOMPARE(model.roleProperties(1), durationProperties);
    QCOMPARE(model.roleProperties(2), ratingProperties);
    QCOMPARE(model.roleProperties(3), albumProperties);
    QCOMPARE(model.roleProperties(4), turtleProperties);

    QCOMPARE(model.index(0, 0).isValid(), true);
    QCOMPARE(model.index(0, 1).isValid(), true);
    QCOMPARE(model.index(0, 2).isValid(), true);
    QCOMPARE(model.index(0, 3).isValid(), true);
    QCOMPARE(model.index(0, 4).isValid(), true);

    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 1).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 2).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 3).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 4).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant(QLatin1String("Title")));
    QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant(QLatin1String("Duration")));
    QCOMPARE(model.headerData(2, Qt::Horizontal), QVariant(QLatin1String("Rating")));
    QCOMPARE(model.headerData(3, Qt::Horizontal), QVariant(QLatin1String("Album")));
    QCOMPARE(model.headerData(4, Qt::Horizontal), QVariant(QLatin1String("Turtle")));

    QModelIndex index;

    index = model.index(0, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(120100));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 2);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(4));
    QCOMPARE(index.data(Qt::EditRole), QVariant(4));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 3);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Greatest Hits")));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant(QLatin1String("Self Titled")));
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:SelfTitled:GreatestHits")));

    index = model.index(0, 4);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(1, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));
    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));

    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::EditRole), true);
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Beep Sound")));

    index = model.index(1, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(600));
    QCOMPARE(model.setData(index, 1000, Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(600));

    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(model.setData(index, 1000, Qt::EditRole), false);
    QCOMPARE(index.data(Qt::EditRole), QVariant());

    index = model.index(1, 2);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(model.setData(index, 2, Qt::DisplayRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(2));
    QCOMPARE(index.data(Qt::EditRole), QVariant(2));
    QCOMPARE(model.setData(index, 5, Qt::EditRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(5));
    QCOMPARE(index.data(Qt::EditRole), QVariant(5));

    index = model.index(1, 3);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Noise"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());

    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Noise"), Qt::EditRole), false);
    QCOMPARE(index.data(Qt::EditRole), QVariant());

    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Effects People"), Qt::UserRole), false);
    QCOMPARE(index.data(Qt::UserRole), QVariant());

    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("album:EffectsPeople:Noise"), Qt::UserRole + 1), true);
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:EffectsPeople:Noise")));
}

void tst_QGalleryItemListModel::removeColumn()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;

    QCOMPARE(model.index(0, 0).isValid(), false);
    QCOMPARE(model.index(0, 1).isValid(), false);
    QCOMPARE(model.index(0, 2).isValid(), false);
    QCOMPARE(model.index(0, 3).isValid(), false);
    QCOMPARE(model.index(0, 4).isValid(), false);

    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"));

    QCOMPARE(model.setHeaderData(0, Qt::Horizontal, QLatin1String("Album")), true);
    QCOMPARE(model.setHeaderData(1, Qt::Horizontal, QLatin1String("Title")), true);
    QCOMPARE(model.setHeaderData(2, Qt::Horizontal, QLatin1String("Duration")), true);
    QCOMPARE(model.setHeaderData(3, Qt::Horizontal, QLatin1String("Rating")), true);
    QCOMPARE(model.setHeaderData(4, Qt::Horizontal, QLatin1String("Turtle")), true);

    model.setItemList(&itemList);

    QSignalSpy columnSpy(&model, SIGNAL(columnsRemoved(QModelIndex,int,int)));

    model.removeColumn(4);
    QCOMPARE(columnSpy.count(), 1);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 4);
    QCOMPARE(columnSpy.last().at(2).toInt(), 4);

    model.removeColumn(0);
    QCOMPARE(columnSpy.count(), 2);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 0);
    QCOMPARE(columnSpy.last().at(2).toInt(), 0);

    model.removeColumn(1);
    QCOMPARE(columnSpy.count(), 3);
    QCOMPARE(columnSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(columnSpy.last().at(1).toInt(), 1);
    QCOMPARE(columnSpy.last().at(2).toInt(), 1);

    QCOMPARE(model.columnCount(), 2);

    QCOMPARE(model.roleProperties(0), titleProperties);
    QCOMPARE(model.roleProperties(1), ratingProperties);

    QCOMPARE(model.index(0, 0).isValid(), true);
    QCOMPARE(model.index(0, 1).isValid(), true);

    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 1).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

    QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant(QLatin1String("Title")));
    QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant(QLatin1String("Rating")));

    QModelIndex index;

    index = model.index(0, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(4));
    QCOMPARE(index.data(Qt::EditRole), QVariant(4));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(1, 0);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));
    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));

    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::EditRole), true);
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Beep Sound")));

    index = model.index(1, 1);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(model.setData(index, 2, Qt::DisplayRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(2));
    QCOMPARE(index.data(Qt::EditRole), QVariant(2));
    QCOMPARE(model.setData(index, 5, Qt::EditRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(5));
    QCOMPARE(index.data(Qt::EditRole), QVariant(5));
}

void tst_QGalleryItemListModel::setRoleProperties()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);

    QGalleryItemListModel model;
    model.setItemList(&itemList);

    QSignalSpy dataSpy(&model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    QCOMPARE(model.index(0, 0).isValid(), false);

    model.addColumn(albumProperties);
    QCOMPARE(model.index(0, 0).isValid(), true);
    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    model.setRoleProperties(0, titleProperties);
    QCOMPARE(model.roleProperties(0), titleProperties);
    QCOMPARE(model.index(0, 0).isValid(), true);
    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

    QCOMPARE(dataSpy.count(), 1);
    QCOMPARE(dataSpy.last().at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.last().at(1).value<QModelIndex>(), model.index(1, 0));

    model.setRoleProperties(0, durationProperties);
    QCOMPARE(model.roleProperties(0), durationProperties);
    QCOMPARE(model.index(0, 0).isValid(), true);
    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QCOMPARE(dataSpy.count(), 2);
    QCOMPARE(dataSpy.last().at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.last().at(1).value<QModelIndex>(), model.index(1, 0));

    model.setRoleProperties(0, ratingProperties);
    QCOMPARE(model.roleProperties(0), ratingProperties);
    QCOMPARE(model.index(0, 0).isValid(), true);
    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

    QCOMPARE(dataSpy.count(), 3);
    QCOMPARE(dataSpy.last().at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.last().at(1).value<QModelIndex>(), model.index(1, 0));

    const QHash<int, QString> emptyProperties;

    model.setRoleProperties(-1, albumProperties);
    QCOMPARE(model.roleProperties(-1), emptyProperties);
    QCOMPARE(dataSpy.count(), 3);

    model.setRoleProperties(1, albumProperties);
    QCOMPARE(model.roleProperties(-1), emptyProperties);
    QCOMPARE(dataSpy.count(), 3);

    itemList.removeRows(0, 2);

    model.setRoleProperties(0, albumProperties);
    QCOMPARE(model.roleProperties(0), albumProperties);
    QCOMPARE(dataSpy.count(), 3);
}

void tst_QGalleryItemListModel::cursorPosition()
{
    QtTestGalleryItemList itemList;
    itemList.appendOutOfRangeItems(48);

    QCOMPARE(itemList.minimumPagedItems(), 0);
    QCOMPARE(itemList.cursorPosition(), 0);

    QGalleryItemListModel model;
    model.addColumn(QLatin1String("turtle"));
    model.setItemList(&itemList);

    // Minimum paged items is 0, so auto update is effectively disabled.
    QCOMPARE(model.autoUpdateCursorPosition(), true);
    QCOMPARE(itemList.cursorPosition(), 0);

    model.index( 0, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(12, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(13, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(14, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(15, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(24, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(18, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(17, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(16, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(15, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(14, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(32, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(47, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(39, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(38, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index( 0, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);

    itemList.setMinimumPagedItems(16);
    model.setItemList(&itemList);

    model.index( 0, 0).flags(); QCOMPARE(itemList.cursorPosition(),  0);
    model.index(12, 0).flags(); QCOMPARE(itemList.cursorPosition(),  0);
    model.index(13, 0).flags(); QCOMPARE(itemList.cursorPosition(),  1);
    model.index(14, 0).flags(); QCOMPARE(itemList.cursorPosition(),  2);
    model.index(15, 0).flags(); QCOMPARE(itemList.cursorPosition(),  3);
    model.index(24, 0).flags(); QCOMPARE(itemList.cursorPosition(), 12);
    model.index(18, 0).flags(); QCOMPARE(itemList.cursorPosition(), 12);
    model.index(17, 0).flags(); QCOMPARE(itemList.cursorPosition(), 12);
    model.index(16, 0).flags(); QCOMPARE(itemList.cursorPosition(), 12);
    model.index(15, 0).flags(); QCOMPARE(itemList.cursorPosition(), 11);
    model.index(14, 0).flags(); QCOMPARE(itemList.cursorPosition(), 10);
    model.index(32, 0).flags(); QCOMPARE(itemList.cursorPosition(), 20);
    model.index(47, 0).flags(); QCOMPARE(itemList.cursorPosition(), 35);
    model.index(39, 0).flags(); QCOMPARE(itemList.cursorPosition(), 35);
    model.index(38, 0).flags(); QCOMPARE(itemList.cursorPosition(), 34);
    model.index( 0, 0).flags(); QCOMPARE(itemList.cursorPosition(),  0);

    model.setAutoUpdateCursorPosition(false);
    QCOMPARE(model.autoUpdateCursorPosition(), false);

    model.index( 0, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(12, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(13, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(14, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(15, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(24, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(18, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(17, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(16, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(15, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(14, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(32, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(47, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(39, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index(38, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
    model.index( 0, 0).flags(); QCOMPARE(itemList.cursorPosition(), 0);
}

void tst_QGalleryItemListModel::itemsInserted()
{
    QtTestGalleryItemList itemList;

    QGalleryItemListModel model;
    model.setItemList(&itemList);

    QSignalSpy insertSpy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QCOMPARE(model.rowCount(), 0);

    itemList.beginInsertRows(0);
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.endInsertRows();

    QCOMPARE(model.rowCount(), 4);

    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 0);
    QCOMPARE(insertSpy.last().at(2).toInt(), 3);

    itemList.beginInsertRows(2);
    itemList.addRow(QVector<QVariant>());
    itemList.endInsertRows();

    QCOMPARE(model.rowCount(), 5);

    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 2);
    QCOMPARE(insertSpy.last().at(2).toInt(), 2);

    itemList.beginInsertRows(5);
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.endInsertRows();

    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(insertSpy.count(), 3);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 5);
    QCOMPARE(insertSpy.last().at(2).toInt(), 7);
}

void tst_QGalleryItemListModel::itemsRemoved()
{
    QtTestGalleryItemList itemList;

    QGalleryItemListModel model;
    model.setItemList(&itemList);

    itemList.beginInsertRows(0);
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.endInsertRows();

    QCOMPARE(model.rowCount(), 8);

    QSignalSpy removeSpy(&model, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    itemList.removeRows(5, 3);
    QCOMPARE(model.rowCount(), 5);

    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 5);
    QCOMPARE(removeSpy.last().at(2).toInt(), 7);

    itemList.removeRows(2, 1);
    QCOMPARE(model.rowCount(), 4);

    QCOMPARE(removeSpy.count(), 2);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 2);
    QCOMPARE(removeSpy.last().at(2).toInt(), 2);

    itemList.removeRows(0, 4);
    QCOMPARE(model.rowCount(), 0);

    QCOMPARE(removeSpy.count(), 3);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 0);
    QCOMPARE(removeSpy.last().at(2).toInt(), 3);
}

void tst_QGalleryItemListModel::itemsMoved()
{
    QtTestGalleryItemList itemList;

    QGalleryItemListModel model;
    model.setItemList(&itemList);

    itemList.beginInsertRows(0);
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.addRow(QVector<QVariant>());
    itemList.endInsertRows();

    QCOMPARE(model.rowCount(), 8);

    QSignalSpy moveSpy(&model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)));

    itemList.moved(5, 2, 3);
    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(moveSpy.count(), 1);
    QCOMPARE(moveSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(1).toInt(), 5);
    QCOMPARE(moveSpy.last().at(2).toInt(), 7);
    QCOMPARE(moveSpy.last().at(3).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(4).toInt(), 2);

    itemList.moved(2, 7, 1);
    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(moveSpy.count(), 2);
    QCOMPARE(moveSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(1).toInt(), 2);
    QCOMPARE(moveSpy.last().at(2).toInt(), 2);
    QCOMPARE(moveSpy.last().at(3).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(4).toInt(), 7);

    itemList.moved(0, 4, 3);
    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(moveSpy.count(), 3);
    QCOMPARE(moveSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(1).toInt(), 0);
    QCOMPARE(moveSpy.last().at(2).toInt(), 2);
    QCOMPARE(moveSpy.last().at(3).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(4).toInt(), 4);
}

void tst_QGalleryItemListModel::metaDataChanged()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;

    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);

    model.setItemList(&itemList);

    QSignalSpy dataSpy(&model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    itemList.metaDataChanged(0, 1);
    QCOMPARE(dataSpy.count(), 1);
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(0, 3));

    itemList.metaDataChanged(1, 1, QList<int>() << 2 << 6);
    QCOMPARE(dataSpy.count(), 3);
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(0).value<QModelIndex>(), model.index(1, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(1).value<QModelIndex>(), model.index(1, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(1, 3));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(1, 3));

    itemList.metaDataChanged(0, 2, QList<int>() << 0 << 1 << 2 << 3 << 4 << 5 << 6);
    QCOMPARE(dataSpy.count(), 4);
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(1, 3));

    itemList.metaDataChanged(1, 3, QList<int>() << 1 << 5);
    QCOMPARE(dataSpy.count(), 5);
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(1, 1));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(3, 2));

    itemList.metaDataChanged(0, 1, QList<int>() << 1 << 4 << 6);
    QCOMPARE(dataSpy.count(), 7);
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(1).value<QModelIndex>(), model.index(0, 1));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(0, 3));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(0, 3));

    model.removeColumn(0);
    model.removeColumn(0);
    model.removeColumn(0);
    model.removeColumn(0);

    itemList.metaDataChanged(0, 1);
    QCOMPARE(dataSpy.count(), 7);

    itemList.metaDataChanged(1, 1, QList<int>() << 2 << 6);
    QCOMPARE(dataSpy.count(), 7);

    itemList.metaDataChanged(0, 2, QList<int>() << 0 << 1 << 2 << 3 << 4 << 5 << 6);
    QCOMPARE(dataSpy.count(), 7);

    itemList.metaDataChanged(1, 3, QList<int>() << 1 << 5);
    QCOMPARE(dataSpy.count(), 7);

    itemList.metaDataChanged(0, 1, QList<int>() << 1 << 4 << 6);
    QCOMPARE(dataSpy.count(), 7);
}

void tst_QGalleryItemListModel::invalidIndex()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);

    QCOMPARE(model.flags(QModelIndex()), Qt::ItemFlags());
    QCOMPARE(model.data(QModelIndex(), Qt::DisplayRole), QVariant());
    QCOMPARE(model.data(QModelIndex(), Qt::EditRole), QVariant());
    QCOMPARE(model.data(QModelIndex(), Qt::UserRole), QVariant());
    QCOMPARE(model.data(QModelIndex(), Qt::UserRole + 1), QVariant());
    QCOMPARE(model.setData(QModelIndex(), QLatin1String("Hello"), Qt::DisplayRole), false);
    QCOMPARE(model.setData(QModelIndex(), QLatin1String("Hello"), Qt::EditRole), false);
    QCOMPARE(model.setData(QModelIndex(), QLatin1String("Hello"), Qt::UserRole), false);
    QCOMPARE(model.setData(QModelIndex(), QLatin1String("Hello"), Qt::UserRole + 1), false);
    QCOMPARE(model.data(QModelIndex(), Qt::DisplayRole), QVariant());
    QCOMPARE(model.data(QModelIndex(), Qt::EditRole), QVariant());
    QCOMPARE(model.data(QModelIndex(), Qt::UserRole), QVariant());
    QCOMPARE(model.data(QModelIndex(), Qt::UserRole + 1), QVariant());
}
void tst_QGalleryItemListModel::hierarchy()
{
    QtTestGalleryItemList itemList;
    populateItemList(&itemList);
    itemList.appendOutOfRangeItems(3);

    QGalleryItemListModel model;
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.setItemList(&itemList);

    QModelIndex index = model.index(0, 0);
    QCOMPARE(index.isValid(), true);
    QCOMPARE(model.parent(QModelIndex()).isValid(), false);
    QCOMPARE(model.parent(index).isValid(), false);
    QCOMPARE(model.rowCount(index), 0);
    QCOMPARE(model.columnCount(index), 0);
    QCOMPARE(model.index(-1, 0, index).isValid(), false);
    QCOMPARE(model.index(-1, 2, index).isValid(), false);
    QCOMPARE(model.index(-1, 3, index).isValid(), false);
    QCOMPARE(model.index(-1, 4, index).isValid(), false);
    QCOMPARE(model.index( 0, 0, index).isValid(), false);
    QCOMPARE(model.index( 0, 2, index).isValid(), false);
    QCOMPARE(model.index( 0, 3, index).isValid(), false);
    QCOMPARE(model.index( 0, 4, index).isValid(), false);
    QCOMPARE(model.index( 1, 0, index).isValid(), false);
    QCOMPARE(model.index( 1, 2, index).isValid(), false);
    QCOMPARE(model.index( 1, 3, index).isValid(), false);
    QCOMPARE(model.index( 1, 4, index).isValid(), false);
    QCOMPARE(model.index( 4, 0, index).isValid(), false);
    QCOMPARE(model.index( 4, 2, index).isValid(), false);
    QCOMPARE(model.index( 4, 3, index).isValid(), false);
    QCOMPARE(model.index( 4, 4, index).isValid(), false);
    QCOMPARE(model.index( 5, 0, index).isValid(), false);
    QCOMPARE(model.index( 5, 2, index).isValid(), false);
    QCOMPARE(model.index( 5, 3, index).isValid(), false);
    QCOMPARE(model.index( 5, 4, index).isValid(), false);
}

QTEST_MAIN(tst_QGalleryItemListModel)

#include "tst_qgalleryitemlistmodel.moc"

