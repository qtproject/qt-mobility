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

#include <maemo5/qgallerytrackeritemlist_p.h>
#include <maemo5/qgallerytrackerlistcolumn_p.h>

#include <QtDBus/qdbusabstractadaptor.h>
#include <QtDBus/qdbusmetatype.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_USE_NAMESPACE

class QtTestQueryAdaptor;

class tst_QGalleryTrackerItemList : public QObject
{
    Q_OBJECT
public:
    tst_QGalleryTrackerItemList()
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
//private:
    void query();
    void refresh();
    void reset();
    void removeItem();
    void insertItem();
    void replaceFirstItem();
    void replaceLastItem();
    void replaceMiddleItem();
    void scrollDownOverlapping();
    void scrollUpOverlapping();
    void scrollJitterOverlapping();
    void scrollDownNoOverlap();
    void scrollUpNoOverlap();
    void scrollJitterNoOverlap();

private:
    void populateArguments(
            QGalleryTrackerItemListArguments *arguments, const QString &argument);

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
    Q_CLASSINFO("D-Bus Interface", "com.nokia.QtTest.QGalleryTrackerItemList.Query")
public:
    QtTestQueryAdaptor(QObject *object) : QDBusAbstractAdaptor(object)
    {
        qMemSet(m_counts, 0, sizeof(m_counts));
    }

public:
    int count(char id) const { return m_counts[id - 'a']; }
    void setCount(char id, int count) { m_counts[id - 'a'] = count; }

    void clear() { qMemSet(m_counts, 0, sizeof(m_counts)); }

public Q_SLOTS:
    QVector<QStringList> GetResults(const QString &argument, int offset, int limit)
    {
        QVector<QStringList> results;

        for (int c = 0; c < 26; ++c) {
            const QString character = QChar(QLatin1Char('a' + c));

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
    int m_counts[26];
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

class QtTestImageColumn : public QGalleryTrackerImageColumn
{
    Q_OBJECT
public:
    QtTestImageColumn(int column) : m_column(column) {}

    void insertImages(
            int index, int count, QVector<QVariant>::const_iterator begin, int tableWidth)
    {
        typedef QVector<QVariant>::const_iterator iterator;
        for (iterator it = begin + m_column; index < count; ++index, it += tableWidth)
            m_images.insert(index, *it);
    }

    void removeImages(int index, int count)
    {
        m_images.remove(index, count);
    }

private:
    const int m_column;
};

void tst_QGalleryTrackerItemList::initTestCase()
{
    const QString service
            = QLatin1String("com.nokia.QtTest.QGalleryTrackerItemList")
              + QString::number(QCoreApplication::applicationPid());
    const QString path = QLatin1String("/");
    const char *interface = "com.nokia.QtTest.QGalleryTrackerItemList.Query";

    qDBusRegisterMetaType<QVector<QStringList> >();

    m_queryAdaptor = new QtTestQueryAdaptor(this);

    m_connection.registerService(service);
    m_connection.registerObject(path, this);

    m_queryInterface = new QGalleryDBusInterface(service, path, interface);

    QVERIFY(m_queryInterface->isValid());
}

void tst_QGalleryTrackerItemList::cleanup()
{
    m_queryAdaptor->clear();
}

void tst_QGalleryTrackerItemList::populateArguments(
        QGalleryTrackerItemListArguments *arguments, const QString &argument)
{
    arguments->idColumn = new QtTestIdentityColumn(0);
    arguments->urlColumn = new QtTestIdentityColumn(1);
    arguments->typeColumn = new QtTestIdentityColumn(2);
    arguments->updateMask = 0x01;
    arguments->identityWidth = 1;
    arguments->valueOffset = 2;
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
            << m_alias
            << m_image;
    arguments->fieldNames = QStringList();
    arguments->propertyAttributes = QVector<QGalleryProperty::Attributes>()
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead)
            << (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort)
            << (QGalleryProperty::CanRead);
    arguments->valueColumns = QVector<QGalleryTrackerValueColumn *>()
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn
            << new QtTestStringValueColumn;
    arguments->compositeColumns = QVector<QGalleryTrackerCompositeColumn *>()
            << new QtTestCompositeColumn(3, 4);
    arguments->aliasColumns = QVector<int>()
            << 4;
    arguments->imageColumns = QVector<QGalleryTrackerImageColumn *>()
            << new QtTestImageColumn(0);
    arguments->sortCriteria = QVector<QGalleryTrackerSortCriteria>();
    arguments->resourceKeys = QVector<int>()
            << 2
            << 3;
}

void tst_QGalleryTrackerItemList::query()
{
    const QStringList propertyNames = QStringList()
            << m_itemType
            << m_character
            << m_number
            << m_argument
            << m_composite
            << m_alias
            << m_image;

    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerItemList list(arguments, false, 0, 32);
    QCOMPARE(list.propertyNames(), propertyNames);
    QCOMPARE(list.propertyKey(m_itemType), 2);
    QCOMPARE(list.propertyKey(m_alias), 7);
    QCOMPARE(list.propertyAttributes(2), (QGalleryProperty::CanRead | QGalleryProperty::CanFilter));
    QCOMPARE(list.propertyAttributes(7), (QGalleryProperty::CanRead | QGalleryProperty::CanFilter | QGalleryProperty::CanSort));

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.url(0), QUrl(QLatin1String("file:///a/000")));
    QCOMPARE(list.type(0), QLatin1String("typea"));
    QCOMPARE(list.metaData(0, 2), QVariant(QLatin1String("typea")));
    QCOMPARE(list.metaData(0, 3), QVariant(QLatin1String("a")));
    QCOMPARE(list.metaData(0, 4), QVariant(QLatin1String("000")));
    QCOMPARE(list.metaData(0, 5), QVariant(QLatin1String("query")));
    QCOMPARE(list.metaData(0, 6), QVariant(QLatin1String("a|000")));
    QCOMPARE(list.metaData(0, 7), QVariant(QLatin1String("000")));
//    QCOMPARE(list.metaData(0, 8), QVariant(QLatin1String("a-000")));

    QCOMPARE(list.id(15), QVariant(QLatin1String("a-015")));
    QCOMPARE(list.url(15), QUrl(QLatin1String("file:///a/015")));
    QCOMPARE(list.type(15), QLatin1String("typea"));
    QCOMPARE(list.metaData(15, 2), QVariant(QLatin1String("typea")));
    QCOMPARE(list.metaData(15, 3), QVariant(QLatin1String("a")));
    QCOMPARE(list.metaData(15, 4), QVariant(QLatin1String("015")));
    QCOMPARE(list.metaData(15, 5), QVariant(QLatin1String("query")));
    QCOMPARE(list.metaData(15, 6), QVariant(QLatin1String("a|015")));
    QCOMPARE(list.metaData(15, 7), QVariant(QLatin1String("015")));
//    QCOMPARE(list.metaData(15, 8), QVariant(QLatin1String("a-015")));
}


void tst_QGalleryTrackerItemList::refresh()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);


    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(15), QVariant(QLatin1String("a-015")));

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(15), QVariant(QLatin1String("a-015")));
}

