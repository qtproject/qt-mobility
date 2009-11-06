/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QDebug>
#include "qmediaservice.h"
#include "qmediaplaylist.h"
#include "qmediaplaylistcontrol.h"
#include "qmediaplaylistnavigator.h"

class MockReadOnlyPlaylistProvider : public QMediaPlaylistProvider
{
    Q_OBJECT
public:
    MockReadOnlyPlaylistProvider(QObject *parent)
        :QMediaPlaylistProvider(parent)
    {
        m_items.append(QMediaContent(QUrl(QLatin1String("file:///1"))));
        m_items.append(QMediaContent(QUrl(QLatin1String("file:///2"))));
        m_items.append(QMediaContent(QUrl(QLatin1String("file:///3"))));
    }

    int size() const { return m_items.size(); }
    QMediaContent media(int index) const
    {
        return index >=0 && index < size() ? m_items.at(index) : QMediaContent();
    }

private:
    QList<QMediaContent> m_items;
};

class MockPlaylistControl : public QMediaPlaylistControl
{
    Q_OBJECT
public:
    MockPlaylistControl(QObject *parent) : QMediaPlaylistControl(parent)
    {        
        m_navigator = new QMediaPlaylistNavigator(new MockReadOnlyPlaylistProvider(this), this);
    }

    ~MockPlaylistControl()
    {
    }

    QMediaPlaylistProvider* playlistProvider() const { return m_navigator->playlist(); }
    bool setPlaylistProvider(QMediaPlaylistProvider *playlist) { m_navigator->setPlaylist(playlist); return true; }

    int currentPosition() const { return m_navigator->currentPosition(); }
    void setCurrentPosition(int position) { m_navigator->jump(position); }
    int nextPosition(int steps) const { return m_navigator->nextPosition(steps); }
    int previousPosition(int steps) const { return m_navigator->previousPosition(steps); }

    void next() { m_navigator->next(); }
    void previous() { m_navigator->previous(); }

    QMediaPlaylist::PlaybackMode playbackMode() const { return m_navigator->playbackMode(); }
    void setPlaybackMode(QMediaPlaylist::PlaybackMode mode) { m_navigator->setPlaybackMode(mode); }

private:    
    QMediaPlaylistNavigator *m_navigator;
};

class MockPlaylistService : public QMediaService
{
    Q_OBJECT

public:
    MockPlaylistService():QMediaService(0)
    {
        mockControl = new MockPlaylistControl(this);
    }

    ~MockPlaylistService()
    {        
    }

    QMediaControl* control(const char *iid) const
    {
        if (qstrcmp(iid, QMediaPlaylistControl_iid) == 0)
            return mockControl;
        return 0;
    }

    MockPlaylistControl *mockControl;
};

class MockReadOnlyPlaylistObject : public QMediaObject
{
    Q_OBJECT
public:
    MockReadOnlyPlaylistObject(QObject *parent = 0)
        :QMediaObject(parent, new MockPlaylistService)
    {
    }
};


class tst_QMediaPlaylist : public QObject
{
    Q_OBJECT
public slots:
    void init();
    void cleanup();
    void initTestCase();

private slots:
    void construction();
    void append();
    void insert();
    void clear();
    void removeItems();
    void currentItem();
    void saveAndLoad();
    void playbackMode();
    void playbackMode_data();
    void shuffle();
    void readOnlyPlaylist();

private:
    QMediaContent content1;
    QMediaContent content2;
    QMediaContent content3;
};

void tst_QMediaPlaylist::init()
{
}

void tst_QMediaPlaylist::initTestCase()
{
    content1 = QMediaContent(QUrl(QLatin1String("file:///1")));
    content2 = QMediaContent(QUrl(QLatin1String("file:///2")));
    content3 = QMediaContent(QUrl(QLatin1String("file:///3")));
}

void tst_QMediaPlaylist::cleanup()
{
}

void tst_QMediaPlaylist::construction()
{
    QMediaPlaylist playlist;
    QCOMPARE(playlist.size(), 0);
    QVERIFY(playlist.isEmpty());
}

void tst_QMediaPlaylist::append()
{
    QMediaPlaylist playlist;
    QVERIFY(!playlist.isReadOnly());

    playlist.appendItem(content1);
    QCOMPARE(playlist.size(), 1);
    QCOMPARE(playlist.media(0), content1);

    playlist.appendItem(content2);
    QCOMPARE(playlist.size(), 2);
    QCOMPARE(playlist.media(1), content2);
}

