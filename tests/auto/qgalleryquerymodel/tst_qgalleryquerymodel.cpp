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

#include <qgalleryquerymodel.h>

#include <qabstractgallery.h>
#include <qgalleryqueryrequest.h>
#include <qgalleryresultset.h>

#include <QtCore/qpointer.h>

#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QModelIndex)
Q_DECLARE_METATYPE(QGalleryAbstractRequest::State)

class QtTestGallery;

class tst_QGalleryQueryModel : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void initTestCase();

private Q_SLOTS:
    void execute();
    void sortPropertyNames();
    void autoUpdate();
    void offset();
    void limit();
    void rootType();
    void rootItem();
    void scope();
    void filter();
    void indexes();
    void data();
    void flags();
    void headerData();
    void addColumn();
    void insertColumn();
    void removeColumn();
    void setRoleProperties();
    void itemsInserted();
    void itemsRemoved();
    void itemsMoved();
    void metaDataChanged();
    void invalidIndex();
    void hierarchy();
    void setGallery();

private:
    void populateGallery(QtTestGallery *gallery) const;

    QHash<int, QString> albumProperties;
    QHash<int, QString> titleProperties;
    QHash<int, QString> durationProperties;
    QHash<int, QString> ratingProperties;
    QHash<int, QString> turtleProperties;
};

class QtTestResultSet : public QGalleryResultSet
{
    Q_OBJECT
public:
    struct Row
    {
        Row() {}
        Row(const QVariant &itemId,
                const QUrl &itemUrl,
                const QString &itemType,
                const QHash<QString, QVariant> &metaData)
            : itemId(itemId)
            , itemUrl(itemUrl)
            , itemType(itemType)
            , metaData(metaData) {}
        QVariant itemId;
        QUrl itemUrl;
        QString itemType;
        QHash<QString, QVariant> metaData;
    };

    QtTestResultSet(
            QGalleryAbstractRequest::State state,
            int error,
            const QString &errorString,
            const QHash<QString, QGalleryProperty::Attributes> &propertyAttributes,
            const QVector<Row> &rows)
        : m_propertyNames(propertyAttributes.keys())
        , m_propertyAttributes(propertyAttributes)
        , m_rows(rows)
        , m_currentIndex(-1)
        , m_insertIndex(0)
        , m_insertCount(0)
    {
        if (error != QGalleryAbstractRequest::NoError)
            QGalleryAbstractResponse::error(error, errorString);
        else if (state == QGalleryAbstractRequest::Finished)
            finish();
        else if (state == QGalleryAbstractRequest::Idle)
            finish(true);
    }

    QStringList propertyNames() const { return m_propertyNames; }

    int propertyKey(const QString &propertyName) const {
        return  m_propertyNames.indexOf(propertyName); }

    QGalleryProperty::Attributes propertyAttributes(int key) const {
        return m_propertyAttributes.value(m_propertyNames.value(key)); }

    QVariant::Type propertyType(int) const { return QVariant::Invalid; }

    int itemCount() const { return m_rows.count(); }

    int currentIndex() const { return m_currentIndex; }
    bool fetch(int index)
    {
        emit currentIndexChanged(m_currentIndex = index);
        emit currentItemChanged();

        return isValid();
    }

    QVariant itemId() const { return m_rows.value(m_currentIndex).itemId; }
    QUrl itemUrl() const { return m_rows.value(m_currentIndex).itemUrl; }
    QString itemType() const { return m_rows.value(m_currentIndex).itemType; }

    QVariant metaData(int key) const {
        return m_rows.value(m_currentIndex).metaData.value(m_propertyNames.at(key)); }

    bool setMetaData(int key, const QVariant &value)
    {
        if (isValid() && key >= 0) {
            const QString propertyName = m_propertyNames.at(key);

            if (m_propertyAttributes.value(propertyName) & QGalleryProperty::CanWrite) {
                m_rows[m_currentIndex].metaData[propertyName] = value;

                emit metaDataChanged(m_currentIndex, 1, QList<int>() << key);

                return true;
            }
        }
        return false;
    }