void tst_QGalleryTrackerItemList::reset()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 16);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(15), QVariant(QLatin1String("a-015")));

    m_queryAdaptor->setCount('a', 0);
    m_queryAdaptor->setCount('b', 16);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().value(0).toInt(), 0);
    QCOMPARE(removeSpy.last().value(1).toInt(), 16);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(15), QVariant(QLatin1String("b-015")));
}


void tst_QGalleryTrackerItemList::removeItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('b', 2);
    m_queryAdaptor->setCount('c', 8);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 18);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 18);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(7), QVariant(QLatin1String("a-007")));
    QCOMPARE(list.id(8), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(10), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('b', 0);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().value(0).toInt(), 8);
    QCOMPARE(removeSpy.last().value(1).toInt(), 2);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(7), QVariant(QLatin1String("a-007")));
    QCOMPARE(list.id(8), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerItemList::insertItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('c', 8);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(7), QVariant(QLatin1String("a-007")));
    QCOMPARE(list.id(8), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('b', 2);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 18);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 8);
    QCOMPARE(insertSpy.last().value(1).toInt(), 2);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(7), QVariant(QLatin1String("a-007")));
    QCOMPARE(list.id(8), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(10), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerItemList::replaceFirstItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 1);
    m_queryAdaptor->setCount('c', 15);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(1), QVariant(QLatin1String("c-000")));

    m_queryAdaptor->setCount('a', 0);
    m_queryAdaptor->setCount('b', 1);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 1);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().value(0).toInt(), 0);
    QCOMPARE(removeSpy.last().value(1).toInt(), 1);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(1), QVariant(QLatin1String("c-000")));
}