void tst_QMediaPlaylist::insert()
{
    QMediaPlaylist playlist;
    QVERIFY(!playlist.isReadOnly());

    playlist.appendItem(content1);
    QCOMPARE(playlist.size(), 1);
    QCOMPARE(playlist.media(0), content1);

    playlist.appendItem(content2);
    QCOMPARE(playlist.size(), 2);
    QCOMPARE(playlist.media(1), content2);

    QSignalSpy aboutToBeInsertedSignalSpy(&playlist, SIGNAL(itemsAboutToBeInserted(int,int)));
    QSignalSpy insertedSignalSpy(&playlist, SIGNAL(itemsInserted(int,int)));

    playlist.insertItem(1, content3);
    QCOMPARE(playlist.size(), 3);
    QCOMPARE(playlist.media(0), content1);
    QCOMPARE(playlist.media(1), content3);
    QCOMPARE(playlist.media(2), content2);

    QCOMPARE(aboutToBeInsertedSignalSpy.count(), 1);
    QCOMPARE(aboutToBeInsertedSignalSpy.first()[0].toInt(), 1);
    QCOMPARE(aboutToBeInsertedSignalSpy.first()[1].toInt(), 1);

    QCOMPARE(insertedSignalSpy.count(), 1);
    QCOMPARE(insertedSignalSpy.first()[0].toInt(), 1);
    QCOMPARE(insertedSignalSpy.first()[1].toInt(), 1);
}


void tst_QMediaPlaylist::currentItem()
{
    QMediaPlaylist playlist;
    playlist.appendItem(content1);
    playlist.appendItem(content2);

    QCOMPARE(playlist.currentPosition(), -1);
    QCOMPARE(playlist.currentMedia(), QMediaContent());

    QCOMPARE(playlist.nextPosition(), 0);
    QCOMPARE(playlist.nextPosition(2), 1);
    QCOMPARE(playlist.previousPosition(), 1);
    QCOMPARE(playlist.previousPosition(2), 0);

    playlist.setCurrentPosition(0);
    QCOMPARE(playlist.currentPosition(), 0);
    QCOMPARE(playlist.currentMedia(), content1);

    QCOMPARE(playlist.nextPosition(), 1);
    QCOMPARE(playlist.nextPosition(2), -1);
    QCOMPARE(playlist.previousPosition(), -1);
    QCOMPARE(playlist.previousPosition(2), -1);

    playlist.setCurrentPosition(1);
    QCOMPARE(playlist.currentPosition(), 1);
    QCOMPARE(playlist.currentMedia(), content2);

    QCOMPARE(playlist.nextPosition(), -1);
    QCOMPARE(playlist.nextPosition(2), -1);
    QCOMPARE(playlist.previousPosition(), 0);
    QCOMPARE(playlist.previousPosition(2), -1);

    QTest::ignoreMessage(QtWarningMsg, "QMediaPlaylistNavigator: Jump outside playlist range ");
    playlist.setCurrentPosition(2);

    QCOMPARE(playlist.currentPosition(), -1);
    QCOMPARE(playlist.currentMedia(), QMediaContent());
}

void tst_QMediaPlaylist::clear()
{
    QMediaPlaylist playlist;
    playlist.appendItem(content1);
    playlist.appendItem(content2);

    playlist.clear();
    QVERIFY(playlist.isEmpty());
    QCOMPARE(playlist.size(), 0);
}

void tst_QMediaPlaylist::removeItems()
{
    QMediaPlaylist playlist;
    playlist.appendItem(content1);
    playlist.appendItem(content2);
    playlist.appendItem(content3);

    QSignalSpy aboutToBeRemovedSignalSpy(&playlist, SIGNAL(itemsAboutToBeRemoved(int,int)));
    QSignalSpy removedSignalSpy(&playlist, SIGNAL(itemsRemoved(int,int)));
    playlist.removeItem(1);
    QCOMPARE(playlist.size(), 2);
    QCOMPARE(playlist.media(1), content3);

    QCOMPARE(aboutToBeRemovedSignalSpy.count(), 1);
    QCOMPARE(aboutToBeRemovedSignalSpy.first()[0].toInt(), 1);
    QCOMPARE(aboutToBeRemovedSignalSpy.first()[1].toInt(), 1);

    QCOMPARE(removedSignalSpy.count(), 1);
    QCOMPARE(removedSignalSpy.first()[0].toInt(), 1);
    QCOMPARE(removedSignalSpy.first()[1].toInt(), 1);

    aboutToBeRemovedSignalSpy.clear();
    removedSignalSpy.clear();

    playlist.removeItems(0,1);
    QVERIFY(playlist.isEmpty());

    QCOMPARE(aboutToBeRemovedSignalSpy.count(), 1);
    QCOMPARE(aboutToBeRemovedSignalSpy.first()[0].toInt(), 0);
    QCOMPARE(aboutToBeRemovedSignalSpy.first()[1].toInt(), 1);

    QCOMPARE(removedSignalSpy.count(), 1);
    QCOMPARE(removedSignalSpy.first()[0].toInt(), 0);
    QCOMPARE(removedSignalSpy.first()[1].toInt(), 1);


    playlist.appendItem(content1);
    playlist.appendItem(content2);
    playlist.appendItem(content3);

    playlist.removeItems(0,1);
    QCOMPARE(playlist.size(), 1);
    QCOMPARE(playlist.media(0), content3);
}