    void beginInsertRows(int index) { m_insertIndex = index; m_insertCount = 0; }
    void addRow()
    {
        m_rows.insert(m_insertIndex + m_insertCount++, Row());
    }
    void addRow(
            const QString &itemId,
            const QUrl &itemUrl,
            const QString &itemType,
            const QHash<QString, QVariant> &metaData)
    {
        m_rows.insert(m_insertIndex + m_insertCount++, Row(itemId, itemUrl, itemType, metaData));
    }
    void endInsertRows() { emit itemsInserted(m_insertIndex, m_insertCount); }

    void removeRows(int index, int count) {
        m_rows.remove(index, count); emit itemsRemoved(index, count); }

    using QGalleryResultSet::metaDataChanged;
    using QGalleryResultSet::itemsMoved;

private:
    QStringList m_propertyNames;
    const QHash<QString, QGalleryProperty::Attributes> m_propertyAttributes;
    QVector<Row> m_rows;
    int m_currentIndex;
    int m_insertIndex;
    int m_insertCount;
};

class QtTestGallery : public QAbstractGallery
{
public:
    QtTestGallery()
        : m_state(QGalleryAbstractRequest::Finished)
        , m_error(QGalleryAbstractRequest::NoError)
    {
    }

    bool isRequestSupported(QGalleryAbstractRequest::RequestType) const { return true; }

    void setPropertyAttributes(const QHash<QString, QGalleryProperty::Attributes> &attributes)
    {
        m_propertyAttributes = attributes;
    }

    void setState(QGalleryAbstractRequest::State state) { m_state = state; }
    void setError(int error, const QString &errorString) {
        m_error = error; m_errorString = errorString; }

    void addRow()
    {
        m_rows.append(QtTestResultSet::Row());
    }

    void addRow(
            const QVariant &itemId,
            const QUrl &itemUrl,
            const QString &itemType,
            const QHash<QString, QVariant> &metaData)
    {
        m_rows.append(QtTestResultSet::Row(itemId, itemUrl, itemType, metaData));
    }

    QGalleryQueryRequest *request() const { return m_request; }

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request)
    {
        m_request = qobject_cast<QGalleryQueryRequest *>(request);

        return new QtTestResultSet(m_state, m_error, m_errorString, m_propertyAttributes, m_rows);
    }


private:
    QHash<QString, QGalleryProperty::Attributes> m_propertyAttributes;
    QVector<QtTestResultSet::Row> m_rows;
    QPointer<QGalleryQueryRequest> m_request;
    QGalleryAbstractRequest::State m_state;
    int m_error;
    QString m_errorString;
};

void tst_QGalleryQueryModel::initTestCase()
{
    qRegisterMetaType<QModelIndex>();
    qRegisterMetaType<QGalleryAbstractRequest::State>();

    albumProperties.insert(Qt::DisplayRole, QLatin1String("albumTitle"));
    albumProperties.insert(Qt::UserRole, QLatin1String("albumArtist"));
    albumProperties.insert(Qt::UserRole + 1, QLatin1String("albumId"));

    titleProperties.insert(Qt::DisplayRole, QLatin1String("displayName"));
    titleProperties.insert(Qt::EditRole, QLatin1String("title"));

    durationProperties.insert(Qt::DisplayRole, QLatin1String("duration"));

    ratingProperties.insert(Qt::DisplayRole, QLatin1String("rating"));

    turtleProperties.insert(Qt::DisplayRole, QLatin1String("turtle"));
}

