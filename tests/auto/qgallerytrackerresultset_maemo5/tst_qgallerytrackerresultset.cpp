/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/documentgallery

#include <maemo5/qgallerytrackerresultset_p.h>
#include <maemo5/qgallerytrackerlistcolumn_p.h>

#include <qgalleryresource.h>

#include <QtDBus/qdbusabstractadaptor.h>
#include <QtDBus/qdbusmetatype.h>

#include <QtTest/QtTest>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_USE_NAMESPACE

class QtTestQueryAdaptor;

class tst_QGalleryTrackerResultSet : public QObject
{
    Q_OBJECT
public:
    tst_QGalleryTrackerResultSet()
        : m_itemType(QLatin1String("itemType"))
        , m_character(QLatin1String("character"))
        , m_number(QLatin1String("number"))
        , m_argument(QLatin1String("argument"))
        , m_composite(QLatin1String("composite"))
        , m_alias(QLatin1String("alias"))
        , m_image(QLatin1String("image"))
        , m_connection(QDBusConnection::sessionBus())
        , m_queryAdaptor(0) {}

public Q_SLOTS:
    void initTestCase();
    void cleanup();

private Q_SLOTS:
    void query();
    void refresh();
    void reset();
    void removeItem();
    void insertItem();
    void replaceFirstItem();
    void replaceLastItem();
    void replaceMiddleItem();

private:
    void populateArguments(
            QGalleryTrackerResultSetArguments *arguments, const QString &argument);

    const QString m_itemType;
    const QString m_character;
    const QString m_number;
    const QString m_argument;
    const QString m_composite;
    const QString m_alias;
    const QString m_image;
    QDBusConnection m_connection;
    QtTestQueryAdaptor *m_queryAdaptor;
    QGalleryDBusInterfacePointer m_queryInterface;
};

class QtTestProgressMonitor : public QObject
{
    Q_OBJECT
public:
    QtTestProgressMonitor() : m_currentProgress(0), m_maximumProgress(1) {}

    bool isFinished() const { return m_currentProgress == m_maximumProgress; }

public Q_SLOTS:
    void progressChanged(int current, int maximum)
    {
        m_currentProgress = current;
        m_maximumProgress = maximum;
    }

private:
    int m_currentProgress;
    int m_maximumProgress;
};

class QtTestQueryAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.QtTest.QGalleryTrackerResultSet.Query")
public:
    QtTestQueryAdaptor(QObject *object) : QDBusAbstractAdaptor(object)
    {
        qMemSet(m_counts, 0, sizeof(m_counts));
    }

public:
    int count(char id) const { return m_counts[id != '\0' ? (id + 1 - 'a') : 0]; }
    void setCount(char id, int count) { m_counts[id != '\0' ? (id + 1 - 'a') : 0] = count; }

    void clear() { qMemSet(m_counts, 0, sizeof(m_counts)); }

public Q_SLOTS:
    QVector<QStringList> GetResults(const QString &argument, int offset, int limit)
    {
        QVector<QStringList> results;

        for (int i = offset; i < m_counts[0] && limit > 0; ++i, --limit) {
            QStringList result;

            QString number = QString(QLatin1String("%1")).arg(i, 3, 10, QLatin1Char('0'));

            QString id = QLatin1String("0-") + number;
            QString url;
            QString type = QLatin1String("type0");

            result.append(id);
            result.append(url);
            result.append(type);
            result.append(QString());
            result.append(number);
            result.append(argument);

            results.append(result);
        }

        offset = qMax(0, offset - m_counts[0]);

        for (int c = 1; c < 26; ++c) {
            const QString character = QChar(QLatin1Char('a' + c - 1));

            for (int i = offset; i < m_counts[c] && limit > 0; ++i, --limit) {
                QStringList result;

                QString number = QString(QLatin1String("%1")).arg(i, 3, 10, QLatin1Char('0'));

                QString id = character + QLatin1Char('-') + number;
                QString url
                        = QLatin1String("file:///")
                        + character
                        + QLatin1Char('/')
                        + number;
                QString type = QLatin1String("type") + character;

                result.append(id);
                result.append(url);
                result.append(type);
                result.append(character);
                result.append(number);
                result.append(argument);

                results.append(result);
            }

            offset = qMax(0, offset - m_counts[c]);
        }

        return results;
    }

private:
    int m_counts[27];
};

class QtTestStringValueColumn : public QGalleryTrackerValueColumn
{
public:
    QVariant toVariant(const QString &string) const { return string; }
    QString toString(const QVariant &variant) const { return variant.toString(); }
};

class QtTestIdentityColumn : public QGalleryTrackerCompositeColumn
{
public:
    QtTestIdentityColumn(int column) : m_column(column) {}

    QVariant value(QVector<QVariant>::const_iterator row) const { return *(row + m_column); }

private:
    const int m_column;
};

