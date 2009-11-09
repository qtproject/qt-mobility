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


#include <qmediaformatcontrol.h>

/*!
    \class QMediaFormatControl
    \preliminary
    \brief The QMediaFormat class provides access to the output format of a QMediaService

    If a QMediaService supports writing encoded data it will implement QMediaFormatControl.  This
    control provides information about the output formats supported by a media service and allows
    one to be selected as the current output format..

    The functionality provided by this control is exposed to application code through the
    QMediaRecorder class.

    The interface name of QMediaFormatControl is \c com.nokia.Qt.QMediaFormatControl/1.0 as
    defined in QMediaFormatControl_iid.

    \sa QMediaService::control(), QMediaRecorder
*/

/*!
    \macro QMediaFormatControl_iid

    \c com.nokia.Qt.QMediaFormatControl/1.0

    Defines the interface name of the QMediaFormatControl class.

    \relates QMediaFormatControl
*/

/*!
    Constructs a new media format control with the given \a parent.
*/
QMediaFormatControl::QMediaFormatControl(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
    Destroys a media format control.
*/
QMediaFormatControl::~QMediaFormatControl()
{
}


/*!
    \fn QMediaFormatControl::supportedFormats() const

    Returns a list of MIME types of supported formats.
*/

/*!
    \fn QMediaFormatControl::format() const

    Returns the MIME type of the selected format.
*/

/*!
    \fn QMediaFormatControl::setFormat(const QString &mimeType)

    Sets the current format to the format identified by the given \a mimeType.
*/

/*!
    \fn QMediaFormatControl::formatDescription(const QString &mimeType) const

    Returns a description of the format identified by the given \a mimeType.
*/