void tst_QGalleryQueryModel::populateGallery(QtTestGallery *gallery) const
{
    QHash<QString, QGalleryProperty::Attributes> attributes;
    attributes.insert(QLatin1String("displayName"), QGalleryProperty::CanRead);
    attributes.insert(QLatin1String("title"), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    attributes.insert(QLatin1String("albumTitle"), QGalleryProperty::CanRead);
    attributes.insert(QLatin1String("albumArtist"), QGalleryProperty::CanRead);
    attributes.insert(QLatin1String("albumId"), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    attributes.insert(QLatin1String("duration"), QGalleryProperty::CanRead);
    attributes.insert(QLatin1String("rating"), QGalleryProperty::CanRead | QGalleryProperty::CanWrite);
    gallery->setPropertyAttributes(attributes);

    {
        QHash<QString, QVariant> metaData;
        metaData.insert(QLatin1String("displayName"), QLatin1String("Interlude"));
        metaData.insert(QLatin1String("title"), QLatin1String("Interlude"));
        metaData.insert(QLatin1String("albumTitle"), QLatin1String("Greatest Hits"));
        metaData.insert(QLatin1String("albumArtist"), QLatin1String("Self Titled"));
        metaData.insert(QLatin1String("albumId"), QLatin1String("album:SelfTitled:GreatestHits"));
        metaData.insert(QLatin1String("duration"), 120100);
        metaData.insert(QLatin1String("rating"), 4);

        gallery->addRow(
                3,
                QUrl(QLatin1String("file:///music/interlude.mp3")),
                QLatin1String("Audio"),
                metaData);
    } {
        QHash<QString, QVariant> metaData;
        metaData.insert(QLatin1String("displayName"), QLatin1String("beep.wav"));
        metaData.insert(QLatin1String("duration"), 600);

        gallery->addRow(
                45,
                QUrl(QLatin1String("file:///sounds/beep.wav")),
                QLatin1String("Audio"),
                metaData);
    }
}

void tst_QGalleryQueryModel::execute()
{
    QtTestGallery gallery;

    QGalleryQueryModel model(&gallery);

    QSignalSpy finishedSpy(&model, SIGNAL(finished()));
    QSignalSpy canceledSpy(&model, SIGNAL(canceled()));
    QSignalSpy errorSpy(&model, SIGNAL(error(int,QString)));
    QSignalSpy stateSpy(&model, SIGNAL(stateChanged(QGalleryAbstractRequest::State)));

    model.execute();
    QCOMPARE(model.state(), QGalleryAbstractRequest::Finished);
    QCOMPARE(model.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(canceledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.last().value(0).value<QGalleryAbstractRequest::State>(), model.state());

    gallery.setState(QGalleryAbstractRequest::Active);
    model.execute();
    QCOMPARE(model.state(), QGalleryAbstractRequest::Active);
    QCOMPARE(model.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(canceledSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(stateSpy.last().value(0).value<QGalleryAbstractRequest::State>(), model.state());

    model.cancel();
    QCOMPARE(model.state(), QGalleryAbstractRequest::Canceled);
    QCOMPARE(model.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(canceledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(stateSpy.count(), 3);
    QCOMPARE(stateSpy.last().value(0).value<QGalleryAbstractRequest::State>(), model.state());

    gallery.setError(120, QLatin1String("bad connection"));
    model.execute();
    QCOMPARE(model.state(), QGalleryAbstractRequest::Error);
    QCOMPARE(model.error(), 120);
    QCOMPARE(model.errorString(), QLatin1String("bad connection"));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(canceledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(errorSpy.last().value(0).toInt(), model.error());
    QCOMPARE(errorSpy.last().value(1).toString(), model.errorString());
    QCOMPARE(stateSpy.last().value(0).value<QGalleryAbstractRequest::State>(), model.state());

    model.clear();
    QCOMPARE(model.state(), QGalleryAbstractRequest::Inactive);
    QCOMPARE(model.error(), int(QGalleryAbstractRequest::NoError));
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(canceledSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 5);
}

void tst_QGalleryQueryModel::sortPropertyNames()
{
    const QStringList propertyNames = QStringList()
            << QLatin1String("-rating")
            << QLatin1String("+duration");

    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(sortPropertyNamesChanged()));

    QCOMPARE(model.sortPropertyNames(), QStringList());

    model.setSortPropertyNames(QStringList());
    QCOMPARE(model.sortPropertyNames(), QStringList());
    QCOMPARE(spy.count(), 0);

    model.setSortPropertyNames(propertyNames);
    QCOMPARE(model.sortPropertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    model.setSortPropertyNames(propertyNames);
    QCOMPARE(model.sortPropertyNames(), propertyNames);
    QCOMPARE(spy.count(), 1);

    model.setSortPropertyNames(QStringList());
    QCOMPARE(model.sortPropertyNames(), QStringList());
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryModel::autoUpdate()
{
    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(autoUpdateChanged()));

    QCOMPARE(model.autoUpdate(), false);

    model.setAutoUpdate(false);
    QCOMPARE(model.autoUpdate(), false);
    QCOMPARE(spy.count(), 0);

    model.setAutoUpdate(true);
    QCOMPARE(model.autoUpdate(), true);
    QCOMPARE(spy.count(), 1);

    model.setAutoUpdate(true);
    QCOMPARE(model.autoUpdate(), true);
    QCOMPARE(spy.count(), 1);

    model.setAutoUpdate(false);
    QCOMPARE(model.autoUpdate(), false);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryModel::offset()
{
    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(offsetChanged()));

    QCOMPARE(model.offset(), 0);

    model.setOffset(0);
    QCOMPARE(model.offset(), 0);
    QCOMPARE(spy.count(), 0);

    model.setOffset(-45);
    QCOMPARE(model.offset(), 0);
    QCOMPARE(spy.count(), 0);

    model.setOffset(32);
    QCOMPARE(model.offset(), 32);
    QCOMPARE(spy.count(), 1);

    model.setOffset(32);
    QCOMPARE(model.offset(), 32);
    QCOMPARE(spy.count(), 1);

    model.setOffset(-45);
    QCOMPARE(model.offset(), 0);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryModel::limit()
{
    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(limitChanged()));

    QCOMPARE(model.limit(), 0);

    model.setLimit(0);
    QCOMPARE(model.limit(), 0);
    QCOMPARE(spy.count(), 0);

    model.setLimit(-21);
    QCOMPARE(model.limit(), 0);
    QCOMPARE(spy.count(), 0);

    model.setLimit(102);
    QCOMPARE(model.limit(), 102);
    QCOMPARE(spy.count(), 1);

    model.setLimit(102);
    QCOMPARE(model.limit(), 102);
    QCOMPARE(spy.count(), 1);

    model.setLimit(-21);
    QCOMPARE(model.limit(), 0);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryModel::rootType()
{
    const QString itemType = QLatin1String("Audio");

    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(rootTypeChanged()));

    QCOMPARE(model.rootType(), QString());

    model.setRootType(QString());
    QCOMPARE(model.rootType(), QString());
    QCOMPARE(spy.count(), 0);

    model.setRootType(itemType);
    QCOMPARE(model.rootType(), itemType);
    QCOMPARE(spy.count(), 1);

    model.setRootType(itemType);
    QCOMPARE(model.rootType(), itemType);
    QCOMPARE(spy.count(), 1);

    model.setRootType(QString());
    QCOMPARE(model.rootType(), QString());
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryModel::rootItem()
{
    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(rootItemChanged()));

    QCOMPARE(model.rootItem(), QVariant());

    model.setRootItem(QVariant());
    QCOMPARE(model.rootItem(), QVariant());
    QCOMPARE(spy.count(), 0);

    model.setRootItem(76);
    QCOMPARE(model.rootItem(), QVariant(76));
    QCOMPARE(spy.count(), 1);

    model.setRootItem(76);
    QCOMPARE(model.rootItem(), QVariant(76));
    QCOMPARE(spy.count(), 1);

    model.setRootItem(QLatin1String("65"));
    QCOMPARE(model.rootItem(), QVariant(QLatin1String("65")));
    QCOMPARE(spy.count(), 2);

    model.setRootItem(QLatin1String("65"));
    QCOMPARE(model.rootItem(), QVariant(QLatin1String("65")));
    QCOMPARE(spy.count(), 2);

    model.setRootItem(QVariant());
    QCOMPARE(model.rootItem(), QVariant());
    QCOMPARE(spy.count(), 3);
}

void tst_QGalleryQueryModel::scope()
{
    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(scopeChanged()));

    QCOMPARE(model.scope(), QGalleryQueryRequest::AllDescendants);

    model.setScope(QGalleryQueryRequest::AllDescendants);
    QCOMPARE(model.scope(), QGalleryQueryRequest::AllDescendants);
    QCOMPARE(spy.count(), 0);

    model.setScope(QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(model.scope(), QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(spy.count(), 1);

    model.setScope(QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(model.scope(), QGalleryQueryRequest::DirectDescendants);
    QCOMPARE(spy.count(), 1);

    model.setScope(QGalleryQueryRequest::AllDescendants);
    QCOMPARE(model.scope(), QGalleryQueryRequest::AllDescendants);
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryModel::filter()
{
    const QGalleryFilter filter = QGalleryMetaDataFilter(
            QLatin1String("rating"), 3, QGalleryFilter::GreaterThan);

    QGalleryQueryModel model;

    QSignalSpy spy(&model, SIGNAL(filterChanged()));

    QCOMPARE(model.filter(), QGalleryFilter());

    model.setFilter(QGalleryFilter());
    QCOMPARE(model.filter(), QGalleryFilter());
    QCOMPARE(spy.count(), 0);

    model.setFilter(filter);
    QCOMPARE(model.filter(), filter);
    QCOMPARE(spy.count(), 1);

    model.setFilter(filter);
    QCOMPARE(model.filter(), filter);
    QCOMPARE(spy.count(), 1);

    model.setFilter(QGalleryFilter());
    QCOMPARE(model.filter(), QGalleryFilter());
    QCOMPARE(spy.count(), 2);
}

void tst_QGalleryQueryModel::indexes()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);

    QSignalSpy insertSpy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QSignalSpy removeSpy(&model, SIGNAL(rowsRemoved(QModelIndex,int,int)));

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
    QCOMPARE(model.index( 2, -1).isValid(), false);
    QCOMPARE(model.index( 2,  0).isValid(), false);
    QCOMPARE(model.index( 2,  2).isValid(), false);
    QCOMPARE(model.index( 2,  3).isValid(), false);
    QCOMPARE(model.index( 2,  4).isValid(), false);
    QCOMPARE(model.index( 2,  5).isValid(), false);

    model.execute();

    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.columnCount(), 5);

    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 0);
    QCOMPARE(insertSpy.last().at(2).toInt(), 1);

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
    QCOMPARE(model.index( 2, -1).isValid(), false);
    QCOMPARE(model.index( 2,  0).isValid(), false);
    QCOMPARE(model.index( 2,  2).isValid(), false);
    QCOMPARE(model.index( 2,  3).isValid(), false);
    QCOMPARE(model.index( 2,  4).isValid(), false);
    QCOMPARE(model.index( 2,  5).isValid(), false);

    model.clear();

    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.columnCount(), 5);

    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 0);
    QCOMPARE(removeSpy.last().at(2).toInt(), 1);

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
    QCOMPARE(model.index( 2, -1).isValid(), false);
    QCOMPARE(model.index( 2,  0).isValid(), false);
    QCOMPARE(model.index( 2,  2).isValid(), false);
    QCOMPARE(model.index( 2,  3).isValid(), false);
    QCOMPARE(model.index( 2,  4).isValid(), false);
    QCOMPARE(model.index( 2,  5).isValid(), false);
}

void tst_QGalleryQueryModel::data()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);
    model.execute();

    QModelIndex index;

    index = model.index(0, 0);
    QCOMPARE(model.itemId(index), QVariant(3));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///music/interlude.mp3")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Greatest Hits")));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant(QLatin1String("Self Titled")));
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:SelfTitled:GreatestHits")));

    index = model.index(0, 1);
    QCOMPARE(model.itemId(index), QVariant(3));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///music/interlude.mp3")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Interlude")));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 2);
    QCOMPARE(model.itemId(index), QVariant(3));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///music/interlude.mp3")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(120100));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 3);
    QCOMPARE(model.itemId(index), QVariant(3));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///music/interlude.mp3")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(4));
    QCOMPARE(index.data(Qt::EditRole), QVariant(4));
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(0, 4);
    QCOMPARE(model.itemId(index), QVariant(3));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///music/interlude.mp3")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    index = model.index(1, 0);
    QCOMPARE(model.itemId(index), QVariant(45));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///sounds/beep.wav")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
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
    QCOMPARE(model.itemId(index), QVariant(45));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///sounds/beep.wav")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::DisplayRole), false);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("beep.wav")));

    QCOMPARE(model.setData(index, QLatin1String("Beep Sound"), Qt::EditRole), true);
    QCOMPARE(index.data(Qt::EditRole), QVariant(QLatin1String("Beep Sound")));

    index = model.index(1, 2);
    QCOMPARE(model.itemId(index), QVariant(45));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///sounds/beep.wav")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
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
    QCOMPARE(model.itemId(index), QVariant(45));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///sounds/beep.wav")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
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
    QCOMPARE(model.itemId(index), QVariant(45));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///sounds/beep.wav")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant());
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant());

    // Random access.
    index = model.index(0, 3);
    QCOMPARE(model.setData(index, 4, Qt::DisplayRole), true);
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(4));
    QCOMPARE(index.data(Qt::EditRole), QVariant(4));

    index = model.index(1, 0);
    QCOMPARE(model.itemId(index), QVariant(45));

    index = model.index(0, 0);
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///music/interlude.mp3")));

    index = model.index(1, 0);
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
}

void tst_QGalleryQueryModel::flags()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);
    model.execute();

    QCOMPARE(model.index(0, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 1).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 2).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(0, 3).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(0, 4).flags(), Qt::ItemFlags());

    QCOMPARE(model.index(1, 0).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);;
    QCOMPARE(model.index(1, 1).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(1, 2).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QCOMPARE(model.index(1, 3).flags(), Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    QCOMPARE(model.index(1, 4).flags(), Qt::ItemFlags());
}

void tst_QGalleryQueryModel::headerData()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);

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

