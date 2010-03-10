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

#include <experimental/qimagecapturecontrol.h>
#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QImageCaptureControl
    
    \brief The QImageCaptureControl class provides a control interface
    for image capture services.

    \ingroup camera

    \preliminary

    The interface name of QImageCaptureControl is \c com.nokia.Qt.QImageCaptureControl/1.0 as
    defined in QImageCaptureControl_iid.

The Camera API of Qt Mobility is still in \bold Technology Preview. It has not undergone
the same level of review and testing as the rest of the APIs.

The API exposed by the classes in this component are not stable, and will
undergo modification or removal prior to the final release of Qt Mobility.

    \sa QMediaService::control()
*/

/*!
    \macro QImageCaptureControl_iid

    \c com.nokia.Qt.QImageCaptureControl/1.0

    Defines the interface name of the QImageCaptureControl class.

    \relates QImageCaptureControl
*/

/*!
    Constructs a new image capture control object with the given \a parent
*/
QImageCaptureControl::QImageCaptureControl(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
    Destroys an image capture control.
*/
QImageCaptureControl::~QImageCaptureControl()
{
}

/*!
    \fn QImageCaptureControl::isReadyForCapture() const

    Identifies if a capture control is ready to perform a capture.

    Returns true if the control is ready; and false if it is not.
*/

/*!
    \fn QImageCaptureControl::readyForCaptureChanged(bool ready)

    Signals that a capture control's \a ready state has changed.
*/

/*!
    \fn QImageCaptureControl::capture(const QString &fileName)

    Initiates the capture of an image to \a fileName.
    The \a fileName can be relative or empty,
    in this case the service should use the system specific place
    and file naming scheme.

    The sequental imageSaved() signal should use
    the absolute file name.
*/

/*!
    \fn QImageCaptureControl::cancelCapture()

    Cancel pending capture requests.
*/

/*!
    \fn QImageCaptureControl::imageCaptured(const QString &fileName, const QImage &preview)

    Signals that an image intendec to be saved to to \a fileName
    has been captured and a \a preview is available.
*/


/*!
    \fn QImageCaptureControl::imageSaved(const QString &fileName)

    Signals that an captured image has been saved to \a fileName.
*/

#include "moc_qimagecapturecontrol.cpp"
QTM_END_NAMESPACE

