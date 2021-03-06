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
#include "simulatorcamerasession.h"
#include "simulatorcamerasettings.h"
#include "../qsimulatormultimediaconnection_p.h"
#include <qmediarecorder.h>
#include <qcameraimagecapture.h>

#include <QtCore/qdebug.h>
#include <QCoreApplication>
#include <QtCore/qmetaobject.h>

#include <QtGui/qimage.h>

//#define CAMERA_DEBUG 1

SimulatorCameraSession::SimulatorCameraSession(QObject *parent)
    :QObject(parent),
      mViewfinder(0),
      mImage(0),
      mRequestId(0)
{
    mSettings = new SimulatorCameraSettings(this);
}

SimulatorCameraSession::~SimulatorCameraSession()
{
}

int SimulatorCameraSession::captureImage(const QString &fileName)
{
    QTM_USE_NAMESPACE;
    ++mRequestId;
    emit imageExposed(mRequestId);

    QString actualFileName = fileName;
    if (actualFileName.isEmpty()) {
        actualFileName = generateFileName("img_", defaultDir(QCamera::CaptureStillImage), "jpg");
    }

    emit imageCaptured(mRequestId, *mImage);

    if (!mImage->save(actualFileName)) {
        emit captureError(mRequestId, QCameraImageCapture::ResourceError, "Could not save file");
        return mRequestId;
    }
    emit imageSaved(mRequestId, actualFileName);
    return mRequestId;
}

void SimulatorCameraSession::setCaptureMode(QCamera::CaptureMode mode)
{
    mCaptureMode = mode;
}

QDir SimulatorCameraSession::defaultDir(QCamera::CaptureMode) const
{
    const QString temp = QDir::tempPath();
    if (QFileInfo(temp).isWritable())
        return QDir(temp);

    return QDir();
}

QString SimulatorCameraSession::generateFileName(const QString &prefix, const QDir &dir, const QString &ext) const
{
    int lastClip = 0;
    foreach(QString fileName, dir.entryList(QStringList() << QString("%1*.%2").arg(prefix).arg(ext))) {
        int imgNumber = fileName.mid(prefix.length(), fileName.size()-prefix.length()-ext.length()-1).toInt();
        lastClip = qMax(lastClip, imgNumber);
    }

    QString name = QString("%1%2.%3").arg(prefix)
                                     .arg(lastClip+1,
                                     4, //fieldWidth
                                     10,
                                     QLatin1Char('0'))
                                     .arg(ext);

    return dir.absoluteFilePath(name);
}

void SimulatorCameraSession::setViewfinder(QObject *viewfinder)
{
    if (mViewfinder != viewfinder) {
        mViewfinder = viewfinder;
        emit viewfinderChanged();
    }
}

QCamera::CaptureMode SimulatorCameraSession::captureMode()
{
    return mCaptureMode;
}

void SimulatorCameraSession::setImage(const QImage *image)
{
    mImage = image;
}

QObject *SimulatorCameraSession::viewfinder() const
{
    return mViewfinder;
}

SimulatorCameraSettings *SimulatorCameraSession::settings() const
{
    return mSettings;
}