void tst_QGalleryQueryModel::addColumn()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.execute();

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
    QCOMPARE(model.index(0, 4).flags(), Qt::ItemFlags());

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

void tst_QGalleryQueryModel::insertColumn()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.execute();

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
    QCOMPARE(model.index(0, 4).flags(), Qt::ItemFlags());

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

void tst_QGalleryQueryModel::removeColumn()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);

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

    model.execute();

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

void tst_QGalleryQueryModel::setRoleProperties()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.execute();
    QVERIFY(gallery.request() != 0);

    QtTestResultSet *resultSet = qobject_cast<QtTestResultSet *>(gallery.request()->resultSet());
    QVERIFY(resultSet != 0);

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

    resultSet->removeRows(0, 2);

    model.setRoleProperties(0, albumProperties);
    QCOMPARE(model.roleProperties(0), albumProperties);
    QCOMPARE(dataSpy.count(), 3);
}

void tst_QGalleryQueryModel::itemsInserted()
{
    QtTestGallery gallery;

    QGalleryQueryModel model(&gallery);
    model.execute();
    QVERIFY(gallery.request() != 0);

    QtTestResultSet *resultSet = qobject_cast<QtTestResultSet *>(gallery.request()->resultSet());
    QVERIFY(resultSet != 0);

    QSignalSpy insertSpy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QCOMPARE(model.rowCount(), 0);

    resultSet->beginInsertRows(0);
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->endInsertRows();

    QCOMPARE(model.rowCount(), 4);

    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 0);
    QCOMPARE(insertSpy.last().at(2).toInt(), 3);

    resultSet->beginInsertRows(2);
    resultSet->addRow();
    resultSet->endInsertRows();

    QCOMPARE(model.rowCount(), 5);

    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 2);
    QCOMPARE(insertSpy.last().at(2).toInt(), 2);

    resultSet->beginInsertRows(5);
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->endInsertRows();

    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(insertSpy.count(), 3);
    QCOMPARE(insertSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(insertSpy.last().at(1).toInt(), 5);
    QCOMPARE(insertSpy.last().at(2).toInt(), 7);
}

