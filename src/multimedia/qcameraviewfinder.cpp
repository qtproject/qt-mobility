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

#include <QDebug>

#include <qmediaobject_p.h>

#include <qcamera.h>
#include <qcameraviewfinder.h>
#include <qvideowidget_p.h>
#include <qvideodevicecontrol.h>

QT_USE_NAMESPACE

/*!
    \class QCameraViewfinder

    \preliminary
    \brief The QCameraViewfinder class provides camera viewfinder widget.

    \inmodule QtMultimediaKit
    \ingroup camera

    \code
        camera = new QCamera;
        viewFinder = new QCameraViewfinder();
        viewFinder->show();
        camera->setViewfinder(viewFinder);

        camera->setCaptureMode(QCamera::CaptureStillImage);
        camera->start();
    \endcode

The Camera API of Qt Mobility is still in \bold Technology Preview. It has
not undergone the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.
*/

class QCameraViewfinderPrivate : public QVideoWidgetPrivate
{
    Q_DECLARE_NON_CONST_PUBLIC(QCameraViewfinder)
public:
    QCameraViewfinderPrivate():
        QVideoWidgetPrivate()
    {
    }
};

QCameraViewfinder::QCameraViewfinder(QWidget *parent)
    :QVideoWidget(*new QCameraViewfinderPrivate, parent)
{
}

QCameraViewfinder::~QCameraViewfinder()
{
}

/*!
  \reimp
*/
QMediaObject *QCameraViewfinder::mediaObject() const
{
    return QVideoWidget::mediaObject();
}

/*!
  \reimp
*/
bool QCameraViewfinder::setMediaObject(QMediaObject *object)
{
    Q_D(QCameraViewfinder);

    return QVideoWidget::setMediaObject(object);
}

#include "moc_qcameraviewfinder.cpp"
