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
#include "playlistmodel.h"
#include "videowidget.h"

#include <qmediaservice.h>
#include <qmediaplaylist.h>
#include <qaudioendpointselector.h>

#include <QtGui>

#ifdef Q_OS_SYMBIAN
#include <QtGui/QDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtNetwork/QHttp>
#include <QDomDocument>

#include "mediakeysobserver.h"
#endif

Player::Player(QWidget *parent)
    : QWidget(parent)
    , videoWidget(0)
    , coverLabel(0)
    , slider(0)
#ifdef Q_OS_SYMBIAN
    , mediaKeysObserver(0)
    , playlistDialog(0)
    , toggleAspectRatio(0)
    , showYoutubeDialog(0)
    , youtubeDialog(0)
    , audioEndpointSelector(0)
#else
    , colorDialog(0)
#endif
{
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(this);
    player->setPlaylist(playlist);

    connect(player, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(player, SIGNAL(positionChanged(qint64)), SLOT(positionChanged(qint64)));
    connect(player, SIGNAL(metaDataChanged()), SLOT(metaDataChanged()));
    connect(playlist, SIGNAL(currentIndexChanged(int)), SLOT(playlistPositionChanged(int)));
    connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(statusChanged(QMediaPlayer::MediaStatus)));
    connect(player, SIGNAL(bufferStatusChanged(int)), this, SLOT(bufferingProgress(int)));
    connect(player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(displayErrorMessage()));

    videoWidget = new VideoWidget(this);
    player->addVideoOutput(videoWidget);

    playlistModel = new PlaylistModel(this);
    playlistModel->setPlaylist(playlist);

    playlistView = new QListView(this);
    playlistView->setModel(playlistModel);
    playlistView->setCurrentIndex(playlistModel->index(playlist->currentIndex(), 0));

    connect(playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(jump(QModelIndex)));

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, player->duration() / 1000);

    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(seek(int)));
    
    audioEndpointSelector = qobject_cast<QAudioEndpointSelector*>(player->service()->requestControl(QAudioEndpointSelector_iid));
    connect(audioEndpointSelector, SIGNAL(activeEndpointChanged(const QString&)), this, SLOT(handleAudioOutputChangedSignal(const QString&)));

#ifndef Q_OS_SYMBIAN
    QPushButton *openButton = new QPushButton(tr("Open"), this);

    connect(openButton, SIGNAL(clicked()), this, SLOT(open()));
#endif

    PlayerControls *controls = new PlayerControls(this);
    controls->setState(player->state());
    controls->setVolume(player->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, SIGNAL(play()), player, SLOT(play()));
    connect(controls, SIGNAL(pause()), player, SLOT(pause()));
    connect(controls, SIGNAL(stop()), player, SLOT(stop()));
    connect(controls, SIGNAL(next()), playlist, SLOT(next()));
    connect(controls, SIGNAL(previous()), this, SLOT(previousClicked()));
    connect(controls, SIGNAL(changeVolume(int)), player, SLOT(setVolume(int)));
    connect(controls, SIGNAL(changeMuting(bool)), player, SLOT(setMuted(bool)));
    connect(controls, SIGNAL(changeRate(qreal)), player, SLOT(setPlaybackRate(qreal)));

    connect(controls, SIGNAL(stop()), videoWidget, SLOT(update()));

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
            controls, SLOT(setState(QMediaPlayer::State)));
    connect(player, SIGNAL(volumeChanged(int)), controls, SLOT(setVolume(int)));
    connect(player, SIGNAL(mutedChanged(bool)), controls, SLOT(setMuted(bool)));

#ifndef Q_OS_SYMBIAN
    QPushButton *fullScreenButton = new QPushButton(tr("FullScreen"), this);
    fullScreenButton->setCheckable(true);

    if (videoWidget != 0) {
        connect(fullScreenButton, SIGNAL(clicked(bool)), videoWidget, SLOT(setFullScreen(bool)));
        connect(videoWidget, SIGNAL(fullScreenChanged(bool)),
                fullScreenButton, SLOT(setChecked(bool)));
    } else {
        fullScreenButton->setEnabled(false);
    }

    QPushButton *colorButton = new QPushButton(tr("Color Options..."), this);
    if (videoWidget)
        connect(colorButton, SIGNAL(clicked()), this, SLOT(showColorDialog()));
    else
        colorButton->setEnabled(false);