void tst_QGalleryQueryModel::itemsRemoved()
{
    QtTestGallery gallery;

    QGalleryQueryModel model(&gallery);
    model.execute();
    QVERIFY(gallery.request() != 0);

    QtTestResultSet *resultSet = qobject_cast<QtTestResultSet *>(gallery.request()->resultSet());
    QVERIFY(resultSet != 0);

    resultSet->beginInsertRows(0);
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->endInsertRows();

    QCOMPARE(model.rowCount(), 8);

    QSignalSpy removeSpy(&model, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    resultSet->removeRows(5, 3);
    QCOMPARE(model.rowCount(), 5);

    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 5);
    QCOMPARE(removeSpy.last().at(2).toInt(), 7);

    resultSet->removeRows(2, 1);
    QCOMPARE(model.rowCount(), 4);

    QCOMPARE(removeSpy.count(), 2);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 2);
    QCOMPARE(removeSpy.last().at(2).toInt(), 2);

    resultSet->removeRows(0, 4);
    QCOMPARE(model.rowCount(), 0);

    QCOMPARE(removeSpy.count(), 3);
    QCOMPARE(removeSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(removeSpy.last().at(1).toInt(), 0);
    QCOMPARE(removeSpy.last().at(2).toInt(), 3);
}

void tst_QGalleryQueryModel::itemsMoved()
{
    QtTestGallery gallery;

    QGalleryQueryModel model(&gallery);
    model.execute();
    QVERIFY(gallery.request() != 0);

    QtTestResultSet *resultSet = qobject_cast<QtTestResultSet *>(gallery.request()->resultSet());
    QVERIFY(resultSet != 0);

    resultSet->beginInsertRows(0);
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->addRow();
    resultSet->endInsertRows();

    QCOMPARE(model.rowCount(), 8);

    QSignalSpy moveSpy(&model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)));

    resultSet->itemsMoved(5, 2, 3);
    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(moveSpy.count(), 1);
    QCOMPARE(moveSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(1).toInt(), 5);
    QCOMPARE(moveSpy.last().at(2).toInt(), 7);
    QCOMPARE(moveSpy.last().at(3).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(4).toInt(), 2);

    resultSet->itemsMoved(2, 7, 1);
    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(moveSpy.count(), 2);
    QCOMPARE(moveSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(1).toInt(), 2);
    QCOMPARE(moveSpy.last().at(2).toInt(), 2);
    QCOMPARE(moveSpy.last().at(3).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(4).toInt(), 7);

    resultSet->itemsMoved(0, 4, 3);
    QCOMPARE(model.rowCount(), 8);

    QCOMPARE(moveSpy.count(), 3);
    QCOMPARE(moveSpy.last().at(0).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(1).toInt(), 0);
    QCOMPARE(moveSpy.last().at(2).toInt(), 2);
    QCOMPARE(moveSpy.last().at(3).value<QModelIndex>(), QModelIndex());
    QCOMPARE(moveSpy.last().at(4).toInt(), 4);
}

