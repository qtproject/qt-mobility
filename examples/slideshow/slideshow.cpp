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

#include "slideshow.h"

#include <multimedia/qabstractmediaservice.h>
#include <multimedia/qmediaplaylistprovider.h>

#include <QtGui>

SlideShow::SlideShow(QWidget *parent)
    : QWidget(parent)
    , slideShow(0)
    , imageLabel(0)
    , playButton(0)
    , stopButton(0)
{
    slideShow = new QMediaSlideShow(this);

    connect(slideShow->service(), SIGNAL(currentImageChanged(QImage)),
            this, SLOT(setImage(QImage)));
    connect(slideShow, SIGNAL(stateChanged(QMediaSlideShow::State)),
            this, SLOT(stateChanged(QMediaSlideShow::State)));

    imageLabel = new QLabel;
    imageLabel->setScaledContents(true);
    imageLabel->setMinimumSize(320, 240);
    imageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    QMenu *openMenu = new QMenu(this);
    openMenu->addAction(tr("Directory..."), this, SLOT(openDirectory()));
    openMenu->addAction(tr("Playlist..."), this, SLOT(openPlaylist()));
    openMenu->addAction(tr("Location..."), this, SLOT(openLocation()));

    QToolButton *openButton = new QToolButton;
    openButton->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
    openButton->setMenu(openMenu);
    openButton->setPopupMode(QToolButton::InstantPopup);

    playButton = new QToolButton;
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(playButton, SIGNAL(clicked()), this, SLOT(play()));

    stopButton = new QToolButton;
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setEnabled(false);

    connect(stopButton, SIGNAL(clicked()), slideShow, SLOT(stop()));

    QAbstractButton *nextButton = new QToolButton;
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    connect(nextButton, SIGNAL(clicked()), slideShow, SLOT(next()));

    QAbstractButton *previousButton = new QToolButton;
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

    connect(previousButton, SIGNAL(clicked()), slideShow, SLOT(previous()));

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addStretch(1);
    controlLayout->addWidget(previousButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(playButton);
    controlLayout->addWidget(nextButton);
    controlLayout->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel, Qt::AlignCenter);
    layout->addLayout(controlLayout);

    setLayout(layout);

}

void SlideShow::openPlaylist()
{
    QString path = QFileDialog::getOpenFileName(this);

    if (!path.isEmpty()) {
#ifndef Q_OS_WIN
        slideShow->setMedia(QMediaSource(QUrl(QLatin1String("file://") + path)));
#else
        slideShow->setMedia(QMediaSource(QUrl(QLatin1String("file:///") + path)));
#endif
    }
}

void SlideShow::openDirectory()
{
    QString path = QFileDialog::getExistingDirectory(this);

    if (!path.isEmpty()) {
        slideShow->playlist()->clear();

        QDir dir(path);

        foreach (const QString &fileName, dir.entryList(QDir::Files)) {
            QString absolutePath = dir.absoluteFilePath(fileName);
#ifndef Q_OS_WIN
            slideShow->playlist()->appendItem(
                    QMediaSource(QUrl(QLatin1String("file://") + absolutePath)));
#else
            slideShow->playlist()->appendItem(
                    QMediaSource(QUrl(QLatin1String("file:///") + absolutePath)));
#endif
        }
    }
}

void SlideShow::openLocation()
{
    QLineEdit *urlEdit = new QLineEdit;

    QDialogButtonBox *buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(tr("Location"), urlEdit);
    layout->addWidget(buttons);

    QDialog dialog(this);
    dialog.setLayout(layout);

    connect(urlEdit, SIGNAL(returnPressed()), &dialog, SLOT(accept()));
    connect(buttons, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {
        QUrl url(urlEdit->text());

        if (url.isValid())
            slideShow->setMedia(QMediaSource(url));
    }
}

void SlideShow::play()
{
    switch (slideShow->state()) {
    case QMediaSlideShow::StoppedState:
    case QMediaSlideShow::PausedState:
        slideShow->play();
        break;
    case QMediaSlideShow::PlayingState:
        slideShow->pause();
        break;
    }
}

void SlideShow::stateChanged(QMediaSlideShow::State state)
{
    switch (state) {
    case QMediaSlideShow::StoppedState:
        stopButton->setEnabled(false);
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMediaSlideShow::PlayingState:
        stopButton->setEnabled(true);
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaSlideShow::PausedState:
        stopButton->setEnabled(true);
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void SlideShow::setImage(const QImage &image)
{
    imageLabel->setPixmap(QPixmap::fromImage(image));
}
