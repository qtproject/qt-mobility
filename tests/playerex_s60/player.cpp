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

#include "player.h"

#include "playercontrols.h"
#include "videowidget.h"

#include <qmediaservice.h>
#include <qmediaplaylist.h>

#include <QtGui>
#include <QDomDocument>

S60Player::S60Player(QMainWindow *parent)
    : QMainWindow(parent)
    , videoWidget(0)
    , coverLabel(0)
    , slider(0)
    , colorDialog(0)
{
    player = new QMediaPlayer(this);
    player->setVolume(50);
    playlist = new QMediaPlaylist(this);
    playlist->setMediaObject(player);

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));

    videoWidget = new VideoWidget;
    videoWidget->setMediaObject(player);

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, player->duration() / 1000);

    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));

    QPushButton *openButton = new QPushButton(tr("Open"));
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));

    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

    PlayerControls *controls = new PlayerControls;
    controls->setState(player->state());
    controls->setMuted(controls->isMuted());

    connect(controls, SIGNAL(play()), player, SLOT(play()));
    connect(controls, SIGNAL(pause()), player, SLOT(pause()));
    connect(controls, SIGNAL(next()), playlist, SLOT(next()));
    connect(controls, SIGNAL(previous()), playlist, SLOT(previous()));
    connect(controls, SIGNAL(changeMuting(bool)), player, SLOT(setMuted(bool)));

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
            controls, SLOT(setState(QMediaPlayer::State)));
    connect(player, SIGNAL(mutedChanged(bool)), controls, SLOT(setMuted(bool)));

    QBoxLayout *displayLayout = new QHBoxLayout;
    if (videoWidget)
        displayLayout->addWidget(videoWidget, 2);
    else
        displayLayout->addWidget(coverLabel, 2);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    layout->addWidget(slider);
    layout->addLayout(controlLayout);
    
    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    createMenu();
    
    metaDataChanged();
    mediaKeysObserver = new MediaKeysObserver(this);
    connect(mediaKeysObserver, SIGNAL(mediaKeyPressed(MediaKeysObserver::MediaKeys)), this, SLOT(handleMediaKeyEvent(MediaKeysObserver::MediaKeys)));
}

S60Player::~S60Player()
{
    delete playlist;
    delete player;
}

void S60Player::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames();
    foreach (QString const &fileName, fileNames)
        playlist->addMedia(QUrl::fromLocalFile(fileName));
}

void S60Player::addMediaToPlayList(const QString &url)
{
	playlist->addMedia(QUrl(url));
	
	playlist->setCurrentIndex(playlist->mediaCount() > 0 ? playlist->mediaCount()-1 : 0);
}

void S60Player::durationChanged(qint64 duration)
{
    slider->setMaximum(duration / 1000);
}

void S60Player::positionChanged(qint64 progress)
{
    slider->setValue(progress / 1000);
}

void S60Player::metaDataChanged()
{
    //qDebug() << "update metadata" << player->metaData(QtMedia::Title).toString();
    if (player->isMetaDataAvailable()) {
        setTrackInfo(QString("(%1/%2) %3 - %4")
        		.arg(playlist->currentIndex()+1)
        		.arg(playlist->mediaCount())
                .arg(player->metaData(QtMedia::AlbumArtist).toString())
                .arg(player->metaData(QtMedia::Title).toString()));

        if (coverLabel) {
            QUrl uri = player->metaData(QtMedia::CoverArtUrlLarge).value<QUrl>();

            coverLabel->setPixmap(!uri.isEmpty()
                    ? QPixmap(uri.toString())
                    : QPixmap());
        }
    }
}

void S60Player::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void S60Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
#ifndef QT_NO_CURSOR
        setCursor(QCursor(Qt::BusyCursor));
#endif
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
#ifndef QT_NO_CURSOR
        setCursor(QCursor(Qt::BusyCursor));
#endif
        break;
    case QMediaPlayer::EndOfMedia:
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        setStatusInfo(QString());
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        setStatusInfo(player->errorString());
        break;
    }
}

void S60Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%%").arg(progress));
}

void S60Player::setTrackInfo(const QString &info)
{
    trackInfo = info;

    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);

}

void S60Player::setStatusInfo(const QString &info)
{
    statusInfo = info;

    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
}

