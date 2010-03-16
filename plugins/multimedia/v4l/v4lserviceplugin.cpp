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
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdir.h>

#include "v4lserviceplugin.h"
#ifndef Q_WS_MAEMO_5
#include "v4lcameraservice.h"
#endif
#include "v4lradioservice.h"

#include <qmediaserviceprovider.h>


QStringList V4LServicePlugin::keys() const
{
#ifdef Q_WS_MAEMO_5
    return QStringList() <<
        QLatin1String(Q_MEDIASERVICE_RADIO);
#else
    return QStringList() <<
        QLatin1String(Q_MEDIASERVICE_RADIO) <<
        QLatin1String(Q_MEDIASERVICE_CAMERA);

#endif
}

QMediaService* V4LServicePlugin::create(QString const& key)
{
    if (key == QLatin1String(Q_MEDIASERVICE_RADIO))
        return new V4LRadioService;
#ifndef Q_WS_MAEMO_5
    if (key == QLatin1String(Q_MEDIASERVICE_CAMERA))
        return new V4LCameraService;
#endif
    return 0;
}

void V4LServicePlugin::release(QMediaService *service)
{
    delete service;
}

QList<QByteArray> V4LServicePlugin::devices(const QByteArray &service) const
{
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        return m_cameraDevices;
    }

    return QList<QByteArray>();
}

QString V4LServicePlugin::deviceDescription(const QByteArray &service, const QByteArray &device)
{
    if (service == Q_MEDIASERVICE_CAMERA) {
        if (m_cameraDevices.isEmpty())
            updateDevices();

        for (int i=0; i<m_cameraDevices.count(); i++)
            if (m_cameraDevices[i] == device)
                return m_cameraDescriptions[i];
    }

    return QString();
}

void V4LServicePlugin::updateDevices() const
{
    m_cameraDevices.clear();
    m_cameraDescriptions.clear();

    QDir cameras = QDir::root();
    cameras.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    if (!cameras.cd("sys/class/video4linux"))
        return;
    else {
        QFileInfoList list = cameras.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.fileName().contains(QLatin1String("video"))) {
                QString videoName = QString("%1/%2/name").arg("/sys/class/video4linux").
                    arg(fileInfo.fileName());
                QFile videoFile(videoName);
                if (videoFile.exists()) {
                    if (videoFile.open(QIODevice::ReadOnly)) {
                        char str[31];
                        memset(str,0,31);
                        if (videoFile.read(str,30) > 0) {
                            m_cameraDevices.append(QString("v4l:/dev/%1").arg(fileInfo.fileName()).
                                    toLocal8Bit().constData());
                            m_cameraDescriptions.append(QString(str).simplified());
                        }
                        videoFile.close();
                    }
                }
            }
        }
    }
}


Q_EXPORT_PLUGIN2(v4lengine, V4LServicePlugin);

