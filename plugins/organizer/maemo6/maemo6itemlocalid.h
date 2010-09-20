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

#ifndef MAEMO6ITEMLOCALID_H
#define MAEMO6ITEMLOCALID_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qorganizeritemenginelocalid.h"

QTM_USE_NAMESPACE

class Maemo6ItemLocalId : public QOrganizerItemEngineLocalId
{
public:
    Maemo6ItemLocalId() {}
    Maemo6ItemLocalId(const QString& id) : m_id(id) {}
    bool isEqualTo(const QOrganizerItemEngineLocalId* other) const
    {
        return m_id == static_cast<const Maemo6ItemLocalId*>(other)->m_id;
    }
    bool isLessThan(const QOrganizerItemEngineLocalId* other) const
    {
        return m_id < static_cast<const Maemo6ItemLocalId*>(other)->m_id;
    }
    uint engineLocalIdType() const
    {
        static uint t = qHash("maemo6");
        return t;
    }
    QOrganizerItemEngineLocalId* clone() const
    {
        return new Maemo6ItemLocalId(m_id);
    }
#ifndef QT_NO_DEBUG_STREAM
    QDebug debugStreamOut(QDebug dbg)
    {
        return dbg << m_id;
    }
#endif
#ifndef QT_NO_DATASTREAM
    QDataStream& dataStreamOut(QDataStream& out)
    {
        return out << m_id;
    }
    QDataStream& dataStreamIn(QDataStream& in)
    {
        in >> m_id;
        return in;
    }
#endif
    uint hash() const
    {
        return qHash(m_id);
    }

    QString toString() const
    {
        return m_id;
    }

private:
    QString m_id;
};

#endif
