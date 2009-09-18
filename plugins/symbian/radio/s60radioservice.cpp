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

#include "s60radioservice.h"
#include "s60radiocontrol.h"

S60RadioService::S60RadioService(QObject *parent)
    : QRadioService(parent)
{
    m_control = new S60RadioControl(this);
}

S60RadioService::~S60RadioService()
{
}

QAbstractMediaControl *S60RadioService::control(const char* name) const
{
    Q_UNUSED(name)

    return m_control;
}

bool S60RadioService::isEndpointSupported(QAbstractMediaService::MediaEndpoint endpointType)
{
    return false;
}

void S60RadioService::setInputStream(QIODevice* stream)
{
}

QIODevice* S60RadioService::inputStream() const
{
    return 0;
}

void S60RadioService::setOutputStream(QIODevice* stream)
{
}

QIODevice* S60RadioService::outputStream() const
{
    return 0;
}

QString S60RadioService::activeEndpoint(QAbstractMediaService::MediaEndpoint endpointType)
{
    return QByteArray();
}

bool S60RadioService::setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    return true;
}

QList<QString> S60RadioService::supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const
{
    QList<QString> list;
    //TODO
    return list;
}