void tst_QGalleryQueryModel::metaDataChanged()
{
    QtTestGallery gallery;
    populateGallery(&gallery);
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();
    gallery.addRow();

    QGalleryQueryModel model(&gallery);

    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);

    model.execute();
    QVERIFY(gallery.request() != 0);

    QtTestResultSet *resultSet = qobject_cast<QtTestResultSet *>(gallery.request()->resultSet());
    QVERIFY(resultSet != 0);

    const int key0_0 = resultSet->propertyKey(QLatin1String("albumTitle"));
    const int key0_1 = resultSet->propertyKey(QLatin1String("albumArtist"));
    const int key0_2 = resultSet->propertyKey(QLatin1String("albumId"));
    const int key1_0 = resultSet->propertyKey(QLatin1String("displayName"));
    const int key1_1 = resultSet->propertyKey(QLatin1String("title"));
    const int key2_0 = resultSet->propertyKey(QLatin1String("duration"));
    const int key3_0 = resultSet->propertyKey(QLatin1String("rating"));

    QSignalSpy dataSpy(&model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
    resultSet->metaDataChanged(0, 1, QList<int>());
    QCOMPARE(dataSpy.count(), 0);

    resultSet->metaDataChanged(0, 1, QList<int>()
            << key0_0 << key0_1 << key0_2
            << key1_0 << key1_1
            << key2_0
            << key3_0);
    QCOMPARE(dataSpy.count(), 1);
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(0, 3));

    resultSet->metaDataChanged(1, 1, QList<int>() << key0_2 << key3_0);
    QCOMPARE(dataSpy.count(), 3);
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(0).value<QModelIndex>(), model.index(1, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(1).value<QModelIndex>(), model.index(1, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(1, 3));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(1, 3));

    resultSet->metaDataChanged(0, 2, QList<int>()
            << key0_0 << key0_1 << key0_2
            << key1_0 << key1_1
            << key2_0
            << key3_0);
    QCOMPARE(dataSpy.count(), 4);
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(1, 3));

    resultSet->metaDataChanged(1, 3, QList<int>() << key1_1 << key2_0);
    QCOMPARE(dataSpy.count(), 5);
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(1, 1));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(3, 2));

    resultSet->metaDataChanged(0, 1, QList<int>() << key0_1 << key1_1 << key3_0);
    QCOMPARE(dataSpy.count(), 7);
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(0).value<QModelIndex>(), model.index(0, 0));
    QCOMPARE(dataSpy.at(dataSpy.count() - 2).at(1).value<QModelIndex>(), model.index(0, 1));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(0).value<QModelIndex>(), model.index(0, 3));
    QCOMPARE(dataSpy.at(dataSpy.count() - 1).at(1).value<QModelIndex>(), model.index(0, 3));

    model.removeColumn(0);
    model.removeColumn(0);
    model.removeColumn(0);
    model.removeColumn(0);

    resultSet->metaDataChanged(0, 1, QList<int>());
    QCOMPARE(dataSpy.count(), 7);

    resultSet->metaDataChanged(1, 1, QList<int>() << key0_2 << key3_0);
    QCOMPARE(dataSpy.count(), 7);

    resultSet->metaDataChanged(0, 2, QList<int>()
            << key0_0 << key0_1 << key0_2
            << key1_0 << key1_1
            << key2_0
            << key3_0);
    QCOMPARE(dataSpy.count(), 7);

    resultSet->metaDataChanged(1, 3, QList<int>() << key0_1 << key2_0);
    QCOMPARE(dataSpy.count(), 7);

    resultSet->metaDataChanged(0, 1, QList<int>() << key0_1 << key1_1 << key3_0);
    QCOMPARE(dataSpy.count(), 7);
}

