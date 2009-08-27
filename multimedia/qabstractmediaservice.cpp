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

#include "qabstractmediaservice.h"

#include <QtCore/qtimer.h>

class QAbstractMediaServicePrivate
{
public:
    QAbstractMediaServicePrivate()
    {
    }

    QByteArray audioOutput;
    QByteArray videoOutput;
    QByteArray dataOutput;
    QByteArray audioInput;
    QByteArray videoInput;
    QByteArray dataInput;
};

/*!
    \class QAbstractMediaService
*/

/*!
    Construct a QAbstractMediaService with \a parent. This class is meant as a
    base class for Multimedia services so this constructor is protected.
*/

QAbstractMediaService::QAbstractMediaService(QObject *parent)
    : QObject(parent)
    , d_ptr(new QAbstractMediaServicePrivate)
{
}

QAbstractMediaService::~QAbstractMediaService()
{
    delete d_ptr;
}

/*!
    Returns the currently active endpoint id for \endpointType.
*/

/*!
    Sets the active endpoint for \a endpointType to \a endpoint
*/

/*!
    Returns a list of endpoints available for the \a endpointType.
*/

QList<QString> QAbstractMediaService::supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const
{
    return QList<QString>();
}