#endif

#ifdef Q_OS_SYMBIAN
    // Set some sensible default volume.
    player->setVolume(50);

    QLabel *label = new QLabel(tr("Playlist"), this);
    QVBoxLayout *playlistDialogLayout = new QVBoxLayout;
    playlistDialogLayout->addWidget(label);
    playlistDialogLayout->addWidget(playlistView);
    playlistDialog = new QDialog(this);
    playlistDialog->setWindowTitle(tr("Playlist"));
    playlistDialog->setLayout(playlistDialogLayout);
    playlistDialog->setContextMenuPolicy(Qt::NoContextMenu);

    QAction *close = new QAction(tr("Close"), this);
    close->setSoftKeyRole(QAction::NegativeSoftKey);
    playlistDialog->addAction(close);

    mediaKeysObserver = new MediaKeysObserver(this);

    coverLabel = new QLabel(this);
    coverLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    coverLabel->setMinimumSize(1, 1);
    coverLabel->hide();

    slider->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    slider->setMinimumSize(1, 1);

    connect(controls, SIGNAL(open()), this, SLOT(open()));
    connect(controls, SIGNAL(fullScreen(bool)), this, SLOT(handleFullScreen(bool)));
    connect(controls, SIGNAL(openPlayList()), this, SLOT(showPlayList()));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(handleStateChange(QMediaPlayer::State)));
    connect(mediaKeysObserver, SIGNAL(mediaKeyPressed(MediaKeysObserver::MediaKeys)), this, SLOT(handleMediaKeyEvent(MediaKeysObserver::MediaKeys)));
    connect(close, SIGNAL(triggered()), playlistDialog, SLOT(reject()));

    QBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(videoWidget, 7);
    layout->addWidget(coverLabel, 7);
    layout->addWidget(slider, 1);
    layout->addWidget(controls, 2);

    createMenus();
#else
    QBoxLayout *displayLayout = new QHBoxLayout;
    if (videoWidget)
        displayLayout->addWidget(videoWidget, 2);
    else
        displayLayout->addWidget(coverLabel, 2);
    displayLayout->addWidget(playlistView);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);
    controlLayout->addWidget(fullScreenButton);
    controlLayout->addWidget(colorButton);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    layout->addWidget(slider);
    layout->addLayout(controlLayout);
#endif

    setLayout(layout);

    metaDataChanged();

    QStringList arguments = qApp->arguments();
    arguments.removeAt(0);
    foreach (QString const &argument, arguments) {
        QFileInfo fileInfo(argument);
        if (fileInfo.exists()) {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            if (fileInfo.suffix().toLower() == QLatin1String("m3u")) {
                playlist->load(url);
            } else
                playlist->addMedia(url);
        } else {
            QUrl url(argument);
            if (url.isValid()) {
                playlist->addMedia(url);
            }
        }
    }
}

Player::~Player()
{
}

void Player::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames();
    foreach (QString const &fileName, fileNames)
        playlist->addMedia(QUrl::fromLocalFile(fileName));
}

void Player::durationChanged(qint64 duration)
{
    slider->setMaximum(duration / 1000);
}

void Player::positionChanged(qint64 progress)
{
    if (!slider->isSliderDown()) {
        slider->setValue(progress / 1000);
    }
}