void tst_QGalleryQueryModel::invalidIndex()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);

    QCOMPARE(model.itemId(QModelIndex()), QVariant());
    QCOMPARE(model.itemUrl(QModelIndex()), QUrl());
    QCOMPARE(model.itemType(QModelIndex()), QString());
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
void tst_QGalleryQueryModel::hierarchy()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QGalleryQueryModel model(&gallery);
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.execute();

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

void tst_QGalleryQueryModel::setGallery()
{
    QtTestGallery gallery;
    populateGallery(&gallery);

    QModelIndex index;

    QGalleryQueryModel model;
    model.addColumn(albumProperties);
    model.addColumn(titleProperties);
    model.addColumn(QLatin1String("duration"), Qt::DisplayRole);
    model.addColumn(QLatin1String("rating"), Qt::DisplayRole);
    model.addColumn(QLatin1String("turtle"), Qt::DisplayRole);

    QVERIFY(model.gallery() == 0);
    model.execute();

    index = model.index(0, 0);
    QCOMPARE(index.isValid(), false);

    model.setGallery(&gallery);
    QVERIFY(model.gallery() == &gallery);
    model.execute();

    index = model.index(0, 0);
    QCOMPARE(index.isValid(), true);
    QCOMPARE(model.itemId(index), QVariant(3));
    QCOMPARE(model.itemUrl(index), QUrl(QLatin1String("file:///music/interlude.mp3")));
    QCOMPARE(model.itemType(index), QString::fromLatin1("Audio"));
    QCOMPARE(index.data(Qt::DisplayRole), QVariant(QLatin1String("Greatest Hits")));
    QCOMPARE(index.data(Qt::EditRole), QVariant());
    QCOMPARE(index.data(Qt::UserRole), QVariant(QLatin1String("Self Titled")));
    QCOMPARE(index.data(Qt::UserRole + 1), QVariant(QLatin1String("album:SelfTitled:GreatestHits")));

    model.setGallery(0);
    QVERIFY(model.gallery() == 0);
    model.execute();

    index = model.index(0, 0);
    QCOMPARE(index.isValid(), false);
}

QTEST_MAIN(tst_QGalleryQueryModel)

#include "tst_qgalleryquerymodel.moc"

