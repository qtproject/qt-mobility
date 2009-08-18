/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDebug>
#include <qabstractmediaobject.h>
#include <qabstractmediacontrol.h>
#include <qabstractmediaservice.h>
#include <qmetadataprovidercontrol.h>
#include <qmediametadata.h>

class MockProvider : public QMetadataProviderControl
{
    Q_OBJECT

public:
    MockProvider(QObject *parent):
        QMetadataProviderControl(parent),
        m_metadataAvailable(false),
        m_readOnly(true) {}

    bool metadataAvailable() const
    {
        return m_metadataAvailable;
    }

    bool isReadOnly() const
    {
        return m_readOnly;
    }

    QList<QString> availableMetadata() const
    {
        return data.keys();
    }

    QVariant metadata(QString const &name) const
    {
        return data[name];
    }

    void setMetadata(QString const &name, QVariant const &value)
    {
        if (!m_readOnly)
            data[name] = value;
    }

    void setNoMetadata()
    {
        m_metadataAvailable = false;
        data.clear();
    }

    void setMetadata()
    {
        m_metadataAvailable = true;
        data.insert("Artist", "Dead Can Dance");
        data.insert("Title", "Host of Seraphim");
        data.insert("Genre", "Awesome");
    }

    void setNoReadOnly()
    {
        m_readOnly = false;
    }

    void setReadOnly()
    {
        m_readOnly = true;
    }

    bool m_metadataAvailable;
    bool m_readOnly;
    QMap<QString, QVariant> data;
};

class MockService : public QAbstractMediaService
{
    Q_OBJECT
public:
    MockService(QObject *parent, QAbstractMediaControl *control):
        QAbstractMediaService(parent),
        mockControl(control) {}

    QAbstractMediaControl* control(const char *) const
    {
        return mockControl;
    }

    QAbstractMediaControl   *mockControl;
};

class MockObject : public QAbstractMediaObject
{
    Q_OBJECT
public:
    MockObject(QObject *parent, QAbstractMediaControl *control):
        QAbstractMediaObject(parent),
        mockService(new MockService(this, control))
    {
    }

    bool isValid() const
    {
        return true;
    }

    QAbstractMediaService* service() const
    {
        return mockService;
    }

    QAbstractMediaService *mockService;
};

class tst_QMediaMetadata: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testMetadataAvailable();
    void testReadOnly();
    void testListAvailable();
    void testChanged();
    void testReadMetadata();
    void testWriteMetadata();

private:
    MockObject      *object;
    MockProvider    *mock;
    QMediaMetadata  *metadata;
};

void tst_QMediaMetadata::init()
{
    mock = new MockProvider(this);
    object = new MockObject(this, mock);
    metadata = new QMediaMetadata(object);
}

void tst_QMediaMetadata::cleanup()
{
}

void tst_QMediaMetadata::testMetadataAvailable()
{
    mock->setNoMetadata();
    QVERIFY(!metadata->metadataAvailable());

    mock->setMetadata();
    QVERIFY(metadata->metadataAvailable());

    // check signal
}

void tst_QMediaMetadata::testReadOnly()
{
    mock->setNoReadOnly();
    QVERIFY(!metadata->isReadOnly());

    mock->setReadOnly();
    QVERIFY(metadata->isReadOnly());

    // check signal
}

void tst_QMediaMetadata::testListAvailable()
{
    mock->setMetadata();
    QCOMPARE(metadata->availableMetadata().size(), 3);
}

void tst_QMediaMetadata::testChanged()
{
    // check signal
}

void tst_QMediaMetadata::testReadMetadata()
{
    mock->setMetadata();

    QCOMPARE(metadata->metadata("Artist").toString(), QString("Dead Can Dance"));
    QCOMPARE(metadata->metadata("Title").toString(), QString("Host of Seraphim"));
}

void tst_QMediaMetadata::testWriteMetadata()
{
    mock->setMetadata();
    mock->setNoReadOnly();

    metadata->setMetadata("Title", QLatin1String("In the Kingdom of the Blind the One eyed are Kings"));
    QCOMPARE(metadata->metadata("Title").toString(), QLatin1String("In the Kingdom of the Blind the One eyed are Kings"));
}


QTEST_MAIN(tst_QMediaMetadata)

#include "tst_qmediametadata.moc"
