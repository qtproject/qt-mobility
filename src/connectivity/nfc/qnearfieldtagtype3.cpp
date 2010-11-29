/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qnearfieldtagtype3.h"
#include <QtCore/QByteArray>

QTM_BEGIN_NAMESPACE

QNearFieldTagType3::QNearFieldTagType3(QObject *parent) : QNearFieldTarget(parent)
{
}

quint16 QNearFieldTagType3::systemCode()
{
    return 0;
}

QList<quint16> QNearFieldTagType3::services()
{
    return QList<quint16>();
}

int QNearFieldTagType3::serviceMemorySize(quint16 serviceCode)
{
    Q_UNUSED(serviceCode);
    return -1;
}

QByteArray serviceData(quint16 serviceCode)
{
    Q_UNUSED(serviceCode);
    return QByteArray();
}

void QNearFieldTagType3::writeServiceData(quint16 serviceCode, const QByteArray &data)
{
    Q_UNUSED(serviceCode);
    Q_UNUSED(data);
}

QMap<quint16, QByteArray> QNearFieldTagType3::check(const QMap<quint16, QList<unsigned int> > &serviceBlockList)
{
    Q_UNUSED(serviceBlockList);
    return QMap<quint16, QByteArray>();
}

void QNearFieldTagType3::update(const QMap<quint16, QList<unsigned int> > &serviceBlockList, const QByteArray &data)
{
    Q_UNUSED(serviceBlockList);
    Q_UNUSED(data);
}
#include "moc_qnearfieldtagtype3.cpp"

QTM_END_NAMESPACE