class QtTestCompositeColumn : public QGalleryTrackerCompositeColumn
{
public:
    QtTestCompositeColumn(int columnA, int columnB) : m_columnA(columnA), m_columnB(columnB) {}

    QVariant value(QVector<QVariant>::const_iterator row) const {
        return (row + m_columnA)->toString() + QLatin1Char('|') + (row + m_columnB)->toString(); }

private:
    const int m_columnA;
    const int m_columnB;
};

void tst_QGalleryTrackerResultSet::initTestCase()
{
    const QString service
            = QLatin1String("com.nokia.QtTest.QGalleryTrackerResultSet")
              + QString::number(QCoreApplication::applicationPid());
    const QString path = QLatin1String("/");
    const char *interface = "com.nokia.QtTest.QGalleryTrackerResultSet.Query";

    qDBusRegisterMetaType<QVector<QStringList> >();

    m_queryAdaptor = new QtTestQueryAdaptor(this);

    m_connection.registerService(service);
    m_connection.registerObject(path, this);

    m_queryInterface = new QGalleryDBusInterface(service, path, interface);

    QVERIFY(m_queryInterface->isValid());
}

void tst_QGalleryTrackerResultSet::cleanup()
{
    m_queryAdaptor->clear();
}

void tst_QGalleryTrackerResultSet::populateArguments(
        QGalleryTrackerResultSetArguments *arguments, const QString &argument)
{
    arguments->idColumn.reset(new QtTestIdentityColumn(0));
    arguments->urlColumn.reset(new QtTestIdentityColumn(1));
    arguments->typeColumn.reset(new QtTestIdentityColumn(2));
    arguments->updateMask = 0x01;
    arguments->identityWidth = 1;
    arguments->tableWidth = 6;
    arguments->valueOffset = 2;
    arguments->compositeOffset = 6;
    arguments->queryInterface = m_queryInterface;
    arguments->queryMethod = QLatin1String("GetResults");
    arguments->queryArguments = QVariantList()
            << argument;
    arguments->propertyNames = QStringList()
            << m_itemType
            << m_character
            << m_number
            << m_argument
            << m_composite
            << m_alias;
    arguments->fieldNames = QStringList();
    arguments->propertyAttributes = QVector<QGalleryProperty::Attributes>()
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort);
    arguments->propertyTypes = QVector<QVariant::Type>()
                               << QVariant::String
                               << QVariant::Char
                               << QVariant::Int
                               << QVariant::String
                               << QVariant::String
                               << QVariant::Int;
    arguments->valueColumns = QVector<QGalleryTrackerValueColumn *>()
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn;
    arguments->compositeColumns = QVector<QGalleryTrackerCompositeColumn *>()
            << new QtTestCompositeColumn(3, 4);
    arguments->aliasColumns = QVector<int>()
            << 4;
    arguments->sortCriteria = QVector<QGalleryTrackerSortCriteria>();
    arguments->resourceKeys = QVector<int>()
            << 1
            << 2
            << 3;
}