void tst_QGalleryTrackerItemList::replaceLastItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 15);
    m_queryAdaptor->setCount('b', 1);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(14), QVariant(QLatin1String("a-014")));
    QCOMPARE(list.id(15), QVariant(QLatin1String("b-000")));

    m_queryAdaptor->setCount('b', 0);
    m_queryAdaptor->setCount('c', 1);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 15);
    QCOMPARE(insertSpy.last().value(1).toInt(), 1);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().value(0).toInt(), 15);
    QCOMPARE(removeSpy.last().value(1).toInt(), 1);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(14), QVariant(QLatin1String("a-014")));
    QCOMPARE(list.id(15), QVariant(QLatin1String("c-000")));
}

void tst_QGalleryTrackerItemList::replaceMiddleItem()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    m_queryAdaptor->setCount('a', 8);
    m_queryAdaptor->setCount('b', 2);
    m_queryAdaptor->setCount('d', 6);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 16);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(7), QVariant(QLatin1String("a-007")));
    QCOMPARE(list.id(8), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(9), QVariant(QLatin1String("b-001")));
    QCOMPARE(list.id(10), QVariant(QLatin1String("d-000")));

    m_queryAdaptor->setCount('b', 0);
    m_queryAdaptor->setCount('c', 2);

    list.refresh();
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 16);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 8);
    QCOMPARE(insertSpy.last().value(1).toInt(), 2);
    QCOMPARE(removeSpy.count(), 1);
    QCOMPARE(removeSpy.last().value(0).toInt(), 8);
    QCOMPARE(removeSpy.last().value(1).toInt(), 2);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(7), QVariant(QLatin1String("a-007")));
    QCOMPARE(list.id(8), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(9), QVariant(QLatin1String("c-001")));
    QCOMPARE(list.id(10), QVariant(QLatin1String("d-000")));
}

void tst_QGalleryTrackerItemList::scrollDownOverlapping()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    for (char c = 'a'; c <= 'z'; ++c)
        m_queryAdaptor->setCount(c, 100);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 256);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 256);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant());
    QCOMPARE(list.id(400), QVariant());
    QCOMPARE(list.id(500), QVariant());

    list.setCursorPosition(256);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 448);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 256);
    QCOMPARE(insertSpy.last().value(1).toInt(), 192);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant(QLatin1String("d-000")));
    QCOMPARE(list.id(400), QVariant(QLatin1String("e-000")));
    QCOMPARE(list.id(500), QVariant());
}

void tst_QGalleryTrackerItemList::scrollUpOverlapping()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    for (char c = 'a'; c <= 'z'; ++c)
        m_queryAdaptor->setCount(c, 100);

    QGalleryTrackerItemList list(arguments, true, 192, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 448);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 448);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant(QLatin1String("d-000")));
    QCOMPARE(list.id(400), QVariant(QLatin1String("e-000")));
    QCOMPARE(list.id(500), QVariant());

    list.setCursorPosition(0);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 448);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 1);
    QCOMPARE(changeSpy.last().value(0).toInt(), 0);
    QCOMPARE(changeSpy.last().value(1).toInt(), 192);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant());
    QCOMPARE(list.id(400), QVariant());
    QCOMPARE(list.id(500), QVariant());
}

void tst_QGalleryTrackerItemList::scrollJitterOverlapping()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    for (char c = 'a'; c <= 'z'; ++c)
        m_queryAdaptor->setCount(c, 100);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 256);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 256);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant());
    QCOMPARE(list.id(400), QVariant());
    QCOMPARE(list.id(500), QVariant());

    list.setCursorPosition(256);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 448);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 256);
    QCOMPARE(insertSpy.last().value(1).toInt(), 192);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant(QLatin1String("d-000")));
    QCOMPARE(list.id(400), QVariant(QLatin1String("e-000")));
    QCOMPARE(list.id(500), QVariant());

    list.setCursorPosition(0);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 448);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 1);
    QCOMPARE(changeSpy.last().value(0).toInt(), 0);
    QCOMPARE(changeSpy.last().value(1).toInt(), 192);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant());
    QCOMPARE(list.id(400), QVariant());
    QCOMPARE(list.id(500), QVariant());

    list.setCursorPosition(256);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 448);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 2);
    QCOMPARE(changeSpy.last().value(0).toInt(), 256);
    QCOMPARE(changeSpy.last().value(1).toInt(), 192);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant(QLatin1String("d-000")));
    QCOMPARE(list.id(400), QVariant(QLatin1String("e-000")));
    QCOMPARE(list.id(500), QVariant());
}

