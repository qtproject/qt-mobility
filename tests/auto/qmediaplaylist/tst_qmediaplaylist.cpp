/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
#include "qmediaplaylist.h"

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

private:
    QMediaSource source1;
    QMediaSource source2;
    QMediaSource source3;
};

void tst_QMediaPlaylist::init()
{
}

void tst_QMediaPlaylist::initTestCase()
{
    source1 = QMediaSource(QUrl(QLatin1String("file:///1")));
    source2 = QMediaSource(QUrl(QLatin1String("file:///2")));
    source3 = QMediaSource(QUrl(QLatin1String("file:///3")));
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

    playlist.appendItem(source1);
    QCOMPARE(playlist.size(), 1);
    QCOMPARE(playlist.media(0), source1);

    playlist.appendItem(source2);
    QCOMPARE(playlist.size(), 2);
    QCOMPARE(playlist.media(1), source2);
}

void tst_QMediaPlaylist::insert()
{
    QMediaPlaylist playlist;
    QVERIFY(!playlist.isReadOnly());

    playlist.appendItem(source1);
    QCOMPARE(playlist.size(), 1);
    QCOMPARE(playlist.media(0), source1);

    playlist.appendItem(source2);
    QCOMPARE(playlist.size(), 2);
    QCOMPARE(playlist.media(1), source2);

    QSignalSpy aboutToBeInsertedSignalSpy(&playlist, SIGNAL(itemsAboutToBeInserted(int,int)));
    QSignalSpy insertedSignalSpy(&playlist, SIGNAL(itemsInserted(int,int)));

    playlist.insertItem(1, source3);
    QCOMPARE(playlist.size(), 3);
    QCOMPARE(playlist.media(0), source1);
    QCOMPARE(playlist.media(1), source3);
    QCOMPARE(playlist.media(2), source2);

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
    playlist.appendItem(source1);
    playlist.appendItem(source2);

    QCOMPARE(playlist.currentPosition(), -1);
    QCOMPARE(playlist.currentMedia(), QMediaSource());

    QCOMPARE(playlist.nextPosition(), 0);
    QCOMPARE(playlist.nextPosition(2), 1);
    QCOMPARE(playlist.previousPosition(), -1);
    QCOMPARE(playlist.previousPosition(2), -1);

    playlist.setCurrentPosition(0);
    QCOMPARE(playlist.currentPosition(), 0);
    QCOMPARE(playlist.currentMedia(), source1);

    QCOMPARE(playlist.nextPosition(), 1);
    QCOMPARE(playlist.nextPosition(2), -1);
    QCOMPARE(playlist.previousPosition(), -1);
    QCOMPARE(playlist.previousPosition(2), -1);

    playlist.setCurrentPosition(1);
    QCOMPARE(playlist.currentPosition(), 1);
    QCOMPARE(playlist.currentMedia(), source2);

    playlist.setCurrentPosition(2); //warning is expected
    QCOMPARE(playlist.currentPosition(), 1);
    QCOMPARE(playlist.currentMedia(), source2);

    QCOMPARE(playlist.nextPosition(), -1);
    QCOMPARE(playlist.nextPosition(2), -1);
    QCOMPARE(playlist.previousPosition(), 0);
    QCOMPARE(playlist.previousPosition(2), -1);
}

void tst_QMediaPlaylist::clear()
{
    QMediaPlaylist playlist;
    playlist.appendItem(source1);
    playlist.appendItem(source2);

    playlist.clear();
    QVERIFY(playlist.isEmpty());
    QCOMPARE(playlist.size(), 0);
}

void tst_QMediaPlaylist::removeItems()
{
    QMediaPlaylist playlist;
    playlist.appendItem(source1);
    playlist.appendItem(source2);
    playlist.appendItem(source3);

    QSignalSpy aboutToBeRemovedSignalSpy(&playlist, SIGNAL(itemsAboutToBeRemoved(int,int)));
    QSignalSpy removedSignalSpy(&playlist, SIGNAL(itemsRemoved(int,int)));
    playlist.removeItem(1);
    QCOMPARE(playlist.size(), 2);
    QCOMPARE(playlist.media(1), source3);

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


    playlist.appendItem(source1);
    playlist.appendItem(source2);
    playlist.appendItem(source3);

    playlist.removeItems(0,1);
    QCOMPARE(playlist.size(), 1);
    QCOMPARE(playlist.media(0), source3);
}

void tst_QMediaPlaylist::saveAndLoad()
{
    /*
    QMediaPlaylist playlist;
    playlist.appendItem(source1);
    playlist.appendItem(source2);
    playlist.appendItem(source3);

    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);

    bool res = playlist.save(&buffer, "m3u");

    QVERIFY(res);
    QVERIFY(buffer.pos() > 0);
    buffer.seek(0);

    QMediaPlaylist playlist2;
    res = playlist2.load(&buffer, "m3u");
    QVERIFY(res);
    QCOMPARE(playlist.size(), playlist2.size());
    QCOMPARE(playlist.media(0), playlist2.media(0));
    */
}

QTEST_MAIN(tst_QMediaPlaylist)
#include "tst_qmediaplaylist.moc"

