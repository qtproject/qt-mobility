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

#ifndef PLAYER_H
#define PLAYER_H

#include <QtGui/QMainWindow>
#include <QtGui/QDialog>
#include <qtGui/QLineEdit>
#include <qtGui/QListWidget>

#include <qmediaplayer.h>
#include <qmediaplaylist.h>
#include <qvideowidget.h>
#include "mediakeysobserver.h"

class QAbstractItemView;
class QLabel;
class QModelIndex;
class QSlider;
class QSignalMapper;

QTM_BEGIN_NAMESPACE
class QMediaPlayer;
class QVideoWidget;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class PlaylistModel;

class Player : public QMainWindow
{
    Q_OBJECT
public:
    Player(QMainWindow *parent = 0);
    ~Player();
    
    void createMenu();

Q_SIGNALS:
    void fullScreenChanged(bool fullScreen);

public slots:
	void addMediaToPlayList(const QString &url);
	
private slots:
    void open();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void metaDataChanged();

    void seek(int seconds);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void bufferingProgress(int progress);

    void showColorDialog();
    void handleMediaKeyEvent(MediaKeysObserver::MediaKeys key);
    
    void updateRate(const QString & rate);

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;
    QLabel *coverLabel;
    QSlider *slider;
    QDialog *colorDialog;
    QString trackInfo;
    QString statusInfo;
    QMenu *rateMenu;
    QSignalMapper *signalMapper;	
    MediaKeysObserver *mediaKeysObserver;
};

#include <QHttp>

class YoutubeDialog : public QDialog
{
    Q_OBJECT

public:
	YoutubeDialog(Player *player, QWidget* parent = 0);
	
private Q_SLOTS:
	void updateDataReadProgress(int bytesRead, int totalBytes);
	void httpRequestFinished(int requestId, bool error);
	void readResponseHeader(const QHttpResponseHeader& responseHeader);
	void search();
	void add();

private:
	QLineEdit *searchLine;
	QListWidget *videoList;
	QStringList videoNameList;
    QHttp http;
    bool httpRequestAborted;
    int httpGetId;
    
    Player *m_player;
 };

#endif