void tst_QMediaPlaylist::saveAndLoad()
{
    QMediaPlaylist playlist;
    playlist.appendItem(content1);
    playlist.appendItem(content2);
    playlist.appendItem(content3);

    QCOMPARE(playlist.error(), QMediaPlaylist::NoError);
    QVERIFY(playlist.errorString().isEmpty());

    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    bool res = playlist.save(&buffer, "unsupported_format");
    QVERIFY(!res);
    QVERIFY(playlist.error() != QMediaPlaylist::NoError);
    QVERIFY(!playlist.errorString().isEmpty());

    QSignalSpy errorSignal(&playlist, SIGNAL(loadFailed()));
    playlist.load(&buffer, "unsupported_format");
    QCOMPARE(errorSignal.size(), 1);
    QVERIFY(playlist.error() != QMediaPlaylist::NoError);
    QVERIFY(!playlist.errorString().isEmpty());

    res = playlist.save(QUrl(QLatin1String("tmp.unsupported_format")), "unsupported_format");
    QVERIFY(!res);
    QVERIFY(playlist.error() != QMediaPlaylist::NoError);
    QVERIFY(!playlist.errorString().isEmpty());

    errorSignal.clear();
    playlist.load(QUrl(QLatin1String("tmp.unsupported_format")), "unsupported_format");
    QCOMPARE(errorSignal.size(), 1);
    QVERIFY(playlist.error() != QMediaPlaylist::NoError);
    QVERIFY(!playlist.errorString().isEmpty());

    //it's necessary to ensure the m3u plugin is loaded for this test
    /*
    res = playlist.save(&buffer, "m3u");

    QVERIFY(res);
    QVERIFY(buffer.pos() > 0);
    buffer.seek(0);

    QMediaPlaylist playlist2;
    playlist2.load(&buffer, "m3u");
    QCOMPARE(playlist.error(), QMediaPlaylist::NoError);

    QCOMPARE(playlist.size(), playlist2.size());
    QCOMPARE(playlist.media(0), playlist2.media(0));
    QCOMPARE(playlist.media(1), playlist2.media(1));
    QCOMPARE(playlist.media(3), playlist2.media(3));

    res = playlist.save(QUrl(QLatin1String("tmp.m3u")), "m3u");
    QVERIFY(res);

    playlist2.clear();
    QVERIFY(playlist2.isEmpty());
    playlist2.load(QUrl(QLatin1String("tmp.m3u")), "m3u");
    QCOMPARE(playlist.error(), QMediaPlaylist::NoError);

    QCOMPARE(playlist.size(), playlist2.size());
    QCOMPARE(playlist.media(0), playlist2.media(0));
    QCOMPARE(playlist.media(1), playlist2.media(1));
    QCOMPARE(playlist.media(3), playlist2.media(3));
    */
}

void tst_QMediaPlaylist::playbackMode_data()
{
    QTest::addColumn<QMediaPlaylist::PlaybackMode>("playbackMode");
    QTest::addColumn<int>("expectedPrevious");
    QTest::addColumn<int>("pos");
    QTest::addColumn<int>("expectedNext");

    QTest::newRow("Linear, 0") << QMediaPlaylist::Linear << -1 << 0 << 1;
    QTest::newRow("Linear, 1") << QMediaPlaylist::Linear << 0 << 1 << 2;
    QTest::newRow("Linear, 2") << QMediaPlaylist::Linear << 1 << 2 << -1;

    QTest::newRow("Loop, 0") << QMediaPlaylist::Loop << 2 << 0 << 1;
    QTest::newRow("Loop, 1") << QMediaPlaylist::Loop << 0 << 1 << 2;
    QTest::newRow("Lopp, 2") << QMediaPlaylist::Loop << 1 << 2 << 0;

    QTest::newRow("ItemOnce, 1") << QMediaPlaylist::CurrentItemOnce << -1 << 1 << -1;
    QTest::newRow("ItemInLoop, 1") << QMediaPlaylist::CurrentItemInLoop << 1 << 1 << 1;

}

