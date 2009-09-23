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
#include "multimedia/qlocalmediaplaylistprovider.h"
#include "multimedia/qmediaplaylistnavigator.h"

class tst_QMediaPlaylistNavigator : public QObject
{
    Q_OBJECT
public slots:
    void init();
    void cleanup();

private slots:
    void construction();
    void setPlaylist();
    void linearPlayback();
    void loopPlayback();
};

void tst_QMediaPlaylistNavigator::init()
{
}

void tst_QMediaPlaylistNavigator::cleanup()
{
}

void tst_QMediaPlaylistNavigator::construction()
{
    QLocalMediaPlaylistProvider playlist;
    QCOMPARE(playlist.size(), 0);

    QMediaPlaylistNavigator navigator(&playlist);
    QVERIFY(navigator.currentItem().isNull());
    QCOMPARE(navigator.currentPosition(), -1);
}

void tst_QMediaPlaylistNavigator::setPlaylist()
{
    QMediaPlaylistNavigator navigator(0);
    QVERIFY(navigator.playlist() != 0);
    QCOMPARE(navigator.playlist()->size(), 0);
    QVERIFY(navigator.playlist()->isReadOnly() );

    QLocalMediaPlaylistProvider playlist;
    QCOMPARE(playlist.size(), 0);

    navigator.setPlaylist(&playlist);
    QCOMPARE(navigator.playlist(), &playlist);
    QCOMPARE(navigator.playlist()->size(), 0);
    QVERIFY(!navigator.playlist()->isReadOnly() );
}

void tst_QMediaPlaylistNavigator::linearPlayback()
{
    QLocalMediaPlaylistProvider playlist;
    QMediaPlaylistNavigator navigator(&playlist);

    navigator.setPlaybackMode(QMediaPlaylist::Linear);
    navigator.jump(0);//it's ok to have warning here
    QVERIFY(navigator.currentItem().isNull());
    QCOMPARE(navigator.currentPosition(), -1);

    QMediaSource source1(QUrl(QLatin1String("file:///1")));
    playlist.appendItem(source1);
    navigator.jump(0);
    QVERIFY(!navigator.currentItem().isNull());

    QCOMPARE(navigator.currentPosition(), 0);
    QCOMPARE(navigator.currentItem(), source1);
    QCOMPARE(navigator.nextItem(), QMediaSource());
    QCOMPARE(navigator.nextItem(2), QMediaSource());
    QCOMPARE(navigator.previousItem(), QMediaSource());
    QCOMPARE(navigator.previousItem(2), QMediaSource());

    QMediaSource source2(QUrl(QLatin1String("file:///2")));
    playlist.appendItem(source2);
    QCOMPARE(navigator.currentPosition(), 0);
    QCOMPARE(navigator.currentItem(), source1);
    QCOMPARE(navigator.nextItem(), source2);
    QCOMPARE(navigator.nextItem(2), QMediaSource());
    QCOMPARE(navigator.previousItem(), QMediaSource());
    QCOMPARE(navigator.previousItem(2), QMediaSource());

    navigator.jump(1);
    QCOMPARE(navigator.currentPosition(), 1);
    QCOMPARE(navigator.currentItem(), source2);
    QCOMPARE(navigator.nextItem(), QMediaSource());
    QCOMPARE(navigator.nextItem(2), QMediaSource());
    QCOMPARE(navigator.previousItem(), source1);
    QCOMPARE(navigator.previousItem(2), QMediaSource());

    navigator.jump(0);
    navigator.advance();
    QCOMPARE(navigator.currentPosition(), 1);
    navigator.advance();
    QCOMPARE(navigator.currentPosition(), 1);
    navigator.back();
    QCOMPARE(navigator.currentPosition(), 0);
    navigator.back();
    QCOMPARE(navigator.currentPosition(), 0);
}

void tst_QMediaPlaylistNavigator::loopPlayback()
{
    QLocalMediaPlaylistProvider playlist;
    QMediaPlaylistNavigator navigator(&playlist);

    navigator.setPlaybackMode(QMediaPlaylist::Loop);
    navigator.jump(0); //it's ok to have warning here
    QVERIFY(navigator.currentItem().isNull());
    QCOMPARE(navigator.currentPosition(), -1);

    QMediaSource source1(QUrl(QLatin1String("file:///1")));
    playlist.appendItem(source1);
    navigator.jump(0);
    QVERIFY(!navigator.currentItem().isNull());

    QCOMPARE(navigator.currentPosition(), 0);
    QCOMPARE(navigator.currentItem(), source1);
    QCOMPARE(navigator.nextItem(), source1);
    QCOMPARE(navigator.nextItem(2), source1);
    QCOMPARE(navigator.previousItem(), source1);
    QCOMPARE(navigator.previousItem(2), source1);

    QMediaSource source2(QUrl(QLatin1String("file:///2")));
    playlist.appendItem(source2);
    QCOMPARE(navigator.currentPosition(), 0);
    QCOMPARE(navigator.currentItem(), source1);
    QCOMPARE(navigator.nextItem(), source2);
    QCOMPARE(navigator.nextItem(2), source1); //loop over end of the list
    QCOMPARE(navigator.previousItem(), source2);
    QCOMPARE(navigator.previousItem(2), source1);

    navigator.jump(1);
    QCOMPARE(navigator.currentPosition(), 1);
    QCOMPARE(navigator.currentItem(), source2);
    QCOMPARE(navigator.nextItem(), source1);
    QCOMPARE(navigator.nextItem(2), source2);
    QCOMPARE(navigator.previousItem(), source1);
    QCOMPARE(navigator.previousItem(2), source2);

    navigator.jump(0);
    navigator.advance();
    QCOMPARE(navigator.currentPosition(), 1);
    navigator.advance();
    QCOMPARE(navigator.currentPosition(), 0);
    navigator.back();
    QCOMPARE(navigator.currentPosition(), 1);
    navigator.back();
    QCOMPARE(navigator.currentPosition(), 0);
}

QTEST_MAIN(tst_QMediaPlaylistNavigator)
#include "tst_qmediaplaylistnavigator.moc"
