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
#include "qlocalmediaplaylistprovider.h"

class tst_QMediaPlaylist : public QObject
{
    Q_OBJECT
public slots:
    void init();
    void cleanup();

private slots:
    void construction();
    void append();
};

void tst_QMediaPlaylist::init()
{
}

void tst_QMediaPlaylist::cleanup()
{
}

void tst_QMediaPlaylist::construction()
{
    QLocalMediaPlaylistProvider playlist;
    QCOMPARE(playlist.size(), 0);
}

void tst_QMediaPlaylist::append()
{
    QLocalMediaPlaylistProvider playlist;
    QMediaResourceList resource1;
    resource1 << QUrl(QLatin1String("file:///1"));
    playlist.appendItem(resource1);
    QCOMPARE(playlist.size(), 1);
    QCOMPARE(playlist.resources(0), resource1);

    QMediaResourceList resource2;
    resource2 << QUrl(QLatin1String("file:///2"));
    playlist.appendItem(resource2);
    QCOMPARE(playlist.size(), 2);
    QCOMPARE(playlist.resources(1), resource2);
}

QTEST_MAIN(tst_QMediaPlaylist)
#include "tst_qmediaplaylist.moc"