void tst_QMediaPlaylist::playbackMode()
{
    QFETCH(QMediaPlaylist::PlaybackMode, playbackMode);
    QFETCH(int, expectedPrevious);
    QFETCH(int, pos);
    QFETCH(int, expectedNext);

    QMediaPlaylist playlist;
    playlist.appendItem(content1);
    playlist.appendItem(content2);
    playlist.appendItem(content3);

    QCOMPARE(playlist.playbackMode(), QMediaPlaylist::Linear);
    QCOMPARE(playlist.currentPosition(), -1);

    playlist.setPlaybackMode(playbackMode);
    QCOMPARE(playlist.playbackMode(), playbackMode);

    playlist.setCurrentPosition(pos);
    QCOMPARE(playlist.currentPosition(), pos);
    QCOMPARE(playlist.nextPosition(), expectedNext);
    QCOMPARE(playlist.previousPosition(), expectedPrevious);

    playlist.next();
    QCOMPARE(playlist.currentPosition(), expectedNext);

    playlist.setCurrentPosition(pos);
    playlist.previous();
    QCOMPARE(playlist.currentPosition(), expectedPrevious);
}

void tst_QMediaPlaylist::shuffle()
{
    QMediaPlaylist playlist;
    QList<QMediaContent> contentList;

    for (int i=0; i<100; i++) {
        QMediaContent content(QUrl(QString::number(i)));
        contentList.append(content);
        playlist.appendItem(content);
    }

    playlist.shuffle();

    QList<QMediaContent> shuffledContentList;
    for (int i=0; i<playlist.size(); i++)
        shuffledContentList.append(playlist.media(i));

    QVERIFY(contentList != shuffledContentList);

}

void tst_QMediaPlaylist::readOnlyPlaylist()
{
    MockReadOnlyPlaylistObject mediaObject;
    QMediaPlaylist playlist(&mediaObject);

    QVERIFY(playlist.isReadOnly());
    QVERIFY(!playlist.isEmpty());
    QCOMPARE(playlist.size(), 3);

    QCOMPARE(playlist.media(0), content1);
    QCOMPARE(playlist.media(1), content2);
    QCOMPARE(playlist.media(2), content3);
    QCOMPARE(playlist.media(3), QMediaContent());

    //it's a read only playlist, so all the modification should fail
    QVERIFY(!playlist.appendItem(content1));
    QCOMPARE(playlist.size(), 3);
    QVERIFY(!playlist.insertItem(1, content1));
    QCOMPARE(playlist.size(), 3);
    QVERIFY(!playlist.removeItem(1));
    QCOMPARE(playlist.size(), 3);
    QVERIFY(!playlist.removeItems(0,2));
    QCOMPARE(playlist.size(), 3);
    QVERIFY(!playlist.clear());
    QCOMPARE(playlist.size(), 3);

    playlist.shuffle();
    //it's still the same
    QCOMPARE(playlist.media(0), content1);
    QCOMPARE(playlist.media(1), content2);
    QCOMPARE(playlist.media(2), content3);
    QCOMPARE(playlist.media(3), QMediaContent());


    //load to read only playlist should fail,
    //unless underlaying provider supports it
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    buffer.write(QByteArray("file:///1\nfile:///2"));
    buffer.seek(0);

    QSignalSpy errorSignal(&playlist, SIGNAL(loadFailed()));
    playlist.load(&buffer, "m3u");    
    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(playlist.error(), QMediaPlaylist::AccessDeniedError);
    QVERIFY(!playlist.errorString().isEmpty());
    QCOMPARE(playlist.size(), 3);

    errorSignal.clear();
    playlist.load(QUrl(QLatin1String("tmp.m3u")), "m3u");

    QCOMPARE(errorSignal.size(), 1);
    QCOMPARE(playlist.error(), QMediaPlaylist::AccessDeniedError);
    QVERIFY(!playlist.errorString().isEmpty());
    QCOMPARE(playlist.size(), 3);
}

QTEST_MAIN(tst_QMediaPlaylist)
#include "tst_qmediaplaylist.moc"