void S60Player::showColorDialog()
{
    if (!colorDialog) {
        QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
        brightnessSlider->setRange(-100, 100);
        brightnessSlider->setValue(videoWidget->brightness());
        connect(brightnessSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setBrightness(int)));
        connect(videoWidget, SIGNAL(brightnessChanged(int)), brightnessSlider, SLOT(setValue(int)));

        QSlider *contrastSlider = new QSlider(Qt::Horizontal);
        contrastSlider->setRange(-100, 100);
        contrastSlider->setValue(videoWidget->contrast());
        connect(contrastSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setContrast(int)));
        connect(videoWidget, SIGNAL(contrastChanged(int)), contrastSlider, SLOT(setValue(int)));

        QSlider *hueSlider = new QSlider(Qt::Horizontal);
        hueSlider->setRange(-100, 100);
        hueSlider->setValue(videoWidget->hue());
        connect(hueSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setHue(int)));
        connect(videoWidget, SIGNAL(hueChanged(int)), hueSlider, SLOT(setValue(int)));

        QSlider *saturationSlider = new QSlider(Qt::Horizontal);
        saturationSlider->setRange(-100, 100);
        saturationSlider->setValue(videoWidget->saturation());
        connect(saturationSlider, SIGNAL(sliderMoved(int)), videoWidget, SLOT(setSaturation(int)));
        connect(videoWidget, SIGNAL(saturationChanged(int)), saturationSlider, SLOT(setValue(int)));

        QFormLayout *layout = new QFormLayout;
        layout->addRow(tr("Brightness"), brightnessSlider);
        layout->addRow(tr("Contrast"), contrastSlider);
        layout->addRow(tr("Hue"), hueSlider);
        layout->addRow(tr("Saturation"), saturationSlider);

        colorDialog = new QDialog(this);
        colorDialog->setWindowTitle(tr("Color Options"));
        colorDialog->setLayout(layout);
    }
    colorDialog->show();    
}

void S60Player::handleMediaKeyEvent(MediaKeysObserver::MediaKeys key)
{
    switch (key) {
        case MediaKeysObserver::EVolIncKey: 
            player->setVolume(player->volume() + 10);
            break;
        case MediaKeysObserver::EVolDecKey:
            player->setVolume(player->volume() - 10);
            break;
        default:
        break;
    }
}