void tst_QGalleryTrackerResultSet::query()
{
    const QStringList propertyNames = QStringList()
            << m_itemType
            << m_character
            << m_number
            << m_argument
            << m_composite
            << m_alias;

    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('\0', 1);
    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerResultSet resultSet(&arguments, false, 0, 32);
    QCOMPARE(resultSet.propertyNames(), propertyNames);
    QCOMPARE(resultSet.propertyKey(m_itemType), 2);
    QCOMPARE(resultSet.propertyKey(m_alias), 7);
    QCOMPARE(resultSet.propertyKey(QLatin1String("turtle")), -1);
    QCOMPARE(resultSet.propertyAttributes(2), (QGalleryProperty::CanRead | QGalleryProperty::CanFilter));
    QCOMPARE(resultSet.propertyAttributes(7), (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort));
    QCOMPARE(resultSet.propertyType(2), QVariant::String);
    QCOMPARE(resultSet.propertyType(7), QVariant::Int);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 17);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 17);

    QCOMPARE(resultSet.currentIndex(), -1);
    QCOMPARE(resultSet.itemId(), QVariant());
    QCOMPARE(resultSet.itemUrl(), QUrl());
    QCOMPARE(resultSet.itemType(), QString());
    QCOMPARE(resultSet.metaData(0), QVariant());
    QCOMPARE(resultSet.metaData(1), QVariant());
    QCOMPARE(resultSet.metaData(2), QVariant());
    QCOMPARE(resultSet.metaData(3), QVariant());
    QCOMPARE(resultSet.metaData(4), QVariant());
    QCOMPARE(resultSet.metaData(5), QVariant());
    QCOMPARE(resultSet.metaData(6), QVariant());
    QCOMPARE(resultSet.metaData(7), QVariant());
    QCOMPARE(resultSet.metaData(8), QVariant());
    QCOMPARE(resultSet.resources(), QList<QGalleryResource>());

    QCOMPARE(resultSet.fetchFirst(), true);
    QCOMPARE(resultSet.currentIndex(), 0);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("0-000")));
    QCOMPARE(resultSet.itemUrl(), QUrl());
    QCOMPARE(resultSet.itemType(), QLatin1String("type0"));
    QCOMPARE(resultSet.metaData(0), QVariant());
    QCOMPARE(resultSet.metaData(1), QVariant());
    QCOMPARE(resultSet.metaData(2), QVariant(QLatin1String("type0")));
    QCOMPARE(resultSet.metaData(3), QVariant(QVariant::String));
    QCOMPARE(resultSet.metaData(4), QVariant(QLatin1String("000")));
    QCOMPARE(resultSet.metaData(5), QVariant(QLatin1String("query")));
    QCOMPARE(resultSet.metaData(6), QVariant(QLatin1String("|000")));
    QCOMPARE(resultSet.metaData(7), QVariant(QLatin1String("000")));
    QCOMPARE(resultSet.metaData(8), QVariant());
    QCOMPARE(resultSet.resources(), QList<QGalleryResource>());

    QCOMPARE(resultSet.setMetaData(5, QLatin1String("edit")), false);
    QCOMPARE(resultSet.metaData(5), QVariant(QLatin1String("query")));

    QCOMPARE(resultSet.fetchPrevious(), false);
    QCOMPARE(resultSet.currentIndex(), -1);
    QCOMPARE(resultSet.itemId(), QVariant());
    QCOMPARE(resultSet.itemUrl(), QUrl());
    QCOMPARE(resultSet.itemType(), QString());
    QCOMPARE(resultSet.metaData(0), QVariant());
    QCOMPARE(resultSet.metaData(1), QVariant());
    QCOMPARE(resultSet.metaData(2), QVariant());
    QCOMPARE(resultSet.metaData(3), QVariant());
    QCOMPARE(resultSet.metaData(4), QVariant());
    QCOMPARE(resultSet.metaData(5), QVariant());
    QCOMPARE(resultSet.metaData(6), QVariant());
    QCOMPARE(resultSet.metaData(7), QVariant());
    QCOMPARE(resultSet.metaData(8), QVariant());
    QCOMPARE(resultSet.resources(), QList<QGalleryResource>());

    QCOMPARE(resultSet.fetchLast(), true);
    QCOMPARE(resultSet.currentIndex(), 16);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-015")));
    QCOMPARE(resultSet.itemUrl(), QUrl(QLatin1String("file:///a/015")));
    QCOMPARE(resultSet.itemType(), QLatin1String("typea"));
    QCOMPARE(resultSet.metaData(0), QVariant());
    QCOMPARE(resultSet.metaData(1), QVariant());
    QCOMPARE(resultSet.metaData(2), QVariant(QLatin1String("typea")));
    QCOMPARE(resultSet.metaData(3), QVariant(QLatin1String("a")));
    QCOMPARE(resultSet.metaData(4), QVariant(QLatin1String("015")));
    QCOMPARE(resultSet.metaData(5), QVariant(QLatin1String("query")));
    QCOMPARE(resultSet.metaData(6), QVariant(QLatin1String("a|015")));
    QCOMPARE(resultSet.metaData(7), QVariant(QLatin1String("015")));
    QCOMPARE(resultSet.metaData(8), QVariant());
    {
        QMap<int, QVariant> attributes;
        attributes.insert(2, QLatin1String("typea"));
        attributes.insert(3, QLatin1String("a"));

        QCOMPARE(resultSet.resources(), QList<QGalleryResource>()
                 << QGalleryResource(QUrl(QLatin1String("file:///a/015")), attributes));
    }

    QCOMPARE(resultSet.setMetaData(5, QLatin1String("edit")), false);
    QCOMPARE(resultSet.metaData(5), QVariant(QLatin1String("query")));

    QCOMPARE(resultSet.fetchNext(), false);
    QCOMPARE(resultSet.currentIndex(), 17);
    QCOMPARE(resultSet.itemId(), QVariant());
    QCOMPARE(resultSet.itemUrl(), QUrl());
    QCOMPARE(resultSet.itemType(), QString());
    QCOMPARE(resultSet.metaData(0), QVariant());
    QCOMPARE(resultSet.metaData(1), QVariant());
    QCOMPARE(resultSet.metaData(2), QVariant());
    QCOMPARE(resultSet.metaData(3), QVariant());
    QCOMPARE(resultSet.metaData(4), QVariant());
    QCOMPARE(resultSet.metaData(5), QVariant());
    QCOMPARE(resultSet.metaData(6), QVariant());
    QCOMPARE(resultSet.metaData(7), QVariant());
    QCOMPARE(resultSet.metaData(8), QVariant());
    QCOMPARE(resultSet.resources(), QList<QGalleryResource>());
}


