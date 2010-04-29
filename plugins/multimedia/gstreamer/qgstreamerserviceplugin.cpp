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

#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>
#include <QtGui/QIcon>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#include "qgstreamerserviceplugin.h"

#ifdef QMEDIA_GSTREAMER_PLAYER
#include "qgstreamerplayerservice.h"
#endif
#if defined(QMEDIA_GSTREAMER_CAPTURE) && (defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
#include "qgstreamercaptureservice_maemo.h"
#elif defined(QMEDIA_GSTREAMER_CAPTURE)
#include "qgstreamercaptureservice.h"
#endif

#include <qmediaserviceprovider.h>

#include <linux/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/videodev2.h>


QStringList QGstreamerServicePlugin::keys() const
{
    return QStringList()
#ifdef QMEDIA_GSTREAMER_PLAYER
            << QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER)
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
            << QLatin1String(Q_MEDIASERVICE_AUDIOSOURCE)
#endif
            ;
}

QMediaService* QGstreamerServicePlugin::create(const QString &key)
{
#ifdef QMEDIA_GSTREAMER_PLAYER
    if (key == QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER))
        return new QGstreamerPlayerService;
#endif
#ifdef QMEDIA_GSTREAMER_CAPTURE
    if (key == QLatin1String(Q_MEDIASERVICE_AUDIOSOURCE))
        return new QGstreamerCaptureService(key);
#endif

    //qDebug() << "unsupported key:" << key;
    return 0;
}

void QGstreamerServicePlugin::release(QMediaService *service)
{
    delete service;
}

Q_EXPORT_PLUGIN2(qtmedia_gstengine, QGstreamerServicePlugin);
