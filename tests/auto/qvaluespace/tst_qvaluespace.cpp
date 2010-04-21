/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qvaluespace_p.h>
#include <qvaluespacesubscriber.h>
#include <qvaluespacepublisher.h>

#include <QtTest/QTest>
#include <QSet>
#include <QFile>

#include <QDebug>

QTM_USE_NAMESPACE
class FakeLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT

public:
    QString name() { return QString("Fake Layer"); }

    bool startup(Type) { return true; }
    void shutdown() { }

    QUuid id() { return QUuid(0x76c95ddc, 0xe38d, 0x42f8, 0xb2,
                              0x54, 0x21, 0xff, 0xa3, 0x63, 0x74, 0xf7); }
    unsigned int order() { return 0x20000000; }

    Handle item(Handle parent, const QString &subPath);
    void removeHandle(Handle handle);
    void setProperty(Handle, Properties) { }

    bool value(Handle handle, QVariant *data);
    bool value(Handle handle, const QString &subPath, QVariant *data);
    QSet<QString> children(Handle handle);

    QValueSpace::LayerOptions layerOptions() const;

    /* QValueSpaceSubscriber functions */
    bool supportsInterestNotification() const;
    bool notifyInterest(Handle handle, bool interested);

    /* QValueSpacePublisher functions */
    bool setValue(QValueSpacePublisher *creator, Handle handle,
                  const QString &subPath, const QVariant &value);
    bool removeValue(QValueSpacePublisher *creator, Handle handle, const QString &subPath);
    bool removeSubTree(QValueSpacePublisher *creator, Handle handle);
    void addWatch(QValueSpacePublisher *creator, Handle handle);
    void removeWatches(QValueSpacePublisher *creator, Handle parent);
    void sync() { }

    QStringList testErrors();

private:
    bool testPath(const QString &path);

    QMap<uint, QString> handles;
    QMap<uint, uint> interest;

    QStringList m_testErrors;
};

bool FakeLayer::testPath(const QString &path)
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

QAbstractValueSpaceLayer::Handle FakeLayer::item(Handle parent, const QString &subPath)
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

bool FakeLayer::value(Handle handle, const QString &subPath, QVariant *data)
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

QSet<QString> FakeLayer::children(Handle handle)
{
    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    return QSet<QString>();
}

QValueSpace::LayerOptions FakeLayer::layerOptions() const
{
    return QValueSpace::TransientLayer;
}

bool FakeLayer::supportsInterestNotification() const
{
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

    return true;
}

bool FakeLayer::setValue(QValueSpacePublisher *creator, Handle handle,
                         const QString &subPath, const QVariant &)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    testPath(subPath);

    return true;
}

bool FakeLayer::removeValue(QValueSpacePublisher *creator, Handle handle, const QString &subPath)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    testPath(subPath);

    return true;
}

bool FakeLayer::removeSubTree(QValueSpacePublisher *creator, Handle handle)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");

    return true;
}

void FakeLayer::addWatch(QValueSpacePublisher *creator, Handle handle)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(handle))
        m_testErrors << QLatin1String("Unknown handle");
}

void FakeLayer::removeWatches(QValueSpacePublisher *creator, Handle parent)
{
    if (!creator)
        m_testErrors << QLatin1String("creator is null");

    if (!handles.contains(parent))
        m_testErrors << QLatin1String("Unknown handle");
}

QStringList FakeLayer::testErrors()
{
    QStringList result = m_testErrors;
    m_testErrors.clear();
    return result;
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
#ifdef Q_OS_UNIX
    QFile::remove("/tmp/qt-0/valuespace_shmlayer");
#endif

    qRegisterMetaType<QVariant>("QVariant");

    fakeLayer = new FakeLayer;
    QValueSpace::installLayer(fakeLayer);
    QValueSpace::initValueSpaceServer();
}

void tst_QValueSpace::cleanupTestCase()
{
    delete fakeLayer;
}

void tst_QValueSpace::availableLayers()
{
    QList<QUuid> layers = QValueSpace::availableLayers();

#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QVERIFY(layers.contains(QVALUESPACE_SHAREDMEMORY_LAYER));
#else
    QVERIFY(!layers.contains(QVALUESPACE_SHAREDMEMORY_LAYER));
#endif

#ifdef Q_OS_WIN
    QVERIFY(layers.contains(QVALUESPACE_VOLATILEREGISTRY_LAYER));
    QVERIFY(layers.contains(QVALUESPACE_NONVOLATILEREGISTRY_LAYER));
#else
    QVERIFY(!layers.contains(QVALUESPACE_VOLATILEREGISTRY_LAYER));
    QVERIFY(!layers.contains(QVALUESPACE_NONVOLATILEREGISTRY_LAYER));
#endif

#ifdef Q_WS_MAEMO_5
    QVERIFY(layers.contains(QVALUESPACE_GCONF_LAYER));
#endif

#ifdef Q_WS_MAEMO_6
    QVERIFY(layers.contains(QVALUESPACE_CONTEXTKIT_LAYER));
#else
    QVERIFY(!layers.contains(QVALUESPACE_CONTEXTKIT_LAYER));
#endif

    QVERIFY(layers.contains(fakeLayer->id()));
}

#define CHECK_ERRORS(x) do { \
    x; \
    QStringList testErrors = fakeLayer->testErrors(); \
    if (!testErrors.isEmpty()) { \
        qDebug() << testErrors; \
        QFAIL(#x); \
    } \
} while (false)

void tst_QValueSpace::layerInterface_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("attribute");

    QStringList paths;
    paths << QString() << QString("/") << QString("//")
          << QString("layerInterface") << QString("layerInterface/")
          << QString("/layerInterface") << QString("//layerInterface")
          << QString("/layerInterface/") << QString("/layerInterface//")
          << QString("/layerInterface/subpath") << QString("/layerInterface//subpath")
          << QString("/layerInterface/subpath/") << QString("/layerInterface/subpath//");

    QStringList attributes;
    attributes << QString()
               << QString("/")
               << QString("//")
               << QString("value")
               << QString("value/")
               << QString("value//")
               << QString("/value")
               << QString("//value")
               << QString("/value/")
               << QString("subpath/value")
               << QString("subpath//value");

    foreach (const QString &path, paths) {
        foreach (const QString &attribute, attributes) {
            QString id = QString("p:%1 a:%2").arg(path, attribute);
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
    QFETCH(QString, attribute);

    QValueSpaceSubscriber *subscriber;

    CHECK_ERRORS(subscriber = new QValueSpaceSubscriber(path));

    CHECK_ERRORS(subscriber->value(attribute));
    CHECK_ERRORS(subscriber->children());

    CHECK_ERRORS(delete subscriber);


    QValueSpacePublisher *publisher;

    CHECK_ERRORS(publisher = new QValueSpacePublisher(fakeLayer->id(), path));

    QVERIFY(publisher->isConnected());

    CHECK_ERRORS(publisher->setValue(attribute, 10));
    CHECK_ERRORS(publisher->resetValue(attribute));

    SignalSink sink;
    CHECK_ERRORS(connect(publisher, SIGNAL(interestChanged(QString,bool)),
                         &sink, SLOT(slot())));

    CHECK_ERRORS(delete publisher);
}

QTEST_MAIN(tst_QValueSpace)
#include "tst_qvaluespace.moc"