void S60Player::createMenu()
{
	if (videoWidget != 0) {
		QAction* actionToggleFullscreen = menuBar()->addAction("Toggle fullscreen");
		actionToggleFullscreen->setCheckable(true);
		connect(actionToggleFullscreen, SIGNAL(triggered(bool)), videoWidget, SLOT(setFullScreen(bool)));
		
		QAction* actionColorAdjust = menuBar()->addAction("Color Adjust");
		connect(actionColorAdjust, SIGNAL(triggered()), this, SLOT(showColorDialog()));
	}
	
    QAction *youtubeAction = menuBar()->addAction("Youtube search");
    connect(youtubeAction, SIGNAL(triggered()), this, SLOT(launchYoutubeDialog()));

	rateMenu = menuBar()->addMenu(tr("Rate"));

	signalMapper = new QSignalMapper(this);
	QAction *menu_05Action = new QAction(tr("0.5x"), this);
	connect(menu_05Action, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(menu_05Action, "0.5");

    QAction *menu_10Action = new QAction(tr("1.0x"), this);
	connect(menu_10Action, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(menu_10Action, "1.0");
    
    QAction *menu_20Action = new QAction(tr("2.0x"), this);
	connect(menu_20Action, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(menu_20Action, "2.0");

    connect(signalMapper, SIGNAL(mapped(const QString &)),
            this, SLOT(updateRate(const QString &)));
    
    rateMenu->addAction(menu_05Action);
    rateMenu->addAction(menu_10Action);
    rateMenu->addAction(menu_20Action);
}

void S60Player::updateRate(const QString & rate)
{
	player->setPlaybackRate(rate.toFloat());
}

void S60Player::launchYoutubeDialog()
{
    YoutubeDialog youtube(this);
    youtube.exec();    
}

YoutubeDialog::YoutubeDialog(S60Player *player)
    : QDialog(0), m_player(player) 
{
    searchLine= new QLineEdit(this);
    QPushButton *searchButton = new QPushButton("Search");
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(searchLine);
    topLayout->addWidget(searchButton);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(search()));
    
	videoList = new QListWidget(this);
    QPushButton *playButton = new QPushButton("Add");
    connect(playButton, SIGNAL(clicked()), this, SLOT(add()));
    QPushButton *closeButton = new QPushButton("Close");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    
    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(playButton);
    bottomLayout->addWidget(closeButton);
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(videoList);
    mainLayout->addLayout(bottomLayout);
    
    setLayout(mainLayout);
    
    connect(&http, SIGNAL(requestFinished(int, bool)),
        this, SLOT(httpRequestFinished(int, bool)));
    connect(&http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
        this, SLOT(readResponseHeader(const QHttpResponseHeader&)));
    
    showFullScreen(); 
}

void YoutubeDialog::readResponseHeader(const QHttpResponseHeader& responseHeader)
{
    switch (responseHeader.statusCode())
    {
        case 200:   // Ok
        case 301:   // Moved Permanently
        case 302:   // Found
        case 303:   // See Other
        case 307:   // Temporary Redirect
        {
            // these are not error conditions
            break;
        }
        default:
        {
            QMessageBox::information(this, tr("Youtube"),
                tr("Download failed: %1.").arg(responseHeader.reasonPhrase()));
            httpRequestAborted = true;
            http.abort();
        }
    }
}

void YoutubeDialog::add()
{
	qDebug() << videoList->currentRow();
	qDebug() << videoNameList.at(videoList->currentRow());
	qDebug() << videoNameList;
	m_player->addMediaToPlayList(videoNameList.at(videoList->currentRow()));
}

void YoutubeDialog::search()
{
	QString searchText = searchLine->text().replace(' ', '+');
	
	QString urlstring = QString("http://gdata.youtube.com/feeds/api/videos?" \
	     "q=%1&max-results=25&v=2&format=6").arg(searchText);
	httpRequestAborted = false;
	QUrl url(urlstring);
	http.setHost(url.host(), QHttp::ConnectionModeHttp, url.port() == -1 ? 0 : url.port());   
	videoList->clear();
	videoNameList.clear();
	httpGetId = http.get(urlstring);
}

void YoutubeDialog::httpRequestFinished(int requestId, bool error)
{
    if (requestId != httpGetId || httpRequestAborted)
    {
        return;
    }
    
    if (error)
    {
        QMessageBox::information(this, tr("Youtube"),
            tr("Download failed: %1.")
            .arg(http.errorString()));
    }
    else
    {
		QString data = http.readAll();
        QTextStream in(&data);
        QStringList lines;
        
        QTemporaryFile file;
        if (!file.open()) {	
			QMessageBox::information(this, tr("Youtube"),
					tr("Error"));
			return;
        }
        
        QTextStream out(&file);
        
        while (!in.atEnd())
		{
			QString oneLine = in.readLine();
			out << oneLine;
			lines.append(oneLine);
		}
        file.close();
        
        if (!file.open()) {	
			QMessageBox::information(this, tr("Youtube"),
					tr("Error"));
			return;
        }
    	
		QDomDocument domDocument;
		QString errorMessage;
		if (!domDocument.setContent(&file, true, &errorMessage)) {
			return;
		}

		QDomElement root = domDocument.documentElement();
		if (root.tagName() != "feed")
			return;
		
		QDomElement entryElement = root.firstChildElement("entry");
		
		while(!entryElement.isNull()) 
		{
			QString title = entryElement.firstChildElement("title").text();
			QDomElement groupElement = entryElement.firstChildElement("group");
			QDomElement incidentElement2 = groupElement.firstChildElement("content");
			while(!incidentElement2.isNull()) 
			{
                // "6" = MPEG-4 SP video (up to 176x144) and AAC audio.
				if (incidentElement2.attribute("format") == "6") { 
					videoList->addItem(title);
					videoNameList.append(incidentElement2.attribute("url"));
					break;
				} 
				incidentElement2 = incidentElement2.nextSiblingElement("content");
			}
			
			entryElement = entryElement.nextSiblingElement("entry");
		}
    }
}