void tst_QGalleryTrackerItemList::scrollDownNoOverlap()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    for (char c = 'a'; c <= 'z'; ++c)
        m_queryAdaptor->setCount(c, 100);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 256);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 256);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(500), QVariant());
    QCOMPARE(list.id(600), QVariant());
    QCOMPARE(list.id(700), QVariant());

    list.setCursorPosition(512);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 704);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 256);
    QCOMPARE(insertSpy.last().value(1).toInt(), 448);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant());
    QCOMPARE(list.id(500), QVariant(QLatin1String("f-000")));
    QCOMPARE(list.id(600), QVariant(QLatin1String("g-000")));
    QCOMPARE(list.id(700), QVariant(QLatin1String("h-000")));
}

void tst_QGalleryTrackerItemList::scrollUpNoOverlap()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    for (char c = 'a'; c <= 'z'; ++c)
        m_queryAdaptor->setCount(c, 100);

    QGalleryTrackerItemList list(arguments, true, 448, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 704);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 704);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant());
    QCOMPARE(list.id(500), QVariant(QLatin1String("f-000")));
    QCOMPARE(list.id(600), QVariant(QLatin1String("g-000")));
    QCOMPARE(list.id(700), QVariant(QLatin1String("h-000")));

    list.setCursorPosition(0);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 704);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 1);
    QCOMPARE(changeSpy.last().value(0).toInt(), 0);
    QCOMPARE(changeSpy.last().value(1).toInt(), 256);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(500), QVariant());
    QCOMPARE(list.id(600), QVariant());
    QCOMPARE(list.id(700), QVariant());
}

void tst_QGalleryTrackerItemList::scrollJitterNoOverlap()
{
    QGalleryTrackerItemListArguments arguments;

    populateArguments(&arguments, QLatin1String("query"));

    for (char c = 'a'; c <= 'z'; ++c)
        m_queryAdaptor->setCount(c, 100);

    QGalleryTrackerItemList list(arguments, true, 0, 32);

    QSignalSpy insertSpy(&list, SIGNAL(inserted(int,int)));
    QSignalSpy removeSpy(&list, SIGNAL(removed(int,int)));
    QSignalSpy changeSpy(&list, SIGNAL(metaDataChanged(int,int)));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::NoResult));
    QCOMPARE(list.count(), 0);

    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 256);
    QCOMPARE(insertSpy.count(), 1);
    QCOMPARE(insertSpy.last().value(0).toInt(), 0);
    QCOMPARE(insertSpy.last().value(1).toInt(), 256);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant());
    QCOMPARE(list.id(400), QVariant());
    QCOMPARE(list.id(500), QVariant());

    list.setCursorPosition(512);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 704);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(insertSpy.last().value(0).toInt(), 256);
    QCOMPARE(insertSpy.last().value(1).toInt(), 448);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 0);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant());
    QCOMPARE(list.id(500), QVariant(QLatin1String("f-000")));
    QCOMPARE(list.id(600), QVariant(QLatin1String("g-000")));
    QCOMPARE(list.id(700), QVariant(QLatin1String("h-000")));


    list.setCursorPosition(0);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 704);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 1);
    QCOMPARE(changeSpy.last().value(0).toInt(), 0);
    QCOMPARE(changeSpy.last().value(1).toInt(), 256);

    QCOMPARE(list.id(0), QVariant(QLatin1String("a-000")));
    QCOMPARE(list.id(100), QVariant(QLatin1String("b-000")));
    QCOMPARE(list.id(200), QVariant(QLatin1String("c-000")));
    QCOMPARE(list.id(300), QVariant());
    QCOMPARE(list.id(400), QVariant());
    QCOMPARE(list.id(500), QVariant());

    list.setCursorPosition(512);
    QVERIFY(list.waitForFinished(1000));

    QCOMPARE(list.result(), int(QGalleryAbstractRequest::Succeeded));
    QCOMPARE(list.count(), 704);
    QCOMPARE(insertSpy.count(), 2);
    QCOMPARE(removeSpy.count(), 0);
    QCOMPARE(changeSpy.count(), 2);
    QCOMPARE(changeSpy.last().value(0).toInt(), 448);
    QCOMPARE(changeSpy.last().value(1).toInt(), 256);

    QCOMPARE(list.id(0), QVariant());
    QCOMPARE(list.id(100), QVariant());
    QCOMPARE(list.id(200), QVariant());
    QCOMPARE(list.id(500), QVariant(QLatin1String("f-000")));
    QCOMPARE(list.id(600), QVariant(QLatin1String("g-000")));
    QCOMPARE(list.id(700), QVariant(QLatin1String("h-000")));
}

QTEST_MAIN(tst_QGalleryTrackerItemList)

#include "tst_qgallerytrackeritemlist.moc"