void Player::metaDataChanged()
{
#ifdef Q_OS_SYMBIAN
    if (player->isMetaDataAvailable()) {
        setTrackInfo(QString("(%1/%2) %3 - %4")
                .arg(playlist->currentIndex()+1)
                .arg(playlist->mediaCount())
                .arg(player->metaData(QtMediaServices::AlbumArtist).toString())
                .arg(player->metaData(QtMediaServices::Title).toString()));

        if (!player->isVideoAvailable()) {
            QUrl uri = player->metaData(QtMediaServices::CoverArtUrlLarge).value<QUrl>();
            QPixmap pixmap = NULL;

            if (uri.isEmpty()) {
                QVariant picture = player->extendedMetaData("attachedpicture");
                // Load picture from metadata
                if (!picture.isNull() && picture.canConvert<QByteArray>())
                    pixmap.loadFromData(picture.value<QByteArray>());

                // Load some jpg from same dir as media
                else {
                    QUrl url = player->media().canonicalUrl();
                    QString path = url.path();
                    path = path.mid(1,path.lastIndexOf("/"));
                    QRegExp rx("*.jpg");
                    rx.setCaseSensitivity(Qt::CaseInsensitive);
                    rx.setPatternSyntax(QRegExp::Wildcard);
                    QDir directory(path);
                    QStringList allFiles = directory.entryList(QDir::Files | QDir::NoSymLinks);

                    foreach (QString file, allFiles)
                        if (rx.exactMatch(file)) {
                            path.append(file);
                            break;
                        }
                   pixmap.load(path);
                }
                // Load picture from file pointed by uri
            } else
                pixmap.load(uri.toString());

            coverLabel->setPixmap((!pixmap.isNull())?pixmap:QPixmap());
            }
    hideOrShowCoverArt();
    }
#else
    //qDebug() << "update metadata" << player->metaData(QtMediaServices::Title).toString();
    if (player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(player->metaData(QtMediaServices::AlbumArtist).toString())
                .arg(player->metaData(QtMediaServices::Title).toString()));

        if (coverLabel) {
            QUrl url = player->metaData(QtMediaServices::CoverArtUrlLarge).value<QUrl>();

            coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }
    }
#endif
}

void Player::previousClicked()
{
    // Go to previous track if we are within the first 5 seconds of playback
    // Otherwise, seek to the beginning.
    if(player->position() <= 5000)
        playlist->previous();
    else
        player->setPosition(0);
}

void Player::jump(const QModelIndex &index)
{
#ifdef Q_OS_SYMBIAN
    if (playlistDialog->isVisible())
        playlistDialog->accept();
#endif
    if (index.isValid()) {
        playlist->setCurrentIndex(index.row());
        player->play();
    }
}

void Player::playlistPositionChanged(int currentItem)
{
    playlistView->setCurrentIndex(playlistModel->index(currentItem, 0));
}

