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

#ifndef QMEDIASLIDESHOWSERVICE_P_H
#define QMEDIASLIDESHOWSERVICE_P_H

#include <multimedia/qabstractmediaservice.h>
#include <multimedia/qmediaslideshow.h>
#include <multimedia/qvideooutputcontrol.h>

class QAbstractVideoSurface;
class QNetworkAccessManager;

class QMediaSlideShowServicePrivate;

class QMediaSlideShowService : public QAbstractMediaService
{
    Q_OBJECT
public:
    explicit QMediaSlideShowService(QObject *parent = 0);
    ~QMediaSlideShowService();

    QAbstractMediaControl *control(const char *name) const;

    QNetworkAccessManager *networkManager() const;

private:
    Q_DECLARE_PRIVATE(QMediaSlideShowService)
#ifndef QT_NO_MULTIMEDIA
    Q_PRIVATE_SLOT(d_func(), void _q_surfaceChanged(QAbstractVideoSurface *surface))
#endif
    Q_PRIVATE_SLOT(d_func(), void _q_outputChanged(QVideoOutputControl::Output output))
    friend class QMediaSlideShowControl;
    friend class QMediaSlideShowControlPrivate;
};

class QMediaSlideShowControlPrivate;

class QMediaSlideShowControl : public QAbstractMediaControl
{
    Q_OBJECT
public:
    explicit QMediaSlideShowControl(QMediaSlideShowService *parent);
    ~QMediaSlideShowControl();

    QMediaSlideShow::MediaStatus mediaStatus() const;

    QMediaSource media() const;
    void setMedia(const QMediaSource &media);

    QMediaResource currentMedia() const;

Q_SIGNALS:
    void mediaChanged(const QMediaSource &media);
    void currentMediaChanged(const QMediaResource &media);
    void mediaStatusChanged(QMediaSlideShow::MediaStatus status);

private:
    Q_DECLARE_PRIVATE(QMediaSlideShowControl)
    Q_PRIVATE_SLOT(d_func(), void _q_headFinished())
    Q_PRIVATE_SLOT(d_func(), void _q_getFinished())
};

#define QMediaSlideShowControl_iid "com.nokia.Qt.QMediaSlideShowControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QMediaSlideShowControl, QMediaSlideShowControl_iid)

#endif
