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

#include <QDebug>

#include <qmediaobject_p.h>

#include <qcamera.h>
#include <qcameraviewfinder.h>
#include <qvideowidget_p.h>
#include <qvideodevicecontrol.h>

QT_USE_NAMESPACE

/*!
    \class QCameraViewfinder


    \brief The QCameraViewfinder class provides a camera viewfinder widget.

    \inmodule QtMultimediaKit
    \ingroup camera
    \since 1.1

    \snippet doc/src/snippets/multimedia-snippets/camera.cpp Camera

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

/*!
    Constructs a new camera viewfinder widget.

    The \a parent is passed to QVideoWidget.
*/

QCameraViewfinder::QCameraViewfinder(QWidget *parent)
    :QVideoWidget(*new QCameraViewfinderPrivate, parent)
{
}

/*!
    Destroys a camera viewfinder widget.
*/
QCameraViewfinder::~QCameraViewfinder()
{
}

/*!
  \reimp
  \since 1.1
*/
QMediaObject *QCameraViewfinder::mediaObject() const
{
    return QVideoWidget::mediaObject();
}

/*!
  \reimp
  \since 1.1
*/
bool QCameraViewfinder::setMediaObject(QMediaObject *object)
{
    return QVideoWidget::setMediaObject(object);
}

#include "moc_qcameraviewfinder.cpp"
