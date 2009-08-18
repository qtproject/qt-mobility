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
#include <qmediastreamscontrol.h>
#include <qmediastreams.h>

class MockProvider : public QMediaStreamsControl
{
    Q_OBJECT

public:
    MockProvider(QObject *parent):
        QMediaStreamsControl(parent)
    {}

    virtual ~MockProvider() {}

    int streamCount()
    {
        return m_streamTypes.count();
    };

    QMediaStreamInfo::StreamType streamType(int streamNumber)
    {
        return m_streamTypes[streamNumber];
    };

    QMap<QString,QVariant> streamProperties(int streamNumber)
    {
        return m_streamProperties[streamNumber];
    };

    bool isActive(int streamNumber)
    {
        return m_activeStreams.contains(streamNumber);
    };

    void setActive(int streamNumber, bool state)
    {
        if (isActive(streamNumber) != state) {
            if (state) {
                m_activeStreams.insert(streamNumber);
            } else {
                m_activeStreams.remove(streamNumber);
            }

            emit activeStreamsChanged();
        }
    };

    void addStream(QMediaStreamInfo::StreamType type, const QMap<QString,QVariant> &properties )
    {
        m_streamTypes.append(type);
        m_streamProperties.append(properties);

        emit streamsChanged();
    }

    void addStream(QMediaStreamInfo::StreamType type, const QString &name, const QString &language)
    {
        QMap<QString,QVariant> streamProperties;
        streamProperties["Name"] = name;
        streamProperties["Language"] = language;

        addStream(type, streamProperties);
    }

    void clearStreams()
    {
        m_streamTypes.clear();
        m_streamProperties.clear();

        emit streamsChanged();
    }


public:
    QList<QMediaStreamInfo::StreamType> m_streamTypes;
    QList< QMap<QString,QVariant> > m_streamProperties;
    QSet<int> m_activeStreams;
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

class tst_QMediaStreams: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testIsStreamInfoAvailable();
    void testNewStream();
    void testStreamProperties();
    void testDefaultLanguage();

private:
    MockObject      *object;
    MockProvider    *mock;
    QMediaStreams   *streams;
};

void tst_QMediaStreams::init()
{
    mock = new MockProvider(this);
    object = new MockObject(this, mock);
    streams = new QMediaStreams(object);
}

void tst_QMediaStreams::cleanup()
{
}

void tst_QMediaStreams::testIsStreamInfoAvailable()
{
    QVERIFY(streams->isStreamsInformationAvailable());
}

void tst_QMediaStreams::testNewStream()
{
    QCOMPARE(streams->streams(QMediaStreamInfo::AudioStream).count(), 0);
    QCOMPARE(streams->streams(QMediaStreamInfo::VideoStream).count(), 0);

    QSignalSpy streamsChangedSignal(streams, SIGNAL(streamsChanged()));

    mock->addStream( QMediaStreamInfo::AudioStream, QMap<QString,QVariant>() );

    QCOMPARE(streams->streams(QMediaStreamInfo::AudioStream).count(), 1);
    QCOMPARE(streams->streams(QMediaStreamInfo::VideoStream).count(), 0);
    QCOMPARE(streamsChangedSignal.count(), 1);
    streamsChangedSignal.clear();

    mock->addStream( QMediaStreamInfo::VideoStream, QMap<QString,QVariant>() );
    mock->addStream( QMediaStreamInfo::VideoStream, QMap<QString,QVariant>() );

    QCOMPARE(streams->streams(QMediaStreamInfo::AudioStream).count(), 1);
    QCOMPARE(streams->streams(QMediaStreamInfo::VideoStream).count(), 2);
    QCOMPARE(streamsChangedSignal.count(), 2);
    streamsChangedSignal.clear();

    mock->clearStreams();

    QCOMPARE(streams->streams(QMediaStreamInfo::AudioStream).count(), 0);
    QCOMPARE(streams->streams(QMediaStreamInfo::VideoStream).count(), 0);
    QCOMPARE(streamsChangedSignal.count(), 1);
}

void tst_QMediaStreams::testStreamProperties()
{
    QMap<QString,QVariant> streamProperties;
    streamProperties["Name"] = QString("test stream 1");
    streamProperties["Language"] = QString("test language");
    streamProperties["Extra Property"] = QString("Extra Property Value");

    mock->addStream( QMediaStreamInfo::AudioStream, streamProperties);
    QMediaStreamInfo testStream = streams->streams(QMediaStreamInfo::AudioStream).first();
    QCOMPARE(testStream.type(), QMediaStreamInfo::AudioStream);
    QCOMPARE(testStream.streamName(), streamProperties["Name"].toString());
    QCOMPARE(testStream.streamLanguage(), streamProperties["Language"].toString());
    QCOMPARE(testStream.property("Name").toString(), testStream.streamName());
}


void tst_QMediaStreams::testDefaultLanguage()
{
    mock->clearStreams();

    mock->addStream(QMediaStreamInfo::AudioStream, "name1", "lang1");
    mock->addStream(QMediaStreamInfo::AudioStream, "name2", "lang2");
    mock->addStream(QMediaStreamInfo::AudioStream, "name3", "lang3");

    QVERIFY(streams->activeStream(QMediaStreamInfo::AudioStream).isNull());

    streams->setActiveStream(streams->streams(QMediaStreamInfo::AudioStream).first());
    QCOMPARE(streams->activeStream(QMediaStreamInfo::AudioStream).streamName(), QString("name1"));

    streams->setDefaultLanguage(QMediaStreamInfo::AudioStream, "lang2");
    QCOMPARE(streams->activeStream(QMediaStreamInfo::AudioStream).streamName(), QString("name1"));

    mock->addStream(QMediaStreamInfo::VideoStream, "name4", "lang2");
    QCOMPARE(streams->activeStream(QMediaStreamInfo::AudioStream).streamName(), QString("name2"));
}

QTEST_MAIN(tst_QMediaStreams)

#include "tst_qmediastreams.moc"
