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

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>
#include <QtCore/qfile.h>

#include "v4lradioservice.h"
#include "v4lradiocontrol.h"

V4LRadioService::V4LRadioService(QObject *parent)
    : QRadioService(parent)
{
    m_control = new V4LRadioControl(this);
}

V4LRadioService::~V4LRadioService()
{
}

QAbstractMediaControl *V4LRadioService::control(const char* name) const
{
    Q_UNUSED(name)

    return m_control;
}

bool V4LRadioService::isEndpointSupported(QAbstractMediaService::MediaEndpoint endpointType)
{
    return false;
}

void V4LRadioService::setInputStream(QIODevice* stream)
{
}

QIODevice* V4LRadioService::inputStream() const
{
    return 0;
}

void V4LRadioService::setOutputStream(QIODevice* stream)
{
}

QIODevice* V4LRadioService::outputStream() const
{
    return 0;
}

QString V4LRadioService::activeEndpoint(QAbstractMediaService::MediaEndpoint endpointType)
{
    return QByteArray();
}

void V4LRadioService::setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const char *interface)
{
}

QList<QString> V4LRadioService::supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const
{
    QList<QString> list;
    //TODO
    return list;
}

