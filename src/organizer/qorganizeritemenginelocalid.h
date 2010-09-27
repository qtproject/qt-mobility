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

#ifndef QORGANIZERITEMENGINELOCALID_H
#define QORGANIZERITEMENGINELOCALID_H

#include <QString>
#include <QSharedDataPointer>

#include "qtorganizerglobal.h"

class QDataStream;

QTM_BEGIN_NAMESPACE

class Q_ORGANIZER_EXPORT QOrganizerItemEngineLocalId
{
public:
    virtual ~QOrganizerItemEngineLocalId() {}

    virtual bool isEqualTo(const QOrganizerItemEngineLocalId* other) const = 0;
    virtual bool isLessThan(const QOrganizerItemEngineLocalId* other) const = 0;

    virtual uint engineLocalIdType() const = 0;
    virtual QOrganizerItemEngineLocalId* clone() const = 0;

#ifndef QT_NO_DEBUG_STREAM
    // NOTE: on platforms where Qt is built without debug streams enabled, vtable will differ!
    virtual QDebug debugStreamOut(QDebug dbg) = 0;
#endif
#ifndef QT_NO_DATASTREAM
    // NOTE: on platforms where Qt is built without data streams enabled, vtable will differ!
    virtual QDataStream& dataStreamOut(QDataStream& out) = 0;
    virtual QDataStream& dataStreamIn(QDataStream& in) = 0;
#endif
    virtual uint hash() const = 0;
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemEngineLocalId), Q_MOVABLE_TYPE);


#endif

