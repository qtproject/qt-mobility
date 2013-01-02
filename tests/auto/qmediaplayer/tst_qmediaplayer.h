/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef TST_QMEDIAPLAYER_H
#define TST_QMEDIAPLAYER_H

#include <QtTest/QtTest>
#include <QtCore/qdebug.h>
#include <QtCore/qbuffer.h>
#include <QtNetwork/qnetworkconfiguration.h>

#include <qabstractvideosurface.h>
#include <qmediaplayer.h>
#include <qmediaplayercontrol.h>
#include <qmediaplaylist.h>
#include <qmediaservice.h>
#include <qmediastreamscontrol.h>
#include <qmedianetworkaccesscontrol.h>
#include <qvideorenderercontrol.h>
#include <qvideowindowcontrol.h>

#include "mockmediaserviceprovider.h"
#include "mockmediaplayerservice.h"

QT_USE_NAMESPACE

class AutoConnection
{
public:
    AutoConnection(QObject *sender, const char *signal, QObject *receiver, const char *method)
            : sender(sender), signal(signal), receiver(receiver), method(method)
    {
        QObject::connect(sender, signal, receiver, method);
    }

    ~AutoConnection()
    {
        QObject::disconnect(sender, signal, receiver, method);
    }

private:
    QObject *sender;
    const char *signal;
    QObject *receiver;
    const char *method;
};

class tst_QMediaPlayer: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase_data();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testNullService();
    void testValid();
    void testMedia();
    void testDuration();
    void testPosition();
    void testVolume();
    void testMuted();
    void testIsAvailable();
    void testVideoAvailable();
    void testBufferStatus();
    void testSeekable();
    void testPlaybackRate();
    void testError();
    void testErrorString();
    void testService();
    void testPlay();
    void testPause();
    void testStop();
    void testMediaStatus();
    void testPlaylist();
    void testNetworkAccess();
    void testSetVideoOutput();
    void testSetVideoOutputNoService();
    void testSetVideoOutputNoControl();
    void testSetVideoOutputDestruction();
    void testPositionPropertyWatch();
    void debugEnums();
    void testPlayerFlags();
    void testDestructor();
    void testSupportedMimeTypes();

private:
    MockMediaServiceProvider *mockProvider;
    MockMediaPlayerService  *mockService;
    QMediaPlayer *player;
};

#endif //TST_QMEDIAPLAYER_H