void Player::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void Player::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if( status == QMediaPlayer::LoadingMedia ||
        status == QMediaPlayer::BufferingMedia ||
        status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void Player::setTrackInfo(const QString &info)
{
    trackInfo = info;
#ifdef Q_OS_SYMBIAN
    QMainWindow *main = qobject_cast<QMainWindow *>(this->parent());
    if (!statusInfo.isEmpty())
        main->setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        main->setWindowTitle(trackInfo);
#else
    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
#endif
}

void Player::setStatusInfo(const QString &info)
{
    statusInfo = info;
#ifdef Q_OS_SYMBIAN
    QMainWindow *main = qobject_cast<QMainWindow *>(this->parent());
    if (!statusInfo.isEmpty())
        main->setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        main->setWindowTitle(trackInfo);
#else
    if (!statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(trackInfo).arg(statusInfo));
    else
        setWindowTitle(trackInfo);
#endif
}

void Player::displayErrorMessage()
{
#ifdef Q_OS_SYMBIAN
    if(player->error()!=QMediaPlayer::NoError)
        QMessageBox::critical(NULL, tr("Error"), player->errorString(), QMessageBox::Ok);
#else
    setStatusInfo(player->errorString());
#endif


}

#ifndef Q_OS_SYMBIAN
void Player::showColorDialog()
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
#endif
#ifdef Q_OS_SYMBIAN
void Player::createMenus()
{
    toggleAspectRatio = new QAction(tr("Ignore Aspect Ratio"), this);
    toggleAspectRatio->setCheckable(true);
    qobject_cast<QMainWindow *>(this->parent())->menuBar()->addAction(toggleAspectRatio);
    connect(toggleAspectRatio, SIGNAL(toggled(bool)), this, SLOT(handleAspectRatio(bool)));

    showYoutubeDialog = new QAction(tr("Youtube Search"), this);
    qobject_cast<QMainWindow *>(this->parent())->menuBar()->addAction(showYoutubeDialog);
    connect(showYoutubeDialog, SIGNAL(triggered()), this, SLOT(launchYoutubeDialog()));

    setAudioOutputDefault = new QAction(tr("Default output"), this);
    connect(setAudioOutputDefault, SIGNAL(triggered()), this, SLOT(handleAudioOutputDefault()));

    setAudioOutputAll = new QAction(tr("All outputs"), this);
    connect(setAudioOutputAll, SIGNAL(triggered()), this, SLOT(handleAudioOutputAll()));

    setAudioOutputNone = new QAction(tr("No output"), this);
    connect(setAudioOutputNone, SIGNAL(triggered()), this, SLOT(handleAudioOutputNone()));
    
    setAudioOutputEarphone = new QAction(tr("Earphone output"), this);
    connect(setAudioOutputEarphone, SIGNAL(triggered()), this, SLOT(handleAudioOutputEarphone()));
    
    setAudioOutputSpeaker = new QAction(tr("Speaker output"), this);
    connect(setAudioOutputSpeaker, SIGNAL(triggered()), this, SLOT(handleAudioOutputSpeaker()));

    audioOutputMenu = new QMenu(tr("Set Audio Output"), this);
    audioOutputMenu->addAction(setAudioOutputDefault);
    audioOutputMenu->addAction(setAudioOutputAll);
    audioOutputMenu->addAction(setAudioOutputNone);
    audioOutputMenu->addAction(setAudioOutputEarphone);
    audioOutputMenu->addAction(setAudioOutputSpeaker);

    qobject_cast<QMainWindow *>(this->parent())->menuBar()->addMenu(audioOutputMenu);
}

void Player::handleFullScreen(bool isFullscreen)
{
    QMainWindow* mainWindow = qobject_cast<QMainWindow *>(this->parent());
    if(isFullscreen) {
        if(player->state()==QMediaPlayer::StoppedState)
            videoWidget->setFullScreen(false);
        else
            videoWidget->setFullScreen(true);

        qobject_cast<QMainWindow *>(this->parent())->showFullScreen();
    } else
        qobject_cast<QMainWindow *>(this->parent())->showMaximized();
}

void Player::handleAspectRatio(bool aspectRatio)
{
    if(aspectRatio) {
        toggleAspectRatio->setText(tr("Keep Aspect Ratio"));
        videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);

    } else {
        toggleAspectRatio->setText(tr("Ignore Aspect Ratio"));
        videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    }
}

void Player::handleAudioOutputDefault()
{
    audioEndpointSelector->setActiveEndpoint("Default");
}

void Player::handleAudioOutputAll()
{
    audioEndpointSelector->setActiveEndpoint("All");
}

void Player::handleAudioOutputNone()
{
    audioEndpointSelector->setActiveEndpoint("None");
}

void Player::handleAudioOutputEarphone()
{
    audioEndpointSelector->setActiveEndpoint("Earphone");
}

void Player::handleAudioOutputSpeaker()
{
    audioEndpointSelector->setActiveEndpoint("Speaker");
}

void Player::handleAudioOutputChangedSignal(const QString&)
{
    QMessageBox msgBox;
    msgBox.setText("Output changed: " + audioEndpointSelector->activeEndpoint());
    msgBox.exec();
}

void Player::hideOrShowCoverArt()
{
    if(player->isVideoAvailable()) {
        coverLabel->hide();
        videoWidget->show();
        videoWidget->repaint();
    } else {
        coverLabel->show();
        videoWidget->hide();
    }
}

void Player::handleStateChange(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::PausedState)
        return;

    handleFullScreen(qobject_cast<QMainWindow *>(this->parent())->isFullScreen());
}

void Player::handleMediaKeyEvent(MediaKeysObserver::MediaKeys key)
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
void Player::showPlayList()
{
    if (!playlistDialog)
        return;

    playlistDialog->exec();
}

