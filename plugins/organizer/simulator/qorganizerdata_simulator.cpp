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

#include "qorganizerdata_simulator_p.h"
#include "qorganizeritemenginelocalid.h"
#include "engines/qorganizeritemmemorybackend_p.h"

#include <QtCore/QDataStream>

// ### hack!
// A correct implementation is impossible, since you need to know the managerUri to create
// a QOrganizerItemManagerEngineLocalId for the manager. This one should work for the Simulator's
// needs.
QDataStream& operator>>(QDataStream& in, QtMobility::QOrganizerItemLocalId& id)
{
    quint8 marker;
    in >> marker;
    if (marker) {
        QtMobility::QOrganizerItemEngineLocalId *d = new QtMobility::QOrganizerItemMemoryEngineLocalId;
        d->dataStreamIn(in);
        id = QtMobility::QOrganizerItemLocalId(d);
    }
    return in;
}

QDataStream &operator<<(QDataStream &out, const QtMobility::Simulator::SaveOrganizerItemReply &s)
{
    out << s.savedItem << static_cast<int>(s.error);
    return out;
}

QDataStream &operator>>(QDataStream &in, QtMobility::Simulator::SaveOrganizerItemReply &s)
{
    int errorInt;
    in >> s.savedItem >> errorInt;
    s.error = static_cast<QtMobility::QOrganizerItemManager::Error>(errorInt);
    return in;
}

QTM_BEGIN_NAMESPACE

void qt_registerOrganizerTypes()
{
    qRegisterMetaTypeStreamOperators<QOrganizerItem>("QtMobility::QOrganizerItem");
    qRegisterMetaTypeStreamOperators<QOrganizerItemLocalId>("QtMobility::QOrganizerItemLocalId");
    qRegisterMetaTypeStreamOperators<Simulator::SaveOrganizerItemReply>("QtMobility::Simulator::SaveOrganizerItemReply");
}

QTM_END_NAMESPACE