void tst_QGalleryTrackerResultSet::refresh()
{
    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerResultSet resultSet(&arguments, true, 0, 32);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(resultSet.fetch(0), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(resultSet.fetch(15), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-015")));

    resultSet.refresh();
    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(resultSet.fetch(0), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(resultSet.fetch(15), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-015")));
}

void tst_QGalleryTrackerResultSet::reset()
{
    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerResultSet resultSet(&arguments, true, 0, 32);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(resultSet.fetch(0), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(resultSet.fetch(15), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-015")));

    m_queryAdaptor->setCount('a', 0);
    m_queryAdaptor->setCount('b', 16);

    resultSet.refresh();
    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(), 16);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(resultSet.fetch(0), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(resultSet.fetch(15), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-015")));
}


void tst_QGalleryTrackerResultSet::removeItem()
{
    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('b', 2);
    m_queryAdaptor->setCount('c', 8);

    QGalleryTrackerResultSet resultSet(&arguments, true, 0, 32);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 18);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 18);

    QCOMPARE(resultSet.fetch(7), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(resultSet.fetch(8), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(resultSet.fetch(10), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('b', 0);

    resultSet.refresh();
    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  2);

    QCOMPARE(resultSet.fetch(7), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(resultSet.fetch(8), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerResultSet::insertItem()
{
    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('c', 8);

    QGalleryTrackerResultSet resultSet(&arguments, true, 0, 32);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(resultSet.fetch(7), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(resultSet.fetch(8), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('b', 2);

    resultSet.refresh();
    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 18);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  2);

    QCOMPARE(resultSet.fetch(7), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(resultSet.fetch(8), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(resultSet.fetch(10), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerResultSet::replaceFirstItem()
{
    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 1);
    m_queryAdaptor->setCount('c', 15);

    QGalleryTrackerResultSet resultSet(&arguments, true, 0, 32);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);


    QCOMPARE(resultSet.fetch(0), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-000")));

    QCOMPARE(resultSet.fetch(1), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('a', 0);
    m_queryAdaptor->setCount('b', 1);

    resultSet.refresh();
    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  1);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  1);

    QCOMPARE(resultSet.fetch(0), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(resultSet.fetch(1), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));
}


void tst_QGalleryTrackerResultSet::replaceLastItem()
{
    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 15);
    m_queryAdaptor->setCount('b', 1);

    QGalleryTrackerResultSet resultSet(&arguments, true, 0, 32);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(resultSet.fetch(14), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-014")));

    QCOMPARE(resultSet.fetch(15), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-000")));

    m_queryAdaptor->setCount('b', 0);
    m_queryAdaptor->setCount('c', 1);

    resultSet.refresh();
    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(), 15);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  1);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(), 15);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  1);


    QCOMPARE(resultSet.fetch(14), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-014")));

    QCOMPARE(resultSet.fetch(15), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerResultSet::replaceMiddleItem()
{
    QGalleryTrackerResultSetArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('b', 2);
    m_queryAdaptor->setCount('d', 6);

    QGalleryTrackerResultSet resultSet(&arguments, true, 0, 32);

    QSignalSpy insertSpy(&resultSet, SIGNAL(itemsInserted(int,int)));
    QSignalSpy removeSpy(&resultSet, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy changeSpy(&resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)));

    QCOMPARE(resultSet.isActive(), true);
    QCOMPARE(resultSet.itemCount(), 0);

    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  0);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(), 16);

    QCOMPARE(resultSet.fetch(7), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(resultSet.fetch(8), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-000")));

    QCOMPARE(resultSet.fetch(9), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("b-001")));

    QCOMPARE(resultSet.fetch(10), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("d-000")));

    m_queryAdaptor->setCount('b', 0);
    m_queryAdaptor->setCount('c', 2);

    resultSet.refresh();
    QVERIFY(resultSet.waitForFinished(1000));

    QCOMPARE(resultSet.isActive(), false);
    QCOMPARE(resultSet.itemCount(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(changeSpy.count(), 0);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(removeSpy.at(removeSpy.count() - 1).value(1).toInt(),  2);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(0).toInt(),  8);
    QCOMPARE(insertSpy.at(insertSpy.count() - 1).value(1).toInt(),  2);

    QCOMPARE(resultSet.fetch(7), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("a-007")));

    QCOMPARE(resultSet.fetch(8), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-000")));

    QCOMPARE(resultSet.fetch(9), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("c-001")));

    QCOMPARE(resultSet.fetch(10), true);
    QCOMPARE(resultSet.itemId(), QVariant(QLatin1String("d-000")));
}


QTEST_MAIN(tst_QGalleryTrackerResultSet)

#include "tst_qgallerytrackerresultset.moc"