void Player::launchYoutubeDialog()
{
    if(!youtubeDialog)  {
        youtubeDialog = new QDialog(this);

        QLineEdit *input= new QLineEdit(youtubeDialog);
        QPushButton *searchButton = new QPushButton("Search", youtubeDialog);
        QListWidget *resultList = new QListWidget(youtubeDialog);
        QAction *add = new QAction(tr("Add"), youtubeDialog);
        QAction *close = new QAction(tr("Close"), youtubeDialog);

        add->setSoftKeyRole(QAction::PositiveSoftKey);
        close->setSoftKeyRole(QAction::NegativeSoftKey);
        youtubeDialog->addAction(add);
        youtubeDialog->addAction(close);

        connect(searchButton, SIGNAL(clicked()), this, SLOT(searchYoutubeVideo()));
        connect(add, SIGNAL(triggered()), this, SLOT(addYoutubeVideo()));
        connect(close, SIGNAL(triggered()), youtubeDialog, SLOT(close()));
        connect(&http, SIGNAL(requestFinished(int, bool)), this, SLOT(youtubeHttpRequestFinished(int, bool)));
        connect(&http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)), this, SLOT(youtubeReadResponseHeader(const QHttpResponseHeader&)));

        QHBoxLayout *topLayout = new QHBoxLayout;
        topLayout->addWidget(input);
        topLayout->addWidget(searchButton);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topLayout);
        mainLayout->addWidget(resultList);
        youtubeDialog->setLayout(mainLayout);
    }
    youtubeDialog->showMaximized();
}

void Player::youtubeReadResponseHeader(const QHttpResponseHeader& responseHeader)
{
    switch (responseHeader.statusCode())
    {
        case 200:   // Ok
        case 301:   // Moved Permanently
        case 302:   // Found
        case 303:   // See Other
        case 307:   // Temporary Redirect
            // these are not error conditions
            break;
        default: {
            http.abort();
            QMessageBox::critical(NULL, tr("Error"), tr("Download failed: %1.").arg(responseHeader.reasonPhrase()));
            break;
        }
    }
}

void Player::addYoutubeVideo()
{
    if(!youtubeDialog)
        return;

    QListWidget *resultList = youtubeDialog->findChild<QListWidget *>();
    if(!resultList || resultList->count() == 0)
        return;

    playlist->addMedia(resultList->currentItem()->data(Qt::UserRole).toUrl());
}

void Player::searchYoutubeVideo()
{
    if(!youtubeDialog)
        return;

    QLineEdit *input = youtubeDialog->findChild<QLineEdit *>();
    QListWidget *resultList = youtubeDialog->findChild<QListWidget *>();
    QString urlstring = QString("http://gdata.youtube.com/feeds/api/videos?q=%1&max-results=25&v=2&format=6").arg(input->text().replace(' ', '+'));
    QUrl url(urlstring);
    http.setHost(url.host(), QHttp::ConnectionModeHttp, url.port() == -1 ? 0 : url.port());
    resultList->clear();
    httpGetId = http.get(urlstring);
}

void Player::youtubeHttpRequestFinished(int requestId, bool error)
{
    if(!youtubeDialog || requestId != httpGetId)
        return;

    if (error) {
        QMessageBox::critical(NULL, tr("Error"), tr("Download failed: %1.").arg(http.errorString()));
        return;
    }

    QTemporaryFile file;
    if (!file.open()) {
        QMessageBox::critical(NULL, tr("Error"), tr("Could not open temporary file"));
        return;
    }

    QString data = http.readAll();
    QTextStream out(&file);
    out << data;
    file.close();

    QDomDocument domDocument;
    QString errorMessage;
    if (!domDocument.setContent(&file, true, &errorMessage)) {
        QMessageBox::critical(NULL, tr("Error"), errorMessage);
        return;
    }

    QDomElement root = domDocument.documentElement();
    if (root.tagName() != "feed")
        return;

    QListWidget *resultList = youtubeDialog->findChild<QListWidget *>();
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
                QListWidgetItem* item = new QListWidgetItem(title, resultList);
                item->setData(Qt::UserRole, incidentElement2.attribute("url"));
                break;
            }
            incidentElement2 = incidentElement2.nextSiblingElement("content");
        }
        entryElement = entryElement.nextSiblingElement("entry");
    }
}
#endif
