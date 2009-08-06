/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in Technology Preview License Agreement accompanying
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qvaluespace.h>
#include <qvaluespaceobject.h>

#include <QtTest/QTest>
#include <QSet>

#include <QDebug>

class FakeLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT

public:
    QString name() { return QString("Fake Layer"); }

    bool startup(Type) { return true; }
    bool restart() { return true; }
    void shutdown() { }

    QUuid id() { return QUuid(0x76c95ddc, 0xe38d, 0x42f8, 0xb2,
                              0x54, 0x21, 0xff, 0xa3, 0x63, 0x74, 0xf7); }
    unsigned int order() { return 0x20000000; }

    Handle item(Handle parent, const QByteArray &subPath);
    void removeHandle(Handle handle);
    void setProperty(Handle handle, Properties properties) { }

    bool value(Handle handle, QVariant *data);
    bool value(Handle handle, const QByteArray &subPath, QVariant *data);
    QSet<QByteArray> children(Handle handle);

    /* QValueSpaceItem functions */
    bool supportsRequests() { return true; }
    bool requestSetValue(Handle handle, const QVariant &value);
    bool requestSetValue(Handle handle, const QByteArray &subPath, const QVariant &value);
    bool requestRemoveValue(Handle handle, const QByteArray &path = QByteArray());
    bool notifyInterest(Handle handle, bool interested);
    bool syncRequests() { return true; }

    /* QValueSpaceObject functions */
    bool setValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath, const QVariant &value);
    bool removeValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath);
    bool removeSubTree(QValueSpaceObject *creator, Handle handle);
    void addWatch(QValueSpaceObject *creator, Handle handle);
    void removeWatches(QValueSpaceObject *creator, Handle parent);
    void sync() { }

    QList<QString> testErrors() { QList<QString> result = m_testErrors; m_testErrors.clear(); return result; }

private:
    bool testPath(const QByteArray &path);

    QMap<uint, QByteArray> handles;
    QMap<uint, uint> interest;

    QList<QString> m_testErrors;
};

bool FakeLayer::testPath(const QByteArray &path)
{
    if (path.isEmpty())
        m_testErrors << QLatin1String("path is empty");
    else if (path.at(0) != '/') {
        m_testErrors << QLatin1String("path does not start with /");
    } else {
        if (path.length() > 1 && path.endsWith('/')) {
            m_testErrors << QLatin1String("path ends with /");
        } else if (path.contains("//")) {
            m_testErrors << QLatin1String("path contains //");
        } else {
            return true;
        }
    }

    return false;
}

QAbstractValueSpaceLayer::Handle FakeLayer::item(Handle parent, const QByteArray &subPath)
{
    if (parent == InvalidHandle) {
        if (testPath(subPath)) {
            if (!handles.contains(qHash(subPath)))
                handles.insert(qHash(subPath), subPath);
            return Handle(qHash(subPath));
        }
    } else {
        qDebug() << "relative" << subPath;
        m_testErrors << QLatin1String("Relative subPath, not an error not done yet");
    }

    return InvalidHandle;
}

void FakeLayer::removeHandle(Handle handle)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");
    else
        handles.remove(handle);
}

bool FakeLayer::value(Handle handle, QVariant *data)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    if (!data)
        m_testErrors << QLatin1String("data is null");

    *data = QVariant();
    return true;
}

bool FakeLayer::value(Handle handle, const QByteArray &subPath, QVariant *data)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    if (!data)
        m_testErrors << QLatin1String("data is null");

    if (testPath(subPath)) {
        *data = QVariant();
        return true;
    } else {
        return false;
    }
}

QSet<QByteArray> FakeLayer::children(Handle handle)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    return QSet<QByteArray>();
}

bool FakeLayer::requestSetValue(Handle handle, const QVariant &value)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    return true;
}

bool FakeLayer::requestSetValue(Handle handle, const QByteArray &subPath, const QVariant &value)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    testPath(subPath);

    return true;
}

bool FakeLayer::requestRemoveValue(Handle handle, const QByteArray &path)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    testPath(path);

    return true;
}

bool FakeLayer::notifyInterest(Handle handle, bool interested)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    if (interested) {
        if (interest.contains(handle))
            m_testErrors << QLatin1String("path has been registered for interest");
        ++interest[handle];
    } else {
        if (!interest.contains(handle))
            m_testErrors << QLatin1String("path has not been registered for interest");
        --interest[handle];
        if (interest[handle] == 0)
            interest.remove(handle);
    }
}

bool FakeLayer::setValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath, const QVariant &value)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    testPath(subPath);

    return true;
}

bool FakeLayer::removeValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    testPath(subPath);

    return true;
}

bool FakeLayer::removeSubTree(QValueSpaceObject *creator, Handle handle)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    return true;
}

void FakeLayer::addWatch(QValueSpaceObject *creator, Handle handle)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");
}

void FakeLayer::removeWatches(QValueSpaceObject *creator, Handle parent)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(parent))
        m_testErrors << QLatin1String("Unknown handle");
}

class tst_QValueSpace: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void availableLayers();

    void layerInterface_data();
    void layerInterface();

private:
    FakeLayer *fakeLayer;
};

void tst_QValueSpace::initTestCase()
{
    qRegisterMetaType<QVariant>("QVariant");

    fakeLayer = new FakeLayer;
    QValueSpace::installLayer(fakeLayer);
    QValueSpace::initValuespaceManager();
}

void tst_QValueSpace::cleanupTestCase()
{
    delete fakeLayer;
}

void tst_QValueSpace::availableLayers()
{
    QList<QUuid> layers = QValueSpace::availableLayers();

#ifdef Q_OS_UNIX
    QVERIFY(layers.contains(QVALUESPACE_APPLICATION_LAYER));
#else
    QVERIFY(!layers.contains(QVALUESPACE_APPLICATION_LAYER));
#endif

#ifdef Q_OS_WIN
    QVERIFY(layers.contains(QVALUESPACE_REGISTRY_LAYER));
#else
    QVERIFY(!layers.contains(QVALUESPACE_REGISTRY_LAYER));
#endif

    QVERIFY(layers.contains(fakeLayer->id()));
}

#define CHECK_ERRORS(x) do { \
    x; \
    QList<QString> testErrors = fakeLayer->testErrors(); \
    if (!testErrors.isEmpty()) { \
        qDebug() << testErrors; \
        QFAIL(#x); \
    } \
} while (false)

void tst_QValueSpace::layerInterface_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QByteArray>("attribute");

    QList<QString> paths;
    paths << QString() << QString("/") << QString("//")
          << QString("layerInterface") << QString("layerInterface/")
          << QString("/layerInterface") << QString("//layerInterface")
          << QString("/layerInterface/") << QString("/layerInterface//")
          << QString("/layerInterface/subpath") << QString("/layerInterface//subpath")
          << QString("/layerInterface/subpath/") << QString("/layerInterface/subpath//");

    QList<QByteArray> attributes;
    attributes << QByteArray()
               << QByteArray("/")
               << QByteArray("//")
               << QByteArray("value")
               << QByteArray("value/")
               << QByteArray("value//")
               << QByteArray("/value")
               << QByteArray("//value")
               << QByteArray("/value/")
               << QByteArray("subpath/value")
               << QByteArray("subpath//value");

    foreach (const QString &path, paths) {
        foreach (const QByteArray &attribute, attributes) {
            QString id = QString("p:%1 a:%2").arg(QString(path), QString(attribute));
            QTest::newRow(id.toLocal8Bit().constData()) << path << attribute;
        }
    }
}

class SignalSink : public QObject
{
    Q_OBJECT

public slots:
    void slot() { }
};

void tst_QValueSpace::layerInterface()
{
    QFETCH(QString, path);
    QFETCH(QByteArray, attribute);

    QValueSpaceItem *item;

    CHECK_ERRORS(item = new QValueSpaceItem(path));

    CHECK_ERRORS(item->value(attribute));
    CHECK_ERRORS(item->children());
    CHECK_ERRORS(item->setValue(10); item->sync());
    CHECK_ERRORS(item->setValue(attribute, 10); item->sync());
    CHECK_ERRORS(item->remove(); item->sync());
    CHECK_ERRORS(item->remove(attribute); item->sync());

    CHECK_ERRORS(delete item);


    QValueSpaceObject *object;

    CHECK_ERRORS(object = new QValueSpaceObject(path, fakeLayer->id()));

    QVERIFY(object->isValid());
    QVERIFY(object->supportsRequests());

    CHECK_ERRORS(object->setAttribute(attribute, 10));
    CHECK_ERRORS(object->removeAttribute(attribute));

    SignalSink sink;
    CHECK_ERRORS(connect(object, SIGNAL(itemNotify(QByteArray,bool)), &sink, SLOT(slot())));

    CHECK_ERRORS(delete object);
}

QTEST_MAIN(tst_QValueSpace)
#include "tst_qvaluespace.moc"
